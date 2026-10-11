#include <logging.h>
#include <stdarg.h>

static const char *names[]  = {"error", "warn", "info", "debug"};
static const char *colors[] = {"\x1b[31m", "\x1b[33m", "\x1b[0m", "\x1b[90m"};

void log_init(struct log *lg, LogLevel lvl, FILE *stream, bool color) {
    lg->lvl    = lvl;
    lg->stream = stream;
    lg->color  = color;
}

void log_msg(struct log *lg, LogLevel level, const char *fmt, ...) {
    if (level > lg->lvl) return;

    if (lg->color) fputs(colors[level], lg->stream);
    fprintf(lg->stream, "[%s]: ", names[level]);

    va_list ap;
    va_start(ap, fmt);
    vfprintf(lg->stream, fmt, ap);
    va_end(ap);

    if (lg->color) fputs("\x1b[0m", lg->stream);
    fputc('\n', lg->stream);
}
