#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "command.h"
#include "builtin.h"
#include "tests/syscall_mock.h"

#define COUNT_INTERNAL 3                                        //cantidad de comandos internos
char *internal[COUNT_INTERNAL] = {"cd", "help", "exit"};        //lista de comandos simples

bool builtin_is_internal(scommand cmd) {
    assert(cmd != NULL);
    //char *command = scommand_front(cmd);                      //toma el comando
    for (unsigned int i = 0u; i < COUNT_INTERNAL; i++) {        //busca si está en la lista de comandos simples
        if (!strcmp(scommand_front(cmd), internal[i])) {
            return true;
        }
    }
    return false;                                               //si no lo encuentra en la lista retorna false
}

bool builtin_alone(pipeline p) {
    assert(p != NULL);
    return (pipeline_length(p) == 1 && builtin_is_internal(pipeline_front(p)));//ensure
}

void builtin_run(scommand cmd) {
    assert(builtin_is_internal(cmd));

    char *command = scommand_front(cmd);
    if (strcmp(command, internal[0]) == 0) {
        scommand_pop_front(cmd);                                //quitamos el "cd"
        chdir(scommand_front(cmd));                             //cambiamos al directorio a la ruta que brinda el usuario
    } else if (strcmp(command, internal[1]) == 0) {             //comando help: imprime un texto plano con información
        //Bienvenida
        printf("\x1b[31m");                                     //Red color
        printf("\x1b[4m");                                      //Underline style
        printf("Bienvenido a MyBash!\n");
        printf("\x1b[0m");                                      //Reset color and format
        
        //Autores
        printf("\x1b[31m");
        printf("Autores: ");
        printf("\x1b[0m");                                      
        printf("Equipo Alfa Dinamita: ");
            printf("Nicolás Frattini, ");
            printf("Nicolás Greiff, ");
            printf("Lautaro Pinilla, ");
            printf("Milena Juarez.\n");
        
        //Lista de comandos
        printf("\x1b[31m");
        printf("Lista de comandos internos:\n");
            printf("\x1b[1m");                                  //Bold style
            printf("\x1b[34m");                                 //Blue color
            printf("  *  ");
            printf("\x1b[36m");                                 //Cyan color
            printf("cd");
            printf("\x1b[0m");
            printf(": Cambio de directorio\n");

            printf("\x1b[1m");
            printf("\x1b[34m");
            printf("  *  ");
            printf("\x1b[36m");
            printf("help");
            printf("\x1b[0m");
            printf(": Breve explicacion de comandos internos y otras especificaciones\n");

            printf("\x1b[1m");
            printf("\x1b[34m");
            printf("  *  ");
            printf("\x1b[36m");
            printf("exit");
            printf("\x1b[0m");
            printf(": salir de consola\n");
    } else if (strcmp(command, internal[2]) == 0) {             //comando exit: mensaje de despedida y termina el ejecutable mybash
        printf("\x1b[32m");
        printf("Adios, vuelva prontos...\n");
        exit(EXIT_SUCCESS);
    }
}