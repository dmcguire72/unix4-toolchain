#define NOB_IMPLEMENTATION
#define NOB_NO_ECHO
#include "tools/nob.h"

#include <sys/stat.h>

#define EXAMPLE "example"
#define BIN EXAMPLE "/bin"
#define OBJ EXAMPLE "/obj"
#define LIB_A OBJ "/lib.a"

static const char *plain_src[] = {
    EXAMPLE"/char.c", EXAMPLE"/crash.c", EXAMPLE"/float.c", EXAMPLE"/quine.c", EXAMPLE"/cal.c"
};

static const char *lib_prog_src[] = {
    EXAMPLE"/crypttest.c", EXAMPLE"/alloctest.c", EXAMPLE"/hello.c"
};

static const char *lib_src[] = {
    EXAMPLE"/lib/allocator.c", EXAMPLE"/lib/io.c", EXAMPLE"/lib/string.c"
};

static bool verbose = false;
static const char *cc, *ar, *strip_tool;
static Cmd cflags = {0};
static Procs procs = {0};

static void log_action(const char *action, const char *target) {
    if (!verbose) printf("  %-7s %s\n", action, target);
}

static void echo_cmd(Cmd cmd) {
    if (!verbose) return;
    String_Builder sb = {0};
    cmd_render(cmd, &sb);
    sb_append_null(&sb);
    puts(sb.items);
    sb_free(sb);
}

#undef cmd_run
#define cmd_run(cmd, ...) (echo_cmd(*(cmd)), nob_cmd_run(cmd, ##__VA_ARGS__))

static const char *tool_path(const char *env, const char *local, const char *system) {
    const char *v = getenv(env);
    if (v) return v;
    return file_exists(local) > 0 ? local : system;
}

static void split_words(const char *s, Cmd *out) {
    char *p = strdup(s);
    while (*p) {
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) break;
        da_append(out, p);
        while (*p && !isspace((unsigned char)*p)) p++;
        if (*p) *p++ = '\0';
    }
}

static void setup(void) {
    const char *v = getenv("V");
    verbose = v && strcmp(v, "1") == 0;

    cc         = tool_path("V4_CC11",  "./bin/cc11",  "/usr/bin/v4-cc");
    ar         = tool_path("V4_AR",    "./bin/ar",    "/usr/bin/v4-ar");
    strip_tool = tool_path("V4_STRIP", "./bin/strip", "/usr/bin/v4-strip");

    bool have_cc2 = file_exists("./bin/cc2") > 0 || file_exists("/opt/v4/sys/lib/c2") > 0;
    const char *flags = getenv("V4_CFLAGS");
    split_words(flags ? flags : (have_cc2 ? "-O" : ""), &cflags);
}

static bool make_dir(const char *path) {
    if (!file_exists(path)) {
        log_action("MKDIR", path);
        return mkdir_if_not_exists(path);
    }
    return true;
}

static void remove_file(const char *path) {
    if (file_exists(path) > 0) delete_file(path);
}

static bool is_nonempty(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && st.st_size > 0;
}

static const char *derive_path(const char *dir, const char *src, const char *ext) {
    String_View name = sv_from_cstr(path_name(src));
    sv_chop_suffix(&name, sv_from_cstr(".c"));
    return temp_sprintf("%s/"SV_Fmt"%s", dir, SV_Arg(name), ext);
}

static bool run_cc(const char **inputs, size_t input_count, const char *mode) {
    Cmd shown = {0};
    cmd_append(&shown, cc);
    cmd_extend(&shown, &cflags);
    if (mode) cmd_append(&shown, mode);
    da_append_many(&shown, inputs, input_count);
    echo_cmd(shown);

    Cmd cmd = {0};
    cmd_append(&cmd, "sh", "-c", "\"$@\" || [ $? -eq 1 ]", "sh");
    cmd_extend(&cmd, &shown);
    da_free(shown);

    return nob_cmd_run(&cmd);
}

static bool compile_object(const char *src, const char *obj) {
    const char *tmp_obj = temp_sprintf("%.*s.o", (int)strlen(src) - 2, src);

    log_action("CC", obj);
    remove_file("a.out"); remove_file(tmp_obj); remove_file(obj);

    if (!run_cc(&src, 1, "-c")) return false;
    if (!is_nonempty(tmp_obj)) {
        fprintf(stderr, "CC failed: no object generated for %s\n", src);
        return false;
    }
    return nob_rename(tmp_obj, obj);
}

static bool build_binary(const char **inputs, size_t input_count, const char *bin) {
    if (needs_rebuild(bin, inputs, input_count) <= 0) return true;

    log_action("CC", bin);
    remove_file("a.out"); remove_file(bin);

    if (!run_cc(inputs, input_count, NULL)) return false;
    if (!is_nonempty("a.out")) {
        fprintf(stderr, "CC failed: no executable generated for %s\n", bin);
        return false;
    }
    return nob_rename("a.out", bin);
}

