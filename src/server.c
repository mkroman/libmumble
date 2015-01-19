#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <openssl/err.h>

#include <mumble/mumble.h>
#include <mumble/server.h>
#include <mumble/channel.h>
#include <mumble/user.h>

#include "Mumble.pb-c.h"
#include "protocol.h"
#include "packets.h"
#include "log.h"

static int setnonblock(socket_t fd)
{
#ifdef __unix__
    int flags = fcntl(fd, F_GETFL);

    flags |= O_NONBLOCK;

    return fcntl(fd, F_SETFL, flags);
#endif /* __unix__ */
}

static void print_ssl_error(unsigned long e)
{
    char buffer[120];

    ERR_error_string_n(e, buffer, sizeof buffer);
    fputs(buffer, stderr);
}

socket_t mumble_server_create_socket()
{
    socket_t fd;

    fd = socket(PF_INET, SOCK_STREAM, 0);

    if (fd < 0)
    {
        LOG_ERROR("Socket creation failed");

        return -1;
    }

    if (setnonblock(fd) != 0)
    {
        LOG_ERROR("Could not make file descriptor non-blocking");

        return -1;
    }

    return fd;
}

int mumble_server_init(mumble_t* context, mumble_server_t* server)
{
    if (!server)
        return 1;

    server->host = NULL;
    server->ctx = context;
    server->users = NULL;
    server->channels = NULL;
    server->welcome_text = NULL;
    mumble_buffer_init(&server->wbuffer);
    mumble_buffer_init(&server->rbuffer);

    ev_init(&server->ping_watcher, mumble_server_ping);
    server->ping_watcher.repeat = 5;
    server->ping_watcher.data = server;

    return 0;
}

int mumble_server_init_ssl(mumble_server_t* server)
{
    int result;

    /* Initialize SSL for the given server. */
    server->ssl = SSL_new(server->ctx->ssl_ctx);

    if (server->ssl == NULL)
    {
        LOG_ERROR("Could not create SSL object");

        return 1;
    }

    if (!SSL_set_fd(server->ssl, server->fd))
    {
        LOG_ERROR("Could not set file descriptor on SSL object");

        return 1;
    }

    if ((result = SSL_connect(server->ssl)) != -1)
    {
        LOG_ERROR("SSL Connection failed (err=%d ret=%d)", result,
                  SSL_get_error(server->ssl, result));

        return 1;
    }

    return 0;
}

void mumble_server_destroy(mumble_server_t* server)
{
    mumble_channel_t* channel, *channelptr;
    mumble_user_t* user, *userptr;

    close(server->fd);
    SSL_free(server->ssl);

    free(server->wbuffer.ptr);
    free(server->rbuffer.ptr);

    for (channel = server->channels; channel != NULL; channel = channelptr)
    {
        channelptr = channel->next;
        mumble_channel_destroy(channel);
    }

    for (user = server->users; user != NULL; user = userptr)
    {
        userptr = user->next;
        mumble_user_destroy(user);
    }

    ev_io_stop(server->ctx->loop, &server->watcher);
}

