# include<stdio.h>
#include <unistd.h>
int main(){
	system("aplay test.mp3 -D plughw:1,0");

}
