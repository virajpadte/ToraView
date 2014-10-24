///
/// @file         spilibdemo_lib.c
/// @copyright    Copyright (c) 2013 Toradex AG
/// $Author: gap $
/// $Revision: 1757 $
/// $Date: 2013-08-12 13:20:58 +0200 (Mo, 12 Aug 2013) $
/// @brief        Demo Programme for SPILib\n
///\n
///  For the T20 connect SPI4 (default) with SPI3\n
///  For other modules please consult the chart below and select SPI ports as required
///\n
///  On Evaluation Board 2.1\n
///    upper PlugArea 33 with upper PlugArea 3\n
///    upper PlugArea 32 with upper PlugArea 2\n
///    upper PlugArea 35 with lower PlugArea 20\n
///    upper PlugArea 34 with lower PlugArea 25\n
///\n
///  On Evaluation Board 3.1\n
///     Jumper(SODIMM)\n
///     SPI4=master              SPI3=slave\n
///     CLK   X10-33(88)    -    X10-28(30)\n
///     CS0   X10-32(86)    -    X10-27(28)\n
///     MOSI  X10-35(92)    -    X8-5  (55)\n
///     MISO  X10-34(90)    -    X8-6  (63)\n
///\n
///            |         T20          |         T30          |       PXA270         |       PXA300         |       PXA310         |       PXA320        \n
///  Function  | GPIO SoDIMM Eval 3.1 | GPIO SoDIMM Eval 3.1 | GPIO SoDIMM Eval 3.1 | GPIO SoDIMM Eval 3.1 | GPIO SoDIMM Eval 3.1 | GPIO SoDIMM Eval 3.1\n
///            |                      |                      |                      |                      |                      |                     \n
///  SPI1 CLK  | X5/1   100   X9-7    | Y2/0   88    X10-33  | 23/2    88   X10-33  | 85/1    88   X10-33  | 85/1    88   X10-33  | 83/1    88   X10-33 \n
///  SPI1 MOSI | T2/3    69   X22-21  | Y0/0   92    X10-35  | 25/2    92   X10-35  | 88/6    92   X10-35  | 88/6    92   X10-35  | 86/6    92   X10-35 \n
///  SPI1 MISO | T3/3    77   X22-22  | Y1/0   90    X10-34  | 26/1    90   X10-34  | 87/6    90   X10-34  | 87/6    90   X10-34  | 85/6    90   X10-34 \n
///  SPI1 CS0  | X6/1   102   X9-9    | Y3/0   86    X10-32  | 24/2    86   X10-32  | 86/1    86   X10-32  | 86/1    86   X10-32  | 84/1    86   X10-32 \n
///            |                      |                      |                      |                      |                      |                     \n
///  SPI2 CLK  | A7/3    67   X10-30  | X2/1  138    X9-37   | 19/1    55   X9-5    | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.  \n
///  SPI2 MOSI | D0/3    88   X10-33  | X0/1  142    X9-39   | 38/2    37   X10-12  | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.  \n
///  SPI2 MISO | D1/3    90   X10-34  | X1/1  140    X9-38   | 11/2    28   X10-27  | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.  \n
///  SPI2 CS0  | D3/3    86   X10-32  | X3/1  136    X9-35   | 14/2    63   X9-6    | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.  \n
///            |                      |                      |                      |                      |                      |                     \n
///  SPI3 CLK  | A6/3    30   X10-28  | O3/0   25    X10-5   | 52/2    73   X10-31  | 91/1   154   X3-C7   | 91/1   154   X3-C7   | 69/2    66   X9-31  \n
///  SPI3 MOSI | B6/3    55   X9-5    | O1/0   35    X10-11  | 35/3   135   X10-40  | 93/1   158   X3-C8   | 93/1   158   X3-C8   | 71/2    57   X9-33  \n
///  SPI3 MISO | B7/3    63   X9-6    | O2/0   33    X10-10  | 82/1    77   X22-22  | 94/1   160   X3-A9   | 94/1   160   X3-A9   | 72/2    61   X9-34  \n
///  SPI3 CS0  | B5/3    28   X10-27  | O4/0   27    X10-6   | 83/1    79   X22-11  | 92/1   156   X3-A8   | 92/1   156   X3-A8   | 70/2    64   X9-32  \n
///                                   |                      |                      |                      |                      |                    \n
///  SPI4 CLK  | D0/1    88   X10-33  | J7/1   38    X10-17  | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.  \n
///  SPI4 MOSI | D4/1    92   X10-35  | B0/1   36    X10-16  | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.  \n
///  SPI4 MISO | D1/1    90   X10-34  | B1/1   32    X10-13  | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.  \n
///  SPI4 CS0  | D3/1    86   X10-32  | K7/1   34    X10-14  | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.  \n
///            |                      |                      |                      |                      |                      |                     \n
///  SPI5 CLK  | n.a.   n.a.   n.a.   | Z3/2   99    X3-31A  | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.  \n
///  SPI5 MOSI | n.a.   n.a.   n.a.   | B2/2  154    X3-7C   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.  \n
///  SPI5 MISO | n.a.   n.a.   n.a.   | Z2/2  156    X3-8A   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.  \n
///  SPI5 CS0  | n.a.   n.a.   n.a.   | N4/2  160    X3-9A   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.  \n
///            |                      |                      |                      |                      |                      |                     \n
///  SPI6 CLK  | n.a.   n.a.   n.a.   | X2/0  138    X9-37   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.  \n
///  SPI6 MOSI | n.a.   n.a.   n.a.   | X0/0  142    X9-39   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.  \n
///  SPI6 MISO | n.a.   n.a.   n.a.   | X1/0  140    X9-38   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.  \n
///  SPI6 CS0  | n.a.   n.a.   n.a.   | X3/0  136    X9-35   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.   | n.a.   n.a.   n.a.  \n
///\n
/// @target       Colibri PXAxxx, T20 & T30 Modules
/// @test tested on:  PXAxxx, T20, T30

