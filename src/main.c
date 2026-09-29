#include "stm32f30x_conf.h"
#include "30010_io.h"
#include "lcd.h"
#include "flash.h"
#include "ADC.h"
#include <string.h>

void init_SPI3_9DOF(){
	 GPIO_InitTypeDef GPIO_InitStructAll;
	 SPI_InitTypeDef SPI_InitStructAll;

	 //enable clocks
	 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

	 //init SPI related pins
	 //PC10 - SCLK
	 //PC11 - MISO
	 //PC12 - MOSI
	 GPIO_StructInit(&GPIO_InitStructAll);
	 GPIO_InitStructAll.GPIO_Mode = GPIO_Mode_AF;
	 GPIO_InitStructAll.GPIO_Speed = GPIO_Speed_10MHz;
	 GPIO_InitStructAll.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	 GPIO_InitStructAll.GPIO_OType = GPIO_OType_PP;
	 GPIO_Init(GPIOC, &GPIO_InitStructAll);

	 //connect pins to AF
	 GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_6);
	 GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_6);
	 GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_6);

	 //init SPI
	 SPI_I2S_DeInit(SPI3);
	 SPI_InitStructAll.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	 SPI_InitStructAll.SPI_Mode = SPI_Mode_Master;
	 SPI_InitStructAll.SPI_DataSize = SPI_DataSize_8b;
	 SPI_InitStructAll.SPI_CPOL = SPI_CPOL_High;
	 SPI_InitStructAll.SPI_CPHA = SPI_CPHA_2Edge;
	 SPI_InitStructAll.SPI_NSS = SPI_NSS_Soft;
	 SPI_InitStructAll.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
	 SPI_InitStructAll.SPI_FirstBit = SPI_FirstBit_MSB;
	 SPI_InitStructAll.SPI_CRCPolynomial = 7;
	 SPI_Init(SPI3, &SPI_InitStructAll);
	 SPI_Cmd(SPI3, ENABLE);

	 //init CS_ pins
	 GPIO_StructInit(&GPIO_InitStructAll);
	 GPIO_InitStructAll.GPIO_Mode = GPIO_Mode_OUT;
	 GPIO_InitStructAll.GPIO_Speed = GPIO_Speed_10MHz;
	 GPIO_InitStructAll.GPIO_Pin = GPIO_Pin_12;
	 GPIO_InitStructAll.GPIO_OType = GPIO_OType_PP;
	 GPIO_InitStructAll.GPIO_PuPd = GPIO_PuPd_NOPULL;
	 GPIO_Init(GPIOA, &GPIO_InitStructAll);

	 GPIO_StructInit(&GPIO_InitStructAll);
	 GPIO_InitStructAll.GPIO_Mode = GPIO_Mode_OUT;
	 GPIO_InitStructAll.GPIO_Speed = GPIO_Speed_10MHz;
	 GPIO_InitStructAll.GPIO_Pin = GPIO_Pin_8;
	 GPIO_InitStructAll.GPIO_OType = GPIO_OType_PP;
	 GPIO_InitStructAll.GPIO_PuPd = GPIO_PuPd_NOPULL;
	 GPIO_Init(GPIOC, &GPIO_InitStructAll);

	 //GPIO_SetBits(GPIOC, GPIO_Pin_8); // CS = 1 - End Transmission
	 GPIO_SetBits(GPIOA, GPIO_Pin_12); // CS = 1
}

void SPI3_transmit_byte(uint8_t data, uint8_t selector){
	int output;
	switch (selector){
		case 0: //Gyre and accelerometer with CS bound to PD2
			GPIO_ResetBits(GPIOA, GPIO_Pin_12); // CS = 0 - Start Transmission
			while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) != SET) {}
			SPI_SendData8(SPI3, data);
			while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) != SET) {}
			while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET){}
			while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY) == SET){}
			while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) != SET) {}
			output = SPI_ReceiveData8(SPI3);

			/*
			while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET){}
			while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY) == SET){}
			*/
			GPIO_SetBits(GPIOA, GPIO_Pin_12); // CS = 1 - End Transmission
			break;

		case 1: //Magnetometer with CS bound to PC8
			GPIO_ResetBits(GPIOC, GPIO_Pin_8); // CS = 0 - Start Transmission
			while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) != SET) { }
			SPI_SendData8(SPI3, data);
			while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) != SET) { }
			SPI_SendData8(SPI3, 0x00);
			while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET) {}
			output = SPI_ReceiveData8(SPI3);
			while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET){}
			while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY) == SET){}
			GPIO_SetBits(GPIOC, GPIO_Pin_8); // CS = 1 - End Transmission
			break;

		return output;

	}
}

int main(void)
{
	uint8_t data = 0;
	data = 0b10000000 | 0x0F;
	init_SPI3_9DOF();
	while(1){
		SPI3_transmit_byte(data, 1);
		for(int i = 0 ; i < 1000000 ; i++){

		}
	}
}
