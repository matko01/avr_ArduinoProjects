#include "cli.h"
#include "pca.h"

#include <stdio.h>
#include <string.h>


static void fh_hw(void *a_data);
static void fh_hi(void *a_data);
static void fh_argc(void *a_data);


static t_cmd g_cmds[] = {

	{ "hw", fh_hw },
	{ "hi", fh_hi },
	{ "argc", fh_argc },

	// null
	{ 0x00, 0x00 }
};



static void fh_hw(void *a_data) {
	CLI_IO_OUTPUT("hello_world", 11);
}

static void fh_hi(void *a_data) {
	CLI_IO_OUTPUT("hi", 2);
}
		
static void fh_argc(void *a_data) {
	char tmp[16] = { 0x00 };
	t_cli_ctx *ctx = (t_cli_ctx *)a_data;
	sprintf(tmp, "argc: %d", ctx->argc);
	CLI_IO_OUTPUT(tmp, strlen(tmp));
}


int main(int argc, char const *argv[]) {

	serial_init(E_BAUD_9600);	
	serial_install_interrupts(E_FLAGS_SERIAL_RX_INTERRUPT);
	serial_flush();
	serial_install_stdio();

	t_cli_ctx cli_ctx;
	cli_init(&cli_ctx, g_cmds);
	while (1) {
		cli_read(&cli_ctx);
	}
	return 0;
}
