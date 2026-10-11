#ifndef LOGGING_H
#define LOGGING_H

#include <stdbool.h>
#include <stdio.h>

typedef enum {
    LOG_ERROR = 0,
    LOG_WARN  = 1,
    LOG_INFO  = 2,
    LOG_DEBUG = 3,
    LOG_MAX   = 4,
} LogLevel;

struct log {
    LogLevel lvl;
    FILE    *stream;
    bool     color;
};

void log_init(struct log *lg, LogLevel lvl, FILE *stream, bool color);
void log_msg(struct log *lg, LogLevel lvl, const char *fmt, ...) __attribute__((format(printf, 3, 4)));

#define pr_err(lg, fmt, ...)  log_msg((lg), LOG_ERROR, fmt, ##__VA_ARGS__);
#define pr_warn(lg, fmt, ...) log_msg((lg), LOG_WARN, fmt, ##__VA_ARGS__);
#define pr_info(lg, fmt, ...) log_msg((lg), LOG_INFO, fmt, ##__VA_ARGS__);
#define pr_debug(lg, fmt, ...)                                                                                         \
    log_msg((lg), LOG_DEBUG, "[%s - %s:%d]" fmt, __FILE__, __FUNC__, __LINE__, ##__VA_ARGS__);

#endif
