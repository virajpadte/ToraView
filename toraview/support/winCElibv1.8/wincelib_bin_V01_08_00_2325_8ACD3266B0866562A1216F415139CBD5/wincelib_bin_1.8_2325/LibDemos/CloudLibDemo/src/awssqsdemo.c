/// @file       awssqsdemo.c
/// @copyright  Copyright (c) 2012 Toradex AG
///             [Software License Agreement]
/// $Author: gap $
/// $Rev: 1696 $
/// $Date: 2013-06-10 10:10:02 +0200 (Mo, 10 Jun 2013) $
/// @brief      Contains example code, how to access AWS Cloud SQS Services using Toradex Sqs Cloud Library
/// @target     Colibri Modules [PXAxxx, Tegra T20], WinCE 6.0 / 7.0
/// @caveats	

// Header Files
#include "cloud.h"

/// Macros
#define AWS_ACCOUNT_ACCESS_ID					"AKIAJS7ESGJJTVQCNRDA"
#define AWS_ACCOUNT_SECRET_KEY					"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"
#define AWS_ACCOUNT_OWNER_ID					"823890622362"
#define AWS_SQS_SINGAPORE_REGION				"ap-southeast-1.queue.amazonaws.com"
#define AWS_SQS_USA_EAST_REGION					"queue.amazonaws.com"

// Switch cases
#define OPEN_QUEUE								0
#define CLOSE_QUEUE								1
#define CREATE_QUEUE							2
#define DELETE_QUEUE							3
#define GET_QUEUE_URL							4 
#define LIST_QUEUE								5 
#define SEND_MESSAGE_TO_QUEUE					6
#define RECEIVE_MESSAGE_FROM_QUEUE				7 
#define DELETE_MESSAGE							8
#define GET_QUEUE_ATTRIBUTES					9
#define SET_QUEUE_ATTRIBUTES					10 
#define CHANGE_MESSAGE_VISIBILITY_TIMEOUT		11
#define EXIT									12 

#define QUEUE_LIST_DATA_BUFFER_SIZE				1024
#define QUEUE_URL_DATA_BUFFER_SIZE				255
#define QUEUE_ATTRIBUTES_BUFFER_SIZE			1024

//******************************************************************************
/// Console Application main function
/// @param[in]    nothing
/// @retval       TRUE Success
/// @retval       FALSE Error
int _tmain(void);

