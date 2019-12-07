#include "infrared_pro.h"

#include <stdio.h>

#include <rtdevice.h>
#include <rtdbg.h>
#include "board.h"
//easyflash
#include <easyflash.h>
#include "sensor_pro.h"


//����ɨ�裬������� PIN_KEY0,PIN_KEY1
rt_int16_t key_scan(rt_int16_t keynum)
{
		if(rt_pin_read(keynum) == PIN_LOW)
		{
			rt_thread_mdelay(30);
			if(rt_pin_read(keynum) == PIN_LOW)
			{
				return keynum;
			}
		}
		return RT_EOK;
}

rt_thread_t infrared_pro_thread = RT_NULL;
void infrared_pro_thread_entry(void *parameter)
{
	rt_int16_t key0,key1;
	struct infrared_decoder_data recvinfrared_data;
	char tempvalue[3]={0};
	//read
	char *readtemp;
	struct infrared_decoder_data sendkeyvalue;
	//ѡ��nec������
	ir_select_decoder("nec");
	
	while(1)
	{
		key0 = key_scan(PIN_KEY0);
		if(key0 == PIN_KEY0)
		{
			rt_pin_write(PIN_LED_R,PIN_LOW);
			readtemp = ef_get_env("led_infrared_data");
			//��ȡʧ��
			if(readtemp == RT_NULL)
			{
				readtemp[0] = '0';
			}
			sendkeyvalue.data.nec.key = atoi(readtemp);
			sendkeyvalue.data.nec.repeat = 0;
			//���ͺ�������
			infrared_write("nec",&sendkeyvalue);
			rt_thread_mdelay(200);
			LOG_I("SEND    OK: addr:0x%02X key:0x%02X repeat:%d",
					sendkeyvalue.data.nec.addr, sendkeyvalue.data.nec.key, sendkeyvalue.data.nec.repeat);
		}
		else if(infrared_read("nec",&recvinfrared_data) == RT_EOK)
		{
			myhome_sensor.infrared_recv[0] = recvinfrared_data.data.nec.key;
			//ת�ַ���
			sprintf(tempvalue, "%d", recvinfrared_data.data.nec.key);
			//������յ�key
			ef_set_and_save_env("led_infrared_data",tempvalue);
						
			rt_pin_write(PIN_LED_B,PIN_LOW);
		}

		rt_thread_mdelay(300);
		rt_pin_write(PIN_LED_B,PIN_HIGH);
		rt_pin_write(PIN_LED_R,PIN_HIGH);
	}
	
}


void infrared_pro_thread_create(void)
{
	infrared_pro_thread = rt_thread_create("infrared_pro_thread",
																				infrared_pro_thread_entry,
																				RT_NULL,
																				1024,15,100);
	if(infrared_pro_thread != RT_NULL)
	{
		rt_thread_startup(infrared_pro_thread);
	}
}

