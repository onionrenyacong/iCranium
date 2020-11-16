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
# include"setDevice.h"
# include<string.h>
# include<signal.h>
/*
#include <vector>
#include <iostream>
#include<stdio.h>
#include<fstream>
#include<sstream>
#include<math.h>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
*/
# define SIZE 1024

# define device_temperature "/dev/temperature"   //device temperature number
# define device_distance "/dev/distance"      //device distance number
#define device_GPS  "/dev/gps"	     //device GPS number
# define device_brain "/dev/brain"	     //device brain number
# define device_rgb_motor "/dev/arduino"     //device motor,rgb

# define flag_url 0

# define URL_temperature  "http://121.89.202.210:9999/Cranium_war/tempTrans"		     //temperature URL
# define URL_distance     "http://121.89.202.210:9999/Cranium_war/disTrans"		     //distance URL
# define URL_GPS   "http://121.89.202.210:9999/Cranium_war/GpsTrans"			     //GPS URL
# define URL_brain  "http://121.89.202.210:9999/Cranium_war/wavesTrans"			     //brain URL	
# define URL_camera ""                        //the url of the image

# define URL_temperature_test "http://192.168.3.72:8888/Cranium_war_exploded/tempTrans"
# define URL_distance_test "http://192.168.3.72:8888/Cranium_war_exploded/disTrans"
# define URL_GPS_test "http://192.168.3.72:8888/Cranium_war_exploded/GpsTrans"
# define URL_brain_test "http://192.168.3.72:8888/Cranium_war_exploded/wavesTrans"


unsigned char  buf_temperature[SIZE];	    
unsigned char  buf_distance[SIZE];	   
unsigned char  buf_brain[SIZE];		    
char	       buf_gps[SIZE];	        
int flag_temperature = 0;			
int flag_distance = 0;
int flag_gps = 0;
int flag_brain = 0;



struct dev_info {
	int flag;		//
	char* device;		//device number
	int speed;      	//
	char* device_controlled;//controlled  device
	int speed_controlled;
};

struct GPS {
	int flag;	
	char Lng[20];	// dddmm.mmm
	char Lat[20];	// ddmm.mmm
	char Lat_N_S;   
	char Lng_E_W;	
	int Lat_len;
	int Lng_len;
};



void get_info_temperature(void* ptr);
void get_info_distance(void* ptr);
void get_info_gps(void* ptr);
void get_info_brain(void* ptr);

static void handler_exit(int sig){
	unsigned char CLOSE_MOTOR_VIBRATE[]={0xAA,0xAE,0x00,0xAE,0xBB}; //close the motor
	unsigned char CLOSE_RGB_NORMAL[]={0xAA,0xAF,0x00,0x00,0x00,0x00,0x00,0x00,0xAF,0xBB};//close rgb
	unsigned char CLOSE_BEHIND[]={0xAA,0xAD,0x00,0x00,0xAD,0xBB};
	int fd_controlled=open(device_rgb_motor, O_RDWR | O_NOCTTY | O_SYNC);
        if(fd_controlled<0){
                  printf("error %d opening %s: %s\n", errno,device_rgb_motor, strerror(errno));
                  return;
         }
        set_interface_attribs(fd_controlled,B115200,0);
        set_blocking(fd_controlled,0);

	//send the require to motor and rgb
	write(fd_controlled,CLOSE_MOTOR_VIBRATE,sizeof CLOSE_MOTOR_VIBRATE);
        write(fd_controlled,CLOSE_RGB_NORMAL,sizeof CLOSE_RGB_NORMAL);
	write(fd_controlled,CLOSE_BEHIND,sizeof CLOSE_BEHIND);
	close(fd_controlled);	
	printf("enter handler_exit");
	exit(0);
}

void set_init(){
	int fd_controlled=open(device_rgb_motor, O_RDWR | O_NOCTTY | O_SYNC);
        if(fd_controlled<0){
                  printf("error %d opening %s: %s\n", errno,device_rgb_motor, strerror(errno));
                  return;
         }
        set_interface_attribs(fd_controlled,B115200,0);
        set_blocking(fd_controlled,0);
	unsigned char OPEN_RGB_GREEN[]={0xAA,0xAF,0x22,0x8B,0x22,0x22,0x8B,0x22,0x4D,0xBB};
	write(fd_controlled,OPEN_RGB_GREEN,sizeof OPEN_RGB_GREEN);
	close(fd_controlled);
	
}



