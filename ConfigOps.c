#include "DataTypes.h"
#include "ConfigOps.h"
#include "StringUtils.h"
#include "StandardConstants.h"

/*
clearConfigData
frees dynamically allocated config data structure
		if it has not already been freed
Output Parameters: none
Output returned: NULL (ConfigDataType *)
Dependencies: tbd
*/
ConfigDataType *clearConfigData(ConfigDataType *configData)
{
	//check that config data ptr is not null
	if( configData != NULL)
	{
		//free data struct mem
		free( configData );

		//set config ptr to null
		configData = NULL;
	}
	//set conf data ptr to NULL/return
	return NULL;
}

/*
configCodeToString
utility function converts config code numbers to the string they represent
Output Parameters: resulting output string (char*)
Output Returned: none
Dependenceis: copyString
*/
void configCodeToString( int code, char *outString)
{
	//define array with eight items, add short (10) lengths
	char displayStrings[8][10] = { 	"SJF-N", "SRTF-P", "FCFS-P", "RR-P",
									"FCFS-N", "Monitor", "File", "Both"};


	//copy string to return parameter
		//function: copy string
	copyString(outString, displayStrings[ code ] );

}

/*
displayConfigData
screen dump/display all config data
output parameters: none
output retruned: none
dependencies: tbd
*/
void displayConfigData( ConfigDataType *ConfigData)
{
	//initalize function/vars
	char displayString[ STD_STR_LEN ];
	//print lines for diplay of all member values
		//function: printf, codeToString
	printf( "\nConfig File Display\n" );
	printf( "----------------------\n" );
	printf( "Version 				: %3.2f\n", ConfigData->version );
	printf( "Program file name 			: %s\n", ConfigData->metaDataFileName );
	configCodeToString( ConfigData->cpuSchedCode, displayString );
	printf( "CPU schedule selection			: %s\n", displayString );
	printf( "Quantum time 				: %d\n", ConfigData->quantumCycles );
	printf( "Memory Display 				: " );
	if( ConfigData->memDisplay )
		{
			printf( "On\n" );
		}
	else
		{
			printf( "Off\n" );
		}
	printf( "Memory Available 			: %d\n", ConfigData->memAvailable );
	printf( "Process cycle rate 			: %d\n", ConfigData->procCycleRate );
	printf( "I/O cycle rate 				: %d\n", ConfigData->ioCycleRate );
	configCodeToString( ConfigData->logToCode, displayString );
	printf( "Log to selection 			: %s\n", displayString );
	printf( "Log file name 				: %s\n", ConfigData->logToFileName );
}

