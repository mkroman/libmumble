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
#include "buffer.h"
#include "iserver.h"
#include "internal.h"
#include "log.h"

#define EV_IO_RESET(x, y, z)                                                   \
    do                                                                         \
    {                                                                          \
        ev_io_stop((x), (y));                                                  \
        ev_io_set((y), (y)->fd, (z));                                          \
        ev_io_start((x), (y));                                                 \
    } while (0)

/**
 * The mumble message header length.
 */
static const size_t kMumbleHeaderSize = (sizeof(uint16_t) + sizeof(uint32_t));

/**
 * The client name to be sent in the version message.
 */
static const char* kMumbleClientName =
    "libmumble (github.com/mkroman/libmumble)";

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

struct mumble_server_t* mumble_server_new(const char* host, uint32_t port)
{
    struct mumble_server_t* server =
        (struct mumble_server_t*)malloc(sizeof(struct mumble_server_t));

    if (!server)
        return NULL;

    mumble_server_init(server);
    server->host = host;
    server->port = port;

    return server;
}

int mumble_server_init(struct mumble_server_t* server)
{
    if (!server)
        return 1;

    server->users = NULL;
    server->client = NULL;
    server->channels = NULL;
    server->welcome_text = NULL;
    mumble_buffer_init(&server->wbuffer);
    mumble_buffer_init(&server->rbuffer);

    ev_init(&server->ping_timer, mumble_server_ping);
    server->ping_timer.repeat = 5;
    server->ping_timer.data = server;

    return 0;
}

int mumble_server_ssl_init(struct mumble_server_t* server)
{
    /* Initialize SSL for the given server. */
    server->ssl = SSL_new(server->client->ssl_ctx);

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

    return 0;
}

void mumble_server_close(struct mumble_server_t* server)
{
    close(server->fd);
    ev_io_stop(server->client->loop, &server->watcher);
}

void mumble_server_free(struct mumble_server_t* server)
{
    SSL_free(server->ssl);

    free(server->wbuffer.ptr);
    free(server->rbuffer.ptr);
    free(server->welcome_text);
    free(server);
}

int mumble_server_connect(struct mumble_server_t* server)
{
    int result;
    socket_t fd;
    struct addrinfo* results, hints, *ptr;
    char port_buffer[6];

    assert(server->client != NULL);

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

    for (ptr = results; ptr != NULL; ptr = ptr->ai_next)
    {
        fd = mumble_server_create_socket();

        if (fd == -1)
            continue;

        if (connect(fd, (struct sockaddr*)ptr->ai_addr, ptr->ai_addrlen) == -1)
        {
            if (errno == EINPROGRESS)
                break; // Connection status is not yet determined.
        }
        else
        {
            LOG_DEBUG("Server connected instantly");
            break;
        }

        close(fd);
    }

    freeaddrinfo(results);

    if (ptr == NULL)
    {
        LOG_ERROR("Connection failed");

        return 1;
    }

    server->fd = fd;
    server->watcher.data = server;

    if (mumble_server_ssl_init(server) != 0)
    {
        close(fd);
        return 1;
    }

    LOG_DEBUG("Starting watcher (host=%s fd=%d)", server->host, server->fd);
    ev_io_init(&server->watcher, mumble_server_handshake, fd,
               EV_READ | EV_WRITE);
    ev_io_start(server->client->loop, &server->watcher);

    return result;
}

void mumble_server_handshake(struct ev_loop* loop, ev_io* w, int revents)
{
    struct mumble_server_t* srv = (struct mumble_server_t*)w->data;
    int result = SSL_connect(srv->ssl);

    (void)revents;

    if (result == 1)
    {
        /* SSL handshake complete */
        LOG_DEBUG("SSL handshake complete");

        EV_IO_RESET(loop, w, EV_READ);
        ev_set_cb(w, mumble_server_callback);

        /* Announce that the connection has been established. */
        mumble_server_connected(srv);
    }
    else if (result < 0)
    {
        int error = SSL_get_error(srv->ssl, result);

        switch (error)
        {
            case SSL_ERROR_WANT_READ:
            {
                EV_IO_RESET(loop, w, EV_READ);

                break;
            }

            case SSL_ERROR_WANT_WRITE:
            {
                EV_IO_RESET(loop, w, EV_WRITE);

                break;
            }

            case SSL_ERROR_SYSCALL:
            {
                error = ERR_get_error();

                if (!error)
                {
                    perror("SSL_ERROR_SYSCALL");
                }
                else
                {
                    LOG_DEBUG("SSL_ERROR_SYSCALL: %d", error);
                }

                mumble_server_close(srv);

                break;
            }

            default:
            {
                LOG_ERROR(
                    "Error during SSL handshake (server=%s err=%d ret=%d)",
                    srv->host, error, result);

                print_ssl_error(error);
                mumble_server_close(srv);
            }
        }
    }
    else
    {
        LOG_ERROR("The TLS/SSL handshake was not successful but was shut down "
                  "controlled and by the specifications of the TLS/SSL "
                  "protocol. (err=%d)",
                  SSL_get_error(srv->ssl, result));
    }
}

