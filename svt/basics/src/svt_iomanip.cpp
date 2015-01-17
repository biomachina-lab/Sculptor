/********************************************************************
 *                                                                  *
 *  file: svt_iomanip.cpp                                           *
 *                                                                  *
 *  contains functions that manipulate io                           * 
 *                                                                  *
 *  f.delonge                                                       *
 *                                                                  *
 ********************************************************************/

#ifndef WIN32
#  include <unistd.h>
#  include <fcntl.h>
#  include <termios.h>
#else
#  include <conio.h>
#  include <stdio.h>
#endif

#include <svt_iomanip.h>


//
// int svt_cbreak(bool bOn) :
// if bOn is true, put console into cbreak mode; partial character processing by kernel
// only ^C (SIGINT), ^Z (SIGTSTP), and ^\ (SIGQUIT) are processed by kernel
// Otherwise, restore original terminal settings
//
// OUT : -1 on error, 0 otherwise
//
#ifndef WIN32
int svt_cbreak(bool bOn)
{
  struct termios stAttr;
  static struct termios s_stOrigAttr;
  static bool s_bOn = false;  

  // 
  // check for tty on stdin
  //
  if ( !(isatty(0)) )
	return -1;
  
  if ( bOn == s_bOn )
	return 0;
  
  if (bOn)
	{
	//
	// get term attributes, save them in s_stOrigAttr
	if ( tcgetattr(0,&stAttr) < 0 )
	  return -1;
	
	s_stOrigAttr = stAttr;
	
	//
	// manipulate term atiibutes for cbreak mode and set them
	//
	stAttr.c_lflag     &= ~(ICANON | ECHO);
	stAttr.c_cc[VMIN]   = 1;
	stAttr.c_cc[VTIME]  = 0;
	
	if ( tcsetattr(0,TCSAFLUSH,&stAttr) < 0)
	  return -1;

	}
  else
	{
	//
	// restore old settings if bOn == false
	//
	if ( tcsetattr(0, TCSAFLUSH, &s_stOrigAttr) < 0 )
	  return -1;
	}

  s_bOn = bOn;
  return 0;
}
#else

static bool s_bCbreak = false;

int svt_cbreak(bool bOn)
{
  s_bCbreak = bOn;
  return 0;
}
#endif




//
// int svt_disableReadBlock(bool bOn)
// if bOn is true, put console into non-blocked reading 
// (read will not wail for input)
//
// OUT : -1 on error, 0 otherwise
//
#ifndef WIN32
int svt_disableReadBlock(bool bOn)
{
  static int s_iFlSave;
  static bool s_bOn = false;

  if (bOn == s_bOn) 
	return 0;

  if ( bOn )
	{
	s_iFlSave = fcntl(0, F_GETFL);
	fcntl(0,F_SETFL, O_NONBLOCK);
	}
  else
	fcntl(0,F_SETFL,s_iFlSave);

  s_bOn = bOn;
  return 0;

}
#else
int svt_disableReadBlock(bool bOn)
{
  return 0;
}
#endif
  
//
// int svt_getKey()
// 
// returns next available character of stdin buffer, or -1, if none available
// does not block while reading, returns immediately
//
// may be enhanced to read special characters, e.g. up, down, F1, ... and return them as int constants
// problem: special char take up to 4 bytes, unused bytes must be put back to read buffer
#ifndef WIN32
int svt_getKey(bool bBlocked)
{

  char c;
  int i;
 
  if (!bBlocked)
	svt_disableReadBlock(true);

  if ( read(0, &c, 1) == 1 )
	i = c;
  else
	i = -1;

  if (!bBlocked)
	svt_disableReadBlock(false);
  return i;

}
#else
int svt_getKey(bool bBlocked)
{

  int i;
 
  if (!bBlocked)
	{
	if (!_kbhit())
	  return -1;
	}	

  if (s_bCbreak)
	i = _getch();
  else
	i = getchar();

  if (i>0 && i <256)
	return i;
  else
	return -1;
  
}
#endif
