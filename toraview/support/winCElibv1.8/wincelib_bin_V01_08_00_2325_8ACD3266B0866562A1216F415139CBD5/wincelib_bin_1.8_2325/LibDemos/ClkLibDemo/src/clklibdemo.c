/// @file         clklibdemo.c
/// @copyright    Copyright (c) 2013 Toradex AG
/// $Author: gap $
/// $Revision: 1489 $
/// $Date: 2013-04-16 16:48:09 +0200 (Di, 16 Apr 2013) $
/// @brief        Program to test the ClkLib TegraSetClkFrequency Function
/// @target       Colibri T20 Modules
/// @test         Tested on:  T20
/// @caveats      None

#include <windows.h> 
#include "clklib.h"

//*****************************************************************************
/// Main function
/// @param[in]  hInstance       Handle to program instance
/// @param[in]  hPrevInstance   Handle to previous program instance
/// @param[in]  lpCmdLine       Command line string pointer
/// @param[in]  nCmdShow        Window state
/// @retval     0               Always returned
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPWSTR lpCmdLine,
                   int nCmdShow)
{
    DWORD VerMaj = 0;
    DWORD VerMin = 0;
    DWORD Build = 0;

    DWORD actualFreq = 0;
    NKDbgPrintfW(L"******************************************\n\r");
    NKDbgPrintfW(L"*********    Program Start  **************\n\r");
    NKDbgPrintfW(L"******************************************\n\r");

    InitClkLib();

    printf("Precision Tests that should pass:\n\n");

    actualFreq = ClkLibSetClockFrequency(SLINK_SPI4_CLK_TEG, 0, 50000000, ClockOptimiseForPrecision);
    printf("25MHz +/-25MHz, Precision Test: Result=%d, %d\n\n", actualFreq, ClkLibGetClockFrequency(SLINK_SPI4_CLK_TEG));

    actualFreq = ClkLibSetClockFrequency(SLINK_SPI4_CLK_TEG, 54000000, 56000000, ClockOptimiseForPrecision); 
    printf("55MHz +/-1MHz, Precision Test: Result=%d, %d\n\n", actualFreq, ClkLibGetClockFrequency(SLINK_SPI4_CLK_TEG));

    actualFreq = ClkLibSetClockFrequency(SLINK_SPI4_CLK_TEG, 10000000, 9600000, ClockOptimiseForPrecision); 
    printf("9.8MHz +/-0.2Mhz, min>max, Precision Test: Result=%d, %d\n\n", actualFreq, ClkLibGetClockFrequency(SLINK_SPI4_CLK_TEG));

    actualFreq = ClkLibSetClockFrequency(SLINK_SPI4_CLK_TEG, 999999, 1000001, ClockOptimiseForPrecision); 
    printf("1MHz +/-1Hz, Precision Test: Result=%d, %d\n\n", actualFreq, ClkLibGetClockFrequency(SLINK_SPI4_CLK_TEG));

    actualFreq = ClkLibSetClockFrequency(SLINK_SPI4_CLK_TEG, 1000000, 1000000, ClockOptimiseForPrecision); 
    printf("1MHz +/-0Hz, Precision Test: Result=%d, %d\n\n", actualFreq, ClkLibGetClockFrequency(SLINK_SPI4_CLK_TEG));

    actualFreq = ClkLibSetClockFrequency(SLINK_SPI4_CLK_TEG, 116000, 120000, ClockOptimiseForPrecision); 
    printf("118KHz +/-2KHz, Precision Test: Result=%d, %d\n\n", actualFreq, ClkLibGetClockFrequency(SLINK_SPI4_CLK_TEG));

    actualFreq = ClkLibSetClockFrequency(SLINK_SPI4_CLK_TEG, 101000, 103000, ClockOptimiseForPrecision); 
    printf("102KHz +/-1KHz, Precision Test: Result=%d, %d\n\n", actualFreq, ClkLibGetClockFrequency(SLINK_SPI4_CLK_TEG));

    printf("Precision Tests that should fail:\n\n");

    actualFreq = ClkLibSetClockFrequency(SLINK_SPI4_CLK_TEG, 9800000, 9800000, ClockOptimiseForPrecision); 
    printf("9.8MHz +/-0Hz, Precision Test: Result=%d, %d\n\n", actualFreq, ClkLibGetClockFrequency(SLINK_SPI4_CLK_TEG));

    actualFreq = ClkLibSetClockFrequency(SLINK_SPI4_CLK_TEG, 0, 50000, ClockOptimiseForPrecision); 
    printf("25KHz +/-25KHz, Precision Test: Result=%d, %d\n\n", actualFreq, ClkLibGetClockFrequency(SLINK_SPI4_CLK_TEG));

    printf("Power Consumption Tests that should pass:\n\n");

    actualFreq = ClkLibSetClockFrequency(SLINK_SPI4_CLK_TEG, 1900000, 2100000, ClockOptimiseForPower); 
    printf("2MHz +/-0.1MHz, Power Consumption Test: Result=%d, %d\n\n", actualFreq, ClkLibGetClockFrequency(SLINK_SPI4_CLK_TEG));

    actualFreq = ClkLibSetClockFrequency(SLINK_SPI4_CLK_TEG, 1800000, 2200000, ClockOptimiseForPower); 
    printf("2MHz +/-0.2MHz, Power Consumption Test: Result=%d, %d\n\n", actualFreq, ClkLibGetClockFrequency(SLINK_SPI4_CLK_TEG));

    actualFreq = ClkLibSetClockFrequency(SLINK_SPI4_CLK_TEG, 101000, 103000, ClockOptimiseForPower); 
    printf("102KHz +/-1KHz, Power Consumption Test: Result=%d, %d\n\n", actualFreq, ClkLibGetClockFrequency(SLINK_SPI4_CLK_TEG));

    printf("Power Consumption Tests that should fail:\n\n");

    actualFreq = ClkLibSetClockFrequency(SLINK_SPI4_CLK_TEG, 5100000, 5100000, ClockOptimiseForPower); 
    printf("5.1MHz +/-0Hz, Power Consumption Test: Result=%d, %d\n\n", actualFreq, ClkLibGetClockFrequency(SLINK_SPI4_CLK_TEG));

    actualFreq = ClkLibSetClockFrequency(SLINK_SPI4_CLK_TEG, 0, 50000, ClockOptimiseForPower); 
    printf("25KHz +/-25KHz, Power Consumption Test: Result=%d, %d\n\n", actualFreq, ClkLibGetClockFrequency(SLINK_SPI4_CLK_TEG));

    printf("Press any key to exit\r\n\r\n");
    getchar();

    DeInitClkLib();
}
