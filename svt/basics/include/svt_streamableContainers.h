/** \file
	This file contains tempalte functions and stream manipulators
	with which whole stl-containers can be streamed in/out at once.
                                                                     
	\author Frank Delonge
*/


#ifndef STREAMABLE_CONTAINERS_H
#define STREAMABLE_CONTAINERS_H


#include <svt_basics.h>
#include <svt_iostream.h>
//#include <svt_bfstream.h>
//#include <svt_types.h> 
//#include <svt_streamableObject.h>

/** For internal use only.
	This class stores all stream flags to manipulate streaming mode of stl-containers
	as static members and provides static methods to change these falgs.
	These methods are used by the plain stream manipulators containerSize()
    and noContainerSize() defined below. 
	\author Frank Delonge
*/

class DLLEXPORT svt_streamableContainers
{
private:
  /** Private Constructor => No instances of this class allowed 
   */
  svt_streamableContainers();
  
  /** Flag whether to stream in/out container size of not
   */
  static bool sm_bStreamSize;
  static bool sm_bAppend;

public:
  /** Query stream-container-size flag.
   */
  static bool streamSize();

  /** set stream-container-size flag.
	  Provided for stream manipulators containerSize() and noContainerSize()
   */
  static void streamSize(bool b);

  /** Query stream-container append flag.
   * If set to true, reading to a container will append new data,
   * otherwise the vector will be overwritten.
   */
  static bool append();

  /** set stream-container append flag.
   * If set to true, readind to a container will append new data,
   * otherwise the vector will be overwritten.
   */
  static void append(bool b);





};


/** \name Stream Manipulators for stl-Containers 
    Plain stream manipulators that specify whether container size
	shall be streamed in or out.<br> 
	Can be used for input and output streams.<br>
	containerSize turns streaming size on: <br>
	For output streams, this means streaming out
    the conatiner size as first line, then all elements separated by blanks
    (of \n for svt_str, if these are in lineString-mode). <br>
	For input streams, this means streaming in the requested amount of elements 
    first, then that many elements.<br>
	noContainerSize turns streaming size off: <br>
	For output streams, this means streaming out
    only the elements separated by blanks
    (of \n for svt_str, if these are in lineString-mode). <br>
	For input streams, this means streaming in 
	as many elements as possible (until streaming in fails for concrete data type).
*/
//@{
// DLLEXPORT istream& containerSize(istream& istr) {svt_streamableContainers::streamSize(true); return istr;}
// DLLEXPORT istream& noContainerSize(istream& istr) {svt_streamableContainers::streamSize(false); return istr;}
// DLLEXPORT ostream& containerSize(ostream& ostr) {svt_streamableContainers::streamSize(true); return ostr;}
// DLLEXPORT ostream& noContainerSize(ostream& ostr) {svt_streamableContainers::streamSize(false); return ostr;}

inline istream& containerSize(istream& istr) {svt_streamableContainers::streamSize(true); return istr;}
inline istream& noContainerSize(istream& istr) {svt_streamableContainers::streamSize(false); return istr;}
inline ostream& containerSize(ostream& ostr) {svt_streamableContainers::streamSize(true); return ostr;}
inline ostream& noContainerSize(ostream& ostr) {svt_streamableContainers::streamSize(false); return ostr;}

//class svt_ibfstream; class svt_obfstream;
//DLLEXPORT svt_ibfstream& containerSize(svt_ibfstream& istr) {svt_streamableContainers::streamSize(true); return istr;}
//DLLEXPORT svt_ibfstream& noContainerSize(svt_ibfstream& istr) {svt_streamableContainers::streamSize(false); return istr;}
//DLLEXPORT svt_obfstream& containerSize(svt_obfstream& ostr) {svt_streamableContainers::streamSize(true); return ostr;}
//DLLEXPORT svt_obfstream& noContainerSize(svt_obfstream& ostr) {svt_streamableContainers::streamSize(false); return ostr;}
//@}



/** Template function to stream in an arbritary element
  */
template <class T>
inline istream& getNext(istream& s, T& item) {
  return  (s >> item);
}

/** Template function to stream out an arbritary containter element
 	(separation with blanks)
  */
template <class T>
inline void putNext(ostream& s, const T& value) {
  s << value << "  ";
}


#ifdef SVT_STR_H
/** Partial template specailization to stream out an svt_str container element
 	(separation with blanks or \n depending on current svt_str stream mode)
*/
template <>
inline void putNext<svt_str>(ostream& s, const svt_str& value) {
  s << value;
  if (svt_str_manip::streamMode() == svt_str_manip::single)
 	s << " ";
  else
 	s << endl;

}

#endif


#endif
