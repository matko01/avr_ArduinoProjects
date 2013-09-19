/* Copyright (C) 
 * 2013 - Tomasz Wisniewski
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "cli.h"
#include "serial.h"
#include "marquee.h"

/* ================================ private ======================================= */

static unsigned char _cli_interpret_cmd(t_cli_ctx *a_ctx);
static void _cli_prompt(t_cli_ctx *a_ctx, unsigned char nl);
static void _cli_reinterpret_cmd(t_cli_ctx *a_ctx, unsigned char a_response);
static void _cli_history_up(void *a_ctx);
static void _cli_history_down(void *a_ctx);
static void _cli_history_add(t_cli_ctx *a_ctx);
static void _cli_quit_owner(t_cli_ctx *a_ctx);

/* ================================================================================ */


/* ================================= commands ===================================== */

static void fh_ping(void *a_ctx);
static void fh_tomek(void *a_ctx);
static void fh_marque_get(void *a_ctx);
static void fh_delay_inc(void *a_ctx);
static void fh_delay_dec(void *a_ctx);
static void fh_clear(void *a_ctx);
static void fh_serial_get(void *a_ctx);
static void fh_put(void *a_ctx);
static void fh_help(void *a_ctx);

/* ================================================================================ */


/* ================================== globals ===================================== */

/// global context holder
static t_cli_ctx g_cli_ctx;

/**
 * @brief command dispatching table
 */
static t_cmd g_cmds[] = {
	{ "ping", fh_ping, 0x00 },
	{ "clear", fh_clear, 0x00 },
	{ "mget", fh_marque_get, 0x00 },
	{ "dinc", fh_delay_inc, 0x00 },
	{ "ddec", fh_delay_dec, 0x00 },
	{ "sget", fh_serial_get, 0x00 },
	{ "mput", fh_put, 0x01 },
	{ "tomek", fh_tomek, 0x00 },
	{ "help", fh_help, 0x00 },

	// null
	{ {0x00}, 0x00, 0x00 }
};

/**
 * @brief multichar keycodes
 */
t_multichar_cmd g_mchar_cmds[] = {

	{ { 0x1b, 0x5b, 0x41 }, 3, _cli_history_up },
	{ { 0x1b, 0x5b, 0x42 }, 3, _cli_history_down },

	// null
	{ {0x00}, 0, 0x00}
};

/* ================================================================================ */


/**
 * @brief ping response
 */
static void fh_ping(void *a_ctx) {
	char resp[] = "pong\r\n";
	serial_poll_send((void *)resp, strlen(resp));
}

/**
 * @brief tomek to daun
 */
static void fh_tomek(void *a_ctx) {
	char resp[] = "Tomek to daun\r\n";
	serial_poll_send((void *)resp, strlen(resp));
}

/**
 * @brief get marque details
 */
static void fh_marque_get(void *a_ctx) {
	volatile t_marque *m = marque_get_ctx();
	char resp[16] = {0x00};

	memset(resp, 0x00, sizeof(resp));
	snprintf(resp, sizeof(resp), "Delay: %d\r\n", m->delay);
	serial_poll_send((void*)resp, strlen(resp));

	memset(resp, 0x00, sizeof(resp));
	snprintf(resp, sizeof(resp), "Head: %d\r\n", m->buff->head);
	serial_poll_send((void*)resp, strlen(resp));

	memset(resp, 0x00, sizeof(resp));
	snprintf(resp, sizeof(resp), "Current: %d\r\n", m->buff->current);
	serial_poll_send((void*)resp, strlen(resp));
}

/**
 * @brief increase marque scrolling delay
 */
static void fh_delay_inc(void *a_ctx) {
	volatile t_marque *m = marque_get_ctx();
	marque_set_delay(m->delay + 0x1000);
	serial_poll_send((void*)"OK", 2);
}

/**
 * @brief decrease marque scrolling delay
 */
