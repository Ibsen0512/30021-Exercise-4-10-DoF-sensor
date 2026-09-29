#include "ADC.h"

void ADC_setup_PA(void)
{
    // ------------------------------------------------
    // 1. Configure ADC clock
    // ------------------------------------------------

    // ADC clock: PLL 64 MHz / 8 = 8 MHz
    RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div8);


    // ------------------------------------------------
    // 2. Enable ADC peripheral clock
    // ------------------------------------------------

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);


    // ------------------------------------------------
    // 3. Configure PA0 and PA1 as analog inputs
    // ------------------------------------------------

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_StructInit(&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =
        GPIO_Pin_0 | GPIO_Pin_1;

    GPIO_InitStructure.GPIO_Mode =
        GPIO_Mode_AN;

    GPIO_Init(GPIOA, &GPIO_InitStructure);


    // ------------------------------------------------
    // 4. Configure ADC1
    // ------------------------------------------------

    ADC_InitTypeDef ADC_InitStructure;

    ADC_StructInit(&ADC_InitStructure);

    ADC_InitStructure.ADC_Resolution =
        ADC_Resolution_12b;

    ADC_InitStructure.ADC_ContinuousConvMode =
        DISABLE;

    ADC_InitStructure.ADC_ExternalTrigEventEdge =
        ADC_ExternalTrigEventEdge_None;

    ADC_InitStructure.ADC_DataAlign =
        ADC_DataAlign_Right;

    ADC_InitStructure.ADC_NbrOfRegChannel =
        1;

    ADC_Init(ADC1, &ADC_InitStructure);


    // ------------------------------------------------
    // 5. Enable ADC voltage regulator
    // ------------------------------------------------

    ADC_VoltageRegulatorCmd(ADC1, ENABLE);

    /*
     * IMPORTANT:
     * Real delay. "volatile" prevents compiler
     * from optimizing the loop away.
     */
    for(volatile uint32_t i = 0; i < 10000; i++)
    {
        __NOP();
    }


    // ------------------------------------------------
    // 6. Calibrate ADC
    // ------------------------------------------------

    ADC_SelectCalibrationMode(
        ADC1,
        ADC_CalibrationMode_Single
    );

    ADC_StartCalibration(ADC1);

    // Timeout prevents permanent lock-up
    uint32_t calibration_timeout = 1000000;

    while((ADC_GetCalibrationStatus(ADC1) == SET) &&
          (calibration_timeout > 0))
    {
        calibration_timeout--;
    }

    if(calibration_timeout == 0)
    {
        printf("ERROR: ADC calibration timeout\n");
        return;
    }


    // Short delay after calibration
    for(volatile uint32_t i = 0; i < 1000; i++)
    {
        __NOP();
    }


    // ------------------------------------------------
    // 7. Enable ADC1
    // ------------------------------------------------

    // Clear old ready flag
    ADC_ClearFlag(ADC1, ADC_FLAG_RDY);

    ADC_Cmd(ADC1, ENABLE);

    // Timeout prevents permanent lock-up
    uint32_t ready_timeout = 1000000;

    while((ADC_GetFlagStatus(ADC1, ADC_FLAG_RDY) == RESET) &&
          (ready_timeout > 0))
    {
        ready_timeout--;
    }

    if(ready_timeout == 0)
    {
        printf("ERROR: ADC ready timeout\n");
        return;
    }

    printf("ADC setup complete\n");
}   


uint16_t ADC_measure_PA(uint8_t ch)
{
    // Select ADC channel
    if(ch == 1)
    {
        // PA0 -> ADC channel 1
        ADC_RegularChannelConfig(
            ADC1,
            ADC_Channel_1,
            1,
            ADC_SampleTime_1Cycles5
        );
    }
    else if(ch == 2)
    {
        // PA1 -> ADC channel 2
        ADC_RegularChannelConfig(
            ADC1,
            ADC_Channel_2,
            1,
            ADC_SampleTime_1Cycles5
        );
    }
    else
    {
        // Invalid channel
        return 0;
    }

    // Clear old EOC flag before starting a new conversion
    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);

    // Start conversion
    ADC_StartConversion(ADC1);

    // Wait for conversion to complete
    uint32_t timeout = 1000000;

    while((ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET) &&
          (timeout > 0))
    {
        timeout--;
    }

    // ADC did not finish
    if(timeout == 0)
    {
        return 0;
    }

    // Read and return ADC result
    return ADC_GetConversionValue(ADC1);

    /* ADC Debugging code for both PA6 and PA7
    uint16_t value = ADC_GetConversionValue(ADC1);

    printf("ADC result = %u\n", value);

    return value;

    printf("ADC configured\n");

    printf("ADC ISR before start: 0x%08lX\n", ADC1->ISR);
    printf("ADC CR before start:  0x%08lX\n", ADC1->CR);

    printf("Conversion started\n");

    printf("ADC ISR after start:  0x%08lX\n", ADC1->ISR);
    printf("ADC CR after start:   0x%08lX\n", ADC1->CR);

    if(timeout == 0)
    {
        printf("ERROR: ADC EOC timeout!\n");

        printf("ADC ISR timeout: 0x%08lX\n", ADC1->ISR);
        printf("ADC CR timeout:  0x%08lX\n", ADC1->CR);

        return 0;
    }
    


    // Perform ADC conversion
    // Start ADC read
    ADC_StartConversion(ADC1);

    // Wait until conversion is complete
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0)
    {
    }

    // Read ADC value
    uint16_t x = ADC_GetConversionValue(ADC1);

    return x;
    */
}