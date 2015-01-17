/********************************************************************
 *                                                                  *
 *  file: svt_messages.h                                            *
 *                                                                  *
 *  implememtation for svt_messages base class:                     *
 *  (used for platform independant user interaction)                *
 *                                                                  *
 *  this baseclass performs user interaction in terminal mode       *
 *  nice gui pendants should be inherited from this class           * 
 *                                                                  *
 *  f.delonge                                                       *
 *                                                                  *
 ********************************************************************/


//
// system includes
//
 
#include <math.h>
#include <ctype.h>  

//
// user includes
//

#include <svt_messages.h> 
#include <svt_iomanip.h>
#include <svt_exception.h>
#include <svt_str.h>
#include <svt_iostream.h>

#define QUESTION cerr << svt_trimName("question")
#define ERROR cerr << svt_trimName("error")
#define INFO cout << svt_trimName("info")
#define TEXT cout << svt_trimName("text input")

///////////////////////////////////////////////////////////////////////////
// display busy message                                                  //
// (to be replaced by busy box in any gui version)                       //
///////////////////////////////////////////////////////////////////////////
void svt_messages::busyPopup(const char* pszMessage)
{
  svtout << "    Busy.... ";
  if (pszMessage)
	cout << pszMessage;
  cout << " (press ESC to cancel)";
  svt_cbreak(true);
  busyRotate(0);
}


//////////////////////////////////////////////////////////////////////
// let busy slash rotate                                            //
// omit the parameter - for internal use only                       //
//////////////////////////////////////////////////////////////////////
void svt_messages::busyRotate(int iOmitThisParameter)
{

  static const char s_rotChars[] = "|\\-/";
  static int s_iPos = 0;

  //
  // check on esc press ( -> cancel )
  //
  int iKey;
  while ( (iKey=svt_getKey()) != -1 )
 	if ( iKey == c_escKey )
	  {
	  cout << endl << "Cancelled!" << endl;
	  busyPopdown();
	  throw svt_userInterrupt();
	  }

  if (iOmitThisParameter==0)
	s_iPos = 0;
  else
	s_iPos++;
  
  cout << "\r  " << s_rotChars[s_iPos%4] << "\r" << flush;

}


///////////////////////////////////////////////////////////////////////////
// popdown busy message                                                  //
// (just cout "Ready" for terminal version)                              //
///////////////////////////////////////////////////////////////////////////
void svt_messages::busyPopdown()
{
  svt_cbreak(false);
  svtout << endl << "Ready" << endl;
}
 

///////////////////////////////////////////////////////////////////////////
// ask user a simple question, to be answered with                       //
// "Yes" (->true) or "No"(->false)                                       //
//                                                                       //
// Parameters:                                                           //
// pszQuestion:  the question as string                                  //
// bAllowCancel: turn on/off cancel possibility (in text version: ESC)   //
//               if cancelled, svt_userInterrupt is thrown               //
// pszYes:       string for answer no. 1 (for which true is returned)    //
// pszNo:        string for answer no. 2 (for which false is returned)   //
// bYesDefault   if true, answer1 is default, 2 otherwise                //
// (to be replaced by question dialog box in gui version)                //
///////////////////////////////////////////////////////////////////////////
bool svt_messages::question(const char* pszQuestion, bool bAllowCancel,
							const char *pszYes, const char *pszNo, 
							bool bYesDefault)
{

  char cYes = pszYes ? pszYes[0] : 'y';
  char cNo  = pszNo  ? pszNo[0] : 'n';

  char cDefault = bYesDefault ? cYes : cNo;
  bool bDefaultAnswer=bYesDefault;
	
  QUESTION << pszQuestion << " (" << cYes << "/" << cNo;

  if (bAllowCancel)
	cout << "/ESC";

  cout<<") [" << cDefault << "]" << endl;
  QUESTION << flush;

  bool bAnswered = false;
  while (!bAnswered)
  {
      svt_cbreak(true);
      char c = svt_getKey(true);
      svt_cbreak(false);

      if ( bAllowCancel && (c == c_escKey) )
      {
          cout << endl;
          QUESTION << "Cancelled!" << endl;
          throw svt_userInterrupt();
      }

      if ( c == c_retKey )
      {
          cout << cDefault << endl;
          return bDefaultAnswer;
      }
      c = tolower(c);

      if ( c == tolower(cYes) )
      {
          cout << cYes << endl;
          return true;
      }

      if ( c == tolower(cNo) )
      {
          cout << cNo << endl;
          return false;
      }

  }

  return true;
}
                                                      
