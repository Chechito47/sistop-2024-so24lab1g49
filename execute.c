#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <glib.h>

#include "builtin.h"
#include "command.h"
#include "execute.h"

#include "tests/syscall_mock.h"

/*
 * Ejecuta un pipeline, identificando comandos internos, forkeando, y
 *   redirigiendo la entrada y salida. puede modificar `apipe' en el proceso
 *   de ejecución.
 *   apipe: pipeline a ejecutar
 * Requires: apipe!=NULL
 */

/*
    First we need to divide the execution based in the length of the pipeline
*/

/*
void execute_single_pipeline(pipeline apipe) {
    assert(apipe != NULL);

    //First we check how many pipelines are
    unsigned int command_length = pipeline_length(apipe);       //Here we use a function of command.c
    //If there are more thean 1 error
    if (command_length > 1) {
        printf (stderr, "Command too long\n");
        exit(EXIT_FAILURE);
    } else {
        //Now there are at least 1 pipeline, so first we check if it's an interal command or not 
        if (builtin_is_internal(apipe)) {
            builtin_run(apipe);
        } else {
            //If it's not an internal command
        }
    }
} */



//Execute a command as extern without making fork
//Requires: comm!=NULL && scommand_is_empty(comm)==False
void execute_command(scommand comm) {
    assert(comm != NULL && !scommand_is_empty(comm));
    unsigned int comm_length = scommand_length(comm);                       //Get the length of the command
    char *arg_list[] = calloc(comm_length + 1; sizeof(char*));              //+1 due the use of *args_comm[]
    for (unsigned int i; i < comm_length; i++) {
        arg_list[i] = strdup(scommand_front(comm));                         //Saves the pointer of front char of a sequence of chars
        scommand_pop_front(comm);                                           //Takes put the char of front
    }

    char *redir_in = scommand_get_redir_in(comm);                           //Declarate variables for stdin and stdout
    char *redir_out = scommand_get_redir_out(comm);
    
    if (redir_in != NULL) {                                                 //If stdin is not null
        unsigned int fd_redir_in = open(redir_in, O_RDONLY, S_IRWXU);       //Declarate an aux to open redir_in in read only and gives write permission
        if(fd_redir_in < 0 || fd_redir_in == NULL){                         //If is negative error opening the file
            fprintf(stderr, "Error opening the file. bash: %s\n", redir_in);
            exit(EXIT_FAILURE);
        }
        dup2(fd_redir_in, 0);                                               //Redirect the content of fd_redir_in to stdin
        close(fd_redir_in);                                                 //Close fd_redir_in
    }

    if (redir_out != NULL) {                                                //If stdout is not null
        unsigned int fd_redir_out = open(redir_out, O_CREAT | O_WRONLY);    //Declarate an aux to open (or create if not exist) redir_out in only write mode
        if(fd_redir_out < 0 || fd_redir_out == NULL){                       //If is negative error opening the file
            fprintf(stderr, "Error opening the file. bash: %s\n", redir_out);
            exit(EXIT_FAILURE);
        }
        dup2(fd_redir_out, 0);                                              //Redirect the content of fd_redir_out to stdout
        close(fd_redir_out);                                                //Close fd_redir_out
    }

    free(redir_in);                                                         //Free redir_in and out from memory
    free(redir_out);
    if (execvp(arg_list[0], arg_list) == -1) {                              //If replace the process failed
        fprintf(stderr, "Invalid command: %s.\n", arg_list[0]);
        exit(EXIT_FAILURE);
    }
}



//Execute a pipeline of a single command making fork
//Requires: apipe!=NULL && apipe length==1
void execute_single_pipeline(pipeline apipe) {
    assert(apipe != NULL && pipeline_length(apipe)==1);
    pid = fork();
    //
    if (pid < 0) {
        printf("Error couldn't create child, fork failed\n");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        execute_command(pipeline_front(apipe));
    } else if (pipeline_get_wait(apipe)) {
        wait(NULL);
    }
    pipeline_pop_front(apipe);
}
