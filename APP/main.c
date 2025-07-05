#include "gd32f30x.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "mb.h"
#include "mbport.h"
#include "mbutils.h"

//Input Registers start address
#define REG_INPUT_START       0x0000
//Input Registers number
#define REG_INPUT_NREGS       8
//Holding Registers start address
#define REG_HOLDING_START     0x0000
//Holding Registers number
#define REG_HOLDING_NREGS     8

//...please search in the package
#define REG_COILS_START       0x0000

#define REG_COILS_SIZE        16
/*
#define REG_COILS_START1      0x1000

#define REG_COILS_SIZE        16
*/

#define REG_DISCRETE_START    0x0000

#define REG_DISCRETE_SIZE     16


uint16_t usRegInputBuf[REG_INPUT_NREGS] = {0x1000,0x1001,0x1002,0x1003,0x1004,0x1005,0x1006,0x1007};

uint16_t usRegInputStart = REG_INPUT_START;


uint16_t usRegHoldingBuf[REG_HOLDING_NREGS] = {0x147b,0x3f8e,0x147b,0x400e,0x1eb8,0x4055,0x147b,0x408e};

uint16_t usRegHoldingStart = REG_HOLDING_START;


uint8_t ucRegCoilsBuf[REG_COILS_SIZE / 8] = {0x01,0x02};

uint8_t ucRegDiscreteBuf[REG_DISCRETE_SIZE / 8] = {0x01,0x02};


uint16_t last_pwm_frequency = 0;//compare with pwm_frequency

void gpio_config(void);



void
EnterCriticalSection( void )
{
  __disable_irq();
}

void
ExitCriticalSection( void )
{
  __enable_irq();
}

/*!
    \brief      configure the GPIO ports
    \param[in]  none 
    \param[out] none
    \retval     none
*/

void gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
	  rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

    /*Configure PA0 PA1 PA2 as alternate function*/
	gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10);
	gpio_init(GPIOB,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
	
}

/*!
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/


int main(void)
{
	
	systick_config();

uint16_t paramA    = usRegHoldingBuf[0]; // 40001 [4x0000]
uint16_t paramB    = usRegHoldingBuf[1]; // 40002 [4x0001]
uint16_t paramC    = usRegHoldingBuf[2]; // 40003 [4x0002]
uint16_t phaseB    = usRegHoldingBuf[3]; // 40004 [4x0003]
uint16_t phaseC    = usRegHoldingBuf[4]; // 40005 [4x0004]
uint16_t pwm_freq  = usRegHoldingBuf[6]; // 40007 [4x0006]
uint16_t mix_count = usRegHoldingBuf[7]; // 40008 [4x0007]

uint8_t switchA = (ucRegCoilsBuf[0] & 0x01) ? 1 : 0; // 10001 [1x0000]
uint8_t switchB = (ucRegCoilsBuf[0] & 0x02) ? 1 : 0; // 10002 [1x0001]
uint8_t switchC = (ucRegCoilsBuf[0] & 0x04) ? 1 : 0; // 10003 [1x0002]
	
	__disable_irq();
	eMBInit(MB_RTU, 0x01, 0x01, 9600, MB_PAR_NONE); 
	/* Enable the Modbus Protocol Stack. */
	eMBEnable();
   
	
   for( ;; )
    {
        ( void )eMBPoll(  );
			
		for(int i = 0;i <= mix_count;i++)
		{
			
			
						;	
		}
		}
}



    /* ------------------------------------ INPUT REGISTER ----------------------------------------*/
eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( UCHAR )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( UCHAR )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

    /* ------------------------------------ HOIDING REGISTER ----------------------------------------*/

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
    {
        /* error state */
        eMBErrorCode    eStatus = MB_ENOERR;
        /* offset */
        int16_t iRegIndex;

        /* test if the reg is in the range */
        if (((int16_t)usAddress-1 >= REG_HOLDING_START) 
            && (usAddress-1 + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS))
        {
            /* compute the reg's offset */
            iRegIndex = (int16_t)(usAddress-1 - REG_HOLDING_START);
            switch (eMode)
            {
                case MB_REG_READ:
                    while (usNRegs > 0)
                    {
                        *pucRegBuffer++ = (uint8_t)( usRegHoldingBuf[iRegIndex] >> 8 );
                        *pucRegBuffer++ = (uint8_t)( usRegHoldingBuf[iRegIndex] & 0xff);
                        iRegIndex ++;
                        usNRegs --;
                    }
                    break;
                case MB_REG_WRITE:
                    while (usNRegs > 0)
                    {
                        usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                        usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
											
                        iRegIndex ++;
                        usNRegs --;
                    }
                    break;

            }
        }
        else{
            eStatus = MB_ENOREG;
        }

        return eStatus;
    }

    /* ------------------------------------ COTLS REGISTER ----------------------------------------*/
		
eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
              eMBRegisterMode eMode )
{

  eMBErrorCode eStatus = MB_ENOERR;

  int16_t iNCoils = ( int16_t )usNCoils;

  int16_t usBitOffset;

  if( ( (int16_t)usAddress >= REG_COILS_START ) &&
     ( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )
  {

    usBitOffset = ( int16_t )( usAddress - REG_COILS_START );
    switch ( eMode )
    {
    
    case MB_REG_READ:
      while( iNCoils > 0 )
      {
        *pucRegBuffer++ = xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,
                                         ( uint8_t )( iNCoils > 8 ? 8 : iNCoils ) );
        iNCoils -= 8;
        usBitOffset += 8;
      }
      break;
      
 
    case MB_REG_WRITE:
      while( iNCoils > 0 )
      {
        xMBUtilSetBits( ucRegCoilsBuf, usBitOffset,
                       ( uint8_t )( iNCoils > 8 ? 8 : iNCoils ),
                       *pucRegBuffer++ );
        iNCoils -= 8;
      }
      break;
    }
    
  }
  else
  {
    eStatus = MB_ENOREG;
  }
  return eStatus;
}

    /* ------------------------------------ DISCRETE REGISTER ----------------------------------------*/
eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
  
  eMBErrorCode eStatus = MB_ENOERR;

  int16_t iNDiscrete = ( int16_t )usNDiscrete;

  uint16_t usBitOffset;
  

  if( ( (int16_t)usAddress >= REG_DISCRETE_START ) &&
     ( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE ) )
  {

    usBitOffset = ( uint16_t )( usAddress - REG_DISCRETE_START );
    
    while( iNDiscrete > 0 )
    {
      *pucRegBuffer++ = xMBUtilGetBits( ucRegDiscreteBuf, usBitOffset,
                                       ( uint8_t)( iNDiscrete > 8 ? 8 : iNDiscrete ) );
      iNDiscrete -= 8;
      usBitOffset += 8;
    }
    
  }
  else
  {
    eStatus = MB_ENOREG;
  }
  return eStatus;
}



