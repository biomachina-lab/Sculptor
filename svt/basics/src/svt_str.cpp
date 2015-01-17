#include <svt_str.h>

#include <svt_string.h>
#include <string.h>
#include <svt_iostream.h>
#include <svt_bfstream.h>

svt_str_manip::Mode svt_str_manip::sm_eMode = single;

void svt_str_manip::streamSingle() {sm_eMode=single;}
void svt_str_manip::streamLine() {sm_eMode=line;}
svt_str_manip::Mode svt_str_manip::streamMode() {return sm_eMode;}






/////////////////////////////////////////////////////////////////////////
//                         Inline Definitions                          //
/////////////////////////////////////////////////////////////////////////

svt_str::svt_str(const char* str)
{ 
  m_str = svt_strdup(str); 
}


svt_str::svt_str(const svt_str& that)
  : svt_streamableObject()
{ 
  m_str = svt_strdup(that.m_str); 
}


svt_str::~svt_str()
{ 
  if (m_str)
	{
	delete [] m_str;
	m_str = NULL;
	}

}


svt_str& svt_str::operator=(const char* str)
{
  if (str==m_str) return *this;
  if (m_str)
	{
	delete [] m_str;
	m_str = NULL;
	}
  m_str = svt_strdup(str); 
  return *this;
}


svt_str& svt_str::operator=(const svt_str& that)
{
  *this=that.m_str;
  return *this;
}



void svt_str::streamOutA(ostream& s) const
{
  s << m_str;
}

void svt_str::streamOutB(svt_obfstream& s) const
{
  s << m_str;
}

void svt_str::streamInA (istream& s)  
{
  char str[1024];
  if (svt_str_manip::streamMode() == svt_str_manip::single)
	s >> str;
  else
	{
	str[0]='\0';
	do 
	  {
	  s.getline(str, 1023);
	  if (str[strlen(str)-1] == '\r' )
		str[strlen(str)-1] = '\0';
	  } while (strlen(str) == 0 && !s.fail() && !s.eof() );
	}

  if (!s.fail() && strlen(str) > 0)
	*this=str;

}



void svt_str::streamInB (svt_ibfstream& s)  
{
  char str[1024];

  if (s >> str) *this=str;

}






