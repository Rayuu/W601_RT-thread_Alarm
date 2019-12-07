#include "event_pro.h"

#include <rtthread.h>
#include <rtdevice.h>
#include <rtdbg.h>
#include "board.h"

#include "sensor_pro.h"
#include "infrared_pro.h"

//������ƵƵĿ���
void task_auto_open_led_pro(void)
{
	struct infrared_decoder_data sendvalue;
	if(myhome_sensor.distance >= OPEN_THRESHOLD_AD_VAL)
	{
		//���ͺ�������
		sendvalue.data.nec.addr = 0;
		sendvalue.data.nec.key = myhome_sensor.infrared_recv[0];
		sendvalue.data.nec.repeat = 0;
		infrared_write("nec",&sendvalue);
		rt_thread_mdelay(200);
	}

}


//������Ϣ��΢��
void task_pushdata_to_wechat_pro(void)
{
		time_t now,end_time;
		static time_t start_time;
		now = time(RT_NULL);
		//�������
		struct infrared_decoder_data sendvalue;
		myhome_sensor.date = localtime(&now);
		//�����趨��ʱ�䣬������ʪ����Ϣ��΢�ţ��򿪺���ƣ��򿪷�����
		if((myhome_sensor.date->tm_hour == ALARM_HOURS) && (myhome_sensor.date->tm_min == ALARM_MINUTES) &&\
				(myhome_sensor.date->tm_sec == ALARM_SECONDS))
		{
			start_time = time(RT_NULL);
			//��΢��������Ϣ
			extern int mbedtls_client_start(void);
			mbedtls_client_start();
			//�򿪷�����
			rt_pin_write(PIN_BEEP,PIN_HIGH);
			//���ͺ������ݣ�����
			sendvalue.data.nec.addr = 0;
			sendvalue.data.nec.key = myhome_sensor.infrared_recv[0];
			sendvalue.data.nec.repeat = 0;
			infrared_write("nec",&sendvalue);
			rt_thread_mdelay(200);
			
		}
		end_time = time(RT_NULL);
		if((end_time - start_time) == ALARM_DURATION)
		{
			//�رշ�����
			rt_pin_write(PIN_BEEP,PIN_LOW);
		}
}

//�¼������߳̾��
rt_thread_t event_pro_thread = RT_NULL;
//�߳����
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





