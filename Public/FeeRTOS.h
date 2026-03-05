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

typedef struct{
    void (*TaskFunction)(void* aUserData);
    void* UserData;
    uint16_t StackSize;
    TTaskPriority Priority;
    char NameID[16];
} TTaskConfig;

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
 * Bei malloc-Fehler wird kein Task erstellt (stille Rueckkehr).
 * UserData wird in R24:R25 abgelegt (AVR Calling Convention).
 *
 * aTaskConfig - Zeiger auf eine TTaskConfig-Struktur mit:
 *               TaskFunction: Einsprungpunkt des Tasks
 *               UserData:     Parameter fuer den Task
 *               StackSize:    Stack-Groesse in Bytes
 */
void FeeRTOS_CreateTask(TTaskConfig* aTaskConfig);

/*
 * FeeRTOS_DeleteTask
 * Loescht einen Task anhand seiner ID (Index in der Task-Liste).
 * Gibt den Stack-Speicher frei und verschiebt nachfolgende Tasks.
 * Wenn der aktuell laufende Task sich selbst loescht, wird sofort
 * ein Context-Switch ausgeloest.
 *
 * aTaskID - Index des zu loeschenden Tasks (0 = Idle-Task)
 */
void FeeRTOS_DeleteTask(char* aTaskNameID);

/*
 * FeeRTOS_StartScheduler
 * Startet den Scheduler. Initialisiert Timer A0 als Tick-Quelle,
 * laedt den Kontext von Task 0 und springt per RETI in den
 * ersten Task. Diese Funktion kehrt nie zurueck.
 */
void FeeRTOS_StartScheduler(void);

/*
 * FeeRTOS_Malloc
 * Thread-sichere malloc-Implementierung. Sperrt Interrupts und
 * setzt den Stack Pointer auf RAMEND, um die interne malloc-Logik
 * zu schuetzen. Nach der Allokation wird der originale Stack Pointer
 * wiederhergestellt.
 *
 * size - Anzahl der Bytes, die allokiert werden sollen
 * Rueckgabewert: Zeiger auf den allokierten Speicher oder NULL bei Fehler
 */
void* FeeRTOS_Malloc(uint16_t size);

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

void FeeRTOS_SuspendTask(char* aTaskNameID);

void FeeRTOS_ResumeTask(char* aTaskNameID);

// ON the way !!!!
// Yield umbauen das es keine Verzehrung macht // nicht möglich
// Tasks auf Handl umstellen Namen -> Handle (Zeiger auf Task-Struktur)
// FeeRTOS Queues
// FeeRTOS Mailboxes

// 5

#endif