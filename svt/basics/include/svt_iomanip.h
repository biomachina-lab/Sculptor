/********************************************************************
 *                                                                  *
 *  file: svt_iomanip.h                                             *
 *                                                                  *
 *  contains functions that manipulate io                           * 
 *                                                                  *
 *  f.delonge                                                       *
 *                                                                  *
 ********************************************************************/

/** \file
  Cantains funcitons to manipulte io mode.
  e.g. switch into non-blocking read mode, or transfer typed characters
  partial instead of waiting for <ret>
  \author Frank Delonge
*/


#ifndef SVT_IO_MANIP
#define SVT_IO_MANIP


#include <svt_basics.h>

const char c_escKey=27; ///< ASCII code for <esc> key 

#ifdef WIN32_MSVC
const char c_retKey=13; ///< ASCII code for <ret> key 
#else
const char c_retKey=10; ///< ASCII code for <ret> key 
#endif

/** Set terminal in cbreak mode.
  cbreak mode means partial character processing by kernel - 
  only ^C (SIGINT), ^Z (SIGTSTP), and ^\ (SIGQUIT) are processed by kernel.
  \param bOn If true, enable cbreak mode. Also, echo is turned off. <BR>
             If false, disable cbreak mode (restores original settings) 
	
  \return -1 on error, 0 on success
*/
int DLLEXPORT svt_cbreak(bool bOn);




/** Set character reading to nonblocking/blocking mode.
  Normally, any \e read function (read, get, scanf) that reads characters
  from stdin, waits for data to be available (blocking mode). Use this function
  to change this behaviour, which means to return immediately - no matter if data 
  is available.
  \param bOn If true, enable non-blocking read mode for stdin<BR>
             If false, enable blocking read mode for stdin
  \return -1 on error, 0 on success	
  \note For non-blocked character reading, use svt_getKey()
        (which uses this function)
*/
int DLLEXPORT svt_disableReadBlock(bool bOn);



/** Read one character in either blocking or non-blocking mode.
  \param bBlocked If false (default), read in non-blocking mode. <BR>
                  If true, read in (\e normal) blocking mode.
  \return The next available character of stdin buffer, or -1, if no data is available.
  \note If you want to perform partial character processing (cbreak mode), 
        svt_cbreak(true) must be called before - up from the point, when partial characters
		shall be recognized.
  \todo May be enhanced to read special characters, e.g. up, down, F1, ... and return them as int constants.
        The problem is that special char take up to 4 bytes - 
		so if the sequence is not a recognized special key, unused bytes must be put back to read buffer.
*/
int DLLEXPORT svt_getKey(bool bBlocked = false);



#endif
