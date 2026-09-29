#include "stm32f30x_conf.h"
#include "30010_io.h"
#include "lcd.h"
#include "flash.h"
#include "ADC.h"
#include <string.h>

void initSystemClock64MHz(void);
void printClockFrequencies(void);
void printClockSource(void);
void testPLL(void);

void gpio_pwm_init(void);
//void timer16_pwm_init(void); //PA6
//void timer17_pwm_init(void); //PA7
void timer1_pwm_init(void); //can be used for PC2 and PC3 both easier connection points for the Servos

void initSystemClock64MHz(void)
{
    // Make sure HSI is running
    RCC_HSICmd(ENABLE);

    while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
    {
    }

    // Use HSI while configuring the clock system
    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);

    while (RCC_GetSYSCLKSource() != 0x00)
    {
    }

    // Configure Flash for higher CPU frequency
    FLASH_SetLatency(FLASH_Latency_2);

    // Configure buses
    RCC_HCLKConfig(RCC_SYSCLK_Div1);   // HCLK  = 64 MHz
    RCC_PCLK1Config(RCC_HCLK_Div2);    // PCLK1 = 32 MHz
    RCC_PCLK2Config(RCC_HCLK_Div1);    // PCLK2 = 64 MHz

    // Configure PLL
    RCC_PLLCmd(DISABLE);

    RCC_PLLConfig(
        RCC_PLLSource_HSI_Div2,
        RCC_PLLMul_16
    );

    // Start PLL
    RCC_PLLCmd(ENABLE);

    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    // Switch SYSCLK to PLL
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    while (RCC_GetSYSCLKSource() != 0x08)
    {
    }

    SystemCoreClockUpdate();
}

int main(void)
{

    initSystemClock64MHz();

    uart_init(9600);

    gpio_pwm_init();
    timer1_pwm_init();

    printf("PWM started:\n");
    printf("PC2 -> TIM1_CH3\n");
    printf("PC3 -> TIM1_CH4\n");
    printf("PWM frequency: 10 kHz\n");
    printf("Duty cycle: 50 %%\n");
    /*
    timer16_pwm_init();
    timer17_pwm_init();

    printf("PWM started on PA6 (TIM16_CH1)\n");
    printf("PWM frequency: 10 kHz\n");
    printf("Duty cycle: 50 %%\n");
    */

    ADC_setup_PA();

    init_spi_lcd();

    uint8_t fbuffer[512];
    char line1[32];
    char line2[32];

    while (1)
    {
        // Read potentiometers
        uint16_t adc1 = ADC_measure_PA(1);
        uint16_t adc2 = ADC_measure_PA(2);

        // Convert ADC 0-4095
        // to servo pulse 1000-2000 us
        uint16_t pulse1 =
            1000 + ((uint32_t)adc1 * 1000 / 4095);

        uint16_t pulse2 =
            1000 + ((uint32_t)adc2 * 1000 / 4095);

        // Update servo PWM
        TIM_SetCompare3(TIM1, pulse1);
        TIM_SetCompare4(TIM1, pulse2);

        // Calculate Duty Cycle for LCD
        uint16_t duty1_x10 =
        ((uint32_t)pulse1 * 1000) / 20000;

        uint16_t duty2_x10 =
        ((uint32_t)pulse2 * 1000) / 20000;

        // Clear LCD buffer
        memset(fbuffer, 0x00, sizeof(fbuffer));

        // Create LCD text
        sprintf(
            line1,
            "Servo1: %u us %u.%u%%",
            pulse1,
            duty1_x10 / 10,
            duty1_x10 % 10
        );

        sprintf(
            line2,
            "Servo2: %u us %u.%u%%",
            pulse2,
            duty2_x10 / 10,
            duty2_x10 % 10
        );

        // Write to LCD
        lcd_write_string(line1, fbuffer, 0, 0);
        lcd_write_string(line2, fbuffer, 0, 1);

        lcd_push_buffer(fbuffer);

        printf("Servo1: %u us | Servo2: %u us\n",
            pulse1, pulse2);

        for (volatile uint32_t i = 0; i < 100000; i++)
        {
        }
    }

}

