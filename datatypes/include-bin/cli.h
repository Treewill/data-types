#ifndef __CLI_H_
#define __CLI_H__

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum cli_return_status {
	// Continue parsing CLI commands.
	CLI_CONTINUE = 0,
	// Exit CLI, successfully.
	CLI_EXIT_SUCCESS = 1,
	// Exit CLI, unsucessfully, returning 1.
	CLI_EXIT_ERROR = 2
};

struct cli_t;
struct cli_menu_item_t;

struct cli_menu_item_t {
	char * command;
	/** Does the work.
	 *
	 *  Arguments:
	 *    cli: The cli requesting the action.
	 *    args: A user supplied struct for additional
	 *          arguments.
	 *
	 *  Returns:
	 *    A cli_return_status.
	 */
	int (* function)(struct cli_t * cli, void * args);
};

struct cli_t {
	char * prompt;
	FILE * input;
	FILE * output;
	FILE * error;
	char * buffer;
	size_t buffer_size;
	char * token;
	bool eol;
	char * save_tok;
};

/** Initializes a CLI.
 *
 *  Arguments:
 *    cli: The cli to initialize.
 *    prompt: The prompt text.
 *    input: The input file.
 *    output: The output file.
 *    error: The error file.
 *
 *  Returns:
 *    Zero on success. -1 otherwise.
 */
int cli_init(struct cli_t * cli, char * prompt,
	FILE * input, FILE * output, FILE * error);

/** Releases any resources used by the CLI.
 *
 *  Arguments:
 *    cli: The cli.
 *
 *  Notes:
 *    The cli cannot be used until re-initialized.
 */
void cli_dispose(struct cli_t * cli);

/** Advances the cli forward a token.
 *
 *  Arguments:
 *    cli: The cli.
 *
 *  Returns:
 *    Zero if there is more input. -1 otherwise.
 */
int cli_advance_token(struct cli_t * cli);

/** Advances the cli past the current line.
 *
 *  Arguments:
 *    cli: The cli.
 */
void cli_advance_line(struct cli_t * cli);

/** Start the interactive cli.
 *
 *  Arguments:
 *    cli: The cli.
 *    items: The menu items.
 *    item_count: The number of menu items.
 *    args: The arguments to pass to the menu items.
 *
 *  Returns:
 *    Zero on success and the exit code of the last command
 *    less one on failure.
 */
int cli_exec(struct cli_t * cli,
	struct cli_menu_item_t * items, size_t item_count,
	void * args);


#ifdef __cplusplus
}
#endif
#endif //__CLI_H__
