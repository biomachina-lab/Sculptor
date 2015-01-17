/***************************************************************************
                          svt_config.cpp
			  --------------
    begin                : Tue June 13 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_iostream.h>
#include <svt_config.h>

#include <stdlib.h>
#include <string.h>

#ifdef TRUE_WIN32
#include <direct.h>
#define strcasecmp stricmp
#endif

#ifdef WIN32
  #ifndef STATICQT
  extern "C" DLLEXPORT BOOL WINAPI DllMain( HANDLE hModule, DWORD dwFunction, LPVOID lpNot )
  {
    svt_registerCout( &std::cout );
    return true;
  };
  #endif
#endif

/**
 * Constructor
 */
svt_config::svt_config(const char* pFname) :
    m_iItems( 0 )
{
    for(unsigned int i=0; i<MAXPARAM; i++)
    {
	m_aParameter[i].name[0] = 0;
	m_aParameter[i].wert[0] = 0;
    }


    if (pFname == NULL)
	parse(".svtrc");
    else
	parse(pFname);

}
svt_config::~svt_config()
{
}

/**
 * parse the configuration file and store all key value pairs
 * \param pFilename pointer to filename
 */
void svt_config::parse(const char* pFilename)
{
    FILE *pFile = NULL;
    char pTmp[256];
    char* pNotUsed;
    pFile = fopen(pFilename,"r");

    if (pFile)
    {
        while (!feof(pFile) && !ferror(pFile))
        {
	    m_iItems++;
	    pNotUsed = fgets(pTmp, 256, pFile);
	    sscanf( pTmp, "%60s = %[^\n]", m_aParameter[m_iItems].name, m_aParameter[m_iItems].wert);
	    // strip trailing spaces
	    for(int i=strlen(m_aParameter[m_iItems].wert)-1;i>0;i--)
		if (m_aParameter[m_iItems].wert[i] == ' ')
		    m_aParameter[m_iItems].wert[i] = 0;
		else
		    i=0;

            // comment?
	    if (strchr(pTmp, '#') != 0)
		m_iItems--;
            //else
	    //    cout << "name: " << m_aParameter[m_iItems].name << " wert: " << m_aParameter[m_iItems].wert << endl;
        }
        fclose(pFile);
    }
    //else
    //    cout << ".svtrc not found! Parameters set to default values!" << endl;
}

// teste ob NAME als Parameter gespeichert ist, wenn ja gib Feldnummer zurck, sonst 0
int svt_config::findItem(const char *name)
{
    if (name == NULL)
        return 0;

    for (int i=1; i <= m_iItems; i++)
        if (strcasecmp(name,m_aParameter[i].name) == 0) //case
            return i;

    // kein entsprechender Eintrag gefunden
    return 0;
}

// suche Parameter und gib seinen Wert zurck, falls nicht vorhanden DEFAULT_VALUE
bool svt_config::getValue(const char *pParam, bool bDefault)
{
    int iItem = findItem(pParam);

    if(iItem != 0)
    {
	if (strcasecmp(m_aParameter[iItem].wert, "TRUE") == 0)
	    return true;
	else
            return false;
    } else
        return bDefault;
}

int svt_config::getValue(const char *pParam, int default_value)
{
    int iItem = findItem(pParam);

    if(iItem !=0)
	return atoi(m_aParameter[iItem].wert);
    else
	return default_value;
}

float svt_config::getValue(const char *pParam, float default_value)
{
    int iItem = findItem(pParam);

    if(iItem != 0)
	return atof(m_aParameter[iItem].wert);
    else
	return default_value;
}

double svt_config::getValue(const char *pParam, double default_value)
{
    int iItem = findItem(pParam);

    if(iItem != 0)
	return atof(m_aParameter[iItem].wert);
    else
	return default_value;
}

const char *svt_config::getValue(const char *pParam, const char *default_value)
{
    int iItem = findItem(pParam);

    if(iItem != 0)
	return m_aParameter[iItem].wert;
    else
	return default_value;
}


void svt_config::setValue(const char *pParam, bool bNew)
{
    int iItem = findItem(pParam);

    if(iItem == 0)
    {
        m_iItems++;
        strcpy( m_aParameter[ m_iItems ].name, pParam );
        iItem = m_iItems;
    }

    if (bNew)
        sprintf( m_aParameter[iItem].wert, "TRUE" );
    else
        sprintf( m_aParameter[iItem].wert, "FALSE" );
}

void svt_config::setValue(const char *pParam, int iValue)
{
    int iItem = findItem(pParam);

    if(iItem == 0)
    {
        m_iItems++;
        strcpy( m_aParameter[ m_iItems ].name, pParam );
        iItem = m_iItems;
    }

    sprintf( m_aParameter[iItem].wert, "%i", iValue );
}

void svt_config::setValue(const char *pParam, float fValue)
{
    int iItem = findItem(pParam);

    if(iItem == 0)
    {
        m_iItems++;
        strcpy( m_aParameter[ m_iItems ].name, pParam );
        iItem = m_iItems;
    }

    sprintf( m_aParameter[iItem].wert, "%f", fValue );
}

void svt_config::setValue(const char *pParam, double fValue)
{
    int iItem = findItem(pParam);

    if(iItem == 0)
    {
        m_iItems++;
        strcpy( m_aParameter[ m_iItems ].name, pParam );
        iItem = m_iItems;
    }

    sprintf( m_aParameter[iItem].wert, "%f", fValue );
}

void svt_config::setValue(const char *pParam, const char *pValue)
{
    int iItem = findItem(pParam);

    if(iItem == 0)
    {
        m_iItems++;
        strcpy( m_aParameter[ m_iItems ].name, pParam );
        iItem = m_iItems;
    }

    sprintf( m_aParameter[iItem].wert, "%s", pValue );
}
