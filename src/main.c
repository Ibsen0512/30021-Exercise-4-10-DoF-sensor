#include "stm32f30x_conf.h"
#include "30010_io.h"
#include "lcd.h"
#include "flash.h"
#include "ADC.h"
#include <string.h>

int main(void)
{
	uint8_t output;
	int16_t gyroX;
	int16_t gyroY;
	int16_t gyroZ;
	uint8_t data = 0;
	data = READ | 0x0F;
	init_SPI3_9DOF();
	while(1){
		output = SPI3_read_byte(data, GyroAccTemp_SEL);
		readGyroData(&gyroX, &gyroY, &gyroZ);
		for(int i = 0 ; i < 1000000 ; i++){
		}
	}
}
