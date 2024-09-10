#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "command.h"
#include "execute.h"
#include "parser.h"
#include "parsing.h"
#include "builtin.h"
#include "limits.h"
#include "obfuscated.h"

static void show_prompt(void) {
    //Print <mybash>:
    printf ("\x1b[40m");                        //
    printf ("\x1b[36m");
    printf ("<mybash>");
    printf ("\033[0m");
    printf (":");
    char dir[PATH_MAX];
    if (getcwd(dir, sizeof(dir)) != NULL) {
        //Print ~actual directory
        printf ("\x1b[33m");
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

