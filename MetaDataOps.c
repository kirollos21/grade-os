#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "StringUtils.h"
#include "MetaDataOps.h"
#include "DataTypes.h"


/*
addNode
adds metadata node to linked list recursively, handles empty condition
output param: none
output returned: pointer to prev node or head node (OpCodeType* )
dependencies: malloc, copyString
*/
OpCodeType *addNode( OpCodeType *localPtr, OpCodeType *newNode )
{
	//check for local ptr assinged to null
	if(localPtr == NULL )
	{
		//access memory for new link/node
		localPtr = (OpCodeType *)malloc(sizeof(OpCodeType) );

		//assign all 3 values to newly created node
		//assign next ptr to null
		localPtr->pid = newNode->pid;
		copyString(localPtr->command, newNode->command);
		copyString(localPtr->inOutArg, newNode->inOutArg);
		copyString(localPtr->strArg1, newNode->strArg1);
		localPtr->intArg2 = newNode->intArg2;
		localPtr->intArg3 = newNode->intArg3;
		localPtr->opEndTime = newNode->opEndTime;
		localPtr->nextNode = NULL;
		//return current local ptr
		return localPtr;
	}
	//assume end of list not found yet
	//assign recursive function to current's next link
		//addNode
	localPtr->nextNode = addNode(localPtr->nextNode, newNode);
	return localPtr;
}

/*
clearMetaDataList
recursively traverses list, frees dynamically allocated nodes
function output parameters: none
Output returned: NULL (OpCodeType *)
Dependencies: clearMetaDataList, free
*/
OpCodeType *clearMetaDataList( OpCodeType *localPtr )
{
	//initalize function/vars

	//check for local ptr not set to null
	if( localPtr != NULL)
	{
		//call recursive function to next pointer
		clearMetaDataList( localPtr->nextNode );

		//release memory
		free(localPtr);

		//set given ptr to null
		localPtr = NULL;
	}
	//return null
	return NULL;
}

/*
displayMetaData
data dump/display of all op code itmes
output parameters: none
output returned: none
Dependencies: tbd
*/
void displayMetaData( const OpCodeType *localPtr )
{
	//initalize function/vars

	//display title with underline
	printf("Meta-Data File Display\n" );
	printf( "----------------------\n\n");
	//loop to end of linked list
	while (localPtr != NULL)
	{
		//print leader
		printf("OpCode: ");

		//print op code pid
		printf("/pid: %d", localPtr->pid );

		//print op code command
		printf("/cmd: %s", localPtr->command);

		//check for dev op
		if( compareStrings( localPtr->command, "dev" ) == STR_EQ )
		{
			//print in/out paremeter
			printf("/io: %s", localPtr->inOutArg);
		}
		//otherwise assume other than dev
		else
		{
			//print NA
			printf("/io: NA");
		}
		//print first string arg
		printf("\n\t /arg1: %s", localPtr->strArg1 );

		//print first int arg
		printf("/arg 2: %d", localPtr->intArg2);

		//print second int arg if exists
		printf("/arg3: %d", localPtr->intArg3);

		//print op end time (open)
		printf( "/op end time %8.6f", localPtr->opEndTime );

		//end line
		printf("\n\n");

		//assign local ptr to next node
		localPtr = localPtr->nextNode;
	}
}

/*
getCommand
parses 3 letter command part of op code string
output param: parsed command (char*)
output returned: updated start index for use by calling function (int)
dependencies: none
*/
int getCommand(char *cmd, const char *inputStr, int index )
{
	//initalize function/vars
	int lengthOfCommand = 3;

	//loop across cmd length
	while( index < lengthOfCommand )
	{
		//assign char from input string to buffer str
		cmd[index] = inputStr[ index ];

		//inc index
		index++;

		//set next char to NUll char
		cmd[index] = NULL_CHAR;
	}
	//return current index
	return index;
}

