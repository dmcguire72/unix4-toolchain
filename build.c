// usage:
// $ cc build.c -o build
// $ ./build [all|strip|clean]...

// this will rebuild itself if the source file is changed 
// after the first build (given this files name is not changed)

#define NOB_IMPLEMENTATION
#define NOB_NO_ECHO
#define NOBDEF static inline
#include "tools/nob.h"

#define USR "usr"
#define BIN "bin"
#define TMP "tmp"
#define C_SRC USR "/c"
#define S1_SRC USR "/source/s1"
#define BLD TMP "/build"
#define OBJ TMP "/obj"
#define DEP TMP "/deps"

static bool verbose = false;

static void echo_cmd(Cmd cmd) {
    if (!verbose) return;
    String_Builder sb = {0};
    cmd_render(cmd, &sb);
    sb_append_null(&sb);
    puts(sb.items);
    fflush(stdout);
    sb_free(sb);
}

#undef cmd_run
#define cmd_run(cmd, ...) (echo_cmd(*(cmd)), nob_cmd_run(cmd, ##__VA_ARGS__))

static const char *cflags[] = {"-std=gnu11", "-O3", "-Wall", "-Wno-unused-result"};

static const char *emulator_src[] = {
    "tools/emulator/main.c",     "tools/emulator/options.c",
    "tools/emulator/program.c",  "tools/emulator/script.c",
    "tools/emulator/filesystem.c","tools/emulator/state.c",
    "tools/emulator/memory.c",   "tools/emulator/loader.c",
    "tools/emulator/syscalls.c", "tools/emulator/signals.c",
    "tools/emulator/fpu.c",      "tools/emulator/cpu.c",
    "tools/emulator/trace.c",    "tools/emulator/symbols.c",
    "tools/emulator/hooks.c",    "tools/emulator/dump.c"
};

static const char *pass0_src[]  = { C_SRC"/cc0.c", C_SRC"/cc0t.c", C_SRC"/ccrt.c" };
static const char *pass1_src[]  = { C_SRC"/cc1.c", C_SRC"/cc1t.c", C_SRC"/ccrt.c" };
static const char *tables_src[] = { BLD"/regtab.c", BLD"/efftab.c", BLD"/cctab.c", BLD"/sptab.c" };

static const char *as_src[]  = { S1_SRC"/as1.c", S1_SRC"/astab.c" };
static const char *as2_src[] = { S1_SRC"/as2.c", S1_SRC"/astab.c" };

static Procs procs = {0};

static void log_action(const char *action, const char *target) {
    if (!verbose) printf("  %-7s %s\n", action, target);
}

static bool make_dir(const char *path) {
    if (!file_exists(path)) {
        log_action("MKDIR", path);
        if (verbose) printf("mkdir %s\n", path);
        return mkdir_if_not_exists(path);
    }
    return true;
}

static int is_not_ws_or_slash(int x) {
    return !isspace(x) && x != '\\';
}

static bool check_needs_rebuild(const char *obj, const char *d_file) {
    if (!file_exists(obj) || !file_exists(d_file)) return true;

    String_Builder sb = {0};
    if (!read_entire_file(d_file, &sb)) return true;

    String_View sv = sb_to_sv(sb);
    sv_chop_by_delim(&sv, ':');

    bool rebuild = false;
    while (sv.count > 0) {
        while (sv.count > 0 && (isspace(sv.data[0]) || sv.data[0] == '\\')) {
            sv_chop_left(&sv, 1);
        }
        if (sv.count == 0) break;

        String_View dep = sv_chop_while(&sv, is_not_ws_or_slash);
        
        if (dep.count > 0 && dep.data[dep.count - 1] == ':') {
            dep.count--;
        }

        if (dep.count > 0) {
            const char *dep_path = temp_sv_to_cstr(dep);
            if (needs_rebuild1(obj, dep_path) > 0) {
                rebuild = true;
                break;
            }
        }
    }
    sb_free(sb);
    return rebuild;
}

