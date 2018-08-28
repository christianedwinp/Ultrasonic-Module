#include "bsp.h"
#include "bsp_delay.h"
#include "bsp_io.h"
#include "bsp_timer.h"
#include "bsp_can.h"
#include "bsp_encoder.h"
#include "bsp_eps.h"
#include "bsp_usart.h"
#include "bsp_ultrasonic.h"
#include "bsp_time.h"
#include "pin_configuration.h"
#include "PGA460_USSC.h"
#include <math.h>

enum MCO_DEBUG_MODE{NONE,SYSCLK,HSI,HSE,PLLCLK_DIV2};

extern uint32_t start1,finish1, delta1, start2,finish2, delta2;
extern uint32_t transmittime;
extern volatile u32 Millis;
extern int rising1,rising2, transmitFlag1, transmitFlag2;
extern short PWMPeriodCnt;
extern unsigned char ReceiveIO;
extern uint16_t kControllerDeadzoneLow;
extern uint16_t kControllerDeadzoneLowLow;
extern uint16_t kControllerDeadzone;
extern 	byte ultraMeasResult[34+3]; 
const uint8_t kEncoderFrequency = 20;
uint16_t gHeartbeatCnt = 0;
//uint16_t IOITDelayCnt = 0;
const uint16_t kHeartbeatMax = 9;
short BumperCnt;
byte BumperState,OneTimeSend;
double objDist[8] ,objWidth[8],TempDis2,TempWidth2 ;
uint16_t TempDis=0,TempWidth=0;
byte objectDetected[8] = {false,false,false,false,false,false,false,false};

unsigned char UltrSendflag = 0x00;
unsigned short BumperIO;

/*  ONLY FOR DEBUG
*   This func is used to configure MCO which can help debug clock
*   The selected Clock will output to PA.8 (Encoder#1+)
*/
void MCO(enum MCO_DEBUG_MODE mode)
{
	GPIO_InitTypeDef        GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //this function is only valid for STM32F0x
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //this function is only valid for STM32F0x
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//this function is only valid for STM32F0x
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	//GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);
	//RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1);  // HSE=8M --> PA.8
	//RCC_MCO1Config(RCC_MCO1Source_PLLCLK,RCC_MCO1Div_4);  // PLL=168M, 168/4=42M -->PA.8
	
	// pick one of the clocks to spew
	switch(mode){
		case NONE:
			RCC_MCOConfig(RCC_MCO_NoClock); 
			break;
		case SYSCLK:
			RCC_MCOConfig(RCC_MCO_SYSCLK); // Put on MCO pin the: System clock selected
			break;
		case HSI:
			RCC_MCOConfig(RCC_MCO_HSI); // Put on MCO pin the: freq. of high-speed internal 16 MHz RC oscillator
			break;
		case HSE:
			RCC_MCOConfig(RCC_MCO_HSE); // Put on MCO pin the: freq. of external crystal i.e 4-26 MHz
			break;
		case PLLCLK_DIV2:
			RCC_MCOConfig(RCC_MCO_PLLCLK_Div2);
			break;
		default:
			RCC_MCOConfig(RCC_MCO_NoClock); // Put on MCO pin the: System clock selected
			break;
	};
}

/**@brief watch dog
 *
 * @param prer
 * @param rlr
 */
void IWDG_Init(u8 prer,u16 rlr) 
{	
 	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //Ê¹ÄÜ¶Ô¼Ä´æÆ÷IWDG_PRºÍIWDG_RLRµÄÐ´²Ù×÷
	IWDG_SetPrescaler(prer);  //ÉèÖÃIWDGÔ¤·ÖÆµÖµ:ÉèÖÃIWDGÔ¤·ÖÆµÖµÎª64
	IWDG_SetReload(rlr);  //ÉèÖÃIWDGÖØ×°ÔØÖµ
	IWDG_ReloadCounter();  //°´ÕÕIWDGÖØ×°ÔØ¼Ä´æÆ÷µÄÖµÖØ×°ÔØIWDG¼ÆÊýÆ÷
	IWDG_Enable();  //Ê¹ÄÜIWDG	
}

