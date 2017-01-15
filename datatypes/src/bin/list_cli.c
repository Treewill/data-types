#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#include "list.h"
#include "list/vector.h"
#include "list/linked.h"

#include "cli.h"

#define LIST_PROMPT "> "
#define ITERATOR_PROMPT "iterator> "

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

static char * program_name = "list_test";
static bool is_tty = true;
static char * list_prompt = LIST_PROMPT;
static char * iterator_prompt = ITERATOR_PROMPT;
static char haxx[257];

int main(int argc, char ** argv);

/** Prints the usage to the given stream.
 *
 *  Arguments:
 *    stream: The stream to write to.
 */
void usage(FILE * stream);

// List commands.
int list_get(struct cli_t * cli, struct dt_list * list);
int list_insert(struct cli_t * cli, struct dt_list * list);
int list_remove(struct cli_t * cli, struct dt_list * list);
int list_length(struct cli_t * cli, struct dt_list * list);
int list_iterate(struct cli_t * cli, struct dt_list * list);
int list_help(struct cli_t * cli, struct dt_list * list);
int list_print(struct cli_t * cli, struct dt_list * list);
int list_quit(struct cli_t * cli, struct dt_list * list);

// Iterator commands.
int iterator_get(struct cli_t * cli, struct dt_list_iterator * iterator);
int iterator_valid(struct cli_t * cli, struct dt_list_iterator * iterator);
int iterator_next(struct cli_t * cli, struct dt_list_iterator * iterator);
int iterator_previous(struct cli_t * cli, struct dt_list_iterator * iterator);
int iterator_insert(struct cli_t * cli, struct dt_list_iterator * iterator);
int iterator_remove(struct cli_t * cli, struct dt_list_iterator * iterator);
int iterator_help(struct cli_t * cli, struct dt_list_iterator * iterator);
int iterator_print(struct cli_t * cli, struct dt_list_iterator * iterator);
int iterator_quit(struct cli_t * cli, struct dt_list_iterator * iterator);

static struct cli_menu_item_t list_items[] = {
	{LIST_GET_COMMAND, (int (*)(struct cli_t *, void *))&list_get},
	{LIST_INSERT_COMMAND, (int (*)(struct cli_t *, void *))&list_insert},
	{LIST_REMOVE_COMMAND, (int (*)(struct cli_t *, void *))&list_remove},
	{LIST_LENGTH_COMMAND, (int (*)(struct cli_t *, void *))&list_length},
	{LIST_ITERATE_COMMAND, (int (*)(struct cli_t *, void *))&list_iterate},
	{LIST_HELP_COMMAND, (int (*)(struct cli_t *, void *))&list_help},
	{LIST_PRINT_COMMAND, (int (*)(struct cli_t *, void *))&list_print},
	{LIST_QUIT_COMMAND, (int (*)(struct cli_t *, void *))&list_quit}
};

static struct cli_menu_item_t iterator_items[] = {
	{ITERATOR_GET_COMMAND, (int (*)(struct cli_t *, void *))&iterator_get},
	{ITERATOR_VALID_COMMAND, (int (*)(struct cli_t *, void *))&iterator_valid},
	{ITERATOR_NEXT_COMMAND, (int (*)(struct cli_t *, void *))&iterator_next},
	{ITERATOR_PREVIOUS_COMMAND, (int (*)(struct cli_t *, void *))&iterator_previous},
	{ITERATOR_INSERT_COMMAND, (int (*)(struct cli_t *, void *))&iterator_insert},
	{ITERATOR_REMOVE_COMMAND, (int (*)(struct cli_t *, void *))&iterator_remove},
	{ITERATOR_HELP_COMMAND, (int (*)(struct cli_t *, void *))&iterator_help},
	{ITERATOR_PRINT_COMMAND, (int (*)(struct cli_t *, void *))&iterator_print},
	{ITERATOR_QUIT_COMMAND, (int (*)(struct cli_t *, void *))&iterator_quit}
};

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
			list = dt_list_vector_new();
			break;
		} else if (strcmp(argv[1], "linked") == 0) {
			list = dt_list_linked_new();
			break;
		} else {
			usage(stderr);
			return 1;
		}
	case 1:
		list = dt_list_new();
	case 0:
		break;
	default:
		usage(stderr);
		return 1;		
	}

	if(!is_tty) {
		list_prompt = "";
		iterator_prompt = "";
	}

	struct cli_t cli;

	if(cli_init(&cli, list_prompt, stdin, stdout, stderr)) {
		fprintf(stderr, "Failed to initialize CLI.\n");
		return_value = 1;
		goto exit_main;
	}

	if (list) {
		return_value = cli_exec(
			&cli,
			list_items, sizeof(list_items) / sizeof(*list_items),
			list);
	} else {
		fprintf(stderr, "Failed to make list\n");
		return_value = 1;
	}

	cli_dispose(&cli);

	exit_main:
	list->del(list);
	return return_value;
}

