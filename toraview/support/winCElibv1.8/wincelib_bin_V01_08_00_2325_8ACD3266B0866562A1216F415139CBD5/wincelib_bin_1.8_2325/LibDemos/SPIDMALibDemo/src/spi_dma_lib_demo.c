///
/// @file         spi_dma_lib_demo.c
/// @copyright    Copyright (c) 2012 Toradex AG
/// $Author: kia $
/// $Revision: 2096 $
/// $Date: 2014-03-07 16:13:55 +0100 (Fr, 07 Mrz 2014) $
/// @brief        Program to demonstrate the SPI DMA Library on Colibri T20 Modules\n
///\n
///  For the T20 connect SPI4 (default) with SPI3\n
///  For other modules please consult the chart below and select SPI ports as required
///\n
///  On Evaluation Board 2.1\n
///    upper PlugArea 33 with upper PlugArea 3\n
///    upper PlugArea 32 with upper PlugArea 2\n
///    upper PlugArea 35 with lower PlugArea 20\n
///    upper PlugArea 34 with lower PlugArea 25\n
///\n
///  On Evaluation Board 3.1\n
///     Jumper(SODIMM)\n
///     SPI4=master              SPI3=slave\n
///     CLK   X10-33(88)    -    X10-28(30)\n
///     CS0   X10-32(86)    -    X10-27(28)\n
///     MOSI  X10-35(92)    -    X8-5  (55)\n
///     MISO  X10-34(90)    -    X8-6  (63)\n
///\n
///                     T30                    T20                  PXA270                 PXA300                 PXA310                 PXA320        \n
///  Function   GPIO SoDIMM Eval 3.1   GPIO SoDIMM Eval 3.1   GPIO SoDIMM Eval 3.1   GPIO SoDIMM Eval 3.1   GPIO SoDIMM Eval 3.1   GPIO SoDIMM Eval 3.1\n
///                                                                                                                                                    \n
///  SPI1 CLK                X9        X5/1   100   X9-7      23/2   88    X10-33    85/1   88    X10-33    85/1    88   X10-33    83/1    88   X10-33 \n
///  SPI1 MOSI               X22       T2/3    69   X22-21    25/2   92    X10-35    88/6   92    X10-35    88/6    92   X10-35    86/6    92   X10-35 \n
///  SPI1 MISO               X22       T3/3    77   X22-22    26/1   90    X10-34    87/6   90    X10-34    87/6    90   X10-34    85/6    90   X10-34 \n
///  SPI1 CS0                X9        X6/1   102   X9-9      24/2   86    X10-32    86/1   86    X10-32    86/1    86   X10-32    84/1    86   X10-32 \n
///                                                                                                                                                    \n
///  SPI2 CLK                X10       A7/3    67   X10-30    19/1   55    X9-5      n.a.   n.a.   n.a.     n.a.   n.a.   n.a.     n.a.   n.a.   n.a.  \n
///  SPI2 MOSI               X10       D0/3    88   X10-33    38/2   37    X10-12    n.a.   n.a.   n.a.     n.a.   n.a.   n.a.     n.a.   n.a.   n.a.  \n
///  SPI2 MISO               X10       D1/3    90   X10-34    11/2   28    X10-27    n.a.   n.a.   n.a.     n.a.   n.a.   n.a.     n.a.   n.a.   n.a.  \n
///  SPI2 CS0                X10       D3/3    86   X10-32    14/2   63    X9-6      n.a.   n.a.   n.a.     n.a.   n.a.   n.a.     n.a.   n.a.   n.a.  \n
///                                                                                                                                                 \n
///  SPI3 CLK   O3/P    25   X10-5     A6/3    30   X10-28    52/2   73    X10-31    91/1   154   X3-C7     91/1   154   X3-C7     69/2    66   X9-31  \n
///  SPI3 MOSI  O1/P    35   X10-11    B6/3    55   X9-5      35/3  135    X10-40    93/1   158   X3-C8     93/1   158   X3-C8     71/2    57   X9-33  \n
///  SPI3 MISO  O2/P    33   X10-10    B7/3    63   X9-6      82/1   77    X22-22    94/1   160   X3-A9     94/1   160   X3-A9     72/2    61   X9-34  \n
///  SPI3 CS0   O4/P    27?  X10-6     B5/3    28   X10-27    83/1   79    X22-11    92/1   156   X3-A8     92/1   156   X3-A8     70/2    64   X9-32  \n
///                                                                                                                                                 \n
///  SPI4 CLK                X10       D0/1    88   X10-33    n.a.  n.a.    n.a.     n.a.   n.a.   n.a.     n.a.   n.a.   n.a.     n.a.   n.a.   n.a.  \n
///  SPI4 MOSI               X10       D4/1    92   X10-35    n.a.  n.a.    n.a.     n.a.   n.a.   n.a.     n.a.   n.a.   n.a.     n.a.   n.a.   n.a.  \n
///  SPI4 MISO               X10       D1/1    90   X10-34    n.a.  n.a.    n.a.     n.a.   n.a.   n.a.     n.a.   n.a.   n.a.     n.a.   n.a.   n.a.  \n
///  SPI4 CS0                X10       D3/1    86   X10-32    n.a.  n.a.    n.a.     n.a.   n.a.   n.a.     n.a.   n.a.   n.a.     n.a.   n.a.   n.a.  \n
///                                                                                                                                                    \n
/// @target           Colibri T20 Modules
/// @test tested on:  T20
#include <windows.h> 
#include "spilib.h"
#include "CoProcLib.h"