/*
getMetaData
main driver function to upload, parse, and store list of 
		op code commands in a linked list

output parameters: pointer to op code linked list head ptr (OpCodeType **),
							result message of function state
							after completion (char *)
output returned: boolean result of operation (bool)
device input/file: op code list uploaded
dependencies: copyString, fopen, getStringToDelimiter, compareString, fclose,
				malloc, getOpCommand, updateStartcount, updateEndCount,
				clearMetaDataList, free, addNode
*/
bool getMetaData( const char *fileName, OpCodeType **opCodeDataHead,
										char *endStateMsg)
{
	//read only constants
	const char READ_ONLY_FLAG[] = "r";

	//initalize vars
	int accessResult, startCount = 0, endCount = 0;
	char dataBuffer [ MAX_STR_LEN ];
	bool returnState = true;
	OpCodeType *newNodePtr;
	OpCodeType *localHeadPtr = NULL;
	FILE *fileAccessPtr;

	//initalize op code data ptr inc ase of return error
	*opCodeDataHead = NULL;

	//initalize end state message
	copyString( endStateMsg, "Metadata File upload successful" );

	//open file
	fileAccessPtr = fopen( fileName, READ_ONLY_FLAG );

	//check for file open failure
	if( fileAccessPtr == NULL )
	{
		//set end state message
		copyString( endStateMsg, "Metadata file access error" );

		//return file access error
		return false;
	}
	//check first line for correct leader
		//function: getStringToDelimiter, compareString
	if( !getStringToDelimiter( fileAccessPtr, COLON, dataBuffer ) ||
		compareStrings( dataBuffer, "Start Program Meta-Data Code") != STR_EQ ) 
	{
		//close file
		fclose( fileAccessPtr );

		//set end state message
		copyString(endStateMsg, "Corrupt metadata leader line error" );

		//return corrupt descriptor error
		return false;
	}
	//allocate memory for temp data struct
		//malloc
	newNodePtr = (OpCodeType *) malloc( sizeof( OpCodeType ) );

	//get fist op command
		//function getOpcommnand
	accessResult = getOpCommand(fileAccessPtr, newNodePtr);

	//get start and end counts for later comparison
		//function? updateStartCount, UpdateEndCount
	startCount = updateStartCount( startCount, newNodePtr->strArg1 );
	endCount = updateEndCount( endCount, newNodePtr->strArg1 );

	//debugging
	//printf("accessResult is:%d", accessResult );

	//check for failure of first complet op code
	if( accessResult != COMPLETE_OPCMD_FOUND_MSG )
	{
		//close file
		fclose( fileAccessPtr );

		//clear data from struct list
			//clearMetadataList
		*opCodeDataHead = clearMetaDataList( localHeadPtr );

		//free temp struct memeory
		free( newNodePtr );

		//set end state message
		copyString( endStateMsg, "Metadata incomplete first op command found");

		return false;
	}
	//loop across all remaining op commands
		//while complete cop commands are found
	while( accessResult == COMPLETE_OPCMD_FOUND_MSG )
	{

		//add the new op command to linked list
			//addNode
		localHeadPtr = addNode( localHeadPtr, newNodePtr );

		//get a new op command
			//getOpCommand
		accessResult = getOpCommand( fileAccessPtr, newNodePtr );

		//update start and end counts for later comparison
			//updateStartCount, updateEndCount
		startCount = updateStartCount( startCount, newNodePtr->strArg1 );
		endCount = updateEndCount( endCount, newNodePtr->strArg1 );
	}
	//after loop, check for last op command
	if( accessResult == LAST_OPCMD_FOUND_MSG )
	{
		//check for start and end op code counts equal
		if( startCount == endCount )
		{
			//add the last node to the linked list
				//addNode
			localHeadPtr = addNode(localHeadPtr, newNodePtr );

			//set access result to no error for later op
			accessResult = NO_ACCESS_ERR;

			//check last line for incorect descriptor
				//getStringToDelimiter, compareString
			if( !getStringToDelimiter( fileAccessPtr, PERIOD, dataBuffer) ||
				compareStrings(dataBuffer, "End Program Meta-Data Code" ) != STR_EQ)
			{

				//set access result to corrupted descriptor error
				accessResult = MD_CORRUPT_DESCRIPTOR_ERR;

				//set end state message
				copyString( endStateMsg, "Metadata corrupted descriptor error");
			}
		}
	
		//assume start/end counts not equal
		else
		{
			//close file
			fclose( fileAccessPtr );

			//clear data
			*opCodeDataHead = clearMetaDataList( localHeadPtr );

			//free memeory
			free( newNodePtr );

			//set access result to error
			accessResult = UNBALANCED_START_END_ERR;

			//return result
			return false;
		}
	}
	//otherwise, assume didn't find end
	else
	{
		//set end state msg
		copyString(endStateMsg, "Corrupted metadata op code");

		//unset return state
		returnState = false;
	}
	//check for any errors found (not no error)
	if( accessResult != NO_ACCESS_ERR)
	{
		//clear op cmd list
		localHeadPtr = clearMetaDataList( localHeadPtr );
	}
	//close access file	
	fclose(fileAccessPtr );

	//free memory
	free( newNodePtr );
	//assign temp local head ptr to parameter return ptr
	*opCodeDataHead = localHeadPtr;
	
	free( localHeadPtr );

	//return access result
	return returnState;

}

