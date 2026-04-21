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
