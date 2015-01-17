/********************************************************************
 *                                                                  *
 *  file: svt_streamableObject.h                                    *
 *                                                                  *
 *                                                                  *
 *  (pure inline template class)                                    *
 *                                                                  *
 *  f.delonge                                                       *
 *                                                                  *
 ********************************************************************/

/** \file
	Constains the specification of pure virtual base class svt_streamableObject.
	\author Frank Delonge
*/


#ifndef SVT_STREAMABLE_OBJECT_H
#define SVT_STREAMABLE_OBJECT_H
 
//
// system includes
//



//
// user includes
//
#include <svt_basics.h>
#include <svt_iostream.h>


//
// typedefs
//
 
//
// globals
//
 
//
//
// macros
//



class svt_streamableObject;

class svt_obfstream;
class svt_ibfstream;



DLLEXPORT ostream& operator<<(ostream&, const svt_streamableObject&);
DLLEXPORT istream& operator>>(istream&, svt_streamableObject&);

DLLEXPORT svt_obfstream& operator<<(svt_obfstream&, const svt_streamableObject&);
DLLEXPORT svt_ibfstream& operator>>(svt_ibfstream&, svt_streamableObject&);

// svt_ofstream& operator<<(svt_ofstream&, const svt_streamableObject&);
// svt_ifstream& operator>>(svt_ifstream&, svt_streamableObject&);


 

/** Abstract base class for objects to be streamed (pure inline).
	The reason for this base class is to be able to uniquely check
    streams before streaming in/out inherited instances.
	\author Frank Delonge
 */
class DLLEXPORT svt_streamableObject
{

protected:  
  bool m_bComplete;

  friend ostream& operator<<(ostream&, const svt_streamableObject&);
  friend istream& operator>>(istream&, svt_streamableObject&);

  friend svt_obfstream& operator<<(svt_obfstream&, const svt_streamableObject&);
  friend svt_ibfstream& operator>>(svt_ibfstream&, svt_streamableObject&);

  virtual ~svt_streamableObject(){};


  /** \name Streaming methods to be defined in inherited classes.
	  All you have to do to be able to stream your Objects is 
      to inheret from this class and to define the following two 
	  protected methods. The << and >> operators will be
	  overloaded automatically for ascii streams.
  */
  //@{
  virtual void streamOutA(ostream& s) const = 0;
  virtual void streamInA (istream& s) = 0;
  //@}


  /** \name Methods for binary i/o with streams.
	  Stream operators will not be overloaded, methods have to be 
	  called directly with binray stream as argument.
  */
  //@{
  virtual void streamOutB(svt_obfstream& s) const = 0;
  virtual void streamInB (svt_ibfstream& s) = 0;
  //@}
};




#endif