#define DATA_BITS        (8)  ///< Number of databits in the transmitted packet
#define TEST_FRAMES      (100/*100*/)  ///< Number of packets to be transmitted
#define TEST_RUNS        (4000)  ///< Number of test iterations

/// SPI Ports - for other modules consult the chart above and change these to match the configuration selected.
#define SPI_MASTER_CHANNEL 1 ///< Master SPI Channel
#define SPI_SLAVE_CHANNEL  3 ///< Slave SPI Channel

/// T20 SPI Signal Pin Configuration - see spilib.h

// tSpiPinConfiguration spiMasterPinConfiguration = {88, 92, 90, 86, -1, -1, -1}; // T20 SPI4 Default GPIO Settings
// tSpiPinConfiguration spiSlavePinConfiguration  = {30, 55, 63, 28, -1, -1, -1}; // T20 SPI3 Default GPIO Settings

// {100,  69,  77, 102,  -1,  -1, - 1} // T20 SPI1 Default GPIO Settings
// { 22, 134, 104,  24,  -1,  -1,  -1} // T20 SPI1 Alternate GPIO Settings
// {138, 142, 140, 136,  -1,  -1,  -1} // T20 SPI1 Alternate GPIO Settings

// { 67,  88,  90,  86,  -1,  -1,  -1} // T20 SPI2 Default GPIO Settings
// { 67,  88,  90,  86,  92, 129, 131} // T20 SPI2 Alternate GPIO Settings
// {100, 142, 140, 102,  -1,  -1,  -1} // T20 SPI2 Alternate GPIO Settings
// {100, 142, 140,  -1, 102,  -1,  -1} // T20 SPI2 Alternate GPIO Settings
// {138, 134, 104, 136, 129, 131,  -1} // T20 SPI2 Alternate GPIO Settings

// { 30,  55,  63,  28,  -1,  -1,  -1} // T20 SPI3 Default GPIO Settings
// {138, 142, 140,  -1, 136,  -1,  -1} // T20 SPI3 Alternate GPIO Settings
// {129, 142, 104, 131,  -1, 160,  93} // T20 SPI3 Alternate GPIO Settings
// {164, 154, 156, 131,  -1,  -1,  -1} // T20 SPI3 Alternate GPIO Settings
// { 99, 158,  31, 131,  -1,  -1,  -1} // T20 SPI3 Alternate GPIO Settings
// {192,  49,  51,  53,  -1,  -1,  -1} // T20 SPI3 Alternate GPIO Settings
// { 30,  55,  63,  28,  59,  -1,  -1} // T20 SPI3 Alternate GPIO Settings

