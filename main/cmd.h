#define CMD_TAKE	100
#define CMD_SEND	200
#define CMD_HALT	900

typedef struct {
    uint16_t command;
    TaskHandle_t taskHandle;
} CMD_t;

// Message to HTTP Client
typedef struct {
    uint16_t command;
    char localFileName[64];
    char remoteFileName[64];
    TaskHandle_t taskHandle;
} REQUEST_t;

// Message from HTTP Client
typedef struct {
    uint16_t command;
    char response[256];
    TaskHandle_t taskHandle;
} RESPONSE_t;

// Message to HTTP Server
typedef struct {
    char localFileName[64];
    TaskHandle_t taskHandle;
} HTTP_t;
