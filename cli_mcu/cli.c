#include "cli.h"

#include <string.h>
#include <stdio.h>
#include <ctype.h>


#define POSINC(__x) (((__x) < (CLI_CMD_BUFFER_SIZE - 1)) ? (__x + 1) : (__x))
#define POSDEC(__x) ((__x) ? ((__x) - 1) : 0)

/* ================================================================================ */

static void _cli_prompt(t_cli_ctx *a_ctx __attribute__((unused)), unsigned char nl);
static unsigned char _cli_count_arguments(t_cli_ctx *a_ctx);
static unsigned char _cli_interpret_cmd(t_cli_ctx *a_ctx);
static void _cli_reinterpret_cmd(t_cli_ctx *a_ctx, unsigned char a_response);
static void _cli_history_add(t_cli_ctx *a_ctx);
static void _cli_history_up(void* a_ctx);
static void _cli_history_down(void* a_ctx);
static void _cli_delete(void *a_data);
static void _cli_none(void *a_data __attribute__((unused)));

/* ================================================================================ */

/**
 * @brief global definition of multi-code commands
 */
static t_multicode_cmd _g_mc_cmds[] = {

	{ { 0x1b, 0x5b, 0x44 }, 3, _cli_delete },
	{ { 0x1b, 0x5b, 0x43 }, 3, _cli_none },

	{ { 0x1b, 0x5b, 0x41 }, 3, _cli_history_up },
	{ { 0x1b, 0x5b, 0x42 }, 3, _cli_history_down },

	// null
	{ {0x00}, 0, 0x00}
};

/* ================================================================================ */

void cli_init(t_cli_ctx *a_ctx, t_cmd *a_cmds) {
	memset(a_ctx, 0x00, sizeof(t_cli_ctx));
	a_ctx->cmds = a_cmds;
	a_ctx->mcmds = _g_mc_cmds;
}


void cli_read(t_cli_ctx *a_ctx) {

	unsigned char i = 0x00;
	unsigned char res = E_CMD_OK;

	// if no character available - then exit
	if (!CLI_IO_INPUT(&i)) return;

	/// multi-code matching
	if (a_ctx->mc.pos && (a_ctx->mc.pos < MULTICODE_INPUT_MAX_LEN)) {
		unsigned char mi = 0x00;

		a_ctx->mc.buf[a_ctx->mc.pos++] = i;
		while (a_ctx->mcmds[mi].fh) {
			if (!memcmp(a_ctx->mcmds[mi].pattern, 
						a_ctx->mc.buf,
						a_ctx->mcmds[mi].len)) {
				a_ctx->mcmds[mi].fh((void *)a_ctx);
				a_ctx->mc.pos = 0;
				memset(a_ctx->mc.buf, 0x00, MULTICODE_INPUT_MAX_LEN);
				break;
			}
			mi++;
		}
		return;
	}

	/// char by char matching
	switch(i) {
		case KEY_CODE_BACKSPACE: // backspace
		case KEY_CODE_DELETE: // del
			_cli_delete((void *)a_ctx);
			break;

		case KEY_CODE_ESCAPE: // special characters
			if (a_ctx->mcmds) {
				a_ctx->mc.pos = 1;
				a_ctx->mc.buf[0x00] = i;
			}
			break;

		case KEY_CODE_ENTER: // new line
			a_ctx->cmd[POSINC(a_ctx->cpos)] = '\0';
			CLI_IO_OUTPUT("\r\n", 2);
			res = _cli_interpret_cmd(a_ctx);
			_cli_reinterpret_cmd(a_ctx, res);

			a_ctx->cpos = 0;
			memset(a_ctx->cmd, 0x00, CLI_CMD_BUFFER_SIZE);
			_cli_prompt(a_ctx, 1);
			break;

		default:
			/* echo */
			if (a_ctx->cpos < (CLI_CMD_BUFFER_SIZE - 1) && isprint(i)) {
				a_ctx->cmd[a_ctx->cpos++] = i;
				CLI_IO_OUTPUT(&i, 1);
			}
			break;
	}
}

/* ================================================================================ */

static void _cli_prompt(t_cli_ctx *a_ctx __attribute__((unused)), unsigned char nl) {
	if (nl) CLI_IO_OUTPUT("\r\n", 2);
	CLI_IO_OUTPUT("#> ", 3);
}


static unsigned char _cli_count_arguments(t_cli_ctx *a_ctx) {
	char *cur = a_ctx->cmd;
	unsigned char cnt = 0;
	for(;;) {
		while (*cur == ' ') cur++;
		if (*cur == '\0') break;
		cnt++;
		while (*cur != '\0' && *cur != ' ') {
			cur++;
		}
	}
	return cnt;
}


