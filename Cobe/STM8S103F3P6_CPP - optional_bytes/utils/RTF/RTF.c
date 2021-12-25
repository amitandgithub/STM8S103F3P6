


#include"RTF.h"


uint8_t RTF_RequestBuffer[RTF_REQUEST_BUF_SIZE];
uint8_t RTF_ReplyBuffer[RTF_REPLY_BUF_SIZE];

static RTF_Context_t RTF_Context;
static  RTFState_t    RTFState;

RTFStatus_t RTF_Init()
{
  RTFState                      = RTF_STATE_IDLE;
  RTF_Context.RequestBuf        = RTF_RequestBuffer; 
  RTF_Context.ReplyBuf          = RTF_ReplyBuffer; 
  RTF_Context.RequestLen        = RTF_REQUEST_BUF_SIZE; 
  RTF_Context.ReplyLen          = RTF_REPLY_BUF_SIZE;  
  
  return RTF_STATUS_OK;
}

RTFState_t RTF_GetState()
{
  return RTFState;
}

void RTF_SetState(RTFState_t state)
{
  RTFState = state;
}

void RTF_Run()
{
  RTFState_t Current_state = RTF_GetState();
  RTFStatus_t Current_status;
  
  if(Current_state == RTF_STATE_IDLE)
  {
    if(RTF_ReceiveCmd() == RTF_STATUS_CMD_RECIVED )
    {
      RTF_SetState(RTF_STATE_CMD_RECEIVED);
    }   
  }
  else if(Current_state == RTF_STATE_CMD_RECEIVED)
  {
    Current_status = RTF_AuthenticateCmd();
    
    if( Current_status == RTF_STATUS_AUTHENTICATION_SUCCESS)
    {
      RTF_SetState(RTF_STATE_PROCESSING);
    } 
    else if(Current_status == RTF_STATUS_AUTHENTICATION_FAIL)
    {
      RTF_SetState(RTF_STATE_AUTHENTICATION_FAIL);
    } 
  }
  else if(Current_state == RTF_STATE_AUTHENTICATION_FAIL)
  {
    //Reply error response
  }
  else if(Current_state == RTF_STATE_PROCESSING)
  {
    if(RTF_ProcessCmd() == RTF_STATUS_PROCESSING_DONE )
    {
      RTF_SetState(RTF_STATE_PROCESSING_DONE);
    }
  }
  else if(Current_state == RTF_STATE_PROCESSING_DONE)
  {
    if(RTF_SendCmdResponse() == RTF_STATUS_RESPONSE_SENT )
    {
      //Call SPI Slave Rx
      RTF_SetState(RTF_STATE_IDLE);
    }
  }  
  else
  {
    // Error
  }
}

RTFStatus_t RTF_ReceiveCmd()
{
  return RTF_STATUS_CMD_RECIVED;
}

RTFStatus_t RTF_AuthenticateCmd()
{
  return RTF_STATUS_AUTHENTICATION_SUCCESS;
}

RTFStatus_t RTF_ProcessCmd()
{
  return RTF_STATUS_PROCESSING_DONE;
}

RTFStatus_t RTF_SendCmdResponse()
{
  return RTF_STATUS_RESPONSE_SENT;
}


