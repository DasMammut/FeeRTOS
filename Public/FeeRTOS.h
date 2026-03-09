/*
 * FeeRTOS.h
 *
 * Ein einfacher RTOS-Kernel für AVR-Mikrocontroller (atmega4809).
 * Bietet Task-Management, Scheduling und Timing-Funktionen.
 *
 * Autor: Andreas Fanninger
 * Datum: 28.03.2026
 */

#ifndef FEERTOS_H
#define FEERTOS_H

#include <stdint.h>

typedef enum{
    TASK_PRIORITY_IDLE = 0,
    TASK_PRIORITY_LOW,
    TASK_PRIORITY_MEDIUM,
    TASK_PRIORITY_HIGH,
    TASK_PRIORITY_CALLBACK
} TTaskPriority;

typedef struct SFeeRTOS_Task* TFeeRTOS_TaskHandle;

#define FeeRTOS_ENTER_CRITICAL() uint8_t sreg_save = SREG; cli()
#define FeeRTOS_EXIT_CRITICAL() SREG = sreg_save

/*
 * FeeRTOS_Init
 * Initialisiert den RTOS-Kernel. Erstellt automatisch einen Idle-Task
 * der ausgefuehrt wird wenn kein anderer Task lauffaehig ist.
 * Wird automatisch von FeeRTOS_StartScheduler aufgerufen falls
 * noch keine Tasks vorhanden sind.
 */
void FeeRTOS_Init(void);

/*
 * FeeRTOS_CreateTask
 * Erstellt einen neuen Task und fuegt ihn in die Task-Liste ein.
 * Allokiert einen eigenen Stack und baut den initialen Kontext
 * (PC, SREG, R0-R31) auf dem Stack auf, sodass der Task beim
 * ersten Context-Switch korrekt gestartet wird.
 * Bei malloc-Fehler wird NULL zurueckgegeben.
 * UserData wird in R24:R25 abgelegt (AVR Calling Convention).
 *
 * aTaskFunction - Einsprungpunkt des Tasks
 * aUserData     - Parameter fuer den Task
 * StackSize     - Stack-Groesse in Bytes
 * Priority      - Prioritaet des Tasks
 * Rueckgabewert: Handle auf den erstellten Task oder NULL bei Fehler
 */
TFeeRTOS_TaskHandle FeeRTOS_CreateTask(void (*aTaskFunction)(void* aUserData), void* aUserData, uint16_t StackSize, TTaskPriority Priority);

/*
 * FeeRTOS_DeleteTask
 * Loescht einen Task anhand seines Handles.
 * Gibt den Stack-Speicher frei und verschiebt nachfolgende Tasks.
 * Wenn der aktuell laufende Task sich selbst loescht, wird sofort
 * ein Context-Switch ausgeloest.
 *
 * aTaskHandle - Handle des zu loeschenden Tasks
 */
void FeeRTOS_DeleteTask(TFeeRTOS_TaskHandle aTaskHandle);

/*
 * FeeRTOS_StartScheduler
 * Startet den Scheduler. Initialisiert Timer A0 als Tick-Quelle,
 * laedt den Kontext von Task 0 und springt per RETI in den
 * ersten Task. Diese Funktion kehrt nie zurueck.
 */
void FeeRTOS_StartScheduler(void);

/*
 * FeeRTOS_Yield
 * Gibt die CPU freiwillig ab. Startet den Timer neu und loest
 * sofort einen Timer-Overflow-Interrupt aus, was einen
 * Context-Switch zum naechsten Task bewirkt.
 */
void FeeRTOS_Yield(void);

/*
 * FeeRTOS_Delay
 * Blockiert den aktuellen Task fuer die angegebene Zeit in Millisekunden.
 * Der Task wird in dieser Zeit nicht ausgefuehrt, andere Tasks koennen laufen.
 *
 * aDelayMs - Anzahl der Millisekunden, die der Task blockiert werden soll
 */
void FeeRTOS_Delay(uint16_t aDelayMs);

void FeeRTOS_SuspendTask(TFeeRTOS_TaskHandle aTaskHandle);

void FeeRTOS_ResumeTask(TFeeRTOS_TaskHandle aTaskHandle);

// ON the way !!!!

// Yield umbauen das es keine Verzehrung macht // nicht möglich

// Eventgroups testen und Mutexe mit Prioritätsvererbung testen
// Heap2 mehr rechen Zeit weniger Header Speicher

// Structs memory optimierung
// ISR und Hardware code trennen von Kernel code
// Kommentare

#endif