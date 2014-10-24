#include <windows.h> 

#include "MemLib.h"
#include "CoProcLib.h"
#include "MapRegLib.h"


#define CAN_MODE	(0)
#define CAN_CMD     (1)
#define CAN_STATUS  (2)

BYTE GetReg(WORD* pMem, DWORD reg)
{
	pMem[0] = (WORD)reg; // write to CAN chip
	return (BYTE)pMem[2]; // read from CAN chip
}


/*----------------------------------------------------------------------
|       main
+---------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hInstance,      // handle to current instance
                   HINSTANCE hPrevInstance,  // handle to previous instance
                   LPWSTR lpCmdLine,          // pointer to command line
                   int nCmdShow)             // show state of window
{
	TCHAR wcTemp;
	DWORD dwCanBase;
	MSCCFG myTimings;
	
	WORD* pCAN; // this is our pointer to the external peripheral. See Colibri Eval.Board

	myTimings.type = MEMTYPE_VLIO;
	myTimings.buswidth = MEMBUS_16BIT;
	myTimings.vlio.readAssert   = 100;
	myTimings.vlio.readDeassert = 100;


	wprintf(_T("MemLib Demo\r\n\r\n"));

	switch(getPROCID())
	{
		case TYPE_PXA270:
		{
			dwCanBase = 0x04000000; // nCS1/EXT_nCS0 PXA270
			
			myTimings.recovery = 100;
			myTimings.buffer = MEMBUFFER_FAST;
			
			if(!SetMemTimingsEx(dwCanBase, &myTimings))
			{
				wprintf(_T("Error SetMemTimingsEx\r\n"));
				Sleep(5000);
				return FALSE;
			}
		}
		break;

		case TYPE_PXA300:
		case TYPE_PXA310:
		case TYPE_PXA320:
		{
			dwCanBase = 0x14000000; // EXT_nCS0 PXA3xx
			
			if(!SetMemTimingsEx(0x14000000, &myTimings))
			{
				wprintf(_T("Error SetMemTimingsEx\r\n"));
				Sleep(5000);
				return FALSE;
			}
		}
		break;

		default: return FALSE;
	}

	pCAN = MapRegister(dwCanBase); // MapMemory can be used too

	// now we are ready to use our pointer to the external peripheral
    
	wprintf(_T("\r\nReading some CAN registers...\r\n"));
	wprintf(_T("CAN_MODE   = 0x%X\r\n"), GetReg(pCAN,CAN_MODE));
    wprintf(_T("CAN_CMD    = 0x%X\r\n"), GetReg(pCAN,CAN_CMD));
	wprintf(_T("CAN_STATUS = 0x%X\r\n\r\n"), GetReg(pCAN,CAN_STATUS));

	UnMapRegister(pCAN);

	// end programm
	wprintf(L"Press any key to end program");
	wcTemp = getwchar();
	fflush(stdin);
	
	return(TRUE);

}

