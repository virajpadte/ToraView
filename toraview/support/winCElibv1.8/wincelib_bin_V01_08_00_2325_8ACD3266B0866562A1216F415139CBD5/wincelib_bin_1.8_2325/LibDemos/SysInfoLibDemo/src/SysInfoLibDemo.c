///
/// @file         sysinfolibdemo.c
/// @copyright    Copyright (c) 2013 Toradex AG
///               [Software License Agreement]
/// $Author: gap $
/// $Rev: 1754 $
/// $Date: 2013-08-12 11:39:59 +0200 (Mo, 12 Aug 2013) $
/// @brief        Demonstrates System Information functions\n
/// @target       Colibri modules PXAxxx, T20\n
/// Tested on Colibri PXAxxx / T20 on Windows Embedded CE 6.0 / 7
/// @caveats      None

#include <windows.h>

#include "CoProcLib.h"
#include "SysInfoLib.h"

/*----------------------------------------------------------------------
|       main
+---------------------------------------------------------------------*/
int WINAPI WinMainCRTStartup(HINSTANCE hInstance,      // handle to current instance
                             HINSTANCE hPrevInstance,  // handle to previous instance
                             LPWSTR lpCmdLine,         // pointer to command line
                             int nCmdShow)             // show state of window
{
    TCHAR wcTemp;
    DWORD len, dwSerial, dwSize, dwType;
    DWORD cpuSpeed, ramSpeed, busSpeed;
    DWORD ret;

    BSPVER version = {0};
    SYSTEMTIME date = {0};
    TCHAR szBuildDate[64];
    TCHAR szBeta[32];
    TCHAR szTemp[32];
    BYTE  mac[6] = {0};

    OSVERSIONINFO    osInfo;
    PROCESSOR_INFO   cpuInfo;
    HWCFG            hwInfo;
#ifndef SYSINFOLIBBASIC
    USERCFG          userData;
#endif

    wprintf(_T("Toradex System Info Library %d.%d\r\n\r\n"), SIL_VER_MAJ, SIL_VER_MIN);

    if (!SysInfoLibInit())
    { // not able to init SysInfoLib
        wprintf(_T("Error initializing SysInfoLib\r\n"));
        goto exitSysInfoLibDemo;
    }

    // Get Bootloader Version and Build Date:
    GetBootloaderInfo(&version, &date);

    if (version.Min >> 16)
    {    
        swprintf(szBeta, L"BETA, Build %d", version.Min >> 16);
    }
    else
    {
        szBeta[0] = 0;
    }

    len = GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, &date, L"dd MMM yyyy", szBuildDate, 64) - 1;
    szBuildDate[len++] = ' ';
    len += GetTimeFormat(LOCALE_SYSTEM_DEFAULT, LOCALE_NOUSEROVERRIDE|TIME_FORCE24HOURFORMAT, &date, NULL, &szBuildDate[len], 64) - 1;
    lstrcat(&szBuildDate[len], L" (GMT)");

    wprintf(L"Bootloader\r\nID:\t%s\r\nVer:\t%d.%d %s\r\nBuilt:\t%s\r\n\r\n", version.ID == 1 ? L"COLIBRI" : L"UNKNOWN", version.Maj, version.Min&0xffff, szBeta, szBuildDate);
    
    // Get Image Version and Build Date:
    GetImageInfo(&version, &date);
    if (version.Min >> 16)
    {
        swprintf(szBeta, L"BETA, Build %d", version.Min >> 16);
    }
    else
    {
        szBeta[0] = 0;
    }

    len = GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, &date, L"dd MMM yyyy", szBuildDate, 64) - 1;
    szBuildDate[len++] = ' ';
    len += GetTimeFormat(LOCALE_SYSTEM_DEFAULT, LOCALE_NOUSEROVERRIDE|TIME_FORCE24HOURFORMAT, &date, NULL, &szBuildDate[len], 64) - 1;
    lstrcat(&szBuildDate[len], L" (GMT)");

    wprintf(L"Current Image\r\nID:\t%s\r\nVer:\t%d.%d %s\r\nBuilt:\t%s\r\n\r\n", version.ID == 1 ? L"COLIBRI" : L"UNKNOWN", version.Maj, version.Min&0xffff, szBeta, szBuildDate);

    // Get OS Version:
    if (GetOSVersion(&osInfo))
    {
        wprintf(L"Current OS:\t%s %d.%d\r\n\r\n", osInfo.dwPlatformId == VER_PLATFORM_WIN32_CE ? L"Windows CE" : L"UNKNOWN OS", osInfo.dwMajorVersion, osInfo.dwMinorVersion);
    }

    // Get System Parameter:
    if (SystemParametersInfo (SPI_GETOEMINFO, 32, (PVOID)szTemp, 0))
    {
        wprintf(L"PlatformOEM:\t%s\r\n", szTemp);
    }

    if (SystemParametersInfo (SPI_GETPLATFORMTYPE, 32, (PVOID)szTemp, 0))
    {
        wprintf(L"PlatformType:\t%s\r\n\r\n", szTemp);
    }

    // Get Processor Info:
    GetProcessorInfo(&cpuInfo);
    wprintf(L"Processor Info:\t%s, %s, %s, %d MHz\r\n", cpuInfo.szVendor, cpuInfo.szProcessorName, cpuInfo.szProcessCore, cpuInfo.dwClockSpeed / 1000000);

    // Get Processor ID:
    switch(getPROCID())
    {
        case TYPE_PXA270:    swprintf(szTemp, L"PXA270"); break;    
        case TYPE_PXA300:    swprintf(szTemp, L"PXA300"); break;
        case TYPE_PXA310:    swprintf(szTemp, L"PXA310"); break;
        case TYPE_PXA320:    swprintf(szTemp, L"PXA320"); break;
        case TYPE_PXA300XT:  swprintf(szTemp, L"PXA300XT"); break;
        case TYPE_TEGRA2:    swprintf(szTemp, L"TEGRA2"); break;
        case TYPE_TEGRA3:    swprintf(szTemp, L"TEGRA3"); break;
        default:    swprintf(szTemp, L"unknown CPU");
    }
    wprintf(L"Processor ID:\t%s (0x%X)\r\n\r\n", szTemp, getPROCID());

    // Get System Speeds:
    GetSystemSpeeds(&cpuSpeed, &ramSpeed, &busSpeed);
    cpuSpeed = cpuSpeed / (1000 * 1000);
    ramSpeed = ramSpeed / (1000 * 1000);
    busSpeed = busSpeed / (1000 * 1000);
    wprintf(L"System Speeds:\tCPU: %d MHz, RAM: %d MHz, BUS: %d MHz\r\n\r\n", cpuSpeed, ramSpeed, busSpeed);

    //Set CPU Core Voltage
    if (0) {
        DWORD voltage;
        voltage=SetCoreVoltage(1450);
        wprintf(L"Set core voltage to \t%d mV\r\n\r\n", voltage );
    }

    //Set System Frequency
    switch (getPROCID()) {
        case TYPE_PXA270:
            ret=0;
            wprintf(L"Do you want to change CPU Speed to 520MHz(f) or 312MHz(s)? ");
            wcTemp = getwchar(); fflush(stdin);
            switch(wcTemp) {
                case 'f':
                    ret = SetFrequencyPXA270(16, 5, TRUE, FALSE, TRUE, TRUE, TRUE);
                break;
                case 's':
                    ret = SetFrequencyPXA270(16, 3, TRUE, FALSE, TRUE, TRUE, TRUE);
                break;
                default:
                    wprintf(L"\r\nInput not valid\r\n\r\n");
                break;
            }
        break;
    
        case TYPE_PXA320:
            wprintf(L"Do you want to change CPU Speed to 806MHz(f) or 403MHz(s)? ");
            wcTemp = getwchar(); fflush(stdin);
            switch(wcTemp){
                case 'f':
                    ret = SetFrequencyPXA3xx(31, 2, 208, 312, 208);
                break;
                case 's':
                    ret = SetFrequencyPXA3xx(31, 1, 208, 312, 208);
                break;
                default:
                    wprintf(L"Input not valid\r\n\r\n");
                break;
            }
        break;

        default: break;
    }    
    if (ret == 0)
    {
        wprintf(L"Set New FrequencyPXA3xx Error\t%d\r\n", ret );
    }

    // Get System Speeds:
    GetSystemSpeeds(&cpuSpeed, &ramSpeed, &busSpeed);
    cpuSpeed = cpuSpeed / (1000 * 1000);
    ramSpeed = ramSpeed / (1000 * 1000);
    busSpeed = busSpeed / (1000 * 1000);
    wprintf(L"System Speeds:\tCPU: %d MHz, RAM: %d MHz, BUS: %d MHz\r\n\r\n", cpuSpeed, ramSpeed, busSpeed);

    // Get Memory Size:
    wprintf(L"Memory Size:\t%d MB RAM\r\n", GetMemorySize() / (1024 * 1024));
    
    // Get Flash Size:
    dwSize = GetFlashSize(&dwType);
    wprintf(L"Flash Size:\t%d MB %s\r\n\r\n", dwSize / (1024 * 1024), dwType == TYPE_NOR ? L"NOR Flash" : L"NAND Flash");

    // Get Board Serial:
    GetBoardSerial(&dwSerial);
    wprintf(L"Board Serial:\t%d\r\n", dwSerial);

    // Get Board Version:
    if (GetBoardVersion(&hwInfo))
    {
        wprintf(L"Board Version:\t%d.%d %c\r\n\r\n", hwInfo.VerMaj, hwInfo.VerMin, hwInfo.Variant + 0x41);
    }
    else
    {
        wprintf(L"Board Version:\tnot set\r\n\r\n");
    }

    // Get MAC Address:
    GetMACAddress(mac);
    wprintf(L"Board MAC:\t%x:%x:%x:%x:%x:%x\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    
    // Set MAC Address:
    wprintf(L"Do you want to save a new MAC Address (y/n)? ");
    wcTemp = getwchar();
    fflush(stdin);
    if (wcTemp == 'y')
    {
        memset(&mac, 0, sizeof(mac)); // clear to be sure we write to empty space
        wprintf(L"Enter new MAC Address: ");
        len = wscanf(L"%x:%x:%x:%x:%x:%x", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
        fflush(stdin);
        
        if (len == 6)
        {
            SetMACAddress(mac); // save new MAC

            memset(&mac, 0, sizeof(mac)); // clear to be sure we get the newone for Flash
            GetMACAddress(mac);
            wprintf(L"New Board MAC Address:  %x:%x:%x:%x:%x:%x\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        }
        else
        {
            wprintf(L"Wrong input format. %d Bytes detected. Try this format X:X:X:X:X:X, where each X is one Byte HEX-Value.\r\n", len);
        }
        
    }
    wprintf(L"\r\n");

#ifndef SYSINFOLIBBASIC
    // This is not available in the free SysInfoLibBasic version. If you want to use this functionality please contact Toradex

    // Get User Data (String example):
    GetUserData(&userData);
    wprintf(L"Current User String: %s\r\n",  userData.wUserData );

    // Set User Data (String example):
    wprintf(L"Do you want to save a new User String (y/n)? ");
    wcTemp = getwchar(); 
    fflush(stdin);
    if (wcTemp == 'y')
    {
        memset(&userData, 0, sizeof(USERCFG)); // clear to be sure we write to empty space
        wprintf(L"Enter new User String: ");
        wscanf(L"%s",userData.wUserData);
        fflush(stdin);
        SaveUserData(&userData);

        // Get new User Data (String example):
        memset(&userData,0,sizeof(USERCFG)); // clear to be sure we get the newone for Flash
        GetUserData(&userData);
        wprintf(L"New User String:       %s\r\n",  userData.wUserData );
    }
#endif
    
    SysInfoLibDeInit();

exitSysInfoLibDemo:
    wprintf(L"\r\n");
    // end programm
    wprintf(L"Press Enter to close the program");
    wcTemp = getwchar();
    fflush(stdin);

    return(TRUE);
}