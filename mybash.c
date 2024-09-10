#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "command.h"
#include "execute.h"
#include "parser.h"
#include "parsing.h"
#include "builtin.h"
#include "limits.h"                             //IDEM as #include <linux/limits.h>, it's for PATH_MAX
#include "obfuscated.h"

static void show_prompt(void) {
    //Print <mybash>:
    printf ("\x1b[40m");                        //Background black
    printf ("\x1b[36m");                        //Cyan color
    printf ("<mybash>");
    printf ("\033[0m");                         //Reset color and format
    printf (":");
    char dir[PATH_MAX];                         //Define a char for directory
    if (getcwd(dir, sizeof(dir)) != NULL) {     //If getcwd(used to get actual directory path, with the size of dir) is not null (null if error) then print it
        //Print ~actual directory
        printf ("\x1b[33m");                    //Yellow color
        printf("~%s", dir);
        printf ("\033[0m");
        printf ("$ ");
    }
    //Print input and output
    printf("\033[0m");
    fflush (stdout);
}

int main(int argc, char *argv[]) {
    pipeline pipe;
    Parser input;
    bool quit = false;

    input = parser_new(stdin);
    while (!quit) {
        //ping_pong_loop(NULL);
        show_prompt();
        pipe = parse_pipeline(input);

        /* Hay que salir luego de ejecutar? */
        quit = parser_at_eof(input);
        if (pipe != NULL) {
            execute_pipeline(pipe);
            pipeline_destroy(pipe);
            pipe = NULL;
        }
    }
    parser_destroy(input); input = NULL;
    return EXIT_SUCCESS;
}