void mumble_server_callback(EV_P_ ev_io* w, int revents)
{
    int result;
    struct mumble_server_t* srv = (struct mumble_server_t*)w->data;

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
            EV_IO_RESET(loop, w, EV_READ);
        }
    }
    else /* Assume EV_READ. */
    {
        struct mumble_t* ctx = srv->client;
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

int mumble_server_read_packet(struct mumble_server_t* server)
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
                LOG_INFO("Handled packet (size=%zu type=%d)", packet_length,
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
size_t mumble_server_write(struct mumble_server_t* server, char* data,
                           size_t length)
{
    size_t result =
        mumble_buffer_write(&server->wbuffer, (uint8_t*)data, length);
    ev_io* watcher = &server->watcher;
    struct ev_loop* loop = server->client->loop;

    /* Modify the watchers event flags. */
    EV_IO_RESET(loop, watcher, EV_READ | EV_WRITE);

    return result;
}

int mumble_server_handle_packet(struct mumble_server_t* server, uint16_t type,
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

void mumble_server_connected(struct mumble_server_t* server)
{
    LOG_DEBUG("Connected to %s:%d", server->host, server->port);

    /* Start the ping timer. */
    ev_timer_start(server->client->loop, &server->ping_timer);

    mumble_server_send_version(server);
    mumble_server_send_authenticate(server, "libmumble", "");
}

void mumble_server_disconnected(struct mumble_server_t* server)
{
    mumble_channel_t* channel, *channelptr;
    mumble_user_t* user, *userptr;

    LOG_DEBUG("Connection to %s:%d lost", server->host, server->port);

    /* Stop the ping timer. */
    LOG_INFO("Stopping ping timer");
    ev_timer_stop(server->client->loop, &server->ping_timer);

    /* Stop the io watcher. */
    LOG_INFO("Stopping io watcher");
    ev_io_stop(server->client->loop, &server->watcher);

    for (channel = server->channels; channel != NULL; channel = channelptr)
    {
        channelptr = channel->next;
        mumble_channel_free(channel);
    }

    for (user = server->users; user != NULL; user = userptr)
    {
        userptr = user->next;
        mumble_user_free(user);
    }

    server->channels = NULL;
    server->users = NULL;

    close(server->fd);
}

int mumble_server_send(struct mumble_server_t* server,
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

void mumble_server_ping(struct ev_loop* loop, ev_timer* w, int revents)
{
    struct mumble_server_t* srv = (struct mumble_server_t*)w->data;

    (void)revents;

    if (mumble_server_send_ping(srv) != 1)
    {
        LOG_ERROR("Could not send ping packet");
    }
    else
    {
        LOG_INFO("Sending ping packet");
    }

    ev_timer_again(loop, w);
}

int mumble_server_send_ping(struct mumble_server_t* server)
{
    MumbleProto__Ping ping = MUMBLE_PROTO__PING__INIT;

    return mumble_server_send(server, MUMBLE_PACKET_PING, &ping);
}

int mumble_server_send_version(struct mumble_server_t* server)
{
    MumbleProto__Version version = MUMBLE_PROTO__VERSION__INIT;

    version.version =
        (kMumbleClientVersion.major << 16 | kMumbleClientVersion.minor << 8 |
         kMumbleClientVersion.patch);
    version.has_version = 1;
    version.release = (char*)kMumbleClientName;
    version.os = "X11";
    version.os_version = "Linux";

    return mumble_server_send(server, MUMBLE_PACKET_VERSION, &version);
}

int mumble_server_send_authenticate(struct mumble_server_t* server,
                                    const char* username, const char* password)
{
    MumbleProto__Authenticate authenticate = MUMBLE_PROTO__AUTHENTICATE__INIT;

    authenticate.username = (char*)username;
    authenticate.password = (char*)password;
    authenticate.opus = authenticate.has_opus = 1;

    return mumble_server_send(server, MUMBLE_PACKET_AUTHENTICATE,
                              &authenticate);
}

const struct mumble_user_t*
mumble_server_get_user_by_id(struct mumble_server_t* server, uint32_t id)
{
    const struct mumble_user_t* user;

    if (!server)
        return NULL;

    for (user = server->users; user != NULL; user = user->next)
        if (user->id == id)
            break;

    return user;
}

const struct mumble_user_t*
mumble_server_get_user_by_session_id(struct mumble_server_t* server,
                                     uint32_t session_id)
{
    const struct mumble_user_t* user;

    if (!server)
        return NULL;

    for (user = server->users; user != NULL; user = user->next)
        if (user->session == session_id)
            break;

    return user;
}

const struct mumble_user_t*
mumble_server_get_user_by_name(struct mumble_server_t* server, const char* name)
{
    const struct mumble_user_t* user;

    if (!server || !name)
        return NULL;

    for (user = server->users; user != NULL; user = user->next)
        if (user->name && strcmp(user->name, name) == 0)
            break;

    return user;
}

const char* mumble_server_get_host(const struct mumble_server_t* server)
{
    return server->host;
}

uint32_t mumble_server_get_port(const struct mumble_server_t* server)
{
    return server->port;
}
