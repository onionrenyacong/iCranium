 
# include<stdio.h>
# include<stdlib.h>
# include<pthread.h>
# include<string.h>
# include<errno.h>
# include<fcntl.h>
# include<termios.h>
# include"cJSON.h"
# include<unistd.h>
# include<curl/curl.h>
# include"http.h"

# define FLAG_OPEN_MOTOR 0
# define FLAG_CLOSE_MOTOR 1
# define FLAG_OPEN_RED 2
# define FLAG_OPEN_YELLOW 3
# define FLAG_CLOSE_RGB 4
# define FLAG_OPEN_GREEN 5
# define FLAG_OPEN_RIGHT_GREEN 6
# define FLAG_OPEN_LEFT_GREEN 7
# define FLAG_OPEN_LEFT_BEHIND 8
# define FLAG_OPEN_RIGHT_BEHIND 9
# define FLAG_CLOSE_BEHIND 10
unsigned char OPEN_RGB_RED[]={0xAA,0xAF,0xDC,0x14,0x3C,0xDC,0x14,0x3C,0x07,0xBB};     //set the rgb color
unsigned char OPEN_RGB_YELLOW[]={0xAA,0xAF,0xFF,0xD7,0x00,0xFF,0xD7,0x00,0x5B,0xBB};
unsigned char CLOSE_RGB_NORMAL[]={0xAA,0xAF,0x00,0x00,0x00,0x00,0x00,0x00,0xAF,0xBB};
unsigned char OPEN_RGB_GREEN[]={0xAA,0xAF,0x22,0x8B,0x22,0x22,0x8B,0x22,0x4D,0xBB};//¿¿¿
unsigned char OPEN_RGB_RIGHT_GREEN[]={0xAA,0xAF,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xAC,0xBB};//¿¿¿
unsigned char OPEN_RGB_LEFT_GREEN[]={0xAA,0xAF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xAC,0xBB};//¿¿¿

unsigned char OPEN_MOTOR_VIBRATE[]={0xAA,0xAE,0x01,0xAF,0xBB}; //open the motor
unsigned char CLOSE_MOTOR_VIBRATE[]={0xAA,0xAE,0x00,0xAE,0xBB}; //close the motor

//Behind rgb
unsigned char CLOSE_BEHIND[]={0xAA,0xAD,0x00,0x00,0xAD,0xBB};
unsigned char OPEN_LEFT_BEHIND[]={0xAA,0xAD,0x01,0x00,0xAE,0xBB};
unsigned char OPEN_RIGHT_BEHIND[]={0xAA,0xAD,0x00,0x01,0xAE,0xBB};



int fd_controlled;
//*****************************************controll device**********************************************************************
void set_device_controlled(int fd,int flag){	
	printf("enter set_device_controlled\n");
	switch(flag){
		case 0: write(fd_controlled,OPEN_MOTOR_VIBRATE,sizeof OPEN_MOTOR_VIBRATE);break;
		case 1: write(fd_controlled,CLOSE_MOTOR_VIBRATE,sizeof CLOSE_MOTOR_VIBRATE);break;
		case 2: write(fd_controlled,OPEN_RGB_RED,sizeof OPEN_RGB_RED);break;
		case 3: write(fd_controlled,OPEN_RGB_YELLOW,sizeof OPEN_RGB_YELLOW);break;
		case 4: write(fd_controlled,CLOSE_RGB_NORMAL,sizeof CLOSE_RGB_NORMAL);break;
		case 5:write(fd_controlled,OPEN_RGB_GREEN,sizeof OPEN_RGB_GREEN);break;
		case 6:write(fd_controlled,OPEN_RGB_RIGHT_GREEN,sizeof OPEN_RGB_RIGHT_GREEN);break;
		case 7:write(fd_controlled,OPEN_RGB_LEFT_GREEN,sizeof OPEN_RGB_LEFT_GREEN);break; 
		case 8:write(fd_controlled,CLOSE_BEHIND,sizeof CLOSE_BEHIND);break;
		case 9:write(fd_controlled,OPEN_LEFT_BEHIND,sizeof OPEN_LEFT_BEHIND);break;
		case 10:write(fd_controlled,OPEN_RIGHT_BEHIND,sizeof OPEN_RIGHT_BEHIND);break;
	}
}

