#ifndef I_FEERTOS_MAILBOX_H
#define I_FEERTOS_MAILBOX_H

#include "FeeRTOS_Mailbox.h"
#include "IFeeRTOS.h"

typedef struct SFeeRTOS_Mailbox {
    void* Message;
    uint8_t MessageSize;
} TFeeRTOS_Mailbox;

#endif