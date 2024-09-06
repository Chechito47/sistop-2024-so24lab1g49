#include <stdlib.h>
#include <stdbool.h>

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
	pipeline result = pipeline_new();
	scommand cmd = NULL;
    bool error = false, another_pipe=true, is_background=false;  
    
    while (another_pipe && !error) {
     cmd = parse_scommand(p);
    if(cmd==NULL){
    	printf("Error");
    	pipeline_destroy(result);
    	return NULL;
    }
     
    pipeline_push_back(result, cmd);
    parser_skip_blanks(p);
     
    bool is_pipe = false;
    parser_op_pipe(p, &is_pipe);
     
    if(is_pipe){
     	another_pipe = true;
     }else{
     	another_pipe = false;
     }
}
     
	parser_op_background(p,&is_background);
	pipeline_set_wait(result, is_background);	

	bool garbage = false;
	parser_garbage(p, &garbage);
	if(garbage){
		pipeline_destroy(result);
		return NULL;
	}

   return result;
}