// { 88,  92,  90,  86,  -1,  -1,  -1} // T20 SPI4 Default GPIO Settings
// { 21,  19, 111,  -1, 113,  -1,  -1} // T20 SPI4 Alternate GPIO Settings
// { 38,  36,  32,  -1,  34,  -1,  -1} // T20 SPI4 Alternate GPIO Settings

#define SPI_MASTER_CS 0  ///< Master SPI Chip Select 
#define SPI_SLAVE_CS  0  ///< Slave SPI Chip Select, strictly not needed in slave mode

/// Required when testing the deprecated functions 
#define CLOCK_SPEED 106       ///< SPI Clock Speed 106 = 1Mhz

DWORD *txBufferMaster; ///< Master TX Buffer Virtual Address
DWORD *rxBufferMaster; ///< Slave RX Buffer Virtual Address
DWORD *txBufferSlave;  ///< Master TX Buffer Virtual Address
DWORD *rxBufferSlave;  ///< Slave RX Buffer Virtual Address

DWORD time0;           ///< Program start time

typedef BOOL (*PFNCeSetProcessAffinity)(HANDLE, DWORD);
PFNCeSetProcessAffinity pCeSetProcessAffinity;

static  int   gStop = 0;
static  DWORD defaultSpiChannel;

DWORD dummy[10];

//*****************************************************************************
/// Generate a test pattern in RAM
///  * Each frame is stored as a 32bit value. The test pattern is a simple 
///    counter containing the frame index (except special frames listed below).
///    All unused bits set to 1 (highest bits >frameWidth are not part of a frame and are not transmitted)
///  * frame 0 is binary ...001100110011
///  * frame 1 is the MSB of numFrames
///  * frame 2 is the LSB of numFrames
///  * The last frame is binary ...010101010101
/// @param[out] buf         buffer containing the test pattern. The *caller* is responsible
///                         to allocate sufficient memory (4 x numFrames Bytes)
/// @param[in]  numFrames   size of the test pattern (number of frames)
/// @param[in]  frameWidth  size of one frame in bits. (1..32)
static void GenerateTestPattern(DWORD buf[], DWORD numFrames, DWORD frameWidth)
{
    DWORD i;
    DWORD unusedMask;
    DWORD MSB;

    ASSERT (frameWidth <= 32);
    ASSERT (numFrames >= 4);

    unusedMask =  ~((1 << frameWidth) - 1);

    for (i = 0; i < numFrames; i++)
        buf[i] = i | unusedMask;        // define frame[n] = n, with all unused bits set to 1

    buf[0]             = 0x33333333 | unusedMask;   // mark 1st frame
    MSB                = numFrames / (1 << frameWidth);
    buf[1]             = MSB | unusedMask;
    buf[2]             = (numFrames - MSB * (1 << frameWidth)) | unusedMask;
    buf[numFrames - 1] = 0x55555555 | unusedMask;  // mark last frame
}

//*****************************************************************************
/// @param[in] buf          buffer containing the received test pattern. 
///                         All unused (highest) bits must be 0
/// @param[in]  numFrames   size of the test pattern (number of frames)
/// @param[in]  frameWidth  size of one frame in bits. (1..32)
/// @retval     TRUE        verification successful
/// @retval     FALSE       buf does not contain the expected pattern.
static BOOL VerifyTestPattern(DWORD *buf, DWORD numFrames, DWORD frameWidth)
{
    DWORD i;
    DWORD unusedMask;
    DWORD expectedVal;
    
    ASSERT (frameWidth <= 32);
    ASSERT (numFrames >= 4);

    unusedMask =  ~((1 << frameWidth) - 1);

    for (i = 0; i < numFrames; i++)
    {
        if (i == 0)
            expectedVal = 0x33333333 & ~unusedMask;
        if (i == 1)
            expectedVal = numFrames / (1 << frameWidth);
        if (i == 2)
            expectedVal = numFrames - buf[1] * (1 << frameWidth);
        else if (i == numFrames - 1)
            expectedVal = 0x55555555 & ~unusedMask;
        else 
            expectedVal = i & ~unusedMask;

        if (buf[i] != expectedVal)
            return FALSE;
    }
    
    return TRUE;    // if we get here, the whole pattern was tested successfully
}

