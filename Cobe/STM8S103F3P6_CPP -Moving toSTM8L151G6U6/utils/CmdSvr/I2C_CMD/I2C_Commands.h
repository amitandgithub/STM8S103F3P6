


#include"system.h"
#include"CmdSvr.h"


/* I2C Command Table*/
enum I2C_CdmID
{
  I2C_CMDID_ADD_REQUEST,
  I2C_CMDID_ADD_RESPONSE,
  I2C_CMDID_SUB_REQUEST,
  I2C_CMDID_SUB_RESPONSE,
  I2C_CMDID_MUL_REQUEST,
  I2C_CMDID_MUL_RESPONSE,
  I2C_CMDID_DIV_REQUEST,
  I2C_CMDID_DIV_RESPONSE,
};
struct I2C_Add_Cmd_Response
{
  CmdStatus_t Status;
  uint8_t ModuleId;
  uint8_t CmdId;
  uint8_t Len;
  uint8_t Result;
  uint8_t CRC8;  
};

struct I2C_Add_Cmd_Request
{  
  uint8_t ModuleId;
  uint8_t CmdId;
  uint8_t Len;
  uint8_t Param1;
  uint8_t Param2;
  uint8_t CRC8;  
};

typedef union 
{
  struct I2C_Add_Cmd_Request;
  struct I2C_Add_Cmd_Response;
  
}I2C_Cmds_t;

CmdStatus_t I2C_CmdHandler(Cmd_t* CmdHdr);

CmdStatus_t I2C_Add(Cmd_t *pCmd);
CmdStatus_t I2C_Sub(Cmd_t *pCmd);
CmdStatus_t I2C_Mul(Cmd_t *pCmd);
CmdStatus_t I2C_Div(Cmd_t *pCmd);