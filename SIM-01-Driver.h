#ifndef SIM_01_DRIVER_H
#define OS_SIM_DRIVER_H

//header files
#include <stdio.h>

#include "StandardConstants.h"
#include "DataTypes.h"
#include "ConfigOps.h"
#include "MetaDataOps.h"
#include "Simulator.h"
#include "StringUtils.h"


//program constants
typedef enum { MIN_NUM_ARGS = 3, LAST_FOUR_LETTERS = 4} PRGM_CONSTANTS;

//command line struct for sharing command line switch settings
typedef struct CmdLineDataStruct
{
	bool programRunFlag;
	bool configDisplayFlag;
	bool mdDisplayFlag;
	bool runSimFlag;

	char fileName[ STD_STR_LEN];
}CmdLineData;

//function prototypes

/*
clearCmdLineStruct
sets command line structure data to defaults
	booleans to false, fileName to empty string
Input Parameters: pointer to cmd line struct (CmdLineData *)
Output Parameters: pointer to command line struct (CmdLineData*)
							with updated members
Output Retrurned: none
Dependencies: none
*/
void clearCmdLineStruct( CmdLineData *clDataPtr );

/*
processCmdLine
checks for at least 2 arguments, then sets booleans dpeending on 
	cmd line switches which can be in any order, also captures
	config file name which must be the last argument
output parameters: pointer to cmd line structure (CmdLineData *)
											with updated members
											set to default values if failure
											to capture arguments
output returned: boolean result of argument capture, 
						true if at least one switch and config file name, 
						false otherwise

*/
bool processCmdLine(int numArgs, char **strVector, CmdLineData *clDataPtr);

/*
showCommandLineFormat
displays command line format as assistance to user
Output Parameters: none
Output Returned: none
device Output/monitor: data displayed as specified
dependencies: printf
*/
void showCommandLineFormat();

#endif