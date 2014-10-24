/********************************************************
Toradex AG

Small Demo App for the OverlayLib.
Tested with 24BPP and 18LDD

For an interface description see OverlayLib.h

This Demo use 16BPP RGB Overlays 
(5:5:5 and the MSB as transparency bit)

Overlay 1 supports full and half transparency
Overlay 2 only full transparency

Details about the Overlay and Pixel Format can be found 
In the Marvell PXA Datasheet.

The animation function are not optimized, this function 
are just for a small demo
*********************************************************/


#include <windows.h>

#include "OverlayLib.h"

#define OV1_XPOS		10
#define OV1_YPOS		100
#define OV1_WIDTH		200
#define OV1_HEIGHT		100

#define OV2_XPOS		100
#define OV2_YPOS		50
#define OV2_WIDTH		100
#define OV2_HEIGHT		100

#define A1_BORDERCOLOR (0x1f<<10) //RED
#define A1_BACKCOLOR   ((0x1f<<5)|0x8000) //Green Half Transparenc
#define A1_BOXCOLOR    ((0x0f<<10)|(0x1f)) 
#define A1_BOXSIZE		40
#define A1_BORDERTHICKNESS 8

#define A2_BACKCOLOR   ((0x1f<<10)|(0x1f<<5)) 


#define DELTA_X 2
#define DELTA_Y 2
#define DELTA2_X 10
#define DELTA2_Y 10


XLLP_OVERLAY_T	ov1;
XLLP_OVERLAY_T	ov2;


static BOOL bAnimationThreadStop1;
static BOOL bAnimationThreadStop2;

static int BoxPosX=0;
static int BoxPosY=0;
static int deltaY=DELTA_Y;
static int deltaX=DELTA_X;
static int delta2Y=DELTA2_Y;
static int delta2X=DELTA2_X;



SetColor(WORD* pBuffer, WORD color, DWORD size)
{
	DWORD i;
	for(i=size;i!=0;i--)
	{
		*pBuffer=color;
		pBuffer++;
	}
}

static firstTime=0;


void DrawAnimation(WORD* pBuffer, int width, int height, int borderThickness, WORD borderColor, int boxSize, WORD boxColor, WORD backColor)
{
	int y;
	int xBorder =(width-borderThickness);

	SetColor(pBuffer,backColor,width*height);

	if(BoxPosX+boxSize>width-borderThickness){
		BoxPosX=width-borderThickness-boxSize;
		deltaX=-DELTA_X;
	}
	if(BoxPosX<borderThickness){
		BoxPosX=borderThickness;
		deltaX=+DELTA_X;
	}
	if(BoxPosY+boxSize>height-borderThickness){
		BoxPosY=height-borderThickness-boxSize;
		deltaY=-DELTA_Y;
	}
	if(BoxPosY<borderThickness){
		BoxPosY=borderThickness;

		deltaY=+DELTA_Y;
	}


	for(y=0;y<height;y++){
		if((y>borderThickness)&&(y<height-borderThickness)){
			SetColor(&pBuffer[y*width],borderColor,borderThickness); //border
			SetColor(&pBuffer[(y*width)+xBorder],borderColor,borderThickness); //border

			if((y>BoxPosY)&&(y<BoxPosY+boxSize))
			{
				SetColor(&pBuffer[(y*width)+BoxPosX],boxColor,boxSize);
			}

		}
		else{ //Top and Bottom
			SetColor(&pBuffer[y*width],borderColor,width); //border
		}
	}

	BoxPosY+=deltaY;
	BoxPosX+=deltaX;

}


void DrawSolidOverlay(XLLP_OVERLAY_T* pOv, WORD color)
{
	Overlay_GetBuffer(pOv);
	
	if(pOv->ovNumber==1)
		SetColor((WORD*)pOv->CH1.va,color,pOv->OverlayHeight*pOv->OverlayWidth);
	else
		SetColor((WORD*)pOv->CH2_Y.va,color,pOv->OverlayHeight*pOv->OverlayWidth);

}

DWORD WINAPI AnimationThread1(LPVOID pVoid)
{
	
	XLLP_OVERLAY_T*	pOv = (XLLP_OVERLAY_T*)pVoid;

	WORD* pWOverlayBuffer1;
	WORD* pWOverlayBuffer2;
	DWORD i=0,n=0;

	

	Overlay_Enable(pOv);

	
	//Get buffers
	Overlay_GetBuffer(pOv);
	if(pOv->ovNumber==1) //Overlay1
		pWOverlayBuffer1=(WORD*)pOv->CH1.va;
	else  //Overlay 1
		pWOverlayBuffer1=(WORD*)pOv->CH2_Y.va;
	DrawAnimation(pWOverlayBuffer1,pOv->OverlayWidth,pOv->OverlayHeight,A1_BORDERTHICKNESS,A1_BORDERCOLOR,A1_BOXSIZE,A1_BOXCOLOR,A1_BACKCOLOR);
	Overlay_Flip(pOv); //Change Buffer
	Overlay_GetBuffer(pOv);
	if(pOv->ovNumber==1)
		pWOverlayBuffer2=(WORD*)pOv->CH1.va;
	else
		pWOverlayBuffer2=(WORD*)pOv->CH2_Y.va;
	DrawAnimation(pWOverlayBuffer1,pOv->OverlayWidth,pOv->OverlayHeight,A1_BORDERTHICKNESS,A1_BORDERCOLOR,A1_BOXSIZE,A1_BOXCOLOR,A1_BACKCOLOR);
	Overlay_Flip(pOv);



	while(!bAnimationThreadStop1){
		if(((i++)%100)==0){
			SetTransparencyFactor(n++%7);
		}
		
		DrawAnimation(pWOverlayBuffer1,pOv->OverlayWidth,pOv->OverlayHeight,A1_BORDERTHICKNESS,A1_BORDERCOLOR,A1_BOXSIZE,A1_BOXCOLOR,A1_BACKCOLOR);
		Overlay_Flip(pOv);
		DrawAnimation(pWOverlayBuffer2,pOv->OverlayWidth,pOv->OverlayHeight,A1_BORDERTHICKNESS,A1_BORDERCOLOR,A1_BOXSIZE,A1_BOXCOLOR,A1_BACKCOLOR);
		Overlay_Flip(pOv);
	}

	Overlay_Disable(pOv);
	return TRUE;

}

