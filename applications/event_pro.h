#ifndef __EVENT_PRO_H__
#define __EVENT_PRO_H__

#include <rtthread.h>

//���صƵ���ֵ
#define OPEN_THRESHOLD_AD_VAL		400
#define CLOSE_THRESHOLD_AD_VAL	200

//��ʱ��ʱ����
#define ALARM_HOURS		14
#define ALARM_MINUTES	58
#define ALARM_SECONDS	0

#define ALARM_DURATION	20 //����ʱ��

//�����¼������߳�
void event_pro_thread_create(void);

#endif
