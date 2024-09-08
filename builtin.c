#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "command.h"
#include "builtin.h"
#include "tests/syscall_mock.h"

#define COUNT_INTERNAL 3                                    //cantidad de comandos internos
char *internal[COUNT_INTERNAL] = {"cd", "help", "exit"};    //lista de comandos simples

bool builtin_is_internal(scommand cmd)
{
    assert(cmd != NULL);
    char *command = scommand_front(cmd);                    //toma el comando
    for (unsigned int i = 0u; i < COUNT_INTERNAL; i++)      //busca si está en la lista de comandos simples
    {
        if (command == internal[i])
        {
            return true;                                    //si lo encuentra retorna true
        }
    }

    return false;                                           //si no lo encuentra en la lista retorna false
}

bool builtin_alone(pipeline p)
{
    assert(p != NULL);
    assert(pipeline_length(p) == 1 && builtin_is_internal(pipeline_front(p)));

    return true;
}

void builtin_run(scommand cmd)
{
    assert(builtin_is_internal(cmd));

    char *command = scommand_front(cmd);
    if (strcmp(command, internal[0]) == 0)
    {
        scommand_pop_front(cmd);                            //quitamos el "cd"
        chdir(scommand_front(cmd));                         //cambiamos al directorio a la ruta que brinda el usuario
    }
    else if (strcmp(command, internal[1]) == 0)             //comando help: imprime un texto plano con información
    {
        printf("Bienvenido a MyBash! \n Autores: Equipo Alfa Dinamita \n Lista de comandos internos: \n cd: Cambio de directorio \n help: Breve explicacion de comandos internos y otras especificaciones \n exit: salir de consola\n");
    }
    else if (strcmp(command, internal[2]) == 0)             //comando exit: mensaje de despedida y termina el ejecutable mybash
    {
        printf("Adios, vuelva prontos...\n");
        exit(EXIT_SUCCESS);
    }
}