static void fh_delay_dec(void *a_ctx) {
	volatile t_marque *m = marque_get_ctx();
	unsigned int x = m->delay - 0x1000;
	marque_set_delay(x > 0x00 ? x : 0x00);
	serial_poll_send((void*)"OK", 2);
}

static void fh_clear(void *a_ctx) {
	marque_clear();
	serial_poll_send((void*)"OK", 2);
	serial_flush();
}

static void fh_help(void *a_ctx) {
	t_cli_ctx *ctx = (t_cli_ctx *)a_ctx;
	unsigned char i = 0x00;	

	while (ctx->cmds[i].fh) {
		serial_poll_send("\r\n", 2);
		serial_poll_send(ctx->cmds[i].cmd, strlen(ctx->cmds[i].cmd));
		i++;
	}
}

/**
 * @brief get serial port informations
 */
static void fh_serial_get(void *a_ctx) {
	volatile t_buffer *sb = serial_get_rx_state();
	char resp[16] = {0x00};

	memset(resp, 0x00, sizeof(resp));
	snprintf(resp, sizeof(resp), "Head: %d\r\n", sb->head);
	serial_poll_send((void*)resp, strlen(resp));

	memset(resp, 0x00, sizeof(resp));
	snprintf(resp, sizeof(resp), "Tail: %d\r\n", sb->head);
	serial_poll_send((void*)resp, strlen(resp));

	memset(resp, 0x00, sizeof(resp));
	snprintf(resp, sizeof(resp), "RX Ok: %d\r\n", (unsigned int) sb->stats.ok);
	serial_poll_send((void*)resp, strlen(resp));
	
	memset(resp, 0x00, sizeof(resp));
	snprintf(resp, sizeof(resp), "RX Dropped: %d\r\n", (unsigned int) sb->stats.dropped);
	serial_poll_send((void*)resp, strlen(resp));

	memset(resp, 0x00, sizeof(resp));
	snprintf(resp, sizeof(resp), "RX FE: %d\r\n", (unsigned int) sb->stats.frame_error);
	serial_poll_send((void*)resp, strlen(resp));
}

static void fh_put(void *a_ctx __attribute__((unused))) {
	t_cli_ctx *ctx = (t_cli_ctx *)a_ctx;
	static unsigned char init = 0;

	if (!init) {
		char str[] = "Type text and it will occur in the marque, <ENTER> to quit\r\n";
		serial_poll_send(str, strlen(str));
		init = 1;
	}

	if (isprint(ctx->cmd[0])) {
		ctx->cmd[1] = '\0';
		marque_print(ctx->cmd);
		serial_poll_send(ctx->cmd, 1);
		return;
	}

	if (ctx->cmd[0] == 0x0d) {
		_cli_quit_owner(ctx);
		return;
	}
}

/* ================================================================================ */

static void _cli_quit_owner(t_cli_ctx *a_ctx) {
	_cli_prompt(a_ctx, 1);

	a_ctx->flags = 0x00;
	a_ctx->pos = 0x00;
	memset(a_ctx->cmd, 0x00, sizeof(a_ctx->cmd));
	serial_flush();
}

static void _cli_prompt(t_cli_ctx *a_ctx __attribute__((unused)), unsigned char nl) {
	if (nl) serial_poll_send("\r\n", 2);
	serial_poll_send("#> ", 3);
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

			// command will own the cli or not
			a_ctx->flags = a_ctx->cmds[i].flags;

			// the command owns interpreter and its idx is stored at pos
			if (a_ctx->flags & 0x01) a_ctx->pos = i;

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
				serial_poll_send(str, strlen(str));
				_cli_history_add(a_ctx);
			}
			break;

		case E_CMD_TOO_SHORT: {
				char str[] = "\r\nCommand too short";
				serial_poll_send(str, strlen(str));
			}
			break;

		default:
			_cli_history_add(a_ctx);
			break;
	} // switch
}

