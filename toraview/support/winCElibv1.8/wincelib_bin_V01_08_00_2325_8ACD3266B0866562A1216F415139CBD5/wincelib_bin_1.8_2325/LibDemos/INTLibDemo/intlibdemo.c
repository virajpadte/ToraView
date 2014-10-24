///
/// @file        intlibdemo.c
/// @copyright   Copyright (c) 2013 Toradex AG
/// @author	     $Author: gap $
/// @version     $Rev: 1455 $ 
/// @date        $Date: 2013-04-05 13:05:12 +0200 (Fr, 05 Apr 2013) $
/// @brief       Program to demonstrate the Interrupt Library\n
///              On Colibri Evaluation Board V3.1 link X21 Pin 11 (SW8) to X11 Pin B40 (SODIMM 135).
///              On Colibri Evaluation Board V2.1 link X21 Pin 1 (S1) to X11 Pin B48 (SODIMM 135). 
///              Asserting the Switch will generate the Interrupt in this demo.
///              /n
///              TIPs for a short interrupt latency:/n
///              - on Colibri Tegra, deactivate the DFS (Variable Frequency/Voltage switching)/n
///              - Consider using an ISR (installable interrupt routine)
/// @target      Colibri PXAxxx/Txx Modules
/// @test        tested on:  PXAxxx/Txx

#include <windows.h>
#include "IntLib.h"
#include "CoProcLib.h"
#include "MapRegLib.h"
#include "GPIOLib.h"

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
	HANDLE	     hEvent;           ///< Handle to the Interrupt Event
	DWORD	     dwEdge;
    DWORD        dwGpio;
	DWORD	     dwIrq;
    DWORD        dwSysIntr;
	PIN_INSTANCE gpio;
    int          returnValue = -1;

    NKDbgPrintfW(L"******************************************\n\r");
    NKDbgPrintfW(L"*********    Program Start  **************\n\r");
    NKDbgPrintfW(L"******************************************\n\r");
 
	//************************************
	// GPIO Specific Section

	// Set SODIMM Pin 135 as GPIO Input
	SetPinAltFn(135, -1, DIR_IN);

	//Get GPIO Number for SODIMM Pin 135
	GetGPIOFromPin(135, FALSE, &gpio);
	dwGpio = gpio.inst1;

	//Get IRQ for this particular GPIO
	dwIrq = GetGPIOIrq(dwGpio);
    if (dwIrq)
    {
	    // Set Edge to trigger
	    // dwEdge=GPIO_LEVEL_HIGH
	    // dwEdge=GPIO_LEVEL_LOW
	    // dwEdge=GPIO_EDGE_RISING | GPIO_EDGE_FALLING; // (rising & falling edge)
	    dwEdge = GPIO_EDGE_RISING;	
    	
	    if (SetGPIOIrqEdge(dwGpio, dwEdge))
        {
	        //************************************
	        // General Interrupt Handling Section	

	        // Create an Event to wait on
	        hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	        if (hEvent != NULL)
            {
	            // Get the SYSINTR that corresponds to dwIrq (Irqs correspond to the bit numbers of the PXAx Interrupt registers)
	            dwSysIntr = RequestSysInterrupt(dwIrq);
	            if (dwSysIntr)
                {
	                // Link our Event with the SYSINTR
	                if (InterruptInitializeCompat(dwSysIntr, hEvent, NULL, 0))
                    {
	                    // NOTE: Normally this will be done in a separate Thread

	                    // Increase the priorty of your interrupt Thread
	                    CeSetThreadPriority(GetCurrentThread(), 200); 
	                    while(1)
	                    {
                            NKDbgPrintfW(L"\n\rWaiting for Interrupt Event1\n\r");
		                    // Wait for Event (Interrupt)
		                    if (WaitForSingleObject(hEvent, INFINITE) == WAIT_OBJECT_0)
		                    {
			                    if (MessageBox(NULL, L"Interrupt Event1 Detected, continue Waiting?", L"Interrupt", MB_YESNO) == IDNO)
                                {
                                    break;
                                }
			                    
                                // NOTE: It's possible that we get the same interrupt twice if the signal is not debounced
			                    InterruptDoneCompat(dwSysIntr);
		                    }
	                    }

	                    //It is very important to deinitalise the interrupt, otherwise it will not work the next time you start the program
	                    InterruptDisableCompat(dwSysIntr);
	                    if (ReleaseSysIntr(dwSysIntr))
                        {
                            returnValue = 0;
                        }
                    }
                }
            }
        }
    }

    if (returnValue != 0)
    {
        MessageBox(NULL, L"An Error occurred!\nDid you release the interrupt correctly?", L"ERROR", MB_OK);
    }

	return(returnValue);
}