#include <windows.h> 
#include "spiLib.h"

// DATATYPE should be DWORD
#define DATATYPE DWORD
#define DATABITS 8
#define TESTSAMPLES 16

DWORD sbufferTx[TESTSAMPLES];
DWORD sbufferRx[TESTSAMPLES];

DATATYPE *bufferTx;
DATATYPE *bufferRx;

DWORD txSSPport=4; 
DWORD rxSSPport=1;

DWORD TXThread(PVOID p)
{
	DWORD i,y;
	DWORD packets = TESTSAMPLES;
	DWORD transmittedSamples;

	for (i = 0; i < packets; i++)
	{
		bufferTx[i] = (DATATYPE) i;
	}

	for (y = 0; y < 1; y++)
	{
		printf("Transmitting data....\r\n");
        transmittedSamples = SPIWritePolling(txSSPport, bufferTx, packets, 0);
		printf("Data transmitted (%d samples)\r\n", transmittedSamples);
		Sleep(10);
	}

	return 0;
}

DWORD RXThread(PVOID p)
{
	DWORD receivedSamples;
	DWORD packets = TESTSAMPLES;
	DWORD y, i;
    DWORD mask;
	DWORD timeout = 1000;
    DWORD bitwidth = DATABITS;

    for (i = 0; i < packets; i++)
	{
		bufferTx[i] = 0xFF;
	}

	for (y = 0; y < 1; y++)
    {
		memset(bufferRx, 0x00, (packets) *sizeof(DATATYPE));
		printf("Receive begin...\n\r");
        receivedSamples = SPIReadPolling(rxSSPport, bufferRx, packets, 0);
		printf("%d samples received\n\r", receivedSamples);
		
		for (i = 0; i < packets; i++)
        {
            if (bitwidth < 32) 
            {
                mask = (1 << bitwidth) - 1;
            }
            else
            {
                mask = 0xffffffff;
            }
			if ((DATATYPE)(i & mask) != (DATATYPE) (bufferRx[i] & mask))
			{
				printf("Data Test failed at sample: %d\n\r",i);
				return -1;
			}
		}
		printf("Data Test: ok    (%d samples tested, sample width %d bits))\n\r", i, DATABITS);
	}

	return 0;
}

/*----------------------------------------------------------------------
  |       main
+---------------------------------------------------------------------*/

int WINAPI WinMain(HINSTANCE hInstance,      // handle to current instance				   
                   HINSTANCE hPrevInstance,  // handle to previous instance				   
                   LPWSTR lpCmdLine,          // pointer to command line				   
                   int nCmdShow)             // show state of window				   
{	

	HANDLE tRx, tTx;
	BOOL dma_enable = FALSE;

	NKDbgPrintfW(L"******************************************\n\r");
	NKDbgPrintfW(L"*********    Program Start  **************\n\r");
	NKDbgPrintfW(L"******************************************\n\r");

	dma_enable = FALSE;

	bufferTx = sbufferTx;
	bufferRx = sbufferRx;

	if ((bufferRx == NULL) || (bufferTx == NULL))
	{
		printf("Could not alloc buffers");
		return FALSE;
	}
	
	//Initialize SSP1 (Master)
	//-----------------------------
    if (!SPIInit(txSSPport, DATABITS, SPI_CLOCK_812_KHZ, SPI_MASTER, SPI_MODE_3, NULL))
	{
		printf("Error in InitSPI");
		return FALSE;
	}
	
	//Initialize SSP2 (Slave)
	//-----------------------------
    if (!SPIInit(rxSSPport, DATABITS, SPI_CLOCK_812_KHZ, SPI_SLAVE, SPI_MODE_3, NULL))
	{
		printf("Error in InitSPI");
		return FALSE;
	}
	
	//Create and start transmit/receive threads
	//-----------------------------
	tRx = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) RXThread, 0, 0, NULL);
	CeSetThreadPriority(tRx,10);  //Set receive thread to a high priority to avoid buffer overflows

	//Sleep(100);
	tTx = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) TXThread, 0, 0, NULL);

	printf("\r\nPress ENTER to leave the application.\r\n");
	getchar();

	TerminateThread(tRx, 0);
	TerminateThread(tTx, 0);
	Sleep(20);

	CloseHandle(tRx);
	CloseHandle(tTx);
		
	//Deinit
	//Very Important 
	//if you forget to Deinit a SPI and Init the SPI new the spi interrupts will not work.
	//In this case you have to reset the System
	DeinitSPI(txSSPport);
	DeinitSPI(rxSSPport);

	return TRUE;

}