int main() {
	//set the exit function
	signal(SIGINT,handler_exit);
	set_init();
	printf("out");
	void* retval;
	
	pthread_t thread_temperature, thread_distance, thread_gps,thread_brain;
	
	struct dev_info device1,device2, device3, device4;
	
        device1.device = device_temperature;
	device1.speed = B115200;
	device1.flag = 0;
	device1.device_controlled=device_rgb_motor;
	device1.speed_controlled=B115200;	

	device2.device = device_distance;
	device2.speed = B9600;
	device2.flag = 1;
	device2.device_controlled=device_rgb_motor;
        device2.speed_controlled=B115200;

	device3.device = device_GPS;
	device3.speed = B115200;
	device3.flag = 2;
	device3.device_controlled=device_rgb_motor;
 	device3.speed_controlled=B115200;	



	device4.device = device_brain;
	device4.speed = B57600;
	device4.flag = 3;
        device4.device_controlled=device_rgb_motor;
        device4.speed_controlled=B115200;
        
         
	struct dev_info* dev1 = &device1;
	struct dev_info* dev2 = &device2;
	struct dev_info* dev3 = &device3;
	struct dev_info* dev4 = &device4;

	//create the global HTTP
	CURLcode return_code = curl_global_init(CURL_GLOBAL_ALL);
	if (CURLE_OK != return_code) {
		printf("initlibcurl falied!\n");
		return -1;
	}

	//create Thread
	int ret_temperature = pthread_create(&thread_temperature, NULL, (void*)& get_info_temperature, (void*)dev1);
	int ret_distance = pthread_create(&thread_distance, NULL, (void*)& get_info_distance, (void*)dev2);
	int ret_gps = pthread_create(&thread_gps, NULL, (void*)& get_info_gps, (void*)dev3);
	int ret_brain = pthread_create(&thread_brain, NULL, (void*)& get_info_brain, (void*)dev4);

	if (ret_temperature != 0) {
		perror("open the thread temperature failed!\n");
		return -1;
	}
	else {
		printf("open the thread temperature success!\n");
	}

	if (ret_distance != 0) {
		perror("open the thread distance failed!\n");
		return -1;
	}
	else {
		printf("open the thread distance success!\n");
	}

	if (ret_gps != 0) {
		perror("open the thread gps failed!\n");
		return -1;
	}
	else {
		printf("open the thread gps success!\n");
	}

	if (ret_brain != 0) {
		perror("open the thread brain failed!\n");
		return -1;
	}
	else {
		printf("open the thread brain success!\n");
	}

	
	int tmp1 = pthread_join(thread_temperature, &retval);
	int tmp2 = pthread_join(thread_distance, &retval);
	int tmp3 = pthread_join(thread_gps, &retval);
	int tmp4 = pthread_join(thread_brain, &retval);


	if (tmp1 != 0 || tmp2 != 0 || tmp3 != 0||tmp4!=0) {
		perror("add the temperature thread " + (tmp1 == 0) ? "success!\n" : "failed!\n");
		perror("add the distance thread " + (tmp2 == 0) ? "success!\n" : "failed!\n");
		perror("add the gps thread " + (tmp3 == 0) ? "success!\n" : "failed!\n");
		perror("add the brain thread " + (tmp4 == 0) ? "success!\n" : "failed!\n");
	}
	else {
		printf("all have been joint!\n");
	}

	
	curl_global_cleanup();
}


/******************************temperature model *********************************************************/