static unsigned char _cli_interpret_cmd(t_cli_ctx *a_ctx) {
	unsigned char i = 0;
	unsigned char ret = E_CMD_OK;

	if (!strlen(a_ctx->cmd)) {
		return E_CMD_EMPTY;
	}

	if (strlen(a_ctx->cmd) < 2) {
		return E_CMD_TOO_SHORT;
	}

	while (a_ctx->cmds[i].fh) {
		if (!strncmp(a_ctx->cmds[i].cmd, a_ctx->cmd, strlen(a_ctx->cmds[i].cmd))) {

			a_ctx->argc = _cli_count_arguments(a_ctx);

			// call the handler
			a_ctx->cmds[i].fh((void *)a_ctx);
			break;
		}
		i++;
	}

	if (!a_ctx->cmds[i].fh) {
		ret = E_CMD_NOT_FOUND;
	}

	return ret;
}


static void _cli_reinterpret_cmd(t_cli_ctx *a_ctx, unsigned char a_response) {
	switch(a_response) {
		case E_CMD_NOT_FOUND: {
				char str[] = "\r\nCommand not found";
				CLI_IO_OUTPUT(str, strlen(str));
				_cli_history_add(a_ctx);
			}
			break;

		case E_CMD_TOO_SHORT: {
				char str[] = "\r\nCommand too short";
				CLI_IO_OUTPUT(str, strlen(str));
			}
			break;

		default:
			_cli_history_add(a_ctx);
			break;
	} // switch
}


static void _cli_history_add(t_cli_ctx *a_ctx) {	

	if (a_ctx->hhead == CLI_CMD_HISTORY_LEN &&
			a_ctx->htail == CLI_CMD_HISTORY_LEN) {
		a_ctx->hhead = 0;
		a_ctx->htail = 0;
	}
	else {
		a_ctx->hhead++;
		a_ctx->hhead %= CLI_CMD_HISTORY_LEN;
		a_ctx->hpos = a_ctx->hhead;

		if (a_ctx->htail >= a_ctx->hhead) {
			a_ctx->htail = (a_ctx->hhead + 1) % CLI_CMD_HISTORY_LEN;
		}
	}

	memset(a_ctx->history[a_ctx->hhead], 0x00, CLI_CMD_BUFFER_SIZE);
	strcpy(a_ctx->history[a_ctx->hhead], a_ctx->cmd);
}


static void _cli_history_up(void* a_ctx) {

	t_cli_ctx *ctx = (t_cli_ctx *)a_ctx;
	char prompt[12] = {0x00};

	if ((ctx->hhead != ctx->htail) && 
			strlen(ctx->history[ctx->hpos])) {
		memset(ctx->cmd, 0x00, CLI_CMD_BUFFER_SIZE);
		
		strcpy(ctx->cmd, ctx->history[ctx->hpos]);
		ctx->cpos = strlen(ctx->cmd);

		if (!ctx->hpos)
			ctx->hpos = CLI_CMD_HISTORY_LEN - 1;
		else
			ctx->hpos--;

		snprintf(prompt, sizeof(prompt), "\r\n(%d/%d)",
			   	ctx->hpos, CLI_CMD_HISTORY_LEN);
	
		CLI_IO_OUTPUT(prompt, strlen(prompt));
		CLI_IO_OUTPUT(ctx->cmd, strlen(ctx->cmd));
	}
}


static void _cli_history_down(void* a_ctx) {
	t_cli_ctx *ctx = (t_cli_ctx *)a_ctx;
	char prompt[12] = {0x00};

	if ((ctx->hhead != ctx->htail) &&
			strlen(ctx->history[ctx->hpos])) {
		memset(ctx->cmd, 0x00, CLI_CMD_BUFFER_SIZE);
		
		ctx->hpos++;
		ctx->hpos %= CLI_CMD_HISTORY_LEN;

		strcpy(ctx->cmd, ctx->history[ctx->hpos]);
		ctx->cpos = strlen(ctx->cmd);

		snprintf(prompt, sizeof(prompt), "\r\n(%d/%d)",
			   	ctx->hpos, CLI_CMD_HISTORY_LEN);
	
		CLI_IO_OUTPUT(prompt, strlen(prompt));
		CLI_IO_OUTPUT(ctx->cmd, strlen(ctx->cmd));
	}
}


static void _cli_delete(void *a_data) {
	t_cli_ctx *ctx = (t_cli_ctx *)a_data;
	
	if (ctx->cpos) {
		ctx->cmd[ctx->cpos--] = '\0';
		CLI_IO_OUTPUT("\b \b", 3);
	}
}


static void _cli_none(void *a_data) {
	return;
}

/* ================================================================================ */
