#ifndef __PARSER_H__
#define __PARSER_H__

# define  FILENAME_LENGTH       256
# define  DEVICE_NAME_LENGTH     32

# include <stddef.h>

typedef enum {
	T_LIST,                     // list all processes
	T_LIST_INACTIVE,            // list inactive processes
	T_ADD,                      // add a process
	T_DELETE_ALL,               // delete all processes
	T_DELETE_ID,                // delete by pid
	T_DELETE_DEVICE,            // delete by name
	T_DELETE_INACTIVE,          // remove all inactive processes
	T_TERMINATE,                // terminate program
	T_EXIT,                     // exit (only for the shell)
	T_LOAD,                     // load a file (only for the shell)
	T_INVALID                   // invalid command
} type_t;

typedef struct {
	type_t type;                          // type of token

	char filename[FILENAME_LENGTH];       // used by T_LOAD
	char device[DEVICE_NAME_LENGTH];      // used by T_ADD_DEVICE and T_DELETE_DEVICE
	int mode;                             // used by T_ADD_DEVICE
	size_t id;                            // used by T_DELETE_ID
} token_t;


void parse_command(token_t *token, char* command);

#endif
