/*
 * Ad Soyad   : Berkay Alperen Bayram
 * Ogrenci No : 439541
 */
#include "parser.h"
#include "math_utils.h"

#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *trim(char *s) {
    char *end = NULL;
    while (*s && isspace((unsigned char)*s)) {
        ++s;
    }
    if (*s == '\0') {
        return s;
    }
    end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) {
        --end;
    }
    end[1] = '\0';
    return s;
}

void records_init(RecordList *list) {
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

void records_free(RecordList *list) {
    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

int records_push(RecordList *list, const char *cmd, const char *result) {
    if (list->count == list->capacity) {
        size_t new_cap = (list->capacity == 0) ? 16 : list->capacity * 2;
        CommandRecord *new_items =
            (CommandRecord *)realloc(list->items, new_cap * sizeof(CommandRecord));
        if (!new_items) {
            return 0;
        }
        list->items = new_items;
        list->capacity = new_cap;
    }
    strncpy(list->items[list->count].command, cmd, MAX_CMD_TEXT - 1);
    list->items[list->count].command[MAX_CMD_TEXT - 1] = '\0';
    strncpy(list->items[list->count].result, result, MAX_RESULT_TEXT - 1);
    list->items[list->count].result[MAX_RESULT_TEXT - 1] = '\0';
    ++list->count;
    return 1;
}

static void eval_command(const char *cmd, char *out, size_t out_sz) {
    char op[32] = {0};
    int64_t a = 0, b = 0, c = 0, inv = 0;
    int n = 0;

    n = sscanf(cmd, "%31s %" SCNd64 " %" SCNd64 " %" SCNd64, op, &a, &b, &c);
    if (n <= 0) {
        snprintf(out, out_sz, "ERROR_INVALID_COMMAND");
        return;
    }

    if (strcmp(op, "GCD") == 0) {
        if (n != 3 || a <= 0 || b <= 0) {
            snprintf(out, out_sz, "ERROR_INVALID_INPUT");
            return;
        }
        snprintf(out, out_sz, "%" PRId64, gcd_ll(a, b));
        return;
    }
    if (strcmp(op, "INV") == 0) {
        if (n != 3) {
            snprintf(out, out_sz, "ERROR_INVALID_INPUT");
            return;
        }
        if (!mod_inverse(a, b, &inv)) {
            snprintf(out, out_sz, "ERROR_NO_INVERSE");
            return;
        }
        snprintf(out, out_sz, "%" PRId64, inv);
        return;
    }
    if (strcmp(op, "POW") == 0) {
        if (n != 4) {
            snprintf(out, out_sz, "ERROR_INVALID_INPUT");
            return;
        }
        int64_t v = mod_pow(a, b, c);
        if (v < 0) {
            snprintf(out, out_sz, "ERROR_INVALID_INPUT");
            return;
        }
        snprintf(out, out_sz, "%" PRId64, v);
        return;
    }
    if (strcmp(op, "PRIME") == 0) {
        if (n != 2 || a < 2) {
            snprintf(out, out_sz, "ERROR_INVALID_INPUT");
            return;
        }
        snprintf(out, out_sz, is_prime_ll(a) ? "YES" : "NO");
        return;
    }
    if (strcmp(op, "PHI") == 0) {
        if (n != 2 || a < 1) {
            snprintf(out, out_sz, "ERROR_INVALID_INPUT");
            return;
        }
        int64_t v = phi_ll(a);
        if (v < 0) {
            snprintf(out, out_sz, "ERROR_INVALID_INPUT");
            return;
        }
        snprintf(out, out_sz, "%" PRId64, v);
        return;
    }
    if (strcmp(op, "CHECK") == 0) {
        if (n != 3) {
            snprintf(out, out_sz, "ERROR_INVALID_INPUT");
            return;
        }
        int res = check_inverse(a, b);
        if (res == -1) {
            snprintf(out, out_sz, "ERROR_NO_INVERSE");
        } else if (res == 1) {
            snprintf(out, out_sz, "CORRECT");
        } else {
            snprintf(out, out_sz, "FAILED");
        }
        return;
    }

    snprintf(out, out_sz, "ERROR_UNKNOWN_COMMAND");
}

int process_input(FILE *in, FILE *out, RecordList *records) {
    char line[1024];
    while (fgets(line, sizeof(line), in)) {
        char *p = trim(line);
        if (*p == '\0' || *p == '#') {
            continue;
        }

        char *token = strtok(p, ";");
        while (token) {
            char *cmd = trim(token);
            if (*cmd != '\0' && *cmd != '#') {
                char result[MAX_RESULT_TEXT];
                eval_command(cmd, result, sizeof(result));
                if (!records_push(records, cmd, result)) {
                    return 0;
                }
                fprintf(out, "%s -> %s\n", cmd, result);
            }
            token = strtok(NULL, ";");
        }
    }
    return 1;
}