static bool build_lib(File_Paths *out_objs) {
    for (size_t i = 0; i < NOB_ARRAY_LEN(lib_src); ++i) {
        const char *src = lib_src[i];
        const char *obj = derive_path(OBJ, src, ".o");
        da_append(out_objs, obj);

        if (needs_rebuild1(obj, src) > 0 && !compile_object(src, obj)) return false;
    }

    if (needs_rebuild(LIB_A, out_objs->items, out_objs->count) <= 0) return true;

    log_action("AR", LIB_A);
    remove_file(LIB_A);
    Cmd cmd = {0};
    cmd_append(&cmd, ar, "r", LIB_A);
    da_append_many(&cmd, out_objs->items, out_objs->count);
    return cmd_run(&cmd);
}

static bool build_all(void) {
    const char *dirs[] = { OBJ, BIN };
    for (size_t i = 0; i < NOB_ARRAY_LEN(dirs); ++i) {
        if (!make_dir(dirs[i])) return false;
    }

    File_Paths lib_objs = {0};
    if (!build_lib(&lib_objs)) return false;

    for (size_t i = 0; i < NOB_ARRAY_LEN(plain_src); ++i) {
        const char *src = plain_src[i];
        if (!build_binary(&src, 1, derive_path(BIN, src, ""))) return false;
    }

    for (size_t i = 0; i < NOB_ARRAY_LEN(lib_prog_src); ++i) {
        const char *inputs[] = { lib_prog_src[i], LIB_A };
        if (!build_binary(inputs, NOB_ARRAY_LEN(inputs), derive_path(BIN, lib_prog_src[i], ""))) return false;
    }

    return true;
}

static bool build_pvm(void) {
    const char *src    = "extra/pvm/unix4/pvm.c";
    const char *header = "extra/pvm/unix4/pvm-common.h";
    const char *obj    = OBJ "/pvm.o";

    if (!make_dir(OBJ)) return false;

    const char *deps[] = { src, header };
    if (needs_rebuild(obj, deps, NOB_ARRAY_LEN(deps)) <= 0) return true;

    if (!copy_file(header, EXAMPLE "/pvm-common.h")) return false;
    bool ok = compile_object(src, obj);
    remove_file(EXAMPLE "/pvm-common.h");
    return ok;
}

static void clean_all(void) {
    log_action("CLEAN", BIN " " OBJ);
    for (size_t i = 0; i < NOB_ARRAY_LEN(plain_src); ++i) {
        remove_file(derive_path(BIN, plain_src[i], ""));
        remove_file(derive_path(EXAMPLE, plain_src[i], ".o"));
    }
    for (size_t i = 0; i < NOB_ARRAY_LEN(lib_prog_src); ++i) {
        remove_file(derive_path(BIN, lib_prog_src[i], ""));
        remove_file(derive_path(EXAMPLE, lib_prog_src[i], ".o"));
    }
    for (size_t i = 0; i < NOB_ARRAY_LEN(lib_src); ++i) {
        remove_file(derive_path(OBJ, lib_src[i], ".o"));
        remove_file(derive_path(EXAMPLE "/lib", lib_src[i], ".o"));
    }
    remove_file(LIB_A);
    remove_file("a.out");
}

static bool strip_all(void) {
    const char **groups[] = { plain_src, lib_prog_src };
    size_t counts[] = { NOB_ARRAY_LEN(plain_src), NOB_ARRAY_LEN(lib_prog_src) };

    for (size_t g = 0; g < NOB_ARRAY_LEN(groups); ++g) {
        for (size_t i = 0; i < counts[g]; ++i) {
            const char *bin = derive_path(BIN, groups[g][i], "");
            log_action("STRIP", bin);
            Cmd cmd = {0};
            cmd_append(&cmd, strip_tool, bin);
            if (!cmd_run(&cmd, .async = &procs)) return false;
        }
    }
    return procs_flush(&procs);
}

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);
    const char *program = shift_args(&argc, &argv);

    bool do_clean = false, do_strip = false, do_pvm = false;

    while (argc > 0) {
        const char *arg = shift_args(&argc, &argv);
        if (strcmp(arg, "all") == 0) continue;
        else if (strcmp(arg, "clean") == 0) do_clean = true;
        else if (strcmp(arg, "strip") == 0) do_strip = true;
        else if (strcmp(arg, "pvm") == 0) do_pvm = true;
        else {
            fprintf(stderr, "unknown target `%s`\nusage: %s [all|strip|clean|pvm]...\n", arg, program);
            return 2;
        }
    }

    setup();

    if (do_clean) {
        clean_all();
        return 0;
    }

    if (!build_all()) return 1;
    if (do_strip && !strip_all()) return 1;
    if (do_pvm && !build_pvm()) return 1;

    return 0;
}