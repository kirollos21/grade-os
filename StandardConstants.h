// Preprocessor directives
#ifndef STANDARD_CONSTANTS_H
#define STANDARD_CONSTANTS_H

// Include Boolean
#include <stdbool.h>

// global constants

//constant for large string length
#define LARGE_STR_LEN 96

// constant for small string length
#define MIN_STR_LEN 32

// constant for maximum string length
#define MAX_STR_LEN 128

// constant for standard string length
#define STD_STR_LEN 64

//constant for command string len
#define CMD_STR_LEN 3

//constant for Input output string length
#define IO_ARG_STR_LEN 32

//constant for string argument length
#define STR_ARG_LEN 32

//constant for failure to find substring (findSubString)
#define SUBSTRING_NOT_FOUND -1

//constant for string equal (compareStrings)
#define STR_EQ 0

//constant for non printable char
#define NON_PRINTABLE_CHAR (char)127 //aka delete key


// constant for end of c-string null character
#define CARRIAGE_RETURN_CHAR '\r'

// constant for end of c-string null character
#define NEWLINE_CHAR '\n'

// constant for end of c-string null character
#define NULL_CHAR '\0'

//constant for a COLON
#define COLON ':'

//constant for a COMMA
#define COMMA ','

// constant for space character
#define SPACE ' '

//Constant for a period
#define PERIOD '.'

//Constant for semi colon
#define SEMICOLON ';'

// constant for huge string length
#define HUGE_STR_LEN 256


// columns found in grade sheet before grading columns
#define NUM_BASE_COLS 5

// constant for number of grading frequency bins
#define NUM_FREQUENCY_BINS 10

//Constant for printing a pipe
#define PIPE '|'

//Constant for PRECISION, 2 digits
#define PRECISION_2 2

//Constant for PRECISION, 4 digits
#define PRECISION_4 4

// constant for smallest string length
#define TINY_STR_LEN 16

#endif  // STANDARD_CONSTANTS
