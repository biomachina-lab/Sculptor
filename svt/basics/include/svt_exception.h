/********************************************************************
 *                                                                  *
 *  file: svt_exception.h                                           *
 *                                                                  *
 *  define exception classes svt_exception (base class)             *
 *                           svt_userInterrupt                      *
 *                           svt_Warning                            *
 *  (pure inline template class)                                    *
 *                                                                  *
 *  f.delonge                                                       *
 *                                                                  *
 ********************************************************************/

/*! \file
  
  Defines general exception classes for svt (pure inline).
  All exceptions are inherited  from svt_exception.
  \sa svt_messages
  \author Frank Delonge
*/

#ifndef SVT_EXCEPTION_H
#define SVT_EXCEPTION_H


#include <svt_basics.h>

class svt_messages;
 
///////////////////////////////////////////////////////////////////////////////
// class svt_exception
///////////////////////////////////////////////////////////////////////////////
 


/** General exception class to be thrown by SVT libraries.
  An svt_messages object is instantiated for error messages or user 
  interaction as static member.
  \author Frank Delonge
 */

class DLLEXPORT svt_exception
{
public:

  /** Class Constructor.
	Creates an exception with given message.
	\param pszMessage The message to be thrown along with the exception
   */
  svt_exception(const char* pszMessage = NULL);

  /** Copy constructor.
   */
  svt_exception(const svt_exception& oException);

  /** Class destructor.
   */
  virtual ~svt_exception();

  /** Returns the message thrown along with the exception.
	If the message shall be shown to the user, either use ui() or, 
	for convenience only, uiMessage() for an error message.
   */
  const char* message() const;

  /** Sets svt_messages object.
	  Deletes current object!
	  This method has to be called if the (default) terminal-version
	  of svt_messages shall be replaces with a svt_messages object that
	  refers to a different ui, e.g. qt.
	  @see svt_messages
   */
  static void set_ui(svt_messages* poUserInteraction);

  /** Returns pointer to the currently used User Interface (svt_messages).
	  Provided for user interaction, e.g. to display error, warnings...
	  Refers to different user interfaces, e.g. qt or terminal version.
	  \sa svt_messages
   */
  static svt_messages* ui();

  /** Display message in error box.
	  For convenience: same as svt_exception::ui()->error(oError.message()).
   */
  void uiMessage();

 
protected:
  /// The message string.
  char*        m_pszMessage;

 
private:
  /// Currently connected User Interface.
  static svt_messages* s_poMessages;

 
};
                                         
///////////////////////////////////////////////////////////////////////////////
// class svt_userInterrupt
///////////////////////////////////////////////////////////////////////////////
 
/** User interrupt exception.
 Inherited from svt_exception to destinguish between errors and interrupts	
 \author Frank Delonge
 */
class DLLEXPORT svt_userInterrupt : public svt_exception
{
public:
  /// Class Constructor. Sets message to "user interupt".
  svt_userInterrupt();

  svt_userInterrupt(const svt_userInterrupt& that);

  /// Class Destructor
  virtual ~svt_userInterrupt();
  
};
 
///////////////////////////////////////////////////////////////////////////////
// inline definitions
///////////////////////////////////////////////////////////////////////////////
 

 
inline const char* svt_exception::message() const
{ return m_pszMessage; }




#endif         
