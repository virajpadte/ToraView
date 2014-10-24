/// @file         cantest.c
/// @copyright Copyright (c) 2012 Toradex AG
///               [Software License Agreement]
/// $Author: kia $
/// $Rev: 2323 $
/// $Date: 2014-07-09 15:29:10 +0200 (Mi, 09 Jul 2014) $
/// @brief   Contains CAN test demo functions definition. This program uses ISR (CANLibISRDll.dll).\n
///\n
/// This program demonstrates MCP2515 CAN working.\n
/// Disable DFC via ColibriMonitor or Registry.\n
///\n
/// Test setup:\n
/// Take two Colibri Eval. Board 3.1 and connect using CAN bus at X2.\n
/// To make CAN bus, do the following:\n
/// -. Take two different colored (i.e. Red and Green) wires of length around 40 cm and twist them.\n
/// -. Terminate both ends of twisted pair with 120 Ohm resistor.\n
/// -. Solder Red and Green wire to pin 2 and pin 7 of Female D-Type connectors respectively in both side\n
/// -. Connect Female D-Type connector to X2 of respective Colibri Eval. boards.\n
/// \n
/// On Evaluation Board 3.1, SPI Lines 
/// SODIMM_73(CAN_INT) , SODIMM_86(SSP_FRM), SODIMM_88(SSP_SCLK), SODIMM_90(SSP_RXD), SODIMM_92(SSP_TXD)
/// are directly connected with MCP2515 CAN controller.\n\n
/// So you must ensure that jumpers are fixed and these lines are not used for other purposes.\n\n
/// Caution: You must run receive in first device before starting transmit in other device.\n
/// Note: 
/// To use the CAN-Lib on a PXA310 you have to connect X12 pin 41 with X12 pin 31 on the Colibri EvalBoard 3.0.
/// \n\n
/// @target      Colibri modules\n
/// Tested on Colibri PXAxxx  on Windows Embedded CE 6.0
/// @caveats

#include <windows.h>
#include "CoProcLib.h"
#include "GPIOLib.h"
#include "canlib-MCP2515.h"
#include "cantest.h"

static BYTE sampleData = 0; ///< Used to fill incremented data into message buffer

canMessageExt message;  ///< Keeps message structure contains
canMessageExt receiveMessage[NUMBER_OF_TEST_MESSAGES]; /// Array of CAN message structure

//******************************************************************************

static void ConfigureTestMessage(DWORD n)
{
    /// Configuration for message type (Standard)
     //message.frameType = STANDARD;
     //message.id = 0x410;

    /// Configuration for message type (Extended)
    message.frameType = EXTENDED;
    message.id = 1 << n;

    message.isRemoteTransmitRequest = FALSE;
    message.length = CAN_MESSAGE_LENGTH;

    message.data[0] = sampleData++;
    message.data[1] = sampleData++;
    message.data[2] = sampleData++;
    message.data[3] = sampleData++;
    message.data[4] = sampleData++;
    message.data[5] = sampleData++;
    message.data[6] = sampleData++;
    message.data[7] = sampleData++;
}

//******************************************************************************

/// Transmits the CAN message
/// @param[in]    device      Instance number
static void TransmitTest(DWORD device)
{
    DWORD count; ///< Keeps counter values
    DWORD messages = NUMBER_OF_TEST_MESSAGES; ///< Keeps number of messages to transmit
    //DWORD messages = 1; ///< Keeps number of messages to transmit
    DWORD canStatus;    ///< Keeps CAN status
    DWORD tickCount1, tickCount2; ///< Keeps tick counts

    printf("Start Transmit Test\n\r");
    tickCount1 = GetTickCount();
    for (count = 0; count < 29; count++)
    {
        ConfigureTestMessage(count);
        if (!CANLibMCP2515TransmitMessage(device, &message, 1000))
        {
            printf("Could not transmit data\n\r");
            canStatus = CANLibMCP2515GetStatus(device);
            printf("CAN Status: 0x%x\n\r", canStatus);
        }
    }
    tickCount2 = GetTickCount();

    printf("End message transmit. %d ms \n\r", (tickCount2 - tickCount1));
}

//******************************************************************************
/// @internal
/// Receives CAN messages from other CAN interface
/// @param[in]    device      Instance number
static void ReceiveTest(DWORD device)
{
    DWORD messagesReceived = 0;
    DWORD totalMessageReceived = 0; ///< Keeps updated value of received messages.
    DWORD messages = NUMBER_OF_TEST_MESSAGES; ///< Number of messages to receive or transmit
    DWORD count = 0; ///< Keeps counter values

    printf("Start Receive Test\n\r");
    fflush(stdin);

    totalMessageReceived = CANLibMCP2515ReceiveMessages(device, receiveMessage, messages, RECEIVE_TIMEOUT);
    while (messages > totalMessageReceived)
    {
        printf("\nTimeout! Only %d messages received \n\r", totalMessageReceived);
        messagesReceived = CANLibMCP2515ReceiveMessages(device, &receiveMessage[totalMessageReceived], (messages - totalMessageReceived), RECEIVE_TIMEOUT);
        totalMessageReceived += messagesReceived;
        if (messagesReceived == 0)
        {
            count++;
            if (count == 3)
            {
                break;
            }
        }
    }
    printf("\n******* %d  Messages received ******** \n\r", totalMessageReceived);

    for (count = 0; count < totalMessageReceived; count++)
    {
        CANLibMCP2515PrintMessage(&receiveMessage[count]);
    }
}