/*
getConfigData
driver function for capturing config data from a config file
output parameters: pointer to config data ptr (configDataType **),
							end/result state message ptr (char *)
output returned: boolean result of data access operation
device input/file: config data uploaded
dependencies: copyString, fopen, getStringToDelimiter, compareString
			fclose, malloc, free, stripTrailingSpaces, getDataLineCode,
			fscanf, setStrToLowerCase, valueInRange, getCpuSchedCode,
			getLogToCode
*/
bool getConfigData( const char *fileName, 
						ConfigDataType **configData, char *endStateMsg )
{
	//initialize function/vars

	//const num of data lines
	const int NUM_DATA_LINES = 10;

	///read only constatn
	const char READ_ONLY_FLAG[] = "r";

	//declare other vars
	ConfigDataType *tempData;

	FILE *fileAccessPtr;
	char dataBuffer[MAX_STR_LEN], lowerCaseDataBuffer[ MAX_STR_LEN ];
	int intData, dataLineCode, lineCtr = 0;
	double doubleData;

	//set endStateMsg to success
		//function: copyString
	char testMsg[] = "configuration file upload Successful";
	copyString( endStateMsg, testMsg);

	//initalize config data ptr in case of return error
	*configData = NULL;

	//open file
		//function: fopen
	fileAccessPtr = fopen( fileName, READ_ONLY_FLAG );

	//check for file open failure
	if( fileAccessPtr == NULL )
	{
		//set end state message to config file access error
			//function: copyString
		char testMsg[] = "configuration file access error";
		copyString( endStateMsg, testMsg );

		//return file access error
		return false;

	}
	//get first line, check for failure
		//function getStringToDelimiter, compare string
	if( !getStringToDelimiter( fileAccessPtr, COLON, dataBuffer ) ||
		compareStrings(dataBuffer, "Start Simulator Configuration File" ) != STR_EQ )
	{
		//close file access
			//function: fclose
		fclose( fileAccessPtr );

		//set end state msg to corrupt leader line err
		copyString ( endStateMsg, "Corrupt configuration leader line error");

		//return corrupt file data
		return false;
	}
	//create temp ptr to config data structure
		//function: malloc
	tempData = (ConfigDataType *) malloc( sizeof( ConfigDataType ) );

	//loop to end of config data items
	while (lineCtr < NUM_DATA_LINES )
	{
		//get line leader, check for failure
			//function: getStringToDelimiter
		if( !getStringToDelimiter( fileAccessPtr, COLON, dataBuffer ) )
		{

			//free temp struct memory
				//function: free
			free(tempData);

			//close file access
				//function: fclose
			fclose( fileAccessPtr );
			//set end state message to line capture failure
				//function: copyString
			copyString( endStateMsg, "Configuration Data Leader line capture error");
			//return incomplete file error
			return false;
		}

		//strip trailing spaces of config line leader
		//for use in other functions
			//function: stripTrailingSpaces
		stripTrailingSpaces(dataBuffer);

		//find correct data line code number from string
			//function: getDataLinecCode
		dataLineCode = getDataLineCode( dataBuffer);
		//check for data line found
		if( dataLineCode != CFG_CORRUPT_PROMPT_ERR )
		{
			//get data value
			
				//check for version numbe r(double value)
				if( dataLineCode == CFG_VERSION_CODE )
				{
					//get version number
						//function: fscanf
					fscanf( fileAccessPtr, "%lf", &doubleData);
				}
				//otherwise check for metaData, file names
				//cpu scheduling names, or memory display settings (strings)
				else if( dataLineCode == CFG_MD_FILE_NAME_CODE ||
						dataLineCode == CFG_LOG_FILE_NAME_CODE ||
							dataLineCode == CFG_CPU_SCHED_CODE ||
							dataLineCode == CFG_LOG_TO_CODE ||
							dataLineCode == CFG_MEM_DISPLAY_CODE )
				{
					//sget string input
						//function: fscanf
					fscanf( fileAccessPtr, "%s", dataBuffer );		

					//ste string to lower case for testing in valueIsRange
						//function: setStrToLowerCase
					setStrToLowerCase( lowerCaseDataBuffer, dataBuffer);
				}
				//otherwise, assume int data
				else
				{
					//get int input
						//function: fscanf
					fscanf(fileAccessPtr, "%d", &intData );
				}
			//check for data value in range
				//function: valueInRange
			if( valueInRange( dataLineCode, intData, 
											doubleData, lowerCaseDataBuffer ) )
			{
				//assign to data ptr depending on config item
				//all config line possiblities
					//function: copyString, getCPUSchedCod, 
					//			compareString, getLogToCode as needed
				switch( dataLineCode )
				{
					case CFG_VERSION_CODE:
						tempData->version = doubleData;
						break;

					case CFG_MD_FILE_NAME_CODE:
						copyString( tempData->metaDataFileName, dataBuffer);
						break;

					case CFG_CPU_SCHED_CODE:
						tempData->cpuSchedCode = 
									getCpuSchedCode( lowerCaseDataBuffer );
						break;

					case CFG_QUANT_CYCLES_CODE:
						tempData->quantumCycles = intData;
						break;

					case CFG_MEM_DISPLAY_CODE:
						tempData->memDisplay =
							compareStrings( lowerCaseDataBuffer, "on") == 0;
						break;
					
					case CFG_MEM_AVAILABLE_CODE:
						tempData->memAvailable = intData;
						break;

					case CFG_PROC_CYCLES_CODE:
						tempData->procCycleRate = intData;
						break;

					case CFG_IO_CYCLES_CODE:
						tempData->ioCycleRate = intData;
						break;

					case CFG_LOG_TO_CODE:
						tempData->logToCode =
								getLogToCode( lowerCaseDataBuffer );
						break;
					
					case CFG_LOG_FILE_NAME_CODE:
						copyString( tempData->logToFileName, dataBuffer);
						break;
					
				}
			}
			//otherwise assume not in range
			else
			{
				//free temp struct
					//function: free
				free( tempData );

				//close file access
					//function: fclose
				fclose( fileAccessPtr );
				//set end state message to config data out of range
					//function: copyString
				copyString( endStateMsg, "configuration item out of range" );

				//return data out of range
				return false;
			}
		}
		//assume data line not found
		else
		{
			//free temp struct
				//function: free
			free( tempData );
			//close file access
				//function: fclose
			fclose( fileAccessPtr );

			//set end state message to config corrupt prompt error
				//function: copyString
			copyString( endStateMsg, "corrupted configuration prompt" );

			//return corrupt file code
			return false;
		}
		//increment line counter
		lineCtr++;
	}
	//end master loop


	//debugging
	bool getStringToDelim = !getStringToDelimiter( fileAccessPtr, PERIOD, dataBuffer );
	
	//printf("getStringToDelim is: %d\n", getStringToDelim );
	//printf("compareStr is:")
	//aquire end of sim config string
		//function: getStringToDelimiter, compareString
	if( getStringToDelim ||
			compareStrings( dataBuffer, 
								"End Simulator Configuration File") != STR_EQ )
	{

		//free temp struct memeory
			//function: free
		free(tempData);

		//close file access
		fclose( fileAccessPtr );

		//set end state message to corrupt config end line
			//function: copyString
		copyString( endStateMsg, "configuration end line capture error");

		//return corrupt file data
		return false;
	}
	//test for "file only" output so mem diagnostics do not display
	tempData->memDisplay = tempData->memDisplay &&
							tempData->logToCode != LOGTO_FILE_CODE;
	
	//assign temp ptr to parameter return pointer
	*configData = tempData;

	free( tempData );
	//close file access
	fclose( fileAccessPtr );

	//return no error code
	return true;
}

