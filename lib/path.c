#include <lib/path.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void normalize(char *path) {
    char       *out = path;
    const char *p   = path;

    *out++ = '/';
    while (*p) {
        p += strspn(p, "/");
        size_t len = strcspn(p, "/");
        if (len == 0) { break; }

        if (len == 1 && p[0] == '.') {
            /* void */
        } else if (len == 2 && p[0] == '.' && p[1] == '.') {
            if (out > path + 1) {
                out--;
                while (out > path && out[-1] != '/') { out--; }
            }
        } else {
            memmove(out, p, len);
            out += len;
            *out++ = '/';
        }
        p += len;
    }
    if (out > path + 1) { out--; }
    *out = '\0';
}

char *abspath(const char *path) {
    char  *cwd = NULL, *out;
    size_t len = strlen(path);
    size_t cwdlen;

    if (path[0] != '/') {
        cwd = getcwd(NULL, 0);
        if (!cwd) { return NULL; }
    }

    cwdlen = cwd ? strlen(cwd) : 0;
    out    = malloc(cwdlen + 1 + len + 1);
    if (!out) { goto fail0; }

    if (cwd) {
        sprintf(out, "%s/%s", cwd, path);
    } else {
        strcpy(out, path);
    }
    free(cwd);

    normalize(out);
    return out;

fail0:
    free(cwd);
    return NULL;
}
