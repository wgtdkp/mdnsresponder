#include "ioloop.h"

static void
usage(void)
{
    ERROR("srp-mdns-proxy [--max-lease-time <seconds>] [--log-stderr]");
    exit(1);
}

int
main(int argc, char **argv)
{
    int i;
    char *end;
    int log_stderr = false;

    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--max-lease-time")) {
            if (i + 1 == argc) {
                usage();
            }
            max_lease_time = (uint32_t)strtoul(argv[i + 1], &end, 10);
            if (end == argv[i + 1] || end[0] != 0) {
                usage();
            }
            i++;
        } else if (!strcmp(argv[i], "--log-stderr")) {
            log_stderr = true;
        } else {
            usage();
        }
    }

    OPENLOG(log_stderr);
    INFO("--------------------------------srp-mdns-proxy starting--------------------------------");

    if (!ioloop_init()) {
        return 1;
    }

    if (!start_icmp_listener()) {
        return 1;
    }

    thread_network_startup();

    // We require one open file per service and one per instance.
    struct rlimit limits;
    if (getrlimit(RLIMIT_NOFILE, &limits) < 0) {
        ERROR("getrlimit failed: " PUB_S_SRP, strerror(errno));
        return 1;
    }

    if (limits.rlim_cur < 1024) {
        if (limits.rlim_max < 1024) {
            INFO("getrlimit: file descriptor hard limit is %llu", (unsigned long long)limits.rlim_max);
            if (limits.rlim_cur != limits.rlim_max) {
                limits.rlim_cur = limits.rlim_max;
            }
        } else {
            limits.rlim_cur = 1024;
        }
        if (setrlimit(RLIMIT_NOFILE, &limits) < 0) {
            ERROR("setrlimit failed: " PUB_S_SRP, strerror(errno));
        }
    }

    do {
        int something = 0;
        ioloop();
        INFO("dispatched %d events.", something);
    } while (1);
}
