#include "https_get_wechat.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rtthread.h>
#include <finsh.h>
#include <msh.h>

#include <tls_certificate.h>
#include <tls_client.h>

//home_sensor传入线程
#include "sensor_pro.h"

#if !defined(MBEDTLS_CONFIG_FILE)
#include <mbedtls/config.h>
#else
#include MBEDTLS_CONFIG_FILE
#endif

#define MBEDTLS_WEB_SERVER  "sc.ftqq.com"
#define MBEDTLS_WEB_PORT    "443"

#define MBEDTLS_READ_BUFFER 1024

//去http://sc.ftqq.com/?c=wechat&a=bind自行申请ID
#ifndef WeChat_send_secret

#define WeChat_send_secret "**************.send"

#endif

char *s1 = "GET /%s?text=%s&desp=%s";
char *s3 = " HTTP/1.1\r\n"
		"Host: sc.ftqq.com\r\n"
    "User-Agent: rtthread/3.1 rtt\r\n"
    "\r\n";
char *d_text = RT_NULL;
char *d_desp = RT_NULL;
char sREQUEST[2048];

static void mbedtls_client_entry(void *parameter)
{
    int ret = 0;
    MbedTLSSession *tls_session = RT_NULL;
    char *pers = "hello_world";
		char s_info[128];//报文内容
		//将线程参数赋值给read_data，结构体指针指向parameter
		home_sensor read_data = (home_sensor)parameter;
		
		//主人，早上好。请点击查看传感器信息。	
		d_desp = "%e4%b8%bb%e4%ba%ba%ef%bc%8c%e6%97%a9%e4%b8%8a%e5%a5%bd%e3%80%82%e8%af%b7%e7%82%b9%e5%87%bb%e6%9f%a5%e7%9c%8b%e4%bc%a0%e6%84%9f%e5%99%a8%e4%bf%a1%e6%81%af%e3%80%82%09";
		//温度： 湿度：
		d_text = "%e6%b8%a9%e5%ba%a6=%d.%dC,%e6%b9%bf%e5%ba%a6=%d.%d%";
		
		//温湿度信息打印到字符串
		rt_snprintf(s_info,128,d_text,read_data->temperature/10,read_data->temperature%10,\
																	read_data->humidity/10,read_data->humidity%10);
		//组合get的报文
		rt_snprintf(sREQUEST,2048,s1,WeChat_send_secret,d_desp,s_info);
		strcat(sREQUEST,s3);
		
#ifdef FINSH_USING_MSH
    rt_kprintf("Memory usage before the handshake connection is established:\r\n");
    msh_exec("free", rt_strlen("free"));
#endif
    tls_session = (MbedTLSSession *) tls_malloc(sizeof(MbedTLSSession));
    if (tls_session == RT_NULL)
    {
        rt_kprintf("No memory for MbedTLS session object.\n");
        return;
    }
    rt_memset(tls_session, 0x0, sizeof(MbedTLSSession));

    tls_session->host = tls_strdup(MBEDTLS_WEB_SERVER);
    tls_session->port = tls_strdup(MBEDTLS_WEB_PORT);

    tls_session->buffer_len = MBEDTLS_READ_BUFFER;
    tls_session->buffer = tls_malloc(tls_session->buffer_len);
    if (tls_session->buffer == RT_NULL)
    {
        rt_kprintf("No memory for MbedTLS buffer\n");
        if (tls_session->host)
        {
            tls_free(tls_session->host);
        }
        if (tls_session->port)
        {
            tls_free(tls_session->port);
        }
        tls_free(tls_session);
        return;
    }

    rt_kprintf("Start handshake tick:%d\n", rt_tick_get());

    if ((ret = mbedtls_client_init(tls_session, (void *) pers, rt_strlen(pers))) != 0)
    {
        rt_kprintf("MbedTLSClientInit err return : -0x%x\n", -ret);
        goto __exit;
    }

    if ((ret = mbedtls_client_context(tls_session)) < 0)
    {
        rt_kprintf("MbedTLSCLlientContext err return : -0x%x\n", -ret);
        goto __exit;
    }

    if ((ret = mbedtls_client_connect(tls_session)) != 0)
    {
        rt_kprintf("MbedTLSCLlientConnect err return : -0x%x\n", -ret);
        goto __exit;
    }

    rt_kprintf("Finish handshake tick:%d\n", rt_tick_get());

    rt_kprintf("MbedTLS connect success...\n");

#ifdef FINSH_USING_MSH
    rt_kprintf("Memory usage after the handshake connection is established:\r\n");
    msh_exec("free", rt_strlen("free"));
#endif

    while ((ret = mbedtls_client_write(tls_session, (const unsigned char *) sREQUEST, rt_strlen(sREQUEST))) <= 0)
    {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            rt_kprintf("mbedtls_ssl_write returned -0x%x\n", -ret);
            goto __exit;
        }
    }
    rt_kprintf("Writing HTTP request success...\n");

    rt_kprintf("Getting HTTP response...\n");
    {
        int i = 0;

        rt_memset(tls_session->buffer, 0x00, MBEDTLS_READ_BUFFER);
        ret = mbedtls_client_read(tls_session, (unsigned char *) tls_session->buffer, MBEDTLS_READ_BUFFER);
        if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE
                || ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
            goto __exit;

        if (ret < 0)
        {
            rt_kprintf("Mbedtls_ssl_read returned -0x%x\n", -ret);
            goto __exit;
        }

        if (ret == 0)
        {
            rt_kprintf("TCP server connection closed.\n");
            goto __exit;
        }

        for (i = 0; i < ret; i++)
            rt_kprintf("%c", tls_session->buffer[i]);

        rt_kprintf("\n");
    }

__exit:
    mbedtls_client_close(tls_session);

    rt_kprintf("MbedTLS connection close success.\n");

    return;
}

int mbedtls_client_start(void)
{
    rt_thread_t tid;
		//传入温湿度数据
    tid = rt_thread_create("tls_c", mbedtls_client_entry, &myhome_sensor, 6 * 1024, RT_THREAD_PRIORITY_MAX / 3 - 1, 5);
    if (tid)
    {
        rt_thread_startup(tid);
        return RT_EOK;
    }

    return -RT_ERROR;
}
