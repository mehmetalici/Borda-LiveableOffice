#include <string.h>

// SimpleLink includes
#include "simplelink.h"

// driverlib includes
#include "hw_ints.h"
#include "hw_types.h"
#include "rom.h"
#include "rom_map.h"
#include "prcm.h"
#include "utils.h"
#include "interrupt.h"
// common interface includes
#include "uart_if.h"
#include "common.h"
#include "pinmux.h"
// HTTP Client lib
#include <http/client/httpcli.h>
#include <http/client/common.h>

// JSON Parser
#include "jsmn.h"
#ifndef HTTP_H_
#define HTTP_H_



#define POST_TOKEN_URI      "/api/v1/gettoken"
#define POST_RESTROOM_URI   "/api/v1/RestRoom"
#define POST_COFFEE_URI     "/api/v1/Coffee/Post"

#define DELETE_REQUEST_URI  "/delete"


#define PUT_REQUEST_URI     "/put"
#define PUT_DATA            "PUT request."

#define GET_REQUEST_URI     "/get"

#define HOST_NAME           "13.69.96.43"//"40.113.130.155"

#define HOST_PORT           80

#define PROXY_IP            <proxy_ip>
#define PROXY_PORT          <proxy_port>

#define READ_SIZE           1450
#define MAX_BUFF_SIZE       1460

extern char POST_DATA[100];
extern char* prttoData;
// Application specific status/error codes
typedef enum{
 /* Choosing this number to avoid overlap with host-driver's error codes */
    DEVICE_NOT_IN_STATION_MODE = -0x7D0,
    DEVICE_START_FAILED = DEVICE_NOT_IN_STATION_MODE - 1,
    INVALID_HEX_STRING = DEVICE_START_FAILED - 1,
    TCP_RECV_ERROR = INVALID_HEX_STRING - 1,
    TCP_SEND_ERROR = TCP_RECV_ERROR - 1,
    FILE_NOT_FOUND_ERROR = TCP_SEND_ERROR - 1,
    INVALID_SERVER_RESPONSE = FILE_NOT_FOUND_ERROR - 1,
    FORMAT_NOT_SUPPORTED = INVALID_SERVER_RESPONSE - 1,
    FILE_OPEN_FAILED = FORMAT_NOT_SUPPORTED - 1,
    FILE_WRITE_ERROR = FILE_OPEN_FAILED - 1,
    INVALID_FILE = FILE_WRITE_ERROR - 1,
    SERVER_CONNECTION_FAILED = INVALID_FILE - 1,
    GET_HOST_IP_FAILED = SERVER_CONNECTION_FAILED  - 1,
    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;


typedef enum {
    ID_KEY = 1,
    ID_VAL,
    ID_KEY2,
    ID_VAL2
}e_PostDataIdx;


typedef struct {
     char id_key[5];
     char id[6];
     char val_key[9];
     char val[31];
}s_PostData;



/* FUNCTION PROTOTYPES */
void HTTP_URISet(char* src);
long SetAuthentication(HTTPCli_Handle httpClient);
jsmntok_t *JSONTokensGet(char *ptr, int *noOfToken);
long HTTP_PostDataSet(s_PostData sensor);
int ParseJSONData(char *ptr);
long ResetNwp();
long SetConnectionPolicy();
long ConnectToAP();
int ConnectToHTTPServer(HTTPCli_Handle httpClient);
void DisplayBanner(char * AppName);
int HTTPGetMethod(HTTPCli_Handle httpClient);
int HTTPPutMethod(HTTPCli_Handle httpClient);
void InitializeAppVariables();
void BoardInit(void);
int HTTPPostMethod(HTTPCli_Handle httpClient);
int HTTPDeleteMethod(HTTPCli_Handle httpClient);
int readResponse(HTTPCli_Handle httpClient);
float getSample();
char *replace(char *str, char *orig, char *rep);
void HTTP_IF_ConfigureNEnable(HTTPCli_Struct *httpClient);
#endif /* HTTP_H_ */
