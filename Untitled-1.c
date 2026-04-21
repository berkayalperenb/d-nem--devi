#include "parser.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: numtool <input_file> <output_file>\n");
        return 1;
    }

    FILE *in = fopen(argv[1], "r");
    if (!in) {
        fprintf(stderr, "ERROR: cannot open input file: %s\n", argv[1]);
        return 1;
    }

    FILE *out = fopen(argv[2], "w");
    if (!out) {
        fprintf(stderr, "ERROR: cannot open output file: %s\n", argv[2]);
        fclose(in);
        return 1;
    }

    RecordList records;
    records_init(&records);

    if (!process_input(in, out, &records)) {
        fprintf(stderr, "ERROR: processing failed (memory allocation)\n");
        records_free(&records);
        fclose(in);
        fclose(out);
        return 1;
    }

    records_free(&records);
    fclose(in);
    fclose(out);
    return 0;
}



CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
TARGET = numtool
SRCS = main.c parser.c math_utils.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

#include "math_utils.h"

static int64_t ll_abs(int64_t x) {
    return (x < 0) ? -x : x;
}

int64_t gcd_ll(int64_t a, int64_t b) {
    a = ll_abs(a);
    b = ll_abs(b);
    while (b != 0) {
        int64_t t = a % b;
        a = b;
        b = t;
    }
    return a;
}

static int64_t ext_gcd(int64_t a, int64_t b, int64_t *x, int64_t *y) {
    if (b == 0) {
        *x = 1;
        *y = 0;
        return a;
    }

    int64_t x1 = 0, y1 = 0;
    int64_t g = ext_gcd(b, a % b, &x1, &y1);
    *x = y1;
    *y = x1 - (a / b) * y1;
    return g;
}

int mod_inverse(int64_t a, int64_t m, int64_t *inv) {
    if (m <= 1) {
        return 0;
    }
    int64_t x = 0, y = 0;
    int64_t g = ext_gcd(a, m, &x, &y);
    if (g != 1 && g != -1) {
        return 0;
    }
    int64_t r = x % m;
    if (r < 0) {
        r += m;
    }
    *inv = r;
    return 1;
}

#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <stdint.h>

int64_t gcd_ll(int64_t a, int64_t b);
int mod_inverse(int64_t a, int64_t m, int64_t *inv);
int64_t mod_pow(int64_t a, int64_t b, int64_t m);
int is_prime_ll(int64_t n);
int64_t phi_ll(int64_t n);
int check_inverse(int64_t a, int64_t m);

#endif

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
        if (n != 3) {
            snprintf(out, out_sz, "ERROR_INVALID_PARAMS");
            return;
        }
        snprintf(out, out_sz, "%" PRId64, gcd_ll(a, b));
        return;
    }
    if (strcmp(op, "INV") == 0) {
        if (n != 3) {
            snprintf(out, out_sz, "ERROR_INVALID_PARAMS");
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
            snprintf(out, out_sz, "ERROR_INVALID_PARAMS");
            return;
        }
        int64_t v = mod_pow(a, b, c);
        if (v < 0) {
            snprintf(out, out_sz, "ERROR_INVALID_PARAMS");
            return;
        }
        snprintf(out, out_sz, "%" PRId64, v);
        return;
    }
    if (strcmp(op, "PRIME") == 0) {
        if (n != 2) {
            snprintf(out, out_sz, "ERROR_INVALID_PARAMS");
            return;
        }
        snprintf(out, out_sz, is_prime_ll(a) ? "YES" : "NO");
        return;
    }
    if (strcmp(op, "PHI") == 0) {
        if (n != 2) {
            snprintf(out, out_sz, "ERROR_INVALID_PARAMS");
            return;
        }
        int64_t v = phi_ll(a);
        if (v < 0) {
            snprintf(out, out_sz, "ERROR_INVALID_PARAMS");
            return;
        }
        snprintf(out, out_sz, "%" PRId64, v);
        return;
    }
    if (strcmp(op, "CHECK") == 0) {
        if (n != 3) {
            snprintf(out, out_sz, "ERROR_INVALID_PARAMS");
            return;
        }
        snprintf(out, out_sz, check_inverse(a, b) ? "CORRECT" : "ERROR_NO_INVERSE");
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
                fprintf(out, "%s\n", result);
            }
            token = strtok(NULL, ";");
        }
    }
    return 1;
}

#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stddef.h>

#define MAX_CMD_TEXT 256
#define MAX_RESULT_TEXT 256

typedef struct {
    char command[MAX_CMD_TEXT];
    char result[MAX_RESULT_TEXT];
} CommandRecord;

typedef struct {
    CommandRecord *items;
    size_t count;
    size_t capacity;
} RecordList;

void records_init(RecordList *list);
void records_free(RecordList *list);
int records_push(RecordList *list, const char *cmd, const char *result);

int process_input(FILE *in, FILE *out, RecordList *records);

#endif
