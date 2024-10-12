
#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "DataTypes.h"
#include "StandardConstants.h"
#include "ConfigOps.h"
#include "MetaDataOps.h"
#include "SimTimer.h"


//for coopy paste////////////
/*
Name: 
Process: 
Function Input/Parameters: 
Function Output/Parameters: none
Function Output/Returned: none
Device Input/Device: none
Device Output/Device: none
Dependencies: ???
*/
///////////////////////////

//declare structs
//define process states
typedef enum{ 
                PROCESS_STATE_NEW = 0,
                PROCESS_STATE_READY,
                PROCESS_STATE_RUNNING,
                PROCESS_STATE_BLOCKED,
                PROCESS_STATE_TERMINATED
            } PROCESS_STATES;

//holds all necessary information for PCB
typedef struct PCBStruct
{
    //unique process id
    int pid;

    //enum to determine what state the process is in
    PROCESS_STATES process_state;

    //head pointer for op commands
    OpCodeType *nextOpCode;

    //count io cycles
    int ioCycles;

    //count cpu cycles
    int cpuCycles;

    //count total cycles?
    int totalCycles;

    //time remaining from given time for process execution
        //time given - time taken, may be 0 but will not be negative
    double timeLeft;

    //next PCBObj pointer (next process to be run)
    struct PCBStruct *nextPtr;
} PCBObj;

//holds head ptr for PCB linked list
typedef struct PCBLinkedListStruct
{
    PCBObj *headPtr;
} PCBLinkedList;

/*
Name: addProcess
Process: allocates memory for a new pcb, gets data from the new pcb and dumps
         data to link the new pcb
Function Input/Parameters: PCBObj
Function Output/Parameters: none
Function Output/Returned: new PCBObj with new data
Device Input/Device: none
Device Output/Device: none
Dependencies: malloc, sizeof
*/
PCBObj *addProcess(const PCBObj sourcePCB);

/*
Name: createProcessList
Process: initalizes PCB head pointer 
Function Input/Parameters: none
Function Output/Parameters: none
Function Output/Returned: PCB head ptr
Device Input/Device: none
Device Output/Device: none
Dependencies: malloc, sizeof
*/
PCBLinkedList *createProcessList();

/*
Name: destroyProcessList
Process: deallocates memory for PCB linked list
Function Input/Parameters: head ptr to linked list struct
Function Output/Parameters: none
Function Output/Returned: NULL
Device Input/Device: none
Device Output/Device: none
Dependencies: free, destroyProcessListHelper
*/
PCBLinkedList *destroyProcessList(PCBLinkedList *linkedListPtr );



/*
Name: destroyProcessListHelper
Process: recursively deallocates the PCB linked list struct memory
Function Input/Parameters: head pointer to linked list 
Function Output/Parameters: none
Function Output/Returned: NULL
Device Input/Device: none
Device Output/Device: none
Dependencies: free, destroyProcessListHelper(recursively)
*/
PCBObj *destroyProcessListHelper(PCBObj *nodePtr);

/*
Name: fillProcessList
Process: iterates through the metadata LL, fills out each process as needed.
Function Input/Parameters: metadata (OpCodeType *),
                           headPointer (ProcessControlBlockLL *)
Function Output/Parameters: none
Function Output/Returned: pointer to filled PCB (ProcessControlBlockLL *)
Device Input/Device: none
Device Output/Device: none
Dependencies: ???
*/
PCBLinkedList *fillProcessList(ConfigDataType *configPtr, OpCodeType *metadataPtr, PCBLinkedList *headPointer);

/*
Name: logOutput
Process: Takes a string and outputs it to monitor AND/OR file
Function Input/Parameters: string (char *), file pointer (FILE *), 
                           configuration data (ConfigDataType *),
                           end of simulator (bool)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/Device: none
Device Output/Device: none
Dependencies: sprintf
*/
void logOutput(ConfigDataType *configPtr, char *logStr, bool isEnd);

/*
Name: runCPU
Process: runs the CPU operations for however long it takes
Function Input/Parameters: command, argument(s) configData, PID
Function Output/Parameters: none
Function Output/Returned: none
Device Input/Device: none
Device Output/Device: none
Dependencies: ???
*/
void runCPU(char *command, char *argument, int intArg, ConfigDataType *configPtr, int PID);

/*
Name: runIO
Process: runs the IO operations for however long it takes
Function Input/Parameters: command, argument(s), configData, PID
Function Output/Parameters: none
Function Output/Returned: none
Device Input/Device: none
Device Output/Device: none
Dependencies: ???
*/
void runIO(char *command, char *argument, int intArg, ConfigDataType *configPtr, int processId);

/*
Name: runProcess
Process: runs the process that was selected
Function Input/Parameters: ???
Function Output/Parameters: none
Function Output/Returned: none
Device Input/Device: none
Device Output/Device: none
Dependencies: accessTimer, compareString, runCPU, runIO, ???
*/
void runProcess(PCBObj *singlePCB, ConfigDataType *configPtr, OpCodeType *metaDataPtr );

/*
Name: scheduleProcess
Process: takes scheduling data, returns which process to run
Function Input/Parameters: head pointer of PCB List 
Function Output/Parameters: none
Function Output/Returned: none
Device Input/Device: none
Device Output/Device: none
Dependencies: ???
*/
int scheduleProcess(PCBLinkedList *headPointer, ConfigDataType *configPtr);

/*
Name: simThread
Process: wrapper functions for threading in the simulator
Function Input/Parameters: milliseconds (int)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/Device: none
Device Output/Device: none
Dependencies: ???
*/
void *simThread(void *milSec);

/*
Name: runSim
Process: primary sim driver
Function Input/Parameters: ConfigData, opCodes
Function Output/Parameters: none
Function Output/Returned: none
Device Input/Device: none
Device Output/Device: none
Dependencies: ???
*/
void runSim( ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr);


#endif //SIMULATOR_H