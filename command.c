#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <glib.h>

#include "command.h"

struct scommand_s {
    GList* comm;
    char* redir_in;
    char* redir_out;
};

scommand scommand_new(void) {
    scommand res = malloc(sizeof(struct scommand_s));
    res->comm = NULL;
    res->redir_in = NULL;
    res->redir_out = NULL;
    assert(res != NULL && scommand_is_empty(res) && scommand_get_redir_in(res) == NULL && scommand_get_redir_out(res) == NULL);
    return res;
}

scommand scommand_destroy(scommand self) {
    assert(self != NULL);
    g_list_free_full(self->comm, free);
    g_list_free(self->comm);
    free(self->redir_in);
    free(self->redir_out);
    free(self);
    return NULL;
}

void scommand_push_back(scommand self, char* argument) {
    assert(self != NULL && argument != NULL);
    self->comm = g_list_append(self->comm, argument);
    assert(!scommand_is_empty(self));
}

void scommand_pop_front(scommand self) {
    assert(self != NULL && !scommand_is_empty(self));
    Glist* aux = g_list_first(self->comm);
    free(aux->data);
    self->comm = g_list_remove(self->comm, aux->data);
}

void scommand_set_redir_in(scommand self, char* filename) {
    assert(self != NULL);
    free(self->redir_in);
    self->redir_in = filename;
}

void scommand_set_redir_out(scommand self, char* filename) {
    assert(self != NULL);
    free(self->redir_out);
    self->redir_out = filename;
}

bool scommand_is_empty(const scommand self) {
    assert(self != NULL);
    return g_list_length(self->comm) == 0 && self->redir_in == NULL && self->redir_out == NULL;
}

unsigned int scommand_length(const scommand self) {
    assert(self != NULL);
    return g_list_length(self->comm);
    assert((scommand_length(self) == 0) == scommand_is_empty(self));
}

char * scommand_front(const scommand self) {
    assert(self != NULL && !scommand_is_empty(self));
    assert(self->comm != NULL);
    return self->comm->data;
}

char * scommand_get_redir_in(const scommand self) {
    assert(self != NULL);
    return self->redir_in;
}

char * scommand_get_redir_out(const scommand self) {
    assert(self != NULL);
    return self->redir_out;
}

char * scommand_to_string(const scommand self) {
    assert(self != NULL);
    

    assert(scommand_is_empty(self) || scommand_get_redir_in(self) == NULL || scommand_get_redir_out(self) == NULL || strlen(res) > 0);
}