#include <windows.h>

#include "adclib.h"


/*----------------------------------------------------------------------
|       main
+---------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hInstance,      // handle to current instance
                   HINSTANCE hPrevInstance,  // handle to previous instance
                   LPWSTR lpCmdLine,          // pointer to command line
                   int nCmdShow)             // show state of window
{
	BOOL ret=FALSE;
	BYTE i;

	DWORD data=0;
	double temp;
	int samples = 10;
	
	
	if(!Ac97Init()){		//Initialize AC97 and get a MUTEX for the AC97 access (Touchpanel and Sounddriver use AC97 too)
		printf("Could not initalize codec\n\r");
	}
	else{
		do{
			DWORD max=0;
			DWORD min = 10000;
			for(i=0; i<4;i++){
				ret = ReadADCChannel(i,&data,samples);		//Read AD channel
				temp=(double)(MicrovoltsPerLsb(i)*data)/(1000000.0*samples);

				printf("Analog Value Chanel %d: %.2f V Raw: %d\n",i,temp,data);

				
			}
			printf("\nPress e to close the program\n");
		}while('e'!=getchar());
	}



	//Only DeInit if your use the AC97 alone
	//ret = Ac97DeInit;
    return(TRUE);

}