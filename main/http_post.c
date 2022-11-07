/* HTTP POST Example using plain POSIX sockets

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "cmd.h"

/* Constants that are configurable in menuconfig */
#if 0
#define CONFIG_WEB_SERVER "192.168.10.43"
#define CONFIG_WEB_PORT "8080"
#define CONFIG_WEB_PATH "/upload_multipart"
#endif

#define BOUNDARY "X-ESPIDF_MULTIPART"

extern QueueHandle_t xQueueRequest;
extern QueueHandle_t xQueueResponse;

static const char *TAG = "POST";

void http_post_task(void *pvParameters)
{
	const struct addrinfo hints = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM,
	};
	struct addrinfo *res;
	struct in_addr *addr;
	char recv_buf[64];

	REQUEST_t requestBuf;
	while(1) {
		ESP_LOGI(TAG,"Waitting....");
		xQueueReceive(xQueueRequest, &requestBuf, portMAX_DELAY);
		ESP_LOGI(TAG,"requestBuf.command=%d", requestBuf.command);
		if (requestBuf.command == CMD_HALT) break;

		ESP_LOGI(TAG,"requestBuf.localFileName=%s", requestBuf.localFileName);
		ESP_LOGI(TAG,"requestBuf.remoteFileName=%s", requestBuf.remoteFileName);
		struct stat statBuf;
		if (stat(requestBuf.localFileName, &statBuf) == 0) {
			ESP_LOGI(TAG, "st_size=%d", (int)statBuf.st_size);
		} else {
			ESP_LOGE(TAG, "stat fail");
			continue;
		}

		int err = getaddrinfo(CONFIG_WEB_SERVER, CONFIG_WEB_PORT, &hints, &res);

		if(err != 0 || res == NULL) {
			ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			continue;
		}

		/* Code to print the resolved IP.

		   Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
		addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
		ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

		int s = socket(res->ai_family, res->ai_socktype, 0);
		if(s < 0) {
			ESP_LOGE(TAG, "... Failed to allocate socket.");
			freeaddrinfo(res);
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			continue;
		}
		ESP_LOGI(TAG, "... allocated socket");

		if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
			ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
			close(s);
			freeaddrinfo(res);
			vTaskDelay(4000 / portTICK_PERIOD_MS);
			continue;
		}

		ESP_LOGI(TAG, "... connected");
		freeaddrinfo(res);

		char HEADER[512];
		char header[128];

		sprintf(header, "POST %s HTTP/1.1\r\n", CONFIG_WEB_PATH);
		strcpy(HEADER, header);
		sprintf(header, "Host: %s:%s\r\n", CONFIG_WEB_SERVER, CONFIG_WEB_PORT);
		strcat(HEADER, header);
		sprintf(header, "User-Agent: esp-idf/%d.%d.%d esp32\r\n", ESP_IDF_VERSION_MAJOR, ESP_IDF_VERSION_MINOR, ESP_IDF_VERSION_PATCH);
		strcat(HEADER, header);
		sprintf(header, "Accept: */*\r\n");
		strcat(HEADER, header);
		sprintf(header, "Content-Type: multipart/form-data; boundary=%s\r\n", BOUNDARY);
		strcat(HEADER, header);
	
		char BODY[512];
		sprintf(header, "--%s\r\n", BOUNDARY);
		strcpy(BODY, header);
		sprintf(header, "Content-Disposition: form-data; name=\"upfile\"; filename=\"%s\"\r\n", requestBuf.remoteFileName);
		strcat(BODY, header);
		sprintf(header, "Content-Type: application/octet-stream\r\n\r\n");
		strcat(BODY, header);

		char END[128];
		sprintf(header, "\r\n--%s--\r\n\r\n", BOUNDARY);
		strcpy(END, header);
		
		int dataLength = strlen(BODY) + strlen(END) + statBuf.st_size;
		sprintf(header, "Content-Length: %d\r\n\r\n", dataLength);
		strcat(HEADER, header);

		ESP_LOGD(TAG, "[%s]", HEADER);
		if (write(s, HEADER, strlen(HEADER)) < 0) {
			ESP_LOGE(TAG, "... socket send failed");
			close(s);
			vTaskDelay(4000 / portTICK_PERIOD_MS);
			continue;
		}
		ESP_LOGI(TAG, "HEADER socket send success");

		ESP_LOGD(TAG, "[%s]", BODY);
		if (write(s, BODY, strlen(BODY)) < 0) {
			ESP_LOGE(TAG, "... socket send failed");
			close(s);
			vTaskDelay(4000 / portTICK_PERIOD_MS);
			continue;
		}
		ESP_LOGI(TAG, "BODY socket send success");

		FILE* f=fopen(requestBuf.localFileName, "rb");
		uint8_t dataBuffer[128];
		if (f == NULL) {
			ESP_LOGE(TAG, "Failed to open file for reading");
		}
		while(!feof(f)) {
			int len = fread(dataBuffer, 1, sizeof(dataBuffer), f);
			if (write(s, dataBuffer, len) < 0) {
				ESP_LOGE(TAG, "... socket send failed");
				close(s);
				vTaskDelay(4000 / portTICK_PERIOD_MS);
				continue;
			}
		}
		fclose(f);
		ESP_LOGI(TAG, "DATA socket send success");

		if (write(s, END, strlen(END)) < 0) {
			ESP_LOGE(TAG, "... socket send failed");
			close(s);
			vTaskDelay(4000 / portTICK_PERIOD_MS);
			continue;
		}
		ESP_LOGI(TAG, "END socket send success");

		struct timeval receiving_timeout;
		receiving_timeout.tv_sec = 5;
		receiving_timeout.tv_usec = 0;
		if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
				sizeof(receiving_timeout)) < 0) {
			ESP_LOGE(TAG, "... failed to set socket receiving timeout");
			close(s);
			vTaskDelay(4000 / portTICK_PERIOD_MS);
			continue;
		}
		ESP_LOGI(TAG, "... set socket receiving timeout success");

		/* Read HTTP response */
		int readed;
		RESPONSE_t responseBuf;
		bzero(responseBuf.response, sizeof(responseBuf.response));
		do {
			bzero(recv_buf, sizeof(recv_buf));
			ESP_LOGD(TAG, "Start read now=%"PRIu32, xTaskGetTickCount());
			readed = read(s, recv_buf, sizeof(recv_buf)-1);
			ESP_LOGD(TAG, "End	 read now=%"PRIu32" readed=%d", xTaskGetTickCount(), readed);
#if 0
			for(int i = 0; i < readed; i++) {
				putchar(recv_buf[i]);
			}
#endif
			strcat(responseBuf.response, recv_buf);
		} while(readed > 0);
#if 0
		printf("\n");
#endif

		/* send HTTP response */
		if (xQueueSend(xQueueResponse, &responseBuf, 10) != pdPASS) {
			ESP_LOGE(TAG, "xQueueSend fail");
		}

		ESP_LOGI(TAG, "... done reading from socket. Last read return=%d errno=%d.", readed, errno);
		close(s);

	}
}
