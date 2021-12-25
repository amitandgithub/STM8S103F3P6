


#ifndef CmdSvr_h
#define CmdSvr_h

#include"system.h"

typedef enum
{
  MODULE_ID_I2C,
  MODULE_ID_SPI,
  
  
  MODULE_ID_MAX, 
}ModuleID_t;


typedef enum
{
  CMD_STATUS_OK,
  CMD_STATUS_CRC_ERROR,
  CMD_STATUS_INVALID_MODULEID,
  CMD_STATUS_INVALID_CMDID,  
}CmdStatus_t;

typedef struct
{
  uint8_t ModuleID;
  uint8_t CmdID;
  uint8_t Len;
}CmdHdr_t;

typedef CmdStatus_t (*CmdHandler_t)(CmdHdr_t* CmdHdr);

typedef CmdStatus_t (*CmdResponse_t)(CmdHdr_t* CmdHdr);

typedef struct
{
  CmdHandler_t          CmdHandler;
  CmdResponse_t         CmdResponse;
  uint8_t               ProcessingTime;
}CmdElement_t;

typedef struct
{
  const CmdElement_t * const    pCommands;
  const uint8_t                 TotalCommands;
}CmdTable_t;

uint8_t Cmdsvr_GetCmd(CmdHdr_t *pCmd);

uint8_t Cmdsvr_AuthenticateCmd(CmdHdr_t *pCmd);

uint8_t Cmdsvr_DispatchCmd(CmdHdr_t *pCmd);

uint8_t Cmdsvr_DispatchResponse(CmdHdr_t *pCmd);























#endif