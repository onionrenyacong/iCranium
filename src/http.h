#ifndef _http__
#define _http__

#include<stdio.h>

#define FLAG_TEMPERATURE 0
#define FLAG_DISTANCE 1
#define FLAG_GPS 2
#define FLAG_BRAIN 3
// define the call back function
size_t recv_data_temperature(void* buffer,size_t size,size_t nmemb,FILE* file);
size_t recv_data_distance(void* buffer,size_t size,size_t nmemb,FILE* file);
size_t recv_data_gps(void* buffer,size_t size,size_t nmemb,FILE* file);
size_t recv_data_brain(void* buffer,size_t size,size_t nmemb,FILE* file);
size_t recv_data_camera(void* buffer, size_t size, size_t nmemb, FILE* file);

//define the url_post function
int call_url_post(char * url,char * strJSON,int flag,int fd);
int call_image_post(char* url,char* filename,int fd);

#endif
