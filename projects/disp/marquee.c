#include "marquee.h"
#include <string.h>

/// data buffer
static unsigned char g_marque_buffer[BUFFER_SIZE + BUFFER_PADDING] = {0x00};

/// position in the buffer
static unsigned int g_head = 0x00;


/**
 * @brief clear the buffer
 */
void marque_clear() {
	memset(g_marque_buffer, 0x00, sizeof(g_marque_buffer));
	g_head = 0x00;
}


void marque_print(const char* a_str)
{

}


void marque_scroll() {

}
