/* Simple HTTP Server

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

   ex)
   curl "http://192.168.10.110:8080/take_picture"

*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"

#include "esp_http_server.h"

#include "cmd.h"

#if CONFIG_SHUTTER_HTTP

static const char *TAG = "WEB";

extern QueueHandle_t xQueueCmd;


/* An HTTP GET handler */
static esp_err_t my_uri_handler(httpd_req_t *req)
{
	CMD_t cmdBuf;
	cmdBuf.taskHandle = xTaskGetCurrentTaskHandle();
	cmdBuf.command = CMD_TAKE;
	if (xQueueSend(xQueueCmd, &cmdBuf, 10) != pdPASS) {
		ESP_LOGE(TAG, "xQueueSend fail");
	} else {
		ESP_LOGI(TAG, "xQueueSend success");
	}

	/* Send response with custom headers and body set as the
	 * string passed in user context*/
	const char* resp_str = (const char*) req->user_ctx;
	httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);

	return ESP_OK;
}


static httpd_handle_t start_webserver(void)
{
	httpd_handle_t server = NULL;
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	//config.server_port=8080;
	ESP_LOGI(TAG, "CONFIG_LOCAL_WEB_PORT=%d", CONFIG_LOCAL_WEB_PORT);
	config.server_port = CONFIG_LOCAL_WEB_PORT;

	// Start the httpd server
	ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
	if (httpd_start(&server, &config) == ESP_OK) {
		// Set URI handlers
		ESP_LOGI(TAG, "Registering URI handlers. CONFIG_LOCAL_WEB_URL=%s", CONFIG_LOCAL_WEB_URL);

		httpd_uri_t uri_handler;
		//uri_handler.uri	= "/hello";
		uri_handler.uri	= CONFIG_LOCAL_WEB_URL;
		uri_handler.method = HTTP_GET;
		uri_handler.handler = my_uri_handler;
		/* Let's pass response string in user
	 	* context to demonstrate it's usage */
		//.user_ctx  = "Hello World!"
		uri_handler.user_ctx  = "{\'result\':\'OK\'}";

		httpd_register_uri_handler(server, &uri_handler);
		return server;
	}

	ESP_LOGI(TAG, "Error starting server!");
	return NULL;
}

static void stop_webserver(httpd_handle_t server)
{
	// Stop the httpd server
	httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
							   int32_t event_id, void* event_data)
{
	httpd_handle_t* server = (httpd_handle_t*) arg;
	if (*server) {
		ESP_LOGI(TAG, "Stopping webserver");
		stop_webserver(*server);
		*server = NULL;
	}
}

static void connect_handler(void* arg, esp_event_base_t event_base,
							int32_t event_id, void* event_data)
{
	httpd_handle_t* server = (httpd_handle_t*) arg;
	if (*server == NULL) {
		ESP_LOGI(TAG, "Starting webserver");
		*server = start_webserver();
	}
}


void web_server(void *pvParameter)
{
	ESP_LOGI(TAG, "web_server start");

	static httpd_handle_t server = NULL;
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));

	/* Start the server for the first time */
	server = start_webserver();

	ESP_LOGI(TAG, "started web server");

	while(1) {
		vTaskDelay(100);
	}

}
#endif
