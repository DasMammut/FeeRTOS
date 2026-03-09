#ifndef FEERTOS_RINGBUFFER_H
#define FEERTOS_RINGBUFFER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct SFeeRTOS_Ringbuffer* TFeeRTOS_RingbufferHandle;

// Item size in bytes, buffer size in number of items
/**
 * FeeRTOS_CreateRingbuffer
 * Erstellt einen neuen Ringbuffer mit angegebener Groesse und Item-Groesse.
 * Gibt ein Handle zurueck oder NULL bei Fehler.
 *
 * aBufferSize - Anzahl der Items
 * aItemSize   - Groesse eines Items in Bytes
 */
TFeeRTOS_RingbufferHandle FeeRTOS_CreateRingbuffer(uint16_t aBufferSize, uint8_t aItemSize);

/**
 * FeeRTOS_DestroyRingbuffer
 * Loescht einen Ringbuffer anhand seines Handles.
 * Gibt alle wartenden Tasks frei.
 *
 * aRingbuffer - Handle des zu loeschenden Ringbuffers
 */
void FeeRTOS_DestroyRingbuffer(TFeeRTOS_RingbufferHandle aRingbuffer);

/**
 * FeeRTOS_RingbufferWrite
 * Schreibt Daten in den Ringbuffer.
 * Gibt true bei Erfolg, false bei Fehler.
 *
 * aRingbuffer - Handle des Ringbuffers
 * aData       - Zeiger auf die zu schreibenden Daten
 */
bool FeeRTOS_RingbufferWrite(TFeeRTOS_RingbufferHandle aRingbuffer, const void* aData);

/**
 * FeeRTOS_RingbufferRead
 * Liest Daten aus dem Ringbuffer.
 * Gibt true bei Erfolg, false bei Fehler.
 *
 * aRingbuffer - Handle des Ringbuffers
 * aData       - Zeiger auf den Speicher fuer gelesene Daten
 */
bool FeeRTOS_RingbufferRead(TFeeRTOS_RingbufferHandle aRingbuffer, void *aData);

#endif