///
/// @file         I2cLibDemo.c 
/// @copyright    Copyright (c) 2013 Toradex AG
/// $Author: gap $
/// $Revision: 1633 $
/// $Date: 2013-05-24 09:22:26 +0200 (Fr, 24 Mai 2013) $
/// @brief        Demo for I2C Library
/// @target       Colibri Modules
/// @test tested on:  Colibri PXAxxx, T20, T30

#include <windows.h>
#include "I2cLib.h"

//------------------------------------------------------------------------------------
/// Try to access all Slave Addresses without data. If a slave is present, it will ACK the address

void i2cScan(void)
{
	int i, count=0;
	BOOL Success;

	wprintf (L"Scanning i2c bus...\r\n");
	for (i = 1; i < 127; i++)
	{
		Success = I2CBurstWrite(i, NULL, 0, 0);
		if (Success)
        {
			wprintf(L"%s0x%02x", ((count++ == 0)? L"  " : L", "), i);
        }
        //Sleep(1);
	}
	wprintf (L"\r\n%d device(s) found.\r\n", count);
}

//------------------------------------------------------------------------------------
/// WinMain
/// @param	no parameters supported
/// @retval	application exit code
int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	BOOL Success;
	DWORD i;
	DWORD VerMaj;
	DWORD VerMin;
	DWORD VerBuild;

	I2cIntGetLibVersion(&VerMaj, &VerMin, &VerBuild);
	wprintf(L"Demo Application for I2cLib\r\n");
	wprintf(L"Library Version %d.%d Build %d\r\n\r\n", VerMaj, VerMin, VerBuild);

	if (I2CInit())
	{
		BYTE data[100] = {0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 
						  0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 
						  0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 
						  0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 
						  0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x0F
		};

		I2CSetSpeed(I2C_400KBPS);

		Success = GetI2CLock(500);
		wprintf(L"write bytes...");			// write 50 bursts, 100 bytes each
		for (i = 0; i < 50; i++)
		{
			if (!I2CBurstWrite(0x68, data, 0, 100))  // slave addr + offset are already 2 bytes
            {
				break;
            }
		}
		wprintf(L"      %i bytes sent\r\nLast Error Code %d\r\n", i * 100, GetLastError());

		// BusScan
		i2cScan();

        wprintf(L"Reading the RTC...\r\n");
        I2CBurstRead(0x68, data, 0, 8);

        wprintf(L"Resetting the RTC...\r\n");
        for (i = 0; i < 8; i++)
        {
            data[i] = 0;
        }
        I2CBurstWrite(0x68, data, 0, 8);

        wprintf(L"Reading the RTC...\r\n");
        I2CBurstRead(0x68, data, 0, 8);
		ReleaseI2CLock();
	}
	I2CDeInit();

	wprintf(L"\r\npress any key to end...");
	getchar();
	return(0);
}

