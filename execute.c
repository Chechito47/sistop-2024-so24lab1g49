#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>                     //Asserts
#include <glib.h>
#include <fcntl.h>                      //For open
#include <unistd.h>                     //For dup2 and close
#include <sys/wait.h>                   //For wait
//#include <sys/types.h>
//#include <sys/stat.h>

#include "builtin.h"
#include "command.h"
#include "execute.h"

#include "tests/syscall_mock.h"

#define STD_IN 0
#define STD_OUT 1

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
    char **arg_list = calloc(comm_length + 1, sizeof(char*));              //+1 due the use of *args_comm[]
    for (unsigned int i = 0; i < comm_length; i++) {
        arg_list[i] = strdup(scommand_front(comm));                         //Saves the pointer of front char of a sequence of chars
        scommand_pop_front(comm);                                           //Takes put the char of front
    }

    char *redir_in = scommand_get_redir_in(comm);                           //Declarate variables for stdin and stdout
    char *redir_out = scommand_get_redir_out(comm);
    
    if (redir_in != NULL) {                                                 //If stdin is not null
        int fd_redir_in = open(redir_in, O_CREAT | O_RDONLY, 0666);               //Declarate an aux to open redir_in in read only and gives write permission. S_IRWXU instead of WRONLY
        if(fd_redir_in < 0) {                                               //If is negative error opening the file
            fprintf(stderr, "Error opening the file. bash: %s\n", redir_in);
            exit(EXIT_FAILURE);
        }
        dup2(fd_redir_in, 0);                                               //Redirect the content of fd_redir_in to stdin
        close(fd_redir_in);                                                 //Close fd_redir_in
    }

    if (redir_out != NULL) {                                                //If stdout is not null
        int fd_redir_out = open(redir_out, O_CREAT | O_WRONLY, 0666);             //Declarate an aux to open (or create if not exist) redir_out in only write mode
        if(fd_redir_out < 0)    {                                           //If is negative error opening the file
            fprintf(stderr, "Error opening the file. bash: %s\n", redir_out);
            exit(EXIT_FAILURE);
        }
        dup2(fd_redir_out, 1);                                              //Redirect the content of fd_redir_out to stdout
        close(fd_redir_out);                                                //Close fd_redir_out
    }
    
    //free(redir_in);                                                         //Free redir_in and out from memory
    //free(redir_out);
    if (execvp(arg_list[0], arg_list) == -1) {                              //If replace the process failed
        fprintf(stderr, "Invalid command: %s.\n", arg_list[0]);
        exit(EXIT_FAILURE);
    }
    free(redir_in);
    free(redir_out);
}



//Execute a pipeline of a single command making fork
//Requires: apipe!=NULL && apipe length==1
static void execute_single_pipeline(pipeline apipe) {
    assert(apipe != NULL && pipeline_length(apipe) == 1);                   //Check assert
    pid_t pid = fork();                                                    //Make fork
    unsigned int child_quantity = 0;

    if (pid < 0) {                                                          //If pid<0 =>error
        printf("Error couldn't create child, fork failed\n");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {                                                  //If pid==0 is the child
        execute_extern(pipeline_front(apipe));
    } else {
            if (pipeline_get_wait(apipe)) {                                  //If pid>0 is the parent
            wait(NULL);                                                         //waitpid(pid, NULL, 0);
        }
    child_quantity++;
    pipeline_pop_front(apipe);
    }
    //pipeline_pop_front(apipe);
    assert(child_quantity == 1);
}



//Execute a pipeline of two command making two fork and one pipe
//Requires: apipe!=NULL && apipe length(apipe)==2
static void execute_multiple_pipeline(pipeline apipe) {
    assert(apipe != NULL && pipeline_length(apipe) >= 2);                   //IDEM as single pipeline with pipeline_length==2
    //unsigned int length_pipe = pipeline_length(apipe);
    int argv[2];

    if (pipe(argv) < 0) {
        printf("Error, pipe fail\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();   
    //pipe(argv);                                                      //Declarete argv to make execvp

    if (pid < 0) {                                                          //If pid<0 => error
        printf("Error, couldn't create child, fork failed\n");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {                                                  //If pid==0 => it's the child
        close(argv[STD_IN]);
        
        dup2(argv[STD_OUT], STDOUT_FILENO);                                                      //Dup standar output => output of execute_extern(pipeline_front(apipe))
        close(argv[STD_OUT]);
        /*
        if (dup2(argv[STD_OUT], STDOUT_FILENO) == -1) {
            printf("Error, error with dup2\n");
            exit(EXIT_FAILURE);
        }
        if (close(argv[STD_OUT]) == -1) {
            printf("Error, error with file descriptor\n");
            exit(EXIT_FAILURE);
        }*/
        execute_extern(pipeline_front(apipe));                              //Execute command as an external
        pipeline_pop_front(apipe);
    } else if (pid > 0) {
        close(argv[STD_OUT]);

        pid = fork();
        if (pid == 0) {
            
            dup2(argv[STD_IN], STDIN_FILENO);
            close(argv[STD_IN]);
            //close(argv[STD_OUT]);
            /*
            if (dup2(argv[STD_IN], STDIN_FILENO) == -1) {
            printf("Error, error with dup2\n");
            exit(EXIT_FAILURE);
            }
            if (close(argv[STD_IN]) == -1) {
                printf("Error, error with file descriptor\n");
                exit(EXIT_FAILURE);
            }*/
            pipeline_pop_front(apipe);
            execute_extern(pipeline_front(apipe));
        } else if (pid > 0) {
            close(argv[STD_IN]);
        }
    }
    wait(NULL);
    wait(NULL);
    //if (pipeline_get_wait(apipe)) {                                         //Check if the pipeline has to wait before being execute or not (because now we don't have one)
    //    waitpid(pid, NULL, 0);
    //}
}




//Execute a pipeline making calls to different functions
//Functions:
//  Execute internal command
//  Execute one external command
//  Execute two external command chained by a pipe
//Requires: cmd!=NULL && pipeline_length(apipe)>3
void execute_pipeline(pipeline apipe) {
    assert(apipe != NULL);                                                  //Check assert

    if (pipeline_is_empty(apipe)) {                                         //If not pipeline => go back to wait for an input 
        printf("Non entry found, blank input\n");
        return;
    } else if (builtin_alone(apipe)) {                                      //If apipe is a builtin command => builtin_run (it's an internal command)
        builtin_run(pipeline_front(apipe));                                 //pipeline_front because if it's a builtin then it's going to be single command => help, cd, exit
    } else if (pipeline_length(apipe) == 1) {                               //If pipeline_length == 1 then call the function to execute a single pipeline
        execute_single_pipeline(apipe);
    //} else if (pipeline_length(apipe) == 2) {                               //If pipeline_length == 2 then call the function to execute two pipelines
    //    execute_double_pipeline(apipe);
    } else {                                                                //Else error => don't have execute_pipeline for multiple pipelines
        execute_multiple_pipeline(apipe);
        //printf("Invalid input\n");
        //return;
    }
}