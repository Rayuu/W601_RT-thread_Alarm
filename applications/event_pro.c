#include "event_pro.h"

#include <rtthread.h>
#include <rtdevice.h>
#include <rtdbg.h>
#include "board.h"

#include "sensor_pro.h"
#include "infrared_pro.h"

//红外控制灯的开关
void task_auto_open_led_pro(void)
{
	struct infrared_decoder_data sendvalue;
	if(myhome_sensor.distance >= OPEN_THRESHOLD_AD_VAL)
	{
		//发送红外数据
		sendvalue.data.nec.addr = 0;
		sendvalue.data.nec.key = myhome_sensor.infrared_recv[0];
		sendvalue.data.nec.repeat = 0;
		infrared_write("nec",&sendvalue);
		rt_thread_mdelay(200);
	}

}


//推送消息到微信
void task_pushdata_to_wechat_pro(void)
{
		time_t now,end_time;
		static time_t start_time;
		now = time(RT_NULL);
		//红外编码
		struct infrared_decoder_data sendvalue;
		myhome_sensor.date = localtime(&now);
		//到达设定的时间，推送温湿度消息到微信，打开红外灯，打开蜂鸣器
		if((myhome_sensor.date->tm_hour == ALARM_HOURS) && (myhome_sensor.date->tm_min == ALARM_MINUTES) &&\
				(myhome_sensor.date->tm_sec == ALARM_SECONDS))
		{
			start_time = time(RT_NULL);
			//往微信推送消息
			extern int mbedtls_client_start(void);
			mbedtls_client_start();
			//打开蜂鸣器
			rt_pin_write(PIN_BEEP,PIN_HIGH);
			//发送红外数据，开灯
			sendvalue.data.nec.addr = 0;
			sendvalue.data.nec.key = myhome_sensor.infrared_recv[0];
			sendvalue.data.nec.repeat = 0;
			infrared_write("nec",&sendvalue);
			rt_thread_mdelay(200);
			
		}
		end_time = time(RT_NULL);
		if((end_time - start_time) == ALARM_DURATION)
		{
			//关闭蜂鸣器
			rt_pin_write(PIN_BEEP,PIN_LOW);
		}
}

//事件处理线程句柄
rt_thread_t event_pro_thread = RT_NULL;
//线程入口
void event_pro_thread_entry(void *parameter)
{
	
	while(1)
	{
		task_auto_open_led_pro();
		task_pushdata_to_wechat_pro();
		rt_thread_mdelay(500);
	}
}

void event_pro_thread_create(void)
{
	event_pro_thread = rt_thread_create("event_pro_thread",
																			event_pro_thread_entry,
																			RT_NULL,
																			1024,16,300);
	if(event_pro_thread != RT_NULL)
	{
		rt_thread_startup(event_pro_thread);
	}
}





