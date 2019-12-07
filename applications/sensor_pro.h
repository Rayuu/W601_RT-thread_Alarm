#ifndef __SENSOR_PRO_H__
#define __SENSOR_PRO_H__

#include <rtthread.h>


//配置温湿度传感器
#define TEMP_DEV	"temp_aht"
#define HUMI_DEV	"humi_aht"

//光强和接近传感器
#define ALS_DEV		"li_ap321"
#define PS_DEV		"pr_ap321"

#define SENSOR_PRO_THREAD_STACK_SIZE 			1024
#define SENSOR_PRO_THREAD_PRIORITY					9
#define SENSOR_PRO_THREAD_TIMESLICE					1000	

//板子传感器定义，用于LC显示
struct board_sensor
{
	rt_int32_t temperature;//温度0.1C
	rt_int32_t humidity;	//湿度%
	rt_int32_t   lux;			//光照强度LUX
	rt_int32_t distance;	//接近传感器，距离AD值
	rt_int32_t close_flag;//靠近标志
	rt_uint8_t	infrared_recv[2];//红外编码
	struct tm *date;//日期
};
typedef struct board_sensor *home_sensor;//定义结构体指针，传参时候使用
//传感器数据结构体
extern struct board_sensor myhome_sensor;

//创建传感器处理线程
rt_int8_t sensor_pro_thread_create(void);

#endif