///////////////////////////////////////////////////////////////////////////
// ask user a question, multiple possible answers                        // 
// oList:        list of strings, fist element is question, followed by  //
//               all possible answers                                    // 
// bAllowCancel: turn on/off cancel possibility (in text version: ESC)   //
//               if cancelled, svt_userInterrupt is thrown               //
// iDefault:     default answer (1-n). if out of range, no default answer//
//               is available                                            //
// returns number of chosen answer(1-n)                                  //
// (to be replaced by question dialog box in gui version)                // 
///////////////////////////////////////////////////////////////////////////
int svt_messages::question(const list<svt_str>& oList, bool bAllowCancel, 
						   int iDefault)
{
  if (oList.size() <=1 )
	return 0;

  if (oList.size() == 2)
	return 1;



  list<svt_str>::const_iterator il = oList.begin();
  cout << "Question: " <<  *il;
  if (bAllowCancel)
	cout << "  (ESC to cancel)";
  cout << endl;

  int i=1;
  il++;
  for (;il != oList.end(); il++)
	{
	if ( i == iDefault )
	  cout << "*";
	else 
	  cout << " ";

	cout.width(2);
	cout << i;
	cout.width(0);

	cout << ". " << *il << endl;
	i++;
	}
  cout << "> " << flush;

  bool bAnswered = false;
  int iAnswer = 0;

  while (!bAnswered)
	{
	svt_cbreak(true); 
	char c = svt_getKey(true);
	svt_cbreak(false); 

	if ( bAllowCancel && c == c_escKey )
	  {
	  cout << endl << "Cancelled!" << endl;
	  throw svt_userInterrupt();
	  }	

	if ( c == c_retKey && iDefault >= 1 && unsigned(iDefault) < oList.size() )
	  return iDefault;
	
	iAnswer = c - '0';
	if ( iAnswer >= 1 && unsigned(iAnswer) < oList.size() )
	  return iAnswer;
	} 

  
  return iAnswer;
}
 




///////////////////////////////////////////////////////////////////////////
// advance progress status (bar-style version) with new value iPercent   // 
// checks for ESC press, throws svt_userInterrupt if pressed             //
// pops down if iPercent >=100                                           //
///////////////////////////////////////////////////////////////////////////
void svt_messages::progress(int iPercent)
{
  //
  // check on esc press ( -> cancel )
  //
  int iKey;
  while ( (iKey=svt_getKey()) != -1 )
      if ( iKey == c_escKey )
      {
	  cout << endl; svtout << "Cancelled!" << endl;
	  progressPopdown();
	  throw svt_userInterrupt();
      }

  // 
  // display new percent value if < 100
  //
  svtout << "  <|";
  int i;
  for ( i=1; i<=iPercent/3; i++)
	cout << "=";
  for(;i<=33;i++)
	cout << " ";
  cout << "|>   ";

  cout.width(3);
  cout << iPercent;
  cout.width(0);
  cout << " %        \r" << flush;
}


///////////////////////////////////////////////////////////////////////////
// advance progress status (i of n version) with new values              // 
// checks for ESC press, throws svt_userInterrupt if pressed             //
// pops down if iPercent >=100                                           //
/////////////////////////////////////////////////////////////////////////// 
void svt_messages::progress(int iDone, int iToDo)
{

  //
  // check on esc press ( -> cancel )
  //
  int iKey;
  while ( (iKey=svt_getKey()) != -1 )
      if ( iKey == c_escKey )
      {
	  cout << endl; svtout << "Cancelled!" << endl;
	  progressPopdown();
	  throw svt_userInterrupt();
      }

  // 
  // display new values if iDone < iToDo
  //  
  svtout << "  <|";
  int i;
  for ( i=1; i<= int(iDone * (33.0/iToDo)); i++)
	cout << "=";
  for(;i<=33;i++)
	cout << " ";
  cout << "|>   ";
  cout.width(int(log10((float)iToDo))+1);
  cout << iDone;
  cout.width(0);
  cout << " / " << iToDo << "     \r" << flush;
}

                                                
///////////////////////////////////////////////////////////////////////////
// print a progress message (bar style)                                  //
// initialized with iPercent (=0 by default)                             // 
// (to be replaced by a progress box in gui version)                     //
///////////////////////////////////////////////////////////////////////////
void svt_messages::progressPopup(const char* pszMessage, int iPercent)
{
    svtout << pszMessage << " (press ESC to cancel)" << endl;
    svt_cbreak(true);
    progress(iPercent);
}


///////////////////////////////////////////////////////////////////////////
// print a progress message (style: iDone of iToDo)                      //
// init with given values                                                // 
// (to be replaced by a progress box in gui version)                     //
/////////////////////////////////////////////////////////////////////////// 
void svt_messages::progressPopup(const char* pszMessage, int iDone, int iToDo)
{
  svtout << pszMessage << " (press ESC to cancel)" << endl;
  svt_cbreak(true);
  progress(iDone,iToDo);
}



///////////////////////////////////////////////////////////////////////////
// pop down the progress dialog                                          // 
// this method is automatically called when iPercent >= 100 or if the    // 
// user cancelles action                                                 //
/////////////////////////////////////////////////////////////////////////// 
void svt_messages::progressPopdown()
{
    svt_cbreak(false);
    svtout << endl;
}







void svt_messages::info(const char* pszMessage)
{
    INFO << pszMessage << endl;
}

void svt_messages::error(const char* pszFailure)
{ 
    ERROR << pszFailure << endl; 
}

void svt_messages::helpPopup(const char* pszMessage)
{
    cout << pszMessage << endl;
}

inline void svt_messages::helpPopdown()
{
}

/**
 * text input dialog
 * \param pszMessage A message that will appear before the text input starts
 * \param pszDefault Default answer, if user presses return
 */
string svt_messages::textInput(const char* pszMessage, const char* pszDefault)
{
    string oAns;

    TEXT << pszMessage << endl;
    TEXT << flush;

    cin >> oAns;

    if (oAns.size() == 0)
        oAns = string(pszDefault);

    return oAns;
};

