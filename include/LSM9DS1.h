#ifndef LSM9DS1_H_
#define LSM9DS1_H_

#include "stm32f30x_conf.h" // STM32 config
#include "30010_io.h" 		// Input/output library for this course

#define READ 0b10000000
#define WRITE 0b00000000
#define GyroAccTemp_SEL 0
#define Magnetometer_SEL 1

void init_SPI3_9DOF();
uint8_t SPI3_read_byte(uint8_t data, uint8_t selector);
void readGyroData(int16_t *out_x, int16_t *out_y, int16_t *out_z);
void readAccData(int16_t *out_x, int16_t *out_y, int16_t *out_z);
void readTempData(int16_t *out_temp);
void readMagData(int16_t *out_x, int16_t *out_y, int16_t *out_z);

#endif /* LSM9DS1_H_ */
