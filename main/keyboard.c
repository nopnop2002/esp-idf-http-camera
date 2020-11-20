/* The example of Keyboard Input
 *
 * This sample code is in the public domain.
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "cmd.h"

#if CONFIG_SHUTTER_ENTER
extern QueueHandle_t xQueueCmd;

static const char *TAG = "KEYBOARD";

void keyin(void *pvParameters)
{
	ESP_LOGI(TAG, "Start");
	CMD_t cmdBuf;
	cmdBuf.taskHandle = xTaskGetCurrentTaskHandle();
	cmdBuf.command = CMD_TAKE;

	uint16_t c;
	while (1) {
		c = fgetc(stdin);
		if (c == 0xffff) {
			vTaskDelay(10);
			continue;
		}
		//ESP_LOGI(TAG, "c=%x", c);
		if (c == 0x0a) {
			ESP_LOGI(TAG, "Push Enter");
			if (xQueueSend(xQueueCmd, &cmdBuf, 10) != pdPASS) {
				ESP_LOGE(TAG, "xQueueSend fail");
			}
		}
	}

	/* Never reach */
	vTaskDelete( NULL );
}
#endif

