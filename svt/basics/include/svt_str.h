/** \file
    This file declares and defined the very simple class svt_str that can be 
    treated as char* and servers as a string class for containers based on
    char* as long as stl-strings are not commonly available (all inline).
                                                                     
    This class does the memory management to allocate/delete a       
    zero-terminated character vector.                                
    This class can also be istreamed, since it is inherited from     
    svt_streamableObject. In addition, it defines the plain stream   
    manipulators singleString and lineString to switch between       
    streaming in a whole line or single strings separated by any     
    white space characters.                                          
                                                                     
	\author Frank Delonge
*/

#ifndef SVT_STR_H
#define SVT_STR_H


//
// system includes
//
 

//
// user includes
//
#include <svt_basics.h>
#include <svt_streamableObject.h>





/** Stores the current streamIn-Mode of svt_str class 
	(singleString or lineString mode).
	\author Frank Delonge
 */
class DLLEXPORT svt_str_manip
{

public:
  typedef enum {single, line} Mode;

  static void streamSingle();
  static void streamLine();
  static Mode streamMode();

private:
  svt_str_manip();
  static Mode sm_eMode;
};

/** \name Stream Manipulators for svt_str Class
	The following stream manipulators control whether a single word or an entire
    line shall be streamed in for an svt_str instace.
	The manipulators can be called for input and output streams.
*/
//@{
inline DLLEXPORT istream& singleStrings(istream& istr) {svt_str_manip::streamSingle(); return istr;}
inline DLLEXPORT istream& lineStrings(istream& istr) {svt_str_manip::streamLine(); return istr;}
inline DLLEXPORT ostream& singleStrings(ostream& ostr) {svt_str_manip::streamSingle(); return ostr;}
inline DLLEXPORT ostream& lineStrings(ostream& ostr) {svt_str_manip::streamLine(); return ostr;}
//@}


/** svt_str is a very simple class that can be 
    treated as char* and servers as a string class for containers based on
    char* as long as stl-strings are not commonly available.
                                                                     
    This class was originally desigend only to be able to handle stl-vectors
	and -lists of char*, that automatically copy the given strings into the
	container and thereby have their own char*-memory-management. Therefore,
	using list<svt_str> or vector<svt_str> is always memory-save.<br> 
    If svt_streamableLists.h of svt_streamableVectors.h is included, 
    whole containers can be streamed in/out. See the
	test programs streamList_test and streamVector_test as an example.
    The class was redesigned to be able to also serve as simple string class 
    that automatically performs allocation and deallocation of 
    zero-terminated character vectors.                                
    In addition, instances can be istreamed, since this class inherits from
    svt_streamableObject. As an extra goody, the plain stream manipulators
    singleString and lineString can be used (for input and output streams)
    to switch between       
    streaming in a whole line or single strings separated by any     
    white space characters.                                          
                                                                     
	\author Frank Delonge
*/
class DLLEXPORT svt_str : public svt_streamableObject
{
private:
  char* m_str;

public:

  /** Constructor.
	  Creates an instance of str_str from given character string str.
	  The given string will be copied to an internal member. <br>
	  The default contructor creates an empty (NULL-)string
  */
  svt_str(const char* str = NULL);

  /** Copy-Constructor.
	  Copies the string of the given instance.
  */
  svt_str(const svt_str& that);

  /** Destructor.
  */
  virtual ~svt_str();

  /** Assign-operator for svt_str on rhs.
   */
  svt_str& operator=(const svt_str& that);

  /** Assign-operator for char* on rhs.
   */
  svt_str& operator=(const char* that);

  /** Cast-operator to const char*.
	  Together with the assign operator for char*, instances
      of svt_str can always be treated as char*.
  */
  operator const char*() const {
	return m_str;
  }

  bool operator !() const {
	return m_str==NULL;
  }

protected:

  /** Stream-Method used by operators << and >>.
   */
  virtual void streamOutA(ostream& s) const;
  virtual void streamInA (istream& s);  


  /** methods for binary i/o
   */
  virtual void streamOutB(svt_obfstream& s) const;
  virtual void streamInB(svt_ibfstream& s);  

};

#endif
