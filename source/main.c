/*
The MIT License (MIT)

Copyright (c) 2014 ARMstrap Community

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include "main.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_i2c.h"
#include "misc.h"
#include <math.h>



int main()
{
    setup();

    int SAMPLE[120];
    generateArray(SAMPLE);
    int i = 0;
    while (1)
    {
    	if(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE))
    		{
    			SPI_I2S_SendData(SPI3, SAMPLE[i]);
    			i++;
    			if(i>=120) i = 0;
    		}
        //loop();
    }
    return 0; // never reached
}

void generateArray(int* AUDIO_SAMPLE){
	//*IF YOU CHANGE THESE PARAMETERS, YOU MUST ALSO CHANGE THE ARRAY SIZE OF SAMPLES*//
	int Amp = 10000; //amplitude of wave (within 16bit signed integer)
	int SR = 48000; //sample rate of I2S channel
	int freq = 400; //frequency of sine wave you want to produce
	int i;
	for(i=0; i <= (2*SR/freq); i++){ //mult SR/Freq by 2 b/c of two channels
		if(i%2)
			AUDIO_SAMPLE[i] = Amp*sin(2*3.14*(i/(2.0*SR/freq))); //right channel
		else
			AUDIO_SAMPLE[i]=0; //left channel
	}
}

void setup()
{
	initialize_led();
    initialize_button();
    initialize_ext_interrupt();
    initialize_I2S();
}

void loop()
{

}

void initialize_led()
{
	// On startup, all peripheral clocks are disabled.  Before using a GPIO
	// pin, its peripheral clock must be enabled.
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    // For the ARMstrap Eagle board, the USER LED is hooked-up to GPIOC Pin 1.
    // You can see this by looking at the freely available schematics on armstrap.org
    GPIO_InitTypeDef ledGPIO;
    GPIO_StructInit(&ledGPIO);
    ledGPIO.GPIO_Mode = GPIO_Mode_OUT;
    ledGPIO.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOC, &ledGPIO);
}

void initialize_button()
{
	// On startup, all peripheral clocks are disabled.  Before using a GPIO
	// pin, its peripheral clock must be enabled.
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    // For the ARMstrap Eagle board, the USER Button is hooked-up to GPIOC Pin 0.
    // You can see this by looking at the freely available schematics on armstrap.org
    GPIO_InitTypeDef buttonGPIO;
	GPIO_StructInit(&buttonGPIO);
	buttonGPIO.GPIO_Mode = GPIO_Mode_IN;
	buttonGPIO.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOC, &buttonGPIO);
}

void initialize_ext_interrupt()
{
    EXTI_InitTypeDef   EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource0);

    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void initialize_I2S()
{
//Enable peripheral clocks
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	RCC_PLLI2SCmd(ENABLE);

//Configure GPIOs for SCLK, SD, MCLK, WS
	GPIO_InitTypeDef PinInitStruct;
	PinInitStruct.GPIO_Mode=GPIO_Mode_AF;
	PinInitStruct.GPIO_OType=GPIO_OType_PP;
	PinInitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
	PinInitStruct.GPIO_Speed=GPIO_Speed_50MHz;

	PinInitStruct.GPIO_Pin = I2S3_SCLK_PIN | I2S3_SD_PIN | I2S3_MCLK_PIN;
	GPIO_Init(GPIOC, &PinInitStruct);

	PinInitStruct.GPIO_Pin = I2S3_WS_PIN;
	GPIO_Init(GPIOA, &PinInitStruct);

	//prepare output ports for alternate function
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_SPI3); //I2S3_WS
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_SPI3); //I2S3_MCK
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);//I2S3_SCK
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);//I2S3_SD

//Configure I2S peripheral
	I2S_InitTypeDef I2S_InitType;
	SPI_I2S_DeInit(SPI3);
	I2S_InitType.I2S_AudioFreq = I2S_AudioFreq_48k;
	I2S_InitType.I2S_MCLKOutput = I2S_MCLKOutput_Enable;
	I2S_InitType.I2S_DataFormat = I2S_DataFormat_16b;
	I2S_InitType.I2S_Mode = I2S_Mode_MasterTx;
	I2S_InitType.I2S_Standard = I2S_Standard_Phillips;
	I2S_InitType.I2S_CPOL = I2S_CPOL_Low;

	I2S_Init(SPI3, &I2S_InitType);
	I2S_Cmd(SPI3, ENABLE);
}



void delay(uint32_t ms)
{
    ms *= 3360;
    while(ms--) {
        __NOP();
    }
}

void EXTI0_IRQHandler()
{
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
        GPIO_ToggleBits(GPIOC, GPIO_Pin_1);
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

