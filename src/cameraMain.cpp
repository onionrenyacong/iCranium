#include <iostream>
#include <opencv2/opencv.hpp>
# include<stdio.h>
# include<vector>
#include<fstream>
#include<sstream>
#include<math.h>
# include<curl/curl.h>
# include<stdlib.h>
# include<fcntl.h>
# include<unistd.h>
using namespace std;
using namespace cv; 


#define FLAG_URL 0 //0 test  1 表示公网
#define URL_POST "http://121.89.202.210:9999/Cranium_war/savePhoto"
#define URL_POST_test "http://192.168.3.72:8888/Cranium_war_exploded/savePhoto" 
#define FLAG_CAMERA 0 //0 color 1 red
#define URL_camera_color "http://192.168.10.1:8090/?action=stream"
#define URL_camera_red "http://192.168.10.1:8080/?action=stream"

//call function
size_t recv_data_camera(void* buffer, size_t size, size_t nmemb, FILE* file) {
	printf("rcv_data:%s\n",buffer);
	//0 stand for danger; 1 stand for customer
	char * tmp=(char*)buffer;
	printf("tmp:%s\n",tmp);
	//sleep(10);
	if(tmp[0]=='0'){
		system("aplay ../music/danger.wav -D plughw:1,0");
	}
	if(tmp[0]=='1'){
		printf("enter 1\n");
		system("aplay ../music/customer.wav -D plughw:1,0");
	}
}


// the post require of the image
int call_image_post(char* url,char* filename){
	printf("enter call_image_post!\n");
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
		printf("%d\n",nRet);
	}
	
	curl_formfree(FormPost);//free the form
	curl_easy_cleanup(CurlHandle);//free the CURL
	return nRet;

}


//send the post require
void http_post_camera(Mat myFace,int pic_num){
	printf("enter http_camera_post\n");
	char filename[20];
	sprintf(filename,"%d.jpg",pic_num);
	//first store the image in local
	imwrite(filename,myFace);
	//send the post require
	int flag=call_image_post(FLAG_URL==0?URL_POST_test:URL_POST, filename);	
	remove(filename);//delete the file
}	
				





//function:load the image from URL
void get_info_camera(void* ptr){

	CascadeClassifier cascada;
	cascada.load("haarcascade_frontalface_alt2.xml");
	VideoCapture cap(FLAG_CAMERA==0?URL_camera_color:URL_camera_red);//get the image from URL
	if (!cap.isOpened()) {
		printf("don't open the camera !\n");
		return;
	}
	Mat frame, myFace;
	//deep loop to send the post require
	int pic_num = 0;
	while(1){
		cap >> frame;//input the url image
		vector<Rect> faces;//the location of faces in the url image
		Mat frame_gray; //the gray image
		frame_gray=frame;
		//cvtColor(frame, frame_gray, COLOR_BGR2GRAY);//BGR 2 GRAY 
		//return faces(vector) include four number meaning the left top location and the right bottom 
		cascada.detectMultiScale(frame_gray, faces, 1.1, 3, CASCADE_DO_ROUGH_SEARCH, Size(70, 70), Size(1000, 100));				
		printf("pic_number:%d   the number of the detected faces :%d\n", pic_num,faces.size());
		for (int i = 0; i < faces.size(); i++) {
				pic_num++;
				Mat faceROI = frame_gray(faces[0]);//cut the specific region in the image
				resize(faceROI, myFace, Size(92, 112));
				sleep(1);			
				http_post_camera(myFace,pic_num);
																			
		}
	}
}

int main(){

	get_info_camera(NULL);

}
