# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <ctype.h>
# include <fcntl.h>
# include <unistd.h>

# include "fifo.h"
# include "proclist.h"

# include "parser.h"


# define  FLUSH(print)                                         \
    do {                                                       \
        print;                                                 \
        fflush(stdout);                                        \
    } while (0);


# define  ERROR(errmsg)                                        \
    fprintf( stderr                                            \
           , "error: %s\n"                                     \
             "called from\n"                                   \
             "  file: %s\n"                                    \
             "  line: %d\n"                                    \
           , errmsg, __FILE__, __LINE__ );



void
fifo_listen()
{
	// printf("listening on: %s\n", fifofile);
	const int fd = open(fifofile, O_RDWR);

	proclist_t list;
	proclist_init(&list);

	char buffer[FIFO_BUFFER_SIZE];

	size_t count;

	/*
	 * the main loop
	 *
	 * parse the command and dispatch
	 */
	while ( memset(buffer, 0, FIFO_BUFFER_SIZE)                // clear buffer
	      , read(fd, buffer, FIFO_BUFFER_SIZE) ) {

		token_t token;
		parse_command(&token, buffer);

		switch (token.type) {
			case T_LIST:
				proclist_print(&list);
				break;

			case T_LIST_INACTIVE:
				proclist_print_inactive(&list);
				break;

			case T_ADD:
				FLUSH(printf("adding { device = %s; mode = %d } ", token.device, token.mode))
				if (proclist_add(&list, proc_new(token.device, token.mode)))
					puts("added!");
				else
					puts("not added!");
				break;

			case T_DELETE_ID:
				FLUSH(printf("deleting { id = %zu } ", token.id))
				if (proclist_deleteby_id(&list, token.id))
					puts("deleted!");
				else
					puts("not deleted!");
				break;

			case T_DELETE_DEVICE:
				FLUSH(printf("deleting { device = %s } ", token.device))
				if (proclist_deleteby_device(&list, token.device))
					puts("deleted!");
				else
					puts("not deleted!");
				break;

			case T_DELETE_ALL:
				FLUSH(printf("deleting all processes... "))
				count = list.size;
				if (proclist_delete_all(&list))
					FLUSH(printf("done, deleted %zu processes!\n", count))
				else
					ERROR("delete all failed")
				break;

			case T_DELETE_INACTIVE:
				FLUSH(printf("removing inactive processes... "))
				count = proclist_delete_inactive(&list);
				FLUSH(printf("done, %zu processes found!\n", count))
				break;

			case T_TERMINATE:
				puts("terminating program...");
				exit(0);
				break;

			case T_INVALID:
				FLUSH(printf("invalid commnad: %s\n", buffer))
				break;
		}
	}

	close(fd);
}

