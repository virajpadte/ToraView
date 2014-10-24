///
/// @file        FreqLibDemo_Lib.c
/// @copyright   Copyright (c) 2013 Toradex AG
/// @author      $Author: kia $
/// @version     $Rev: 2296 $
/// @date        $Date: 2014-05-30 16:18:47 +0200 (Fr, 30 Mai 2014) $
/// @brief       Demonstration program for FreqLib
///
/// @target      PXAxxx, Txx, Windows CE 6 & 7
/// @caveats     Not used for Windows CE 5
///

#include <windows.h>

#include "FreqLib.h"
#include "Coproclib.h"

/********************************************************
    Small Demo App for the FreqLib.
    For an interface description see FreqLib.h
********************************************************/
static const tTegDfsBusyHint   busyHints[4]    = {{TegDfsClockId_Emc, -1,  80000, TRUE},  // frequencies to boost during SPI activity
                                                 {TegDfsClockId_Ahb,  -1,  80000, TRUE},
                                                 {TegDfsClockId_Apb,  -1,  80000, TRUE},
                                                 {TegDfsClockId_Cpu,  -1, 600000, TRUE}};

static const tTegDfsBusyHint   busyHintsOff[4] = {{TegDfsClockId_Emc, -1, 0, TRUE},  // to finish frequencies boost 
                                                 {TegDfsClockId_Ahb,  -1, 0, TRUE},
                                                 {TegDfsClockId_Apb,  -1, 0, TRUE},
                                                 {TegDfsClockId_Cpu,  -1, 0, TRUE}};

//*******************************************************
// Print the status of the Tegra system clocks
void Tegra_PrintClocks(FRQTEGClocks tegClocks,DWORD dfsState)
{
    printf("CPU Clock running @ %dMHz\r\n", tegClocks.cpuClk);
    printf("System Clock running @ %dMHz\r\n", tegClocks.sysClk);
    printf("AVP Clock running @ %dMHz\r\n", tegClocks.avpClk);
    printf("AHB Clock running @ %dMHz\r\n", tegClocks.ahbClk);
    printf("APB Clock running @ %dMHz\r\n", tegClocks.apbClk);
    printf("VID Clock running @ %dMHz\r\n", tegClocks.vidClk);
    printf("EMC Clock running @ %dMHz\r\n", tegClocks.emcClk);
    switch (dfsState)
    {
        case TEG_DFS_STATE_STOPPED:
            printf("DFS State = Stopped\r\n");
            break;

        case TEG_DFS_STATE_CLOSED_LOOP:
            printf("DFS State = Closed Loop\r\n");
            break;

        default:
            printf("DFS State = Unknown\r\n");
            break;
    }
}


/********************************************************
********************************************************/

int WINAPI WinMain(HINSTANCE hInstance,      // handle to current instance
                   HINSTANCE hPrevInstance,  // handle to previous instance
                   LPWSTR lpCmdLine,          // pointer to command line
                   int nCmdShow)             // show state of window
{
    DWORD VerMaj, VerMin, Build;
    DWORD coreFreqMHz, coreFreq;
    MULTCFG mcfg;

    FRQTEGClocks tegClocks;
    DWORD dfsState;

    FRQGetLibVersion(&VerMaj, &VerMin, &Build);
    printf("FreqLibDemo Application. FreqLibVersion: %d.%d.%d\r\n", VerMaj, VerMin, Build);
    printf("**********************************************\r\n\r\n");

    if (GetSoCType() == PXA)
    {
        mcfg = GetFreq();

        CalcSystemFreq(mcfg, &coreFreq, NULL, NULL, NULL, NULL, NULL, NULL);

        printf("CPU running @ %dMHz\r\n", coreFreq / 1000000);

        printf("Enter new speed in MHz (104 - 806): ");
        scanf("%d", &coreFreqMHz);

        if (coreFreqMHz < 208)
        { // set to 104MHz
            mcfg.l = 8;
            mcfg.n = 1;
        }
        else if (coreFreqMHz < 416)
        { // set to 208MHz
            mcfg.l = 16;
            mcfg.n = 1;
        }
        else if (coreFreqMHz < 624)
        { // set to 416MHz
            mcfg.l = 16;
            mcfg.n = 2;
        }
        else if (coreFreqMHz < 806)
        { // set to 624MHz
            mcfg.l = 24;
            mcfg.n = 2;
        }
        else
        { // set to 806MHz
            mcfg.l = 31;
            mcfg.n = 2;
        }

        SetFreq(mcfg);

        printf("Changed CPU Speed to %dMHz\r\n\r\n", 13 * mcfg.l * mcfg.n);
    }
    else
    {
        FRQInit();
        
        // print intial clock frequencies
        Sleep(1000); // idle, will cause the system to throttle clocks down
        printf ("-- Initial Clock Frequencies --\r\n");
        FRQGetTEGClocks(&tegClocks);
        dfsState = FRQGetTEGDFSState();
        Tegra_PrintClocks(tegClocks, dfsState);
        
        // boost frequencies and print the new values
        printf ("\r\n-- Clock Frequencies With Boost Active --\r\n");
        TegFrq_Boost(busyHints, 4, TegDfsBusyHintSyncMode_Sync);
        Sleep(1000);    // idle, system clocks still remain at the boosted frequencies.
        FRQGetTEGClocks(&tegClocks);
        dfsState = FRQGetTEGDFSState();
        Tegra_PrintClocks(tegClocks, dfsState);

        // turn boosting off and print clocks again
        printf ("\r\n-- Clock Frequencies After Turning Boost Off --\r\n");
        TegFrq_Boost(busyHintsOff, 4, TegDfsBusyHintSyncMode_Sync);
        Sleep(1000); // Give the system some time to throttle down the clock
        FRQGetTEGClocks(&tegClocks);
        dfsState = FRQGetTEGDFSState();
        Tegra_PrintClocks(tegClocks, dfsState);


        FRQDeInit();
    }

    // done...
    printf("Press Enter to close.\r\n");
    getchar();
    return(TRUE);
}
