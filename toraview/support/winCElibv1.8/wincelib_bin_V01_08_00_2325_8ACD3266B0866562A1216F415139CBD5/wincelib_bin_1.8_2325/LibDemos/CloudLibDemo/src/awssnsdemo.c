/// @file       awssnsdemo.c
/// @copyright  Copyright (c) 2012 Toradex AG
///             [Software License Agreement]
/// $Author: agp $
/// $Rev: 1181 $
/// $Date: 2012-12-06 06:12:11 +0100 (Do, 06 Dez 2012) $
/// @brief      Contains example code, how to access AWS Cloud SNS Services using Toradex Sns Cloud Library
/// @target     Colibri Modules [PXAxxx, Tegra T20], WinCE 6.0 / 7.0
/// @caveats	

#include "cloud.h"

// Macros
#define AWS_ACCOUNT_ACCESS_ID					"AKIAJS7ESGJJTVQCNRDA"
#define AWS_ACCOUNT_SECRET_KEY					"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"
#define AWS_ACCOUNT_OWNER_ID					"823890622362"
#define AWS_SNS_SINGAPORE_REGION				"sns.us-east-1.amazonaws.com"
#define AWS_SQS_USA_EAST_REGION					"sns.us-east-1.amazonaws.com"
//After Login to your AWS account, you can find your security credentials in My Account/Console >> Security Credentials

#define BUFFER_SIZE_EMAIL_ID					255
#define BUFFER_SIZE_TOPIC_DATA					512
#define BUFFER_SIZE_SUBSCRIPTION_DATA			(1 * 1024)
#define BUFFER_SIZE_SUBSCRIPTION_BY_TOPIC_DATA	(1 * 1024)
#define BUFFER_SIZE_TOPIC_ATTRIBUTE_DATA		(3 * 1024)
#define BUFFER_SIZE_SUBSCRIPTION_ATTRIBUTE_DATA	(3 * 1024)

// Switch cases
#define OPEN_TOPIC								0
#define CREATE_TOPIC							1
#define LIST_TOPICS								2
#define LIST_SUBSCRIPTION        				3
#define DELETE_TOPIC							4
#define SUBSCRIBE								5
#define UNSUBSCRIBE								6
#define GET_SUBSCRIPTION_ATTRIBUTES				7
#define GET_TOPIC_ATTRIBUTES					8
#define SET_TOPIC_ATTRIBUTES					9
#define PUBLISH									10
#define CLOSE_TOPIC								11
#define EXIT									12

//******************************************************************************
/// Console Application main function
/// @param[in]    nothing
/// @retval       TRUE Success
/// @retval       FALSE Error
int _tmain(void);

