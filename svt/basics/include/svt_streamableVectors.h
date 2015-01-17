/** \file
	This file contains tempalte stream functions for streaming stl-vectors
	with which whole stl-vectors can be streamed in/out at once.
                                                                     
	\author Frank Delonge
*/

#ifndef STREAMABLE_VECTORS_H
#define STREAMABLE_VECTORS_H

#include <svt_stlVector.h>
#include <svt_iostream.h>
#include <svt_fstream.h>
#include <svt_streamableContainers.h>


/** This template function overloads the <<-stream-operator to stream 
	out entire stl-vectors.
*/
template <class T>
inline ostream& operator<<(ostream& s, const vector<T>& vec)
{
  if (svt_streamableContainers::streamSize())
	s << vec.size() << endl;
  for (unsigned i=0; i<vec.size(); i++)
	putNext(s, vec[i]);
  return s;
}
 

/** This template function overloads the >>-stream-operator to stream 
	in entire stl-vectors, depending on stream-in-mode defined for
	svt_streamableContainers
*/
template <class T>
inline istream& operator>>(istream& s, vector<T>& vec)
{  

  unsigned i;
 
  unsigned iOldSize = svt_streamableContainers::append() ? vec.size() :0;
 


  if (svt_streamableContainers::streamSize())
	{
	unsigned iSize;
	s >> iSize;
	
	if (s.fail() || s.eof())
	  return s;

	vec.resize(iOldSize+iSize);

	for (i=iOldSize; i<iOldSize+iSize; i++)
	  {
	  if (s.eof()) break;
	  if (!getNext(s, vec[i]))
		break;

	  //	  if (s.fail()) 
	  //		break;

	  }
	
	if ((s.eof() || s.fail()) && i <iOldSize+iSize)
	  {
	  vec.resize(i);
	  //	  if (s.fail()) s.clear();
	  }
	}
  else
	{
	bool bFinished = false;
	int iBufSize=200;
	do 
	  {

	  vec.resize(iOldSize+iBufSize);
	  for (i=iOldSize; i<iOldSize+iBufSize; i++)
		{
		if (s.eof()) {bFinished=true;break;}
		if (!getNext(s, vec[i]))
		  {
		  bFinished=true;
		  break;
		  }
// 		if (s.fail()) 
// 		  {
// 		  bFinished=true;
// 		  break;
// 		  }
		}	  
	  iOldSize=vec.size();

	  } while (!bFinished);
	  if (s.fail()) s.clear();
	  vec.resize(i);
	}


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
// template <class T>
// inline svt_obfstream& operator<<(svt_obfstream& s, const vector<T>& oVec) {
//   if (svt_streamableContainers::streamSize())
// 	s << (unsigned) oVec.size();

//   for (unsigned i=0;i<oVec.size();i++)
// 	s <<  oVec[i];

//   return s;
// }

template <class T>
inline void finallyStreamOutVector(svt_obfstream& s, const vector<T>& oVec) {
  for (unsigned i=0;i<oVec.size();i++)
	s <<  oVec[i];
}

// // specailization
// #define DEFINE_FINALLY_STREAM_OUT_VECTOR(type)            
// template <>                                               
// inline void finallyStreamOutVector <type>                 
// (svt_obfstream& s, const vector<type>& oVec) {            
//   if (s.swapping())                                       
//     for (unsigned i=0;i<oVec.size();i++)                  
// 	  s <<  oVec[i];                                      
//  else                                                     
//     s.write((char*)&oVec[0], oVec.size()*sizeof(type) );  
// }


// specailization by simple overloading
#define DEFINE_FINALLY_STREAM_OUT_VECTOR(type)           \
inline void finallyStreamOutVector                       \
(svt_obfstream& s, const vector<type>& oVec) {           \
  if (s.swapping())                                      \
    for (unsigned i=0;i<oVec.size();i++)                 \
	  s <<  oVec[i];                                     \
 else                                                    \
    s.write((char*)(&(oVec[0])), oVec.size()*sizeof(type) ); \
}


//DEFINE_FINALLY_STREAM_OUT_VECTOR(bool)
DEFINE_FINALLY_STREAM_OUT_VECTOR(char)
DEFINE_FINALLY_STREAM_OUT_VECTOR(unsigned char)
DEFINE_FINALLY_STREAM_OUT_VECTOR(signed char)
DEFINE_FINALLY_STREAM_OUT_VECTOR(short)
DEFINE_FINALLY_STREAM_OUT_VECTOR(unsigned short)
DEFINE_FINALLY_STREAM_OUT_VECTOR(int)
DEFINE_FINALLY_STREAM_OUT_VECTOR(unsigned int)
DEFINE_FINALLY_STREAM_OUT_VECTOR(long)
DEFINE_FINALLY_STREAM_OUT_VECTOR(unsigned long)
DEFINE_FINALLY_STREAM_OUT_VECTOR(float)
DEFINE_FINALLY_STREAM_OUT_VECTOR(double)


template <class T>
inline svt_obfstream& operator<<(svt_obfstream& s, const vector<T>& oVec) {
  if (svt_streamableContainers::streamSize())
	s << (unsigned) oVec.size();

  finallyStreamOutVector(s, oVec);
  return s;
}




/** This template function is for binary reading of entire stl-lists.
	Flags in svt_streamableContainers control whether size is read first 
	or not.
*/

template <class T>
inline svt_ibfstream& svt_readVector
(svt_ibfstream& s, vector<T>& oVec, unsigned iStart, unsigned iSize) {
  for (unsigned i=0;i<iSize;i++)
	if ( ! (s >>  oVec[iStart+i]) ) break;
  return s;
} 

#define DEFINE_READ_VECTOR(type)                                                \
  template <>                                                                   \
  inline svt_ibfstream& svt_readVector<type>                                    \
      (svt_ibfstream& s, vector<type>& oVec, unsigned iStart, unsigned iSize) {  \
    if (s.swapping()) {                                                          \
      for (unsigned i=0;i<iSize;i++)                                            \
	    if( ! (s >> oVec[iStart+i])) break;}                                    \
    else                                                                         \
    s.read((char*)(&oVec[iStart]), iSize*sizeof(type));                         \
    return s;                                                                   \
  }


//DEFINE_READ_VECTOR(bool)
DEFINE_READ_VECTOR(char)
DEFINE_READ_VECTOR(unsigned char)
DEFINE_READ_VECTOR(signed char)
DEFINE_READ_VECTOR(short)
DEFINE_READ_VECTOR(unsigned short)
DEFINE_READ_VECTOR(int)
DEFINE_READ_VECTOR(unsigned int)
DEFINE_READ_VECTOR(long)
DEFINE_READ_VECTOR(unsigned long)
DEFINE_READ_VECTOR(float)
DEFINE_READ_VECTOR(double)


template <class T>
inline svt_ibfstream& operator>>(svt_ibfstream& s, vector<T>& oVec)
{  
  unsigned i;
  unsigned iOldSize = svt_streamableContainers::append() ? oVec.size() :0;

  if (svt_streamableContainers::streamSize()) {
    unsigned iSize;
	if (!(s>>iSize))
	  return s;

	oVec.resize(iOldSize+iSize);
	svt_readVector(s, oVec, iOldSize, iSize);
  }
  else {
    bool bFinished = false;
	int iBufSize=200;
	do {

	  oVec.resize(iOldSize+iBufSize);
	  for (i=iOldSize; i<iOldSize+iBufSize; i++)
		if (!(s>>oVec[i])) {bFinished=true;break;}
      iOldSize=oVec.size();
    } while (!bFinished);
    oVec.resize(i);
  }

  return s;
}






#endif

