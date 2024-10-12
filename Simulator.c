#include "Simulator.h"
#include "DataTypes.h"
#include "StringUtils.h"
#include <stdio.h>
#include <pthread.h>


/*
runSim
primary simulation driver
output parameters: none
output returned: none
Dependencies: tbd
*/
//biggest project, not part of SIM-01 "does it all"
void runSim( ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr)
{
	printf("\nSimulator Run\n");
	printf("---------------\n\n");

	//create pcb object, and queue
	//OpCodeType *wkgMasterPtr;
	PCBLinkedList *pcbList;
	PCBObj *pcbPtr;
	int currentPid;

	//strings for output and time management
	char simulatorOutput[ MAX_STR_LEN ];
	char timeStr[ MAX_STR_LEN ];

	//start sim/clock
	accessTimer(ZERO_TIMER, timeStr);

	//log timer start
	sprintf( simulatorOutput, "%f, OS: Simulator start\n", accessTimer(LAP_TIMER, timeStr ));
	logOutput( configPtr, simulatorOutput, false );

	//create Process list
	pcbList = createProcessList();

	//fill in processes
	pcbList = fillProcessList( configPtr, metaDataMstrPtr, pcbList );

	//set local PCB ptr to first process in list
	pcbPtr = pcbList->headPtr;

	//calculate time to load all PCBs
	int *totalPCBTime = malloc( sizeof( int ));
	*totalPCBTime = 0;

	/*while( pcbPtr != NULL )
	{
		*totalPCBTime = *totalPCBTime + pcbPtr->timeLeft;
		pcbPtr = pcbPtr->nextPtr;
	}
	pcbPtr = pcbList->headPtr;
	*/
	

	//implement PThreads
	pthread_t *cpuThread = (pthread_t*)malloc( sizeof( pthread_t ));
	pthread_create( cpuThread, NULL, simThread, totalPCBTime );

	double *processTimeLeft = malloc(sizeof(double));
	*processTimeLeft = pcbPtr->timeLeft;

	// set the current process id to the first scheduled process FCFS for now
    currentPid = scheduleProcess(pcbList, configPtr);

	//loop through process queue (while head != null)
	while( pcbPtr != NULL )
	{
		if( pcbPtr->pid == currentPid )
		{
			//set process state to ready
			pcbPtr->process_state = PROCESS_STATE_READY;
			
			//log state
			sprintf(simulatorOutput, "%f, OS: Process %d set to RUNNING from READY\n\n", 
											accessTimer(LAP_TIMER, timeStr), pcbPtr->pid);
			//call logOutput
			logOutput( configPtr, simulatorOutput, false );

			//create PThread
			pthread_t *runThread = (pthread_t*)malloc(sizeof(pthread_t));
			pthread_create(runThread, NULL, simThread, processTimeLeft);

			//call run process to run the process to completion
			runProcess(pcbPtr, configPtr, metaDataMstrPtr );

			//when finished, set to terminated ( set output str)
			sprintf(simulatorOutput, "%f, OS: Process %d set to TERMINATED state from RUNNING state\n\n", 
						accessTimer(LAP_TIMER, timeStr), pcbPtr->pid);
			
			//call log output
			logOutput(configPtr, simulatorOutput, false);
			pcbPtr->process_state = PROCESS_STATE_TERMINATED;

			//end get value from thread
			pthread_join(*runThread, NULL);

			//free mem
			free( runThread );
		}

		//set time left in process
		*processTimeLeft = pcbPtr->timeLeft;
		printf("time left: %f\n", pcbPtr->timeLeft );

		//increment to next PCB
		pcbPtr = pcbPtr->nextPtr;
		currentPid = scheduleProcess( pcbList, configPtr );

	}
	//end queueloop

	//return thread val
	pthread_join(*cpuThread, NULL);

	//free mem
	free(cpuThread);

	//log output
	sprintf( simulatorOutput, "%f, OS: System Stop\n", accessTimer(LAP_TIMER, timeStr ));
	logOutput( configPtr, simulatorOutput, false );
	sprintf( simulatorOutput, "%f, OS: Simulation End\n", accessTimer(LAP_TIMER, timeStr ));
	logOutput( configPtr, simulatorOutput, true );

	//deallocate the process list with destroyProcessList
	destroyProcessList(pcbList);

	//log the output to FILE if required
	logOutput( configPtr, "", true );

	//free mem
	free( totalPCBTime );
	free( processTimeLeft );

}



