#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#include "list.h"
#include "list/vector.h"
#include "list/linked.h"

#define COMMAND_LENGTH 4096
#define WHITESPACE " \t"

#define LIST_GET_COMMAND "get"
#define LIST_INSERT_COMMAND "insert"
#define LIST_REMOVE_COMMAND "remove"
#define LIST_LENGTH_COMMAND "length"
#define LIST_ITERATE_COMMAND "iterate"
#define LIST_HELP_COMMAND "help"
#define LIST_PRINT_COMMAND "print"
#define LIST_QUIT_COMMAND "quit"

#define ITERATOR_GET_COMMAND "get"
#define ITERATOR_VALID_COMMAND "valid"
#define ITERATOR_NEXT_COMMAND "next"
#define ITERATOR_PREVIOUS_COMMAND "previous"
#define ITERATOR_INSERT_COMMAND "insert"
#define ITERATOR_REMOVE_COMMAND "remove"
#define ITERATOR_HELP_COMMAND "help"
#define ITERATOR_PRINT_COMMAND "print"
#define ITERATOR_QUIT_COMMAND "quit"

// Note: There's actually a bug here.
//       if the command ever exceeds
//       COMMAND_LENGTH we may cut up
//       tokens.

static char * program_name = "list_test";
static bool is_tty = true;
static char haxx[257];

struct cli_info{
	FILE * input;
	FILE * output;
	char * buffer;
	size_t buffer_length;
	char * token;
	bool eol;
	char * save_tok;
};

enum list_command {
	LIST_INVALID,
	LIST_NONE,
	LIST_GET,
	LIST_INSERT,
	LIST_REMOVE,
	LIST_LENGTH,
	LIST_ITERATE,
	LIST_HELP,
	LIST_PRINT,
	LIST_QUIT
};

enum iterator_command {
	ITERATOR_INVALID,
	ITERATOR_NONE,
	ITERATOR_GET,
	ITERATOR_VALID,
	ITERATOR_NEXT,
	ITERATOR_PREVIOUS,
	ITERATOR_INSERT,
	ITERATOR_REMOVE,
	ITERATOR_HELP,
	ITERATOR_PRINT,
	ITERATOR_QUIT
};


int main(int argc, char ** argv);

/** Prints the usage to the given stream.
 *  
 *  Arguments:
 *    stream: The stream to write to.
 */
void usage(FILE * stream);

/** Consumes the rest of the line
 *
 *  Arguments:
 *    cli: The cli to consume.
 */
void cli_consume_line(struct cli_info * cli);

/** Gets the next token.
 *
 *  Arguments:
 *    cli: The cli to advance.
 *
 *  Returns:
 *    Zero on success. One on EOF.
 */
int cli_next_token(struct cli_info * cli);

/** Check if prefix is a prefix of str
 *
 *  Arguments:
 *    prefix: The prefix to find.
 *    str: The string to check.
 *
 *  Returns:
 *    true if prefix is a prefix of str,
 *    false otherwise.
 */
bool is_prefix(const char *prefix, const char * str);

/** Runs the CLI for lists
 *
 *  Arguments:
 *    cli: The I/O info for the command line.
 *    list: The list to work with.
 */
void list_command_line(struct cli_info * cli, struct dt_list * list);
enum list_command list_command_get(struct cli_info * cli);
void list_get(struct cli_info * cli, struct dt_list * list);
void list_insert(struct cli_info * cli, struct dt_list * list);
void list_remove(struct cli_info * cli, struct dt_list * list);
void list_length(struct cli_info * cli, struct dt_list * list);
void list_iterate(struct cli_info * cli, struct dt_list * list);
void list_help(struct cli_info * cli, struct dt_list * list);
void list_print(struct cli_info * cli, struct dt_list * list);
void list_invalid(struct cli_info * cli, struct dt_list * list);



/** Runs the CLI for list iterators
 *
 *  Arguments:
 *    cli: The I/O info for the command line.
 *    iterator: The list iterator to work with.
 */
void iter_command_line
	(struct cli_info * cli, struct dt_list_iterator * iterator);