static bool compile_group(const char **srcs, size_t count, const char *obj_dir, const char *dep_dir, const char **incs, size_t incs_count, File_Paths *out_objs) {
    for (size_t i = 0; i < count; ++i) {
        const char *src = srcs[i];
        String_View name = sv_from_cstr(path_name(src));
        sv_chop_suffix(&name, sv_from_cstr(".c"));

        const char *obj = temp_sprintf("%s/"SV_Fmt".o", obj_dir, SV_Arg(name));
        const char *dep = temp_sprintf("%s/"SV_Fmt".d", dep_dir, SV_Arg(name));
        da_append(out_objs, obj);

        if (check_needs_rebuild(obj, dep)) {
            log_action("CC", obj);
            Cmd cmd = {0};
            cmd_append(&cmd, "cc");
            da_append_many(&cmd, cflags, ARRAY_LEN(cflags));
            cmd_append(&cmd, "-MMD", "-MP", "-MF", dep, "-MT", obj);
            for (size_t j = 0; j < incs_count; ++j) cmd_append(&cmd, temp_sprintf("-I%s", incs[j]));
            cmd_append(&cmd, "-c", src, "-o", obj);
            
            if (!cmd_run(&cmd, .async = &procs)) return false;
        }
    }
    return true;
}

static bool compile_direct(const char *src, const char *bin, const char *dep, const char **extra_flags, size_t extra_flags_count, const char **incs, size_t incs_count) {
    if (!check_needs_rebuild(bin, dep)) return true;
    log_action("CC", bin);
    Cmd cmd = {0};
    cmd_append(&cmd, "cc");
    da_append_many(&cmd, cflags, ARRAY_LEN(cflags));
    cmd_append(&cmd, "-MMD", "-MP", "-MF", dep, "-MT", bin);
    for(size_t i = 0; i < extra_flags_count; i++) cmd_append(&cmd, extra_flags[i]);
    for(size_t i = 0; i < incs_count; i++) cmd_append(&cmd, temp_sprintf("-I%s", incs[i]));
    cmd_append(&cmd, "-o", bin, src);
    return cmd_run(&cmd, .async = &procs);
}

static bool link_bin(const char *bin, const char **objs, size_t obj_count, bool math) {
    if (needs_rebuild(bin, objs, obj_count) <= 0) return true;
    log_action("LD", bin);
    Cmd cmd = {0};
    cmd_append(&cmd, "cc");
    da_append_many(&cmd, cflags, ARRAY_LEN(cflags));
    cmd_append(&cmd, "-o", bin);
    da_append_many(&cmd, objs, obj_count);
    if (math) cmd_append(&cmd, "-lm");
    return cmd_run(&cmd, .async = &procs);
}

static bool probe_lib(const char *header, const char *body, const char *lib) {
    FILE *f = fopen(TMP"/probe.c", "w");
    if (!f) return false;
    fprintf(f, "%s\nint main(void) { %s }\n", header, body);
    fclose(f);
    Cmd cmd = {0};
    cmd_append(&cmd, "cc", "-o", TMP"/probe", TMP"/probe.c", lib);
    return nob_cmd_run(&cmd, .stdout_path = "/dev/null", .stderr_path = "/dev/null");
}

static bool build_installer(void) {
    const char *defs[8], *libs[8];
    size_t ndefs = 0, nlibs = 0;
    String_Builder stamp = {0};
    sb_append_cstr(&stamp, "v1");

    #define PROBE(label, header, body, lib, ...) do { \
        bool ok_ = probe_lib(header, body, lib); \
        printf("  CHECK   %-10s %s\n", label, ok_ ? "yes" : "no (using the fallback)"); \
        if (ok_) { const char *d_[] = {__VA_ARGS__}; \
            for (size_t i_ = 0; i_ < sizeof d_ / sizeof *d_; i_++) { defs[ndefs++] = d_[i_]; sb_append_cstr(&stamp, d_[i_]); } \
            libs[nlibs++] = lib; sb_append_cstr(&stamp, lib); } \
    } while (0)

    #define NCURSES_BODY "return initscr() == NULL;"
    if (probe_lib("#include <ncurses.h>", NCURSES_BODY, "-lncursesw")) {
        printf("  CHECK   %-10s yes (wide characters)\n", "ncurses");
        defs[ndefs++] = "-DV4_HAVE_NCURSES"; defs[ndefs++] = "-DV4_NCURSESW"; libs[nlibs++] = "-lncursesw";
        sb_append_cstr(&stamp, "ncursesw");
    } else if (probe_lib("#include <ncurses.h>", NCURSES_BODY, "-lncurses")) {
        printf("  CHECK   %-10s yes (no wide characters: ASCII glyphs only)\n", "ncurses");
        defs[ndefs++] = "-DV4_HAVE_NCURSES"; libs[nlibs++] = "-lncurses";
        sb_append_cstr(&stamp, "ncurses");
    } else {
        printf("  CHECK   %-10s no (using the plain-text UI)\n", "ncurses");
    }
    PROBE("libcurl",    "#include <curl/curl.h>", "curl_easy_cleanup(curl_easy_init()); return 0;", "-lcurl", "-DV4_HAVE_CURL");
    PROBE("libarchive", "#include <archive.h>\n#include <archive_entry.h>", "archive_read_free(archive_read_new()); return 0;", "-larchive", "-DV4_HAVE_LIBARCHIVE");
    PROBE("zlib",       "#include <zlib.h>", "return deflateEnd((z_streamp)0);", "-lz", "-DV4_HAVE_ZLIB");
    unlink(TMP"/probe.c"); unlink(TMP"/probe");

    const char *stamp_file = TMP"/v4linux-setup.cfg";
    String_Builder old = {0};
    if (!file_exists(stamp_file) || !read_entire_file(stamp_file, &old) || old.count != stamp.count || memcmp(old.items, stamp.items, stamp.count) != 0) {
        unlink(TMP"/v4linux-setup");
        write_entire_file(stamp_file, stamp.items, stamp.count);
    }
    sb_free(old); sb_free(stamp);

    const char *bin = TMP"/v4linux-setup", *dep = DEP"/bin/v4linux-setup.d";
    if (!make_dir(DEP"/bin")) return false;
    if (!check_needs_rebuild(bin, dep)) return true;
    log_action("CC", bin);
    Cmd cmd = {0};
    cmd_append(&cmd, "cc");
    da_append_many(&cmd, cflags, ARRAY_LEN(cflags));
    cmd_append(&cmd, "-pthread", "-MMD", "-MP", "-MF", dep, "-MT", bin);
    for (size_t i = 0; i < ndefs; i++) cmd_append(&cmd, defs[i]);
    cmd_append(&cmd, "-o", bin, "tools/v4linux-setup.c");
    for (size_t i = 0; i < nlibs; i++) cmd_append(&cmd, libs[i]);
    return cmd_run(&cmd);
}

