#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>                     //Asserts
#include <glib.h>
#include <fcntl.h>                      //For open
#include <unistd.h>                     //For dup2 and close
#include <sys/wait.h>                   //For wait

#include "builtin.h"
#include "command.h"
#include "execute.h"

#include "tests/syscall_mock.h"

#define STD_IN 0                        //stdin=0
#define STD_OUT 1                       //stdout=1

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

//Execute a command as extern (if is not builtin) without making fork
//Requires: comm!=NULL && scommand_is_empty(comm)==False
static void execute_external(scommand comm) {
    assert(comm != NULL && !scommand_is_empty(comm));                       //Assert conditions
    unsigned int comm_length = scommand_length(comm);                       //Get the length of string
    char **arg_list = calloc(comm_length + 1, sizeof(char*));               //+1 due the use of **arg_list
    for (unsigned int i = 0; i < comm_length; i++) {
        arg_list[i] = strdup(scommand_front(comm));                         //Saves commands in the array
        scommand_pop_front(comm);                                           //Takes out the front string of the string sequence
    }

    char *redir_in = scommand_get_redir_in(comm);                           //Declarate variables for stdin and stdout
    char *redir_out = scommand_get_redir_out(comm);

    if (redir_in != NULL) {                                                 //If stdin is not null
        int fd_redir_in = open(redir_in, O_RDONLY, 0666);                   //Declarate an aux to open redir_in in read only. Because we include syscall_mock.h we need to add 0666 to open this file normally
        if(fd_redir_in < 0) {                                               //If is negative error opening the file
            fprintf(stderr, "Error: file or directory '%s' doesn't exists\n", redir_in);
            exit(EXIT_FAILURE);
        }
        dup2(fd_redir_in, 0);                                               //Redirect the content of fd_redir_in to stdin
        close(fd_redir_in);                                                 //Close fd_redir_in
    }

    if (redir_out != NULL) {
        int fd_redir_out = open(redir_out, O_CREAT | O_WRONLY, 0666);       //Declarate an aux to open redir_out in write only mode or create it if don't exists (because it's an out we need to assert that it exists)
        if(fd_redir_out < 0) {
            fprintf(stderr, "Error: file or directory '%s' doesn't exists\n", redir_out);
            exit(EXIT_FAILURE);
        }
        dup2(fd_redir_out, STD_OUT);                                        //Redirect the content of fd_redir_out to stdout
        close(fd_redir_out);
    }

    if (execvp(arg_list[STD_IN], arg_list) == -1) {                         //If couldn't change the execution of the process => error
        fprintf(stderr, "Error: command '%s' not found\n", arg_list[0]);
        exit(EXIT_FAILURE);
    }

    free(redir_in);                                                         //Free memory
    free(redir_out);
}



//Execute a pipeline of a single command making fork
//Requires: apipe!=NULL && apipe length==1
static void execute_single_pipeline(pipeline apipe) {
    assert(apipe != NULL && pipeline_length(apipe) == 1);                   //Check assert
    pid_t pid = fork();                                                     //Make fork, used pid_t because that's the type for pids
    //unsigned int child_quantity = 0;                                        //Count childs

    if (pid < 0) {                                                          //If pid<0 =>error
        printf("Error: fork failed\n");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {                                                  //If pid==0 is the child
        execute_external(pipeline_front(apipe));
    } else {
            if (pipeline_get_wait(apipe)) {                                 //If pid>0 it's the parent
            wait(NULL);                                                     //The parent wait for the child to end his execution. waitpid(pid, NULL, 0);
        }
        //child_quantity++;
        //pipeline_pop_front(apipe);                                        //Takes out the simple command
    }
    pipeline_pop_front(apipe);                                              //Takes out the simple command
    //assert(child_quantity == 1);
}



//Execute a pipeline of multiple commands
//Requires: apipe!=NULL && apipe length(apipe)>=2
static void execute_multiple_pipeline(pipeline apipe) {
    assert(apipe != NULL && pipeline_length(apipe) >= 2);
    int argv[2];                                                            //Declarate argv to make execvp

    if (pipe(argv) < 0) {                                                   //Check if pipe fail
        printf("Error, pipe fail\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();                                                     //Make fork
    //pipe(argv);                                                           //Declarete argv to make execvp

    if (pid < 0) {                                                          //If pid<0 => error
        printf("Error: fork failed\n");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {                                                         //If pid==0 => it's the child
        close(argv[STD_IN]);                                                //Close stdin because we are not going to use it here
        dup2(argv[STD_OUT], STD_OUT);                                       //Think argv as fd, redirect output to stdout, STD_OUT=1
        close(argv[STD_OUT]);                                               //Close stdout
        execute_external(pipeline_front(apipe));                              //Execute the simple command of front as an external
        pipeline_pop_front(apipe);                                          //After execute the command of front, take it out
    } else if (pid > 0) {                                                   //If pid>0 => it's the parent
        close(argv[STD_OUT]);                                               //If it's the parent close stdout because here we are not going to use it
        pid = fork();
        if (pid == 0) {
            dup2(argv[STD_IN], STD_IN);                                     //Redirect input to stdin, STD_IN=0
            close(argv[STD_IN]);                                            //Close stdin
            pipeline_pop_front(apipe);                                      //Order change due it's not the front command
            execute_external(pipeline_front(apipe));
        } else if (pid > 0) {
            close(argv[STD_IN]);                                            //If it's the parent close stdin, because firtsly we closed stdout, now stdin
        }
    }
    wait(NULL);                                                             //Parents wait for childs
    wait(NULL);
}



//Execute a pipeline making calls to different functions
//Functions:
//  Execute internal command
//  Execute one external command
//  Execute multiple external command chained by a pipe
//Requires: cmd!=NULL
void execute_pipeline(pipeline apipe) {
    assert(apipe != NULL);                                                  //Check assert
    if (pipeline_is_empty(apipe)) {                                         //If not pipeline => go back to wait for an input 
        printf("Error: blank input\n");
        return;
    } else if (builtin_alone(apipe)) {                                      //If apipe is a builtin command => builtin_run (it's an internal command)
        builtin_run(pipeline_front(apipe));                                 //pipeline_front because if it's a builtin then it's going to be single command => help, cd, exit
    } else if (pipeline_length(apipe) == 1) {                               //If pipeline_length == 1 then call the function to execute a single pipeline
        execute_single_pipeline(apipe);
    } else if (pipeline_length(apipe) >= 2) {                               //If pipeline_length >= 2 then call the function to execute multiple pipelines
        execute_multiple_pipeline(apipe);
    } else {                                                                //Else return => wait for an input
        return;
    }
}