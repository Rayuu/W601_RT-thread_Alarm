#include "sensor_pro.h"

#include <rtdevice.h>
#include <rtdbg.h>
#include "sensor_asair_aht10.h"
#include "sensor_lsc_ap3216c.h"


//传感器数据结构体
struct board_sensor myhome_sensor;

//要打开的设备传感器句柄
rt_device_t temp_dev,humi_dev,lux_dev,dis_dev;

//传感器处理线程句柄
rt_thread_t sensor_pro_thread = RT_NULL;

//查找并打开设备，传入参数设备的对象结构体，设备名称，返回是否错误
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

//传感器处理线程入口函数，参数为homesensor数据
void sensor_pro_thread_entry(void *parameter)
{
	static unsigned int dev_find_and_opened  = 0;

	//将线程参数赋值给read_data，结构体指针指向parameter
	home_sensor read_data = (home_sensor)parameter; 
	//临时存储传感器数据
	struct rt_sensor_data sensordata;
	//判断是否初始化完成
	rt_uint8_t rtempdev,rhumidev,rluxdev,rdisdev;
	//初始化一次，实际进入这个线程只执行一次，后续考虑删除判断
	if(dev_find_and_opened == 0)
	{
		//分别初始化温湿度，光照和距离传感器
		rtempdev = board_sensor_find_task(&temp_dev,TEMP_DEV);
		rhumidev = board_sensor_find_task(&humi_dev,HUMI_DEV);
		rluxdev = board_sensor_find_task(&lux_dev,ALS_DEV);
		rdisdev = board_sensor_find_task(&dis_dev,PS_DEV);
		if((rtempdev != RT_ERROR) && (rhumidev != RT_ERROR) &&(rluxdev != RT_ERROR) &&(rdisdev != RT_ERROR) )
		{
			//初始化完成
			dev_find_and_opened = 1;
		}
	}
	//读取数据
	while(1)
	{
		//读取温度
		rt_device_read(temp_dev,0,&sensordata,1);
		read_data->temperature = sensordata.data.temp;
		
		//读取湿度
		rt_device_read(humi_dev,0,&sensordata,1);
		read_data->humidity = sensordata.data.humi;
		
		
		//读取光照强度 lux
		rt_device_read(lux_dev,0,&sensordata,1);
		read_data->lux = sensordata.data.light;
		
		//读取距离(AD值 0~1023 离得越近值越大)
		rt_device_read(dis_dev,0,&sensordata,1);
		read_data->distance = sensordata.data.proximity;
		
		rt_thread_mdelay(300);
	}
}

//创建传感器处理线程
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


