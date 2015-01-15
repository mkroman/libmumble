#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "log.h"

static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

static size_t log_write_timestamp(char* buffer, size_t size)
{
	size_t result;
	time_t current_time = time(NULL);
	struct tm local_time;

	if (localtime_r(&current_time, &local_time) == NULL)
		return 0;

	result = strftime(buffer, size, "%X", &local_time);

	return result;
}

void mumble_log(const char* func, const char* format, ...)
{
	va_list ap;
	static char time_buffer[12];
	static char buffer[1024];

	va_start(ap, format);
	pthread_mutex_lock(&log_mutex);

	vsnprintf(buffer, sizeof buffer, format, ap);
	log_write_timestamp(time_buffer, sizeof time_buffer);
	printf(ANSI_BOLD "%s" ANSI_RESET " %-20s %s\n", time_buffer, func, buffer);

	pthread_mutex_unlock(&log_mutex);

	va_end(ap);
}
