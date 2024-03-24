#include <argp.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/resource.h>
#include <bpf/libbpf.h>

#include "sockrdr.skel.h"

static volatile bool exiting = false;
static void sig_handler(int sig)
{
    exiting = true;
}

static struct env {
    bool verbose;
    long min_duration_ms;
} env;

/*
const char argp_program_doc [] = \
    "Socket redirector\n"
    "\n"
    "USAGE: ./sockrdr [-d <min-duration-ms>] [-v]\n";

static const struct argp_option opts[] = {
    { "verbose", 'v', NULL, 0, "Verbose debug output" },
    { "duration", 'd', "DURATION-MS", 0, "Minimum process duration (ms) to report" },
    {},
};

static error_t parse_arg(int key, char *arg, struct argp_state *state) {
    switch (key) {
        case 'v':
            printf("Verbose debug output\n");
            break;
        case 'd':
            errno = 0;
            env.min_duration_ms = strtol(arg, NULL, 10);
            if (errno || env.min_duration_ms <= 0) {
                fprintf(stderr, "Invalid duration: %s\n", arg);
                argp_usage(state);
            }
            break;
        case ARGP_KEY_ARG:
            argp_usage(state);
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static const struct argp argp = {
    .options = opts,
    .parser = parse_arg,
    .doc = argp_program_doc,
};
*/

int libbpf_print_fn(enum libbpf_print_level level, const char *format, va_list args)
{
    if (level == LIBBPF_DEBUG && !env.verbose)
        return 0;
    return vfprintf(stderr, format, args);
}

int main(int argc, char **argv)
{
    struct sockrdr_bpf *skel;
    int err;

    printf("Hello, World!\n");

    /* parse command argument */
    /*
    err = argp_parse(&argp, argc, argv, 0, NULL, NULL);
    if (err) {
        fprintf(stderr, "Failed to parse arguments, Code: %d\n", err);
        return err;
    }
    */

    /* setup libbpf errors and debug callback */
    libbpf_set_print(libbpf_print_fn);

    /* cleaner handling of Ctrl-C */
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    /* open and load the bpf map */
    skel = sockrdr_bpf__open_and_load();
    if (!skel) {
        fprintf(stderr, "Failed to open and load BPF skeleton\n");
        return 1;
    }

    /* verify the bpf map by fd return code */
    int map_fd = bpf_map__fd(skel->maps.sock_ops);
    if (map_fd < 0) {
        fprintf(stderr, "Failed to load bpf program, map_fd: %d\n", map_fd);
        return 1;
    }

    /* attach bpf program */
    err = sockrdr_bpf__attach(skel);
    if (err) {
        fprintf(stderr, "Failed to attach BPF skeleton, Code: %d\n", err);
        goto cleanup;
    }

    printf("Running... (Ctrl-C to stop)\n");
    /* start daemon */
    while (!exiting) {
        fprintf(stderr, ".");
        sleep(1);
    }

cleanup:
    sockrdr_bpf__destroy(skel);
    printf("Goodbye, World!\n");
    return 0;
}