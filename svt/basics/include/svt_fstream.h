/** \file
	This file defines svt streams inherited from stl's istream / ostream.
	Use these streams for file i/o! In addition to the stl versions,
    	theses streams throw svt_exceptions with useful error messages
    	if a file operation fails. Also, the user can be queried if he allows
    	over writing a file, and streaming char* is limited to an upper limit
    	that can be changed in order to avoid memory errors.

	\author Frank Delonge
*/


#ifndef SVT_FSTREAM
#define SVT_FSTREAM


#include <svt_basics.h>

#include <stdio.h>

#include <string.h>
#include <svt_messages.h>
#include <svt_exception.h>


#ifdef SVT_NEW_STREAMS
#  include <fstream>
#else
#  include <fstream.h>
#endif

#ifdef GPLUSPLUS3
#  define SVT_OPEN_MODE std::_Ios_Openmode
#else
#  define SVT_OPEN_MODE int
#endif

#ifdef WIN32_MSVC
#  include <io.h>
#else
#  include <unistd.h>
#endif

#ifdef SVT_NEW_STREAMS
using namespace std;
#endif


///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  class: svt_ifstream                                                  //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

/** Input File Stream Class.
	When constructed or opened, open flags for binary are 
	automatically added. On file i/o error, useful error messasges 
    are generated and thrown as svt_exception.
	\author Frank Delonge
*/
class DLLEXPORT svt_ifstream_static {
private:
  svt_ifstream_static();
  static unsigned sm_uMaxStrLen;
  friend class svt_ifstream;
};

class svt_ifstream : public ifstream {

protected:

  static  unsigned sm_uMaxStrLen;
  char    m_cMode[4];

public:

  /** Constructor.
	  @exception svt_exception if filename is specified and open fails.
  */
  svt_ifstream(const char* filename = NULL, SVT_OPEN_MODE mode = SVT_OPEN_MODE(0))
	: ifstream() {
	strcpy(m_cMode, "r");
	if (filename) 
	  open(filename, mode);
  }


  /** Destructor.
   */
  virtual ~svt_ifstream() {};


  /** Open the specified file as binary stream.
	  The falg ios::in is  atomatically added
	  to mode.
	  @exception svt_exception if filename is NULL or open fails.
  */
  inline void open(const char* filename, SVT_OPEN_MODE mode = SVT_OPEN_MODE(0) );

  /** get / set maximum string size.
   *  The given value is generally used as maximum field width
   *  when streaming in char*'s, if not specified elsewise by the 
   *  setw() stream manipulator (which is only applied for the 
   *  first following value).
   *  In both cases, the given length includes the terminating '\0' character. 
   *  Note that these mehtods is static, so
   *  calling it changes the value globally for all instaces.  
   *  if not set by setw()  
   */	
  unsigned maxStrSize() const {
	return svt_ifstream_static::sm_uMaxStrLen;
  }

  void setMaxStrSize(unsigned size) {
	svt_ifstream_static::sm_uMaxStrLen=size;
  }
  
};




///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  class: svt_ofstream                                                  //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

/** Output File Stream Class.
	When constructed or opened, open flags for writing are 
	automatically added.On file i/o error, useful error messasges 
    are generated and thrown as svt_exception.    
	\author Frank Delonge
*/

class svt_ofstream : public ofstream
{
protected:
  char m_cMode[4];

public:

  /** Constructor.
	  If filename is given, the file will be opened 
      (see open() for further explanations).
	  @exception svt_exception if filename is specified and open fails.
	  @exception svt_userInterrupt if bQueryOverwrite is true, mode does not contain ios::app abd the user cancelled overwriting
  */
  svt_ofstream(const char* filename=NULL, SVT_OPEN_MODE mode = SVT_OPEN_MODE(0), 
			   bool bQueryOverwrite=false)
	: ofstream() {
	strcpy(m_cMode, "w");
	if (filename)
	  open(filename, mode, bQueryOverwrite);
  }



  /** Destructor.
   */
  virtual ~svt_ofstream() {};


  /** Open the specified file as otuput stream.
	  The falgs ios::out is atomatically added
	  to mode. 
	  If mode does not contain ios::app, then ios::trunc is used by default.
	  If bQueryOverwrite is true and mode does not contain ios::app,
	  the user is queried if whether he wants to overwrite the file.
	  @exception svt_exception if filename is NULL or open fails.
	  @exception svt_userInterrupt if bQueryOverwrite is true, mode does not contain ios::app abd the user cancelled overwriting
  */
  inline void open(const char* filename, SVT_OPEN_MODE mode = SVT_OPEN_MODE(0), bool bQueryOverwrite=false);


};







//
// function prototypes
//

DLLEXPORT bool svt_file_exisits(const char* filename);
DLLEXPORT bool svt_read_access(const char* filename);
DLLEXPORT bool svt_write_access(const char* filename);
DLLEXPORT const char* svt_os_error(const char* filename, const char* mode);


//
// inline definitions
//


///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  class: svt_ifstream                                                  //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

inline void svt_ifstream::open(const char* filename, SVT_OPEN_MODE mode) {
  if (!filename)
	throw svt_exception("File Open Error!\n  No Filename specified!");
  
  if (!svt_read_access(filename)) {
	char szError[1024];
	if (!svt_file_exisits(filename))
	  sprintf(szError, 
			  "File Open Error:\n  The file <%s> does not exist!", 
			  filename);
	else
	  sprintf(szError, 
			  "File Open Error:\n  No read permisson for file <%s>!", 
			  filename);
	throw svt_exception(szError);
	}

  ifstream::open(filename, mode | ios::in);

  if (fail())
	{
	char szError[1024];
	sprintf(szError, 
			"File Open Error:\n  <%s>\nReason: %s\n", 
			filename, svt_os_error(filename, m_cMode));
	throw svt_exception(szError);
	}
}

///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  class: svt_ofstream                                                  //
//                                                                       //
///////////////////////////////////////////////////////////////////////////





inline void svt_ofstream::open(const char* filename, SVT_OPEN_MODE mode, bool bQueryOverwrite) 
{
  if (!filename)
	throw svt_exception("File Open Error!\n  No Filename specified!");
	
  //
  // check if file alredy exists
  //
  if (svt_file_exisits(filename))
	{
	if (!svt_write_access(filename))
	  {
	  char szError[1024];
	  sprintf(szError, "File Open Error:\n  No write permisson for file <%s>!", filename);
	  throw svt_exception(szError);
	  }

	//
	// query for overwrite if according flag is set and file shall not
	// be opened in append mode
	//
	if (bQueryOverwrite && !(mode & ios::app)) 
	  {
	  char szMessage[1024];
	  sprintf( szMessage, "File:\n   %s\nalready exists.\nOverwrite?", filename);
	  if (!svt_exception::ui()->question(szMessage))
		throw svt_userInterrupt();
	  }
	}
  
  //
  // open file for writing
  //  
  mode |= ios::out;
  if ( mode & ios::app )
	m_cMode[0]='a';
  else
	mode |= ios::trunc;

  ofstream::open(filename, mode);

  if (fail())
	{
	char szError[1024];
	sprintf(szError, 
			"File Open Error:\n  <%s>\nReason: %s\n", 
			filename, svt_os_error(filename, m_cMode));
	throw svt_exception(szError);	  
	}

}









#endif
