#include "IFeeRTOS_Eventgroup.h"

TFeeRTOS_EventgroupHandle FeeRTOS_CreateEventgroup() {
    TFeeRTOS_EventgroupHandle eventgroup = (TFeeRTOS_EventgroupHandle)FeeRTOS_Malloc(sizeof(TFeeRTOS_Eventgroup));
    if(eventgroup == NULL) return NULL;

    eventgroup->Bits = 0;
    eventgroup->WaitingListHead = NULL;
    return eventgroup;
}

void FeeRTOS_DeleteEventgroup(TFeeRTOS_EventgroupHandle eventgroup) {
    if(eventgroup == NULL) return;

    TFeeRTOS_WaitingTask* current = eventgroup->WaitingListHead;
    while(current != NULL) {
        TFeeRTOS_WaitingTask* next = current->Next;
        FeeRTOS_Free(current);
        current = next;
    }

    FeeRTOS_Free(eventgroup);
}

void FeeRTOS_EventgroupWait(TFeeRTOS_EventgroupHandle eventgroup, uint8_t aBits, bool aWaitForAll) {
    if(eventgroup == NULL) return;

    TFeeRTOS_TaskHandle currentTask = getCurrentTask();
    TFeeRTOS_WaitingTask* waitingTask = (TFeeRTOS_WaitingTask*)FeeRTOS_Malloc(sizeof(TFeeRTOS_WaitingTask));
    if(waitingTask == NULL) return;

    waitingTask->TaskHandle = currentTask;
    waitingTask->Bits = aBits;
    waitingTask->WaitForAll = aWaitForAll;
    waitingTask->Next = eventgroup->WaitingListHead;
    eventgroup->WaitingListHead = waitingTask;

    FeeRTOS_SuspendTask(currentTask);
}

void FeeRTOS_EventgroupSetBit(TFeeRTOS_EventgroupHandle eventgroup, uint8_t aBitIndex, bool aSet) {
    if(eventgroup == NULL || aBitIndex >= 8) return;

    if(aSet) {
        eventgroup->Bits |= (1 << aBitIndex);
    } else {
        eventgroup->Bits &= ~(1 << aBitIndex);
    }

    TFeeRTOS_WaitingTask* current = eventgroup->WaitingListHead;
    while(current != NULL) {
        bool conditionMet = current->WaitForAll ? ((eventgroup->Bits & current->Bits) == current->Bits) : ((eventgroup->Bits & current->Bits) != 0);
        if(conditionMet) {
            TFeeRTOS_TaskHandle taskToWake = current->TaskHandle;
            TFeeRTOS_WaitingTask* taskToRemove = current;
            current = current->Next;

            // Remove the task from the waiting list
            if(taskToRemove == eventgroup->WaitingListHead) {
                eventgroup->WaitingListHead = taskToRemove->Next;
            } 
            else {
                TFeeRTOS_WaitingTask* prev = eventgroup->WaitingListHead;
                while(prev != NULL && prev->Next != taskToRemove) {
                    prev = prev->Next;
                }
                if(prev != NULL) {
                    prev->Next = taskToRemove->Next;
                }
            }

            FeeRTOS_Free(taskToRemove);

            FeeRTOS_ResumeTask(taskToWake);
        } 
        else {
            current = current->Next;
        }
    }
}