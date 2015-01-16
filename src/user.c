#include <stdlib.h>
#include "user.h"

mumble_user_t* mumble_user_init(mumble_user_t* user)
{
	user->name = NULL;
	user->comment = NULL;
	user->hash = NULL;
	user->next = NULL;
	user->flags = 0;

	return user;
}

void mumble_user_destroy(mumble_user_t* user)
{
	if (user->name)
		free(user->name);

	if (user->comment)
		free(user->comment);

	if (user->hash)
		free(user->hash);

	free(user);
}
