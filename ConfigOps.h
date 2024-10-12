
#ifndef CONFIGOPS_H
#define CONFIGOPS_H

#include <stdbool.h>
#include "DataTypes.h"

//global constants
typedef enum { 	CFG_FILE_ACCESS_ERR,
				CFG_CORRUPT_DESCRIPTION_ERR,
				CFG_DATA_OUT_OF_RANGE_ERR,
				CFG_VERSION_CODE,
				CFG_MD_FILE_NAME_CODE,
				CFG_CPU_SCHED_CODE,
				CFG_QUANT_CYCLES_CODE,
				CFG_MEM_DISPLAY_CODE,
				CFG_MEM_AVAILABLE_CODE,
				CFG_PROC_CYCLES_CODE,
				CFG_IO_CYCLES_CODE,
				CFG_LOG_TO_CODE,
				CFG_LOG_FILE_NAME_CODE,
				CFG_CORRUPT_PROMPT_ERR } ConfigCodeMessages;

typedef enum { 	CPU_SCHED_SJF_N_CODE,
				CPU_SCHED_SRTF_P_CODE,
				CPU_SCHED_FCFS_P_CODE,
				CPU_SCHED_RR_P_CODE,
				CPU_SCHED_FCFS_N_CODE,
				LOGTO_MONITOR_CODE,
				LOGTO_FILE_CODE,
				LOGTO_BOTH_CODE,
				NON_PREEMPTIVE_CODE,
				PREEMPTIVE_CODE } ConfigDataCodes;

/*
clearConfigData
frees dynamically allocated config data structure
		if it has not already been freed
Output Parameters: none
Output returned: NULL (ConfigDataType *)
Dependencies: tbd
*/
ConfigDataType *clearConfigData(ConfigDataType *configData);

/*
configCodeToString
utility function converts config code numbers to the string they represent
Output Parameters: resulting output string (char*)
Output Returned: none
Dependenceis: copyString
*/
void configCodeToString( int code, char *outsString);

/*
displayConfigData
screen dump/display all config data
output parameters: none
output retruned: none
dependencies: tbd
*/
void displayConfigData( ConfigDataType *ConfigData);

/*
getConfigData
driver function for capturing config data from a config file
output parameters: pointer to config data ptr (configDataType **),
							end/result state message ptr (char *)
output returned: boolean result of data access operation
device input/file: config data uploaded
dependencies: tbd
*/
bool getConfigData( const char *fileName, 
						ConfigDataType **configData, char *endStateMsg );

/*
getCpuSchedCode
converts cpu schedule string to code (all sched. possiblities
*/
ConfigDataCodes getCpuSchedCode( const char *lowerCaseCodeStr );

/*
getadataLineCode
converts leader line string to configuration code value
			(all config file leader lines)
Output?parameters: none
Output Returned: configuration code value (configCodeMessages)
dependencies: compareString
*/
ConfigCodeMessages getDataLineCode( const char *dataBuffer );

/*
getLogToCode
converts "log to" text to configuration data code
		(3 log to strings)
Output Returned: config data code
output Parameters: none
dependencies: */
ConfigDataCodes getLogToCode( const char *lowerCaseLogToStr );

/*
stripTrailingSpaces
removes trailing spaces from input config leader lines
output Parameters: updated config leader line string (char*)
output returned: none
Dependencies: getStringLength
*/
void stripTrailingSpaces( char *str );

/*
valueInRange
checks for config data values in range, including all string values
				(all config data values)
Output Parameters: none
Output Returned: boolean result of range test (bool)
Dependencies: compareString
*/
bool valueInRange( int lineCode, int intVal, 
							double doubleVal, const char *lowerCaseStringVal );




#endif //CONFIGOPS_H