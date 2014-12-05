#include "mumble.h"

static mumble_t g_mumble;

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	mumble_init(&g_mumble);
	mumble_connect(&g_mumble, "s.uplink.io", 64738);

	return 0;
}
