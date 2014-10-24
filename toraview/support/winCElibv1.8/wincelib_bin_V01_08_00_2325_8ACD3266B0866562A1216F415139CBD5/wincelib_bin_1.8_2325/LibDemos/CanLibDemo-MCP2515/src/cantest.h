/// @file        cantest.h
/// @copyright   Copyright (c) 2012 Toradex AG
///              [Software License Agreement]
/// $Author: kia $
/// $Rev: 2085 $
/// $Date: 2014-03-06 16:42:02 +0100 (Do, 06 Mrz 2014) $
/// @brief       Contains function declaration along \n
///              with register information macros, and other variables\n
/// @target      Colibri modules\n
/// Tested on    Colibri PXAxxx on Windows Embedded CE 6.0
/// @caveats    

#ifndef _CANTEST_H_
#define _CANTEST_H_

//******************************************************************************

#define NUMBER_OF_TEST_MESSAGES (8)

#define CAN_ID                  (0x100E8000)    ///< CAN identifier

#define CAN_MESSAGE_LENGTH      (8)             ///< CAN message length

#define RECEIVE_TIMEOUT         (5000)          ///< Receive timeout in ms

#define WATER_MARK              (1)             ///< Water mark

#define BUFFER_SIZE             (10000)         ///< Buffer size

///@name  CAN descriptor and mask
///@{

/// Only allowed standard messages with id 1000XXXXXXX
#define CAN_DESCRIPTOR1         (0x80000000)
#define CAN_MASK1               (0x0fff000f)

/// Only allowed standard messages with id 10000010XXX
#define CAN_DESCRIPTOR2         (0x82000000)
#define CAN_MASK2               (0x0000000f)

/// Only allowed extended messages with id 10000000011 101XX XXXXXXXXXXXXX
#define CAN_DESCRIPTOR3         (0x80740000)
#define CAN_MASK3               (0x00030000)

/// All messages
#define CAN_DESCRIPTOR          (0x80000000)
#define CAN_MASK                (0xFFFFFFFF)
///@}

#endif /* _CANTEST_H_ */
