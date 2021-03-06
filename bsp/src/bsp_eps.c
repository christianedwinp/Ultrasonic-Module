/**
  ******************************************************************************
  * @file    bsp_eps.c
  * @author  LIAO Qinghai
  * @version V0.0.0
  * @date    27-April-2018
  * @brief   ------------------------
  *          | PA10  - USART3(Tx)      |
  *          | PA11  - USART3(Rx)      |
  *          | PC12  - RE & DE         |
  *           ------------------------
  *
@verbatim
 ===============================================================================
                        ##### How to use this driver #####
 ===============================================================================
 

@endverbatim
           
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2018 LIAO Qinghai</center></h2>
  *
  ******************************************************************************  
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp_eps.h"
#include "bsp_delay.h"
#include "bsp_time.h"
#include "pin_configuration.h"
#include "math.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define RE			( PCout(5) )
#define DE			( PCout(4) )

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static const uint16_t kEpsSpeedMax = 2000;
//static const
	uint16_t kEpsBias = 300;
static const uint8_t kEpsBufferSize = 64;
static uint8_t gEpsBufferHead = 0;
//static uint8_t gEpsBuffer [kEpsBufferSize];

static const uint8_t kEpsCmdReleaseSize = 8;
static const uint8_t kEpsCmdRelease[kEpsCmdReleaseSize]={0x01,0x06,0x00,0x44,0x00,0x01,0x08, 0x1F};
static const uint8_t kEpsCmdSpeedSize = 8;
static const uint8_t kEpsCmdStop[kEpsCmdSpeedSize]={0x01,0x06,0x00,0x40,0x00,0x00,0x88, 0x1E};
static uint8_t gEpsCmdSpeed[kEpsCmdSpeedSize] ={0x01,0x06,0x00,0x40,0x00,0x00,0x88, 0x1E};
static const uint8_t auchCRCHi[]=  {  
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,  
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,  
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,  
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,  
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,  
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,  
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,  
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,  
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,  
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,  
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,  
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,  
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,  
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,  
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,  
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,  
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,  
0x40  
};  
  
static const uint8_t auchCRCLo[] =  {  
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,  
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,  
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,  
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,  
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,  
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,  
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,  
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,  
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,  
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,  
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,  
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,  
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,  
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,  
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,  
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,  
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,  
0x40  
};

static uint16_t velocity_flag = 0;
uint16_t kControlSpeedMax = 1500;
uint16_t kControllerDeadzoneHigh = 20;
uint16_t kControllerDeadzoneLow = 15;
uint16_t kControllerDeadzoneLowLow = 5;
uint16_t kControllerDeadzone = 10;
float kControllerP = 1.5;
float kControllerI = 0.3;
float kControllerD = 0;
float kControllerDeadzoneHighRatioR = 0.07;
float kControllerDeadzoneHighRatioL = 0.04;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

  //???fputc?? 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//????,??????   
	USART1->DR = (uint8_t) ch;      
	return ch;
}
  
uint16_t ModbusRtuCRC16(uint8_t *updata, uint16_t len)  
{  
  uint8_t uchCRCHi=0xff;  
  uint8_t uchCRCLo=0xff;  
  uint16_t  uindex;  
  while(len--)  
  {  
	uindex=uchCRCHi^*updata++;  
	uchCRCHi=uchCRCLo^auchCRCHi[uindex];  
	uchCRCLo=auchCRCLo[uindex];  
  }  
  return (uchCRCHi<<8|uchCRCLo);  
}  

/**
  * @brief  
  * @param  
  * @note   
  * @retval None.
  */
void BSP_EpsInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);//?????


	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //??????
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);    

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//????
	GPIO_Init(GPIOC, &GPIO_InitStructure);   //???GPIOA

	USART_InitStructure.USART_BaudRate = 115200;				//?????:115200
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;	//??????:8?
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 		//?????:1?
	USART_InitStructure.USART_Parity = USART_Parity_Even ;  		//??????:?
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//?????????:????
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//????????
	
	USART_Init(USART3, &USART_InitStructure);  					//???USART1
	
	USART_Cmd(USART3, ENABLE);

}


void BSP_EpsSendCmd(const uint8_t* ptr, uint16_t len)
{
	uint8_t i = 0;
	RE = 1;
	DE = 1;
	delay_us(10);
	
	for(i=0;i<len;i++)		//循环发送数据
	{		   
		while( (RS485_USART->SR&0x40) == 0 );	//循环发送,直到发送完毕   
		RS485_USART->DR = (uint8_t)ptr[i]; 
	}	 
	while( (RS485_USART->SR&0x40) == 0 );
	gEpsBufferHead = 0;
	
	delay_us(50);
	RE = 0;
	DE = 0;
}

//@TODO: 1. release eps 2. crc 3. send speed cmd
void BSP_EpsSet(int16_t value)
{	
	uint16_t crc = 0;
	
	if(value > 0)
	{
		value += kEpsBias;
	}
	else if(value < 0)
	{
		value -= kEpsBias;
	}
	
	if(value > kEpsSpeedMax)
	{
		value =kEpsSpeedMax;
	}
	else if(value <-kEpsSpeedMax)
	{
		value = -kEpsSpeedMax;
	}
	
	if(value == 0)
	{
		BSP_EpsSendCmd(kEpsCmdStop, kEpsCmdSpeedSize);
		//BSP_EpsRelease();
	}
	else
	{
		gEpsCmdSpeed[4] = value>>8;
		gEpsCmdSpeed[5] = value &0x00ff;
		crc = ModbusRtuCRC16(gEpsCmdSpeed, 6);
		gEpsCmdSpeed[6] = crc>>8;
		gEpsCmdSpeed[7] = crc&0x00ff;
		
		BSP_EpsSendCmd(gEpsCmdSpeed, kEpsCmdSpeedSize);
	}
}

void BSP_EpsRelease(void)
{
	BSP_EpsSendCmd(kEpsCmdRelease, kEpsCmdReleaseSize);
}
/** @brief steering Controller
 *
 * @param target
 * @param current
 * @param velocity
 */
void steeringController(int16_t target, int16_t current, int16_t velocity)
{
	float error = target - current;
	int16_t speed = 0;
	
	
	if( error > kControllerDeadzone || error < -kControllerDeadzone )
	{
		speed = error*kControllerP;
		velocity_flag++;
	}
	else
	{
		kControllerDeadzone = kControllerDeadzoneHigh;
		if(target < -100)
		{
			kControllerDeadzone += fabs(target*kControllerDeadzoneHighRatioR);
		}
		else if(target > 100)
		{
			kControllerDeadzone += fabs(target*kControllerDeadzoneHighRatioL);
		}
	}
	
	if(speed > kControlSpeedMax)
	{
		speed = kControlSpeedMax;
	}
	else if(speed < -kControlSpeedMax)
	{
		speed = -kControlSpeedMax;
	}
		
	BSP_EpsSet(speed);
}

void USART3_IRQHandler(void)
{
//	uint8_t data = 0;
 	if(USART_GetITStatus(RS485_USART, USART_IT_RXNE) != RESET && gEpsBufferHead < kEpsBufferSize) 
	{	 
		//data = USART_ReceiveData(USART3); 
	 	//gEpsBuffer[gEpsBufferHead] = data;	
		//gEpsBufferHead++;
		//while( (USART1->SR&0X40) == 0 );	 
		//USART1->DR = data; 
	}  					
	USART_ClearFlag(RS485_USART, USART_IT_RXNE);
	
} 



/************************ (C) COPYRIGHT LIAO Qinghai *****END OF FILE****/