//******************************************************************************
int _tmain(void)
{
    HSQS SendQueue = NULL;
    BYTE firstChoice = 0;
    char queueUrlData[QUEUE_URL_DATA_BUFFER_SIZE] = {0};
    char queueListData[QUEUE_LIST_DATA_BUFFER_SIZE] = {0};
    char queueAttributesData[QUEUE_ATTRIBUTES_BUFFER_SIZE] = {0};
    DWORD listFilesize = 0;
    DWORD listQueueNextPointer = 0;
    char queueName[SQS_QUEUE_NAME_MAX_LENGTH + 1] = {0};
    DWORD attributeType = 0;
    int attributeValue = 0;
    char sendMessageID[SQS_MESSAGE_ID_LENGTH + 1] = {0}; // Send Message Response buffer, maximum 100 byte message Id

    char receiveMessageID[SQS_MESSAGE_ID_LENGTH + 1] = {0}; // Receive Message Response, maximum 100 byte message Id
    char *receiveMessageBody = NULL; // pointer to the received Message , max 64KB message body
    char recieptHandle[SQS_RECEIPT_HANDLE_MAX_LENGTH + 1] = {0}; // Maximum 1024 bytes recieptHandle

    DWORD majorVersion = 0;
    DWORD minorVersion = 0;
    DWORD buildVersion = 0;

    while (TRUE)
    {
        printf("\n*****************************************\n");
        printf("Toradex Systems India Pvt. Ltd, Bangalore\n");
        printf("Amazon Cloud SQS API for Colibri Modules\n");
        printf("*****************************************\n");
        CloudGetLibVersion(&majorVersion, &minorVersion, &buildVersion);
        printf("Sqs Library Version = %d.%d.%d\n", majorVersion, minorVersion, buildVersion); 
        printf("[0] Open Queue\n");					
        printf("[1] Close Queue\n");					
        printf("[2] Create Queue\n");	
        printf("[3] Delete Queue\n");	
        printf("[4] Get Queue Url\n");
        printf("[5] List Queue\n");	
        printf("[6] Send Message\n");	
        printf("[7] Receive Message\n");
        printf("[8] Delete Message\n");
        printf("[9] Get Queue Attributes\n");
        printf("[10] Set Queue Attributes\n");	
        printf("[11] Change Message Visibility Timeout\n");	
        printf("[12] Exit\n");						
        printf("Select:");
        scanf_s("%2d", &firstChoice);
        if ((firstChoice > EXIT))
        {
            printf("Only Enter Numbers: 0 - 12....Program needs to Exit, Press Enter\n");
            getchar();
            return FALSE;
        }

        switch(firstChoice)
        {
        case OPEN_QUEUE:
            printf("Enter Queue Name:");
            scanf_s("%s", &queueName);
            if (!SqsOpenQueue(AWS_ACCOUNT_ACCESS_ID, AWS_ACCOUNT_SECRET_KEY, AWS_SQS_SINGAPORE_REGION, AWS_ACCOUNT_OWNER_ID, queueName, &SendQueue))
            {
                printf("SqsOpenQueue Fails: Error Code: ");
                printf("%d\n", GetLastError());
            }
            else 
            {
                printf("SqsOpenQueue Success\n");

                //allocate memory for receive message
                receiveMessageBody = (char *)malloc(SQS_MESSAGE_BODY_MAX_LENGTH + 1);
                if(!receiveMessageBody)
                {
                    printf("Error: Not enough memory for receive message");
                    break;
                }
            }
            break;

        case CLOSE_QUEUE:
            // free allocated memory to receive message 
            free(receiveMessageBody);
            receiveMessageBody = NULL;

            if (!SqsCloseQueue(&SendQueue))
            {
                printf("SqsCloseQueue Fails: Error Code: ");
                printf("%d\n", GetLastError());
                break;
            }
            else
            {
                printf("SqsCloseQueue Success\n");
            }
            break;

        case CREATE_QUEUE:
            if (!SqsCreateQueue(SendQueue))
            {
                printf("SqsCreateQueue Fails: Error Code: ");
                printf("%d\n", GetLastError());
                break;
            }
            else
            {
                printf("SqsCreateQueue Success\n");
            }
            break;

        case DELETE_QUEUE:
            if (!SqsDeleteQueue(SendQueue))
            {
                printf("SqsDeleteQueue Fails: Error Code: ");
                printf("%d\n", GetLastError());
                break;
            }
            else 
            {
                printf("SqsDeleteQueue Success\n");
            }
            break;

        case GET_QUEUE_URL:
            memset(queueUrlData, 0, QUEUE_URL_DATA_BUFFER_SIZE);
            if (!SqsGetQueueUrl(SendQueue, queueUrlData, 255))
            {
                printf("SqsGetQueueUrl Fails: Error Code: ");
                printf("%d\n", GetLastError());
                break;
            }
            else 
            {
                printf("SqsGetQueueUrl Success\n");
                printf("> %s\n", queueUrlData);
            }
            break;

        case LIST_QUEUE:
            // Check if file already exists, Delete File
            if (!DeleteFile(TEXT("\\queueList.xml")))
            {
                if (GetLastError() != ERROR_FILE_NOT_FOUND)
                {
                    printf("File Delete Error: ");
                    printf("%d\n", GetLastError());
                }
            }

            // Get list data in a local file
            if (!SqsListQueues(SendQueue, "", "\\queueList.xml"))
            {
                printf("SqsListQueues Fails: Error Code: ");
                printf("%d\n", GetLastError());
                break;
            }
            else 
            {
                printf("SqsListQueues Success\n");
            }

            // Get file size
            if (!GetLocalFileSizeInBytes("\\queueList.xml", &listFilesize))
            {
                printf("Error\n");
            }
            else
            {
                printf("File Size = %d\n", listFilesize);
            }

            // Print list of Queues
            listQueueNextPointer = 0;
            while (TRUE)
            {
                memset(queueListData, 0, QUEUE_LIST_DATA_BUFFER_SIZE);
                if (SqsFindQueue("\\queueList.xml", queueListData, QUEUE_LIST_DATA_BUFFER_SIZE, &listQueueNextPointer, listFilesize))	
                {
                    printf("> %s\n", queueListData);
                }
                else 
                {
                    break;
                }
            }
            break;

        case SEND_MESSAGE_TO_QUEUE:
            memset(sendMessageID, 0, SQS_MESSAGE_ID_LENGTH + 1); 
            if (!SqsSendMessage(SendQueue, "Hello, Message from Toradex Colibri T20, How are you doing", 0, sendMessageID, SQS_MESSAGE_ID_LENGTH)) // No Delay in delivery
            {
                printf("SqsSendMessage Fails: Error Code: ");
                printf("%d\n", GetLastError());
            }
            else 
            {
                printf("SqsSendMessage Success, ");
                printf("MessageID: %s\n", sendMessageID);
            }

            break;

        case RECEIVE_MESSAGE_FROM_QUEUE:
            memset(receiveMessageBody, 0, SQS_MESSAGE_BODY_MAX_LENGTH + 1);
            memset(receiveMessageID, 0, SQS_MESSAGE_ID_LENGTH + 1);
            memset(recieptHandle, 0, SQS_RECEIPT_HANDLE_MAX_LENGTH + 1);
            if (!SqsReceiveMessage(SendQueue, "ALL", 10, 30, receiveMessageBody, receiveMessageID, recieptHandle))
            {
                printf("SqsReceiveMessage Fails: Error Code: ");
                printf("%d\n", GetLastError());
            }
            else 
            {
                printf("SqsReceiveMessage Success\n");
                printf("MessageID:%s\n", receiveMessageID);
                printf("Received Message: %s\n", receiveMessageBody);
                printf("RecieptHandle: %s\n", recieptHandle);
            }
            break;

        case DELETE_MESSAGE:
            if (!SqsDeleteMessage(SendQueue, recieptHandle))
            {
                printf("SnsDeleteMessage Fails: Error Code: ");
                printf("%d\n", GetLastError());
            }
            else 
            {
                printf("SnsDeleteMessage Success\n");
            }
            break;

        case CHANGE_MESSAGE_VISIBILITY_TIMEOUT:
            if (!SqsChangeMessageVisibility(SendQueue, recieptHandle, 90)) // New Timeout setting
            {
                printf("SnsChangeMessageVisibility Fails: Error Code: ");
                printf("%d\n", GetLastError());
            }
            else 
            {
                printf("SnsChangeMessageVisibility Success\n");
            }
            break;

        case GET_QUEUE_ATTRIBUTES:
            memset(queueAttributesData, 0, QUEUE_ATTRIBUTES_BUFFER_SIZE);
            if (!SqsGetQueueAttributes(SendQueue, queueAttributesData, QUEUE_ATTRIBUTES_BUFFER_SIZE))
            {
                printf("SqsGetQueueAttributes Fails: Error Code: ");
                printf("%d\n", GetLastError());
                break;
            }
            else 
            {
                printf("SqsGetQueueAttributes Success\n");
                printf("> %s\n", queueAttributesData);
            }
            break;

        case SET_QUEUE_ATTRIBUTES:
            printf("Enter Attribute Type:");
            scanf_s("%d", &attributeType);

            printf("Enter Attribute Value:");
            scanf_s("%d", &attributeValue);

            if (!SqsSetQueueAttributes(SendQueue, attributeType, attributeValue))
            {
                printf("SqsSetQueueAttributes Fails: Error Code: ");
                printf("%d\n", GetLastError());
                break;
            }
            else 
            {
                printf("SqsSetQueueAttributes Success\n");
            }
            break;

        case EXIT:
            return TRUE;
            break;
        } // Switch
    } // while (TRUE)
}// main

