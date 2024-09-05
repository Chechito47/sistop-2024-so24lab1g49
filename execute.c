#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <glib.h>
#include <fcntl.h>

#include "builtin.h"
#include "command.h"
#include "execute.h"

#include "tests/syscall_mock.h"

/*
execute_pipeline(apipe)
([scommand], bool)
CASOS:
[] -> nada

x:[] -> if (x is builtin_is_internal) -> builtin_run(scommand)
     ->    (x is not builtin_is_interl) -> defino execute_scommand -> hace for y hace el execvp

pipe!=pipeline
x:y:xs (con xs!=[]) -> execute_multiple_scommand -> crear un pipe, hacer el fork, execute
*/

//Execute a command as extern without making fork
//Requires: comm!=NULL && scommand_is_empty(comm)==False
static void execute_extern(scommand comm) {
    assert(comm != NULL && !scommand_is_empty(comm));
    unsigned int comm_length = scommand_length(comm);                       //Get the length of the command
    char *arg_list[] = calloc(comm_length + 1, sizeof(char*));              //+1 due the use of *args_comm[]
    for (unsigned int i; i < comm_length; i++) {
        arg_list[i] = strdup(scommand_front(comm));                         //Saves the pointer of front char of a sequence of chars
        scommand_pop_front(comm);                                           //Takes put the char of front
    }

    char *redir_in = scommand_get_redir_in(comm);                           //Declarate variables for stdin and stdout
    char *redir_out = scommand_get_redir_out(comm);
    
    if (redir_in != NULL) {                                                 //If stdin is not null
        unsigned int fd_redir_in = open(redir_in, O_RDONLY, O_WRONLY);      //Declarate an aux to open redir_in in read only and gives write permission. S_IRWXU instead of WRONLY
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
static void execute_single_pipeline(pipeline apipe) {
    assert(apipe != NULL && pipeline_length(apipe) == 1);                   //Check assert
    pid_t pid = fork();                                                     //Make fork

    if (pid < 0) {                                                          //If pid<0 =>error
        printf("Error couldn't create child, fork failed\n");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {                                                  //If pid==0 is the child
        execute_command(pipeline_front(apipe));
    } else if (pipeline_get_wait(apipe)) {                                  //If pid>0 is the parent
        wait(NULL);                                                         //waitpid(pid, NULL, 0);
    }
    //pipeline_pop_front(apipe);
}



//Execute a pipeline of two command making two fork and one pipe
//Requires: apipe!=NULL && apipe length(apipe)==2
static void execute_double_pipeline(pipeline apipe) {
    assert(apipe != NULL && pipeline_length(apipe) == 2);                   //IDEM as single pipeline with pipeline_length==2
    pid_t pid = fork();                                                     //Make fork
    char *argv[3];                                                          //Declarete argv to make execvp

    if (pid < 0) {                                                          //If pid<0 => error
        printf("Error, couldn't create child, fork failed\n");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {                                                  //If pid==0 => it's the child
        dup2(argv[1]);                                                      //Dup standar output => output of execute_extern(pipeline_front(apipe)) 
        close(argv[0]);                                                     //Close what we opened
        close(argv[1]);
        close(argv[2]);
        execute_extern(pipeline_front(apipe));                              //Execute command as an external
    }
    //pipeline_pop_front(apipe);

    if (pipeline_get_wait(apipe)) {                                         //Check if the pipeline has to wait before being execute or not (because now we don't have one)
        waitpid(pid, NULL, 0);
    }
}



//Execute a pipeline making calls to different functions
//Functions:
//  Execute internal command
//  Execute one external command
//  Execute two external command chained by a pipe
//Requires: cmd!=NULL && pipeline_length(apipe)>3
void execute_pipeline(apipe) {
    assert(apipe != NULL);                                                  //Check assert

    if (pipeline_is_empty(apipe)) {                                         //If apipe==NULL => go back to wait for an input 
        printf("Non entry found, blank input\n");
        return;
    } else if (builtin_alone(apipe)) {                                      //If apipe is a builtin command => builtin_run (it's an internal command)
        builtin_run(pipeline_front(apipe));                                 //pipeline_front because if it's a builtin then it's going to be single command => help, cd, exit
    } else if (pipeline_length(apipe) == 1) {                               //If pipeline_length == 1 then call the function to execute a single pipeline
        execute_single_pipeline(apipe);
    } else if (pipeline_length(apipe) == 2) {                               //If pipeline_length == 2 then call the function to execute two pipelines
        execute_double_pipeline(apipe);
    } else {                                                                //Else error => don't have execute_pipeline for multiple pipelines
        printf("Invalid input\n");
        return;
    }
}