RCC_ClocksTypeDef rcc;
uint8_t regaddrread = 0x1f,regaddrwr = 0x02;
short IntervelCnt[2];
int main(void)
{
	int Cnt;
	short uartAddrUpdate = 0;
	int speedSound = 343; // 343 degC at room temperature
	double digitalDelay = 0.00005*343;
	RCC_GetClocksFreq(&rcc);
	Systick_Init();
	BSP_Init();
	BSP_DelayInit();	         		// Init delpy parameters
	BSP_LEDTestInit();       	    // Init LED on board to control rely	
	BSP_BumperIO();
	BSP_UsartInit(115200);     	// Init USART1 for debug, baudrate=115200
	//MCO(NONE);                  // Debug system clock, use it only when you want to check clock
	BSP_CanInit(250);           	// Init CAN1, baudrate=250Kbps
	BSP_EncoderInit();						
	BSP_EpsInit();              	// Init RS485
	InitPGA460();
	
<<<<<<< HEAD
//delay_ms(500);
	BSP_TimerInit(kEncoderFrequency);
	
=======
			
	delay_ms(500);
//	
	BSP_TimerInit(kEncoderFrequency);
//	BSP_Timer6PWM();
>>>>>>> 22a6036e6d1cac7ade49591b56d0063bf6fd7d34
//	IWDG_Init(4, 120);	// 192ms
//	IWDG_ReloadCounter();
	
	// flash out
	BSP_EncoderRead();
	BSP_EncoderRead();
	BSP_EncoderRead();
	printf("all initialized  \n");
	
	while (1)
	{
		//update by timer
		if(gTimerFlag == 1)
		{
			gTimerFlag = 0 ;
			Cnt++;
			Cnt = Cnt%10;
			if((Cnt>5)&&(Cnt<10))
			{
				registerRead(0x1f,0);
				GPIO_SetBits(GPIOB,GPIO_Pin_13);
			
			}else if(Cnt<5){
//				GPIO_ResetBits(GPIOB,GPIO_Pin_13);
				GPIO_ResetBits(TEST_LED_PORT, TEST_LED_1 );	 
			
		  }
		}
					//Ultrasonic Routine
		IntervelCnt[0] = IntervelCnt[1];
		IntervelCnt[1] = 0;
		ultrasonicCmd(0,1,uartAddrUpdate);// run preset 1 (short distance) burst+listen for 1 object
//		ultrasonicCmd(0,1,uartAddrUpdate+4);
		delay_ms(20);//this delay important, it use as interval between pulse burst
		pullUltrasonicMeasResult(false,uartAddrUpdate);      // Pull Ultrasonic Measurement Result
		TempDis = (ultraMeasResult[1]<<8) + ultraMeasResult[2];
		TempWidth = ultraMeasResult[3];
		TempDis2 = (TempDis/2*0.000001*speedSound) - digitalDelay;
		TempWidth2 = TempWidth * 16;
		if((TempDis2>0.15)&(TempDis2<1.2))
		{
			objectDetected[uartAddrUpdate] = true;
		}
		
		if(objectDetected[uartAddrUpdate] == false) //如果短距离检测失败则开启长距离检测程序
		{
			ultrasonicCmd(1,1,uartAddrUpdate);
//			ultrasonicCmd(1,1,uartAddrUpdate+4);
			//this delay important, it use as interval between pulse burst
			delay_ms(35); // maximum record length is 65ms, so delay with margin
			pullUltrasonicMeasResult(false,uartAddrUpdate);
			TempDis = (ultraMeasResult[1]<<8) + ultraMeasResult[2];
			TempWidth = ultraMeasResult[3];
			TempDis2 = (TempDis/2*0.000001*speedSound) - digitalDelay;
			TempWidth2= TempWidth * 16;

			if((TempDis2<11.2)&&(TempDis2>0))
			{
				objectDetected[uartAddrUpdate] = true;
				
			}else if(TempDis2 == 0)
			{
			}else{
				printf("No object!!!\n");
			}
		}
		
		if(objectDetected[uartAddrUpdate] == false)//just value final output objDist and objWigth once when short distance detect fail.
		{
			printf("no project or ultrasonic error\n");
		}else{
			objDist[uartAddrUpdate] = TempDis2;
			objWidth[uartAddrUpdate] = TempWidth2;
		}
		
		uartAddrUpdate ++;
	  uartAddrUpdate =uartAddrUpdate%UltraDevNum;
	  uartAddrUpdate = LIMIT_MAX_MIN(uartAddrUpdate,7,0);
		objectDetected[uartAddrUpdate] =false;
	}
}


void SysTick_Handler()
{

	// time counter
	Millis++;
	IntervelCnt[1]++;
		//bumper 
	BumperIO = GPIO_ReadInputData(GPIOC)&0x0008;
	if(BumperIO == 0x0000)
	{
		BumperCnt++;
	}else {
		BumperState = 0x00;
		BumperCnt = 0;
	}
	if(BumperCnt>20)
	{
		BumperCnt = 0;
		BumperState = 0x01;
	}
	
	if((Millis%100) == 0)
	{
		OneTimeSend = 0x01;
		if(BumperState == 0x00)
			BSP_CanSendBumper(BUMPER_NO_CRASH);
	}
	if((BumperState == 0x01)&&(OneTimeSend == 0x01))
	{
		OneTimeSend = 0x00;
		BSP_CanSendBumper(BUMPER_CRASH);
	}

}

void TIM6_IRQHandler()
{

	if(TIM_GetITStatus(TIM6,TIM_IT_Update) == SET ) 
	{
	

	  }//-11.2us
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update); 
		TIM_ClearFlag(TIM6,TIM_FLAG_Update);
}