static bool build_all() {
    const char *dirs[] = { 
        TMP, BIN, BLD, OBJ, DEP, 
        OBJ"/emulator", DEP"/emulator", 
        OBJ"/s1", DEP"/s1", 
        OBJ"/c", DEP"/c", 
        OBJ"/bld", DEP"/bld",
        DEP"/bin"
    };
    for (size_t i = 0; i < ARRAY_LEN(dirs); ++i) {
        if (!make_dir(dirs[i])) return false;
    }

    if (check_needs_rebuild(BIN"/cvopt", DEP"/bin/cvopt.d")) {
        log_action("CC", BIN"/cvopt");
        Cmd cmd = {0};
        cmd_append(&cmd, "cc");
        da_append_many(&cmd, cflags, ARRAY_LEN(cflags));
        cmd_append(&cmd, "-MMD", "-MP", "-MF", DEP"/bin/cvopt.d", "-MT", BIN"/cvopt");
        cmd_append(&cmd, "-o", BIN"/cvopt", C_SRC"/cvopt.c");
        if (!cmd_run(&cmd)) return false;
    }

    const char *tabs[] = {"regtab", "efftab", "cctab", "sptab"};
    for (size_t i = 0; i < ARRAY_LEN(tabs); ++i) {
        const char *src = temp_sprintf("tables/%s.s", tabs[i]);
        const char *dst = temp_sprintf(BLD"/%s.c", tabs[i]);
        
        const char *deps[] = {src, BIN"/cvopt"};
        if (needs_rebuild(dst, deps, 2) > 0) {
            log_action("CVOPT", dst);
            Cmd cmd = {0};
            cmd_append(&cmd, BIN"/cvopt", src, dst);
            if (!cmd_run(&cmd, .async = &procs)) return false;
        }
    }
    if (!procs_flush(&procs)) return false;

    File_Paths pass0_objs = {0}, pass1_objs = {0}, as_objs = {0}, as2_objs = {0}, emulator_objs = {0};
    
    const char *c_incs[]   = { C_SRC };
    const char *s1_incs[]  = { "src" };
    const char *emu_incs[] = { "tools/emulator", "tools/common" };

    if (!compile_group(pass0_src, ARRAY_LEN(pass0_src), OBJ"/c", DEP"/c", c_incs, 1, &pass0_objs)) return false;
    if (!compile_group(pass1_src, ARRAY_LEN(pass1_src), OBJ"/c", DEP"/c", c_incs, 1, &pass1_objs)) return false;
    if (!compile_group(tables_src, ARRAY_LEN(tables_src), OBJ"/bld", DEP"/bld", c_incs, 1, &pass1_objs)) return false;
    if (!compile_group(as_src, ARRAY_LEN(as_src), OBJ"/s1", DEP"/s1", s1_incs, 1, &as_objs)) return false;
    if (!compile_group(as2_src, ARRAY_LEN(as2_src), OBJ"/s1", DEP"/s1", s1_incs, 1, &as2_objs)) return false;
    if (!compile_group(emulator_src, ARRAY_LEN(emulator_src), OBJ"/emulator", DEP"/emulator", emu_incs, 2, &emulator_objs)) return false;

    const char *no_fmt_trunc[] = { "-Wno-format-truncation" };
    const char *dumpinfo_incs[] = { "tools/common" };

    if (!compile_direct(S1_SRC"/ar.c", BIN"/ar", DEP"/bin/ar.d", no_fmt_trunc, 1, NULL, 0)) return false;
    if (!compile_direct(S1_SRC"/ld.c", BIN"/ld", DEP"/bin/ld.d", NULL, 0, NULL, 0)) return false;
    if (!compile_direct(C_SRC"/cc11.c", BIN"/cc11", DEP"/bin/cc11.d", no_fmt_trunc, 1, NULL, 0)) return false;
    if (!compile_direct(S1_SRC"/strip.c", BIN"/strip", DEP"/bin/strip.d", no_fmt_trunc, 1, NULL, 0)) return false;
    if (!compile_direct("tools/v4dis/v4dis.c", BIN"/v4dis", DEP"/bin/v4dis.d", NULL, 0, NULL, 0)) return false;
    if (!compile_direct(S1_SRC"/tp.c", BIN"/tp", DEP"/bin/tp.d", NULL, 0, NULL, 0)) return false;
    if (!compile_direct("tools/dumpinfo/dumpinfo.c", BIN"/dumpinfo", DEP"/bin/dumpinfo.d", NULL, 0, dumpinfo_incs, 1)) return false;
    if (!compile_direct("tools/emulator/linux/v4linux-dispatch.c", BIN"/v4linux-dispatch", DEP"/bin/v4linux-dispatch.d", NULL, 0, NULL, 0)) return false;

    if (!procs_flush(&procs)) return false; 

    if (!link_bin(BIN"/as", as_objs.items, as_objs.count, false)) return false;
    if (!link_bin(BIN"/as2", as2_objs.items, as2_objs.count, false)) return false;
    if (!link_bin(BIN"/cc0", pass0_objs.items, pass0_objs.count, true)) return false;
    if (!link_bin(BIN"/cc1", pass1_objs.items, pass1_objs.count, true)) return false;
    if (!link_bin(BIN"/v4linux", emulator_objs.items, emulator_objs.count, true)) return false;

    if (!procs_flush(&procs)) return false; 

    return true;
}

