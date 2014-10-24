#include <windows.h> 

#include "..\inc\dmalib.h"



/*----------------------------------------------------------------------
|       main
+---------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hInstance,      // handle to current instance
                   HINSTANCE hPrevInstance,  // handle to previous instance
                   LPWSTR lpCmdLine,          // pointer to command line
                   int nCmdShow)             // show state of window
{	
	DWORD i=50;
	DWORD channel;
	MEM_T src, target,desc;
	DWORD size;
	BOOL bLoop;

	// transfer a size of 100kBytes
	size = 100*1024;

	// do the transfer in a loop
	bLoop = TRUE;

	DMAInit();

	// allocate a src buffer in the RAM
	DMAAllocBuffer(&src, size);
	memset ( (DWORD*)src.va, 0xAA, size );

	// allocate a target buffer in the RAM
	DMAAllocBuffer(&target, size);

	// Create a DMA descriptor with a burs size of 32Bytes (8 and 16 bytes are also allowed) and auto-increment the source and target addresses.
	// the addresses always have to be PHYSICAL!!!!!!
	desc.pa = 0; //DMACreateTransferDescriptors will allocate memory if desc = NULL
	desc.va = 0;
	DMACreateTransferDescriptors(src.pa, target.pa, size, DCMD_BSIZE32|DCMD_INCSRCADDR|DCMD_INCTRGADDR, bLoop, &desc);

	// allocate a channel for a peripheral device if you need one
	//channel=DMAAllocChannelEx(PERIPH_CIF1,DMA_PRIORITY_MEDIUM);
	channel = DMAAllocChannelEx(DMA_PERIPH_MEM2MEM,DMA_PRIORITY_MEDIUM);

	
	//Notify DMALib about usage
	DMASetChannelUsage(channel,"DMALibDemo",10);

	DMAStartTransferEx(channel, &desc, 0,FALSE);

	// do something meaningfull here....
	while(i--)
	{
		Sleep(1);
	}

	// deinitialize the dma channel and buffers

	DMAStopTransfer(channel);


	if(memcmp((void*)src.va, (void*)target.va,size)==0)
	{
		printf("DMA transfer sucessfull\n\r");
	}
	else{
		printf("DMA transfer failed\n\r");
	}

	printf("Press Enter to close the programn\r");
	getchar();

	DMADestroyTransferDescriptors(&desc);
	DMAFreeBuffer(&src);
	DMAFreeBuffer(&target);

	DMADeInit();


    return(TRUE);

}

