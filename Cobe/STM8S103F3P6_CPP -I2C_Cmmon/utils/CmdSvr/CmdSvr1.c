


#include "CmdSvr.h"

/* I2C Command Table*/
 const CmdElement_t I2C_Cmds[] = 
{
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0}
}; 

 const CmdTable_t I2C_CmdTable = { I2C_Cmds, sizeof(I2C_Cmds)/sizeof(CmdElement_t) };


/* SPI Command Table*/
 const CmdElement_t SPI_Cmds[] = 
{
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0}
}; 

 const CmdTable_t SPI_CmdTable = { SPI_Cmds, sizeof(SPI_Cmds)/sizeof(CmdElement_t) };

 CmdTable_t CmdTables[MODULE_ID_MAX] = 
{       
  I2C_CmdTable,
  SPI_CmdTable
                                            
};