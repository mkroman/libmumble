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
