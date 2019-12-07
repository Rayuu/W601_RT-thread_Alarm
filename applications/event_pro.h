#ifndef __EVENT_PRO_H__
#define __EVENT_PRO_H__

#include <rtthread.h>

//开关灯的阈值
#define OPEN_THRESHOLD_AD_VAL		400
#define CLOSE_THRESHOLD_AD_VAL	200

//定时：时分秒
#define ALARM_HOURS		14
#define ALARM_MINUTES	58
#define ALARM_SECONDS	0

#define ALARM_DURATION	20 //持续时间

//创建事件处理线程
void event_pro_thread_create(void);

#endif
