#pragma once

#include <stdint.h>

typedef enum kwl_log_levels {
	KWL_LOG_INFO,
	KWL_LOG_DEBUG,
	KWL_LOG_WARN,
	KWL_LOG_ERROR,
	KWL_LOG_FATAL,
	KWL_LOG_MAX,
} kwl_log_levels_t;

int kwl_log(kwl_log_levels_t level, uint64_t line, const char *file,
		const char *fmt, ...);
void kwl_log_set_level(kwl_log_levels_t level);
int kwl_log_printf(kwl_log_levels_t level, const char *fmt, ...);	

#define kwl_info(...) kwl_log(KWL_LOG_INFO, __LINE__, __FILE__, __VA_ARGS__)
#define kwl_debug(...) kwl_log(KWL_LOG_DEBUG, __LINE__, __FILE__, __VA_ARGS__)
#define kwl_warn(...) kwl_log(KWL_LOG_WARN, __LINE__, __FILE__, __VA_ARGS__)
#define kwl_error(...) kwl_log(KWL_LOG_ERROR, __LINE__, __FILE__, __VA_ARGS__)
#define kwl_fatal(...) kwl_log(KWL_LOG_FATAL, __LINE__, __FILE__, __VA_ARGS__)