//*****************************************************************************
/// Verifies the received data is the same as the transmitted data
/// @retval     TRUE        Data matches
/// @retval     FALSE       Data does not match
static BOOL Verify(void)
{
    BOOL  isEqual1 = TRUE;  ///< 1st comparison result
    BOOL  isEqual2 = TRUE;  ///< 2nd comparison result
    DWORD index;            ///< array index pointer
    static errorCount = 0;

    DWORD mask;
    DWORD bitwidth = DATA_BITS; 
    
    if (bitwidth < 32) 
    {
        mask = (1 << bitwidth) - 1;
    }
    else
    {
        mask = 0xffffffff;
    }

    printf ("Verification... \r\n");

    // verify MasterTx vs. SlaveRx
    for (index = 0; index < TEST_FRAMES; index++)
    {
        if ((txBufferMaster[index] & mask) != (rxBufferSlave[index] & mask))
        {
            if (isEqual1 == TRUE)
            {
                isEqual1 = FALSE;
                printf("  Slave-RX does not match Master-Tx\r\n");
                errorCount++;
            }
        }
    }

    if (isEqual1 == TRUE)
    {
        printf("  Slave-RX matches Master-TX\r\n");
    }
        
    // verify SlaveTx vs. MasterRx
    for (index = 0; index < TEST_FRAMES; index++)
    {
        if ((txBufferSlave[index] & mask) != (rxBufferMaster[index] & mask))
        {
            if (isEqual2 == TRUE)
            {
                isEqual2 = FALSE;
                printf("  Master-RX does not match Slave-TX\r\n\r\n");
                errorCount++;
            }
        }
    }
    
    if (isEqual2 == TRUE)
    {
        printf("  Master-RX matches Slave-TX\r\n\r\n");
    }

    printf("Error Count: %d\r\n\r\n", errorCount);

    return isEqual1 && isEqual2;
}

//*****************************************************************************
/// Outputs the Master TX & RX and Slave TX and RX data arrays in 32 bit Hex format
/// @retval    None
static void Output(void)
{
    DWORD index; ///< array index

    DWORD mask;
    DWORD bitwidth = DATA_BITS; 
    
    if (bitwidth < 32) 
    {
        mask = (1 << bitwidth) - 1;
    }
    else
    {
        mask = 0xffffffff;
    }

    printf("\r\nTX-M: ");
    for (index = 0; index < TEST_FRAMES; index++)
    {
        printf("%08X  ", txBufferMaster[index] & mask);
    }
    printf("\r\nRX-S: ");
    for (index = 0; index < TEST_FRAMES; index++)
    {
        printf("%08X  ", rxBufferSlave[index] & mask);
    }
    printf("\r\n");
    printf("\r\nTX-S: ");
    for (index = 0; index < TEST_FRAMES; index++)
    {
        printf("%08X  ", txBufferSlave[index] & mask);
    }
    printf("\r\nRX-M: ");
    for (index = 0; index < TEST_FRAMES; index++)
    {
        printf("%08X  ", rxBufferMaster[index] & mask);
    }
    printf("\r\n\r\n\r\n");
}
DWORD dbg_getLastStatus(DWORD spiChannel);
//*****************************************************************************
/// Thread to handle Master transmission and reception
/// @param[in]  p   Not used
/// @retval     0   Always returned
static DWORD MasterThread(PVOID p)
{
    DWORD receivedSamples;    ///< Number of bytes received
    DWORD index;              ///< array index
    DWORD index2;             ///< array index
    byte count;
    printf("MasterThread: begin....\r\n");

    switch(getPROCID())
    {
        case TYPE_PXA270:
        case TYPE_PXA320:
        case TYPE_PXA300XT:  
        case TYPE_PXA300:
        case TYPE_PXA310:
        case TYPE_TEGRA3:   defaultSpiChannel = 1; 
                            break;
        case TYPE_TEGRA2:   defaultSpiChannel = 4; 
                            break;
    }

    if (SPIInit(defaultSpiChannel, DATA_BITS, 10000 /* kbit/s */, SPI_MASTER, SPI_MODE_3, NULL))
    {
        for (index = 0; index < TEST_RUNS; index++)
        {
            
            GenerateTestPattern(txBufferMaster, TEST_FRAMES, 5);    // Setup Master Transmit buffer (incrementing DWORDs)

            // Pre-Initialize Master Rx buffer to 0xAA
            memset(rxBufferMaster, 0xAA, TEST_FRAMES * sizeof(DWORD));

            // bi-directional transmission
            for (index2 = 0; index2 < 3; index2++)
                //receivedSamples = SPIReadWrite(defaultSpiChannel, rxBufferMaster/*NULL*/, txBufferMaster, TEST_FRAMES, 1000, SPI_MASTER_CS);
                receivedSamples = SPIReadWritePolling(defaultSpiChannel, (DWORD *) rxBufferMaster, (DWORD *) txBufferMaster, TEST_FRAMES, SPI_MASTER_CS);

            printf("MasterThread received %d frames. Status=0x%08x. ", receivedSamples, dbg_getLastStatus(defaultSpiChannel));
            if (VerifyTestPattern(rxBufferMaster, TEST_FRAMES, 8))
                printf("Verification successful.\r\n");
            else
                printf("Verification FAILED!\r\n");
            Sleep(1000);
            if (gStop)
            {
                break;
            }
        }
        SPIDeinit(defaultSpiChannel);
    }
    else
    {
        printf("Error in Master Thread InitSPI\n\r");
    }

    printf("\r\nMasterThread: ...finished\r\n");
    
    return(0);
}