const char *check_as_sh =
    "fail=0; t=$(mktemp -d); "
    "for f in tests/as/src/*.s; do "
        "n=$(basename $f .s); "
        "rm -rf $t/w; mkdir -p $t/w; cp $f $t/w/in.s; "
        "(cd $t/w && TMPDIR=$t/w $PWD/"BIN"/as in.s >out.txt 2>&1); "
        "ok=1; "
        "if [ -f tests/as/expected/$n.out ]; then "
            "cmp -s $t/w/a.out tests/as/expected/$n.out || ok=0; "
        "else [ ! -f $t/w/a.out ] || ok=0; fi; "
        "cmp -s $t/w/out.txt tests/as/expected/$n.err || ok=0; "
        "if [ $ok = 1 ]; then echo \"ok $n\"; else echo \"FAIL $n\"; fail=1; fi; "
    "done; rm -rf $t; exit $fail";

const char *check_cc_sh =
    "fail=0; t=$(mktemp -d); "
    "for f in tests/cc/src/*.c; do "
        "n=$(basename $f .c); cp $f $t/$n.c; "
        "(cd $t && $PWD/"BIN"/cc11 -S $n.c >/dev/null 2>&1); "
        "if cmp -s $t/$n.s tests/cc/expected/$n.s; then "
            "echo \"ok $n\"; "
        "else echo \"FAIL $n\"; fail=1; fi; "
    "done; rm -rf $t; exit $fail";


