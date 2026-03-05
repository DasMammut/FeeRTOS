#ifndef FEERTOS_MAILBOX_H
#define FEERTOS_MAILBOX_H

#include <stdint.h>

typedef struct SFeeRTOS_Mailbox* TFeeRTOS_MailboxHandle;

TFeeRTOS_MailboxHandle FeeRTOS_CreateMailbox(uint8_t aMessageSize);

void FeeRTOS_DeleteMailbox(TFeeRTOS_MailboxHandle aMailbox);

void FeeRTOS_MailboxPost(TFeeRTOS_MailboxHandle aMailbox, const void* aMessage);

void FeeRTOS_MailboxPeek(TFeeRTOS_MailboxHandle aMailbox, void* aMessage);

#endif