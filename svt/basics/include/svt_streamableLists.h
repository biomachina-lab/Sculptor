/** \file
	This file contains template stream functions for streaming stl-lists
	with which whole stl-lists can be streamed in/out at once.
                                                                     
	\author Frank Delonge
*/

#ifndef STREAMABLE_LISTS_H
#define STREAMABLE_LISTS_H

#include <svt_str.h>
#include <svt_stlList.h>
#include <svt_bfstream.h>
#include <svt_streamableContainers.h>
#include <svt_iostream.h>
//#include <svt_fstream.h>
//#include <svt_types.h>


////////////////////////////////////////////////////////////////////
//                                                                //
//         ASCII Versions for stream operators                    //
//                                                                //
////////////////////////////////////////////////////////////////////


/** This template function overloads the <<-stream-operator to stream 
	out entire stl-lists.
*/
template <class T>
inline ostream& operator<<(ostream& s, const list<T>& oList)
{
  if (svt_streamableContainers::streamSize())
        s << oList.size() << endl;

  typename list<T>::const_iterator il;

  for (il=oList.begin(); il!=oList.end(); il++)
	putNext(s, *il);
  return s;
}


/** This template function overloads the >>-stream-operator to stream 
	in entire stl-lists, depending on stream-in-mode defined for
	svt_streamableContainers
*/
template <class T>
inline istream& operator>>(istream& s, list<T>& oList)
{  
  T item;
  if (!svt_streamableContainers::append()) oList.clear();
  if (svt_streamableContainers::streamSize()) {
    unsigned iSize;
	if (! (s >> iSize)) return s;

	for (unsigned i=0; (i<iSize) && s; i++)
	  if (getNext(s, item))
		oList.push_back(item);
	}
  else 
	while (getNext(s,item))
	  oList.push_back(item);
  
  // clear fail-bit if not eof to allow next stream call (???)
  if (!s.eof()) s.clear();  
  return s;
}       


////////////////////////////////////////////////////////////////////
//                                                                //
//         Binary Versions for stream operators                   //
//                                                                //
////////////////////////////////////////////////////////////////////

/** This template function is for binary writing of entire stl-lists.
	Flags in svt_streamableContainers control whether the list size is wirtten 
	out or not.
*/
template <class T>
inline svt_obfstream& operator<<(svt_obfstream& s, const list<T>& oList) {
  if (svt_streamableContainers::streamSize())
	s << (unsigned) oList.size();

  typename list<T>::const_iterator il;
  for (il=oList.begin(); il!=oList.end(); il++)
	s <<  *il;

  return s;
}


/** This template function is for binary reading of entire stl-lists.
	Flags in svt_streamableContainers control whether size is read first 
	or not.
*/
template <class T>
inline svt_ibfstream& operator>>(svt_ibfstream& s, list<T>& oList)
{  

  T item;
  if (!svt_streamableContainers::append()) oList.clear();
  if (svt_streamableContainers::streamSize()) {
	unsigned uSize;
	if (!(s>>uSize)) return s;
	for (unsigned i=0; (i<uSize) && s; i++)
	  if (s>>item) {
		oList.push_back(item);
	  }
	}
  else 
	while (s>>item) 
	  oList.push_back(item);


  // clear fail-bit if not eof to allow next stream call (???)
  if (!s.eof()) s.clear();  
  return s;
}       




#endif