//******************************************************************************
int _tmain(void)
{
	HSNS NewTopic;
	BYTE firstChoice = 0;
	char enterTopicName[SNS_TOPIC_NAME_MAX_LENGTH + 1] = {0};
	char enterDisplayName[SNS_DISPLAY_NAME_MAX_LENGTH + 1] = {0};
	char enterEmailId[BUFFER_SIZE_EMAIL_ID + 1] = {0};
	char topicData[BUFFER_SIZE_TOPIC_DATA] = {0};
	DWORD listTopicNextPointer = 0;
	char subscriptionByTopicData[BUFFER_SIZE_SUBSCRIPTION_BY_TOPIC_DATA] = {0};
	DWORD listSubscriptionByTopicNextPointer = 0;
	DWORD fileSize = 0;
	char topicAttributesData[BUFFER_SIZE_TOPIC_ATTRIBUTE_DATA] = {0};
	char subscriptionAttributesData[BUFFER_SIZE_SUBSCRIPTION_ATTRIBUTE_DATA] = {0};
	BYTE attributeType = 0;

    DWORD majorVersion = 0;
    DWORD minorVersion = 0;
    DWORD buildVersion = 0;

	while (TRUE)
	{
		printf("\n\n\n*****************************************\n");
		printf("Toradex Systems India Pvt. Ltd, Bangalore\n");
		printf("Amazon Cloud SNS API for Colibri Modules\n");
		printf("*****************************************\n");
        CloudGetLibVersion(&majorVersion, &minorVersion, &buildVersion);
        printf("Sns Library Version = %d.%d.%d\n", majorVersion, minorVersion, buildVersion); 
		printf("[0] Open Topics\n");					
		printf("[1] Create Topic\n");					
		printf("[2] List Topics\n");					
		printf("[3] List Subscriptions By Topic\n");	
		printf("[4] Delete Topic\n");					
		printf("[5] Subscribe [Email]\n");				
		printf("[6] Unsubscribe\n");	                // User need to enter Subscription Arn
		printf("[7] Read Subscription Attributes\n");   // User need to enter Subscription Arn
		printf("[8] Read Topic Attributes\n");			
		printf("[9] Set Topic Attributes\n");			// Only set TopicDisplayName works
		printf("[10]Publish\n");						
		printf("[11]Close Topic\n");					
		printf("[12]Exit\n");							
		printf("Enter your choice:");
		scanf_s("%2d", &firstChoice);

		if ((firstChoice > EXIT))
		{
			printf("Only Enter Numbers : 0 - 13....Program needs to Exit, Press Enter\n");
			getchar();
			return FALSE;
		}

		switch(firstChoice)
		{
		case OPEN_TOPIC:
			printf("Enter Topic Name (256 Character Max, Alphanumeric Char, \"-\", \"_\" only): ");
			scanf_s("%s", &enterTopicName);
			printf("Enter Topic Display Name (100 Character Max, Alphanumeric Char, \"-\", \"_\" only): ");
			scanf_s("%s", &enterDisplayName);
			if (!SnsOpenTopic(AWS_ACCOUNT_ACCESS_ID, AWS_ACCOUNT_SECRET_KEY, AWS_ACCOUNT_OWNER_ID, AWS_SQS_USA_EAST_REGION, enterTopicName, enterDisplayName, &NewTopic))
			{
				printf("SnsOpenTopic Fail");
				printf(", Error Code: %d\n", GetLastError());
				break;
			}
			else 
			{
				printf("SnsOpenTopic Success\n");
			}
			break;

		case CREATE_TOPIC:
			if (!SnsCreateTopic(NewTopic))
			{
				printf("SnsCreateTopic Fail");
				printf(", Error Code: %d\n", GetLastError());
				break;
			}
			else 
			{
				printf("SnsCreateTopic Success\n");
			}

			attributeType = ATTRIBUTE_TYPE_DISPLAYNAME;
			if (!SnsSetTopicAttributes(NewTopic, attributeType, enterDisplayName))
			{
				printf("SnsSetTopicAttributes Fail");
				printf(", Error Code: %d\n", GetLastError());
				break;
			}
			else 
			{
				printf("SnsSetTopicAttributes Success\n");
			}
			break;

		case LIST_TOPICS:
			listTopicNextPointer = 0;
			printf("\n");
			// Check if listoftopics.txt file already exists Delete File
			if (!DeleteFile(TEXT("\\listoftopics.txt")))
			{
				if (GetLastError() != ERROR_FILE_NOT_FOUND)
				{
					printf("File Delete Error: ");
					printf("%d\n", GetLastError());
				}
			}

			if (!SnsListTopics(NewTopic, "\\listoftopics.txt"))
			{
				printf("SnsListTopics Fail");
				printf(", Error Code: %d\n", GetLastError());
				break;
			}
			else 
			{
				printf("SnsListTopics Success\n");
			}

			// Get file size
			if (!GetLocalFileSizeInBytes("\\listoftopics.txt", &fileSize))
			{
				printf("File not found\n");
				break;
			}

			while (TRUE)
			{
				memset(topicData, 0, BUFFER_SIZE_TOPIC_DATA);
				if (SnsFindTopic("\\listoftopics.txt", topicData, BUFFER_SIZE_TOPIC_DATA, &listTopicNextPointer, fileSize))	
				{
					printf("> %s\n", topicData);
				}
				else 
				{
					break;
				}
			}
			break;

		case LIST_SUBSCRIPTION:
			listSubscriptionByTopicNextPointer = 0;
			printf("\n");
			// Check if ListSubscriptions.txt file already exists Delete File
			if (!DeleteFile(TEXT("\\ListSubscriptionsByTopic.txt")))
			{
				if (GetLastError() != ERROR_FILE_NOT_FOUND)
				{
					printf("File Delete Error: ");
					printf("%d\n", GetLastError());
				}
			}

			if (!SnsListSubscriptions(NewTopic, "\\ListSubscriptionsByTopic.txt"))
			{
				printf("SnsListSubscriptions Fail");
				printf(", Error Code: %d\n", GetLastError());
				break;
			}
			else
			{
				printf("SnsListSubscriptions Success\n");
			}

			// Get file size
			if (!GetLocalFileSizeInBytes("\\ListSubscriptionsByTopic.txt", &fileSize))
			{
				printf("File not found\n");
				break;
			}

			while (TRUE)
			{
				memset(subscriptionByTopicData, 0, BUFFER_SIZE_SUBSCRIPTION_BY_TOPIC_DATA);
				if (SnsFindSubscriptions("\\ListSubscriptionsByTopic.txt", subscriptionByTopicData, BUFFER_SIZE_SUBSCRIPTION_BY_TOPIC_DATA, &listSubscriptionByTopicNextPointer, fileSize))	printf("> %s\n", subscriptionByTopicData);
				else 
				{
					break;
				}
			}
			break;
		case DELETE_TOPIC:
			if (!SnsDeleteTopic(NewTopic))
			{
				printf("SnsDeleteTopic Fail");
				printf(", Error Code: %d\n", GetLastError());
				break;
			}
			else 
			{
				printf("SnsDeleteTopic Success\n");
			}
			break;

		case SUBSCRIBE:
			printf("Enter Email Id: ");
			scanf_s("%s", &enterEmailId);
			if (!SnsSubscribe(NewTopic, enterEmailId, PROTOCOL_TYPE_EMAIL))
			{
				printf("SnsSubscribe Fail");
				printf(", Error Code: %d\n", GetLastError());
				break;
			}
			else 
			{
				printf("SnsSubscribe Success\n");
			}
			break;

		case UNSUBSCRIBE:
			if (!SnsUnsubscribe(NewTopic, "arn:aws:sns:us-east-1:823890622362:USA_NewAlerts-1:e86e12f4-ccb7-4153-acd2-33a2c8e30de5"))
			{
				printf("SnsUnsubscribe Fail");
				printf(", Error Code: %d\n", GetLastError());
				break;
			}
			else 
			{
				printf("SnsUnsubscribe Success\n");
			}
			break;

		case GET_SUBSCRIPTION_ATTRIBUTES:
			memset(subscriptionAttributesData, 0, BUFFER_SIZE_SUBSCRIPTION_ATTRIBUTE_DATA);
			if (!SnsGetSubscriptionAttribtues(NewTopic, "arn:aws:sns:us-east-1:823890622362:test:81bb4313-b150-48bf-8fd5-cf98a6a85535", subscriptionAttributesData, BUFFER_SIZE_SUBSCRIPTION_ATTRIBUTE_DATA))
			{
				printf("SnsGetSubscriptionAttribtues Fail");
				printf(", Error Code: %d\n", GetLastError());
				break;
			}
			else 
			{
				printf("%s\n", subscriptionAttributesData);
				printf("SnsGetSubscriptionAttribtues Success\n");
			}
			break;

		case GET_TOPIC_ATTRIBUTES:
			memset(topicAttributesData, 0, BUFFER_SIZE_TOPIC_ATTRIBUTE_DATA);
			if (!SnsGetTopicAttributes(NewTopic, topicAttributesData, BUFFER_SIZE_TOPIC_ATTRIBUTE_DATA))
			{
				printf("SnsGetTopicAttributes Fail");
				printf(", Error Code: %d\n", GetLastError());
				break;
			}
			else 
			{
				printf("%s\n", topicAttributesData);
				printf("SnsGetTopicAttributes Success\n");
			}
			break;

		case SET_TOPIC_ATTRIBUTES:
			printf("Enter Attribute Type: ");
			scanf_s("%d", &attributeType);

			printf("Enter Attribute Value: ");
			scanf_s("%s", &enterDisplayName);

			if (!SnsSetTopicAttributes(NewTopic, attributeType, enterDisplayName))
			{
				printf("SnsSetTopicAttributes Fail");
				printf(", Error Code: %d\n", GetLastError());
				break;
			}
			else 
			{
				printf("SnsSetTopicAttributes Success\n");
			}
			break;

		case PUBLISH:
			if (!SnsPublish(NewTopic, "Hello", "I am message, sent by Toradex SNS Cloud Library :)"))
			{
				printf("SnsPublish Fail");
				printf(", Error Code: %d\n", GetLastError());
				break;
			}
			else 
			{
				printf("SnsPublish Success\n");
			}
			break;

		case CLOSE_TOPIC:
			if (!SnsCloseTopic(&NewTopic))
			{
				printf("SnsCloseTopic Fail");
				printf(", Error Code: %d\n", GetLastError());
				break;
			}
			else 
			{
				printf("SnsCloseTopic Success\n");
			}
			break;

		case EXIT:
			return TRUE;
			break;
		}// switch
	}// while
}// main
