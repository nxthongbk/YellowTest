/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef YELLOW_TEST_MESSAGES_H_INCLUDE_GUARD
#define YELLOW_TEST_MESSAGES_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR "bf4a208dcef318279ea2539c1c94e1a6"

#ifdef MK_TOOLS_BUILD
    extern const char** yellow_test_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*yellow_test_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "yellow_test"
#endif


#define _MAX_MSG_SIZE 16

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_yellow_test_CheckSimState 0
#define _MSGID_yellow_test_MeasureSignalStrength 1
#define _MSGID_yellow_test_MainBusI2C 2
#define _MSGID_yellow_test_Port1HubI2C 3
#define _MSGID_yellow_test_Port2HubI2C 4
#define _MSGID_yellow_test_Port3HubI2C 5
#define _MSGID_yellow_test_AcceGyroRead 6


// Define type-safe pack/unpack functions for all enums, including included types

// Define pack/unpack functions for all structures, including included types


#endif // YELLOW_TEST_MESSAGES_H_INCLUDE_GUARD