//post temperature
int http_temperature_post(int n, char* URL,int fd_controlled) {
	printf("enter http_temperature_post\n");

	//get the value of temperature
	float temperature = -1;
	int i = 0;
	for (i = 0; i < (n - 3); i++) {
		if (buf_temperature[i] == 0xA5 && buf_temperature[i + 1] == 0x55) {
		//printf("temperature:%x %x %x %x\n", buf_temperature[i], buf_temperature[i + 1], buf_temperature[i + 2], buf_temperature[i + 3]);
			temperature = (buf_temperature[i + 2] + buf_temperature[i + 3] * 256) / 100.0;
			break;
		}
	}

	// printf("temperature:%f\n",temperature);

	//cJSON Object->string
	cJSON* tmp = cJSON_CreateObject();
	cJSON_AddNumberToObject(tmp, "temp", (float)temperature);
	char* out = cJSON_Print(tmp);
	printf("temperature:%s\n", out);
	cJSON_Delete(tmp);

	//send the post request
	int flag = call_url_post(URL, out, FLAG_TEMPERATURE,fd_controlled);
	if (flag == 1) {
		printf("temperature_http success!\n");
	}
	else {
		printf("temperature_http failed!\n");
	}
	return flag;
}

//the entry of the temperature thread
void get_info_temperature(void* ptr) {
	printf("enter get_info_temperature!\n");
	struct dev_info* dev = (struct dev_info*)ptr;
	int fd = open((*dev).device, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		printf("error %d opening %s: %s\n", errno, (*dev).device, strerror(errno));
		return;
	}
	set_interface_attribs(fd, (*dev).speed, 0);
	set_blocking(fd, 0);



	// the controlled device 
	int fd_controlled=open((*dev).device_controlled, O_RDWR | O_NOCTTY | O_SYNC);
	if(fd_controlled<0){
		 printf("error %d opening %s: %s\n", errno, (*dev).device_controlled, strerror(errno));
	         return;
	}
	 set_interface_attribs(fd_controlled,(*dev).speed_controlled,0);
         set_blocking(fd_controlled,0);

	//Loop not jump out 
	const unsigned char CMD_AUTO_NOREP[] = { 0xA5,0x55,0x03,0x02,0xFF };//the request of query the value of temperature to temperature model
	write(fd, CMD_AUTO_NOREP, sizeof CMD_AUTO_NOREP);
	int flag = 0;
	while (1) {
		flag_temperature++;
		//printf("%d\n", flag_temperature);
		int n = read(fd, buf_temperature, sizeof buf_temperature);
		sleep(1);
		flag = http_temperature_post(n, flag_url==0?URL_temperature_test:URL_temperature,fd_controlled);
	};
	close(fd);
	close(fd_controlled);
	/*int i=0;
	printf("%s:",(*dev).device);
	for(;i<n;i++){
	printf("%x ",buf[i]);
	}
	printf("\n");*/
}




/*********************************distance model**************************************************************/
//post distance
int http_distance_post(int n, char* URL,int fd_controlled) {
	printf("enter http_distance_post\ndistance:");
	int i = 0;
	for (i = 0; i < n; i++) {
		printf("%x ", buf_distance[i]);
	}
	printf("\n");
	unsigned int dis = -1;//mm

	//get the distance value
	for (i = 0; i < (n - 3); i++) {
		if (buf_distance[i] == 0xff) {
			unsigned char sum = (buf_distance[i] + buf_distance[i + 1] + buf_distance[i + 2]) & 0x00ff;
			if (sum == buf_distance[i + 3]) {//ะฃั้
				//printf("distance:%x %x %x %x\n", buf_distance[i], buf_distance[i + 1], buf_distance[i + 2], buf_distance[i + 3]);
				dis = buf_distance[i + 1] * 256 + buf_distance[i + 2];
				break;
			}
		}
	}
	// printf("distance:%d\n",dis);

	 //cJSON->string

	cJSON* tmp = cJSON_CreateObject();
	cJSON_AddNumberToObject(tmp, "distance", dis);
	char* out = cJSON_Print(tmp);
	printf("distance:%s\n", out);
	cJSON_Delete(tmp);

	//http post 
	int flag = call_url_post(URL, out, FLAG_DISTANCE,fd_controlled);
	if (flag == 1) {
		printf("distance_http success!\n");
	}
	else printf("distance_http failed!\n");
	return flag;
}


