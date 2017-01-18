#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#include "stack.h"
#include "stack/vector.h"
#include "stack/linked.h"

#include "cli.h"

#define STACK_PROMPT "> "

#define STACK_PUSH_COMMAND "push"
#define STACK_POP_COMMAND "pop"
#define STACK_PEEK_COMMAND "peek"
#define STACK_LENGTH_COMMAND "length"
#define STACK_HELP_COMMAND "help"
#define STACK_PRINT_COMMAND "print"
#define STACK_QUIT_COMMAND "quit"


static char * program_name = "stack_test";
static bool is_tty = true;
static char * stack_prompt = STACK_PROMPT;
static char haxx[257];

int main(int argc, char ** argv);

/** Prints the usage to the given stream.
 *
 *  Arguments:
 *    stream: The stream to write to.
 */
void usage(FILE * stream);

// Stack commands.
int stack_push(struct cli_t * cli, struct dt_stack * stack);
int stack_pop(struct cli_t * cli, struct dt_stack * stack);
int stack_peek(struct cli_t * cli, struct dt_stack * stack);
int stack_length(struct cli_t * cli, struct dt_stack * stack);
int stack_help(struct cli_t * cli, struct dt_stack * stack);
int stack_quit(struct cli_t * cli, struct dt_stack * stack);

static struct cli_menu_item_t stack_items[] = {
	{STACK_PUSH_COMMAND, (int (*)(struct cli_t *, void *))&stack_push},
	{STACK_POP_COMMAND, (int (*)(struct cli_t *, void *))&stack_pop},
	{STACK_PEEK_COMMAND, (int (*)(struct cli_t *, void *))&stack_peek},
	{STACK_LENGTH_COMMAND, (int (*)(struct cli_t *, void *))&stack_length},
	{STACK_HELP_COMMAND, (int (*)(struct cli_t *, void *))&stack_help},
	{STACK_QUIT_COMMAND, (int (*)(struct cli_t *, void *))&stack_quit}
};

void usage(FILE * stream)
{
	fprintf(stream, "usage: %s <linked|vector> [[no]tty]\n", program_name);
	fprintf(stream, "\tlinked: test a linked stack\n");
	fprintf(stream, "\tvector: test a vector stack\n");
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

	struct dt_stack * stack = NULL;

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
			stack = dt_stack_vector_new();
			break;
		} else if (strcmp(argv[1], "linked") == 0) {
			stack = dt_stack_linked_new();
			break;
		} else {
			usage(stderr);
			return 1;
		}
	case 1:
		stack = dt_stack_new();
	case 0:
		break;
	default:
		usage(stderr);
		return 1;		
	}

	if(!is_tty) {
		stack_prompt = "";
	}

	struct cli_t cli;

	if(cli_init(&cli, stack_prompt, stdin, stdout, stderr)) {
		fprintf(stderr, "Failed to initialize CLI.\n");
		return_value = 1;
		goto exit_main;
	}

	if (stack) {
		return_value = cli_exec(
			&cli,
			stack_items, sizeof(stack_items) / sizeof(*stack_items),
			stack);
	} else {
		fprintf(stderr, "Failed to make stack\n");
		return_value = 1;
	}

	cli_dispose(&cli);

	exit_main:
	stack->del(stack);
	return return_value;
}

int stack_push(struct cli_t * cli, struct dt_stack * stack)
{
	if (cli->eol) {
		fprintf(cli->output,
			"push: expected item to push (see help).\n");
		return CLI_CONTINUE;
	}

	while (!cli->eol) {
		cli_advance_token(cli);

		if (strlen(cli->token) > 1) {
			fprintf(cli->output,
				"push: item must be a single character.\n");
			return CLI_CONTINUE;
		}

		haxx[(int)*(cli->token)] = *(cli->token);
		stack->push(stack, haxx + *(cli->token));
	}

	return CLI_CONTINUE;
}

int stack_pop(struct cli_t * cli, struct dt_stack * stack)
{
	long count = 1;
	if (!cli->eol) {
		cli_advance_token(cli);

		char * endptr;

		errno = 0;

		count = strtol(cli->token, &endptr, 0);

		if (*endptr != '\0') {
			fprintf(cli->output, "pop: invalid amount.\n");
			return CLI_CONTINUE;
		} else if (count > stack->length(stack) || errno == ERANGE || count < 0) {
			fprintf(cli->output, "pop: can't pop that many (see length).\n");
			return CLI_CONTINUE;
		}
	}

	for (;count;count--) {
		char * character = stack->pop(stack);
		if (character == NULL){
			fprintf(cli->output, "%s", "(NULL)");
		} else {
			fprintf(cli->output, "%c", *character);
		}
		if (count - 1) fprintf(cli->output, " ");
	}

	fprintf(cli->output, "\n");
	return CLI_CONTINUE;
}

int stack_peek(struct cli_t * cli, struct dt_stack * stack)
{
	char * character = stack->peek(stack);
	if (character == NULL) {
		fprintf(cli->output, "%s\n", "(NULL)");
	} else {
		fprintf(cli->output, "%c\n", *character);
	}
	return CLI_CONTINUE;
}

int stack_length(struct cli_t * cli, struct dt_stack * stack)
{
	fprintf(cli->output, "Length: %ld\n", stack->length(stack));
	return CLI_CONTINUE;
}

int stack_help(struct cli_t * cli, struct dt_stack * stack)
{
	fprintf(cli->output,
		"push <item>: Push the item on to the stack.\n");
	fprintf(cli->output,
		"pop: Pops an item from the stack.\n");
	fprintf(cli->output,
		"peek: Peeks at the item on top of the stack without removing it.\n");
	fprintf(cli->output,
		"length: Prints the length of the stack.\n");
	fprintf(cli->output,
		"help: Prints this help.\n");
	fprintf(cli->output,
		"quit: Quits.\n");
	return CLI_CONTINUE;
}


int stack_quit(struct cli_t * cli, struct dt_stack * stack)
{
	return CLI_EXIT_SUCCESS;
}
