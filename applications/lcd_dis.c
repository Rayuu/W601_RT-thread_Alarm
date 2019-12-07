#include "lcd_dis.h"

#include "sensor_pro.h"
#include "event_pro.h"
#include <drv_lcd.h>

//定义LCD显示线程
rt_thread_t lcd_dis_thread = RT_NULL;

extern struct rt_semaphore net_ready;
//lcd显示函数
void lcd_task_pro(struct board_sensor *tsensor)
{
		//第一行
		lcd_show_string(0,0,24,"temp: %3d.%d C",(tsensor)->temperature/10,(tsensor)->temperature%10);
		//第二行
		lcd_show_string(0,0+24,24,"humi: %3d.%d%%",(tsensor)->humidity/10,(tsensor)->humidity%10);
	  //第三行
		lcd_show_string(0,0+24+24,24,"lux : %3d.%d(lux)",(tsensor)->lux/10,(tsensor)->lux%10);
		//第四行
		lcd_show_string(0,0+24+24+24,24,"dist: %4d",(tsensor)->distance);

		//第五行，显示时间
		time_t now;
		now = time(RT_NULL);
		tsensor->date = localtime(&now);
		lcd_show_string(0,0+24+24+24+24,24,"time:%2d-%2d-%2d",tsensor->date->tm_hour,tsensor->date->tm_min,tsensor->date->tm_sec);
		//第六行，显示日期
		lcd_show_string(0,0+24+24+24+24+24,24,"date:%4d-%2d-%2d",1900+tsensor->date->tm_year,1+tsensor->date->tm_mon,tsensor->date->tm_mday);
		
		//第七行，红外编码
		lcd_show_string(0,0+24+24+24+24+24+24,24,"recv_infr0:%3d",tsensor->infrared_recv[0]);
		//第七行，红外编码
		lcd_show_string(0,0+24+24+24+24+24+24+24,24,"recv_infr1:%3d",tsensor->infrared_recv[1]);
		//第八行，闹钟时间
		lcd_show_string(0,0+24+24+24+24+24+24+24+24,24,"alarm_t:%2d-%2d-%2d",ALARM_HOURS,ALARM_MINUTES,ALARM_SECONDS);
}

//LCD显示线程入口
void lcd_dis_thread_entry(void *parameter)
{
	while(1)
	{
		lcd_task_pro(&myhome_sensor);
		rt_thread_mdelay(100);
	}
}

//创建LCD显示线程
rt_int8_t lcd_dis_thread_create(void)
{
	lcd_dis_thread = rt_thread_create("lcd_dis",
																		lcd_dis_thread_entry,
																		RT_NULL,
																		LCD_DIS_THREAD_STACK_SIZE,
																		LCD_DIS_THREAD_PRIORITY,
																		LCD_DIS_THREAD_TIMESLICE);
	if(lcd_dis_thread != RT_NULL)
	{
		rt_thread_startup(lcd_dis_thread);
	}
	return RT_EOK;
}

