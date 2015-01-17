/********************************************************************
 *                                                                  *
 *  file: svt_string.h                                              *
 *                                                                  *
 *  Description: string utilities                                   *
 *                                                                  *
 *                                                                  *
 *  f.delonge                                                       *
 *                                                                  *
 ********************************************************************/

#ifndef INCLUDE_SVT_STR_H
#define INCLUDE_SVT_STR_H

// system includes
#include <sstream>
#include <iostream>
// svt includes
#include <svt_stlString.h>
#include <svt_stlVector.h>
#include <svt_structs.h>


using namespace std;
/**
 * \file Contains general string functions.
 * \author Frank Delonge
 */

/**
 * Copies the given string.
 * To be used instead of strdup() to make sure that memory is allocated
 * with new. Therefore, the returned string must be deleted with delete [].
 * (NULL-Pointer safe)
 */
DLLEXPORT char* svt_strdup  (const char* pszString );

/**
 * Checks if 2 strings are equal. (NULL-Pointer safe)
 */
DLLEXPORT bool  svt_strequal(const char* pszString1, const char* pszString2);

/**
 * same as strcmp(), but NULL-Pointer safe.
 */
DLLEXPORT int   svt_strcmp  (const char* pszString1, const char* pszString2);

/**
 * Removes leading white spaces.
 */
DLLEXPORT char* svt_strCompressL( char*pszString );

/**
 * Removes ending white spaces.
 */
DLLEXPORT char* svt_strCompressR( char*pszString );

/**
 * Removes both leading and ending white spaces.
 */
DLLEXPORT char* svt_strCompress( char*pszString );

/**
 * Non lexicographical sort...could be used with sort in case there is a vector of strings
 * containing numbers and characters
 */
DLLEXPORT bool svt_nonLexSort(const string& left, const string& right);

/**
 * Expands a range. i.e when given 1-3,A-B returns a vector with 1,2,3,A,B,C
 */
DLLEXPORT vector<string > svt_expandRange(string str);

/**
 * Makes a range out of consecutive numbers and characters, i.e when a vector containing
 * "1,2,3,A,B,C" is passed, this method returns "1-3,A-B"
 */
DLLEXPORT string svt_makeRange(vector<string> nums);

/**
 * Makes the occurences of sep unique in the string, also removes separator from first and last
 * positions of the string if present, i.e an input of ",,1,2,,,A," returns "1,2,A"
 */
DLLEXPORT string svt_removeDuplicateSep(string sep, string str);

#endif
