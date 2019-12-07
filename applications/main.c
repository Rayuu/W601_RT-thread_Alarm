/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-06-03     ChenYong     first implementation
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <wifi_config.h>
#include <webclient.h>

#include <board.h>
#include <drv_lcd.h>
#include <smartconfig.h>

//easyflash
#include <easyflash.h>
#include <fal.h>
//红外处理
#include "infrared_pro.h"

//LCD display
#include "lcd_dis.h"
//传感器处理
#include "sensor_pro.h"
//事件处理
#include "event_pro.h"


#include "ntp.h"

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

/* 配置 LED 灯引脚 */
#define LED_PIN     PIN_LED_R

struct rt_semaphore net_ready;

void wlan_ready_handler(int event, struct rt_wlan_buff *buff, void *parameter);
void wlan_station_disconnect_handler(int event, struct rt_wlan_buff *buff, void *parameter);

int main(void)
{
    int result = RT_EOK;
		//当前时间
		static struct tm *tm;
		//读取红外编码值到临时变量
		char *openled_infrared_data;

		/* 设置 LED 引脚为输出模式 */
    rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);
		rt_pin_mode(PIN_LED_B, PIN_MODE_OUTPUT);
		//初始化蜂鸣器
		rt_pin_mode(PIN_BEEP,PIN_MODE_OUTPUT);
	
		//这两个函数在wifi config.c中的wlan_autoconnect_init()初始化
		//初始化flash
		//fal_init();
		//easyflash初始化，读取红外编码
    //if (easyflash_init() == EF_NO_ERR)

    /* 配置 wifi 工作模式 */
    rt_wlan_set_mode(RT_WLAN_DEVICE_STA_NAME, RT_WLAN_STATION);
    
    /* 初始化 wlan 自动连接功能 */
    wlan_autoconnect_init();

    /* 使能 wlan 自动连接功能 */
    rt_wlan_config_autoreconnect(RT_TRUE);

    /* 创建 'net_ready' 信号量 */
    result = rt_sem_init(&net_ready, "net_ready", 0, RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        return -RT_ERROR;
    }

    /* 注册 wlan 连接网络成功的回调，wlan 连接网络成功后释放 'net_ready' 信号量 */
    rt_wlan_register_event_handler(RT_WLAN_EVT_READY, wlan_ready_handler, RT_NULL);
    /* 注册 wlan 网络断开连接的回调 */
    rt_wlan_register_event_handler(RT_WLAN_EVT_STA_DISCONNECTED, wlan_station_disconnect_handler, RT_NULL);

		//读取数据暂存
		openled_infrared_data = ef_get_env("led_infrared_data");
		//读取失败
		if(openled_infrared_data == RT_NULL)
		{
			openled_infrared_data[0] = '0';
		}
		//读取的红外编码值存到myhome_sensor
		myhome_sensor.infrared_recv[0] = (rt_uint8_t)atoi(openled_infrared_data);

		//lcd init
		lcd_clear(WHITE);
		lcd_set_color(WHITE,BLACK);
	
		//查找传感器并打开，采集数据
		sensor_pro_thread_create();
	
		//创建infrared处理线程并启动
		infrared_pro_thread_create();
		
		//lcd显示线程创建并启动
		lcd_dis_thread_create();
		//事件处理
		event_pro_thread_create();
		
    /* 等待 wlan 连接网络成功 */
    result = rt_sem_take(&net_ready, RT_WAITING_FOREVER);
    if (result != RT_EOK)
    {
        LOG_E("Wait net ready failed!");
        rt_sem_delete(&net_ready);
        return -RT_ERROR;
    }
		else
		{
			//同步网络时间
			ntp_sync_to_rtc(RT_NULL);
			time_t now;
			now = time(RT_NULL);
			
			tm = localtime(&now);
			rt_kprintf("now time:%d-%d-%d\n", tm->tm_hour,tm->tm_min,tm->tm_sec);
		}

		//test_网络连接成功，启动 mbedTLS 客户端 推送温湿度消息到微信
//		extern int mbedtls_client_start(void);
//    mbedtls_client_start();
}

/**
 * The callback of network ready event
 */
void wlan_ready_handler(int event, struct rt_wlan_buff *buff, void *parameter)
{
    rt_sem_release(&net_ready);
}

/**
 * The callback of wlan disconected event
 */
void wlan_station_disconnect_handler(int event, struct rt_wlan_buff *buff, void *parameter)
{
    LOG_I("disconnect from the network!");
}


