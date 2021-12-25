


#ifndef CmdSvr_h
#define CmdSvr_h

#include"system.h"


#define CMDSVR_CMD_LENGTH_MAX 16

typedef enum
{
  MODULE_ID_I2C,
  
  
  MODULE_ID_MAX, 
}ModuleID_t;


typedef enum
{
  CMD_STATUS_OK,
  CMD_STATUS_INVALID_PARAMS,
  CMD_STATUS_LEN_ERROR,
  CMD_STATUS_CRC_ERROR,
  CMD_STATUS_INVALID_MODULEID,
  CMD_STATUS_INVALID_CMDID,  
  CMD_STATUS_ACK,
  CMD_STATUS_PROCESSING, // 7
  CMD_STATUS_PROCESSING_DONE, // 8
  CMD_STATUS_READY,  // 9
}CmdStatus_t;

typedef struct
{
  uint8_t ModuleID;
  uint8_t CmdID;
  uint8_t Len;
}CmdHdr_t;

typedef struct
{
  CmdStatus_t   Status;
  uint8_t       ModuleID;
  uint8_t       CmdID;
  uint8_t       Len;
}RspHdr_t;

typedef struct
{
  CmdHdr_t* Request;
  RspHdr_t* Response;
}Cmd_t;

typedef struct
{
  Cmd_t Cmd;
}CmdSvrContext_t;

typedef CmdStatus_t (*CmdHandler)(Cmd_t* CmdHdr);

CmdStatus_t Cmdsvr_Init();

CmdStatus_t Cmdsvr_GetCmd(Cmd_t *pCmd);

CmdStatus_t Cmdsvr_AuthenticateCmd(Cmd_t *pCmd);

CmdStatus_t Cmdsvr_DispatchCmd(Cmd_t *pCmd);

CmdStatus_t Cmdsvr_DispatchResponse(Cmd_t *pCmd);

void I2C_CmdSvr_Callback(i2c::I2CStatus_t status);

uint8_t* CmdSvr_SwitchBuf(uint8_t* Buffer);

CmdStatus_t CmdSvr_GetStatus();

void CmdSvr_Run(void);






















#endif