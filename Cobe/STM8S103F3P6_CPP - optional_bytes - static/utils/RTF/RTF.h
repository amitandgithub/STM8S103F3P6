


#ifndef __RTF_h
#define __RTF_h

#include"system.h"

#define RTF_REQUEST_BUF_SIZE    128
#define RTF_REPLY_BUF_SIZE      30

typedef enum
{
  RTF_STATUS_OK,
  RTF_STATUS_CMD_RECIVED,
  RTF_STATUS_AUTHENTICATION_SUCCESS,
  RTF_STATUS_AUTHENTICATION_FAIL,
  RTF_STATUS_PROCESSING,
  RTF_STATUS_PROCESSING_DONE,
  RTF_STATUS_RESPONSE_SENT
}RTFStatus_t;

typedef enum
{
  RTF_STATE_IDLE,
  RTF_STATE_CMD_RECEIVED,
  RTF_STATE_AUTHENTICATION_SUCCESS,
  RTF_STATE_AUTHENTICATION_FAIL,
  RTF_STATE_PROCESSING, 
  RTF_STATE_PROCESSING_DONE,
  RTF_STATE_ERROR  
}RTFState_t;

enum
{
  RTF_EVENT_NONE                        = 0x00U,
  RTF_EVENT_CMD_RECEIVED                = 0x01U,
  RTF_EVENT_AUTHENTICATION_SUCCESS      = 0x02U,
  RTF_EVENT_AUTHENTICATION_FAIL         = 0x04U,
  RTF_EVENT_CMD_RESPONSE_AVAILABLE      = 0x08U,
  RTF_EVENT_CMD_RESPONSE_SENT           = 0x10U
};

typedef struct
{
  uint8_t*      RequestBuf;
  uint8_t*      ReplyBuf;
  uint8_t       RequestLen;
  uint8_t       ReplyLen;  
}RTF_Context_t;


RTFStatus_t RTF_Init();

RTFState_t RTF_GetState();

void RTF_SetState(RTFState_t state);

void RTF_Run();

RTFStatus_t RTF_ReceiveCmd();

RTFStatus_t RTF_AuthenticateCmd();

RTFStatus_t RTF_ProcessCmd();

RTFStatus_t RTF_SendCmdResponse();





#endif //__RTF_h