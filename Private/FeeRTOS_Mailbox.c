#include "IFeeRTOS_Mailbox.h"

TFeeRTOS_MailboxHandle FeeRTOS_CreateMailbox(uint8_t aMessageSize) {
    if (aMessageSize == 0) return NULL;
    TFeeRTOS_MailboxHandle mailbox = (TFeeRTOS_MailboxHandle)FeeRTOS_Malloc(sizeof(TFeeRTOS_Mailbox));
    if (mailbox == NULL) return NULL;

    mailbox->Message = FeeRTOS_Malloc(aMessageSize);
    if (mailbox->Message == NULL) {
        FeeRTOS_Free(mailbox);
        return NULL;
    }
    mailbox->MessageSize = aMessageSize;
    return mailbox;
}

void FeeRTOS_DeleteMailbox(TFeeRTOS_MailboxHandle aMailbox) {
    if (aMailbox == NULL) return;
    FeeRTOS_Free(aMailbox->Message);
    FeeRTOS_Free(aMailbox);
}

void FeeRTOS_MailboxPost(TFeeRTOS_MailboxHandle aMailbox, const void* aMessage) {
    if (aMailbox == NULL || aMessage == NULL) return;
    FeeRTOS_ENTER_CRITICAL();
    memcpy(aMailbox->Message, aMessage, aMailbox->MessageSize);
    FeeRTOS_EXIT_CRITICAL();
}

void FeeRTOS_MailboxPeek(TFeeRTOS_MailboxHandle aMailbox, void* aMessage) {
    if (aMailbox == NULL || aMessage == NULL) return;
    FeeRTOS_ENTER_CRITICAL();
    memcpy(aMessage, aMailbox->Message, aMailbox->MessageSize);
    FeeRTOS_EXIT_CRITICAL();
}