/*
getCpuSchedCode
converts cpu schedule string to code (all sched. possiblities
*/
ConfigDataCodes getCpuSchedCode( const char *lowerCaseCodeStr )
{
	//set default return
	ConfigDataCodes  returnval = CPU_SCHED_FCFS_N_CODE;

	//check for sjf-n
	if( compareStrings( lowerCaseCodeStr, "sjf-n" ) == STR_EQ)
	{
		returnval = CPU_SCHED_SJF_N_CODE;
	}
	//check for srtf
	if( compareStrings( lowerCaseCodeStr, "srtf-p" ) == STR_EQ)
	{
		returnval = CPU_SCHED_SRTF_P_CODE;
	}

	//check for fcfs-p
	if( compareStrings( lowerCaseCodeStr, "fcfs-p" ) == STR_EQ)
	{
		returnval = CPU_SCHED_FCFS_P_CODE;
	}

	//check for rr-p
	if( compareStrings( lowerCaseCodeStr, "rr-p" ) == STR_EQ)
	{
		returnval = CPU_SCHED_RR_P_CODE;
	}

	//return selected value
	return returnval;
}

/*
getadataLineCode
converts leader line string to configuration code value
			(all config file leader lines)
Output?parameters: none
Output Returned: configuration code value (configCodeMessages)
dependencies: compareString
*/
ConfigCodeMessages getDataLineCode( const char *dataBuffer )
{
	//return appropriate code depending on prompt stirn provided
		//compareString
	if( compareStrings( dataBuffer, "Version/Phase" ) == STR_EQ )
	{
		return CFG_VERSION_CODE;
	}

	if( compareStrings( dataBuffer, "File Path" ) == STR_EQ )
	{
		return CFG_MD_FILE_NAME_CODE;
	}

	if( compareStrings( dataBuffer, "CPU Scheduling Code" ) == STR_EQ )
	{
		return CFG_CPU_SCHED_CODE;
	}

	if( compareStrings( dataBuffer, "Quantum Time (cycles)" ) == STR_EQ )
	{
		return CFG_QUANT_CYCLES_CODE;
	}

	if( compareStrings( dataBuffer, "Memory Display (On/Off)" ) == STR_EQ )
	{
		return CFG_MEM_DISPLAY_CODE;
	}

	if( compareStrings( dataBuffer, "Memory Available (KB)" ) == STR_EQ )
	{
		return CFG_MEM_AVAILABLE_CODE;
	}

	if( compareStrings( dataBuffer, "Processor Cycle Time (msec)" ) == STR_EQ )
	{
		return CFG_PROC_CYCLES_CODE;
	}

	if( compareStrings( dataBuffer, "I/O Cycle Time (msec)" ) == STR_EQ )
	{
		return CFG_IO_CYCLES_CODE;
	}

	if( compareStrings( dataBuffer, "Log To" ) == STR_EQ )
	{
		return CFG_LOG_TO_CODE;
	}

	if( compareStrings( dataBuffer, "Log File Path" ) == STR_EQ )
	{
		return CFG_LOG_FILE_NAME_CODE;
	}

	//return corrupt leader line error code
	return CFG_CORRUPT_PROMPT_ERR;
}

