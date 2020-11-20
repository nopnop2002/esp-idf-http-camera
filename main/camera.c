/* ESPRESSIF MIT License
 * 
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 * 
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_camera.h"
#include "camera.h"
//#include "sdkconfig.h"


static const char *TAG = "camera";

esp_err_t ledc_init(camera_config_t * config) {
	ledc_timer_config_t timer_conf;
	timer_conf.duty_resolution = 2;
	timer_conf.freq_hz = config->xclk_freq_hz;
	timer_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
	timer_conf.timer_num = config->ledc_timer;
	ESP_LOGI(TAG,"dr = %d, fr = %d", timer_conf.duty_resolution, timer_conf.freq_hz);
	timer_conf.clk_cfg = LEDC_AUTO_CLK;
	//timer_conf.clk_cfg = LEDC_USE_APB_CLK;
	ESP_LOGI(TAG, "start ledc_timer_config");
	esp_err_t err = ledc_timer_config(&timer_conf);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "ledc_timer_config failed, rc=%x", err);
		return err;
	}

	ledc_channel_config_t ch_conf;
	ch_conf.gpio_num = config->pin_xclk;
	ch_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
	ch_conf.channel = config->ledc_channel;
	ch_conf.intr_type = LEDC_INTR_DISABLE;
	ch_conf.timer_sel = config->ledc_timer;
	ch_conf.duty = 2;
	ch_conf.hpoint = 0;
	ESP_LOGI(TAG, "start ledc_channel_config");
	err = ledc_channel_config(&ch_conf);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "ledc_channel_config failed, rc=%x", err);
		return err;
	}
	ESP_LOGI(TAG, "start periph_module_enable");
	periph_module_enable(PERIPH_LEDC_MODULE);
	return ESP_OK;
}

esp_err_t camera_detect()
{
	ESP_LOGI(TAG, "CAMERA_MODEL:%s", CAMERA_MODEL);
#if CONFIG_CAMERA_MODEL_ESP_EYE
	/* IO13, IO14 is designed for JTAG by default,
	 * to use it as generalized input,
	 * firstly declair it as pullup input */
	gpio_config_t conf;
	conf.mode = GPIO_MODE_INPUT;
	conf.pull_up_en = GPIO_PULLUP_ENABLE;
	conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	conf.intr_type = GPIO_INTR_DISABLE;
	conf.pin_bit_mask = 1LL << 13;
	gpio_config(&conf);
	conf.pin_bit_mask = 1LL << 14;
	gpio_config(&conf);
#endif

	camera_config_t config;
	config.ledc_channel = LEDC_CHANNEL_0;
	config.ledc_timer = LEDC_TIMER_0;
	config.pin_d0 = Y2_GPIO_NUM;
	config.pin_d1 = Y3_GPIO_NUM;
	config.pin_d2 = Y4_GPIO_NUM;
	config.pin_d3 = Y5_GPIO_NUM;
	config.pin_d4 = Y6_GPIO_NUM;
	config.pin_d5 = Y7_GPIO_NUM;
	config.pin_d6 = Y8_GPIO_NUM;
	config.pin_d7 = Y9_GPIO_NUM;
	config.pin_xclk = XCLK_GPIO_NUM;
	config.pin_pclk = PCLK_GPIO_NUM;
	config.pin_vsync = VSYNC_GPIO_NUM;
	config.pin_href = HREF_GPIO_NUM;
	config.pin_sscb_sda = SIOD_GPIO_NUM;
	config.pin_sscb_scl = SIOC_GPIO_NUM;
	config.pin_pwdn = PWDN_GPIO_NUM;
	config.pin_reset = RESET_GPIO_NUM;
	config.xclk_freq_hz = 20000000;
	config.pixel_format = PIXFORMAT_JPEG;

	//init with high specs to pre-allocate larger buffers
#if CONFIG_USE_PSRAM
	ESP_LOGI(TAG, "Use PSRAM");
	//config.frame_size = FRAMESIZE_UXGA; // 1600x1200
	config.jpeg_quality = 10;
	config.fb_count = 2;
#endif

#if CONFIG_DONT_USE_PSRAM
	ESP_LOGI(TAG, "Don't Use PSRAM");
	//config.frame_size = FRAMESIZE_SVGA; // 800x600
	config.jpeg_quality = 12;
	config.fb_count = 1;
#endif

#if CONFIG_FRAMESIZE_QQVGA
	config.frame_size = FRAMESIZE_QQVGA;  // 160x120
#endif
#if CONFIG_FRAMESIZE_QCIF
	config.frame_size = FRAMESIZE_QCIF; // 176x144
#endif
#if CONFIG_FRAMESIZE_HQVGA
	config.frame_size = FRAMESIZE_HQVGA; // 240x176
#endif
#if CONFIG_FRAMESIZE_240X240
	config.frame_size = FRAMESIZE_240X240; // 240x240
#endif
#if CONFIG_FRAMESIZE_QVGA
	config.frame_size = FRAMESIZE_QVGA; // 320x240
#endif
#if CONFIG_FRAMESIZE_CIF
	config.frame_size = FRAMESIZE_CIF; // 400x296
#endif
#if CONFIG_FRAMESIZE_HVGA
	config.frame_size = FRAMESIZE_HVGA; // 480x320
