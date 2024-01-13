#include <kwl/logger/log.h>

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

static kwl_log_levels_t glog_level;

static const char *kwl_log_level_str(kwl_log_levels_t level) {
	switch (level) {
		case KWL_LOG_INFO: return "INFO";
		case KWL_LOG_WARN: return "WARN";
		case KWL_LOG_DEBUG: return "DEBUG";
		case KWL_LOG_ERROR: return "ERROR";
		case KWL_LOG_FATAL: return "FATAL";
		default: return "UNKNOWN";
	}
}

static const char *kwl_log_color_str(kwl_log_levels_t level) {
	switch (level) {
		case KWL_LOG_INFO: return "\x1b[1;36m";
		case KWL_LOG_DEBUG: return "\x1b[1;32m";
		case KWL_LOG_WARN: return "\x1b[1;33m";
		case KWL_LOG_ERROR: return "\x1b[1;31m";
		case KWL_LOG_FATAL: return "\x1b[1;41m";
		default: return "\x1b[1;41m";
	}
}

void kwl_log_set_level(kwl_log_levels_t level) {
	glog_level = level;
}

int kwl_log(kwl_log_levels_t level, uint64_t line, const char *file,
		const char *fmt, ...) {
	va_list args;
	int res = 0;
	
	if(level < glog_level) {
		return res; /*Log is silenced on this level*/
	}

	fprintf(stderr, "%s%s\x1b[0;35m %s\x1b[1;34m(%lu): \x1b[0m", 
			kwl_log_color_str(level), kwl_log_level_str(level), file, line);

	va_start(args, fmt);
	res = vfprintf(stderr, fmt, args);
	va_end(args);

	return res;
}

int kwl_log_printf(kwl_log_levels_t level, const char *fmt, ...) {
	va_list args;
	int res = 0;

	if(level < glog_level) {
		return 0; /*Log is silenced on this level*/
	}

	va_start(args, fmt);
	res = vfprintf(stderr, fmt, args);
	va_end(args);

	return res;
}