/*
Name: addProcess
Process: allocates memory for a new pcb, gets data from the new pcb and sets
				new data to new PCBObj ptr
Function Input/Parameters: PCBObj
Function Output/Parameters: none
Function Output/Returned: new PCBObj with new data
Device Input/Device: none
Device Output/Device: none
Dependencies: malloc, sizeof
*/
PCBObj *addProcess(const PCBObj sourcePCB)
{
	//declare variables
	PCBObj *workingNode;

	//malloc size for new obj
	workingNode = malloc(sizeof(PCBObj));

	//set source data to local ptr
	workingNode->pid = sourcePCB.pid;
	workingNode->cpuCycles = sourcePCB.cpuCycles;
	workingNode->ioCycles = sourcePCB.ioCycles;
	workingNode->nextOpCode = sourcePCB.nextOpCode;
	workingNode->process_state = sourcePCB.process_state;
	workingNode->timeLeft = sourcePCB.timeLeft;
	workingNode->totalCycles = sourcePCB.totalCycles;

	//set next ptr to null
	workingNode->nextPtr = NULL;

	//return new node
	return workingNode;
}

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
PCBLinkedList *createProcessList()
{
	//create new PCB linked list
	PCBLinkedList *PCBLL;
	PCBLL = malloc(sizeof(PCBLinkedList));

	//set head ptr to null
	PCBLL->headPtr = NULL;

	return PCBLL;
	}

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
PCBLinkedList *destroyProcessList(PCBLinkedList *linkedListPtr )
{
	//free the PCBs with helper function
	destroyProcessListHelper( linkedListPtr->headPtr );

	//free the LL struct
	free( linkedListPtr );

	return NULL;
}


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
PCBObj *destroyProcessListHelper(PCBObj *nodePtr)
{
	//base case
	if( nodePtr != NULL)
	{
		nodePtr->nextPtr = destroyProcessListHelper( nodePtr->nextPtr );
	}
	//else at the end of the linked list
	free( nodePtr );

	//return 
	return NULL;
}

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
PCBLinkedList *fillProcessList(ConfigDataType *configPtr, OpCodeType *metadataPtr, PCBLinkedList *headPointer)
{
	//declare variables
	OpCodeType *wkgMetaDataPtr;
	OpCodeType *wkgInstPtr;
	PCBObj currentPCB;
	PCBObj *wkgPCBPtr;

	int ioCycleCount = 0;
	int cpuCycleCount = 0;
	double timeLeft = 0.0;
	int processID = 0;
	char *cmd, *arg;// *inOutArg;


	//set wkg ptr to first metaData pointer
	wkgMetaDataPtr = metadataPtr;
	wkgInstPtr = metadataPtr;

	//set other wkg ptr to first part of PCB
	wkgPCBPtr = headPointer->headPtr;

	//get first cmd and argument
	cmd = wkgMetaDataPtr->command;
	arg = wkgMetaDataPtr->strArg1;
	//inOutArg = wkgMetaDataPtr->inOutArg;

	//loop through process list until sys end
	while( wkgMetaDataPtr != NULL && !( compareStrings( arg, "end") == STR_EQ && 
										compareStrings( cmd, "sys") == STR_EQ ))
	{
		currentPCB.nextOpCode = wkgInstPtr->nextNode;

		//if app start
		if( compareStrings( cmd, "app" ) && compareStrings( arg, "start" ) )
		{
			//set ID for PCB and metadata
			wkgMetaDataPtr->pid = processID;
			currentPCB.pid = processID;

			//set process state
			currentPCB.process_state = PROCESS_STATE_NEW;
		}

		//printf("inside fileProcessList\n");
		//printf("cmd is: %s.  arg is: %s\n\n", cmd, arg);
		//count num of io cycles
		if( compareStrings( cmd, "dev" ) == STR_EQ)
		{
			//set ioCycleCount and time remaining
			ioCycleCount = ioCycleCount + wkgMetaDataPtr->intArg2;

		}

		//count cpu cycles
		if( compareStrings( arg, "process" ) == STR_EQ && 
			compareStrings( cmd, "cpu" ) == STR_EQ)
		{
			cpuCycleCount = cpuCycleCount + wkgMetaDataPtr->intArg2;;
		}
		if( compareStrings( arg, "end") == STR_EQ &&
			compareStrings(cmd, "app") == STR_EQ)
		{
			//set cycle count variables
			currentPCB.ioCycles = ioCycleCount;
			currentPCB.cpuCycles = cpuCycleCount;
			currentPCB.totalCycles = ioCycleCount + cpuCycleCount;

			//set current pcb starting instruction
			wkgInstPtr = wkgMetaDataPtr;

			//set the time remaining
			timeLeft = ( ioCycleCount * configPtr->ioCycleRate ) +
						( cpuCycleCount * configPtr->procCycleRate );

			//increment PID ctr
			processID = processID + 1;

			//set PID variables
			currentPCB.timeLeft = timeLeft;

			//reset variables
			ioCycleCount = 0;
			cpuCycleCount = 0;
			timeLeft = 0;

			//add nodes to the end of all data, for both metadata and pcbs
			if(headPointer->headPtr == NULL)
			{
				//set data
				headPointer->headPtr = addProcess( currentPCB );

				//increment headPtr to next
				wkgPCBPtr = headPointer->headPtr;
			}
			else
			{
				//set data
				wkgPCBPtr->nextPtr = addProcess(currentPCB);

				//increment next ptr
				wkgPCBPtr = wkgPCBPtr->nextPtr;
			}
		}
		//set new data
		wkgMetaDataPtr = wkgMetaDataPtr->nextNode;
		cmd = wkgMetaDataPtr->command;
		arg = wkgMetaDataPtr->strArg1;
		//inOutArg = wkgMetaDataPtr->inOutArg;
	}

	//return new head ptr
	return headPointer;

}

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
void logOutput(ConfigDataType *configPtr, char *logStr, bool isEnd)
{

	//create file ptr
	FILE *outputFilePtr = NULL;

	//log to both
	if( configPtr->logToCode == LOGTO_BOTH_CODE )
	{
		//open the output file with mode "append" (a+)
		outputFilePtr = fopen(configPtr->logToFileName, "a+" );
		if( outputFilePtr != NULL )
		{
			//write to file
			fprintf(outputFilePtr, "%s", logStr );

			//print to screen
			fprintf(stdout, "%s", logStr);
		}
		else
		{
			fprintf(stderr, "Err: could not open file %s for logging.\n", 
													configPtr->logToFileName );
		}
		//if at end
		if( isEnd )
		{
			//close file ptr
			fclose( outputFilePtr );
		}
	}
	//log to file only
	else if( configPtr->logToCode == LOGTO_FILE_CODE )
	{
		//log output to file only
		printf("\nSim will create output file\n");

		//open the output file with mode "append" (a+)
		outputFilePtr = fopen(configPtr->logToFileName, "a+" );
		if( outputFilePtr != NULL )
		{
			//add logStr to the file
			fprintf( outputFilePtr, "%s", logStr );
		}
		else
		{
			fprintf(stderr, "Error: could not open file %s for logging.\n", 
													configPtr->logToFileName );
		}
		//if at end
		if( isEnd )
		{
			fopen(configPtr->logToFileName, "w+");
			//close file ptr
			fclose( outputFilePtr );
		}
	}
	//log to screen only
	else if ( configPtr->logToCode == LOGTO_MONITOR_CODE )
	{
		fprintf(stdout, "%s", logStr );
	}

}