//*****************************************************************************
/// Thread to handle Slave transmission and reception
/// @param[in]  p   Not used
/// @retval     0   Always returned
static DWORD SlaveThread(PVOID p)
{
    DWORD receivedSamples; ///< number of bytes received
    DWORD index = 0;       ///< array index
    DWORD index2= 0;       ///< array index
    byte count;
    char *slvRcv;

    printf("SlaveThread: begin...\n\r");

    //if (SPIInit(SPI_SLAVE_CHANNEL, DATA_BITS, SPI_CLOCK_1_MHZ, SPI_SLAVE, SPI_MODE_3, &spiSlavePinConfiguration))
    if (SPIInit(SPI_SLAVE_CHANNEL, DATA_BITS, SPI_CLOCK_6_5_MHZ, SPI_SLAVE, SPI_MODE_3, 0))
    {
        for (index = 0; index < TEST_RUNS; index++)
        {
            count = 0;
            // Setup Slave Transmit buffer (incrementing DWORDs)
            for (index2 = 0; index2 < TEST_FRAMES; index2++ )
            {
                // create an incrementing byte pattern
                txBufferSlave[index2] = (count << 24)
                                      + (count << 16)
                                      + (count << 8)
                                      + (count << 0);
                count = (count + 1);
            }
            // Pre-Initialize Slave Rx buffer to 0x55
            memset(rxBufferSlave, 0x55, TEST_FRAMES * sizeof(DWORD));

            // bi-directional transmission
            //receivedSamples = SPIReadWrite(SPI_SLAVE_CHANNEL, (DWORD *) rxBufferSlave, (DWORD *) txBufferSlave, TEST_FRAMES, 1000, SPI_SLAVE_CS);
            
            // uni-directional transmission
            receivedSamples = SPIReadWritePolling(SPI_SLAVE_CHANNEL, rxBufferSlave, NULL, TEST_FRAMES, SPI_SLAVE_CS);
            printf("I=%d, SlaveThread: %d frames received %d ms\n\r", index, receivedSamples, GetTickCount() - time0);
            slvRcv = (char*)rxBufferSlave;
            for (index2 = 0; index2 < 2; index2++)
                printf ("0x%02x ", *slvRcv++);
            printf("\r\n");
            Verify();
            Sleep(10);
        }
        SPIDeinit(SPI_SLAVE_CHANNEL);
    }
    else
    {
        printf("Error in Slave Thread InitSPI\n\r");
    }
    printf("\r\nSlaveThread: ...finished\r\n");

    return 0;
}

