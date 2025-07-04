/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.

// Reference voltage 5.0 V
// ADC Resolution 12-bit 
// 
 *
/* ========================================
 * Motor Speed Measurement Code
 * ========================================
*/
    #include "project.h"

    uint16 adcresult = 0 ;          // To store the ADC Result
    //uint32 total = 0;               // Total number of samples to average ADC result
    float adcVoltage = 0.0 ;        // Motor input voltage (in Volts)
    float backEMF = 0.0 ;           // Calculated Back EMF
    float Speed_RPM = 0.0 ;         // Speed in RPM
    
    #define ADC_REF_VOLTAGE 4.5     // Reference voltage of ADC (3 AA-pack Batteries)
    #define ADC_RESOLUTION 4095.0   // 12-bit ADC resolution
    #define MOTOR_CONSTANT 0.3466   // Ka of Motor (mV/min^(-1))
    //#define NUM_SAMPLES 4           // Number of samples for averaging ADC result 
    
    int main(void)
    
    {
        
    CyGlobalIntEnable; /* Enable global interrupts. */
        
    LCD_Start();
    ADC_SAR_1_Start() ; // Start ADC
    ADC_SAR_1_Enable() ; // Enable ADC (Just to ensure if Start command is not enough)
    //ADC_SAR_1_StartConvert() ; // Enable ADC to convert Analog to Digital
    CyDelay(500) ; // Delay to ensure proper startup 
    // P12_6_Write(1); // FET ON
    // CyDelay(6000) ; // Delay to allow motor run freely
    // P12_6_Write(0); // Initialize FET (MOSFET OFF)
    
    for (;;)
    {
        
        // Gate signal test
        
        // Step 1: Turn the MOSFET on
        P12_6_Write(1); // P12_6 sets high to turn MOSFET on
        CyDelay(3000); // 5 seconds run
        
        // Step 2: Turn the MOSFET off
        P12_6_Write(0); // P12_6 sets low to turn MOSFET off
        CyDelay(50); // Short delay to stabilize the voltage
        
        // Step 3: ADC sample 
        ADC_SAR_1_StartConvert();
        CyDelay(1); 
        ADC_SAR_1_IsEndConversion(ADC_SAR_1_WAIT_FOR_RESULT);
        ADC_SAR_1_Stop ;
        adcresult = ADC_SAR_1_GetResult16();
        
        #if 0 // Another method to eliminate noise - Averaging over 4 samples
            for (int i = 0; i < NUM_SAMPLES; i++) {
                
            ADC_SAR_1_StartConvert();
            CyDelay(2);
            
            ADC_SAR_1_IsEndConversion(ADC_SAR_1_WAIT_FOR_RESULT);
            
            total += ADC_SAR_1_GetResult16();
            
            }
            
        adcresult = total / NUM_SAMPLES;
        
        #endif 
        
        // Step 4: Measure the motor input voltage
        adcVoltage = ADC_REF_VOLTAGE * ((float)adcresult / ADC_RESOLUTION); // Convert ADC output to voltage
        
        uint16_t adcVoltage_mV = (uint16_t)(adcVoltage * 1000) ;
        
        uint16_t adcVoltage_whole = adcVoltage_mV / 1000 ;
        uint16_t adcVoltage_decimal = adcVoltage_mV % 1000 ; 
        
        backEMF = (ADC_REF_VOLTAGE * 1000) - adcVoltage_mV ; // Calculating Back EMF
            
        Speed_RPM = backEMF / MOTOR_CONSTANT; // Calculate speed in RPM
        
        #if 1
        //Display results on LCD
        LCD_ClearDisplay();
        LCD_Position(0, 0);
        LCD_PrintString("V:");
        LCD_Position(0, 5);
        LCD_PrintNumber(adcVoltage_whole);
        LCD_PrintString(".");
        if (adcVoltage_decimal < 100) LCD_PrintString("0");
        if (adcVoltage_decimal < 10) LCD_PrintString("0");
        LCD_PrintNumber(adcVoltage_decimal);
        LCD_Position(1, 0);
        LCD_PrintString("RPM:");
        LCD_Position(1, 5);
        LCD_PrintNumber((uint16)Speed_RPM);
        #endif
        
        //P12_6_Write(1);        // Turn FET back ON immediately
        //CyDelay(2000); // Wait before restarting the cycle so user can read display
        
    }
    
    }