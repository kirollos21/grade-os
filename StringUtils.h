
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

//function prototypes
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
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
Dependencies: getStringLength, toLowerCaseLetter
*/
int compareStrings( const char *firstStr, const char *secondStr );

/*
concatenateString
appends one string ot another
output parameters: c style string char* 
output returned: none
dependencies getStringLength
*/
void concatenateString( char *destStr, const char *sourceStr);

/*
Name: copyString
Process: copies characters from source string to destination string
         up to NULL_CHAR

output parameters: destination string (char *)
output returned: none

Dependencies: none
*/
void copyString( char *destStr, const char *srcStr );

/*
findSubString
finds a given sub string inside of another string given a start and end index
output parameters: c style dest string (char*) 
output return: none
dependencies: getStringLen, malloc, copyString, free
*/
int findSubString (const char *testStr, const char *searchSubStr);

/*
getStringConstrained
captures a string from the input stream with various constraints
output parameters: string returned (char*)
output returned: success of operation (bool)
dependencies: fgetc
*/
bool getStringConstrained(FILE *inStream, bool clearLeadingNonPrintable,
							bool clearLeadingSpace, bool stopAtNonprintable,
							char delimiter, char *capturedString);

/*
Name: getStringLen
finds the length of string
output parameters: none
output returned: calculated length of string (int)
Dependencies: none
*/
int getStringLength(const char *testStr);

/*
getStringToDelimiter
captures a string from the input stream to a specified delimiter, consumes delim
output parameters: string returned char*
output returned: success of operation (bool)
dependencies: getStringConstrained
*/
bool getStringToDelimiter( FILE *inStream, char delimiter, char *capturedStr);

/*
getStringToLineEnd
captures a string from the input stream to the end of th eline
output parameters: string returned (char*)
output returned: success of operation (bool)
dependencies: getStringConstrained
*/
bool getStringToLineEnd( FILE *inStream, char *capturedString);

/*
getSubstring
captures sub string within larger string betwen two inclusive indicies
	returns empty string if either index is out of range, assumes enough
		memory in destination string
output parameters: c style destination string char* 
output returned: none
dependencies: getStringLength, malloc, copyString, free
*/
void getSubString(char *destStr, const char *sourceStr, int startIndex, int endIndex);


/*
setStrToLowerCase
sets string to lower case, does nothing if already lowercase
output parameters: c style dest string (char*)
output returned: none
dependencies toLowerCaseLetter, malloc, copyString
*/
void setStrToLowerCase( char *destStr, const char *sourceStr );

/*
Name: toLowerCaseLetter
if character is upper case letter, returns that char as a lower case letter
output parameters: noneoutput returned: character processed as specified (char)
Dependencies: none
*/
char toLowerCaseLetter( char testChar );

#endif //STRING_UTILS_H
