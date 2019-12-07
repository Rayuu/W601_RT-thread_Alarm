#include "sensor_pro.h"

#include <rtdevice.h>
#include <rtdbg.h>
#include "sensor_asair_aht10.h"
#include "sensor_lsc_ap3216c.h"


//���������ݽṹ��
struct board_sensor myhome_sensor;

//Ҫ�򿪵��豸���������
rt_device_t temp_dev,humi_dev,lux_dev,dis_dev;

//�����������߳̾��
rt_thread_t sensor_pro_thread = RT_NULL;

//���Ҳ����豸����������豸�Ķ���ṹ�壬�豸���ƣ������Ƿ����
rt_uint8_t board_sensor_find_task(rt_device_t *tdevice,char* devname)
{
	*tdevice = rt_device_find(devname);
	if(*tdevice == RT_NULL)
	{
		LOG_E("can not find device: %s",devname);
		return RT_ERROR;
	}
	else
	{
		if(rt_device_open(*tdevice,RT_DEVICE_FLAG_RDONLY) != RT_EOK)
		{
			LOG_E("open %s device failed!",devname);
			return RT_ERROR;
		}
	}
	LOG_E("open %s device success!",devname);
	return RT_EOK;
		
		
}

//�����������߳���ں���������Ϊhomesensor����
void sensor_pro_thread_entry(void *parameter)
{
	static unsigned int dev_find_and_opened  = 0;

	//���̲߳�����ֵ��read_data���ṹ��ָ��ָ��parameter
	home_sensor read_data = (home_sensor)parameter; 
	//��ʱ�洢����������
	struct rt_sensor_data sensordata;
	//�ж��Ƿ��ʼ�����
	rt_uint8_t rtempdev,rhumidev,rluxdev,rdisdev;
	//��ʼ��һ�Σ�ʵ�ʽ�������߳�ִֻ��һ�Σ���������ɾ���ж�
	if(dev_find_and_opened == 0)
	{
		//�ֱ��ʼ����ʪ�ȣ����պ;��봫����
		rtempdev = board_sensor_find_task(&temp_dev,TEMP_DEV);
		rhumidev = board_sensor_find_task(&humi_dev,HUMI_DEV);
		rluxdev = board_sensor_find_task(&lux_dev,ALS_DEV);
		rdisdev = board_sensor_find_task(&dis_dev,PS_DEV);
		if((rtempdev != RT_ERROR) && (rhumidev != RT_ERROR) &&(rluxdev != RT_ERROR) &&(rdisdev != RT_ERROR) )
		{
			//��ʼ�����
			dev_find_and_opened = 1;
		}
	}
	//��ȡ����
	while(1)
	{
		//��ȡ�¶�
		rt_device_read(temp_dev,0,&sensordata,1);
		read_data->temperature = sensordata.data.temp;
		
		//��ȡʪ��
		rt_device_read(humi_dev,0,&sensordata,1);
		read_data->humidity = sensordata.data.humi;
		
		
		//��ȡ����ǿ�� lux
		rt_device_read(lux_dev,0,&sensordata,1);
		read_data->lux = sensordata.data.light;
		
		//��ȡ����(ADֵ 0~1023 ���Խ��ֵԽ��)
		rt_device_read(dis_dev,0,&sensordata,1);
		read_data->distance = sensordata.data.proximity;
		
		rt_thread_mdelay(300);
	}
}

//���������������߳�
rt_int8_t sensor_pro_thread_create(void)
{
	sensor_pro_thread = rt_thread_create("sensor_pro",
																			sensor_pro_thread_entry,
																			&myhome_sensor,
																			SENSOR_PRO_THREAD_STACK_SIZE,
																			SENSOR_PRO_THREAD_PRIORITY,
																			SENSOR_PRO_THREAD_TIMESLICE);
	if(sensor_pro_thread != RT_NULL)
	{
		rt_thread_startup(sensor_pro_thread);
	}
	return RT_EOK;
}