int list_get(struct cli_t * cli, struct dt_list * list)
{
	if (cli->eol) {
		fprintf(cli->output, "get: expected index (see help).\n");
		return CLI_CONTINUE;
	}

	while (!cli->eol) {
		cli_advance_token(cli);

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
	return CLI_CONTINUE;
}

int list_insert(struct cli_t * cli, struct dt_list * list)
{
	if (cli->eol) {
		fprintf(cli->output, "insert: expected index and element (see help).\n");
		return CLI_CONTINUE;
	}

	cli_advance_token(cli);

	if (cli->eol) {
		fprintf(cli->output, "insert: expected element (see help).\n");
		return CLI_CONTINUE;
	}

	long index;
	char * endptr;

	index = strtol(cli->token, &endptr, 0);
	
	if (*endptr != '\0') {
		fprintf(cli->output, "insert: invalid index (must be a number).\n");
		return CLI_CONTINUE;
	} else if (index > list->length(list) || errno == ERANGE || index < 0) {
		fprintf(cli->output, "insert: index out of range (see length).\n");
		return CLI_CONTINUE;
	}

	while (!cli->eol) {
		cli_advance_token(cli);

		if (strlen(cli->token) > 1) {
			fprintf(cli->output, "insert: item must be a single charcter.\n");
			return CLI_CONTINUE;
		}

		haxx[(int)*(cli->token)] = *(cli->token);
		list->insert(list, index, haxx + *(cli->token));
		index++;
	}
	return CLI_CONTINUE;
}

int list_remove(struct cli_t * cli, struct dt_list * list)
{
	if (cli->eol) {
		fprintf(cli->output, "remove: expected index (see help).\n");
		return CLI_CONTINUE;
	}

	while (!cli->eol) {
		cli_advance_token(cli);

		long index;
		char * endptr;

		errno = 0;

		index = strtol(cli->token, &endptr, 0);

		if (*endptr != '\0') {
			fprintf(cli->output, "remove: invalid index (must be a number).\n");
			return CLI_CONTINUE;
		} else if (index >= list->length(list) || errno == ERANGE || index < 0) {
			fprintf(cli->output, "remove: index out of range (see length).\n");
			continue;
		}

		list->remove(list, index);
	}
	return CLI_CONTINUE;
}

int list_length(struct cli_t * cli, struct dt_list * list)
{
	fprintf(cli->output, "Length: %ld\n", list->length(list));
	return CLI_CONTINUE;
}

int list_iterate(struct cli_t * cli, struct dt_list * list)
{
	struct dt_list_iterator * iterator =
		list->iterator(list);

	if (iterator) {
		cli_advance_line(cli);
		char * last_prompt = cli->prompt;
		cli->prompt = iterator_prompt;
		cli_exec(
			cli,
			iterator_items,
			sizeof(iterator_items) / sizeof(*iterator_items),
			iterator);
		cli->prompt = last_prompt;
		iterator->del(iterator);
	} else {
		fprintf(cli->output, "iterate: creation failed.\n");
	}
	return CLI_CONTINUE;
}

int list_help(struct cli_t * cli, struct dt_list * list)
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
	return CLI_CONTINUE;
}

int list_print(struct cli_t * cli, struct dt_list * list)
{
	struct dt_list_iterator * iterator =
		list->iterator(list);

	if (!iterator) {
		fprintf(cli->output, "print: iterator creation failed.\n");
		return CLI_CONTINUE;
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
	return CLI_CONTINUE;
}

int list_quit(struct cli_t * cli, struct dt_list * list) {
	return CLI_EXIT_SUCCESS;
}


int iterator_get
	(struct cli_t * cli, struct dt_list_iterator * iterator)
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
	return CLI_CONTINUE;
}

int iterator_valid
	(struct cli_t * cli, struct dt_list_iterator * iterator)
{
	if (iterator->valid(iterator)) {
		fprintf(cli->output, "true\n");
	} else {
		fprintf(cli->output, "false\n");
	}
	return CLI_CONTINUE;
}

int iterator_next
	(struct cli_t * cli, struct dt_list_iterator * iterator)
{
	iterator->next(iterator);
	return CLI_CONTINUE;
}

int iterator_previous
	(struct cli_t * cli, struct dt_list_iterator * iterator)
{
	iterator->previous(iterator);
	return CLI_CONTINUE;
}

int iterator_insert
	(struct cli_t * cli, struct dt_list_iterator * iterator)
{
	if (cli->eol) {
		fprintf(cli->output, "insert: expected item to insert (see help).\n");
		return CLI_CONTINUE;
	}

	cli_advance_token(cli);
	if (strlen(cli->token) > 1) {
		fprintf(cli->output, "insert: item must be a single charcter.\n");
		return CLI_CONTINUE;
	}

	haxx[(int)*(cli->token)] = *(cli->token);
	iterator->insert(iterator, haxx + *(cli->token));

	return CLI_CONTINUE;
}

int iterator_remove
	(struct cli_t * cli, struct dt_list_iterator * iterator)
{
	if (!iterator->valid(iterator)) {
		fprintf(cli->output, "remove: iterator not pointing at item.\n");
	} else {
		iterator->remove(iterator);
	}
	return CLI_CONTINUE;
}

int iterator_help
	(struct cli_t * cli, struct dt_list_iterator * iterator)
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
	return CLI_CONTINUE;
}

int iterator_print
	(struct cli_t * cli, struct dt_list_iterator * iterator)
{
	fprintf(cli->output, "Index: %ld\n", iterator->position);
	return CLI_CONTINUE;
}

int iterator_quit(struct cli_t * cli, struct dt_list_iterator * iterator)
{
	return CLI_EXIT_SUCCESS;
}
