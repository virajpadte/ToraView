/********************************************************
	Small Demo App for the UpdateLib.
	For an interface describtion see UpdateLib.h
*********************************************************/

#include <windows.h>
#include "UpdateLib.h"

BOOL ReadFileProgress(DWORD done, DWORD totsize)
{
    // ADD your callback code here
    return TRUE;
}

BOOL EraseProgress(DWORD done, DWORD totsize)
{
    // ADD your callback code here
    return TRUE;
}

BOOL SectionInfo(DWORD SectionStart, DWORD SectionLen, DWORD SectionCRC)
{
    // ADD your callback code here
    return TRUE;
}

BOOL WriteProgress(DWORD writtenSectionLen, DWORD writtenTotalLen)
{
    // ADD your callback code here
    wprintf(TEXT("."));
    return TRUE;
}

BOOL ImageInfo(UDL_REGIONINFO *imgInfo)
{
    // ADD your callback code here
    return TRUE;
}

BOOL DoUpdate(TCHAR *path)
{
    BOOL ret = TRUE;
    UDL_REGIONINFO RegInfo;
    UDL_REGION_ID AllowedRegions;

    // initialize the update lib
    if(UDLInit()!=UDL_SUCCESS)
    {
        wprintf(TEXT("Error: Could not initialize UpdateLib\r\n"));
        return FALSE;
    }

    // set the binfile and get the region info of this file
    if(UDLSetBinFile(path, &RegInfo, ReadFileProgress, TRUE ) != UDL_SUCCESS)
    {
        wprintf(TEXT("Error: Invalid file path or format\r\n"));
        ret = FALSE;
        goto exitDoUpdate;
    }

    // update the current region info on the flash
    UDLUpdateRegInfo(UDL_ALL_REGIONS);

    // check the range of this file
    AllowedRegions = UDL_ALL_REGIONS;// & ~UDL_FILE_SYSTEM;  // e.g. File system  update not allowed
    if(UDLCheckRange(&AllowedRegions, RegInfo.start, RegInfo.length)!=UDL_SUCCESS)
    {
        wprintf(TEXT("Error: Range check failed.\r\n"));
        ret = FALSE;
        goto exitDoUpdate;
    }

    if(RegInfo.flags & UDL_F_FAILSAFE)
    { // this bin file contains data that would overwrite the first bootloader. therefore the startaddress gets shifted to bl2start automatically
        if(AllowedRegions & ~UDL_2ND_BOOTLOADER)
        { // we overwrite more than just the second bootloader, we do not allow this!
            wprintf(TEXT("Updating with this bin file is not allowed since the FailSafe feature is enabled!\r\n"));
            goto exitDoUpdate;
        }
    }

    if(RegInfo.id & UDL_CPLD) // CPLD is also overwritten
        AllowedRegions |= UDL_CPLD;

    {
        TCHAR scratch[128];
        wcscpy(scratch,L"This Image will overwrite the following sections:\n");
    
        if(AllowedRegions & UDL_BOOTLOADER)
        {
            wcscat(scratch, TEXT("- Bootloader\r\n"));
        }
        if(AllowedRegions & UDL_2ND_BOOTLOADER) 
        {
            wcscat(scratch, TEXT("- Second Bootloader\r\n"));
        }
        if(AllowedRegions & UDL_CONFIG_BLOCK)
        {
            wcscat(scratch, TEXT("- ConfigBlock\r\n"));
        }
        if(AllowedRegions & UDL_OS_IMAGE) 
        {
            wcscat(scratch, TEXT("- OS Image\r\n"));
        }
        if(AllowedRegions & UDL_REGISTRY) 
        {
            wcscat(scratch, TEXT("- Registry\r\n"));
        }
        if(AllowedRegions & UDL_FILE_SYSTEM) 
        {
            wcscat(scratch, TEXT("- FileSystem\r\n"));
        }
        if(AllowedRegions & UDL_CPLD) 
        {
            wcscat(scratch, TEXT("- CPLD\r\n"));
        }
        if(AllowedRegions == UDL_NO_REGION)
        {
            wcscat(scratch, TEXT("- No Section\r\n"));
        }
        wcscat(scratch, L"\r\n");
        wprintf(scratch);
    }

    // perform the update
    wprintf(TEXT("Performing Update"));
    if(UDLDoUpdate( EraseProgress, SectionInfo, WriteProgress)==UDL_SUCCESS)
    {
        wprintf(TEXT("\r\nUpdate successful\r\n"));
    }
    else
    {
        wprintf(TEXT("\r\nError: During update.\r\n"));
        ret = FALSE;
        goto exitDoUpdate;
    }

exitDoUpdate:
    UDLReleaseBinFile();
    UDLDeInit();
    return ret;
}