void gpio_pwm_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;


    // Enable clock for GPIOC
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

    // Load default GPIO values
    GPIO_StructInit(&GPIO_InitStructure);

    // Configure PC2 and PC3 identically
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Connect PC2 to TIM1_CH3
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF_2);

    // Connect PC3 to TIM1_CH4
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_2);

    /*GPIO Init code for PA6 and PA7
    // Enable clock for GPIOB
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

    // Load default GPIO values
    GPIO_StructInit(&GPIO_InitStructure);

    // Configure PA6
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Connect PA6 to TIM16 Channel 1
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_1);


    // Configure PA7
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Connect PA7 to TIM17 Channel 1
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_1);
    */    

    /* Debugging from PA7 being a floating signal before init code was written
    // Force PA7 to ordinary LOW output
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOA, GPIO_Pin_7);
    */

}

void timer1_pwm_init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // Enable TIM1 peripheral clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    // TIMER BASE
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

    // 64 MHz / 64 = 1 MHz, 1 timer tick = 1 us
    TIM_TimeBaseStructure.TIM_Prescaler = 63;

    // 20000 us = 20 ms, 20 ms period = 50 Hz
    TIM_TimeBaseStructure.TIM_Period = 19999;

    TIM_TimeBaseStructure.TIM_CounterMode =
        TIM_CounterMode_Up;

    TIM_TimeBaseStructure.TIM_ClockDivision =
        TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    // PWM OUTPUT
    TIM_OCStructInit(&TIM_OCInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;

    // Start servos in center position, 1500 ticks = 1500 us = 1.5 ms
    TIM_OCInitStructure.TIM_Pulse = 1500;

    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    // TIM1 Channel 3 -> PC2
    TIM_OC3Init(TIM1, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

    // TIM1 Channel 4 -> PC3
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);

    // TIM1 requires Main Output Enable
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    // Start TIM1
    TIM_Cmd(TIM1, ENABLE);
}

/*  Init functions for Timer 16 and 17
    void timer16_pwm_init(void)
    {
        TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
        TIM_OCInitTypeDef TIM_OCInitStructure;

        // Enable TIM16 peripheral clock
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);


        //TIMER BASE

        TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

        TIM_TimeBaseStructure.TIM_Prescaler = 24;
        TIM_TimeBaseStructure.TIM_Period = 255;
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
        TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;

        TIM_TimeBaseInit(TIM16, &TIM_TimeBaseStructure);


        // PWM output

        TIM_OCStructInit(&TIM_OCInitStructure);

        TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;

        // 128 / 256 = 50 % duty cycle
        TIM_OCInitStructure.TIM_Pulse = 128;

        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

        // TIM16 only has Channel 1
        TIM_OC1Init(TIM16, &TIM_OCInitStructure);

        // Enable Channel 1 preload
        TIM_OC1PreloadConfig(TIM16, TIM_OCPreload_Enable);


        //TIM16 has a Main Output Enable (MOE).
        //This must be enabled before the PWM appears on PA6.
        TIM_CtrlPWMOutputs(TIM16, ENABLE);

        // Start TIM16
        TIM_Cmd(TIM16, ENABLE);
    }

void timer17_pwm_init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // Enable TIM17 peripheral clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);

    // TIMER BASE
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

    TIM_TimeBaseStructure.TIM_Prescaler = 24;
    TIM_TimeBaseStructure.TIM_Period = 255;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM17, &TIM_TimeBaseStructure);

    // PWM OUTPUT
    TIM_OCStructInit(&TIM_OCInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;

    // 128 / 256 = 50 % duty cycle
    TIM_OCInitStructure.TIM_Pulse = 128;

    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    // TIM17 Channel 1
    TIM_OC1Init(TIM17, &TIM_OCInitStructure);

    // Enable preload
    TIM_OC1PreloadConfig(TIM17, TIM_OCPreload_Enable);

    // Enable main PWM output
    TIM_CtrlPWMOutputs(TIM17, ENABLE);

    // Start TIM17
    TIM_Cmd(TIM17, ENABLE);
}
*/