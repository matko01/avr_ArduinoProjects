#ifndef __CLI_H__
#define __CLI_H__

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

#include "config.h"

#define CLI_CMD_BUFFER_SIZE 16
#define CLI_CMD_HISTORY_LEN 4
#define CLI_CMD_KEYWORD_MAXLEN 16

#define POSINC(__x) (((__x) < (CLI_CMD_BUFFER_SIZE - 1)) ? (__x + 1) : (__x))
#define POSDEC(__x) ((__x) ? ((__x) - 1) : 0)

/**
 * @brief command definition
 */
typedef struct _t_cmd {
	char cmd[CLI_CMD_KEYWORD_MAXLEN];
	void (*fh)(void*);
	unsigned char flags;
} t_cmd;


typedef enum _t_cmd_status {
	E_CMD_OK = 0,
	E_CMD_NOT_FOUND,
	E_CMD_TOO_SHORT,
	E_CMD_EMPTY
} t_cmd_status;


/**
 * @brief multichar context (for characters producing more than one keycode)
 */
typedef struct _t_multichar_ctx {
	unsigned char buf[3];
	unsigned char pos;
} t_multichar_ctx;


/**
 * @brief multichar keycodes
 */
typedef struct _t_multichar_cmd {
	unsigned char pattern[3];
	unsigned char len;
	void (*fh)(void*);
} t_multichar_cmd; 


/**
 * @brief cli context
 */
typedef struct _t_cli_ctx {
	// command set
	t_cmd *cmds;
	t_multichar_cmd *mcmds;

	// cmd
	char cmd[CLI_CMD_BUFFER_SIZE];
	unsigned char pos;

	// allow commands to take over the interpreting
	unsigned char flags;

	// history
	char history[CLI_CMD_HISTORY_LEN][CLI_CMD_BUFFER_SIZE];
	unsigned char hpos;
	unsigned char hhead, htail;

	// multichar status
	t_multichar_ctx mchar;
} t_cli_ctx;




/**
 * @brief initialize internal stuff
 */
void cli_init();

/**
 * @brief read and interpret data
 */
void cli_read();


#endif /* __CLI_H__ */
