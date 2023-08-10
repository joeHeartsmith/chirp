#include "stdcntrl.h"

#include <stdio.h>
#include <string.h>
#include "pddd.h"

static int loglevel = 0;

void set_loglevel(int level) {
    if (level >= 0 && level <= 7) {
        loglevel = level;
    }
    else {
        printf("LOGGING ERROR: LOGLEVEL OUT-OF-RANGE.\n");
        loglevel = 0;
    }
}

int get_loglevel(void) {
    return loglevel;
}

void log_pr(int level, char *print_string) {
    if (level >= 0 && level <= 7) {
        if (level <= loglevel) {
            if (level == LOG_EMERG) {
                printf("pddd (EMERGENCY): %s\n", print_string);
            }
            if (level == LOG_ALERT) {
                printf("pddd (ALERT): %s\n", print_string);
            }
            if (level == LOG_CRIT) {
                printf("pddd (CRITICAL): %s\n", print_string);
            }
            if (level == LOG_ERR) {
                printf("pddd (ERROR): %s\n", print_string);
            }
            if (level == LOG_WARNING) {
                printf("pddd (WARNING): %s\n", print_string);
            }
            if (level == LOG_NOTICE) {
                printf("pddd (NOTICE): %s\n", print_string);
            }
            if (level == LOG_INFO) {
                printf("pddd (INFO): %s\n", print_string);
            }
            if (level == LOG_DEBUG) {
                printf("pddd (DEBUG): %s\n", print_string);
            }
        }
    }
    else {
        printf("LOGGING ERROR: NO FACILITY EXISTS.\n");
    }
}