enum iterator_command iterator_command_get(struct cli_info * cli);
void iterator_get
	(struct cli_info * cli, struct dt_list_iterator * iterator);
void iterator_valid
	(struct cli_info * cli, struct dt_list_iterator * iterator);
void iterator_next
	(struct cli_info * cli, struct dt_list_iterator * iterator);
void iterator_previous
	(struct cli_info * cli, struct dt_list_iterator * iterator);
void iterator_insert
	(struct cli_info * cli, struct dt_list_iterator * iterator);
void iterator_remove
	(struct cli_info * cli, struct dt_list_iterator * iterator);
void iterator_help
	(struct cli_info * cli, struct dt_list_iterator * iterator);
void iterator_print
	(struct cli_info * cli, struct dt_list_iterator * iterator);
void iterator_invalid
	(struct cli_info * cli, struct dt_list_iterator * iterator);

void usage(FILE * stream)
{
	fprintf(stream, "usage: %s <linked|vector> [[no]tty]\n", program_name);
	fprintf(stream, "\tlinked: test a linked list\n");
	fprintf(stream, "\tvector: test a vector list\n");
	fprintf(stream, "\t[no]tty: [do not] start in interactive mode\n");
}


int main (int argc, char ** argv)
{
	int return_value = 0;

	haxx[256] = '\0';

	if (argc) {
		program_name = argv[0];
	}

	is_tty = isatty(fileno(stdin));

	struct dt_list * list = NULL;

	switch(argc){
	case 3:
		if (strcmp(argv[2], "tty") == 0) {
			is_tty = true;
		} else if (strcmp(argv[2], "notty") == 0) {
			is_tty = false;
		} else {
			usage(stderr);
			return 1;
		}
	case 2:
		if (strcmp(argv[1], "vector") == 0) {
			//list = dt_list_vector_new();
			fprintf(stderr, "Not implemented yet.\n");
			return 2;
		} else if (strcmp(argv[1], "linked") == 0) {
			list = dt_list_linked_new();
		} else {
			usage(stderr);
			return 1;
		}
		break;
	default:
		usage(stderr);
		return 1;		
	}

	struct cli_info cli;
	cli.input = stdin;
	cli.output = stdout;
	cli.buffer_length = sizeof(*(cli.buffer)) * COMMAND_LENGTH;
	cli.buffer = malloc(cli.buffer_length);
	cli.save_tok = NULL;

	cli.buffer[0] = '\0';

	if (!cli.buffer) {
		return_value = 2;
		goto exit_main;
	}

	list_command_line(&cli, list);

	free(cli.buffer);

	exit_main:
	list->del(list);
	return return_value;
}

void cli_consume_line(struct cli_info * cli)
{
	if (cli->eol) return;

	char * token = NULL;
	while (true) {
		if (cli->save_tok == NULL) {
			token = strtok_r(cli->buffer, "", &(cli->save_tok));
		} else {
			token = strtok_r(NULL, "", &(cli->save_tok));
		}

		cli->save_tok = NULL;

		if (token && strchr(token, '\n')) break;

		fgets(cli->buffer, cli->buffer_length, cli->input);
	}
	cli->buffer[0] = '\0';
}

int cli_next_token(struct cli_info * cli)
{
	char * token = NULL;
	do {
		if (cli->save_tok == NULL) {
			token = strtok_r(
				cli->buffer, WHITESPACE, &(cli->save_tok));
		} else {
			token = strtok_r(NULL, WHITESPACE, &(cli->save_tok));
		}
		if (token == NULL) {
			char * return_value = fgets(
				cli->buffer, cli->buffer_length, cli->input);
			if (!return_value) return 1;
			cli->save_tok = NULL;
		}
	} while (token == NULL);
	char * nl = strchr(token, '\n');
	cli->eol = nl != NULL;
	if (nl) *nl = '\0';
	cli->token = token;
	return 0;
}


bool is_prefix(const char *prefix, const char * str)
{
	return strcasestr(str, prefix) == str;
}



