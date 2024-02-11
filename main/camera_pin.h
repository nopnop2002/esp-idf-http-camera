// Freenove ESP32-WROVER CAM Board PIN Map
#if CONFIG_BOARD_ESP32_WROVER_FREENOVE
#define CAM_PIN_PWDN  -1 //power down is not used
#define CAM_PIN_RESET -1 //software reset will be performed
#define CAM_PIN_XCLK  21
#define CAM_PIN_SIOD  26
#define CAM_PIN_SIOC  27

#define CAM_PIN_D7    35 // Y9
#define CAM_PIN_D6    34 // Y8
#define CAM_PIN_D5    39 // Y7
#define CAM_PIN_D4    36 // Y6
#define CAM_PIN_D3    19 // Y5
#define CAM_PIN_D2    18 // Y4
#define CAM_PIN_D1     5 // Y3
#define CAM_PIN_D0     4 // Y2
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF  23
#define CAM_PIN_PCLK  22
#endif

// Freenove ESP32S3-WROOM CAM Board PIN Map
#if CONFIG_BOARD_ESP32S3_WROOM_FREENOVE
#define CAM_PIN_PWDN  -1 //power down is not used
#define CAM_PIN_RESET -1 //software reset will be performed
#define CAM_PIN_XCLK  15
#define CAM_PIN_SIOD   4
#define CAM_PIN_SIOC   5

#define CAM_PIN_D7    16 // Y9
#define CAM_PIN_D6    17 // Y8
#define CAM_PIN_D5    18 // Y7
#define CAM_PIN_D4    12 // Y6
#define CAM_PIN_D3    10 // Y5
#define CAM_PIN_D2     8 // Y4
#define CAM_PIN_D1     9 // Y3
#define CAM_PIN_D0    11 // Y2
#define CAM_PIN_VSYNC  6
#define CAM_PIN_HREF   7
#define CAM_PIN_PCLK  13
#endif

// ESP-EYE PIN Map
#if CONFIG_BOARD_CAMERA_MODEL_ESP_EYE
#define CAM_PIN_PWDN  -1 //power down is not used
#define CAM_PIN_RESET -1 //software reset will be performed
#define CAM_PIN_XCLK   4
#define CAM_PIN_SIOD  18
#define CAM_PIN_SIOC  23

#define CAM_PIN_D7    36 // Y9
#define CAM_PIN_D6    37 // Y8
#define CAM_PIN_D5    38 // Y7
#define CAM_PIN_D4    39 // Y6
#define CAM_PIN_D3    35 // Y5
#define CAM_PIN_D2    14 // Y4
#define CAM_PIN_D1    13 // Y3
#define CAM_PIN_D0    34 // Y2
#define CAM_PIN_VSYNC  5
#define CAM_PIN_HREF  27
#define CAM_PIN_PCLK  25
#endif


// AiThinker ESP32Cam PIN Map
#if CONFIG_BOARD_ESP32CAM_AITHINKER
#define CAM_PIN_PWDN  32
#define CAM_PIN_RESET -1 //software reset will be performed
#define CAM_PIN_XCLK   0
#define CAM_PIN_SIOD  26
#define CAM_PIN_SIOC  27

#define CAM_PIN_D7    35 // Y9
#define CAM_PIN_D6    34 // Y8
#define CAM_PIN_D5    39 // Y7
#define CAM_PIN_D4    36 // Y6
#define CAM_PIN_D3    21 // Y5
#define CAM_PIN_D2    19 // Y4
#define CAM_PIN_D1    18 // Y3
#define CAM_PIN_D0     5 // Y2
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF  23
#define CAM_PIN_PCLK  22
#endif

// TTGO T-Journal ESP32 Camera PIN Map
#if CONFIG_BOARD_CAMERA_MODEL_TTGO_T_JOURNAL
#define CAM_PIN_PWDN   0
#define CAM_PIN_RESET 15
#define CAM_PIN_XCLK  27
#define CAM_PIN_SIOD  25
#define CAM_PIN_SIOC  23

#define CAM_PIN_D7    19 // Y9
#define CAM_PIN_D6    36 // Y8
#define CAM_PIN_D5    18 // Y7
#define CAM_PIN_D4    39 // Y6
#define CAM_PIN_D3     5 // Y5
#define CAM_PIN_D2    34 // Y4
#define CAM_PIN_D1    35 // Y3
#define CAM_PIN_D0    17 // Y2
#define CAM_PIN_VSYNC 22
#define CAM_PIN_HREF  26
#define CAM_PIN_PCLK  21
#endif

// UICPAL ESPS3 CAM RE:1.0
#if CONFIG_BOARD_ESPS3_CAM_UICPAL
#define CAM_PIN_PWDN  -1 //power down is not used
#define CAM_PIN_RESET -1 //software reset will be performed
#define CAM_PIN_XCLK  10
#define CAM_PIN_SIOD  21
#define CAM_PIN_SIOC  14

#define CAM_PIN_D7    11 // Y9
#define CAM_PIN_D6     9 // Y8
#define CAM_PIN_D5     8 // Y7
#define CAM_PIN_D4     6 // Y6
#define CAM_PIN_D3     4 // Y5
#define CAM_PIN_D2     2 // Y4
#define CAM_PIN_D1     3 // Y3
#define CAM_PIN_D0     5 // Y2
#define CAM_PIN_VSYNC 13
#define CAM_PIN_HREF  12
#define CAM_PIN_PCLK   7
#endif
