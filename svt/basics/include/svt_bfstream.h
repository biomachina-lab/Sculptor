/** \file
	This file contains stream classes for binary i/o inherited from 
	ifstream and ofstream. 
	Operators << and >> are overloaded for all built-in data types and
	littleEndian / bigEndian swapping is already considered.
                                                                     
	\author Frank Delonge
*/


#ifndef SVT_BF_STREAM
#define SVT_BF_STREAM

#include <string.h>

#include <svt_fstream.h>

#include <svt_binary_io.h>

//#include <iomanip.h>



///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  class: svt_bstream                                                   //
//                                                                       //
///////////////////////////////////////////////////////////////////////////



enum svt_SwapMode {NEVER_SWAP, SWAP_BIG_ENDIAN, SWAP_LITTLE_ENDIAN, AS_GLOBAL};



class DLLEXPORT svt_bstream_global {

private:

  svt_bstream_global() {};

  static svt_SwapMode sm_eSwapMode; 
  static bool sm_bSwapping;

public:
  
  static void setSwapMode(svt_SwapMode m);
  static svt_SwapMode swapMode();
  static bool swapping ();

};



class svt_bstream 
{


private:

  svt_bstream(const svt_bstream&){};
  bool m_bSwapping;
  svt_SwapMode m_eSwapMode;

protected:

  svt_bstream()
	: m_bSwapping(svt_bstream_global::swapping()), m_eSwapMode(AS_GLOBAL) {}



public:

  virtual ~svt_bstream() {};


  svt_SwapMode globalSwapMode() const {
	return svt_bstream_global::swapMode(); 
  }

  void setGlobalSwapMode(svt_SwapMode m) {
	svt_bstream_global::setSwapMode(m);
  }


  svt_SwapMode swapMode() const {
	return (m_eSwapMode==AS_GLOBAL) ? globalSwapMode() : m_eSwapMode;  
  }


  void setSwapMode(svt_SwapMode m) {
	m_eSwapMode = m;
	if (m==AS_GLOBAL)
	  m_bSwapping = svt_bstream_global::swapping();
	else
	  m_bSwapping =    (  svt_BigEndian() && (m==SWAP_BIG_ENDIAN)    )
		             || ( !svt_BigEndian() && (m==SWAP_LITTLE_ENDIAN) );
  }


  bool globalSwapping() const {
	return svt_bstream_global::swapping();
  }

  bool swapping() const {
	return (m_eSwapMode==AS_GLOBAL) ? globalSwapping() : m_bSwapping; 
  }




};



///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  class: svt_ifbstream                                                 //
//                                                                       //
///////////////////////////////////////////////////////////////////////////


//
// define some macros to delegate streaming
// of built-in types to base class
//

//
// stream and swap elements if desired
// (use this macro for types T with sizeof(T)>1)
//

#ifdef SVT_DEFINE_READ_BIN_OPERATOR
#undef SVT_DEFINE_READ_BIN_OPERATOR
#endif

#define SVT_DEFINE_READ_BIN_OPERATOR(type)       \
  svt_ibfstream& operator>> (type& value) {      \
	read((char*)&value, sizeof(type));           \
	if (swapping()) value=svt_swap(value);       \
    return *this;                                \
  }


//
// stream element, no swap
// (use this macro for types T with sizeof(T)==1)
//

#ifdef SVT_DEFINE_READ_BIN_OPERATOR_CH
#undef SVT_DEFINE_READ_BIN_OPERATOR_CH
#endif

#define SVT_DEFINE_READ_BIN_OPERATOR_CH(type)   \
  svt_ibfstream& operator>> (type& value) {     \
	read((char*)&value, sizeof(type));          \
	return *this;                               \
  }



//
// stream char* - strings
// marcro for all char*-types
//

#ifdef SVT_DEFINE_READ_BIN_OPERATOR_STR
#undef SVT_DEFINE_READ_BIN_OPERATOR_STR
#endif

#define SVT_DEFINE_READ_BIN_OPERATOR_STR(type)                    \
  svt_ibfstream& operator>> (type str) {                          \
    if (str) {                                                    \
      unsigned max_size = width();                                \
      if (max_size <= 0)                                          \
	    max_size=maxStrSize();                                    \
  	  else                                                        \
	    width(0);                                                 \
      unsigned i=0;                                               \
      char c;                                                     \
      while ((i<max_size-1) && (*this >> c)) {                    \
        str[i++]=c;                                               \
        if (c=='\0') break;                                       \
      }                                                           \
      if (c!='\0') str[i]='\0';                                   \
	  if (!fail())                                                \
		if ( peek()=='\0' ) *this>>c;                             \
    }                                                             \
    return *this;                                                 \
  }



#ifdef IRIX
#define SVT_BINARY_FLAG 0
#else
#define SVT_BINARY_FLAG ios::binary
#endif

