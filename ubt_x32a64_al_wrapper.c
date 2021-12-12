/**
 * @file ubt_x32a64_al_wrapper.c
 * @author SwimmingTiger (hu60.cn@qq.com)
 * @brief A command line wrapper that allows the use of eltechs exagear (x32a32)
 *        instead of Huawei exagear (x32a64) to achieve better performance on
 *        CPUs that support 32-bit ARM instructions.
 * @copyright No Rights Reserved (CC0)
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>

#define UBT_X32A32 "ubt_x32a32_eltechs"
#define UBT_X32A64 "ubt_x32a64_huawei"
#define REVISION 1773

int usage(const char *self) {
    printf("Usage: %s [compiler_options] -- <guest_executable> [guest_executable_options]\n", self);
    printf("\n");
    printf("General options\n");
    printf("--help/-h - Shows help screen\n");
    printf("--version/-v - Product version\n");
    printf("-f <param> - Execute given file instead of argv[0]\n");
    printf("[STUB] --program-fd <param> - Read executed application file contents from given file descriptor\n");
    printf("[STUB] --use-binfmt_misc - Use binfmt_misc instead of execve() implementation\n");
    printf("[STUB] --force-binfmt_misc - Force execve() to be executed via binfmt_misc\n");
    printf("\n");
    printf("Virtual File System (VFS) options\n");
    printf("--path-prefix <param> - Path to the guest FS image\n");
    printf("--opaths-list <param> - Path to the list of overriden FS elements\n");
    printf("--vpaths-list <param> - Path to the list of substituted FS elements\n");
    printf("[STUB] --utmp-paths-list <param> - Path to the list of utmp paths in system\n");
    printf("--vfs-kind (*guest-first*/host-first) - Selection of the primary and secondary fs roots\n");
    printf("--fs-root <param> - Path to root of the guest file system (chroot emulation)\n");
    printf("--fs-root-shmfd <param> - File descriptor number used for inteprocess filesystem root info storage\n");
    printf("--hifd-base <param> - Highest file descriptor of HiFd area (and RLIMIT_NOFILE softlimit)\n");
    printf("--tmp-dir <param> - Path to the directory to keep temporary files in\n");
    printf("--fd-limit <param> - The maximum number of open file descriptors (RLIMIT_NOFILE hardlimit)\n");
    printf("[STUB] --foreign-ubt-binary <param> - Path to ubt for foreign guest architecture\n");
    printf("[STUB] --foreign-opt-binary <param> - Path to optimizer for foreign guest architecture\n");
    printf("[STUB] --fast-math - Allows to perform mathematical calculations faster, but may lead to inexact results in some cases.\n");
    printf("[STUB] --smo-mode (none/*fbase*/all) - Strong memory ordering enabling mode\n");
    printf("[STUB] --smo-severity (*smart*/full) - Strong memory ordering supporting mechanism\n");
    printf("[STUB] --use_opt <param> - Use region optimizer (enable profiling)\n");
    printf("[STUB] --opt_fd <param> - File descriptor for optimizer regions I/O\n");
    return -1;
}

int version() {
    printf("Dynamic binary translator\nRevision %d. Build: Release\n", REVISION);
    return -1;
}

void setFdLimit(char *fdLimit) {    
    struct rlimit rlm;
    int limit;

    limit = atoi(fdLimit);
    getrlimit(RLIMIT_NOFILE, &rlm);

    if (limit != rlm.rlim_max) {
        rlm.rlim_max = limit;
        setrlimit(RLIMIT_NOFILE, &rlm);
    }
}