void list_command_line(struct cli_info * cli, struct dt_list * list)
{
	while (true){
		if (is_tty) {
			fprintf(cli->output, "> ");
			fflush(cli->output);
		}
		switch(list_command_get(cli)){
		case LIST_NONE: continue;
		case LIST_GET: 
			list_get(cli, list);
			break;
		case LIST_INSERT:
			list_insert(cli, list);
			break;
		case LIST_REMOVE: 
			list_remove(cli, list);
			break;
		case LIST_LENGTH: 
			list_length(cli, list);
			break;
		case LIST_ITERATE:
			list_iterate(cli, list);
			break;
		case LIST_HELP:
			list_help(cli, list);
			break;
		case LIST_PRINT:
			list_print(cli, list);
			break;
		case LIST_QUIT:
			return;
		case LIST_INVALID:
		default:
			list_invalid(cli, list);
		}
		cli_consume_line(cli);
	}
}

enum list_command list_command_get(struct cli_info * cli)
{
	int match_count = 0;
	enum list_command command;

	if (cli_next_token(cli)) {
		fprintf(cli->output, "\n");
		return LIST_QUIT;
	}
	char * text = cli->token;

	if (cli->eol && text[0] == '\0') return LIST_NONE;

	if (is_prefix(text, LIST_GET_COMMAND)) {
		match_count++;
		command = LIST_GET;
	}
	if (is_prefix(text, LIST_INSERT_COMMAND)) {
		match_count++;
		command = LIST_INSERT;
	}
	if (is_prefix(text, LIST_REMOVE_COMMAND)) {
		match_count++;
		command = LIST_REMOVE;
	}
	if (is_prefix(text, LIST_LENGTH_COMMAND)) {
		match_count++;
		command = LIST_LENGTH;
	}
	if (is_prefix(text, LIST_ITERATE_COMMAND)) {
		match_count++;
		command = LIST_ITERATE;
	}
	if (is_prefix(text, LIST_HELP_COMMAND)) {
		match_count++;
		command = LIST_HELP;
	}
	if (is_prefix(text, LIST_PRINT_COMMAND)) {
		match_count++;
		command = LIST_PRINT;
	}
	if (is_prefix(text, LIST_QUIT_COMMAND)) {
		match_count++;
		command = LIST_QUIT;
	}


	if (match_count - 1) return LIST_INVALID;
	return command;
}

void list_get(struct cli_info * cli, struct dt_list * list)
{
	if (cli->eol) {
		fprintf(cli->output, "get: expected index (see help).\n");
		return;
	}

	while (!cli->eol) {
		cli_next_token(cli);

		long index;
		char * endptr;

		errno = 0;

		index = strtol(cli->token, &endptr, 0);

		if (*endptr != '\0') {
			fprintf(cli->output, "get: invalid index (must be a number).\n");
			break;
		} else if (index >= list->length(list) || errno == ERANGE || index < 0) {
			fprintf(cli->output, "get: index out of range (see length).\n");
			continue;
		}

		char * character = list->get(list, index);
		if (character == NULL){
			fprintf(cli->output, "%s", "(NULL)");
		} else {
			fprintf(cli->output, "%c", *character);
		}
		if (!cli->eol) fprintf(cli->output, " ");
	}

	fprintf(cli->output, "\n");
}

void list_insert(struct cli_info * cli, struct dt_list * list)
{
	if (cli->eol) {
		fprintf(cli->output, "insert: expected index and element (see help).\n");
		return;
	}

	cli_next_token(cli);

	if (cli->eol) {
		fprintf(cli->output, "insert: expected element (see help).\n");
		return;
	}

	long index;
	char * endptr;

	index = strtol(cli->token, &endptr, 0);
	
	if (*endptr != '\0') {
		fprintf(cli->output, "insert: invalid index (must be a number).\n");
		return;
	} else if (index > list->length(list) || errno == ERANGE || index < 0) {
		fprintf(cli->output, "insert: index out of range (see length).\n");
		return;
	}

	while (!cli->eol) {
		cli_next_token(cli);

		if (strlen(cli->token) > 1) {
			fprintf(cli->output, "insert: item must be a single charcter.\n");
			return;
		}

		haxx[(int)*(cli->token)] = *(cli->token);
		list->insert(list, index, haxx + *(cli->token));
		index++;
	}

}

