/* The example of GPIO Input
 *
 * This sample code is in the public domain.
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "cmd.h"

#if CONFIG_SHUTTER_GPIO

extern QueueHandle_t xQueueCmd;

static const char *TAG = "GPIO";

void gpio(void *pvParameter)
{
	ESP_LOGI(TAG, "Start CONFIG_GPIO_INPUT=%d", CONFIG_GPIO_INPUT);
	CMD_t cmdBuf;
	cmdBuf.taskHandle = xTaskGetCurrentTaskHandle();
	cmdBuf.command = CMD_TAKE;

	// set the GPIO as a input
	gpio_reset_pin(CONFIG_GPIO_INPUT);
	gpio_set_direction(CONFIG_GPIO_INPUT, GPIO_MODE_DEF_INPUT);

#if CONFIG_GPIO_PULLUP
	ESP_LOGI(TAG, "GPIO%d is PULL UP", CONFIG_GPIO_INPUT);
	int push = 0;
	int release = 1;
#endif
#if CONFIG_GPIO_PULLDOWN
	ESP_LOGI(TAG, "GPIO%d is PULL DOWN", CONFIG_GPIO_INPUT);
	int push = 1;
	int release = 0;
#endif
	ESP_LOGI(TAG, "push=%d release=%d", push, release);

	while(1) {
		int level = gpio_get_level(CONFIG_GPIO_INPUT);
		if (level == push) {
			ESP_LOGI(TAG, "Push Button");
			while(1) {
				level = gpio_get_level(CONFIG_GPIO_INPUT);
				if (level == release) break;
				vTaskDelay(1);
			}
			ESP_LOGI(TAG, "Release Button");
			if (xQueueSend(xQueueCmd, &cmdBuf, 10) != pdPASS) {
				ESP_LOGE(TAG, "xQueueSend fail");
			}
		}
		vTaskDelay(1);
	}

	/* Never reach */
	vTaskDelete( NULL );
}
#endif
