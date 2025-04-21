#define MAC_MAX_LEN (18)
#define IP_MAX_LEN	(16)

typedef enum {CMD_TAKE, CMD_HALT, CMD_INFO, CMD_UPLOAD} COMMAND;

// Shutter trigger request
typedef struct {
	uint16_t command;
	TaskHandle_t taskHandle;
} CMD_t;

// Message to HTTP Client
typedef struct {
	uint16_t command;
	char localFileName[64];
	char remoteFileName[64];
	esp_log_level_t logLevel;
	TaskHandle_t taskHandle;
} REQUEST_t;

#if 0
// Message from HTTP Client
typedef struct {
	uint16_t command;
	char response[256];
	TaskHandle_t taskHandle;
} RESPONSE_t;
#endif

// Message to HTTP Server
typedef struct {
	char localFileName[64];
	TaskHandle_t taskHandle;
} HTTP_t;