//******************************************************************************

/// Gets Interupt Number and Processor Type
/// @param[in]    device           CAN port 
///                                0 or 1 for Apalis
///                                always 0 for Colibri
/// @param[out]   gpio             gpio which is used as MCP2515_INT# signal
/// @param[out]   procType         current processor ID (see coproclib, getPROCID() )
/// @retval       0         Successful exit
static BOOL GetInterruptGpio(DWORD device, DWORD *gpio, DWORD *procType)
{
    BOOL successStatus = TRUE;
    PIN_INSTANCE         IntGpio;
    *procType = getPROCID();

	if(getModuleFamily()==MODULE_FAMILY_APALIS)
    {
			*gpio = TEGRA_GPIONUM('w', 2)+device;  //device 0 -> GPIO W2, device 1 ->GPIO W3
	}
	else
    {
		switch (*procType)		//device is ignored
		{
			case TYPE_PXA270: 
			case TYPE_PXA320:
			case TYPE_PXA300:
			case TYPE_TEGRA2:
			case TYPE_TEGRA3:
                GetGPIOFromPin(73, FALSE, &IntGpio); // CANINT# signal is on SODIMM pin 73
                *gpio = IntGpio.inst1;
                break;
            case TYPE_PXA310: 
                GetGPIOFromPin(133, FALSE, &IntGpio); // for PXA310, CANINT# signal is on SODIMM pin 133!
                *gpio = IntGpio.inst1;
                break;
	        
			default:            
				NKDbgPrintfW(L"CAN Demo: Unknown Processor type");
				successStatus = FALSE;
		}
	}

    return(successStatus);
}

//******************************************************************************

/// prompts user for CAN Baud Rate
/// @param[in]    canBaudRate  Pointer to canBaudRate variable
/// @retval       none
static void GetBaudRate(UINT32 *canBaudRate)
{
    BOOL isSelectionDone = FALSE; ///< Keeps current state of option selected
    char selectedOption[100];     ///< keeps selected option
    UINT32 stringLength = 0;      ///< Selected option's string length

    fflush(stdin);

    printf("\n\nSelect baudrate(in kbps) for CAN:\n\n");
    printf("A. 10\t B. 20\t C. 50\t D. 100\t\nE. 125\t F. 250\t G. 500\t H. 750\t\nI. 800\t J.1000\t\n\n");

    do
    {
        printf("\nChoose the option and press Enter key: ");
        scanf_s("%s", &selectedOption);

        printf("\n");
        stringLength = strlen(selectedOption);

        /// Enters only if there is only one character has entered from key board.
        if (stringLength == 1)
        {
            switch (selectedOption[0])
            {
            case 'a':
            case 'A':
                *canBaudRate = 10;       ///< Sets baudrate to 10 Kbps
                isSelectionDone = TRUE;
                break;  

            case 'b':
            case 'B':
                *canBaudRate = 20;       ///< Sets baudrate to 20 Kbps
                isSelectionDone = TRUE;
                break;

            case 'c':
            case 'C':
                *canBaudRate = 50;       ///< Sets baudrate to 50 Kbps
                isSelectionDone = TRUE;
                break;

            case 'd':
            case 'D':
                *canBaudRate = 100;      ///< Sets baudrate to 100 Kbps
                isSelectionDone = TRUE;
                break;  

            case 'e':
            case 'E':
                *canBaudRate = 125;      ///< Sets baudrate to 125 Kbps
                isSelectionDone = TRUE;
                break;

            case 'f':
            case 'F':
                *canBaudRate = 250;      ///< Sets baudrate to 250 Kbps
                isSelectionDone = TRUE;
                break;

            case 'g':
            case 'G':
                *canBaudRate = 500;      ///< Sets baudrate to 500 Kbps
                isSelectionDone = TRUE;
                break;

            case 'h':
            case 'H':
                *canBaudRate = 750;      ///< Sets baudrate to 750 Kbps
                isSelectionDone = TRUE;
                break;

            case 'i':
            case 'I':
                *canBaudRate = 800;      ///< Sets baudrate to 800 Kbps
                isSelectionDone = TRUE;
                break;

            case 'j':
            case 'J':
                *canBaudRate = 1000;     ///< Sets baudrate to 1000 Kbps
                isSelectionDone = TRUE;
                break;

            default:
                printf("Invalid entry, try again!\n");
                isSelectionDone = FALSE;
            }
        }
        else
        {
            printf("Invalid entry, try again!\n");
        }

    } while (!isSelectionDone);

    isSelectionDone = FALSE;

    printf("\nCAN baudrate - %d kbps\n\n", *canBaudRate);
}