/*
Name: runCPU
Process: runs the CPU operations until complete
Function Input/Parameters: command, argument(s) configData, PID
Function Output/Parameters: none
Function Output/Returned: none
Device Input/Device: none
Device Output/Device: none
Dependencies: ???
*/
void runCPU(char *cmd, char *arg, int intArg2, ConfigDataType *configPtr, int PID)
{
	//create variables
	char timeStr[MAX_STR_LEN];
	char CPUOutput[MAX_STR_LEN];
	int CPUTime = intArg2 * (configPtr->procCycleRate);
	int *CPUTimePtr = malloc(sizeof(int));
	*CPUTimePtr = CPUTime;

	sprintf(CPUOutput, "%f, Process: %d, %s %s operation start\n", 
						accessTimer(LAP_TIMER, timeStr ), PID, cmd, arg );
	logOutput( configPtr, CPUOutput, false );
	
	//printf("CPU Time is: %dms\n", *CPUTimePtr);

	//run timer for intArg * procCycleRate
	//create thread
	pthread_t *cpuThread = (pthread_t *)malloc(sizeof(pthread_t *));
	//run thread
	pthread_create(cpuThread, NULL, simThread, (void *)CPUTimePtr);

	//return value from thread
	pthread_join(*cpuThread, NULL);

	//create output str
	sprintf(CPUOutput, "%f, Process: %d, %s %s operation end\n", 
							accessTimer(LAP_TIMER, timeStr), PID, cmd, arg);
	//log output
	logOutput(configPtr, CPUOutput, false);

	//free pointer mem
	free(CPUTimePtr);

	//free mem
	free( cpuThread );

}

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
void runIO(char *cmd, char *arg, int intArg2, 
								ConfigDataType *configPtr, int PID)
{
	char timeStr[MAX_STR_LEN];
	char ioOutput[MAX_STR_LEN];
	int ioTime = intArg2 * (configPtr->ioCycleRate);
	int *ioTimePtr = malloc( sizeof( int ));
	*ioTimePtr = ioTime;
	
	//printf("ioCycleRate is: %d\n", configPtr->ioCycleRate );
	//printf("intArg2 is: %d\n", intArg2 );
	//printf("I/0 Time is: %dms\n", *ioTimePtr);

	//create output
	sprintf(ioOutput, "%f, Process: %d, %s %s operation start\n", 
			accessTimer(LAP_TIMER, timeStr), PID, cmd, arg);
	//log output
	logOutput(configPtr, ioOutput, false);

	//create/use thread
	pthread_t *ioThread = (pthread_t *)malloc(sizeof(pthread_t *));
	pthread_create(ioThread, NULL, simThread, (void *)ioTimePtr);

	//return thread value
	pthread_join(*ioThread, NULL);

	//create and log output
	sprintf(ioOutput, "%f, Process: %d, %s %s operation end\n", 
				accessTimer(LAP_TIMER, timeStr), PID, cmd, arg);
	logOutput(configPtr, ioOutput, false);

	//free mem
	free( ioThread );
	free( ioTimePtr );
}

