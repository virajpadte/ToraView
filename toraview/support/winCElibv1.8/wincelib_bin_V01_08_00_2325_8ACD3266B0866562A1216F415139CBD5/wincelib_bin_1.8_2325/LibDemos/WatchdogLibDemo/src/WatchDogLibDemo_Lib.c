#include <windows.h> 

#include "watchdoglib.h"


/*----------------------------------------------------------------------
|       main
+---------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hInstance,      // handle to current instance
                   HINSTANCE hPrevInstance,  // handle to previous instance
                   LPWSTR lpCmdLine,          // pointer to command line
                   int nCmdShow)             // show state of window
{	
//------------------------------------------------------------------------------------
/// Test Watchdog functionality.
/// IMPORTANT: Watchdog functonality on the PXA270 has a BUG.
/// You can only use it with the Colibri PXA3xx.
/// 

	BOOL result;
	DWORD i;
    DWORD VerMaj;
    DWORD VerMin; 
    DWORD Build;

	result = WatchdogInit();	
	printf("Watchdog Demo Program\n\r");

    WatchdogGetLibVersion(&VerMaj, &VerMin, &Build);
    printf("Used WatchdogLib: %d.%d.%d\n\r",VerMaj, VerMin, Build);


	for (i=0; i<2; i++)
	{
        printf("Set Watchdog Timeout to 5 Seconds\n\r");
		result = WatchdogRetrigger(5000);	// Set Watchdog Timeout to 5sec
		printf("Wait 4 Seconds\n\r");
		Sleep(4000);
	}

    printf("Set Watchdog Timeout to 5 Seconds\n\r");
	result = WatchdogRetrigger(5000);	// Set Watchdog Timeout to 5sec
    printf("We don't retrigger the Watchdog, System should do a reset in about 5 seconds\n\r");

    Sleep(6000);
    printf("Error System did not reset!\n\r");
    return(TRUE);

}