int mumble_server_connect(mumble_server_t* server, struct mumble_t* context)
{
    int result;
    socket_t fd;
    struct addrinfo* results, hints, *ptr;
    char port_buffer[6];

#ifdef _WIN32
    if (_itoa_s(server->port, port_buffer, sizeof port_buffer, 10) != 0)
#else /* _WIN32 */
    if (snprintf(port_buffer, sizeof port_buffer, "%u", server->port) < 0)
#endif
    {
        LOG_ERROR("Could not convert port to number");

        return 1;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_flags = 0;
    hints.ai_family = AF_UNSPEC;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_socktype = SOCK_STREAM;

    result = getaddrinfo(server->host, port_buffer, &hints, &results);

    if (result != 0)
    {
        LOG_ERROR("Could not resolve remote host: %s", gai_strerror(result));

        return 1;
    }

    fd = mumble_server_create_socket();

    for (ptr = results; ptr != NULL; ptr = ptr->ai_next)
    {
        if (connect(fd, (struct sockaddr*)ptr->ai_addr, ptr->ai_addrlen) != 0)
        {
            if (errno == EINPROGRESS)
                break; // Connection status is not yet determined.
        }
        else
        {
            break;
        }
    }

    freeaddrinfo(results);

    if (ptr == NULL)
    {
        close(fd);
        LOG_ERROR("Connection failed");

        return 1;
    }

    server->fd = fd;
    server->watcher.data = server;

    if (mumble_server_init_ssl(server) != 0)
    {
        close(fd);
        return 1;
    }

    ev_io_init(&server->watcher, mumble_server_handshake, fd,
               EV_READ | EV_WRITE);
    ev_io_start(context->loop, &server->watcher);

    return result;
}

void mumble_server_callback(EV_P_ ev_io* w, int revents)
{
    int result;
    mumble_server_t* srv = (mumble_server_t*)w->data;

    if (revents & EV_WRITE)
    {
        /* Write any pending data. */
        size_t sent;
        size_t buffer_size = srv->wbuffer.size;

        assert(buffer_size > 0);
        sent = SSL_write(srv->ssl, srv->wbuffer.ptr, buffer_size);
        assert(sent > 0);

        if (sent > 0)
        {
            LOG_INFO("Sent %zu bytes", sent);
            mumble_buffer_read(&srv->wbuffer, NULL, sent);
        }
        else
        {
            int err = SSL_get_error(srv->ssl, sent);

            if (err == SSL_ERROR_ZERO_RETURN)
            {
                /* The connection was closed. */
                mumble_server_disconnected(srv);
            }
            else
            {
                LOG_INFO("Could not read from SSL object (err=%d ret=%zu)", err,
                         sent);
            }
        }

        if (srv->wbuffer.size == 0)
        {
            /* If the buffer is empty, mark the io watcher. */
            ev_io_stop(EV_A_ w);
            ev_io_set(w, srv->fd, EV_READ);
            ev_io_start(EV_A_ w);
        }
    }
    else /* Assume EV_READ. */
    {
        mumble_t* ctx = srv->ctx;
        result = SSL_read(srv->ssl, ctx->buffer, sizeof(ctx->buffer));

        if (result > 0)
        {
            LOG_INFO("Received %d bytes", result);
            mumble_buffer_write(&srv->rbuffer, (uint8_t*)ctx->buffer, result);

            if (srv->rbuffer.size > kMumbleHeaderSize)
                while (mumble_server_read_packet(srv))
                    ;
            ;
        }
        else if (result == 0)
        {
            mumble_server_disconnected(srv);
        }
        else
        {
            LOG_ERROR("Could not read from SSL object: %d", result);
        }
    }
}

int mumble_server_read_packet(mumble_server_t* server)
{
    uint16_t type;
    uint32_t length;
    size_t packet_length;

    if (server->rbuffer.size > kMumbleHeaderSize)
    {
        type = ntohs(*(uint16_t*)server->rbuffer.ptr);
        length = ntohl(*(uint32_t*)(server->rbuffer.ptr + sizeof(uint16_t)));
        packet_length = length + kMumbleHeaderSize;

        if (server->rbuffer.size >= packet_length)
        {
            if (mumble_server_handle_packet(server, type, length))
            {
                LOG_INFO("Handled packet (size = %zu type = %d)", packet_length,
                         type);

                /* Discard the data from the buffer. */
                mumble_buffer_read(&server->rbuffer, NULL, packet_length);

                return 1;
            }
        }
    }

    return 0;
}

/**
 * Write data to the connections outgoing buffer.
 *
 * @param[in] server a pointer to the server structure.
 * @param[in] data   a pointer to the data to write to the buffer.
 * @param[in] length the number of bytes contained in `data`.
 *
 * @returns the number of bytes written.
 */
size_t mumble_server_write(mumble_server_t* server, char* data, size_t length)
{
    size_t result;

    result = mumble_buffer_write(&server->wbuffer, (uint8_t*)data, length);

    ev_io* watcher = &server->watcher;
    struct ev_loop* loop = server->ctx->loop;

    // Mark the watcher that we want to read.
    ev_io_stop(EV_A_ watcher);
    ev_io_set(watcher, watcher->fd, EV_READ | EV_WRITE);
    ev_io_start(EV_A_ watcher);

    return result;
}

int mumble_server_handle_packet(mumble_server_t* server, uint16_t type,
                                uint32_t length)
{
    mumble_handler_func_t handler;
    const uint8_t* body =
        (const uint8_t*)server->rbuffer.ptr + kMumbleHeaderSize;

    if (type >= MUMBLE_PACKET_MAX)
    {
        LOG_WARN("Received invalid packet type %d", type);

        return 1;
    }

    if ((handler = g_mumble_packet_handlers[type]) != NULL)
        return handler(server, body, length);

    return 1;
}

void mumble_server_handshake(EV_P_ ev_io* w, int revents)
{
    mumble_server_t* srv = (mumble_server_t*)w->data;
    int result = SSL_do_handshake(srv->ssl);

    if (result == 1)
    {
        /* SSL handshake complete */
        LOG_DEBUG("SSL handshake complete");

        /* Change the callback to the generic, non-handshake one. */
        ev_io_stop(EV_A_ w);
        ev_set_cb(w, mumble_server_callback);
        ev_io_set(w, w->fd, EV_READ);
        ev_io_start(EV_A_ w);

        /* Announce that the connection has been established. */
        mumble_server_connected(srv);
    }
    else
    {
        int err = SSL_get_error(srv->ssl, result);

        if (err == SSL_ERROR_ZERO_RETURN)
        {
            LOG_WARN("Connection was closed during handshake");

            mumble_server_disconnected(srv);
        }
        else if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE)
        {
            LOG_ERROR("Error during SSL handshake (err=%d ret=%d)", err,
                      result);

            print_ssl_error(err);
            mumble_server_disconnected(srv);
        }
    }
}

