#ifndef __SENSOR_PRO_H__
#define __SENSOR_PRO_H__

#include <rtthread.h>


//������ʪ�ȴ�����
#define TEMP_DEV	"temp_aht"
#define HUMI_DEV	"humi_aht"

//��ǿ�ͽӽ�������
#define ALS_DEV		"li_ap321"
#define PS_DEV		"pr_ap321"

#define SENSOR_PRO_THREAD_STACK_SIZE 			1024
#define SENSOR_PRO_THREAD_PRIORITY					9
#define SENSOR_PRO_THREAD_TIMESLICE					1000	

//���Ӵ��������壬����LC��ʾ
struct board_sensor
{
	rt_int32_t temperature;//�¶�0.1C
	rt_int32_t humidity;	//ʪ��%
	rt_int32_t   lux;			//����ǿ��LUX
	rt_int32_t distance;	//�ӽ�������������ADֵ
	rt_int32_t close_flag;//������־
	rt_uint8_t	infrared_recv[2];//�������
	struct tm *date;//����
};
typedef struct board_sensor *home_sensor;//����ṹ��ָ�룬����ʱ��ʹ��
//���������ݽṹ��
extern struct board_sensor myhome_sensor;

//���������������߳�
rt_int8_t sensor_pro_thread_create(void);

#endif