/************************************receive part***************************************************************/

//http receive function 
size_t recv_data_temperature(void* buffer, size_t size, size_t nmemb, FILE* file){
	//buffer only two values == '0' '1'  
	printf("rec_data_temperature:%s\n",buffer);
 	
	char* tmp=(char*)buffer;
	//1 stand for high temperature
	if((tmp[0]-'0')==1){
	//	system("aplay ../music/temperature.wav -D plughw:1,0");
		set_device_controlled(fd_controlled,FLAG_OPEN_MOTOR);
                set_device_controlled(fd_controlled,FLAG_OPEN_RED);
		//play wav file
		system("aplay ../music/temperature.wav -D plughw:1,0");
                sleep(3);
                set_device_controlled(fd_controlled,FLAG_CLOSE_MOTOR);
                set_device_controlled(fd_controlled,FLAG_OPEN_GREEN);
	}
        if((tmp[0]-'0')==0){
		set_device_controlled(fd_controlled,FLAG_OPEN_GREEN);
		set_device_controlled(fd_controlled,FLAG_CLOSE_MOTOR);
	}	
}

size_t recv_data_distance(void* buffer, size_t size, size_t nmemb, FILE* file){
	//buffer 1(below 1 meter)¿2(below 2 meter)¿3(below 3 meter)¿4(below 4meter)¿5(below 5meter)
	printf("rec_data_distance:%s\n",buffer);
		
	char * tmp=(char*) buffer;
        if((tmp[0]-'0')==1){
                set_device_controlled(fd_controlled,FLAG_OPEN_MOTOR);
                set_device_controlled(fd_controlled,FLAG_OPEN_RED);
		//play wav file
		system("aplay ../music/distance.wav -D plughw:1,0");
                sleep(1);
                set_device_controlled(fd_controlled,FLAG_CLOSE_MOTOR);
                set_device_controlled(fd_controlled,FLAG_OPEN_GREEN);
        }
        if((tmp[0]-'0')==2){
                set_device_controlled(fd_controlled,FLAG_OPEN_YELLOW);
                sleep(1);
                set_device_controlled(fd_controlled,FLAG_OPEN_GREEN);
        }
	 if((tmp[0]-'0')==0){                                                                      set_device_controlled(fd_controlled,FLAG_OPEN_GREEN);                             set_device_controlled(fd_controlled,FLAG_CLOSE_MOTOR);                    }	
}

size_t recv_data_gps(void* buffer, size_t size, size_t nmemb, FILE* file){	
	printf("rec_data_gps:%s\n",buffer);
	//0  1:left 2:right	
	char * tmp=(char*) buffer;
	//right
	if((tmp[0]-'0')==2){
		set_device_controlled(fd_controlled,FLAG_OPEN_RIGHT_GREEN);
		set_device_controlled(fd_controlled,FLAG_OPEN_LEFT_BEHIND);
		system("aplay ../music/turn_right.wav -D plughw:1,0");
		sleep(1);
		set_device_controlled(fd_controlled,FLAG_OPEN_GREEN);
		set_device_controlled(fd_controlled,FLAG_CLOSE_BEHIND);		
	}
	//left
	if((tmp[0]-'0')==1){
		set_device_controlled(fd_controlled,FLAG_OPEN_LEFT_GREEN);
		set_device_controlled(fd_controlled,FLAG_OPEN_RIGHT_BEHIND);//¿¿¿
		system("aplay ../music/turn_left.wav -D plughw:1,0");
                sleep(1);
		set_device_controlled(fd_controlled,FLAG_CLOSE_BEHIND); 
                set_device_controlled(fd_controlled,FLAG_OPEN_GREEN);
	}
	 if((tmp[0]-'0')==0){                                                                      set_device_controlled(fd_controlled,FLAG_OPEN_GREEN);                             set_device_controlled(fd_controlled,FLAG_CLOSE_MOTOR);                    }
	
}

