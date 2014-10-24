#include <windows.h> 
#include <MapRegLib.h>
#include <CoProcLib.h>


#define GPIO_BASE	0x40E00000

typedef volatile struct 
{
    DWORD gplr0;	// Level Reg. Bank 0
    DWORD gplr1;	// Level Reg. Bank 1
    DWORD gplr2;	// Level Reg. Bank 2
    DWORD gpdr0;	// Direction Reg. Bank 0
    DWORD gpdr1;	// Direction Reg. Bank 1
    DWORD gpdr2;	// Direction Reg. Bank 2
    DWORD gpsr0;	// Output Set Reg. Bank 0
    DWORD gpsr1;	// Output Set Reg. Bank 1
    DWORD gpsr2;	// Output Set Reg. Bank 2
    DWORD gpcr0;	// Output Clr Reg. Bank 0
    DWORD gpcr1;	// Output Clr Reg. Bank 1
    DWORD gpcr2;	// Output Clr Reg. Bank 2
    DWORD grer0;	// Ris. Edge Detect Enable Reg. Bank 0
    DWORD grer1;	// Ris. Edge Detect Enable Reg. Bank 1
    DWORD grer2;	// Ris. Edge Detect Enable Reg. Bank 2
    DWORD gfer0;	// Fal. Edge Detect Enable Reg. Bank 0
    DWORD gfer1;	// Fal. Edge Detect Enable Reg. Bank 1
    DWORD gfer2;	// Fal. Edge Detect Enable Reg. Bank 2
    DWORD gedr0;	// Edge Detect Status Reg. Bank 0
    DWORD gedr1;	// Edge Detect Status Reg. Bank 1
    DWORD gedr2;	// Edge Detect Status Reg. Bank 2
    DWORD gafr0_l;	// Alt. Function Select Reg.[  0:15 ]
    DWORD gafr0_u;	// Alt. Function Select Reg.[ 16:31 ]
    DWORD gafr1_l;	// Alt. Function Select Reg.[ 32:47 ]
    DWORD gafr1_u;	// Alt. Function Select Reg.[ 48:63 ]
    DWORD gafr2_l;	// Alt. Function Select Reg.[ 64:79 ]
    DWORD gafr2_u;	// Alt. Function Select Reg.[ 80:95 ]
    DWORD gafr3_l;	// Alt. Function Select Reg.[ 96:111]
    DWORD gafr3_u;	// Alt. Function Select Reg.[112:120]
    DWORD res1[35];
    DWORD gplr3;	// Level Detect Reg. Bank 3
    DWORD res2[2];
    DWORD gpdr3;	// Data Direction Reg. Bank 3
    DWORD res3[2];
    DWORD gpsr3;	// Pin Output Set Reg. Bank 3
    DWORD res4[2];
    DWORD gpcr3;	// Pin Output Clr Reg. Bank 3
    DWORD res5[2];
    DWORD grer3;	// Ris. Edge Detect Enable Reg. Bank 3
    DWORD res6[2];
    DWORD gfer3;	// Fal. Edge Detect Enable Reg. Bank 3
    DWORD res7[2];
    DWORD gedr3;	// Edge Detect Status Reg. Bank 3

} GPIOREG;


// NOTE: We want to control a GPIO located on Pin 48 of the Lower PlugBoard of the Colibri evaluation board (EXT_IO[0]).
// On the different Colibri Modules this corresponds to different GPIOs:
// On PXA270: GPIO35
// On PXA320: GPIO95
// On PXA300: GPIO97

/*----------------------------------------------------------------------
|       main
+---------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hInstance,      // handle to current instance
                   HINSTANCE hPrevInstance,  // handle to previous instance
                   LPWSTR lpCmdLine,          // pointer to command line
                   int nCmdShow)             // show state of window
{
	DWORD		ProcID;

	GPIOREG*	pGPIORegs;


	ProcID=getPROCID();
	
	if(pGPIORegs=MapRegister(GPIO_BASE))
	{
		DWORD val;

		if(ProcID==TYPE_PXA270)
			pGPIORegs->gpdr1|=(1<<(35-32)); //Set GPIO35 as output
		else if(ProcID==TYPE_PXA320)
			pGPIORegs->gpdr2|=(1<<(95-64)); //Set GPIO95 as output
		else if(ProcID==TYPE_PXA300)
			pGPIORegs->gpdr3|=(1<<(97-96)); //Set GPIO97 as output

		if(ProcID==TYPE_PXA270)
			pGPIORegs->gpsr1=(1<<(35-32)); // Set GPIO35 HIGH
		else if(ProcID==TYPE_PXA320)
			pGPIORegs->gpsr2=(1<<(95-64)); // Set GPIO95 HIGH
		else if(ProcID==TYPE_PXA300)
			pGPIORegs->gpsr3=(1<<(97-96)); // Set GPIO97 HIGH

		if(ProcID==TYPE_PXA270)
			pGPIORegs->gpcr1=(1<<(35-32)); // Set GPIO35 LOW
		else if(ProcID==TYPE_PXA320)
			pGPIORegs->gpcr2=(1<<(95-64)); // Set GPIO95 LOW
		else if(ProcID==TYPE_PXA300)
			pGPIORegs->gpcr3=(1<<(97-96)); // Set GPIO97 LOW

		if(ProcID==TYPE_PXA270)
			pGPIORegs->gpdr1&=~(1<<(35-32)); // Set GPIO35 as input
		else if(ProcID==TYPE_PXA320)
			pGPIORegs->gpdr2&=~(1<<(95-64)); // Set GPIO95 as input
		else if(ProcID==TYPE_PXA300)
			pGPIORegs->gpdr3&=~(1<<(97-96)); // Set GPIO97 as input
		

		if(ProcID==TYPE_PXA270)
			val=pGPIORegs->gplr1&(1<<(35-32)); // Get GPIO35 level
		else if(ProcID==TYPE_PXA320)
			val=pGPIORegs->gplr2&(1<<(95-64)); // Get GPIO95 level
		else if(ProcID==TYPE_PXA300)
			val=pGPIORegs->gplr3&(1<<(97-96)); // Get GPIO97 level


		UnMapRegister((void*)pGPIORegs);
	}


	
// This code would do the same, but using our GPIOLibrary (code available for purchase)
/*	{

	DWORD gpio;
	
	gpio=35;	
	if(ProcID==TYPE_PXA320) gpio=95;
	else if(ProcID==TYPE_PXA300) gpio=97;

	InitGPIOLib();

	SetGPIODir(gpio, 1);

	SetGPIOLevel(gpio,1);
	SetGPIOLevel(gpio,0);
	
	SetGPIODir(gpio, 1);
	val=GetGPIODir(gpio);


	DeInitGPIOLib();

	} */
	
    return(TRUE);
}

