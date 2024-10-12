//header files
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "StringUtils.h"
#include "StandardConstants.h"

/*
Name: compareStrings
compares text quantities (c-style strings) without case sensitivity
         with the following outcomes:
         - for equal sized strings, if the first parameter is alphabetically
           greater than the second parameter, returns positive number
         - for equal sized strings, if the first parameter is alphabetically
           less than the second parameter, returns negative number
         - for equal sized strings, if the first parameter is alphabetically
           equal to the second parameter, returns zero
         - for unequal sized strings with common letters up to the length
           of the shortest string, returns difference in the length
           of the strings
output parameters: none
output returned: processed result as specified (int)
Dependencies: getStringLength
*/
int compareStrings( const char *oneStr, const char *otherStr )
{
    int diff, index = 0;

    while( oneStr[ index ] != NULL_CHAR && 
						otherStr[ index ] != NULL_CHAR &&
											index < MAX_STR_LEN )
    {
        diff = oneStr[ index ] - otherStr[ index ];

        if( diff != 0 )
        {
			//printf("Diff is %d\n", diff);
            return diff;
        }

        index = index + 1;
    }
	//printf("strings different length, calling getStringLength:\n");
    return getStringLength( oneStr ) - getStringLength( otherStr );
}

/*
concatenateString
appends one string ot another
output parameters: c style string char* 
output returned: none
dependencies getStringLen, malloc, copyString
*/
void concatenateString( char *destStr, const char *sourceStr)
{
	//initalize function/vars
	//set dest index
		//getStringLen
	int destIndex = getStringLength(destStr);

	//get src str len
		//getStringLen
	int sourceStrLen = getStringLength(sourceStr);

	//create temp str ptr
	char *tempStr;

	//create other vars
	int sourceIndex = 0;

	//copy src str in case of aliasing
		//function malloc, copyString
	tempStr = ( char* )malloc( sourceStrLen + 1 );
	copyString( tempStr, sourceStr );

	//loop to end of src str
	while( tempStr[sourceIndex ] != NULL_CHAR && destIndex < MAX_STR_LEN )
	{
		//assign chars to end of dest string
		destStr[destIndex] = tempStr[sourceIndex];

		//update indicies
		destIndex++; sourceIndex++;

		//set temp end of dest str
		destStr[destIndex] = NULL_CHAR;
	}
	//end loop

	//free memory
	free(tempStr);
}

/*
Name: copyString
Process: copies characters from source string to destination string
         up to NULL_CHAR

output parameters: destination string (char *)
output returned: none

Dependencies: getStringLen
*/
void copyString( char *destStr, const char *srcStr )
{
    int index = 0;

	if( destStr != srcStr )
	{
		while( srcStr[ index ] != NULL_CHAR && index < MAX_STR_LEN )
		{
			destStr[ index ] = srcStr[ index ];

			index++;

			destStr[ index ] = NULL_CHAR;
		}
	}
}

/*
findSubString
determines if a larger string contains a substring
output parameters: none
output return: index of found substring, or SUBSTRING_NOT_FOUND constant
dependencies: getStringLength
*/
int findSubString (const char *testStr, const char *searchSubStr)
{
	//test string length
		//getStringLength
	int testStrLen = getStringLength(testStr);

	//create master index, location of sub string start point
	int masterIndex = 0;

	//initialize other vars
	int searchIndex, internalIndex;

	//loop across test string
	while( masterIndex < testStrLen )
	{
		//set internal loop index to current test string index
		internalIndex = masterIndex;

		//set internal search index to zero
		searchIndex = 0;

		//loop to end of test string
		while( internalIndex <= testStrLen && testStr[internalIndex] == 
													searchSubStr[searchIndex])
		{
		//while test string/sub string chars are the same

			//increment test string, substring indicies
			internalIndex++; searchIndex++;

			//check for end of substring
			if( searchSubStr[searchIndex] == NULL_CHAR)
			{
				//return beginning of location to sub string
				return masterIndex;
			}
		}
		//end internal comparison loop

		//increment current beginning location index
		masterIndex++;
	}
	//end loop across test string

	//assume test failed
	return SUBSTRING_NOT_FOUND;
}

/*
getStringConstrained
captures a string from the input stream with various constraints
output parameters: string returned (char*)
output returned: success of operation (bool)
dependencies: fgetc
*/
bool getStringConstrained(FILE *inStream, bool clearLeadingNonPrintable,
							bool clearLeadingSpace, bool stopAtNonprintable,
							char delimiter, char *capturedString)
{
	//printf("begin getStrignconstained\n");
	//initalize variables
	int intChar = EOF, index = 0;

	//printf("set capturedString[index] to null char\n");
	//initalize output string
	capturedString[index] = NULL_CHAR;

	if (inStream == NULL) {
    printf("Error: Input stream is not open.\n");
    return false;
	}


	//printf("capture first value in stream\n");
	//capture first value in stream
	intChar = fgetc( inStream );

	//printf("loop to clear nonprintable or space if indicated\n");
	//loop to clear nonprintable or space, if indicated
	while( ( intChar != EOF) && 
			( ( clearLeadingNonPrintable && intChar < (int)SPACE ) || 
					( clearLeadingSpace && intChar == (int)SPACE ) ) )
	{
		//get next char
		intChar = fgetc( inStream );
	}
	//end clear non printable loop

	//printf("check for end of file\n");
	//check for end of file found
	if( intChar == EOF )
	{
		printf("end of file Error\n");
		//return failed operation
		return false;
	}

	//printf("Loop to capture input: \n");
	//loop to capture input
	while( ( intChar !=EOF && index < MAX_STR_LEN -1) && 
		(( stopAtNonprintable && intChar >= (int)SPACE ) || 
									(!stopAtNonprintable )) && 
										(intChar != (char)delimiter ) )
	{
		//printf("\n\nintChar: %c, index: %d\n", intChar, index);
		//place char in array element
		capturedString[index] = (char)intChar;

		//increment array index
		index = index + 1;

		//set next element to null char / end of c string
		capturedString[ index ] = NULL_CHAR;

		//printf("current string is: %s", capturedString );

		//get another char as int
		intChar = fgetc( inStream );
	}
	//debugging
	//printf("curr str is: %s\n", capturedString );
	//end loop
	return true;
}

