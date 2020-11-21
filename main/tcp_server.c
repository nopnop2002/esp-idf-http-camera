/* BSD Socket API Example

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
#include "mdns.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

#include "cmd.h"

#if CONFIG_SHUTTER_TCP

extern QueueHandle_t xQueueCmd;

static const char *TAG = "TCP";

void tcp_server(void *pvParameters)
{
	ESP_LOGI(TAG, "Start TCP PORT=%d", CONFIG_TCP_PORT);
	CMD_t cmdBuf;
	cmdBuf.taskHandle = xTaskGetCurrentTaskHandle();
	cmdBuf.command = CMD_TAKE;

	char rx_buffer[128];
	char tx_buffer[128];
	char addr_str[128];
	int addr_family;
	int ip_protocol;

#if 1
	struct sockaddr_in dest_addr;
	dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	dest_addr.sin_family = AF_INET;
	//dest_addr.sin_port = htons(PORT);
	dest_addr.sin_port = htons(CONFIG_TCP_PORT);
	addr_family = AF_INET;
	ip_protocol = IPPROTO_IP;
	inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);
#else
	struct sockaddr_in6 dest_addr;
	bzero(&dest_addr.sin6_addr.un, sizeof(dest_addr.sin6_addr.un));
	dest_addr.sin6_family = AF_INET6;
	dest_addr.sin6_port = htons(CONFIG_TCP_PORT);
	addr_family = AF_INET6;
	ip_protocol = IPPROTO_IPV6;
	inet6_ntoa_r(dest_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
#endif

	int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
	if (listen_sock < 0) {
		ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
		return;
	}
	ESP_LOGI(TAG, "Socket created");

	int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
	if (err != 0) {
		ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
		return;
	}
	ESP_LOGI(TAG, "Socket bound, port %d", CONFIG_TCP_PORT);

	err = listen(listen_sock, 1);
	if (err != 0) {
		ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
		return;
	}
	ESP_LOGI(TAG, "Socket listening");

	while (1) {
		struct sockaddr_in6 source_addr; // Large enough for both IPv4 or IPv6
		uint addr_len = sizeof(source_addr);
		int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
		if (sock < 0) {
			ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
			break;
		}
		ESP_LOGI(TAG, "Socket accepted");

		while (1) {
			int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
			// Error occurred during receiving
			if (len < 0) {
				ESP_LOGE(TAG, "recv failed: errno %d", errno);
				break;
			}
			// Connection closed by client
			else if (len == 0) {
				ESP_LOGI(TAG, "Connection closed");
				break;
			}

			// Data received
			// Get the sender's ip address as string
			if (source_addr.sin6_family == PF_INET) {
				inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
			} else if (source_addr.sin6_family == PF_INET6) {
				inet6_ntoa_r(source_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
			}

			rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
			ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
			ESP_LOGI(TAG, "%s", rx_buffer);
			if (xQueueSend(xQueueCmd, &cmdBuf, 10) != pdPASS) {
				ESP_LOGE(TAG, "xQueueSend fail");
				strcpy(tx_buffer, "FAIL");
				int err = send(sock, tx_buffer, strlen(tx_buffer), 0);
				if (err < 0) {
					ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
					break;
				}

			} else {
				strcpy(tx_buffer, "OK");
				int err = send(sock, tx_buffer, strlen(tx_buffer), 0);
				if (err < 0) {
					ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
					break;
				}
			}
		}

		ESP_LOGI(TAG, "Close socket");
		close(sock);
	}

	/* Don't reach here. */
	vTaskDelete(NULL);
}
#endif