/*
getLogToCode
converts "log to" text to configuration data code
		(3 log to strings)
Output Returned: config data code
output Parameters: none
dependencies: compareStrings
*/
ConfigDataCodes getLogToCode( const char *lowerCaseLogToStr )
{
	//initalize function vars

	//set default to log to moniotr
	ConfigDataCodes returnval = LOGTO_MONITOR_CODE;

	//check for both
		//function: compareStrings
	if( compareStrings( lowerCaseLogToStr, "both") == STR_EQ )
	{
		//set return value to both code
		returnval = LOGTO_BOTH_CODE;
	}

	//check for FILE
		//functoin: compareStrings
	if( compareStrings( lowerCaseLogToStr, "file") == STR_EQ )
	{
		//set return value to both code
		returnval = LOGTO_FILE_CODE;
	}

	//return selected code
	return returnval;
}

/*
stripTrailingSpaces
removes trailing spaces from input config leader lines
output Parameters: updated config leader line string (char*)
output returned: none
Dependencies: getStringLength
*/
void stripTrailingSpaces( char *str )
{
	//initalize index to leng -1 to get highest array index
	int index = getStringLength( str) -1;

	//loop while space is found at end of string
	while( str[index] == SPACE )
	{
		//set element to NULL_CHAR
		str[ index ] = NULL_CHAR;

		//decrement index
		index--;
	}
	//end loop from end of str

}

/*
valueInRange
checks for config data values in range, including all string values
				(all config data values)
Output Parameters: none
Output Returned: boolean result of range test (bool)
Dependencies: compareString
*/
bool valueInRange( int lineCode, int intVal, 
							double doubleVal, const char *lowerCaseStringVal )
{
	//initalize function/vars

	//set result to true all test are to find false
	bool result = true;

	//use line code to identify prompt line
	switch( lineCode )
	{
		//for version code
		case CFG_VERSION_CODE:
			//check if limits of version code are exceeded
			if( doubleVal < 0.00 || doubleVal > 10.00)
			{

				//set bool to false
				result = false;
			}
				break;
				
		
		//for cpu scheduling code
		case CFG_CPU_SCHED_CODE:
			//check for not finding one of the scheduling strings
				//function: compareString
			if( compareStrings( lowerCaseStringVal, "fcfs-n" ) != STR_EQ
				&& compareStrings( lowerCaseStringVal, "sjf-n" ) != STR_EQ
				&& compareStrings( lowerCaseStringVal, "srtf-p" ) != STR_EQ
				&& compareStrings( lowerCaseStringVal, "fcfs-p" ) != STR_EQ
				&& compareStrings( lowerCaseStringVal, "rr-p" ) != STR_EQ )
				{
					// set Boolean result to false
					result = false;
				}
			//break
			break;

		//for quantum cycles
			//check for quantum cycles limits exceeded
		case CFG_QUANT_CYCLES_CODE:

			if(intVal < 0 || intVal > 100)
			{
				//set bool
				result = false;
			}
			//break
			break;

		//for mem display
		case CFG_MEM_DISPLAY_CODE:
			//check for finding either on or off
			if( compareStrings( lowerCaseStringVal, "on" ) != STR_EQ &&
				compareStrings( lowerCaseStringVal, "off" ) != STR_EQ )
			{
				//set bool
				result = false;
			}
			//break
			break;

		//for memory avaliable
		case CFG_MEM_AVAILABLE_CODE:

			//check for mem avaliable
			if( intVal < 1024 || intVal > 102400 )
			{
				//set bool
				result = false;
			}
			//break
			break;
		//for process cycles
		case CFG_PROC_CYCLES_CODE:

			//check fo rprocess cycles limits exceeded
			if( intVal < 1 || intVal > 100 )
			{
				//set bool
				result = false;
			}
			//break
			break;

		//check for I/O cycles
		case CFG_IO_CYCLES_CODE:

			//check for I/O cycles limits exceeded
			if( intVal < 1 || intVal > 1000)
			{
				//set bool value
				result = false;
			}
			//break
			break;
		//for log to operation
		case CFG_LOG_TO_CODE:

			if( compareStrings( lowerCaseStringVal, "both") != STR_EQ &&
				compareStrings( lowerCaseStringVal, "monitor" ) != STR_EQ &&
				compareStrings( lowerCaseStringVal, "file" ) != STR_EQ )
			{
			//check for not finding one of the log to strings
				//function? compareString
				result = false;
				//set  bool 
			}

			//break
			break;
	}
	//return resut of limits analysis
	return result;
}