int main(int argc, char **argv) {
    char *f;
    int c;
    int newArgc;
    char **newArgv;
    char *newBin;
    char *fdLimit;

    if (argc < 1) {
        printf("too less args: %d, missing argv[0]\n", argc);
        return -1;
    }
    if (argc < 2) {
        return usage(argv[0]);
    }

    fdLimit = NULL;

    /* exagear binary path */
    c = strlen(argv[0]);
    newBin = malloc(c + 20);
    memcpy(newBin, argv[0], c);
    f = strrchr(newBin, '/');
    if (NULL == f) {
        f = argv[0];
    } else {
        f++;
    }
    strcpy(f, UBT_X32A32);
    f = NULL;

    /* Set the current program path to argv[0] of exagear.
     * Let the current program have the opportunity to process
     * the subsequent startup process of exagear. */
    newArgv = malloc(sizeof(char*) * (argc + 1));
    newArgc = 1;
    newArgv[0] = argv[0];

    /* getopt */
    for (c=1; c<argc; c++) {
        if (strcmp(argv[c], "--") == 0) {
            break;
        }

        if (strcmp(argv[c], "-h") == 0 || strcmp(argv[c], "--help") == 0) {
            return usage(argv[0]);
        }

        if (strcmp(argv[c], "-v") == 0 || strcmp(argv[c], "--version") == 0) {
            return version();
        }

        if (strcmp(argv[c], "-f") == 0) {
            if (++c < argc) {
                f = argv[c];
                continue;
            }
            return usage(argv[0]);
        }

        if (strcmp(argv[c], "--fd-limit") == 0) {
            if (++c < argc) {
                fdLimit = argv[c];
                continue;
            }
            return usage(argv[0]);
        }

        if (
          strcmp(argv[c], "--use-binfmt_misc") == 0 ||
          strcmp(argv[c], "--force-binfmt_misc") == 0
        ) {
            /* don't support */
            continue;
        }

        if (
          strcmp(argv[c], "--program-fd") == 0 ||
          strcmp(argv[c], "--utmp-paths-list") == 0 ||
          strcmp(argv[c], "--foreign-ubt-binary") == 0 ||
          strcmp(argv[c], "--foreign-opt-binary") == 0 ||
          strcmp(argv[c], "--fast-math") == 0 ||
          strcmp(argv[c], "--smo-mode") == 0 ||
          strcmp(argv[c], "--smo-severity") == 0 ||
          strcmp(argv[c], "--use_opt") == 0 ||
          strcmp(argv[c], "--opt_fd") == 0
        ) {
            if (++c < argc) {
                /* don't support */
                continue;
            }
            return usage(argv[0]);
        }

        if (
          strcmp(argv[c], "--path-prefix") == 0 ||
          strcmp(argv[c], "--opaths-list") == 0 ||
          strcmp(argv[c], "--vpaths-list") == 0 ||
          strcmp(argv[c], "--vfs-kind") == 0 ||
          strcmp(argv[c], "--fs-root") == 0 ||
          strcmp(argv[c], "--fs-root-shmfd") == 0 ||
          strcmp(argv[c], "--hifd-base") == 0 ||
          strcmp(argv[c], "--tmp-dir") == 0
        ) {
            newArgv[newArgc++] = argv[c];
            if (++c < argc) {
                newArgv[newArgc++] = argv[c];
                continue;
            }
            return usage(argv[0]);
        }

        /* Unknown options */
        return usage(argv[0]);
    }

    /* fetch extra options */
    if (c < argc) {
        while (c < argc) {
            newArgv[newArgc++] = argv[c++];
        }
    } else if (NULL != f) {
        newArgv[newArgc++] = "--";
        newArgv[newArgc++] = f;
    }

    /* execvp requires a NULL as the end */
    newArgv[newArgc] = NULL;

#ifdef DEBUG_OUTPUT
    /* debug output */
    for (c=0; c<argc; c++) {
        printf("[o] %s\n", argv[c]);
    }
    printf("\n");
    for (c=0; NULL != newArgv[c]; c++) {
        printf("[n] %s\n", newArgv[c]);
    }
    printf("\n");
#endif

    /* set fd limit */
    if (NULL != fdLimit) {
        setFdLimit(fdLimit);
    }

    /* exec */
    execvp(newBin, newArgv);

    /* exec failed */
    return -1;
}