DWORD WINAPI AnimationThread2(LPVOID pVoid){

	XLLP_OVERLAY_T*	pOv = (XLLP_OVERLAY_T*)pVoid;

	DWORD displayWidth = GetSystemMetrics(SM_CXSCREEN);
	DWORD displayHeight = GetSystemMetrics(SM_CYSCREEN);

	Overlay_Enable(pOv);
	DrawSolidOverlay(pOv, A2_BACKCOLOR);

	while(!bAnimationThreadStop2){

		if(pOv->X_Position+pOv->OverlayWidth>displayWidth){
			pOv->X_Position=displayWidth-pOv->OverlayWidth;
			delta2X=-DELTA2_X;
		}
		if(pOv->X_Position<(0+DELTA2_X)){
			pOv->X_Position=0;
			delta2X=+DELTA2_X;
		}
		if(pOv->Y_Position+pOv->OverlayHeight>displayHeight){
			pOv->Y_Position=displayHeight-pOv->OverlayHeight;
			delta2Y=-DELTA2_X;
		}
		if(pOv->Y_Position<(0+DELTA2_Y)){
			pOv->Y_Position=0;

			delta2Y=+DELTA2_Y;
		}

		Overlay_PosSizeFormat(pOv);
		pOv->X_Position+=delta2X;
		pOv->Y_Position+=delta2Y;
		Sleep(20);

	}
	
	Overlay_Disable(pOv);
	return TRUE;
}


void CreateOverlay1(DWORD xPos, DWORD yPos, DWORD width, DWORD height, DWORD format, DWORD bpp)
{

	//Set overlay structure
	ov1.OverlayWidth=width; 
	ov1.OverlayHeight=height;
	ov1.X_Position=xPos;
	ov1.Y_Position=yPos;
	ov1.DegradeBaseFrame=FALSE;
	ov1.OverlayBPP=bpp;
	ov1.HWScaleY=16;		//No Scaling
	ov1.Format = format;	
	ov1.DoubleBuf = 1;		//double buffering
	ov1.ovNumber = 1;		//Overlay 1

	if(!Overlay_Setup(&ov1))
		NKDbgPrintfW(L"Overlay1_Setup faild\n\r");



}

void CreateOverlay2(DWORD xPos, DWORD yPos, DWORD width, DWORD height, DWORD format, DWORD bpp)
{
	//Set overlay structure
	ov2.OverlayWidth=width; 
	ov2.OverlayHeight=height;
	ov2.X_Position=xPos;
	ov2.Y_Position=yPos;
	ov2.DegradeBaseFrame=FALSE;
	ov2.OverlayBPP=bpp;
	ov2.HWScaleY=16;		//No Scaling
	ov2.Format = format;	
	ov2.DoubleBuf = 0;		//No double buffering
	ov2.ovNumber = 2;		//Overlay 2

	if(!Overlay_Setup(&ov2))
		NKDbgPrintfW(L"Overlay2_Setup faild\n\r");
}


/*----------------------------------------------------------------------
|       main
+---------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hInstance,      // handle to current instance
                   HINSTANCE hPrevInstance,  // handle to previous instance
                   LPWSTR lpCmdLine,          // pointer to command line
                   int nCmdShow)             // show state of window
{

	DWORD maj,min,build;
	DWORD posX = 10;
	DWORD posY = 10;

	HANDLE hAnimationThread1;
	HANDLE hAnimationThread2;

	OVLGetLibVersion(&maj,&min,&build);
	printf("Overlay Lib Demo\n\r");
	printf("Overlay Lib Version %d.%d.%d\n\r",maj,min,build);

	LCDInit();

	CreateOverlay1(OV1_XPOS, OV1_YPOS, OV1_WIDTH, OV1_HEIGHT, FORMAT_RGB, O_BPP_16);
	CreateOverlay2(OV2_XPOS, OV2_YPOS, OV2_WIDTH, OV2_HEIGHT, FORMAT_RGB, O_BPP_16);

	bAnimationThreadStop1 = FALSE;
	hAnimationThread1 = CreateThread(NULL, 0, AnimationThread1, &ov1, 0, NULL);

	bAnimationThreadStop2 = FALSE;
	hAnimationThread2 = CreateThread(NULL, 0, AnimationThread2, &ov2, 0, NULL);

	//Wait for Keypad input
	printf("Press Enter to close the demo\n\r");
	getchar();

	
	bAnimationThreadStop1 = TRUE;
	WaitForSingleObject(hAnimationThread1,INFINITE);
	CloseHandle(hAnimationThread1);
	
	bAnimationThreadStop2 = TRUE;
	WaitForSingleObject(hAnimationThread2,INFINITE);
	CloseHandle(hAnimationThread2);


}