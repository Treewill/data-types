#include "cli.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define WHITESPACE " \t"
#define COMMAND_BUFFER_SIZE 4096

#define QUIT_COMMAND "quit"
#define HELP_COMMAND "help"

/** A strtok_r wrapper.
 *
 *  Arguments:
 *    str: The string to parse.
 *         (ignored if saveptr is not pointing at NULL).
 *    delim: The string of delimiters.
 *    saveptr: Saves the context.
 *
 *  Returns:
 *    The token found.
 */
static char * read_token(char * str, char * delim, char ** saveptr);

/** Prints an message about an invalid command.
 *
 *  Arguments:
 *    cli: The cli to use.
 */
static int print_invalid(struct cli_t * cli, void * args);

/** Selects the appropriate menu item.
 *
 *  Arguments:
 *    command_text: The command text.
 *    items: The menu items.
 *    item_count: The number of menu items.
 *
 *  Returns:
 *    The matching menu item. Or a special value
 *    to indicate one of the additional options.
 */
static struct cli_menu_item_t * select_item(
	char * token, struct cli_menu_item_t * items, size_t item_count);

/** Checks if prefix is a prefix of word.
 *
 *  Arguments:
 *    prefix: The prefix.
 *    word: The word.
 *
 *  Returns:
 *    true iff prefix is a prefix of word.
 */
static bool is_prefix(char * prefix, char * word);

// A dummy item to indicate the entry was invalid.
static struct cli_menu_item_t invalid_item = {(char *)NULL, &print_invalid};

int cli_init(struct cli_t * cli, char * prompt,
	FILE * input, FILE * output, FILE * error)
{
	const size_t buffer_size = COMMAND_BUFFER_SIZE * sizeof(*(cli->buffer));

	cli->prompt = prompt;
	cli->input = input;
	cli->output = output;
	cli->error = error;
	
	cli->buffer = malloc(buffer_size);
	cli->buffer_size = buffer_size;

	cli->token = NULL;
	cli->eol = false;

	cli->save_tok = NULL;

	if (!cli->buffer) return -1;
	return 0;

}

void cli_dispose(struct cli_t * cli)
{
	free(cli->buffer);
}

// Note: There's actually a bug here.
//       if the command ever exceeds
//       COMMAND_BUFFER_SIZE we may
//       cut up tokens.
int cli_advance_token(struct cli_t * cli)
{
        char * token = read_token(cli->buffer, WHITESPACE, &(cli->save_tok));
	if (token == NULL) {
		if (!fgets(cli->buffer, cli->buffer_size, cli->input)){
			return -1;
		}
		cli->save_tok = NULL;
		token = read_token(cli->buffer, WHITESPACE, &(cli->save_tok));
	}

        char * nl = strchr(token, '\n');
        cli->eol = nl != NULL;
        if (nl) *nl = '\0';

        cli->token = token;
        return 0;
}

void cli_advance_line(struct cli_t * cli)
{
	do {
		if (cli->eol) break;
	} while (!cli_advance_token(cli));
}


int cli_exec(struct cli_t * cli,
	struct cli_menu_item_t * items, size_t item_count,
	void * args)
{
	int status = 0;

	fprintf(cli->output, "%s", cli->prompt);
	while(!cli_advance_token(cli)) {
		struct cli_menu_item_t * item =
			select_item(cli->token, items, item_count);

		status = item->function(cli, args);
		cli_advance_line(cli);
		
		if (status) break;

		fprintf(cli->output, "%s", cli->prompt);
	}
	return status - 1;
}

static char * read_token(char * str, char * delim, char ** saveptr) {
	if (*saveptr) return strtok_r(NULL, delim, saveptr);
	return strtok_r(str, delim, saveptr);
}

static struct cli_menu_item_t * select_item(
	char * token, struct cli_menu_item_t * items, size_t item_count)
{
	struct cli_menu_item_t * result = NULL;
	
	for (size_t i = 0; i < item_count; i++) {
		if (!strcasecmp(token, items[i].command)) return items + i;
		if (is_prefix(token, items[i].command)) {
			if (result) {
				result = &invalid_item;
			} else {
				result = items + i;
			}
		}
	}

	if (!result) return &invalid_item;
	return result;
}

static bool is_prefix(char * prefix, char * word) {
	if (!(prefix || word)) return false;

	for(int i = 0; prefix[i] && word[i]; i++) {
		if (tolower(prefix[i]) != tolower(word[i])) return false;
	}

	return true;
}

static int print_invalid(struct cli_t * cli, void * args)
{
	fprintf(cli->error, "Unrecognized command.\n");
	return 0;
}
