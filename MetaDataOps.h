
#ifndef METADATAOPS_H
#define METADATAOPS_H

#include <stdbool.h>
#include <stdio.h>
#include "DataTypes.h"
#include "StandardConstants.h"
#include "StringUtils.h"

typedef enum { BAD_ARG_VAL = -1,
               NO_ACCESS_ERR,
               MD_FILE_ACCESS_ERR,
               MD_CORRUPT_DESCRIPTOR_ERR,
               OPCMD_ACCESS_ERR,
               CORRUPT_OPCMD_ERR,
               CORRUPT_OPCMD_ARG_ERR,
               UNBALANCED_START_END_ERR,
               COMPLETE_OPCMD_FOUND_MSG,
               LAST_OPCMD_FOUND_MSG } OpCodeMessages;

/*
addNode
adds metadat node to linked list recursively, handles empty condition
output param: none
output returned: pointer to prev node or head node (OpCodeType* )
dependencies: malloc, copyString
*/
OpCodeType *addNode( OpCodeType *localPtr, OpCodeType *newNode );

/*
clearMetaDataList
recursively traverses list, frees dynamically allocated nodes
function output parameters: none
Output returned: NULL (OpCodeType *)
Dependencies: clearMetaDataList, free
*/
OpCodeType *clearMetaDataList( OpCodeType *localPtr );

/*
displayMetaData
data dump/display of all op code itmes
output parameters: none
output returned: none
Dependencies: tbd
*/
void displayMetaData( const OpCodeType *localPtr );

/*
getCommand
parses 3 letter command part of op code string
output param: parsed command (char*)
output returned: updated start index for use by calling function (int)
dependencies: none
*/
int getCommand(char *cmd, const char *inputStr, int index );

/*
getNumberArgs
starts at a given index, captures and assembles int arg, and returns as param
*/
int getNumberArgs ( int *number, const char *intputStr, int index );

/*
getOpCommand
acquires one op command line from prev. opened file, parses it, and sets
		various struct members according to the 3 letter cmd

output param: pointer to one op code struct OpCodeType*
output Returned: coded result of operation (OpCodeMessages)
Dependenceis: getStringToDelim, getCommand, copyString, verifyValidCommand,
				compareString, getStringArrg, verifyFirstStringArg, getNumArg
*/
OpCodeMessages getOpCommand( FILE *filePtr, OpCodeType *inData );

/*
getStringArg
starts at given index, captures and assembles string argument, returns as param
*/
int getStringArg( char *strArg, const char *inputStr, int index );

/*
getMetaData
main driver function to upload, parse and store a 
			list of op code commands in a linked list
Output Parameters: pointer to op code linked list head ptr (OpCodeType **)
					result  mesasge of function state after completion (char*)
Output Returned: boolean result of operation (bool)
Device Input/file: op code list uploaded
Dependencies: copyString, fopen, getStringToDelimiter, compareString, fclose
				malloc, getOpCommand, updateStartCount, updateEndCount,
				clearMetaDataList, free, addNode
*/
bool getMetaData( const char *fileName, 
							OpCodeType **opCodeDataHead, char *endStateMsg );

/*
isDigit
tests char param for digit, returns true if it is a digit
*/
bool isDigit( char testChar );

/*
manages count of "end" arg to be compared at end of process input
*/
int updateEndCount( int count, const char *opString);

/*
updateStartCount
manages count of "start" arguments to be compared at end of process input
*/
int updateStartCount(int count, const char *opString);

/*
verifyFirstStringArg
checks for all possiblities of first argument string of op command
*/
bool verifyFirstStringArg( const char *strArg);

/*
verifyValidCommand
checks for all possiblities of 3 letter op code cmd
*/
bool verifyValidCommand( char *testCmd);



#endif //METADATAOPS_H