static void _cli_history_up(void* a_ctx) {

	t_cli_ctx *ctx = (t_cli_ctx *)a_ctx;
	char prompt[12] = {0x00};

	if ((ctx->hhead != ctx->htail) && 
			strlen(ctx->history[ctx->hpos])) {
		memset(ctx->cmd, 0x00, CLI_CMD_BUFFER_SIZE);
		
		strcpy(ctx->cmd, ctx->history[ctx->hpos]);
		ctx->pos = strlen(ctx->cmd);

		if (!ctx->hpos)
			ctx->hpos = CLI_CMD_HISTORY_LEN - 1;
		else
			ctx->hpos--;

		snprintf(prompt, sizeof(prompt), "\r\n(%d/%d)",
			   	ctx->hpos, CLI_CMD_HISTORY_LEN);
	
		serial_poll_send(prompt, strlen(prompt));
		serial_poll_send(ctx->cmd, strlen(ctx->cmd));
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
		ctx->pos = strlen(ctx->cmd);

		snprintf(prompt, sizeof(prompt), "\r\n(%d/%d)",
			   	ctx->hpos, CLI_CMD_HISTORY_LEN);
	
		serial_poll_send(prompt, strlen(prompt));
		serial_poll_send(ctx->cmd, strlen(ctx->cmd));
	}
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

/**
 * @brief initialize internals
 */
void cli_init() {
	memset(&g_cli_ctx, 0x00, sizeof(g_cli_ctx));
	g_cli_ctx.cmds = g_cmds;
	g_cli_ctx.mcmds = g_mchar_cmds;

	g_cli_ctx.hhead = 
		g_cli_ctx.htail = CLI_CMD_HISTORY_LEN;
}

/**
 * @brief processing function
 */
void cli_read() {
	unsigned char i = 0x00;
	unsigned char res = 0x00;

	if (!serial_getc(&i))
		return;

	// commands owning the interpretter
	if (g_cli_ctx.flags & 0x01) {
		g_cli_ctx.cmd[0] = i;
		g_cli_ctx.cmds[g_cli_ctx.pos].fh((void *)&g_cli_ctx);
		return;
	}

	/// multichar matching
	if (g_cli_ctx.mchar.pos && g_cli_ctx.mchar.pos < 3) {
		unsigned char mi = 0x00;

		g_cli_ctx.mchar.buf[g_cli_ctx.mchar.pos++] = i;

		while (g_cli_ctx.mcmds[mi].fh) {
			if (!memcmp(g_cli_ctx.mcmds[mi].pattern, 
						g_cli_ctx.mchar.buf,
						g_cli_ctx.mcmds[mi].len)) {
				g_cli_ctx.mcmds[mi].fh((void *)&g_cli_ctx);

				g_cli_ctx.mchar.pos = 0;
				memset(g_cli_ctx.mchar.buf, 0x00, 3);
				break;
			}
			mi++;
		}

		return;
	}

	/// char by char matching
	switch(i) {
		case 0x08: // backspace
		case 0x7f: // del
			if (g_cli_ctx.pos) {
				g_cli_ctx.cmd[g_cli_ctx.pos--] = '\0';
				serial_poll_send("\b \b", 3);
			}
			break;

		case 0x1b: // special characters
			g_cli_ctx.mchar.pos = 1;
			g_cli_ctx.mchar.buf[0x00] = i;
			break;

		case 0x0d: // new line
			g_cli_ctx.cmd[POSINC(g_cli_ctx.pos)] = '\0';

			serial_poll_send("\r\n", 2);

			res = _cli_interpret_cmd(&g_cli_ctx);
			_cli_reinterpret_cmd(&g_cli_ctx, res);			

			if (!g_cli_ctx.flags) {
				g_cli_ctx.pos = 0;
				memset(g_cli_ctx.cmd, 0x00, CLI_CMD_BUFFER_SIZE);
				_cli_prompt(&g_cli_ctx, 1);
			}
			break;

		default:
			/* echo */
			if (g_cli_ctx.pos < (CLI_CMD_BUFFER_SIZE - 1) && isprint(i)) {
				g_cli_ctx.cmd[g_cli_ctx.pos++] = i;
				serial_poll_send(&i, 1);
			}
			break;
	}

}
