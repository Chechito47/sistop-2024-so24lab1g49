#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <glib.h>

#include "command.h"

struct scommand_s {
    GQueue* comm;
    char* argin;
    char* argout;
};

scommand scommand_new(void) {
    scommand res = malloc(sizeof(struct scommand_s));
    res->comm = NULL;  // existe una g_queue_new() pero el .h especifica que res sea NULL, no son lo mismo?
    res->argin = NULL;
    res->argout = NULL;
    assert(res != NULL && scommand_is_empty(res) && scommand_get_redir_in(res) == NULL && scommand_get_redir_out(res) == NULL);
    return res;
}

scommand scommand_destroy(scommand self) {
    assert(self != NULL);
    while (!scommand_is_empty(self)) {
        g_queue_pop_head(self->comm);
    }
    g_queue_free_full(self->comm, free);
    free(self->argin);
    free(self->argout);
    free(self);
    return NULL;
}

void scommand_push_back(scommand self, char* argument) {
    assert(self != NULL && argument != NULL);
    g_queue_push_tail(self->comm, argument);
    assert(!scommand_is_empty(self));
}

void scommand_pop_front(scommand self) {
    assert(self != NULL && !scommand_is_empty(self));
    g_queue_pop_head(self->comm);
}