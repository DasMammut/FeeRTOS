#ifndef FEERTOS_STACK_H
#define FEERTOS_STACK_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct SFeeRTOS_Stack* TFeeRTOS_StackHandle;

/*
 * Stack_Create
 * Erstellt einen neuen Stack mit der angegebenen Groesse.
 * Allokiert Speicher mit malloc. SP zeigt initial auf das
 * oberste Byte des allokierten Bereichs (Base + Size - 1).
 * Bei malloc-Fehler: Base=NULL, SP=NULL, Size=0.
 *
 * aSize - Groesse des Stacks in Bytes
 *
 * Gibt ein TFeeRTOS_StackHandle zurueck oder NULL bei Fehler.
 */
TFeeRTOS_StackHandle Stack_Create(uint16_t aSize);

/*
 * Stack_Push
 * Schiebt Daten auf den Stack.
 * SP wird um aDataSize nach unten verschoben.
 * Prueft ob genug Platz vorhanden ist.
 *
 * aStack    - Zeiger auf den Stack
 * aData     - Zeiger auf die zu pushenden Daten
 * aDataSize - Groesse der Daten in Bytes
 */
void Stack_Push(TFeeRTOS_StackHandle aStack, void* aData, uint16_t aDataSize);

/*
 * Stack_Pop
 * Holt Daten vom Stack.
 * SP wird um aDataSize nach oben verschoben.
 * Prueft ob genug Daten vorhanden sind.
 *
 * aStack    - Zeiger auf den Stack
 * aData     - Zeiger auf den Zielpuffer fuer die Daten
 * aDataSize - Groesse der Daten in Bytes
 */
void Stack_Pop(TFeeRTOS_StackHandle aStack, void* aData, uint16_t aDataSize);

/*
 * Stack_Destroy
 * Gibt den allokierten Stack-Speicher frei und setzt
 * Base, SP auf NULL und Size auf 0.
 *
 * aStack - Zeiger auf den Stack der zerstoert werden soll
 */
void Stack_Destroy(TFeeRTOS_StackHandle aStack);

#endif