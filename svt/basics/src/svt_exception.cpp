/********************************************************************
 *                                                                  *
 *  file: svt_exception.cpp                                         *
 *                                                                  *
 *  specification for svt_exception classes:                        *
 *         svt_exception                                            *
 *         svt_userInterrupt                                        *
 *                                                                  *
 *  f.delonge                                                       *
 *                                                                  *
 ********************************************************************/


//#include <svt_fstream.h>



//
// user includes
//
 
#include <svt_exception.h>
#include <svt_messages.h>
#include <svt_string.h>

 
//
// global types and constants
//
 
// class svt_exception: init static members
svt_messages* svt_exception::s_poMessages      = new svt_messages();         



///////////////////////////////////////////////////////////////////////////
// class constructor: create an exception with given message             //
///////////////////////////////////////////////////////////////////////////
svt_exception::svt_exception(const char* pszMessage)
  : m_pszMessage(NULL)
{                                                                
  if (!s_poMessages) 
	s_poMessages = new svt_messages();

  if (pszMessage ) 
    m_pszMessage = svt_strdup(pszMessage);


}
 

///////////////////////////////////////////////////////////////////////////
// class copy constructor                                                //
///////////////////////////////////////////////////////////////////////////
svt_exception::svt_exception(const svt_exception& oException)
  : m_pszMessage(NULL)
{
  if (oException.m_pszMessage) 
	{
    if (!s_poMessages) 
	  s_poMessages = new svt_messages();
 
    m_pszMessage = svt_strdup(oException.message());
	}

}

 
///////////////////////////////////////////////////////////////////////////
// class destructor                                                      //
///////////////////////////////////////////////////////////////////////////
svt_exception::~svt_exception()
{
  if (m_pszMessage)
  	delete [] m_pszMessage;
}

 
                      
///////////////////////////////////////////////////////////////////////////
// set user interface object                                             //
///////////////////////////////////////////////////////////////////////////
void svt_exception::set_ui(svt_messages* poUserInteraction)
{
  if (poUserInteraction) 
    delete s_poMessages;
  s_poMessages = poUserInteraction;
  
}
 

void svt_exception::uiMessage()
{
  if (!s_poMessages) 
	s_poMessages = new svt_messages();
 
  s_poMessages->error(m_pszMessage);
}

svt_messages* svt_exception::ui()
{ return s_poMessages; }


svt_userInterrupt::svt_userInterrupt(const svt_userInterrupt& that)
  : svt_exception(that)  {}


svt_userInterrupt::svt_userInterrupt()
  : svt_exception("user interrupt") {}


svt_userInterrupt::~svt_userInterrupt(){}
