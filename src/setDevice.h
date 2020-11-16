#ifndef __setDevice__
#define __setDevice__
# include<stdio.h>
void set_blocking(int fd, int should_block);
int set_interface_attribs(int fd, int speed, int parity);


#endif
