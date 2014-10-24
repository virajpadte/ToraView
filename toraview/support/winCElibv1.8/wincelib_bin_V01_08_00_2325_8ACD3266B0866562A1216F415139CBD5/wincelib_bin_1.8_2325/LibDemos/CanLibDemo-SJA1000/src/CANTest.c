/// @file        CANTest.c
/// @copyright   Copyright (c) 2013 Toradex AG
/// @author	     $Author: bis $
/// @version     $Rev: 1718 $ 
/// @date        $Date: 2013-06-28 11:39:53 +0200 (Fr, 28 Jun 2013) $
/// @brief       Demo of SJA12000 CAN on Colibri Evaluation Board V2.1/n
///              This program uses an ISR in CANLibISRDLL
/// @target      Colibri PXAxxx/Txx Modules
/// @test        tested on:  PXAxxx/Txx

#include <windows.h> 
#include "CoProcLib.h"
#include "GPIOLib.h"
#include "CanLib.h"

#define TESTMESSAGES 1000

#define CAN_BASE_PXA3xx	0x14000000
#define CAN_BASE_PXA270	0x04000000
#define CAN_BASE_TEG2 	0xD0000000
#define CAN_BASE_TEG3   0x48000000

typedef BOOL (*PFNCeSetProcessAffinity) (HANDLE, DWORD);
PFNCeSetProcessAffinity pCeSetProcessAffinity;

static BYTE messageIndex = 0;

static CAN_MESSAGE_EXT msg;
static CAN_MESSAGE_EXT receiveMsg[TESTMESSAGES];

//******************************************************************************
/// Setup the CAN Message structure with a test message./n
/// Updates the global msg
/// @retval       NA
void setMsg()
{
	msg.frametype = STANDARD;
	msg.id = 0x410;

	//msg.frametype = EXTENDED;
	//msg.id = 0x100E8000;

	msg.rtr = FALSE;
	msg.length = 8;

	msg.data[0] = messageIndex++;
	msg.data[1] = 'T';
	msg.data[2] = 'O';
	msg.data[3] = 'R';
	msg.data[4] = 'A';
	msg.data[5] = 'D';
	msg.data[6] = 'E';
	msg.data[7] = 'X';
}

//******************************************************************************
/// Executes the transmit test
/// @param[in]    device    CAN Device ID      
/// @retval       NA
void TransmitTest(DWORD device)
{	
	DWORD i;
	DWORD numMessages = TESTMESSAGES;
	DWORD canStatus;
	DWORD t1, t2;

	printf("Start Transmit Test\n\r");

    t1 = GetTickCount();
	for (i = 0; i < numMessages; i++)
    {
	    setMsg();
		if (!CanTransmitMessage(device, &msg))
        {
			printf("Could not transmit data\n\r");
			canStatus = GetCanStatus(device);
			printf("CAN Status: 0x%x\n\r",canStatus);
		}
		//PrintMessage(&msg);
	}
	t2 = GetTickCount();
	printf("End message transmit. %d ms \n\r", t2 - t1);
}

//******************************************************************************
/// Executes the receive test
/// @param[in]    device    CAN Device ID      
/// @retval       NA
void ReceiveTest(DWORD device)
{
	DWORD ret;
    DWORD messageCount;
	DWORD numMessages = TESTMESSAGES;
    DWORD goodCount = 0;

	printf("Start Receive Test\n\r");
	fflush(stdin);

    setMsg();

	ret = CANReceiveMessages(device, receiveMsg, numMessages, 2000);

	while (numMessages > ret)
    {
		printf("%d messages received             \r", ret);
        receiveMsg[0].data[0] = 0;
		messageCount = CANReceiveMessages(device, receiveMsg, numMessages - ret, 2000);
        if (messageCount == 0)
        {
            printf("No message received, resetting...\r");
            CANReInitialise(device);
            ret = 0;
            goodCount = 0;
        }
        ret += messageCount;
	}

	printf("******* %d Messages Received ******** \n\r", numMessages);
	
	//for (i = 0; i < ret; i++){
	//	PrintMessage(&receiveMsg[i]);
	//}
}

