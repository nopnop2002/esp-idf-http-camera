#define CMD_TAKE 100
#define CMD_SEND 100
#define CMD_HALT 400

typedef struct {
    uint16_t command;
    TaskHandle_t taskHandle;
} CMD_t;

typedef struct {
    uint16_t command;
    char localFileName[64];
    char remoteFileName[64];
    TaskHandle_t taskHandle;
} REQUEST_t;

typedef struct {
    uint16_t command;
    char response[256];
    TaskHandle_t taskHandle;
} RESPONSE_t;
