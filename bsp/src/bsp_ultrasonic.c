#include "bsp_ultrasonic.h"
#include "pin_configuration.h"
#include "bsp_time.h"
#include <stdio.h>
#include "PGA460_USSC.h"
void InitPGA460() //发射驱动信号之后2ms左右之后就基本得到了echo信号
	                //我们的驱动信号是40Khz，看官方的仿真资料，发射14个pulse得到的echo信号有比较大的echo信号
{
	double result;
	initSTM32F1PGA460(0,115200,0);//初始化使用uart通信，baud=115200，串口地址为0，<0-7>,
																//这个串口地址是PGA的地址，记录在它的EEPROM中。
																//在这个函数中读取了这个PGA的地址并设置了这个PGA的UART的地址为我们所需要的地址
	defaultPGA460(2); //设置寄存器的前42个的默认参数
;
	initThresholds(1);//50% 的阈值，对应数据手册18页的Threshold Data Assignment,这个是
										//经过转换后数字信号的比较阈值
	initTVG(2,1);  //设置模拟前端的时变增益，用来提高信噪比

	registerWrite(0x4b,0x80);
	byte burnStat = burnEEPROM();
	if(burnStat == true){printf("EEPROM programmed successfully.");}
	else{printf("EEPROM program failed.");}
		result = runDiagnostics(0,1);
	printf("frequency: %f Khz\n",result);
		result = runDiagnostics(0,2);
	printf("decay preiod: %f us\n",result);

}
