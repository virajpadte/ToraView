/// @file       awss3demo.c
/// @copyright  Copyright (c) 2012 Toradex AG
///             [Software License Agreement]
/// $Author: gap $
/// $Rev: 1696 $
/// $Date: 2013-06-10 10:10:02 +0200 (Mo, 10 Jun 2013) $
/// @brief      Contains main function and call back function used in example code
/// @target     Colibri Modules [PXAxxx, Tegra T20], WinCE 6.0 / 7.0
/// @caveats   

#include "cloud.h"

// AWS Account Credentials
#define AWS_ACCESS_ID   "AKIAJS7ESGJJTVQCNRDA"                      // AWS Account Access ID
#define AWS_SECRET_KEY  "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"  // AWS Secret Key 
#define AWS_S3_PATH     "s3-ap-southeast-1.amazonaws.com"           // AWS S3 Access Point

// Switch cases
#define LIST_ALL_BUCKETS            1 
#define LIST_INSIDE_BUCKET          2
#define MAKE_DIRECTORY              3
#define REMOVE_DIRECTORY            4
#define DELETE_FILE                 5
#define DOWNLOAD_FILE               6
#define UPLOAD_FILE                 7
#define CLOSE_S3_HANDLE             8
#define EXIT                        9

#define BUCKET_DATA_BUFFER_SIZE     1024
#define FILE_NAME_BUFFER_SIZE       255
#define BUCKET_FILES_BUFFER_SIZE    1024

//******************************************************************************
/// Call back function called by S3DownloadFile() or S3UploadFile()
/// @param[in]    bytesDone    Number of bytes already downloaded / uploaded
/// @param[in]    totalBytes   Total no of bytes to be downloaded / uploaded
/// @retval       nothing
/// @caveats      Please avoid doing long calculation inside this function otherwise S3DownloadFile() or S3UploadFile() may fail
void ProgressCallback(DWORD bytesDone, DWORD totalBytes);

//******************************************************************************
/// Console Application main function
/// @param[in]    nothing
/// @retval       TRUE Success
/// @retval       FALSE Error
int _tmain(void);

DWORD percent = 0;

