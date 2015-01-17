#include <svt_streamableObject.h>
#include <svt_iostream.h>
#include <svt_fstream.h>
#include <svt_exception.h>
 
#include <svt_bfstream.h>

ostream& operator<<(ostream& s, const svt_streamableObject& outObject)
{
  outObject.streamOutA(s);
  return s;
}
 
istream& operator>>(istream& s, svt_streamableObject& inObject)
{
  if (s.eof())
    throw (svt_exception("Read error: Already at EOF!"));

  inObject.m_bComplete=true;
  inObject.streamInA(s);

  if ( (s.fail() && !s.eof()) || !inObject.m_bComplete )
    throw (svt_exception("Read error: Unable to read object!"));

  return s;
}                         



svt_obfstream& operator<<(svt_obfstream& s, const svt_streamableObject& outObject)
{
  outObject.streamOutB(s);
  return s;
}

svt_ibfstream& operator>>(svt_ibfstream& s, svt_streamableObject& inObject)
{
  if (s.eof())
    throw (svt_exception("Read error: Already at EOF!"));

  inObject.m_bComplete=true;
  inObject.streamInB(s);

  if ( (s.fail() && !s.eof()) || !inObject.m_bComplete )
	throw (svt_exception("Read error: Unable to read object!"));

  return s;
}    