//******************************************************************************

/// prompts user for test mode (TX or RX) and runs the test
/// @param[in]    device  CAN device instance number
/// @retval       none
static void GetMode(DWORD device)
{
    BOOL isSelectionDone = FALSE; ///< Keeps current state of option selected
    char selectedOption[100];     ///< keeps selected option
    UINT32 stringLength = 0;      ///< Selected option's string length

    do
    {
        printf("\nSelect the mode:\n");
        printf("R. Receive  T. Transmit  D. Dump Regs.  I. Interrupts Clear  Q. Quit\n\n");
        printf("Choose the option and press Enter key: ");
        scanf_s("%s", &selectedOption);
        printf("\n");
        stringLength = strlen(selectedOption);
        /// Enter only if there is only one character entered from key board.
        if (stringLength == 1)
        {
            switch (selectedOption[0])
            {
            case 'r':
            case 'R':
                /// Calls receive test
                ReceiveTest(device);
                break;

            case 't':
            case 'T':
                /// Calls transmit test
                TransmitTest(device);
                break;

            case 'q':
            case 'Q':
                /// Quits the demo
                isSelectionDone = TRUE;
                break;

            case 'd':
            case 'D':
                /// Dump the registers
                DumpMcp2515(device);
                break;

            case 'i':
            case 'I':
                /// Reset Interrupt Flags
                CANLibMCP2515ResetInterruptFlags(device);
                break;

            default:
                printf("Invalid entry, try again!\n\n");
                isSelectionDone = FALSE;
            }
        }
        else
        {
            printf("\nInvalid entry, try again!\n\n");
        }

    } while (!isSelectionDone);
}

//******************************************************************************

/// Entry point for a CANLibDemo application
/// @param[in]    hInstance         Handle to current instance
/// @param[in]    hPrevInstance     Handle to previous instance
/// @param[in]    lpCmdLine         Pointer to command line
/// @param[in]    nCmdShow          Show state of window
/// @retval       0                 Successful exit
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    DWORD gpioInterruptNumber = 0;      ///< Interrupt GPIO
    DWORD device = 0;                   ///< Instance number
    DWORD versionMajor, versionMinor, buildNumber;  ///< Major, minor, build number
    DWORD procType = 0;                 ///< Processer type
    UINT32 canBaudRate = 0;             ///< Keeps CAN baud rate
    BOOL successStatus = TRUE;          ///< Keeps success status
    DWORD selectedDevice = 0 ;          ///< Select the CAN1/2 in Apalis, In colibri it is always CAN (i.e device=0).
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    CANLibMCP2515GetLibVersion(&versionMajor, &versionMinor, &buildNumber);

    printf("Start CAN Test using CanLib V%d.%d.%d \n\r", versionMajor, versionMinor, buildNumber);
    printf("***************************************** \n\r");
    
    fflush(stdin);
    // choosing Apalis CAN0 or CAN1
    if(getModuleFamily()==MODULE_FAMILY_APALIS)
    {
        printf("Select CAN you want to use:\n");
        printf("1 ---->  CAN1(Upper Connector) \n"  );  
        printf("2 ---->  CAN2(Bottom Connector) \n"  );
        printf("Choose the option(1/2) and press enter:");
        scanf_s("%d", &selectedDevice);
        printf("\n");
        switch (selectedDevice){
            case 1:
                device = 0;       ///< choose the CAN 1
                break;  
            case 2:
                device = 1;       ///< choose the CAN 2
                break;
            default:
                printf("Wrong entry, selecting CAN1 !\n");
                device = 0; 
            }
         }
    else
    {
            device = 0;    // In case of Colibri
    }


    successStatus = GetInterruptGpio(device, &gpioInterruptNumber, &procType);

    if (successStatus)
    {
        /// Initalizes CAN Chip
        if (0 != CANLibMCP2515Init(device, gpioInterruptNumber, WATER_MARK, BUFFER_SIZE, 0))
        {
            printf("InitMcp2515() failed (See serial debug output for details)\r\n");
            successStatus = FALSE;
        }
    }
    if (successStatus)
    {
        successStatus = CANLibMCP2515WriteDescriptor(device, CAN_DESCRIPTOR, CAN_MASK);
    }

    if (successStatus)
    {
        GetBaudRate(&canBaudRate);
    }

    if (successStatus)
    {

        successStatus = CANLibMCP2515SetBaudrate(device, canBaudRate);
    }
    if (successStatus)
    {
       GetMode(device);
    }
    fflush(stdin);

    if (!successStatus)
    {
        printf("\nError!!!\n");
    }

    printf("\nPress Enter key to exit!\n\n");

    getchar();

    /// De initialises CAN Chip
    CANLibMCP2515DeInit(device);

    return 0;
}