//the entry of distance thread
void get_info_distance(void* ptr) {
	struct dev_info* dev = (struct dev_info*)ptr;
	int fd = open((*dev).device, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		printf("error %d opening %s: %s\n", errno, (*dev).device, strerror(errno));
		return;
	}
	set_interface_attribs(fd, (*dev).speed, 0);
	set_blocking(fd, 0);


	// the controlled device
 	 int fd_controlled=open((*dev).device_controlled,O_RDWR|O_NOCTTY|O_SYNC);
         if(fd_controlled<0){
                 printf("error %d opening %s: %s\n", errno, (*dev).device_controlled, strerror(errno));
		 return;
	 }
 	 set_interface_attribs(fd_controlled,(*dev).speed_controlled,0);
	 set_blocking(fd_controlled,0);

	 int flag = 0;
	 while (1) {
		flag_distance++;
		printf("%d\n", flag_distance);
		int n = read(fd, buf_distance, sizeof buf_distance);
		sleep(1);
		flag = http_distance_post(n,flag_url==0?URL_distance_test:URL_distance,fd_controlled);
	};
	close(fd);
	close(fd_controlled);
	/*int i=0;
	printf("%s:",(*dev).device);
	for(;i<n;i++){
	   printf("%x ",buf[i]);
	}
	printf("\n");*/

}
/****************************GPS Model**********************************************************************/
//get the Lng and Lat and change to the GPS struct
struct GPS getLngLat(int n) {
	printf("enter getLngLat!\n");
	struct GPS ans;
	ans.Lat_len = 0;
	ans.Lng_len = 0;
	ans.flag = 1;
	if (n > 8 && buf_gps[0] == '$' && buf_gps[3] == 'G' && buf_gps[4] == 'L' && buf_gps[5] == 'L') {
		int i = 7;
		//get the info of lat
		while (i < n && buf_gps[i] != ',') {
			ans.Lat[ans.Lat_len++] = buf_gps[i];
			i++;
		}
		ans.Lat[ans.Lat_len] = '\0';
		if (i < n && (buf_gps[i + 1] == 'N' || buf_gps[i + 1] == 'S'))
			ans.Lat_N_S = buf_gps[i + 1];
		i = i + 3;
		//get the info of lng
		while (i < n && buf_gps[i] != ',') {
			ans.Lng[ans.Lng_len++] = buf_gps[i];
			i++;
		}
		ans.Lng[ans.Lng_len] = '\0';
		if (i < n && (buf_gps[i + 1] == 'E' || buf_gps[i + 1] == 'W'))
			ans.Lng_E_W = buf_gps[i + 1];
		if (ans.Lat_len == 0 && ans.Lng_len == 0)
			ans.flag = 0;
	}
	else {
		ans.flag = 0;
	}
	if (ans.flag == 0) {
		printf("the gps info is empty!\n");
	}
	return ans;
}

//gps struct to cJSON
cJSON* Gps2cJSON(struct GPS in) {
	printf("enter gps2cJSON!\n");
	cJSON* out = cJSON_CreateObject();
	cJSON_AddNumberToObject(out, "flag", in.flag);
	printf("1");
	cJSON_AddStringToObject(out, "Lng", (char*)in.Lng);
	printf("2");
	cJSON_AddStringToObject(out, "Lat", (char*)in.Lat);
	printf("3");
	cJSON_AddStringToObject(out, "Lat_N_S", &in.Lat_N_S);
	printf("4");
	cJSON_AddStringToObject(out, "Lng_E_W", &in.Lng_E_W);
	printf("5");
	cJSON_AddNumberToObject(out, "Lat_len", in.Lat_len);
	printf("6");
	cJSON_AddNumberToObject(out, "Lng_len", in.Lng_len);
	//printf("GPScJSON:%s\n",cJSON_Print(out));
	return out;
}


//post the gps request
int http_gps_post(int n, char* URL,int fd_controlled) {
	printf("enter http_gps_post\n");
	struct GPS ans = getLngLat(n);

	cJSON* ans_json = Gps2cJSON(ans);
	char* out = cJSON_Print(ans_json);
	cJSON_Delete(ans_json);

	int flag = call_url_post(URL,out,FLAG_GPS,fd_controlled);
	if (flag == 1) {
		printf("gps_http success!\n");
	}
	else {
		printf("gps_http failded!\n");
	}
	return flag;
}


