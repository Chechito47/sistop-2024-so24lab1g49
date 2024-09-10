/* Ejecuta comandos simples y pipelines.
 * No toca ningún comando interno.
 */

#ifndef EXECUTE_H
#define EXECUTE_H

#include "command.h"

//static void execute_extern(scommand comm);
/*
 * Ejecuta un comando como externo haciendo una llamada a su funcion, 
 *   redirigiendo la entrada y salida. Puede modificar `apipe' en el proceso
 *   de ejecución.
 *   apipe: pipeline a ejecutar
 * Requires: apipe!=NULL && !scommand_is_empty(comm)
 */

//static void execute_single_pipeline(pipeline apipe);
/*
 * Ejecuta comandos con un pipeline, forkeando, y
 *   redirigiendo la entrada y salida. puede modificar `apipe' en el proceso
 *   de ejecución.
 *   apipe: pipeline a ejecutar
 * Requires: apipe!=NULL && pipeline_length(apipe) == 1
 */

//static void execute_multiple_pipeline(pipeline apipe);
 /*
 * Ejecuta comandos con multiples pipelines, forkeando, y
 *   redirigiendo la entrada y salida. puede modificar `apipe' en el proceso
 *   de ejecución.
 *   apipe: pipeline a ejecutar
 * Requires: apipe!=NULL && pipeline_length(apipe) >= 1
 */

void execute_pipeline(pipeline apipe);
/*
 * Decide a que funcion llamar para su correcta ejecucion segun el tipo de cadena
 *   apipe: pipeline a ejecutar
 * Requires: apipe!=NULL
 */

#endif /* EXECUTE_H */