#endif
#if CONFIG_FRAMESIZE_VGA
	config.frame_size = FRAMESIZE_VGA; // 640x480
#endif
#if CONFIG_FRAMESIZE_SVGA
	config.frame_size = FRAMESIZE_SVGA; // 800x600
#endif
#if CONFIG_FRAMESIZE_XGA
	config.frame_size = FRAMESIZE_XGA; // 1024x768
#endif
#if CONFIG_FRAMESIZE_HD
	config.frame_size = FRAMESIZE_HD; // 1280x720
#endif
#if CONFIG_FRAMESIZE_SXGA
	config.frame_size = FRAMESIZE_SXGA; // 1280x1024
#endif
#if CONFIG_FRAMESIZE_UXGA
	config.frame_size = FRAMESIZE_UXGA; // 1600x1200
#endif

	printFrameSize(config.frame_size);

	esp_err_t err;
#if 1
	// initialize LEDC
	// https://github.com/espressif/esp32-camera/issues/66
	err = ledc_init(&config);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "LEDC init failed with error 0x%x", err);
		return err;
	}
#endif

#if CONFIG_CAMERA_MODEL_AI_THINKER
	// initialize PWDN pin
	// https://github.com/espressif/esp32-camera/issues/66
	gpio_config_t gpio_pwr_config;
	gpio_pwr_config.pin_bit_mask = (1ULL << 32);
	gpio_pwr_config.mode = GPIO_MODE_OUTPUT;
	gpio_pwr_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
	gpio_pwr_config.pull_up_en = GPIO_PULLUP_DISABLE;
	gpio_pwr_config.intr_type = GPIO_INTR_DISABLE;
	ESP_LOGI(TAG, "start gpio_config");
	gpio_config(&gpio_pwr_config);
	gpio_set_level(32,0);
	vTaskDelay(10/ portTICK_PERIOD_MS);
#endif

	// initialize the camera
	ESP_LOGI(TAG, "start esp_camera_init");
	err = esp_camera_init(&config);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Camera init failed with error 0x%x", err);
		return err;
	}

	sensor_t * s = esp_camera_sensor_get();
	//initial sensors are flipped vertically and colors are a bit saturated
	if (s->id.PID == OV3660_PID) {
		s->set_vflip(s, 1);//flip it back
		s->set_brightness(s, 1);//up the blightness just a bit
		s->set_saturation(s, -2);//lower the saturation
	}
	//drop down frame size for higher initial frame rate
	//s->set_framesize(s, FRAMESIZE_QVGA);
	s->set_framesize(s, config.frame_size);
	return ESP_OK;
}

esp_err_t camera_capture(char * FileName, size_t *pictureSize)
{
	//acquire a frame
	camera_fb_t * fb = esp_camera_fb_get();
	if (!fb) {
		ESP_LOGE(TAG, "Camera Capture Failed");
		return ESP_FAIL;
	}

	//replace this with your own function
	//process_image(fb->width, fb->height, fb->format, fb->buf, fb->len);
	FILE* f = fopen(FileName, "wb");
	if (f == NULL) {
		ESP_LOGE(TAG, "Failed to open file for writing");
		return ESP_FAIL; 
	}
	fwrite(fb->buf, fb->len, 1, f);
	ESP_LOGI(TAG, "fb->len=%d", fb->len);
	*pictureSize = (size_t)fb->len;
	fclose(f);
  
	//return the frame buffer back to the driver for reuse
	esp_camera_fb_return(fb);
	return ESP_OK;
}

void printFrameSize(int size) {
	if (size == FRAMESIZE_96X96) {
		ESP_LOGI(TAG, "Frame Size:96x96");
	} else if (size == FRAMESIZE_QQVGA) {
		ESP_LOGI(TAG, "Frame Size:160x120");
	} else if (size == FRAMESIZE_QCIF) {
		ESP_LOGI(TAG, "Frame Size:176x144");
	} else if (size == FRAMESIZE_HQVGA) {
		ESP_LOGI(TAG, "Frame Size:240x176");
	} else if (size == FRAMESIZE_240X240) {
		ESP_LOGI(TAG, "Frame Size:240x240");
	} else if (size == FRAMESIZE_QVGA) {
		ESP_LOGI(TAG, "Frame Size:320x240");
	} else if (size == FRAMESIZE_CIF) {
		ESP_LOGI(TAG, "Frame Size:400x296");
	} else if (size == FRAMESIZE_HVGA) {
		ESP_LOGI(TAG, "Frame Size:480x320");
	} else if (size == FRAMESIZE_VGA) {
		ESP_LOGI(TAG, "Frame Size:640x480");
	} else if (size == FRAMESIZE_SVGA) {
		ESP_LOGI(TAG, "Frame Size:800x600");
	} else if (size == FRAMESIZE_XGA) {
		ESP_LOGI(TAG, "Frame Size:1024x768");
	} else if (size == FRAMESIZE_HD) {
		ESP_LOGI(TAG, "Frame Size:1280x720");
	} else if (size == FRAMESIZE_SXGA) {
		ESP_LOGI(TAG, "Frame Size:1280x1024");
	} else if (size == FRAMESIZE_UXGA) {
		ESP_LOGI(TAG, "Frame Size:1600x1200");
	}
}

