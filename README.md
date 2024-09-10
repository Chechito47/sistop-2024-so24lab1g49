# Laboratorio 1 - Grupo 49
## Integrantes:
- Nicolás Frattini
- Nicolás Greiff
- Lautaro Pinilla
- Milena Juarez

## Compilacion y tests
Existen distintos tests para este laboratorio:
### Test command.c
`make test-command`
### Test parsing.c
`make test-parsing`
### Test todos los modulos juntos
`make test`
### Test memory leaks
`make memtest`
Cabe aclarar que para los tres primeros consideramos que se "pasan" al no tener ningun error o fail mientras que para el ultimo con no solo tener memleaks suppressed se considera pasado.

Para compilarlo hacemos `make` y luego `./mybash` para ejecutarlo

## Comandos testeados:
Al referirnos a "funciona correctamente" hacemos referencia a que se hace lo mismo que un terminal comun.

`ls` -> Funciona correctamente

`ls 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1` -> Funciona correctamente

`ls -l -l` -> Funciona correctamente

`xeyes &` y luego `xeyes` -> Funciona correctamente

`ls > out` -> Funciona correctamente

`ls -l > out` -> Funciona correctamente

`wc < /etc/passwd` -> Funciona correctamente, pero queda nuestra linea de input como si fuera una salida, y la salida como si fuera input

`wc < /etc/passwd > out` -> Funciona correctamente

`> out wc < /etc/passwd` -> Funciona correctamente

`wc > /dev/mem` -> No funciona ni en un terminal comun

`wc < /dev/mem` -> No funciona ni en un terminal comun

`ls | wc` -> Funciona correctamente

`du /home/usuario | sort -n &` -> Funciona correctamente

`sleep 1 | sleep 10` -> Funciona correctamente

`sleep 10 | sleep 1` -> Funciona correctamente

`estecomandonoexiste -v` -> Funciona correctamente

`ls >` -> Core dumped, aborta. No deberia abortar si no hacer return y perimitir ingresar otro comando

`ls |` -> Devuelve blank input pero funciona bien

`ls & |` -> Devuelve blank input pero funciona bien

`ls|wc` -> Funciona correctamente

`wc</etc/passwd>out` -> Funciona correctamente

`       ls               -l               command.c` -> Funciona correctamente

` ` (espacio) -> Funciona correctamente

enter -> Funciona correctamente

`ls | wc > out &` -> Hace el > y & pero no hace el ls, probablemente haya un error en el execute

`echo -en "ls\nexit\n" | ./mybash` -> Funciona correctamente

`exit` -> Funciona correctamente

`cd ../..` -> Funciona correctamente

`cd ../../usr/bin/../bin/.././bin` -> Funciona correctamente

`cd /////usr/////bin` -> Funciona correctamente

ctrl + d -> Cierra pero devuelve blank input

Cualquiera cosa con grep no funciona

Cuando la cadena ingresada es de mas de una linea no podemos borrar las primeras lineas, solo la ultima