#ifndef __INFRARED_H__
#define __INFRARED_H__

#include <rtthread.h>
//红外infrared
#include "infrared.h"

//按键扫描，传入参数 PIN_KEY0,PIN_KEY1
rt_int16_t key_scan(rt_int16_t keynum);

void infrared_pro_thread_create(void);

#endif
