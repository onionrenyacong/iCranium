#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/soundcard.h>
#include <alas/asoundlib.h>

snd_pcm_t *handle;
snd_pcm_hw_params_t *params;
snd_pcm_uframes_t frames=1024;//一个周期多少帧
snd_pcm_access_t access_mode=SND_PCM_ACCESS_RW_INTERLEAVED;//访问模式:交错访问
snd_pcm_format_t format=SND_PCM_FORMAT_S16_LE;//样本长度:16位  小端存储

int channel=2;
unsigned int simple_rate=44100;//采样率
char * buffer=NULL;//缓冲区数据

int main(){
  //打开PCM设备(实现播放和录音)
  int rc=snd_pcm_open(&handle,"default",SND_PCM_STREAM_PLAYBACK, 0);//此处选择播放
   if (rc < 0) {
        printf("open device failed\n");
        return 0;
   }
   
 /*分配一个硬件参数对象*/
  snd_pcm_hw_params_alloca(&params);
  snd_pcm_hw_params_any(handle,params);//使用默认值填充参数对象

  //设置硬件参数
  snd_pcm_hw_params_set_access(handle, params,access_mode);//交错模式 
  snd_pcm_hw_params_set_format(handle, params,format);//采样位数
  snd_pcm_hw_params_set_channels(handle, params, channel);//通道数
  snd_pcm_hw_params_set_rate_near(handle, params,&simple_rate, NULL);//采样率
  snd_pcm_hw_params_set_period_size_near(handle,params, &frames, NULL);//设置一个周期的帧
  rc = snd_pcm_hw_params(handle, params);
  if(rc<0){
	printf("unable to set hw parameters: %s\n",snd_strerror(rc));
        return 0;
  }
	 
}
