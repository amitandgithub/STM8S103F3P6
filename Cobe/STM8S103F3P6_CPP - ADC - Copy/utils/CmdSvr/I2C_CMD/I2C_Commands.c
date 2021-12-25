


#include"I2C_Commands.h"


uint8_t I2c_Cmd_response[sizeof(I2C_Add_Cmd_Response)+5];


CmdStatus_t I2C_CmdHandler(Cmd_t* pCmd)
{
  struct I2C_Add_Cmd_Response* ReponseData = (struct I2C_Add_Cmd_Response*)I2c_Cmd_response;
  struct I2C_Add_Cmd_Request* RequestData =  (struct I2C_Add_Cmd_Request*)pCmd->Request;
  
   pCmd->Response = (RspHdr_t*)I2c_Cmd_response;
   
  switch(pCmd->Request->CmdID)
  {
  case I2C_CMDID_ADD_REQUEST:    
    ReponseData->CmdId  = I2C_CMDID_ADD_RESPONSE;
    ReponseData->Len    = 1;
    ReponseData->Result = RequestData->Param1 + RequestData->Param2;
    break;
  case I2C_CMDID_SUB_REQUEST:    
    ReponseData->CmdId  = I2C_CMDID_ADD_RESPONSE;
    ReponseData->Len    = 1;
    ReponseData->Result = RequestData->Param1 - RequestData->Param2;
    break;
  case I2C_CMDID_MUL_REQUEST:
    ReponseData->CmdId  = I2C_CMDID_ADD_RESPONSE;
    ReponseData->Len    = 1;
    ReponseData->Result = RequestData->Param1 * RequestData->Param2;
    break;
  case I2C_CMDID_DIV_REQUEST: 
    ReponseData->CmdId  = I2C_CMDID_ADD_RESPONSE;
    ReponseData->Len    = 1;
    ReponseData->Result = RequestData->Param1 / RequestData->Param2;
    pCmd->Response = 0;
    break;
  default: 
    return CMD_STATUS_INVALID_CMDID;
    break;  
  }  
 
  return CMD_STATUS_OK;
}