//*****************************************************************************
/// Thread to test Master to Slave transmission and reception
/// @retval     TRUE   Always returned
static int SpiMasterSlaveDemo(void)
{
    HANDLE slaveThread;      ///< Slave Thread Handle
    HANDLE masterThread;     ///< Master Thread Handle
    DWORD exitCode;

    printf("To stop the program press Enter.\r\n\r\n");

    // Store the starting tickcount in a global variable
    time0 = GetTickCount();

    // Create and start Slave thread
	//slaveThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) SlaveThread, 0, 0, NULL);
    // Allow 5mS for the slave thread to be up and running
    //Sleep(5);

    // Create and start Master thread
    masterThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) MasterThread, 0, 0, NULL);

    // Normally master and slave threads will finish on their own
    // and user will terminate the program with a key press
    getchar();

    // Stop the threads if they are still running
    //GetExitCodeThread(slaveThread, &exitCode);
    //if (STILL_ACTIVE == exitCode)
    //{
    //    TerminateThread(slaveThread, 0);
    //    DeinitSPI(SPI_SLAVE_CHANNEL);
    //}

    GetExitCodeThread(masterThread, &exitCode);
    if (STILL_ACTIVE == exitCode)
    {
        TerminateThread(masterThread, 0);
        DeinitSPI(defaultSpiChannel);
    }

    Sleep(20);

    CloseHandle(slaveThread);
    CloseHandle(masterThread);
    
    return 0;
}

//*****************************************************************************
/// Main function
/// @param[in]  hInstance       Handle to program instance
/// @param[in]  hPrevInstance   Handle to previous program instance
/// @param[in]  lpCmdLine       Command line string pointer
/// @param[in]  nCmdShow        Window state
/// @retval     0               Always returned
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPWSTR lpCmdLine,
                   int nCmdShow)
{
    int result; ///< program result code

    HANDLE processHandle;
	HMODULE hMod;

    NKDbgPrintfW(L"******************************************\n\r");
    NKDbgPrintfW(L"*********    Program Start  **************\n\r");
    NKDbgPrintfW(L"******************************************\n\r");

    if (getPROCID() == TYPE_TEGRA2)
    {
        // Set process affinity in case we are running on WEC 7
        // to prevent conflicts caused by the multi-core environment
        hMod=LoadLibrary(L"coredll.dll");

        pCeSetProcessAffinity = (PFNCeSetProcessAffinity) GetProcAddress(hMod, L"CeSetProcessAffinity");

        if (pCeSetProcessAffinity)
        {
            processHandle = GetCurrentProcess();
            pCeSetProcessAffinity(processHandle, 1);
        }
    }

    // Store the virtual addresses for ease of access
    txBufferMaster = (DWORD *) VirtualAlloc(0, TEST_FRAMES * 4, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    rxBufferMaster = (DWORD *) VirtualAlloc(0, TEST_FRAMES * 4, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    txBufferSlave  = (DWORD *) VirtualAlloc(0, TEST_FRAMES * 4, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    rxBufferSlave  = (DWORD *) VirtualAlloc(0, TEST_FRAMES * 4, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    // Check that buffers were able to be allocated 
    if ((rxBufferSlave == NULL) ||
        (txBufferSlave == NULL) ||
        (rxBufferMaster == NULL) ||
        (txBufferMaster == NULL))
    {
        NKDbgPrintfW(L"Could not alloc buffers");
        return FALSE;
    }

    // Run the demo code
    result = SpiMasterSlaveDemo();

    // Release buffers
    VirtualFree(txBufferMaster, 0, MEM_RELEASE);
    VirtualFree(rxBufferMaster, 0, MEM_RELEASE);
    VirtualFree(txBufferSlave, 0, MEM_RELEASE);
    VirtualFree(rxBufferSlave, 0, MEM_RELEASE);

    return result;
}
