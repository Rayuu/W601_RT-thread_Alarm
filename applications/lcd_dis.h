#ifndef __LCD_DIS_H__
#define __LCD_DIS_H__


#include <rtthread.h>

#define LCD_DIS_THREAD_STACK_SIZE 			1024
#define LCD_DIS_THREAD_PRIORITY						10
#define LCD_DIS_THREAD_TIMESLICE					10			



rt_int8_t lcd_dis_thread_create(void);

#endif
