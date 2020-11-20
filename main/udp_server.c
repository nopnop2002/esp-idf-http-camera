/*
   UDP Broadcast Receiver Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

#include "cmd.h"

#if CONFIG_SHUTTER_UDP

extern QueueHandle_t xQueueCmd;

static const char *TAG = "UDP";

void udp_server(void *pvParameters)
{
	ESP_LOGI(TAG, "Start UDP PORT=%d", CONFIG_UDP_PORT);
	CMD_t cmdBuf;
	cmdBuf.taskHandle = xTaskGetCurrentTaskHandle();
	cmdBuf.command = CMD_TAKE;

	/* set up address to recvfrom */
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	//addr.sin_port = htons(UDP_PORT);
	addr.sin_port = htons(CONFIG_UDP_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY); /* senderInfo message from ANY */

	/* create the socket */
	int fd;
	int ret;
	fd = lwip_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP ); // Create a UDP socket.
	LWIP_ASSERT("fd >= 0", fd >= 0);

#if 0
	/* set option */
	int broadcast=1;
	ret = lwip_setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast);
	LWIP_ASSERT("ret >= 0", ret >= 0);
#endif

	/* bind socket */
	ret = lwip_bind(fd, (struct sockaddr *)&addr, sizeof(addr));
	LWIP_ASSERT("ret >= 0", ret >= 0);

	/* senderInfo data */
	char buffer[64];
	struct sockaddr_in senderInfo;
	socklen_t senderInfoLen = sizeof(senderInfo);
	char senderstr[16];
	while(1) {
		memset(buffer, 0, sizeof(buffer));
		//ESP_LOGI(TAG, "senderInfoLen=%d", senderInfoLen);
		ret = lwip_recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&senderInfo, &senderInfoLen);
		LWIP_ASSERT("ret > 0", ret > 0);
		ESP_LOGI(TAG, "lwip_recv ret=%d",ret);
		if (ret > 0) {
			buffer[ret] = 0;
			ESP_LOGI(TAG, "lwip_recv buffer=%s",buffer);
			inet_ntop(AF_INET, &senderInfo.sin_addr, senderstr, sizeof(senderstr));
			ESP_LOGI(TAG, "recvfrom : %s, port=%d", senderstr, ntohs(senderInfo.sin_port));
			if (xQueueSend(xQueueCmd, &cmdBuf, 10) != pdPASS) {
				ESP_LOGE(TAG, "xQueueSend fail");
			}

		}
	}

	/* close socket. Don't reach here. */
	ret = lwip_close(fd);
	LWIP_ASSERT("ret == 0", ret == 0);
	vTaskDelete( NULL );
}
#endif