/*
getOpCommand
acquires one op command line from prev. opened file, parses it, and sets
		various struct members according to the 3 letter cmd

output param: pointer to one op code struct OpCodeType*
output Returned: coded result of operation (OpCodeMessages)
Dependenceis: getStringToDelim, getCommand, copyString, verifyValidCommand,
				compareString, getStringArrg, verifyFirstStringArg, getNumArg
*/
OpCodeMessages getOpCommand( FILE *filePtr, OpCodeType *inData )
{
	//initalize func/vars
	const int MAX_CMD_LENGTH = 5;
	const int MAX_ARG_STR_LENGTH = 15;

	int numBuffer = 0;
	char strBuffer[ STD_STR_LEN ];
	char cmdBuffer [MAX_CMD_LENGTH ];
	char argStrBuffer[ MAX_ARG_STR_LENGTH ];
	int runningStringIndex = 0;
	bool arg2FailureFlag = false;
	bool arg3FailureFlag = false;

	//get whole op command as string, check for successful access
	if( getStringToDelimiter( filePtr, SEMICOLON, strBuffer ) )
	{
		//get 3 letter cmd
		runningStringIndex = getCommand( cmdBuffer, 
											strBuffer, runningStringIndex );
		
		//assign op command to node
		copyString( inData->command, cmdBuffer );
	}
	//otherwise, assume unsuccessful accesss
	else
	{
		//set ptr to data struct to null
		inData = NULL;

		//return op command access failure
		return OPCMD_ACCESS_ERR;
	}
	//verify op command
	if( !verifyValidCommand( cmdBuffer ) )
	{
		//return op cmd errro
		return CORRUPT_OPCMD_ERR;
	}

	//set all struct values that may not be initailized to defaults
	inData->pid = 0;
	inData->inOutArg[ 0 ] = NULL_CHAR;
	inData->intArg2 = 0;
	inData->intArg3 = 0;
	inData->opEndTime = 0.0;
	inData->nextNode = NULL;

	//printf("cmdBuffer is: %s\n", cmdBuffer );

	//check for device cmd
	if( compareStrings( cmdBuffer, "dev") == STR_EQ )
	{
		//get in/out arg
		runningStringIndex = getStringArg( argStrBuffer, 
											strBuffer, runningStringIndex );
		
		//set device in/out arg
		copyString( inData->inOutArg, argStrBuffer );

		//check correct arg
		if( compareStrings( argStrBuffer, "in" ) != STR_EQ &&
				compareStrings( argStrBuffer, "out" ) !=STR_EQ )
		{
			//printf("Error up here bozo\n");
			//return arg err
			return CORRUPT_OPCMD_ARG_ERR;
		}
	}

	//get first string arg
	runningStringIndex = getStringArg( argStrBuffer, 
								strBuffer, runningStringIndex );

	//set device in/out arg
	copyString( inData->strArg1, argStrBuffer );

	bool verifyFirstStr = !verifyFirstStringArg( argStrBuffer );
	//check for legit first string arg
	if( verifyFirstStr )
	{
		//return arg error
		return CORRUPT_OPCMD_ARG_ERR;
	}

	//check for last op cmd found
	if( compareStrings( inData->command, "sys") == STR_EQ &&
			compareStrings(inData->strArg1, "end" ) == STR_EQ )
	{
		//return last op cmd found msg
		return LAST_OPCMD_FOUND_MSG;
	}

	//check for app start seconds arg
	if( compareStrings( inData->command, "app" ) == STR_EQ &&
		compareStrings( inData->strArg1, "start" ) == STR_EQ )
	{
		//get num arg
		runningStringIndex = getNumberArgs( &numBuffer, 
										strBuffer, runningStringIndex );

		//check for failed num access
		if( numBuffer <= BAD_ARG_VAL )
		{
			//set failure flag
			arg2FailureFlag = true;
		}
		//set first int arg to num
		inData->intArg2 = numBuffer;
	}
	//check for cpu cycle time
	else if( compareStrings( inData->command, "cpu" ) == STR_EQ )
	{
		//get num arg
		runningStringIndex = getNumberArgs( &numBuffer, 
												strBuffer, runningStringIndex);
		//check for failed num access
		if( numBuffer <= BAD_ARG_VAL )
		{
			//set failure flag
			arg2FailureFlag = true;
		}
		//set first int arg  to num
		inData->intArg2 = numBuffer;
	}
	//check for device cycle time
	else if( compareStrings( inData->command, "dev" ) == STR_EQ)
	{
		//get num arg
		runningStringIndex = getNumberArgs( &numBuffer, strBuffer,
														runningStringIndex);
		//check for failed num access
		if( numBuffer <= BAD_ARG_VAL )
		{
			//set fail flag
			arg2FailureFlag = true;
		}
		//set first int arg to number
		inData->intArg2 = numBuffer;
	}
	//check for mem base and offset
	else if( compareStrings( inData->command, "mem" ) == STR_EQ )
	{
		//get num arg for base
		runningStringIndex = getNumberArgs( &numBuffer, 
										strBuffer, runningStringIndex );

		//check for failed num access
		if( numBuffer <= BAD_ARG_VAL )
		{
			//set fail flag
			arg2FailureFlag = true;
		}

		//set first int arg to num
		inData->intArg2 = numBuffer;

		//get num arg for offset
		runningStringIndex = getNumberArgs( &numBuffer,
								strBuffer, runningStringIndex );

		//check for failed num access
		if( numBuffer <= BAD_ARG_VAL )
		{
			//set failure flag
			arg3FailureFlag = true;
		}

		//set second int arg to number
		inData->intArg3 = numBuffer;
	}
	//check int args for upload failure
	if( arg2FailureFlag || arg3FailureFlag )
	{
		//printf("arg2F is: %d and arg3F is: %d", arg2FailureFlag, arg3FailureFlag );
		//return corrupt op command error error
		return CORRUPT_OPCMD_ARG_ERR;
	}
	//return complete op command found message
	return COMPLETE_OPCMD_FOUND_MSG;
}

