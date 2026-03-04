#ifndef FEERTOS_STACK_H
#define FEERTOS_STACK_H

#include <stdlib.h>
#include <string.h>

typedef struct{
    void* StackPointer;
    void* Base;
    unsigned int Size;
} TStack;

/*
 * Stack_Create
 * Erstellt einen neuen Stack mit der angegebenen Groesse.
 * Allokiert Speicher mit malloc. SP zeigt initial auf das
 * oberste Byte des allokierten Bereichs (Base + Size - 1).
 * Bei malloc-Fehler: Base=NULL, SP=NULL, Size=0.
 *
 * aSize - Groesse des Stacks in Bytes
 *
 * Gibt eine TStack-Struktur zurueck. Base pruefen ob NULL!
 */
TStack Stack_Create(unsigned int aSize);

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
void Stack_Push(TStack* aStack, void* aData, unsigned int aDataSize);

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
void Stack_Pop(TStack* aStack, void* aData, unsigned int aDataSize);

/*
 * Stack_Destroy
 * Gibt den allokierten Stack-Speicher frei und setzt
 * Base, SP auf NULL und Size auf 0.
 *
 * aStack - Zeiger auf den Stack der zerstoert werden soll
 */
void Stack_Destroy(TStack* aStack);

#endif