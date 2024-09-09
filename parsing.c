#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "parsing.h"
#include "parser.h"
#include "command.h"

static scommand parse_scommand(Parser p) {
	scommand new_cmd = scommand_new();
	arg_kind_t arg_type;
	char* arg;
	
	while (!parser_at_eof(p)) {
       arg = parser_next_argument(p, &arg_type);
       if (arg != NULL) {
       	if (arg_type == ARG_NORMAL) {
       	 scommand_push_back(new_cmd, arg);
        }else if (arg_type == ARG_INPUT) {
           scommand_set_redir_in(new_cmd, arg);
        }else if (arg_type == ARG_OUTPUT) {
            scommand_set_redir_out(new_cmd, arg);
            }
      }else if (arg_type == ARG_INPUT || arg_type == ARG_OUTPUT) {
            	return NULL;
      }else{
        break;
    }
   }  
   return new_cmd;
}

pipeline parse_pipeline(Parser p) {
   assert(p != NULL && !parser_at_eof(p));
	pipeline res = pipeline_new();
   scommand comm = NULL;
   //scommand comm = parse_scommand(p);
   bool error = false, another_pipe = true, is_background = false, garbage = false;

   /*
   if(!error) {
      res = pipeline_new();
      pipeline_push_back(res, comm);
      parser_op_pipe(p, &another_pipe);
   }
   */
   while (another_pipe && !error) {
      comm = parse_scommand(p);
      error = (scommand_is_empty(comm));
      pipeline_push_back(res, comm);
      parser_op_pipe(p, &another_pipe);
   /*
   if(cmd==NULL){
    	printf("Error");
    	pipeline_destroy(result);
    	return NULL;
   }
   */
   }  
    //pipeline_push_back(result, cmd);
    //parser_skip_blanks(p);
    /* 
    bool is_pipe = false;
    parser_op_pipe(p, &is_pipe);
     
    if(is_pipe){
     	another_pipe = true;
     }else{
     	another_pipe = false;
     }
}
   */
	parser_op_background(p, &is_background);
   if (is_background) {
      pipeline_set_wait(res, false);
   }

	//bool garbage = false;
	parser_garbage(p, &garbage);
	if(garbage || error){
		pipeline_destroy(res);
		res = NULL;
      printf("Error: blank input\n");
      //return NULL;
	}

   return res;
}