/*
Name: getStringLen
finds the length of string
output parameters: none
output returned: calculated length of string (int)
Dependencies: none
*/
int getStringLength(const char *testStr)
{
	//declare variables
	int index = 0, stringLen = 0;

	//loop to end of str, protect from overflow
	while( index < STD_STR_LEN && testStr[ index ] != NULL_CHAR )
	{
		//update stringLen
		stringLen = stringLen + 1;

		//update index
		index = index + 1;
	}

	//return index/length
	return stringLen;
}

/*
getStringToDelimiter
captures a string from the input stream to a specified delimiter, consumes delim
output parameters: string returned char*
output returned: success of operation (bool)
dependencies: getStringConstrained
*/
bool getStringToDelimiter( FILE *inStream, char delimiter, char *capturedStr)
{
	//printf("\ngetStringToDelimiter: call getStringConstrained:\n\n");

	//call engine function with delimiter
	return getStringConstrained( inStream, true, true, true, 
												delimiter, capturedStr);
}

/*
getStringToLineEnd
captures a string from the input stream to the end of th eline
output parameters: string returned (char*)
output returned: success of operation (bool)
dependencies: getStringConstrained
*/
bool getStringToLineEnd( FILE *inStream, char *capturedString)
{

	return getStringConstrained( inStream, true, true, true,
										NON_PRINTABLE_CHAR, capturedString );
}

/*
getSubstring
captures sub string within larger string betwen two inclusive indicies
	returns empty string if either index is out of range, assumes enough
		memory in destination string
output parameters: c style destination string char* 
output returned: none
dependencies: getStringLength, malloc, copyString, free
*/
void getSubString(char *destStr, const char *sourceStr, int startIndex, 
																int endIndex)
{
	//printf("\nbegin getSubString\n");
	//test string length
		//getStringLength
	int sourceStrLen = getStringLength(sourceStr);

	//initalize the dest index to zero
	int destIndex = 0;

	//initalize source index to start index
	int sourceIndex = startIndex;

	//create ptr for temp str
	char *tempStr;

	//check for indicies within limits
	if( startIndex >= 0 && startIndex <= endIndex && endIndex < sourceStrLen )
	{
		//printf("\nvalid indicies");

		//create temp string
			//malloc, copy string
		tempStr = (char *)malloc(sourceStrLen + 1 );
		copyString( tempStr, sourceStr);

		//loop across requested substring
		while (sourceIndex <= endIndex )
		{
			//assign src char to dest element
			destStr[destIndex] = tempStr[sourceIndex];

			//increment both
			destIndex++; sourceIndex++;

			//set temp end of dest string
			destStr[destIndex] = NULL_CHAR; 

			//printf("\ncurrent destChar is: %s", destStr);
		}
		//end loop
	
		//return memory
			//free
		free(tempStr);
	}
}

/*
setStrToLowerCase
sets string to lower case, does nothing if already lowercase
output parameters: c style dest string (char*)
output returned: none
dependencies toLowerCaseLetter, malloc, copyString, getStringLength
*/
void setStrToLowerCase( char *destStr, const char *sourceStr )
{
	//initalize function/vars
	//printf("\nsetStrToLowerCase Start\n");

	//get source string length
		//function getStringLength
	int sourceStrLen = getStringLength( sourceStr );

	//create temp string ptr
	char *tempStr;

	//create other vars
	int index = 0;

	//copy src string in case of aliasing
		//malloc, copyString
	tempStr = (char *)malloc( sourceStrLen + 1 );
	copyString( tempStr, sourceStr );
	//printf("begin while loop\n");
	//loop across src String
	while (tempStr[ index ]  != NULL_CHAR && index < MAX_STR_LEN )
	{
		//printf("looping @ index: %d\n", index);
		//set individual char to lower case as needed
		//assign dest string
		destStr[index] = toLowerCaseLetter( tempStr[ index ] );

		//update index
		index++;

		//set temp end of dest string
		destStr[index] = NULL_CHAR;

		//printf("destStr is: %s\n", destStr );
	}
	//end loop

	//debugging
	//printf("\n final dest str is: %s\n\n", destStr);

	//free memory
		//free
	free(tempStr);
}

/*
Name: toLowerCaseLetter
if character is upper case letter, returns that char as a lower case letter
output parameters: noneoutput returned: character processed as specified (char)
Dependencies: none
*/
char toLowerCaseLetter( char testChar )
{
    if( testChar >= 'A' && testChar <= 'Z' )
    {
        return testChar - 'A' + 'a';
    }
    return testChar;
}