//the entry of the gps thread   
void get_info_gps(void* ptr) {
	struct dev_info* dev = (struct dev_info*)ptr;
	int fd = open((*dev).device, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		printf("error %d opening %s: %s\n", errno, (*dev).device, strerror(errno));
		return;
	}
	set_interface_attribs(fd, (*dev).speed, 0);
	set_blocking(fd, 0);

	// the controlled device
	int fd_controlled=open((*dev).device_controlled,O_RDWR|O_NOCTTY|O_SYNC);	
	if(fd_controlled<0){
		 printf("error %d opening %s: %s\n", errno, (*dev).device_controlled, strerror(errno));
		 return;
	}
	set_interface_attribs(fd_controlled,(*dev).speed_controlled,0);
	set_blocking(fd_controlled,0);




	printf("now in get_info_gps!\n");
	int flag = 0;
	while (1) {
		flag_gps++;
		printf("%d\n", flag_gps);
		int n = read(fd, buf_gps, sizeof buf_gps);
		//eep(1);
		flag = http_gps_post(n,flag_url==0?URL_GPS_test:URL_GPS,fd_controlled);
		/*int i=0;
		printf("%s:",(*dev).device);
		for(;i<n;i++){
		   printf("%x ",buf[i]);
		}
		printf("\n");*/
	}
	close(fd);
	close(fd_controlled);
}

/************************************brain model*****************************************************************/



//post brain
int http_brain_post(int brain_result, char* URL,int fd_controlled) {
	printf("enter http_brain_post\n");

	//cJSON->string
	cJSON* tmp = cJSON_CreateObject();
	cJSON_AddNumberToObject(tmp, "brain", brain_result);
	char* out = cJSON_Print(tmp);
	printf("brain:%s\n", out);
	cJSON_Delete(tmp);


	//post request
	int flag = call_url_post(URL, out, FLAG_BRAIN,fd_controlled);
	if (flag == 1) {
		printf("brain_http success!\n");
	}
	else printf("brain_http failed!\n");
	return flag;

}





//the entry of brain thread
void get_info_brain(void* ptr) {
	struct dev_info* dev = (struct dev_info*)ptr;
	int fd = open((*dev).device, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		printf("error %d opening %s: %s\n", errno, (*dev).device, strerror(errno));
		return;
	}
	set_interface_attribs(fd, (*dev).speed, 0);
	set_blocking(fd, 0);
	printf("now in get_info_brain!\n");
	
	// the controlled device
	int fd_controlled=open((*dev).device_controlled, O_RDWR | O_NOCTTY | O_SYNC);
	if(fd_controlled<0){
		    printf("error %d opening %s: %s\n", errno, (*dev).device_controlled, strerror(errno));
		    return;
	}
	set_interface_attribs(fd_controlled,(*dev).speed_controlled,0);
	set_blocking(fd_controlled,0);

        int a[100];
        int an=0;	
	int flag = 0;
	while (1) {
		flag_brain++;
		printf("%d\n", flag_brain);

		//get the value of brain
		memset(buf_brain, 0, sizeof(buf_brain));
		int n = read (fd, buf_brain, sizeof buf_brain);  // read up to 100 characters if ready to read
		int brain_result=-1;
		int i,mi,fi,FRAME_LEN=36;
		for(i=0;i<(n-5);i++){
			if(buf_brain[i]==0xAA&&buf_brain[i+1]==0xAA&&buf_brain[i+2]==0x20&&buf_brain[i+3]==0x02){			
			  
				int remain = FRAME_LEN-(n-i);
				unsigned char * pcur = buf_brain;
				while(remain>0 && buf_brain+SIZE-pcur > 0)
				{
					
					pcur += n;
					n = read(fd, pcur, buf_brain+SIZE-pcur);
					remain = remain - n;
					//printf("2:    %.2x\n", buf[i+32]);
											

				}
				
				int j;
				for(j=0;j<FRAME_LEN;j++) printf("%.2x ", buf_brain[i+j]);
				brain_result=buf_brain[i+32];
				printf("attention: %d",brain_result);
			}
				
		}
		sleep(1);
		flag = http_brain_post(brain_result,flag_url==0?URL_brain_test:URL_brain,fd_controlled);
		
	}
	close(fd);	
	close(fd_controlled);

}




//*********************************************************camera model*******************************************

