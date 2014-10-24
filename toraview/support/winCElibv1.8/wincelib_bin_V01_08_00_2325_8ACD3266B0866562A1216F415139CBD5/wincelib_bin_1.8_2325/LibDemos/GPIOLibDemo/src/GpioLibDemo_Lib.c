
//Toradex AG

/********************************************************
Small Demo App for the GPIOlib.
For an interface describtion see GPIOLib.h

On the Evaluation board:
Connect 
X10 pin 47 with X21 pin 1 and
X10 pin 48 with X21 pin 2

You can use the first switch to toggle the pin.

For informantion about comatibility the "Colibri Modules MigrationGuide"
********************************************************/


#include <windows.h> 


#include "gpioLib.h"
#include "CoProcLib.h"



int WINAPI WinMain(HINSTANCE hInstance,      // handle to current instance				   
                   HINSTANCE hPrevInstance,  // handle to previous instance				   
                   LPWSTR lpCmdLine,          // pointer to command line				   
                   int nCmdShow)             // show state of window				   
{	
	

	//DWORD inputGPIONumber;		//GPIO on SODIMM Pin 133
	//DWORD outputGPIONumber;		//GPIO on SODIMM Pin 135
	//PIN_INSTANCE gpioNum;
	BOOL inputVal;
	DWORD dwProcID;
	
	DWORD verMaj,verMin,verBuild;

	GPIOGetLibVersion(&verMaj,&verMin,&verBuild);
	printf("GPIOLibDemo  (GPIOLib Version %d.%d.%d)\n\r\n\r",verMaj,verMin,verBuild);	


	dwProcID = getPROCID();
	switch(dwProcID){
		case TYPE_PXA270:
			printf("PXA270 detected\n\r");
			//inputGPIONumber=37;
			//outputGPIONumber=35;
			printf("Set GPIO 11 to the alternate function 48MHz clock\n\r");
			SetGPIOAltFn(11, 3, DIR_OUT);		//Set GPIO 11, AltFunc 3 (48 MHz Clock) , Output
		break;

		case TYPE_PXA320:
			printf("PXA320 detected\n\r");
			//inputGPIONumber=94;
			//outputGPIONumber=95;
		break;

		case TYPE_PXA300:
			printf("PXA300 detected\n\r");
			//inputGPIONumber=96;
			//outputGPIONumber=97;
		break;

		case TYPE_PXA310:
			printf("PXA310 detected\n\r");
			//inputGPIONumber=96;
			//outputGPIONumber=97;
		break;

		case TYPE_TEGRA2:
			printf("Tegra 2 detected\n\r");
			//inputGPIONumber=
			//outputGPIONumber=
        break;

        case TYPE_TEGRA3:
			printf("Tegra 3 detected\n\r");
			//inputGPIONumber=
			//outputGPIONumber=
		break;

		default:
			printf("Processor type unknown\n\r");
			return FALSE;
		break;

	}


	/*
	In an earlier version of the GPIOLib the manipulation of the pins 
	was based on the GPIO numbers and not on the SODIMM numbers.
	The following code shows how you can get GPIONr form an SODIMM pin.
	This can still be usefull, e.g. for pins with more than one GPIO
	*/


	/*
	//Get GPIO number
	GetGPIOFromPin(135, FALSE,&gpioNum);
	outputGPIONumber = gpioNum.inst1;
	printf("Output SODIMM pin 135, this is GPIO number %d on this Colibri\n\r",outputGPIONumber);
	*/


	/*
	//Get GPIO number
	GetGPIOFromPin(133, FALSE,&gpioNum);
	inputGPIONumber = gpioNum.inst1;
	printf("Input SODIMM pin 133, this is GPIO number %d on this Colibri\n\r",inputGPIONumber);
	*/




	//Test output
	printf("Set SODIMM pin 135 to high\n\r");
	SetPinAltFn(135,-1,DIR_OUT);		//Set Pin functonality to GPIO and set to Input (Alternativ Function -1 is always GPIO function)
	SetPinLevel(135,TRUE);
	printf("Press Enter to swich the LED off.\n\r");
	getchar();
    printf("Set SODIMM pin 135 to low\n\r");
	SetPinLevel(135,FALSE);	
	

	//Test input
	SetPinAltFn(133,-1,DIR_IN);		//Set Pin functonality and set to Input
    inputVal=GetPinLevel(133);

    printf("\n\rRead  SODIMM pin 133 level: %d\n\r",inputVal);

	DeInitGPIOLib();

	printf("Press Enter to end the program\n\r");
	getchar();

	
    return(TRUE);
	

}