int main(int argc, char **argv) {
    GO_REBUILD_URSELF(argc, argv);
    shift(argv, argc);

    const char *v = getenv("V");
    verbose = v && strcmp(v, "1") == 0;

    bool do_clean = false, do_strip = false, do_install = false, do_uninstall = false;
    bool do_check_as = false, do_check_cc = false, do_check_ld = false;
    bool do_check_ar = false, do_check_strip = false, do_check_dump = false;
    bool do_dis_check = false;

    while (argc > 0) {
        const char *arg = shift(argv, argc);
        if (strcmp(arg, "clean") == 0) do_clean = true;
        else if (strcmp(arg, "strip") == 0) do_strip = true;
        else if (strcmp(arg, "install") == 0) do_install = true;
        else if (strcmp(arg, "uninstall") == 0) do_uninstall = true;
        else if (strcmp(arg, "check-as") == 0) do_check_as = true;
        else if (strcmp(arg, "check-cc") == 0) do_check_cc = true;
        else if (strcmp(arg, "check-ld") == 0) do_check_ld = true;
        else if (strcmp(arg, "check-ar") == 0) do_check_ar = true;
        else if (strcmp(arg, "check-strip") == 0) do_check_strip = true;
        else if (strcmp(arg, "check-dump") == 0) do_check_dump = true;
        else if (strcmp(arg, "dis-check") == 0) do_dis_check = true;
        else if (strcmp(arg, "check") == 0) {
            do_check_as = do_check_cc = do_check_ld = do_check_ar = do_check_strip = do_check_dump = true;
        }
    }

    if (do_clean) {
        log_action("CLEAN", TMP " " BIN);
        Cmd rm = {0}; cmd_append(&rm, "rm", "-rf", TMP, BIN);
        return cmd_run(&rm) ? 0 : 1;
    }
    if (do_uninstall) {
        log_action("UNINSTALL", "");
        Cmd c = {0}; cmd_append(&c, "./tools/v4linux-uninstall.sh", "--purge-all");
        return cmd_run(&c) ? 0 : 1;
    }

    if (!build_all()) return 1;

    if (do_strip) {
        const char *strip_targets[] = {
            BIN"/as", BIN"/as2", BIN"/ld", BIN"/ar", BIN"/strip", BIN"/tp",
            BIN"/cc0", BIN"/cc1", BIN"/cc11", BIN"/v4linux",
            BIN"/v4linux-dispatch", BIN"/v4dis", BIN"/dumpinfo"
        };
        for (size_t i = 0; i < ARRAY_LEN(strip_targets); ++i) {
            log_action("STRIP", strip_targets[i]);
            Cmd c = {0}; cmd_append(&c, "strip", strip_targets[i]);
            if (!cmd_run(&c, .async = &procs)) return 1;
        }
        if (!procs_flush(&procs)) return 1;
    }

    if (do_install) {
        if (!build_installer()) return 1;
        log_action("INSTALL", BIN);
        Cmd c = {0}; cmd_append(&c, TMP"/v4linux-setup", BIN);
        if (!cmd_run(&c)) return 1;
    }

    if (do_check_as) {
        log_action("TEST", "check-as");
        Cmd c = {0}; cmd_append(&c, "sh", "-c", check_as_sh);
        if (!cmd_run(&c)) return 1;
    }
    if (do_check_cc) {
        log_action("TEST", "check-cc");
        Cmd c = {0}; cmd_append(&c, "sh", "-c", check_cc_sh);
        if (!cmd_run(&c)) return 1;
    }
    if (do_check_ld) {
        log_action("TEST", "check-ld");
        Cmd c = {0}; cmd_append(&c, "sh", "-c", "BIN="BIN" sh tests/ld/run.sh check");
        if (!cmd_run(&c)) return 1;
    }
    if (do_check_ar) {
        log_action("TEST", "check-ar");
        Cmd c = {0}; cmd_append(&c, "sh", "-c", "BIN="BIN" sh tests/ar/run.sh check");
        if (!cmd_run(&c)) return 1;
    }
    if (do_check_strip) {
        log_action("TEST", "check-strip");
        Cmd c = {0}; cmd_append(&c, "sh", "-c", "BIN="BIN" sh tests/strip/run.sh check");
        if (!cmd_run(&c)) return 1;
    }
    if (do_check_dump) {
        log_action("TEST", "check-dump");
        Cmd c = {0}; cmd_append(&c, "sh", "-c", "BIN="BIN" sh tests/dump/run.sh");
        if (!cmd_run(&c)) return 1;
    }
    if (do_dis_check) {
        log_action("TEST", "dis-check");
        Cmd c = {0}; cmd_append(&c, "python3", "tools/v4dis/roundtrip.py", "--v4dis", BIN"/v4dis", "--emu", BIN"/v4linux", "--root", "v4", "--scan", "v4");
        if (!cmd_run(&c)) return 1;
    }

    return 0;
}