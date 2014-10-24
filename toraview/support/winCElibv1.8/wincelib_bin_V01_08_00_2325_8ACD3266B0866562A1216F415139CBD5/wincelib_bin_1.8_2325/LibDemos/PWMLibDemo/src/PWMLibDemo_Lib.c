///
/// @file        PWMLibDemo_Lib.c
/// @copyright   Copyright (c) 2013 Toradex AG
/// @author      $Author: kia $
/// @version     $Rev: 1817 $ 
/// @date        $Date: 2013-09-20 20:51:32 +0200 (Fr, 20 Sep 2013) $
/// @brief       PWM Demo Source Code
/// 
/// @target      PXAxxx, Tegra 
///

#include <windows.h> 
#include "gpioLib.h"
#include "clklib.h" // only to read PWM base frequency
#include "PWMLib.h"
#include "CoProcLib.h"

static DWORD freqBase = 0;

//*****************************************************************************
/// Main function
/// @param[in]  instance       Handle to program instance
/// @param[in]  prevInstance   Handle to previous program instance
/// @param[in]  cmdLine        Command line string pointer
/// @param[in]  cmdShow        Window state
/// @retval     1              Always returned
int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow)
{
    DWORD moduleFamily = getModuleFamily();
    DWORD processorID = getPROCID();

    // GPIO Initialisation
    switch (moduleFamily)
    {
        case MODULE_FAMILY_COLIBRI:

            switch (processorID)
            {
                case TYPE_PXA270:
                    SetGPIOAltFn(16, 2, DIR_OUT); // PWM0 on GPIO16            
                    SetGPIOAltFn(17, 2, DIR_OUT); // PWM1 on GPIO17            
                    SetGPIOAltFn(11, 2, DIR_OUT); // PWM2 on GPIO11            
                    SetGPIOAltFn(12, 2, DIR_OUT); // PWM3 on GPIO12
                    break;
                
                case TYPE_PXA320:
                    SetGPIOAltFn(11,  1, DIR_OUT); // PWM0 on GPIO11            
                    SetGPIOAltFn(12,  1, DIR_OUT); // PWM1 on GPIO12
                    SetGPIOAltFn(128, 0, DIR_IN);  // Set GPIO0_2 to input (this pin is multiplexed with GPIO12)
                    SetGPIOAltFn(13,  1, DIR_OUT); // PWM2 on GPIO13            
                    SetGPIOAltFn(14,  1, DIR_OUT); // PWM3 on GPIO14
                    break;

                case TYPE_PXA300:
                    SetGPIOAltFn(17,  1, DIR_OUT); // PWM0 on GPIO17
                    SetGPIOAltFn(18,  1, DIR_OUT); // PWM1 on GPIO18
                    SetGPIOAltFn(126, 0, DIR_IN);  // Tristate GPIO46 which is multiplexed with GPIO20
                    SetGPIOAltFn(19,  1, DIR_OUT); // PWM2 on GPIO19            
                    SetGPIOAltFn(20,  1, DIR_OUT); // PWM3 on GPIO20
                    SetGPIOAltFn(46,  0, DIR_IN);  // Tristate GPIO46 which is multiplexed with GPIO20
                    break;
                    
                case TYPE_TEGRA2:
                case TYPE_TEGRA3:
                    freqBase = ClkLibGetClockFrequency(PWM_CLK_TEG); // PWM controller input frequency in [Hz]
                    SetGPIOAltFn(TEGRA_GPIONUM('l', 5), -1, DIR_IN);    // Set to GPIO Input because multiplexed with GPIO B4
                    SetGPIOAltFn(TEGRA_GPIONUM('b', 4),  0, DIR_OUT);   // PWM0 on B.04 in Pin Muxing
                    SetGPIOAltFn(TEGRA_GPIONUM('b', 5),  0, DIR_OUT);   // PWM1 on B.05 in Pin Muxing
                    SetGPIOAltFn(TEGRA_GPIONUM('a', 6),  1, DIR_OUT);   // PWM2 on A.06 in Pin Muxing
                    SetGPIOAltFn(TEGRA_GPIONUM('l', 4), -1, DIR_IN);    // Set to GPIO Input because multiplexed with GPIO B7
                    SetGPIOAltFn(TEGRA_GPIONUM('a', 7),  1, DIR_OUT);   // PWM3 on A.07 in Pin Muxing
                    break;

                default:
                    printf("This CPU is not supported");
                    return -1;
                    break;
            }
            break;

        case MODULE_FAMILY_APALIS:
            switch (getPROCID())
            {
                case TYPE_TEGRA3:
                    SetGPIOAltFn(TEGRA_GPIONUM('u', 6),  0, DIR_OUT);   // PWM1 on U.06 in Pin Muxing
                    SetGPIOAltFn(TEGRA_GPIONUM('u', 4),  0, DIR_OUT);   // PWM2 on U.04 in Pin Muxing
                    SetGPIOAltFn(TEGRA_GPIONUM('u', 5),  0, DIR_OUT);   // PWM3 on U.05 in Pin Muxing
                    SetGPIOAltFn(TEGRA_GPIONUM('u', 3),  0, DIR_OUT);   // PWM4 on U.03 in Pin Muxing
                    break;

                default:
                    printf("This CPU is not supported");
                    return -1;
                    break;
            }
            break;

        default:
            printf("This CPU is not supported");
            return -1;
            break;
    }

    // API Usage
    switch (moduleFamily)
    {
        case MODULE_FAMILY_COLIBRI:

            switch (processorID)
            {
                case TYPE_PXA270:
                case TYPE_PXA320:
                case TYPE_PXA300:
                    InitPWM(COLIBRI_PWM1, 1,     2);  // Fastest possible frequency        
                    InitPWM(COLIBRI_PWM2, 1,   100);  // PWM1 is on the Colibri PXA320 and PXA300 at the same Pin as DATA17 on the Colibri PXA270!
                    InitPWM(COLIBRI_PWM3, 16, 1024); 
                    InitPWM(COLIBRI_PWM4, 64, 1024);  // Slowest possible frequency on Colibri PXA
                    break;
                    
                case TYPE_TEGRA2:
                case TYPE_TEGRA3:
                    InitPWM(COLIBRI_PWM1,    1, 0);                 // Fastest possible frequency  
                    printf("f_PWM1 %dHz\r\n", freqBase /256 /1);

                    InitPWM(COLIBRI_PWM2,    1, 0);  
                    printf("f_PWM2 %dHz\r\n", freqBase /256 /1);

                    InitPWM(COLIBRI_PWM3,   1875, 0); 
                    printf("f_PWM3 %dHz\r\n", freqBase /256 /1875);

                    InitPWM(COLIBRI_PWM4, 8192, 0);                  // Slowest possible frequency
                    printf("f_PWM4 %dHz\r\n", freqBase /256 /8192);
                    break;
            }
            
            SetPWMDutyPercentage(COLIBRI_PWM1, 10); // Set to 10% of period
            SetPWMDutyPercentage(COLIBRI_PWM2, 25); // Set to 25% of period
            SetPWMDutyPercentage(COLIBRI_PWM3, 50); // Set to 50% of period
            SetPWMDutyPercentage(COLIBRI_PWM4, 75); // Set to 75% of period
            break;

        case MODULE_FAMILY_APALIS:
            switch (getPROCID())
            {
                case TYPE_TEGRA3:
                    InitPWM(APALIS_PWM1,    1, 0);  // Fastest possible frequency        
                    InitPWM(APALIS_PWM2,    1, 0);  
                    InitPWM(APALIS_PWM3,   16, 0); 
                    InitPWM(APALIS_PWM4, 8192, 0);  // Slowest possible frequency
                    break;
            }
            SetPWMDutyPercentage(APALIS_PWM1, 10); // Set to 10% of period
            SetPWMDutyPercentage(APALIS_PWM2, 25); // Set to 25% of period
            SetPWMDutyPercentage(APALIS_PWM3, 50); // Set to 50% of period
            SetPWMDutyPercentage(APALIS_PWM4, 75); // Set to 75% of period
            break;
    }

    // If higher precision is required you can use the following function, which set the duty cycle in clocks instead of a percentage
    // Please note that for the Colibri Tegra the maximum is fixed at 256;
    /*
        SetPWMDuty(COLIBRI_PWM1, 1);   // Set to 50% of period(2) for PXAxxx
        SetPWMDuty(COLIBRI_PWM2, 24);  // Set to 25% of period(100) for PXAxxx
        SetPWMDuty(COLIBRI_PWM3, 512); // Set to 50% of period(1024) for PXAxxx
        SetPWMDuty(COLIBRI_PWM4, 768); // Set to 75% of period(1024) for PXAxxx
    */

    Sleep(10000);  // Pause 10 seconds

    DeInitPWM(0);
    DeInitPWM(1);
    DeInitPWM(2);
    DeInitPWM(3);

    DeInitGPIOLib();

    return(TRUE);
}

