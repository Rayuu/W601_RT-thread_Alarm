#ifndef __INFRARED_H__
#define __INFRARED_H__

#include <rtthread.h>
//����infrared
#include "infrared.h"

//����ɨ�裬������� PIN_KEY0,PIN_KEY1
rt_int16_t key_scan(rt_int16_t keynum);

void infrared_pro_thread_create(void);

#endif