size_t recv_data_brain(void* buffer, size_t size, size_t nmemb, FILE* file){
	//0 ¿¿¿¿¿  1¿¿¿¿ 8888 (¿¿¿)
	printf("rec_data_brain:%s\n",buffer);
	char * tmp=(char*)buffer;
	if((tmp[0]-'0')==1){
		set_device_controlled(fd_controlled,FLAG_OPEN_MOTOR);
		system("aplay ../music/attention.wav -D plughw:1,0"); 		
		sleep(3);
 		set_device_controlled(fd_controlled,FLAG_CLOSE_MOTOR);
	}
	 if((tmp[0]-'0')==0){                                                                      set_device_controlled(fd_controlled,FLAG_OPEN_GREEN);                             set_device_controlled(fd_controlled,FLAG_CLOSE_MOTOR);                    }
}

size_t recv_data_camera(void* buffer, size_t size, size_t nmemb, FILE* file) {
	printf("recv_data_camera:%s\n", buffer);

}

/*************************************send part*******************************************************/

int call_url_post(char* url, char* strJSON,int flag,int fd) {
	fd_controlled=fd;
	
	printf("%s\n", url);
	CURL* easy_handle; //µ÷ÓÃcurlµÄ½Ó¿Ú¶¼ÐèÒªÓÃµ½easy_handle
	easy_handle = curl_easy_init();//easy_handle³õÊ¼»¯
	if (NULL == easy_handle) {
		printf("get a easy handle failded.\n");
		//curl_global_cleanup();//Îö¹¹curl
		return -1;
	}

	//ÉèÖÃpost·¢ËÍµÄÍ·ÒÔ¼°ÊµÌå
	curl_easy_setopt(easy_handle, CURLOPT_URL, url);//ÉèÖÃurl
	struct curl_slist* plist = NULL;//ÉèÖÃÍ·ÏûÏ¢
	plist = curl_slist_append(plist, "Content-Type:application/json");
	curl_easy_setopt(easy_handle, CURLOPT_HTTPHEADER, plist);
	printf("The json:%s\n", strJSON);
	curl_easy_setopt(easy_handle, CURLOPT_POSTFIELDS, strJSON);//ÉèÖÃÊµÌåÐÅÏ¢

	switch (flag) {
		case FLAG_TEMPERATURE:
			curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, recv_data_temperature);//ÉèÖÃ»Øµ÷º¯Êý
			printf("enter call_url_post_temperature\n");
			break;
		case FLAG_DISTANCE:
			curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, recv_data_distance);//ÉèÖÃ»Øµ÷º¯Êý
			printf("enter call_url_post_distance\n");
			break;
		case FLAG_GPS:
			curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, recv_data_gps);//ÉèÖÃ»Øµ÷º¯Êý
			printf("enter call_url_post_GPS\n");
			break;
		case FLAG_BRAIN:
			curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, recv_data_brain);//ÉèÖÃ»Øµ÷º¯Êý
			printf("enter call_url_post_brain\n");
			break;
	}

	curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, NULL);//ÉèÖÃrecv_dataµÄµÚËÄ¸ö²ÎÊý
	//easy_handle¿ªÆô
	curl_easy_perform(easy_handle);

	//Îö¹¹easy_handle
	curl_easy_cleanup(easy_handle);
	//curl_global_cleanup();
	return 1;
}

// the post require of the image
int call_image_post(char* url,char* filename,int fd) {
	fd_controlled=fd;
	struct curl_httppost* FormPost = 0;
	struct curl_httppost* LastPost = 0;
	
	CURL* CurlHandle = curl_easy_init(); //CURL init
	//set the header
	curl_easy_setopt(CurlHandle, CURLOPT_CUSTOMREQUEST, "POST");  //set the post way
	curl_easy_setopt(CurlHandle, CURLOPT_URL, url);   //set the url
	curl_easy_setopt(CurlHandle, CURLOPT_WRITEFUNCTION,recv_data_camera);//set the callback function
	curl_easy_setopt(CurlHandle, CURLOPT_WRITEDATA,NULL);
	curl_formadd(&FormPost, &LastPost, CURLFORM_COPYNAME, "file", CURLFORM_FILE,filename, CURLFORM_END); 
	curl_easy_setopt(CurlHandle, CURLOPT_HTTPPOST, FormPost);

	CURLcode nRet = curl_easy_perform(CurlHandle); //perform the http
	if (nRet == CURLE_OK) {
		printf("perform successful!\n");
	}
	else {
		printf("perform failed!\n");
	}
	
	curl_formfree(FormPost);//free the form
	curl_easy_cleanup(CurlHandle);//free the CURL
	return nRet;
}









