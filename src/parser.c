# include <stdlib.h>
# include <stdbool.h>
# include <string.h>
# include <ctype.h>
# include "parser.h"

static void copy_over_string(char *dst, char *src);
static bool matches(const char* user, const char* input, const size_t length);


/*
 * list of commands
 *
 *   ~> list                                               :>  T_LIST
 *     list all process
 *
 *   ~> list inactive                                      :> T_LIST_INACTIVE
 *     list inactive process
 *
 *   ~> add 'device' 'mode'                                :>  T_ADD
 *     add a process to the process list
 *
 *   ~> delete id 'n'                                      :>  T_DELETE_ID
 *     deletes the process with id 'n'
 *
 *   ~> delete all                                         :>  T_DELETE_ALL
 *     deletes all running processes
 *
 *   ~> delete device 'device'                             :>  T_DELETE_DEVICE
 *     deletes process listening on device 'device'
 *
 *   ~> delete inactive                                    :>  T_DELETE_INACTIVE
 *     remove all inactive processes from list
 *
 *   ~> terminate                                          :>  T_TERMINATE
 *     kills this program
 *
 * anything else is an invalid command
 */


void
parse_command(token_t *token, char* raw_command)
{
	char *c = raw_command;

# define  SKIP_WORD             while (isalnum(*c)) c++;
# define  SKIP_WHITESPACE       while (isspace(*c)) c++;
# define  MATCHES(string)       matches(string, c, strlen(string))

	SKIP_WHITESPACE

	switch (*c) {
		case 'l':    // list
			if (MATCHES("list")) {
				SKIP_WORD            // skip "list"
				SKIP_WHITESPACE

				if (*c == '\0')
					token->type = T_LIST;
				else if (MATCHES("inactive"))
					token->type = T_LIST_INACTIVE;
				else
					token->type = T_INVALID;

			}

			break;

		case 'a':    // add
			if (MATCHES("add")) {
				token->type = T_ADD;

				SKIP_WORD          // skip "add"
				SKIP_WHITESPACE

				copy_over_string(token->device, c);

				SKIP_WORD         // skip device name
				SKIP_WHITESPACE

				// copy over mode
				token->mode = atoi(c);
			}
			else
				token->type = T_INVALID;

			break;

		case 'd':    // delete
			if (MATCHES("delete")) {
				SKIP_WORD             // skip "delete"
				SKIP_WHITESPACE

				switch (*c) {
					case 'i':    // id, inactive
						if (MATCHES("id")) {
							token->type = T_DELETE_ID;

							SKIP_WORD          // skip "id"
							SKIP_WHITESPACE

							token->id = atoi(c);
						}
						else if (MATCHES("inactive"))
							token->type = T_DELETE_INACTIVE;
						else
							token->type = T_INVALID;

						break;

					case 'd':    // device
						if (MATCHES("device")) {
							token->type = T_DELETE_DEVICE;

							SKIP_WORD          // skip "device"
							SKIP_WHITESPACE

							// copy over device name
							copy_over_string(token->device, c);
						}
						else
							token->type = T_INVALID;

						break;

					case 'a':    // all
						if (MATCHES("all"))
							token->type = T_DELETE_ALL;
						else
							token->type = T_INVALID;

						break;

					default:
						token->type = T_INVALID;

						break;
				}
			}

			break;

		case 't':    // terminate
			if (MATCHES("terminate"))
				token->type = T_TERMINATE;
			else
				token->type = T_INVALID;

			break;

		default:
			token->type = T_INVALID;

			break;
	}
}

static void
copy_over_string(char *dst, char *src)
{
	size_t n = 0;
	while (!isspace(*src))
		dst[n++] = *src++;
	dst[n] = '\0';
}

static bool
empty(char c)
{
	return c == '\0' || isspace(c);
}


static bool
matches(const char* user, const char* input, const size_t length)
{
	return strncmp(user, input, length) == 0 && empty(input[length]);
}

