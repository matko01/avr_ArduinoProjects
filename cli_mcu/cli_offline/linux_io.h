#ifndef __LINUX_IO_H__
#define __LINUX_IO_H__

void cm_off(void);
void cm_on(void);
unsigned char linux_getc(unsigned char *a_data);
unsigned char linux_putc(unsigned char *data, unsigned char a_len);


#endif /* __LINUX_IO_H__ */