BOOL DoBackup(TCHAR *cmd)
{
    UDL_REGIONINFO regInfoTotal;
    BOOL ret = TRUE;
    DWORD i=0;

    UDL_REGION_ID backupRegions = UDL_NO_REGION;

    // get the regions to update out of the cmd string
    while(cmd[i])
    {
        if(cmd[i]==' ')
        {
            i++;
            break;
        }

        switch(cmd[i])
        {
        case '0':
            backupRegions |= UDL_BOOTLOADER;
            break;
        case '1':
            backupRegions |= UDL_CONFIG_BLOCK;
            break;
        case '2':
            backupRegions |= UDL_OS_IMAGE;
            break;
        case '3':
            backupRegions |= UDL_REGISTRY;
            break;
        case '4':
            backupRegions |= UDL_FILE_SYSTEM;
            break;
        case '5':
            backupRegions |= UDL_2ND_BOOTLOADER;
            break;
        }

        i++;
    }

    // initialize the update lib
    if(UDLInit()!=UDL_SUCCESS)
    {
        wprintf(TEXT("Error: Could not initialize UpdateLib\r\n"));
        return FALSE;
    }

    // update the current region info (read from flash)
    UDLUpdateRegInfo(UDL_ALL_REGIONS);

    // get information about the specified regions
    regInfoTotal.id = backupRegions;
    if(UDLGetRegionInfo(&regInfoTotal, NULL)!=UDL_SUCCESS)
    {
        wprintf(TEXT("Error: GetRegionInfo failed\r\n"));
        ret = FALSE;
        goto exitDoBackup;
    }

    // check if the regions are contiguous (the regions always have to be contiguous!!)
    {
        UDL_RET_TYPE uldRet;
        uldRet = UDLCheckRange(&backupRegions, regInfoTotal.start, regInfoTotal.length);
        if(uldRet)
        {
            if (uldRet == UDL_RANGE_VERIFICATION_FAILED)
            {
                wprintf(TEXT("Error: Sections have to be contiguous!\r\n"));
                ret = FALSE;
                goto exitDoBackup;
            }
            else
            {
                wprintf(TEXT("Error: CheckRange failed.\r\n"));
                ret = FALSE;
                goto exitDoBackup;
            }
        }
    }


    // perform the backup
    wprintf(TEXT("Performing Backup"));
    if(UDLBackupToFile( &cmd[i], backupRegions, ImageInfo, SectionInfo, WriteProgress)==UDL_SUCCESS)
    {
        wprintf(TEXT("\r\nBackup successful\r\n"));
    }
    else
    {
        wprintf(TEXT("\r\nError: During backup.\r\n"));
        ret = FALSE;
        goto exitDoBackup;
    }

exitDoBackup:
    UDLDeInit();
    return ret;
}

ShowHelp()
{
    wprintf(TEXT("CommandLine Parameter:\r\n"));
    wprintf(TEXT("/f              Save Registry to Flash\r\n"));
    wprintf(TEXT("/c              Clear Registry on Flash\r\n"));
    wprintf(TEXT("/u binFilePath  Perform Update with binFilePath\r\n"));
    wprintf(TEXT("/bx binFile     Create backup of region x and write to binFile\r\n"));
    wprintf(TEXT("                x = 0: Bootloader\r\n"));
    wprintf(TEXT("                x = 1: ConfigBlock\r\n"));
    wprintf(TEXT("                x = 2: OS Image\r\n"));
    wprintf(TEXT("                x = 3: Registry\r\n"));
    wprintf(TEXT("                x = 4: FileSystem\r\n"));
    wprintf(TEXT("                x = 5: Second Bootloader\r\n"));
    wprintf(TEXT("                x can also be multiple numbers. eg. /b012\r\n"));
    wprintf(TEXT("/r              Perform warm-boot\r\n"));
    wprintf(TEXT("/rc             Perform cold-boot\r\n"));
}


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd )
{
    DWORD i=0;

    wprintf(TEXT("\r\n"));
    wprintf(TEXT("Toradex UpdateLib V%d.%d.%d Demo Tool\r\n"), UDL_VER_MAJ, UDL_VER_MIN, UDL_VER_BUILD);
    wprintf(TEXT("****************************************\r\n\r\n"));

	if(lpCmdLine[i++]=='/')
	{
        switch(lpCmdLine[i++])
        {
        case 'f':  // save the registry to flash
			if(UDLSaveRegistry() == UDL_SUCCESS)
                wprintf(TEXT("Registry saved successfully\r\n"));
            else
                wprintf(TEXT("Registry save error\r\n"));
            break;
        
        case 'c':  // clear the registry on flash
            if(UDLClearRegistry() == UDL_SUCCESS)
                wprintf(TEXT("Registry cleared successfully\r\n"));
            else
                wprintf(TEXT("Registry clear error\r\n"));
            break;

        case 'r':  // reboot the system
            {
                BOOL clean;

                clean = (lpCmdLine[i]=='c');
                UDLReboot(clean);
            }
            break;

        case 'u':  // perform an update
            DoUpdate(&lpCmdLine[i+1]);
            break;

        case 'b':  // perform a backup
            DoBackup(&lpCmdLine[i]);
            break;
        
        default:
            ShowHelp();
            break;
		}
    }
    else
        ShowHelp();

    wprintf(TEXT("\r\n"));
	return TRUE;
}
