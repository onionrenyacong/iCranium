# include<stdio.h>
# include<stdlib.h>
# include<pthread.h>
# include<string.h>
# include<errno.h>
# include<fcntl.h>
# include<unistd.h> 
# include<termios.h>
# include "setDevice.h"
# define device_rgb "/dev/arduino"
# define device_motor "/dev/arduino"
unsigned char OPEN_RGB_RED[]={0xAA,0xAF,0xFF,0x00,0x00,0xFF,0x00,0x00,0xAD,0xBB};     //set the rgb color
unsigned char OPEN_RGB_pur[]={0xAA,0xAF,0xDC,0x14,0x3C,0xDC,0x14,0x3C,0x07,0xBB};
unsigned char OPEN_RGB_YELLOW[]={0xAA,0xAF,0xFF,0xD7,0x00,0xFF,0xD7,0x00,0x5B,0xBB};
unsigned char CLOSE_RGB_NORMAL[]={0xAA,0xAF,0x00,0x00,0x00,0x00,0x00,0x00,0xAF,0xBB};
unsigned char OPEN_RGB_GREEN[]={0xAA,0xAF,0x22,0x8B,0x22,0x22,0x8B,0x22,0x4D,0xBB};//
unsigned char OPEN_RGB_RIGHT_GREEN[]={0xAA,0xAF,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xAC,0xBB};//
unsigned char OPEN_RGB_LEFT_GREEN[]={0xAA,0xAF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xAC,0xBB};//

unsigned char OPEN_MOTOR_VIBRATE[]={0xAA,0xAE,0x01,0xAF,0xBB}; //open the motor
unsigned char CLOSE_MOTOR_VIBRATE[]={0xAA,0xAE,0x00,0xAE,0xBB}; //close the motor

//Behind rgb
unsigned char CLOSE_BEHIND[]={0xAA,0xAD,0x00,0x00,0xAD,0xBB};
unsigned char OPEN_LEFT_BEHIND[]={0xAA,0xAD,0x01,0x00,0xAE,0xBB};
unsigned char OPEN_RIGHT_BEHIND[]={0xAA,0xAD,0x00,0x01,0xAE,0xBB};

int main(){
	
	int fd_rgb=open(device_rgb,O_RDWR | O_NOCTTY | O_SYNC);
	
	int fd_motor=open(device_motor,O_RDWR | O_NOCTTY | O_SYNC);
	if(fd_rgb<0){
		printf("error %d opening %s: %s\n", errno, device_rgb, strerror(errno));
          	return 1;
	}
	if(fd_motor<0){
		  printf("error %d opening %s: %s\n", errno, device_motor, strerror(errno));                                                                          return 1;
	 }
	 set_interface_attribs(fd_rgb,B115200,0);
	 set_blocking(fd_rgb,0);
	 
	 set_interface_attribs(fd_motor,B115200,0);
	 set_blocking(fd_motor,0);
	 
				 
	/*int i=1;
	for( i=1;i<8;i++){
		[8]+=SET_RGB_COLOR[i];	
 	}
	SET_RGB_COLOR[8]&=0xFF;
	*/
	/*for( i=1;i<3;i++){
		OPEN_MOTOR_VIBRATE[3]+=OPEN_MOTOR_VIBRATE[i];
	}
	OPEN_MOTOR_VIBRATE[3]&=0xFF;
	
	for(i=1;i<3;i++){
		CLOSE_MOTOR_VIBRATE[3]+=CLOSE_MOTOR_VIBRATE[i];
	}
	CLOSE_MOTOR_VIBRATE[3]&=0xFF;	
	*/
	write(fd_rgb,OPEN_RGB_pur,sizeof OPEN_RGB_RED );
	//write(fd_motor,OPEN_MOTOR_VIBRATE,sizeof OPEN_MOTOR_VIBRATE);
         system("aplay ../music/danger.wav -D plughw:1,0"); 	
	//sleep(1);
	//write(fd_motor,CLOSE_MOTOR_VIBRATE,sizeof CLOSE_MOTOR_VIBRATE);
	write(fd_rgb,OPEN_RGB_GREEN,sizeof OPEN_RGB_GREEN);

	close(fd_rgb);
	close(fd_motor);
}