void list_remove(struct cli_info * cli, struct dt_list * list)
{
	if (cli->eol) {
		fprintf(cli->output, "remove: expected index (see help).\n");
		return;
	}

	while (!cli->eol) {
		cli_next_token(cli);

		long index;
		char * endptr;

		errno = 0;

		index = strtol(cli->token, &endptr, 0);

		if (*endptr != '\0') {
			fprintf(cli->output, "remove: invalid index (must be a number).\n");
			return;
		} else if (index >= list->length(list) || errno == ERANGE || index < 0) {
			fprintf(cli->output, "remove: index out of range (see length).\n");
			continue;
		}

		list->remove(list, index);
	}
}

void list_length(struct cli_info * cli, struct dt_list * list)
{
	fprintf(cli->output, "Length: %ld\n", list->length(list));
}

void list_iterate(struct cli_info * cli, struct dt_list * list)
{
	struct dt_list_iterator * iterator =
		list->iterator(list);

	if (iterator) {
		iter_command_line(cli, iterator);
		iterator->del(iterator);
	} else {
		fprintf(cli->output, "iterate: creation failed.\n");
	}
}

void list_help(struct cli_info * cli, struct dt_list * list)
{
	fprintf(cli->output,
		"get <index>: Prints the item at the index in the list.\n");
	fprintf(cli->output,
		"insert <index> <item>: Inserts an item into the list at the index.\n");
	fprintf(cli->output,
		"remove <index>: Removes from the list at the index.\n");
	fprintf(cli->output,
		"length: Prints the length of the list.\n");
	fprintf(cli->output,
		"iterate: Enters iterate mode for the list.\n");
	fprintf(cli->output,
		"help: Prints this help.\n");
	fprintf(cli->output,
		"print: Prints out the list in a human readable form.\n");
	fprintf(cli->output,
		"quit: Quits.\n");

}

void list_print(struct cli_info * cli, struct dt_list * list)
{
	struct dt_list_iterator * iterator =
		list->iterator(list);

	if (!iterator) {
		fprintf(cli->output, "print: iterator creation failed.\n");
		return;
	}

	fprintf(cli->output, "[");

	while (iterator->valid(iterator)) {
		char * character = iterator->get(iterator);
		fprintf(cli->output, " %c", *character);
		fflush(cli->output);
		iterator->next(iterator);
	}

	fprintf(cli->output, " ]\n");

	iterator->del(iterator);
}

void list_invalid(struct cli_info * cli, struct dt_list * list)
{
	fprintf(cli->output, "Unrecognized command. Type help for help.\n");
}



void iter_command_line
	(struct cli_info * cli, struct dt_list_iterator * iterator)
{
	while (true){
		if (is_tty) {
			fprintf(cli->output, "iterator> ");
			fflush(cli->output);
		}
		switch(iterator_command_get(cli)){
		case ITERATOR_NONE: continue;
		case ITERATOR_GET: 
			iterator_get(cli, iterator);
			break;
		case ITERATOR_VALID: 
			iterator_valid(cli, iterator);
			break;
		case ITERATOR_NEXT:
			iterator_next(cli, iterator);
			break;
		case ITERATOR_PREVIOUS: 
			iterator_previous(cli, iterator);
			break;
		case ITERATOR_INSERT:
			iterator_insert(cli, iterator);
			break;
		case ITERATOR_REMOVE: 
			iterator_remove(cli, iterator);
			break;
		case ITERATOR_HELP:
			iterator_help(cli, iterator);
			break;
		case ITERATOR_PRINT:
			iterator_print(cli, iterator);
			break;
		case ITERATOR_QUIT:
			return;
		case ITERATOR_INVALID:
		default:
			iterator_invalid(cli, iterator);
		}
		cli_consume_line(cli);
	}
}

enum iterator_command iterator_command_get(struct cli_info * cli)
{
	int match_count = 0;
	enum iterator_command command;

	if (cli_next_token(cli)) {
		fprintf(cli->output, "\n");
		return ITERATOR_QUIT;
	}
	char * text = cli->token;

	if (cli->eol && text[0] == '\0') return ITERATOR_NONE;