/*
Name: runProcess
Process: runs the selected
Function Input/Parameters: ???
Function Output/Parameters: none
Function Output/Returned: none
Device Input/Device: none
Device Output/Device: none
Dependencies: accessTimer, compareString, runCPU, runIO
*/
void runProcess(PCBObj *singlePCB, ConfigDataType *configPtr, OpCodeType *metaDataPtr)
{
	//"execute" the entire process, running the CPU when a CPU cycle comes up
	//or a run IO when an IO cycle comes up
	//char timeStr[ MAX_STR_LEN ];

	char *command;
	char *argument;
	//char *inOutArg;
	command = metaDataPtr->command;
	argument = metaDataPtr->strArg1;
	//char *inOutArg = workingMeta->inOutArg;

	//inOutArg = workingMeta->inOutArg;
	while (!( compareStrings( command, "app" ) == STR_EQ && 
			compareStrings( argument, "end" ) == STR_EQ) && 
			!( compareStrings( command, "sys" ) == STR_EQ && 
			compareStrings( argument, "end" ) == STR_EQ ) )
	{
		//debugging
		//printf("inOutArg is: %s\n", inOutArg );
		//printf("command is: %s\n", command);
		//printf("argument is: %s\n", argument );

		// check to see what operation will be performed
		if (compareStrings( command, "cpu" ) == STR_EQ && 
		compareStrings( argument, "process" ) == STR_EQ)
		{
			//printf("\ncalling runCPU\n");
			//printf("arg2 from OpCodeType ptr is: %d\n", metaDataPtr->intArg2);

			// run the CPU for the required amount of time.
			runCPU( metaDataPtr->command, metaDataPtr->strArg1, 
						metaDataPtr->intArg2, configPtr, singlePCB->pid );
		}
		if( compareStrings( command, "devin" ) == STR_EQ || 
		compareStrings( command, "dev out" )  )
		{
			//printf("\n\ncalling runIO\n");
			//printf("arg2 from OpCodeType ptr is: %d\n", metaDataPtr->intArg2);

			// run IO operations for the required amount of time.
			runIO( metaDataPtr->command, metaDataPtr->strArg1, 
						metaDataPtr->intArg2, configPtr, singlePCB->pid );
		}
		metaDataPtr = metaDataPtr->nextNode;
		command = metaDataPtr->command;
		argument = metaDataPtr->strArg1;
		//inOutArg = workingMeta->inOutArg;
	}
}