//******************************************************************************
/// Main Application Function
/// @param[in]    hInstance      Handle to the current program instance
/// @param[in]    hPrevInstance  Handle to the previous program instance
/// @param[in]    lpCmdLine      Pointer to the command line
/// @param[in]    nCmdShow       Window show state
/// @retval       0 = Success, -1 = error 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    int returnValue = 0;
    DWORD procId;
	DWORD canAddress = -1;
	DWORD interruptGPIO = -1;
    DWORD device = 0;
	HANDLE processHandle;
	HMODULE hMod;

    DWORD verMaj, verMin, build;
    CANGetLibVersion(&verMaj, &verMin, &build);
	printf("Start CAN Test using CanLib V%d.%d.%d \n\r",verMaj, verMin, build);
	printf("***************************************** \n\r");
	printf("Press Enter to close the program\n\r");
    procId = getPROCID();
	switch(procId)
    {
		case TYPE_PXA270:
			canAddress = CAN_BASE_PXA270;
			interruptGPIO = 52;
			break;

		case TYPE_PXA320:
			canAddress = CAN_BASE_PXA3xx;
			interruptGPIO = 76;
			break;

		case TYPE_PXA310:
            canAddress = CAN_BASE_PXA3xx;
            interruptGPIO = 96; // this gpio is not the default interrupt gpio of the evalboard V2.1.
                                // to use the CAN-Lib on a PXA310 you have to connect X12 pin 31 with X12 pin 47 on the EvalBoard.
			break;

		case TYPE_PXA300:
			canAddress = CAN_BASE_PXA3xx;
			interruptGPIO = 78;
			break;

		case TYPE_TEGRA2:
        case TYPE_TEGRA3:
            if (procId == TYPE_TEGRA2)
            {
                canAddress = CAN_BASE_TEG2;
                interruptGPIO = TEGRA_GPIONUM('a', 0);
            }
            if (procId == TYPE_TEGRA3)
            {
                canAddress = CAN_BASE_TEG3;
                interruptGPIO = TEGRA_GPIONUM('s', 0);
            }
			

			// Set process affinity in case we are running on WEC 7
			// to prevent conflicts caused by the multi-core environment
			hMod = LoadLibrary(L"coredll.dll");
			pCeSetProcessAffinity = (PFNCeSetProcessAffinity) GetProcAddress(hMod, L"CeSetProcessAffinity");
			if (pCeSetProcessAffinity)
			{
			    processHandle = GetCurrentProcess();		
				pCeSetProcessAffinity(processHandle, 1);	
			}
			break;

		default:
			NKDbgPrintfW(L"CAN Demo: Unknown Processor type");
			returnValue = -1;
            break;
	}

    if (returnValue == 0) 
    {
        // Initalize CAN Lib, Watermark = 100, Buffer Size = 10000
	    if (0 != InitSJA1000(device, canAddress, interruptGPIO, 1, 1000))
	    { 
            printf("InitSJA1000() failed (Is CANLibISRDll.dll on the device?)\r\n");
            returnValue = -1;
            getchar();
        }
        else
        {
            CanWriteDescriptor(device, 0x80000000, 0xffffffff);	  // Receive all messages
	        //CanWriteDescriptor(device, 0x80000000, 0x0fff000f); // Receive only standard messages with id 1000XXXXXXX
	        //CanWriteDescriptor(device, 0x82000000, 0x0000000f); // Receive only standard messages with id 10000010XXX
	        //CanWriteDescriptor(device, 0x80740000, 0x00030000); // Receive only extended messages with id 10000000011 101XX XXXXXXXXXXXXX
        	
	        //CanSetBaudrate(device, 125);							//Set baud to 125k
	        CanSetBaudrate(device, 1000);							//Set baud to 1M
        	
	        fflush(stdin);

            // To Test Receive Mode then remove the comment from this line
	        ReceiveTest(device);

            // To Test Transmit Mode then remove the comment from this line
	        TransmitTest(device);
	        getchar();
        	
	        DeInitSJA1000(device);
        }
    }
    return(returnValue);
}

