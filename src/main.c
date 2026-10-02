#include "stm32f30x_conf.h"
#include "30010_io.h"
#include "lcd.h"
#include "flash.h"
#include "ADC.h"
#include <string.h>
#include "LSM9DS1.h"

int main(void)
{

    uint8_t whoAmI;
	int16_t gyroX;
	int16_t gyroY;
	int16_t gyroZ;
    int16_t temperature;
    int16_t magX;
    int16_t magY;
    int16_t magZ;
	// uint8_t data = 0;
    uint8_t output;
    uint8_t dummy = 0;
	uint8_t data = READ | 0x0F;
    uint8_t gyroadr = WRITE | 0x10;
    uint8_t gyro_on = 0x30;
    uint8_t magadr = WRITE | 0x20;
    uint8_t mag_on = 0xF8;

    // Initialize UART for printf()
    uart_init(9600);

	init_SPI3_9DOF();

    printf("SPI3 initialized\n\r");

    // Print important SPI registers
    printf("SPI3->CR1 = 0x%04X\n\r", SPI3->CR1);
    printf("SPI3->CR2 = 0x%04X\n\r", SPI3->CR2);
    printf("SPI3->SR  = 0x%04X\n\r", SPI3->SR);

    // Check CS pins
    printf("PD2 CS Gyro/Acc = %d\n\r",
           GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_2));

    printf("PC8 CS Magnetometer = %d\n\r",
           GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_8));
    
     printf("About to read WHO_AM_I...\n\r");
    
    output = SPI3_read_byte(data, GyroAccTemp_SEL);
    // Read WHO_AM_I from gyro/accelerometer
    whoAmI = SPI3_read_byte(READ | 0x0F, GyroAccTemp_SEL);

    printf("Returned from SPI3_read_byte()\n\r");
    printf("WHO_AM_I = 0x%02X\n\r", output);

    // SPI Gyro Init:
    /*
        GPIO_ResetBits(GPIOD, GPIO_Pin_2); // CS = 0 - Start Transmission
			while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) != SET) { }
			SPI_SendData8(SPI3, gyroadr);
            while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET){}
            dummy = SPI_ReceiveData8(SPI3);
			while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) != SET) { }
			SPI_SendData8(SPI3, gyro_on); //Dummy data to keep clock running for read action
			while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET) {}
			output = SPI_ReceiveData8(SPI3);
			while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET){}
			while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY) == SET){}
			GPIO_SetBits(GPIOD, GPIO_Pin_2); // CS = 1 - End Transmission
    */


    // SPI Mag Init:
    
        GPIO_ResetBits(GPIOC, GPIO_Pin_8); // CS = 0 - Start Transmission
			while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) != SET) { }
			SPI_SendData8(SPI3, magadr);
            while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET){}
            dummy = SPI_ReceiveData8(SPI3);
			while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) != SET) { }
			SPI_SendData8(SPI3, mag_on); //Dummy data to keep clock running for read action
			while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET) {}
			output = SPI_ReceiveData8(SPI3);
			while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET){}
			while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY) == SET){}
			GPIO_SetBits(GPIOC, GPIO_Pin_8); // CS = 1 - End Transmission


	while (1)
    {
        
        //readGyroData(&gyroX, &gyroY, &gyroZ);  
        //readMagData(&magX, &magY, &magZ);  
        readTempData(&temperature);      
        //output = SPI3_read_byte(data, GyroAccTemp_SEL);
		for(int i = 0 ; i < 1000000 ; i++){
		}
        readMagData(&magX, &magY, &magZ);  


        printf("Gyro X: %6d | Y: %6d | Z: %6d | Temp: %6d | Mag X: %6d | Y: %6d | Z: %6d |\n\r",
            gyroX, gyroY, gyroZ, 
            (int)temperature, magX, magY, magZ);

            // Mag X: %6d | Y: %6d | Z: %6d |
            //,            magX, magY, magZ

        printf(" 0x%02X \n", whoAmI);
	}
    }

