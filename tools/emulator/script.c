#include "pdp11.h"
#include "script.h"

#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/stat.h>
#include <errno.h>

static char script_copy[PATH_MAX * 2];
static char created_tmp_dir[PATH_MAX * 2];

void cleanup_script(void)
{
    if (script_copy[0] != '\0')
        unlink(script_copy);

    if (created_tmp_dir[0] != '\0')
        rmdir(created_tmp_dir);
}

int has_valid_magic(const char *path)
{
    FILE *f;
    unsigned char h[2];
    unsigned int magic;

    f = fopen(path, "rb");
    if (!f)
        return -1;

    if (fread(h, 1, sizeof h, f) != sizeof h) {
        fclose(f);
        return -1;
    }

    fclose(f);

    magic = h[0] | ((unsigned int)h[1] << 8);

    return magic == 0407 || magic == 0410;
}

static int path_is_inside_root(const char *path)
{
    size_t len;

    if (root[0] == '\0')
        return 1;

    len = strlen(root);

    return strncmp(path, root, len) == 0 &&
           (path[len] == '/' || path[len] == '\0');
}

static int copy_script_to_root(const char *source,
                               char *guest_path,
                               size_t guest_path_size)
{
    char tmp_dir[PATH_MAX * 2];
    char template[PATH_MAX * 2];
    char resolved[PATH_MAX * 2];
    FILE *in, *out;
    char buf[8192];
    size_t n, tmp_len;
    int created = 0;
    int fd;
    struct stat st;

    if (root[0] == '\0' || realpath(source, resolved) == NULL)
        return -1;

    if (snprintf(tmp_dir, sizeof tmp_dir, "%s/tmp", root)
        >= (int)sizeof tmp_dir)
        return -1;

    if (mkdir(tmp_dir, 0777) == 0) {
        created = 1;
    } else if (errno != EEXIST) {
        return -1;
    }

    if (created) {
        snprintf(created_tmp_dir, sizeof created_tmp_dir,
                 "%s", tmp_dir);
    }

    if (stat(tmp_dir, &st) < 0 || !S_ISDIR(st.st_mode))
        goto fail;

    tmp_len = strlen(tmp_dir);

    if (tmp_len + sizeof "/v4linux-script-XXXXXX" >
        sizeof template)
        goto fail;

    memcpy(template, tmp_dir, tmp_len);
    memcpy(template + tmp_len, "/v4linux-script-XXXXXX",
           sizeof "/v4linux-script-XXXXXX");

    fd = mkstemp(template);
    if (fd < 0)
        goto fail;

    in = fopen(resolved, "rb");
    if (!in) {
        close(fd);
        unlink(template);
        goto fail;
    }

    out = fdopen(fd, "wb");
    if (!out) {
        fclose(in);
        close(fd);
        unlink(template);
        goto fail;
    }

    while ((n = fread(buf, 1, sizeof buf, in)) > 0) {
        if (fwrite(buf, 1, n, out) != n) {
            fclose(in);
            fclose(out);
            unlink(template);
            goto fail;
        }
    }

    if (ferror(in)) {
        fclose(in);
        fclose(out);
        unlink(template);
        goto fail;
    }

    fclose(in);

    if (fclose(out) != 0) {
        unlink(template);
        goto fail;
    }

    if (snprintf(script_copy, sizeof script_copy, "%s",
                 template) >= (int)sizeof script_copy) {
        unlink(template);
        goto fail;
    }

    if (snprintf(guest_path, guest_path_size, "/tmp/%s",
                 strrchr(template, '/') + 1)
        >= (int)guest_path_size) {
        unlink(template);
        script_copy[0] = '\0';
        goto fail;
    }

    return 0;

fail:
    if (created && created_tmp_dir[0] != '\0') {
        rmdir(created_tmp_dir);
        created_tmp_dir[0] = '\0';
    }

    return -1;
}

int prepare_script_path(const char *program,
                        char *guest_path,
                        size_t guest_path_size)
{
    char resolved[PATH_MAX * 2];

    if (root[0] == '\0') {
        snprintf(guest_path, guest_path_size, "%s", program);
        return 0;
    }

    if (realpath(program, resolved) == NULL)
        return -1;

    if (path_is_inside_root(resolved)) {
        snprintf(guest_path, guest_path_size, "%s",
                 resolved + strlen(root));

        if (guest_path[0] == '\0')
            snprintf(guest_path, guest_path_size, "/");

        return 0;
    }

    return copy_script_to_root(resolved, guest_path,
                               guest_path_size);
}