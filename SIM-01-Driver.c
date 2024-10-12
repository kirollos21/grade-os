//header fiels
#include <stdio.h>
#include "SIM-01-Driver.h"


int main( int argc, char **argv)
{
	//initalize program
	ConfigDataType *configDataPtr = NULL;
	OpCodeType *metaDataPtr = NULL;
	char errorMessage[ MAX_STR_LEN ];
	CmdLineData cmdLineData;
	bool configUploadSuccess = false;

	//show title
		//function: printf
	printf("\nSimulator Program\n");
	printf("=================\n\n");

	//process cmd line/check for prog run
	//at least one correct switch/config name verified
		//function: processCmdLine
	if( processCmdLine( argc, argv, &cmdLineData ) )
	{
	
		//upload config file, check for success
			//function: getConfigData
		if( getConfigData( cmdLineData.fileName, 
											&configDataPtr, errorMessage ) )
		{	
			//check config display flag
			if( cmdLineData.configDisplayFlag )
			{

				//display config data
					//function: displayConfigData
				displayConfigData( configDataPtr );
			}
			//set config upload success flag
			configUploadSuccess = true;
		}
		//assume config file upload failed
		else
		{
			//show error message, end prog (falls through to clear config data)
				//function: printf
			printf("\nConfig upload Error: %s, program aborted\n\n", 
															errorMessage );
		}
		//check for config success and need for metadata
		if( configUploadSuccess && 
					(cmdLineData.mdDisplayFlag || cmdLineData.runSimFlag ) )
		{
			//upload metadata flag, check for sucecss
				//function: getMetaData
			if( getMetaData( configDataPtr->metaDataFileName, 
												&metaDataPtr, errorMessage ) )
			{
					//check meta data display flag
				if( cmdLineData.mdDisplayFlag )
				{
					//display meta data
						//function: displayMetaData.
					displayMetaData( metaDataPtr );
				}

				//check run sim flag
				if( cmdLineData.runSimFlag )
				{
					//run simulator
						//function runSim (not part of SIM-01)
					runSim( configDataPtr, metaDataPtr );
				
				}
			}
			//assume meta data upload failed
			else
			{
				//show error message, end prog
					//function: printf
				printf("\nMetadata Upload Error: %s, program aborted\n", 
																errorMessage );
			}
		}
		//end check need for metadata upload

		//clean up config data as needed
			//function: clearConfigData
		configDataPtr = clearConfigData( configDataPtr );

		//clean up metadata as needed
			//function: clearMetaDataList
		metaDataPtr = clearMetaDataList( metaDataPtr );

	//end check for good cmd line
	}

	//assume cmd line failure
	else
	{

		//show cmd argument requirements
			//function: showCommandLineFormat
		showCommandLineFormat();
	}
	//show prog end
		//function: printf
	printf("\nSimulator Program End.\n\n");

	//return success
	return 0;
}

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
void clearCmdLineStruct( CmdLineData *clDataPtr )
{
	//set all struct members to default
	clDataPtr->programRunFlag = false;
	clDataPtr->configDisplayFlag = false;
	clDataPtr->mdDisplayFlag = false;
	clDataPtr->runSimFlag = false;
	clDataPtr->fileName[0] = NULL_CHAR;
}

/*
processCmdLine
checks for at least 2 arguments, then sets booleans 
			(in cmdLineDataStruct inside of SIM-01-Driver.h) 
			depending on cmd line switches which can be in any order, 
			also captures config file name which must be the last argument
output parameters: pointer to cmd line structure (CmdLineData *)
											with updated members
											set to default values if failure
											to capture arguments
output returned: boolean result of argument capture, 
						true if at least one switch and config file name, 
						false otherwise

*/
bool processCmdLine(int numArgs, char **strVector, CmdLineData *clDataPtr)
{
	//initalize function/vars
	//initalize struct to defaults
		//function clearStruct;
	clearCmdLineStruct( clDataPtr );
		//initalize success flags to false
	bool atLeastOneSwitchFlag = false;
	bool correctConfigFileFlag = false;

	//initalize first arg index to one
	int argIndex = 1;

	//declare other vars
	int fileStrLen, fileStrSubLoc;

	//must have prog name, at least one switch, and config file name
	if( numArgs >= MIN_NUM_ARGS ) //3 function name, 1 switch, function 
	{
		//loop across args starting at one and program run flag
		while ( argIndex < numArgs )
		{
			//check for -dc (display config flag)
			if( compareStrings( strVector[argIndex], "-dc" ) == STR_EQ)
			{
				//set config display flag	
				clDataPtr->configDisplayFlag = true;

				//set at least one switch flag
				atLeastOneSwitchFlag = true;
			}
			//check for -dm (display metadata flag)
			else if( compareStrings( strVector[argIndex], "-dm" ) == STR_EQ )
			{
				//set metadata display flag
				clDataPtr->mdDisplayFlag = true;
				
				//set at least one switch flag
				atLeastOneSwitchFlag = true;
			}
			//otherwise check for -rs (run simulator_
			else if( compareStrings( strVector[argIndex], "-rs" ) == STR_EQ )
			{
				//set run sim flag
				clDataPtr->runSimFlag = true;

				//set at least one switch flag
				atLeastOneSwitchFlag = true;
			}
			//otherwise, check for file name, ending in .cnf
				//must be last four chars
			else
			{
				//find lengts to verify file name
				fileStrLen = getStringLength( strVector[numArgs -1] );

				//file name must be last argument and have .cnf
				fileStrSubLoc = findSubString(strVector[ numArgs -1 ], ".cnf" );

				//verify string found and correct format for config file name
				if( fileStrSubLoc != SUBSTRING_NOT_FOUND
						&& fileStrSubLoc == fileStrLen - LAST_FOUR_LETTERS)
				{
					//set file name to variable
					copyString( clDataPtr ->fileName, strVector[numArgs -1] );

					//set success flag to true
					correctConfigFileFlag = true;
				}
				//otherwise assume bad config file name
				else
				{
					//reset struct, correct config file flag stays false
						//function: clearCmdLineStruct
					clearCmdLineStruct( clDataPtr );
				
				}
			}
			//update arg index
			argIndex = argIndex + 1;
		}
		//end arg loop
	}
	//end test for min number of cmd line arguments
	
	//return verification of at least one switch and correct file name
	return atLeastOneSwitchFlag && correctConfigFileFlag;
}


/*
showCommandLineFormat
displays command line format as assistance to user
Output Parameters: none
Output Returned: none
device Output/monitor: data displayed as specified
dependencies: printf
*/
void showCommandLineFormat()
{
	//DISPLAY COMMAND LINE FORMAT
		//function: printf
	printf( "command Line Format:\n");
	printf( "		sim_01 [-dc] [-dm] [-rs] <config file name>\n");
	printf( "		-dc [optional] displays configuration data\n");
	printf( "		-dm [optional] displays metadata\n");
	printf( "		-rs [optional] runs simulator\n");
	printf( "		required config file name\n");
	//no return
}