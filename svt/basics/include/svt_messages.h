/********************************************************************
 *                                                                  *
 *  file: svt_messages.h                                            *
 *                                                                  *
 *  specification for svt_messages base class:                      *
 *  (used for platform independant user interaction)                *
 *                                                                  *
 *  this baseclass performs user interaction in terminal mode       *
 *  nice gui pendants should be inherited from this class           * 
 *                                                                  *
 *  f.delonge                                                       *
 *                                                                  *
 ********************************************************************/

/** \file
  Contains the specification of the svt_mesages base class.
  All dialogs or user interactions should be implemented as a 
  class inhereted from this base class, in order to have a 
  unique interface to all possible gui systems (e.g. qt),
  so that changing the gui system will not affect the underlying 
  code. <BR>
  This base class implements terminal versions of all common dialogs.
  \author Frank Delonge
*/
  


#ifndef SVT_MESSAGES_H
#define SVT_MESSAGES_H
 
//
// system includes
//


//
// user includes
//


#include <svt_basics.h> 
#include <svt_stlList.h>
#include <svt_str.h>

//
// global types and constants
//


//
// forward declarations
//
//class svt_str;
 
///////////////////////////////////////////////////////////////////////////////
// class svt_messages
///////////////////////////////////////////////////////////////////////////////
 
/** Virtual base class for platform independent user interaction.
  This class is a terminal emulation of all common dialogs.<BR> 
  To popup dialogs a GUI version should be inherited (e.g. for qt)
  \author Frank Delonge
*/
class DLLEXPORT svt_messages
{
public:
 
  /// Class Destructor.
  virtual ~svt_messages() {};
 
  /// Displays an info message.
  virtual void info(const char* pszMessage);

  /// Displays an error 
  virtual void error(const char* pszFailure);

  /// Pops up a busy message.
  virtual void busyPopup(const char* pszMessage);

  /** Let a bar rotate within the busy dialog to indicate that the program 
	  is still alive. <BR>
	  This methos lets a bar rotate one position only, so it needs to be called
	  repeatative. 
	  \param iOmitThisParameter This parameter is for internal usage only and should
                                therefore be omitted.
	  \throw svt_userInterrupt, if the user cancels action. In this case, the dialog
             pops down automatically.
  */
  virtual void busyRotate(int iOmitThisParameter=-1);

  /// Pops down the busy dialog. 	
  virtual void busyPopdown();

  /// Pops up a help dialog (info message)                              
  virtual void helpPopup(const char* pszMessage);


  /// Pops down busy message (nothing done in base class implementation)
  virtual void helpPopdown();


  /** Simple question dialog.
    To be used to ask the user a simple question, that can be answered with "Yes" or "No".
    \param pszQuestion:  The question string.
    \param bAllowCancel: Turn on/off cancel possibility <BR>
	                     (in terminal version: <ESC>-key, adds a "Cancel" button in gui versions).
	\param pszYes:       The string to be used for answer no. 1. <BR> 
	                     Set this parameter if you want to overwrite the default string "Yes"
    \param pszNo:        The string to be used for answer no. 2 <BR> 
	                     Set this parameter if you want to overwrite the default string "No"
    \param bYesDefault   if true, answer1 is default, 2 otherwise
	\return true  - if the question was answered with "Yes". <BR>
	        false - if the question was answered with "No".  
	\throw svt_userInterrupt, if the user cancels action. 
  */  
  virtual bool question(const char* pszQuestion, bool bAllowCancel=false,
						const char *pszYes="Yes", const char *pszNo="No", 
						bool bYesDefault=true );


  /** More advanced Question dialog.
	 To be used to ask the user a question with multiple possible answers.

   \param oList: List of strings: <BR>
                 Fist element is the question, followed by
                 all possible answers.
   \param bAllowCancel: turn on/off cancel possibility<BR>
                        (in text version: ESC, adds a "Cancel" button in gui verisons)
   \param iDefault:  The number of the default answer (1-n). if out of range, no default answer
                     is available.
   \return Number of chosen answer(1-n).
   \throw svt_userInterrupt, if the user cancels action. 
  */
  virtual int question(const list<svt_str>& oList, bool bAllowCancel=false, 
					   int iDefault=0);

  /** Pops up a Progress Dialog (percentage version).
    Display a progress bar. The current progress status can be updated 
	with progress(int iPercent).
	\param pszMessage A progress message that will appear in the dialog.
	\param iPercent   The initial progress status in percent.
  */
  virtual void progressPopup(const char* pszMessage, int iPercent=0);

  /** Pops up a Progress Dialog ("m of n" version).
    Display a progress bar. The current progress status can be updated with
    progress(int iPercent).
	\param pszMessage A progress message that will appear in the dialog.
	\param iDone      The initial progress status (amount of steps already performed, usually 0).
    \param iToDo      The overall amount of steps to do.
  */
  virtual void progressPopup(const char* pszMessage, int iDone, int iToDo);


  /** Pops down the progress dialog.
   Automatically performed if iPercent >= 100 or iDone >= iToDo within 
   progress() methods, or if the user cancels action. Therefore,
   it is usually not necessary to call this method manually.
  */
  virtual void progressPopdown();

  /** updates current process state by given value (percentage version).
    Automatically pops down the dialog if iPercent >= 100, or if the user cancels action.
	\param iPercent The current progress sate in percent.
	\throw svt_userInterrupt if user cancels action.
  */ 
  virtual void progress(int iPercent);

  /** updates current process state by given value ("m of n" version).
    Automatically pops down the dialog if iDone >= iToDo, or if the user cancels action.
	\param iDone Amount of steps already performed.
	\param iToDo The overall amount of steps to do.
	\throw svt_userInterrupt if user cancels action.
	\note The parameter iToDo should be kept constant for all calls of this method
	      between a progressPopup and progressPopdown.
  */ 
  virtual void progress(int iDone, int iToDo);

  /**
   * text input dialog
   * \param pszMessage A message that will appear before the text input starts
   * \param pszDefault Default answer, if user presses return
   */
  virtual string textInput(const char* pszMessage, const char* pszDefault);

};                              


 

#endif                       
