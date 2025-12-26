/*
	HTTP Client Example using plain POSIX sockets

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
#include "esp_mac.h" // MACSTR
#include "esp_wifi.h" // esp_wifi_get_mac

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "cJSON.h"
#include "cmd.h"

/* Constants that are configurable in menuconfig */
#if 0
#define CONFIG_SERVER_IP "192.168.10.43"
#define CONFIG_SERVER_PORT 8080
#endif

#define WEB_PATH_INFO "/node_information"
#define WEB_PATH_UPLOAD "/upload_multipart"
#define BOUNDARY "X-ESPIDF_MULTIPART"

extern QueueHandle_t xQueueRequest;

static const char *TAG = "CLIENT";

void http_client(void *pvParameters)
{
	ESP_LOGI(TAG, "Start SERVER_IP:%s SERVER_PORT:%d", CONFIG_SERVER_IP, CONFIG_SERVER_PORT);

	// Get the local IP address
	esp_netif_ip_info_t ip_info;
	ESP_ERROR_CHECK(esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &ip_info));
	char ip_str[IP_MAX_LEN];
	snprintf(ip_str, sizeof(ip_str), IPSTR, IP2STR(&ip_info.ip));

	// Get station mac address
	uint8_t sta_mac[6] = {0};
	esp_wifi_get_mac(WIFI_IF_STA, sta_mac);
	char mac_str[MAC_MAX_LEN];
	snprintf(mac_str, sizeof(mac_str), MACSTR, MAC2STR(sta_mac));

	const struct addrinfo hints = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM,
	};
	struct addrinfo *info;
	struct in_addr *addr;
	char service[16];
	snprintf(service, sizeof(service), "%d", CONFIG_SERVER_PORT);

	REQUEST_t requestBuf;
	TickType_t startTick, endTick, diffTick;
	while(1) {
		ESP_LOGD(TAG,"Waitting....");
		xQueueReceive(xQueueRequest, &requestBuf, portMAX_DELAY);
		esp_log_level_set(TAG, requestBuf.logLevel);
		ESP_LOGI(TAG,"requestBuf.command=%d", requestBuf.command);
		if (requestBuf.command == CMD_HALT) break;

		//int err = getaddrinfo(CONFIG_SERVER_IP, CONFIG_SERVER_PORT, &hints, &info);
		int err = getaddrinfo(CONFIG_SERVER_IP, service, &hints, &info);

		if(err != 0 || info == NULL) {
			ESP_LOGE(TAG, "DNS lookup failed err=%d info=%p", err, info);
			//vTaskDelay(1000 / portTICK_PERIOD_MS);
			if (requestBuf.taskHandle != NULL)
				xTaskNotify(requestBuf.taskHandle, 0x02, eSetValueWithOverwrite);
			continue;
		}

		/* Code to print the resolved IP.
		   Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
		addr = &((struct sockaddr_in *)info->ai_addr)->sin_addr;
		ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

		int sock = socket(info->ai_family, info->ai_socktype, 0);
		if (sock < 0) {
			ESP_LOGE(TAG, "... Failed to allocate socket.");
			freeaddrinfo(info);
			//vTaskDelay(1000 / portTICK_PERIOD_MS);
			if (requestBuf.taskHandle != NULL)
				xTaskNotify(requestBuf.taskHandle, 0x03, eSetValueWithOverwrite);
			continue;
		}
		ESP_LOGI(TAG, "... allocated socket");

		if(connect(sock, info->ai_addr, info->ai_addrlen) != 0) {
			ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
			close(sock);
			freeaddrinfo(info);
			//vTaskDelay(4000 / portTICK_PERIOD_MS);
			if (requestBuf.taskHandle != NULL)
				xTaskNotify(requestBuf.taskHandle, 0x04, eSetValueWithOverwrite);
			continue;
		}

		// I don't use info anymore
		ESP_LOGI(TAG, "... connected");
		freeaddrinfo(info);


		if (requestBuf.command == CMD_INFO) {
			char HEADER[512];
			char header[128];

			sprintf(header, "POST %s HTTP/1.1\r\n", WEB_PATH_INFO);
			strcpy(HEADER, header);
			sprintf(header, "Host: %s:%d\r\n", CONFIG_SERVER_IP, CONFIG_SERVER_PORT);
			strcat(HEADER, header);
			sprintf(header, "User-Agent: esp-idf/%d.%d.%d esp32\r\n", ESP_IDF_VERSION_MAJOR, ESP_IDF_VERSION_MINOR, ESP_IDF_VERSION_PATCH);
			strcat(HEADER, header);
			sprintf(header, "Accept: */*\r\n");
			strcat(HEADER, header);
			sprintf(header, "Content-Type: application/json\r\n");
			strcat(HEADER, header);

			TickType_t nowTick = xTaskGetTickCount();
			cJSON *root;
			root = cJSON_CreateObject();
			cJSON_AddNumberToObject(root, "now", nowTick);
			cJSON_AddStringToObject(root, "ip", ip_str);
			cJSON_AddStringToObject(root, "mac", mac_str);

#if CONFIG_BOARD_ESP32_WROVER_FREENOVE
			cJSON_AddStringToObject(root, "board", "Freenove ESP32-WROVER CAM Board");
#elif CONFIG_BOARD_ESP32S3_WROOM_FREENOVE
			cJSON_AddStringToObject(root, "board", "Freenove ESP32S3-WROVER CAM Board");
#elif CONFIG_BOARD_CAMERA_MODEL_ESP_EYE
			cJSON_AddStringToObject(root, "board", "Espressif ESP-EYE");
#elif CONFIG_BOARD_ESP32CAM_AITHINKER
			cJSON_AddStringToObject(root, "board", "AiThinker ESP32-CAM");
#elif CONFIG_BOARD_CAMERA_MODEL_TTGO_T_JOURNAL
			cJSON_AddStringToObject(root, "board", "TTGO T-Journal ESP32 Camera");
#elif CONFIG_BOARD_ESPS3_CAM_UICPAL
			cJSON_AddStringToObject(root, "board", "UICPAL ESPS3 CAM RE:1.0");
#endif

#if CONFIG_FRAMESIZE_VGA
			cJSON_AddStringToObject(root, "frame", "640x480");
#elif CONFIG_FRAMESIZE_SVGA
			cJSON_AddStringToObject(root, "frame", "800x600");
#elif CONFIG_FRAMESIZE_XGA
			cJSON_AddStringToObject(root, "frame", "1024x768");
#elif CONFIG_FRAMESIZE_HD
			cJSON_AddStringToObject(root, "frame", "1280x720");
#elif CONFIG_FRAMESIZE_SXGA
			cJSON_AddStringToObject(root, "frame", "1280x1024");
#elif CONFIG_FRAMESIZE_UXGA
			cJSON_AddStringToObject(root, "frame", "1600x1200");
#endif
			//char *json_string = cJSON_Print(root);
			char *json_string = cJSON_PrintUnformatted(root);
			cJSON_Delete(root);

			char BODY[512];
			strcpy(BODY, json_string);
			cJSON_free(json_string);

			int dataLength = strlen(BODY);
			sprintf(header, "Content-Length: %d\r\n\r\n", dataLength);
			strcat(HEADER, header);

			ESP_LOGD(TAG, "[%s]", HEADER);
			if (write(sock, HEADER, strlen(HEADER)) < 0) {
				ESP_LOGE(TAG, "... socket send failed");
				close(sock);
				//vTaskDelay(4000 / portTICK_PERIOD_MS);
				if (requestBuf.taskHandle != NULL)
					xTaskNotify(requestBuf.taskHandle, 0x05, eSetValueWithOverwrite);
				continue;
			}
			ESP_LOGI(TAG, "HEADER socket send success");

			ESP_LOGD(TAG, "[%s]", BODY);
			if (write(sock, BODY, strlen(BODY)) < 0) {
				ESP_LOGE(TAG, "... socket send failed");
				close(sock);
				//vTaskDelay(4000 / portTICK_PERIOD_MS);
				if (requestBuf.taskHandle != NULL)
					xTaskNotify(requestBuf.taskHandle, 0x06, eSetValueWithOverwrite);
				continue;
			}
			ESP_LOGI(TAG, "BODY socket send success");
		}

		if (requestBuf.command == CMD_UPLOAD) {
			startTick = xTaskGetTickCount();
			ESP_LOGI(TAG,"requestBuf.localFileName=%s", requestBuf.localFileName);
			ESP_LOGI(TAG,"requestBuf.remoteFileName=%s", requestBuf.remoteFileName);
			struct stat statBuf;
			if (stat(requestBuf.localFileName, &statBuf) == 0) {
				ESP_LOGI(TAG, "st_size=%"PRIi32, statBuf.st_size);
			} else {
				ESP_LOGE(TAG, "stat fail");
				if (requestBuf.taskHandle != NULL)
					xTaskNotify(requestBuf.taskHandle, 0x01, eSetValueWithOverwrite);
				continue;
			}

			char HEADER[512];
			char header[128];

			sprintf(header, "POST %s HTTP/1.1\r\n", WEB_PATH_UPLOAD);
			strcpy(HEADER, header);
			sprintf(header, "Host: %s:%d\r\n", CONFIG_SERVER_IP, CONFIG_SERVER_PORT);
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
			if (write(sock, HEADER, strlen(HEADER)) < 0) {
				ESP_LOGE(TAG, "... socket send failed");
				close(sock);
				//vTaskDelay(4000 / portTICK_PERIOD_MS);
				if (requestBuf.taskHandle != NULL)
					xTaskNotify(requestBuf.taskHandle, 0x05, eSetValueWithOverwrite);
				continue;
			}
			ESP_LOGI(TAG, "HEADER socket send success");

			ESP_LOGD(TAG, "[%s]", BODY);
			if (write(sock, BODY, strlen(BODY)) < 0) {
				ESP_LOGE(TAG, "... socket send failed");
				close(sock);
				//vTaskDelay(4000 / portTICK_PERIOD_MS);
				if (requestBuf.taskHandle != NULL)
					xTaskNotify(requestBuf.taskHandle, 0x06, eSetValueWithOverwrite);
				continue;
			}
			ESP_LOGI(TAG, "BODY socket send success");

			FILE* f=fopen(requestBuf.localFileName, "rb");
			uint8_t dataBuffer[128];
			if (f == NULL) {
				ESP_LOGE(TAG, "Failed to open file for reading");
				break;
			}
			while(!feof(f)) {
				int len = fread(dataBuffer, 1, sizeof(dataBuffer), f);
				if (write(sock, dataBuffer, len) < 0) {
					ESP_LOGE(TAG, "... socket send failed");
					close(sock);
					//vTaskDelay(4000 / portTICK_PERIOD_MS);
					if (requestBuf.taskHandle != NULL)
						xTaskNotify(requestBuf.taskHandle, 0x07, eSetValueWithOverwrite);
					continue;
				}
			}
			fclose(f);
			ESP_LOGI(TAG, "DATA socket send success");

			if (write(sock, END, strlen(END)) < 0) {
				ESP_LOGE(TAG, "... socket send failed");
				close(sock);
				//vTaskDelay(4000 / portTICK_PERIOD_MS);
				if (requestBuf.taskHandle != NULL)
					xTaskNotify(requestBuf.taskHandle, 0x08, eSetValueWithOverwrite);
				continue;
			}
			ESP_LOGI(TAG, "END socket send success");
			endTick = xTaskGetTickCount();
			diffTick = endTick - startTick;
			ESP_LOGI(TAG, "elapsed time[ms]:%"PRIu32,diffTick*portTICK_PERIOD_MS);
		}

		struct timeval receiving_timeout;
		receiving_timeout.tv_sec = 5;
		receiving_timeout.tv_usec = 0;
		if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout, sizeof(receiving_timeout)) < 0) {
			ESP_LOGE(TAG, "... failed to set socket receiving timeout");
			close(sock);
			//vTaskDelay(4000 / portTICK_PERIOD_MS);
			if (requestBuf.taskHandle != NULL)
				xTaskNotify(requestBuf.taskHandle, 0x09, eSetValueWithOverwrite);
			continue;
		}
		ESP_LOGI(TAG, "... set socket receiving timeout success");

		/* Read HTTP response */
		int readed;
		char recv_buf[64];
		char responseBuf[256];
		int responseLen = 0;
		bzero(responseBuf, sizeof(responseBuf));
		do {
			bzero(recv_buf, sizeof(recv_buf));
			readed = read(sock, recv_buf, sizeof(recv_buf)-1);
			ESP_LOGI(TAG, "readed=%d", readed);
			if (readed < 0) {
				ESP_LOGE(TAG, "socket read fail. readed=%d", readed);
			} else {
				if (responseLen + readed < sizeof(responseBuf)-1) {
					strcat(responseBuf, recv_buf);
					responseLen = responseLen + readed;
				} else {
					ESP_LOGW(TAG, "responseBuf too small. responseLen=%d readed=%d", responseLen, readed);
					ESP_LOGW(TAG, "responseBuf\n[%.*s]", responseLen, responseBuf);
				}
			}
		} while(readed > 0);

		/* send response */
		ESP_LOGI(TAG, "done reading from socket. Last readed=%d responseLen=%d errno=%d.", readed, responseLen, errno);
		ESP_LOGI(TAG, "responseBuf\n[%.*s]", responseLen, responseBuf);
		uint32_t ulValue = 0x00;
		if (strncmp(responseBuf, "HTTP/1.0 200", 12) == 0) {
			//xTaskNotify(requestBuf.taskHandle, 0x00, eSetValueWithOverwrite);
		} else if (strncmp(responseBuf, "HTTP/1.1 200", 12) == 0) {
			//xTaskNotify(requestBuf.taskHandle, 0x00, eSetValueWithOverwrite);
		} else {
			ESP_LOGW(TAG, "responseBuf\n[%.*s]", responseLen, responseBuf);
			ESP_LOGW(TAG, "Server does not return a successful response");
			ulValue = 0x90;
			//xTaskNotify(requestBuf.taskHandle, 0x90, eSetValueWithOverwrite);
		}	
		if (requestBuf.taskHandle != NULL)
			xTaskNotify(requestBuf.taskHandle, ulValue, eSetValueWithOverwrite);
		close(sock);

	}
}