int _tmain(void)
{
    HS3 s3Account1 = 0;
    char s3bucketData[BUCKET_DATA_BUFFER_SIZE] = {0};
    DWORD s3NoOfBuckets = 0;
    DWORD s3ListALLBucketsRawDatabytes = 0;
    DWORD nextBucketPointer = 0;
    DWORD s3ListBucketRawDatabytes = 0;
    char *s3KeyData = NULL;
    DWORD noofkeys = 0;
    DWORD nextFilePointer = 0;
    BOOL result = 0;
    DWORD selectionInput = 0;
    char path[FILE_NAME_BUFFER_SIZE] = {0};
    char path1[FILE_NAME_BUFFER_SIZE] = {0};
    DWORD uploadfilesize = 0;

    DWORD bytesDownloaded = 0;
    DWORD bytesUploaded = 0;

    if (!S3Open(&s3Account1, AWS_ACCESS_ID, AWS_SECRET_KEY, AWS_S3_PATH))
    {
        printf("S3Open Fails: Error Code:");
        printf("%d\n", GetLastError());
    }
    else
    {
        printf("S3Open Success\n");
    }
    while (TRUE == TRUE)
    {
        path[0] = 0;
        path1[0] = 0;

        printf("\n\n*****************************************\n");
        printf("Toradex Cloud S3 API for Colibri Modules\n");
        printf("*****************************************\n");
        printf("Select Operation\n");
        printf("(1)List All Buckets (2)List a Bucket (3)Make Directory (4)Remove Directory\n");
        printf("(5)Delete File      (6)Download File (7)Upload File    (8)Close Handle\n(9)Exit\n");
        printf("Please Enter you selection(1 - 9):");

        scanf_s("%1d", &selectionInput);
        if ((selectionInput == LIST_INSIDE_BUCKET) || (selectionInput == MAKE_DIRECTORY) || (selectionInput == REMOVE_DIRECTORY) || 
            (selectionInput == DELETE_FILE) || (selectionInput == DOWNLOAD_FILE) || (selectionInput == UPLOAD_FILE))
        {
            if ((selectionInput == DOWNLOAD_FILE) || (selectionInput == UPLOAD_FILE)) 
            {
                printf("Please Enter Source path:");
                scanf_s("%s", &path);
            }
            else 
            {
                printf("Please Enter Resource(Directory / File) path: ");
                scanf_s("%s", &path);
            }
        }

        if ((selectionInput == DOWNLOAD_FILE) || (selectionInput == UPLOAD_FILE)) 
        {
            printf("Please Enter Destination path: ");scanf_s("%s", &path1);
        }

        switch(selectionInput)
        {
        case LIST_ALL_BUCKETS:
            s3NoOfBuckets = 0;
            nextBucketPointer = 0;
            s3ListALLBucketsRawDatabytes = 0;

            //if allbklist.txt file already exists Delete File
            if (!DeleteFile(TEXT("\\Temp\\allbklist.txt")))
            {
                if (GetLastError() != ERROR_FILE_NOT_FOUND)
                {
                    printf("File Delete Error: ");
                    printf("%d\n", GetLastError());
                }
            }
            result = S3ListAllBuckets(s3Account1, 0, "\\Temp\\allbklist.txt", &s3ListALLBucketsRawDatabytes);
            if (result && (s3ListALLBucketsRawDatabytes > 0))
            {
                printf("List of Buckets\n");
                s3NoOfBuckets = 0;
                if (s3bucketData)
                {
                    while (TRUE)
                    {
                        memset(s3bucketData, 0, BUCKET_DATA_BUFFER_SIZE);
                        result = FindFirstBucket("\\Temp\\allbklist.txt", s3ListALLBucketsRawDatabytes, s3bucketData, BUCKET_DATA_BUFFER_SIZE, &nextBucketPointer);
                        if (result)
                        {
                            s3NoOfBuckets++;
                            printf("%3d %s\n", s3NoOfBuckets, s3bucketData);
                        }
                        else 
                        {
                            break;
                        }
                    }
                }
                else
                {
                    printf("Error allocating memory for List Bucket\n");
                }
            }
            else
            {
                printf("Error getting no of Bucket\n");
            }
            break;

        case LIST_INSIDE_BUCKET:
            s3ListBucketRawDatabytes = 0;
            nextFilePointer = 0;
            noofkeys = 0;
            // Allocate memory for keeping bucket details = Bucket Name & Creation Date
            s3KeyData = (char *)malloc(BUCKET_FILES_BUFFER_SIZE); // File in Bucket name length = 255 max, file size string = 10, and 10 extra

            // if bklist.txt file already exists Delete File
            if (!DeleteFile(TEXT("\\Temp\\bklist.txt")))
            {
                if (GetLastError() != ERROR_FILE_NOT_FOUND)
                {
                    printf("File Delete Error: ");
                    printf("%d\n", GetLastError());
                }
            }

            result = S3ListBucket(s3Account1, path, "\\Temp\\bklist.txt", &s3ListBucketRawDatabytes);
            if (result && (s3ListBucketRawDatabytes > 0)) 
            {
                while (TRUE)
                {
                    memset(s3KeyData, 0, (BUCKET_FILES_BUFFER_SIZE));
                    result = FindFirstFileInBucket("\\Temp\\bklist.txt",s3ListBucketRawDatabytes, s3KeyData, BUCKET_FILES_BUFFER_SIZE, &nextFilePointer);
                    if (!result) break;
                    noofkeys++;
                    printf("%4d %s", noofkeys, s3KeyData);
                }
            }
            else
            {
                printf("Error getting list of files in Bucket\n");
            }
            free(s3KeyData);
            s3KeyData = NULL;
            break;

        case MAKE_DIRECTORY:
            if (!S3MakeDirectory(s3Account1, path))
            {
                printf("S3MakeDirectory Fails: Error Code:");
                printf("%d\n", GetLastError());
            }
            else
            {
                printf("S3MakeDirectory Success\n");
            }
            break;

        case REMOVE_DIRECTORY:
            if (!S3RemoveDirectory(s3Account1, path))
            {
                printf("S3RemoveDirectory Fails: Error Code:");
                printf("%d\n", GetLastError());
            }
            else
            {
                printf("S3RemoveDirectory Success\n");
            }

            break;

        case DELETE_FILE:
            if (!S3DeleteFile(s3Account1, path))
            {
                printf("S3DeleteFile Fails: Error Code:");
                printf("%d\n", GetLastError());
            }
            else
            {
                printf("S3DeleteFile Success\n");
            }
            break;

        case DOWNLOAD_FILE:
            // Check if Destination File Already Exists or not
            if (S3DownloadFile(s3Account1, path, path1, 0, &bytesDownloaded, &ProgressCallback))
            {
                printf("S3DownloadFile Successful\n");
            }
            else
            {
                printf("S3DownloadFile Fails: Error Code: ");
                printf("%d\n", GetLastError());
            }
            break;

        case UPLOAD_FILE:

                printf("Checking File Size...\n");
                if (GetLocalFileSizeInBytes(path, &uploadfilesize))
                {
                    printf("File size = %d\n", uploadfilesize);
                }
                else
                {
                    printf("File not found\n");
                    break;
                }

                if (!S3UploadFile(s3Account1, path, uploadfilesize, path1, &bytesUploaded, &ProgressCallback))
                {
                    printf("UploadFile Fail\n");
                }
                else
                {
                    printf("UploadFile Success\n");
                }
                break;

        case CLOSE_S3_HANDLE:
            // if allbklist.txt file already exists Delete File
            if (!DeleteFile(TEXT("\\Temp\\allbklist.txt")))
            {
                if (GetLastError() != ERROR_FILE_NOT_FOUND)
                {
                    printf("File Delete Error: ");
                    printf("%d\n", GetLastError());
                }
            }

            // if allbklist.txt file already exists Delete File
            if (!DeleteFile(TEXT("\\Temp\\bklist.txt"))) 
            {
                if (GetLastError() != ERROR_FILE_NOT_FOUND)
                {
                    printf("File Delete Error: ");
                    printf("%d\n", GetLastError());
                }
            }
            S3Close(&s3Account1);
            break;

        case EXIT:
            return TRUE;

        }// switch
    }//while (TRUE)
    return 0;
}

//******************************************************************************
void ProgressCallback(DWORD bytesDone, DWORD totalBytes)
{
    printf("> %d bytes, %d \n", bytesDone, totalBytes);
}