/*
getNumberArgs
starts at a given index, captures and assembles int arg, and returns as param
*/
int getNumberArgs ( int *number, const char *inputStr, int index )
{
	bool foundDigit = false;
	*number = 0;
	int multiplier = 1;

	//loop to skip white space
	while( inputStr[index] <= SPACE || inputStr[index] == COMMA )
	{
		index++;
	}
		//increment index
	//loop across str len
		//set digit found flag
	while( isDigit( inputStr[index] ) == true  &&
						inputStr[index] != NULL_CHAR ) 
	{
		//assign digit to output
		foundDigit = true;

		//assign digit to output
		(*number) = (*number) * multiplier + inputStr[index] - '0';

		//increment index and multiplyer
		index = index + 1;
		multiplier = 10;
	}
	//check for digit not found
	if( !foundDigit )
	{
		//set num to BAD_ARG_VAL, use deref for ptr
		*number = BAD_ARG_VAL;
	}

	//return current index
	return index;
}

/*
getStringArg
starts at given index, captures and assembles string argument, returns as param
*/
int getStringArg( char *strArg, const char *inputStr, int index )
{
	int localIndex = 0;
	//loop to skip white space
	while( inputStr[ index ] <= SPACE || inputStr[ index ] == COMMA )
	{
		//inc index
		index++;
	}

	//loop across str len
	while( inputStr[ index ] != COMMA && inputStr[ index ] != NULL_CHAR )
	{
		//assign char from input string to buffer string
		strArg[ localIndex ] = inputStr[ index ];

		//increment indicies
		index++;
		localIndex++;

		//set next char to null char
		strArg[ localIndex ] = NULL_CHAR;
	}
	//return current index
	return index;
}