	if (is_prefix(text, ITERATOR_GET_COMMAND)) {
		match_count++;
		command = ITERATOR_GET;
	}
	if (is_prefix(text, ITERATOR_VALID_COMMAND)) {
		match_count++;
		command = ITERATOR_VALID;
	}
	if (is_prefix(text, ITERATOR_NEXT_COMMAND)) {
		match_count++;
		command = ITERATOR_NEXT;
	}
	if (is_prefix(text, ITERATOR_PREVIOUS_COMMAND)) {
		match_count++;
		command = ITERATOR_PREVIOUS;
	}
	if (is_prefix(text, ITERATOR_INSERT_COMMAND)) {
		match_count++;
		command = ITERATOR_INSERT;
	}
	if (is_prefix(text, ITERATOR_REMOVE_COMMAND)) {
		match_count++;
		command = ITERATOR_REMOVE;
	}
	if (is_prefix(text, ITERATOR_HELP_COMMAND)) {
		match_count++;
		command = ITERATOR_HELP;
	}
	if (is_prefix(text, ITERATOR_PRINT_COMMAND)) {
		match_count++;
		command = ITERATOR_PRINT;
	}
	if (is_prefix(text, ITERATOR_QUIT_COMMAND)) {
		match_count++;
		command = ITERATOR_QUIT;
	}


	if (match_count - 1) return ITERATOR_INVALID;
	return command;
}

void iterator_get
	(struct cli_info * cli, struct dt_list_iterator * iterator)
{
	if (!iterator->valid(iterator)) {
		fprintf(cli->output, "(INVALID)\n");
	} else {
		char * character = iterator->get(iterator);
		if (character == NULL){
			fprintf(cli->output, "%s\n", "(NULL)");
		} else {
			fprintf(cli->output, "%c\n", *character);
		}
	}
}

void iterator_valid
	(struct cli_info * cli, struct dt_list_iterator * iterator)
{
	if (iterator->valid(iterator)) {
		fprintf(cli->output, "true\n");
	} else {
		fprintf(cli->output, "false\n");
	}
}

void iterator_next
	(struct cli_info * cli, struct dt_list_iterator * iterator)
{
	iterator->next(iterator);
}

void iterator_previous
	(struct cli_info * cli, struct dt_list_iterator * iterator)
{
	iterator->previous(iterator);
}

void iterator_insert
	(struct cli_info * cli, struct dt_list_iterator * iterator)
{
	if (cli->eol) {
		fprintf(cli->output, "insert: expected item to insert (see help).\n");
		return;
	}

	cli_next_token(cli);
	if (strlen(cli->token) > 1) {
		fprintf(cli->output, "insert: item must be a single charcter.\n");
		return;
	}

	haxx[(int)*(cli->token)] = *(cli->token);
	iterator->insert(iterator, haxx + *(cli->token));

}

void iterator_remove
	(struct cli_info * cli, struct dt_list_iterator * iterator)
{
	if (!iterator->valid(iterator)) {
		fprintf(cli->output, "remove: iterator not pointing at item.\n");
	} else {
		iterator->remove(iterator);
	}
}

void iterator_help
	(struct cli_info * cli, struct dt_list_iterator * iterator)
{
	fprintf(cli->output,
		"get: Prints the current item.\n");
	fprintf(cli->output,
		"valid: Prints true if there's an item, false otherwise.\n");
	fprintf(cli->output,
		"next: Moves to the nex item.\n");
	fprintf(cli->output,
		"previous: Moves to the previous item.\n");
	fprintf(cli->output,
		"insert <item>: Insert the item at this position.\n");
	fprintf(cli->output,
		"remove: Removes the current item.\n");
	fprintf(cli->output,
		"help: Prints this help.\n");
	fprintf(cli->output,
		"print: Prints the index.\n");
	fprintf(cli->output,
		"quit: Quits.\n");
}

void iterator_print
	(struct cli_info * cli, struct dt_list_iterator * iterator)
{
	fprintf(cli->output, "Index: %ld\n", iterator->position);
}

void iterator_invalid
	(struct cli_info * cli, struct dt_list_iterator * iterator)
{
	fprintf(cli->output, "Unrecognized command. Type help for help.\n");
}