/** Input Binary File Stream Class.
	When constructed or opened, open flags for binary reading are 
	automatically added.
	\author Frank Delonge
*/
class svt_ibfstream : public svt_ifstream, public svt_bstream {

public:

  /** Constructor.
	  @exception svt_exception if filename is specified and open fails.
  */
  svt_ibfstream(const char* filename = NULL, SVT_OPEN_MODE mode = SVT_OPEN_MODE(0))  
	: svt_ifstream(), svt_bstream() {

	strcpy(m_cMode, "rb");
	if (filename) 
	  open(filename, mode);
  }


  /** Destructor.
   */
  virtual ~svt_ibfstream() {};


  /** Open the specified file as binary stream.
	  The falgs ios::binary and ios::in are atomatically added
	  to mode.
	  @exception svt_exception if filename is NULL or open fails.
  */
  void open(const char* filename, SVT_OPEN_MODE mode = SVT_OPEN_MODE(0)) {
	svt_ifstream::open(filename, mode | SVT_BINARY_FLAG);
  }

  /** Stream operators for binary reading of built-in types.
   */
  //@{
  SVT_DEFINE_READ_BIN_OPERATOR(bool)
  SVT_DEFINE_READ_BIN_OPERATOR(short)
  SVT_DEFINE_READ_BIN_OPERATOR(unsigned short)
  SVT_DEFINE_READ_BIN_OPERATOR(int)
  SVT_DEFINE_READ_BIN_OPERATOR(unsigned int)
  SVT_DEFINE_READ_BIN_OPERATOR(long)
  SVT_DEFINE_READ_BIN_OPERATOR(unsigned long)
  SVT_DEFINE_READ_BIN_OPERATOR(float)
  SVT_DEFINE_READ_BIN_OPERATOR(double)

  SVT_DEFINE_READ_BIN_OPERATOR_STR(char*)
  SVT_DEFINE_READ_BIN_OPERATOR_STR(signed char*)
  SVT_DEFINE_READ_BIN_OPERATOR_STR(unsigned char*)

  SVT_DEFINE_READ_BIN_OPERATOR_CH(char)
  SVT_DEFINE_READ_BIN_OPERATOR_CH(signed char)
  SVT_DEFINE_READ_BIN_OPERATOR_CH(unsigned char)
  //@}

  /** operators to delegate stream manipulators
   */
  //@{
	//  svt_ibfstream& operator>> (svt_ibfstream& (*f)(svt_ibfstream&)) {
	//	return (*f)(*this) ; }

  //  svt_ibfstream& operator>> (ios& (*f)(ios&) ) ;
  //@}
};



#undef SVT_DEFINE_READ_BIN_OPERATOR
#undef SVT_DEFINE_READ_BIN_OPERATOR_CH
#undef SVT_DEFINE_READ_BIN_OPERATOR_STR



///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  class: svt_ofbstream                                                 //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

//
// define some macros to delegate streaming
// of built-in types to base class
//

//
// stream and swap elements if desired
// (use this macro for types T with sizeof(T)>1)
//

#ifdef SVT_DEFINE_WRITE_BIN_OPERATOR
#undef SVT_DEFINE_WRITE_BIN_OPERATOR
#endif

#define SVT_DEFINE_WRITE_BIN_OPERATOR(type)             \
  svt_obfstream& operator<< (const type& value) {       \
	if (swapping()) {                                   \
      type swapped_value = svt_swap(value);             \
	  write((char*)&swapped_value, sizeof(type));       \
    } else                                              \
	  write((char*)&value, sizeof(type));               \
    return *this;                                       \
  }


//
// stream element
// (use this macro for types T with sizeof(T)==1)
//     
#ifdef SVT_DEFINE_WRITE_BIN_OPERATOR_CH
#undef SVT_DEFINE_WRITE_BIN_OPERATOR_CH
#endif 

#define SVT_DEFINE_WRITE_BIN_OPERATOR_CH(type)      \
  svt_obfstream& operator<< (const type& value) {   \
	write((char*)&value, sizeof(type));             \
	return *this;                                   \
  }


//
// stream char* - strings
// marcro for all char*-types
//
#ifdef SVT_DEFINE_WRITE_BIN_OPERATOR_STR
#undef SVT_DEFINE_WRITE_BIN_OPERATOR_STR
#endif 

#define SVT_DEFINE_WRITE_BIN_OPERATOR_STR(type)                                      \
  svt_obfstream& operator<< (type str) {                                     \
	if ((char*)str) write((char*)str, (strlen((char*)str)+1)*sizeof(char));  \
	return *this;                                                            \
  }


/** Output Binary File Stream Class.
	When constructed or opened, open flags for binary reading are 
	automatically added.
	\author Frank Delonge
*/
class svt_obfstream : public svt_ofstream, public svt_bstream
{

public:

