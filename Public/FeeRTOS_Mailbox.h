#ifndef FEERTOS_MAILBOX_H
#define FEERTOS_MAILBOX_H

#include <stdint.h>

typedef struct SFeeRTOS_Mailbox* TFeeRTOS_MailboxHandle;

/**
 * FeeRTOS_CreateMailbox
 * Erstellt eine neue Mailbox mit angegebener Message-Groesse.
 * Gibt ein Handle zurueck oder NULL bei Fehler.
 *
 * aMessageSize - Groesse einer Nachricht in Bytes
 */
TFeeRTOS_MailboxHandle FeeRTOS_CreateMailbox(uint8_t aMessageSize);

/**
 * FeeRTOS_DeleteMailbox
 * Loescht eine Mailbox anhand ihres Handles.
 * Gibt alle wartenden Tasks frei.
 *
 * aMailbox - Handle der zu loeschenden Mailbox
 */
void FeeRTOS_DeleteMailbox(TFeeRTOS_MailboxHandle aMailbox);

/**
 * FeeRTOS_MailboxPost
 * Sendet eine Nachricht an die Mailbox.
 *
 * aMailbox  - Handle der Mailbox
 * aMessage  - Zeiger auf die zu sendende Nachricht
 */
void FeeRTOS_MailboxPost(TFeeRTOS_MailboxHandle aMailbox, const void* aMessage);

/**
 * FeeRTOS_MailboxPeek
 * Liest die naechste Nachricht aus der Mailbox ohne sie zu entfernen.
 *
 * aMailbox  - Handle der Mailbox
 * aMessage  - Zeiger auf den Speicher fuer die Nachricht
 */
void FeeRTOS_MailboxPeek(TFeeRTOS_MailboxHandle aMailbox, void* aMessage);

#endif