void mumble_server_connected(mumble_server_t* server)
{
    LOG_DEBUG("Connected to %s:%d", server->host, server->port);

    /* Start the ping watcher. */
    ev_timer_start(server->ctx->loop, &server->ping_watcher);

    mumble_server_send_version(server);
    mumble_server_send_authenticate(server, "libmumble", "");
}

void mumble_server_disconnected(mumble_server_t* server)
{
    mumble_channel_t* channel, *channelptr;
    mumble_user_t* user, *userptr;

    LOG_DEBUG("Connection to %s:%d lost", server->host, server->port);

    /* Stop the ping watcher. */
    LOG_INFO("Stopping ping watcher");
    ev_timer_stop(server->ctx->loop, &server->ping_watcher);

    /* Stop the io watcher. */
    LOG_INFO("Stopping io watcher");
    ev_io_stop(server->ctx->loop, &server->watcher);

    for (channel = server->channels; channel != NULL; channel = channelptr)
    {
        channelptr = channel->next;
        mumble_channel_destroy(channel);
    }

    for (user = server->users; user != NULL; user = userptr)
    {
        userptr = user->next;
        mumble_user_destroy(user);
    }

    server->channels = NULL;
    server->users = NULL;
}

int mumble_server_send(mumble_server_t* server,
                       mumble_packet_type_t packet_type, void* message)
{
    size_t length;
    size_t result;
    uint8_t* body;
    char* buffer;

    /* Get the packed size of the packet. */
    length = mumble_packet_size_packed(packet_type, message);

    /* Construct the packet header. */
    buffer = (char*)malloc(kMumbleHeaderSize + length);

    if (!buffer)
        return 0;

    body = (uint8_t*)(buffer + kMumbleHeaderSize);

    /* Write and pack the protobuf message. */
    result = mumble_packet_proto_pack(packet_type, message, body);

    /* Write the packet header. */
    *(uint16_t*)buffer = htons(packet_type);
    *(uint32_t*)(buffer + sizeof(uint16_t)) = htonl(length);

    result = mumble_server_write(server, buffer, (length + kMumbleHeaderSize));

    /* Free the packet buffer. */
    free(buffer);

    if (result > 0)
        return 1;

    return 0;
}

void mumble_server_ping(EV_P_ ev_timer* w, int revents)
{
    mumble_server_t* srv = (mumble_server_t*)w->data;

    if (mumble_server_send_ping(srv) != 1)
        LOG_ERROR("Could not send ping packet");
    else
        LOG_INFO("Sending ping packet");

    ev_timer_again(srv->ctx->loop, w);
}

int mumble_server_send_ping(mumble_server_t* server)
{
    MumbleProto__Ping ping = MUMBLE_PROTO__PING__INIT;

    return mumble_server_send(server, MUMBLE_PACKET_PING, &ping);
}

int mumble_server_send_version(mumble_server_t* server)
{
    MumbleProto__Version version = MUMBLE_PROTO__VERSION__INIT;

    version.version =
        (kMumbleClientVersion.major << 16 | kMumbleClientVersion.minor << 8 |
         kMumbleClientVersion.patch);
    version.has_version = 1;
    version.release = "libmumble";
    version.os = "X11";
    version.os_version = "Linux";

    return mumble_server_send(server, MUMBLE_PACKET_VERSION, &version);
}

int mumble_server_send_authenticate(mumble_server_t* server,
                                    const char* username, const char* password)
{
    MumbleProto__Authenticate authenticate = MUMBLE_PROTO__AUTHENTICATE__INIT;

    authenticate.username = (char*)username;
    authenticate.password = (char*)password;
    authenticate.opus = authenticate.has_opus = 1;

    return mumble_server_send(server, MUMBLE_PACKET_AUTHENTICATE,
                              &authenticate);
}
