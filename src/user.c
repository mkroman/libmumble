#include <stdlib.h>
#include <mumble/user.h>

mumble_user_t* mumble_user_init(mumble_user_t* user)
{
    user->name = NULL;
    user->comment = NULL;
    user->hash = NULL;
    user->next = NULL;
    user->flags = 0;

    return user;
}

void mumble_user_free(mumble_user_t* user)
{
    free(user->name);
    free(user->comment);
    free(user->hash);
    free(user);
}