/*
Name: scheduleProcess
Process: takes scheduling data, returns which process to run
Function Input/Parameters: head pointer of PCB List 
Function Output/Parameters: none
Function Output/Returned: none
Device Input/Device: none
Device Output/Device: none
Dependencies: accessTimer, sprintf
*/
int scheduleProcess(PCBLinkedList *headPointer, ConfigDataType *configPtr)
{

	PCBObj *wkgPtr;
	int returnPid = -1;
	wkgPtr = headPointer->headPtr;
	char loggingStr[MAX_STR_LEN];
	char timeStr[MAX_STR_LEN];

	// switch case for each scheduling algorithm
	switch (configPtr->cpuSchedCode)
	{
		case CPU_SCHED_FCFS_N_CODE:
			//First Come, First Served, Non-preemptive Scheduling
			//returns first process without fail.
			//need to find whichever process is first, AND the state is NEW.
			//will set the state to READY, return the pid of the process which
				// was set to ready.
			
			// set every process in the list to READY from NEW
			while(wkgPtr != NULL)
			{
				//
				if(wkgPtr->process_state != PROCESS_STATE_READY && 
						wkgPtr->process_state != PROCESS_STATE_TERMINATED)
				{
					// set the process within the PCB to the ready state, 
					//report in ready state.
					wkgPtr->process_state = PROCESS_STATE_READY;

					//debugging
					//printf("Process %d set to READY from NEW\n", wkgPtr->pid);
					sprintf(loggingStr, "%f, OS: Process %d set to READY state from NEW state\n", 
											accessTimer(LAP_TIMER, timeStr), wkgPtr->pid);
					//log output
					logOutput(configPtr, loggingStr, false);
				}
				// move to the next pointer
				wkgPtr = wkgPtr->nextPtr;
			}

			// reset the pointer to get ready for selection of process
			wkgPtr = headPointer->headPtr;
			while(wkgPtr != NULL && (returnPid == -1))
			{
				// check if the process is NOT terminated
				if (wkgPtr->process_state == PROCESS_STATE_READY)
				{
					//printf("Process %d Selected with %.0f ms remaining\n", wkgPtr->pid, wkgPtr->timeLeft);
					sprintf(loggingStr, "%f, OS: Process %d selected with %.0f ms remaining\n", 
									accessTimer(LAP_TIMER, timeStr), wkgPtr->pid, wkgPtr->timeLeft);
					//log output
					logOutput(configPtr, loggingStr, false);
					returnPid = wkgPtr->pid;
				}
				// move to next pointer
				wkgPtr = wkgPtr->nextPtr;
			}
			break;
		case CPU_SCHED_FCFS_P_CODE:
			// TODO: First Come, First Served, Preemptive Scheduling
			printf("FCFS not implemented yet\n");
			break;

		case CPU_SCHED_RR_P_CODE:
			// TODO: Round Robin Scheduling
			printf("Round Robin not implemented yet\n");
			break;

		case CPU_SCHED_SJF_N_CODE:
			// TODO: Shortest Job First Scheduling
			printf("SJF not implemented yet\n");
			break;

		case CPU_SCHED_SRTF_P_CODE:
			// TODO: Shortest Run-time First Scheduling
			printf("SRTF not implemented yet\n");
			break;
	}
	// return returnPid
	return returnPid;
}

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
void *simThread(void *milSec)
{
	//initalize variables
	int cycleTime = *( int *)milSec;

	//"wait" for operation to complete in a thread
	//printf("simulating operation with: %dms\n", cycleTime);
	runTimer(cycleTime);
	pthread_exit(NULL);

}
