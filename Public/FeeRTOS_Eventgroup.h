#ifndef FEERTOS_EVENTGROUP_H
#define FEERTOS_EVENTGROUP_H

#include <stdint.h>
#include <stdbool.h>

typedef struct SFeeRTOS_Eventgroup* TFeeRTOS_EventgroupHandle;

/**
 * FeeRTOS_CreateEventgroup
 * Erstellt eine neue Eventgroup.
 * Gibt ein Handle zurueck oder NULL bei Fehler.
 */
TFeeRTOS_EventgroupHandle FeeRTOS_CreateEventgroup(void);

/**
 * FeeRTOS_DeleteEventgroup
 * Loescht eine Eventgroup anhand ihres Handles.
 * Gibt alle wartenden Tasks frei.
 *
 * eventgroup - Handle der zu loeschenden Eventgroup
 */
void FeeRTOS_DeleteEventgroup(TFeeRTOS_EventgroupHandle eventgroup);

/**
 * FeeRTOS_EventgroupWait
 * Blockiert den Task bis die angegebenen Bits gesetzt sind.
 * Optional kann auf alle Bits oder eines gewartet werden.
 *
 * eventgroup - Handle der Eventgroup
 * aBits      - Bits auf die gewartet wird
 * aWaitForAll - true: warte auf alle Bits, false: warte auf eines
 */
void FeeRTOS_EventgroupWait(TFeeRTOS_EventgroupHandle eventgroup, uint8_t aBits, bool aWaitForAll);

/**
 * FeeRTOS_EventgroupSetBit
 * Setzt oder loescht ein Bit in der Eventgroup.
 *
 * eventgroup - Handle der Eventgroup
 * aBitIndex  - Index des zu setzenden Bits
 * aSet       - true: Bit setzen, false: Bit loeschen
 */
void FeeRTOS_EventgroupSetBit(TFeeRTOS_EventgroupHandle eventgroup, uint8_t aBitIndex, bool aSet);



#endif