  /** Constructor.
	  @exception svt_exception if filename is specified and open fails.
  */
  svt_obfstream(const char* filename=NULL, SVT_OPEN_MODE mode = SVT_OPEN_MODE(0), bool bQueryOverwrite=false)
	: svt_ofstream(), svt_bstream() {
	strcpy(m_cMode, "wb");
  	if (filename)
	  open(filename, mode, bQueryOverwrite);
  }

  /** Destructor.
   */
  virtual ~svt_obfstream() {};


  /** Open the specified file as binary stream.
	  The falgs ios::binary and ios::out are atomatically added
	  to mode. 
	  See also svt_ofstream::open() for further explanation.
  */
  void open(const char* filename, SVT_OPEN_MODE mode = SVT_OPEN_MODE(0), bool bQueryOverwrite=false){
	svt_ofstream::open(filename, mode | SVT_BINARY_FLAG, bQueryOverwrite);
  }



  /** Stream operators for binary writing of built-in types.
   */
  //@{
  SVT_DEFINE_WRITE_BIN_OPERATOR(bool)
  SVT_DEFINE_WRITE_BIN_OPERATOR(short)
  SVT_DEFINE_WRITE_BIN_OPERATOR(unsigned short)
  SVT_DEFINE_WRITE_BIN_OPERATOR(int)
  SVT_DEFINE_WRITE_BIN_OPERATOR(unsigned int)
  SVT_DEFINE_WRITE_BIN_OPERATOR(long)
  SVT_DEFINE_WRITE_BIN_OPERATOR(unsigned long)
  SVT_DEFINE_WRITE_BIN_OPERATOR(float)
  SVT_DEFINE_WRITE_BIN_OPERATOR(double)

  SVT_DEFINE_WRITE_BIN_OPERATOR_STR(char*)
  SVT_DEFINE_WRITE_BIN_OPERATOR_STR(const char*)
  SVT_DEFINE_WRITE_BIN_OPERATOR_STR(signed char*)
  SVT_DEFINE_WRITE_BIN_OPERATOR_STR(const signed char*)
  SVT_DEFINE_WRITE_BIN_OPERATOR_STR(unsigned char*)
  SVT_DEFINE_WRITE_BIN_OPERATOR_STR(const unsigned char*)


  SVT_DEFINE_WRITE_BIN_OPERATOR_CH(char)
  SVT_DEFINE_WRITE_BIN_OPERATOR_CH(signed char)
  SVT_DEFINE_WRITE_BIN_OPERATOR_CH(unsigned char)
  //@}

  /** operators ro delegate stream manipulators
   */
  //@{
	//  svt_obfstream& operator<< (svt_obfstream& (*f)(svt_obfstream&)) {
	//	return (*f)(*this) ; }

  //  svt_ibfstream& operator>> (ios& (*f)(ios&) ) ;
  //@}
};



#undef SVT_DEFINE_WRITE_BIN_OPERATOR
#undef SVT_DEFINE_WRITE_BIN_OPERATOR_CH
#undef SVT_DEFINE_WRITE_BIN_OPERATOR_STR



////////////////////////////////////////////////////////////////////////////////////
//                                                                                //
// stream manipulator:                                                            //
//                                                                                //
//           setSwapMode(svt_bstream::SwapMode)                                   //
//                                                                                //
//                                                                                //
////////////////////////////////////////////////////////////////////////////////////



// class svt_bstream_manip_mode {
// public:
//   svt_ibfstream& (*f1)(svt_ibfstream&, svt_bstream::SwapMode); 
//   svt_obfstream& (*f2)(svt_obfstream&, svt_bstream::SwapMode); 

//   svt_bstream::SwapMode m;

//   svt_bstream_manip_mode(svt_ibfstream& (*ff)(svt_ibfstream&, svt_bstream::SwapMode), svt_bstream::SwapMode mm)
// 	: f1(ff), m(mm) {}

//   svt_bstream_manip_mode(svt_obfstream& (*ff)(svt_obfstream&, svt_bstream::SwapMode), svt_bstream::SwapMode mm)
// 	: f2(ff), m(mm) {}

// };





// inline svt_ibfstream& operator>>(svt_ibfstream& s, svt_bstream_manip_mode& m) {
//   return m.f1(s,m.m);
// }

// inline svt_obfstream& operator<<(svt_obfstream& s, svt_bstream_manip_mode& m) {
//   return m.f2(s,m.m);
// }


// inline svt_bstream& setSwapMode_h(svt_bstream& s, svt_bstream::SwapMode m) {
//   return s.setSwapMode(m);
// }


// inline svt_bstream_manip_mode setSwapMode(svt_bstream::SwapMode m) {
//   return svt_bstream_manip_mode(setSwapMode_h, m);
// }




#endif