/*
isDigit
tests char param for digit, returns true if it is a digit
*/
bool isDigit( char testChar )
{
	//check for test char between chars '0'-'9'
	return (testChar >= '0' && testChar <= '9');
}

/*
manages count of "end" arg to be compared at end of process input
*/
int updateEndCount( int count, const char *opString)
{
	//check for "end in op string
	if( compareStrings( opString, "end" ) == STR_EQ )
	{
		//return incremented end count
		return count + 1;
	}
	//return unchanged end count
	return count;
}

/*
updateStartCount
manages count of "start" arguments to be compared at start of process input
*/
int updateStartCount(int count, const char *opString)
{
	//check for "start in op string
	if( compareStrings( opString, "start" ) == STR_EQ )
	{
		//return incremented start count
		return count + 1;
	}
	//return unchanged start count
	return count;
}

/*
verifyFirstStringArg
checks for all possiblities of first argument string of op command
*/
bool verifyFirstStringArg( const char *strArg)
{

	//check for string holding correct first arg
	return(compareStrings( strArg, "access" ) == STR_EQ ||
	compareStrings( strArg, "allocate" ) == STR_EQ ||
	compareStrings( strArg, "end" ) == STR_EQ ||
	compareStrings( strArg, "ethernet" ) == STR_EQ ||
	compareStrings( strArg, "hard drive" ) == STR_EQ ||
	compareStrings( strArg, "keyboard" ) == STR_EQ ||
	compareStrings( strArg, "monitor" ) == STR_EQ ||
	compareStrings( strArg, "printer" ) == STR_EQ ||
	compareStrings( strArg, "process" ) == STR_EQ ||
	compareStrings( strArg, "serial" ) == STR_EQ ||
	compareStrings( strArg, "sound signal" ) == STR_EQ ||
	compareStrings( strArg, "start" ) == STR_EQ ||
	compareStrings( strArg, "usb" ) == STR_EQ ||
	compareStrings( strArg, "video signal" ) == STR_EQ  );
}

/*
verifyValidCommand
checks for all possiblities of 3 letter op code cmd
*/
bool verifyValidCommand( char *testCmd)
{
	//check for string holding 3 letter op code command
	return( compareStrings( testCmd, "sys" ) == STR_EQ || 
	compareStrings( testCmd, "app" ) == STR_EQ || 
	compareStrings( testCmd, "cpu" ) == STR_EQ || 
	compareStrings( testCmd, "mem" ) == STR_EQ || 
	compareStrings( testCmd, "dev" ) == STR_EQ );

}
