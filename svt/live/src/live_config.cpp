/***************************************************************************
                          live_config.cpp  -  description
                             -------------------
    begin                : 19.04.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_iostream.h>

///////////////////////////////////////////////////////////////////////////////
// includes
///////////////////////////////////////////////////////////////////////////////

#include <live_config.h>

#include <stdlib.h>
#include <string.h>

#ifdef _WINDOWS
#include <direct.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// public member functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 * Create a new configuration object. You should create the object with the configuration filename
 * \param pName name of the configuration file
 */
live_config::live_config(char* pname){
    eintraege = 0;
    eof_flag = 0;
    if (pname == NULL)
        parse(".liverc");
    else
        parse(pname);
}

/**
 * look for parameter and return its value. If there is no parameter specified, return the default value.
 * \param pParam pointer to parameter name
 * \param bDefault default value
 * \return the value of the parameter
 */
bool live_config::getValue(char *pParam, bool bDefault)
{
    int suche = findeEintrag(pParam);

    if(suche != 0)
    {
        if (compare_string(parameter[suche].wert, "TRUE") ||
            compare_string(parameter[suche].wert, "True")||
            compare_string(parameter[suche].wert, "true"))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return bDefault;
    }
}

/**
 * look for parameter and return its value. If there is no parameter specified, return the default value.
 * \param pParam pointer to parameter name
 * \param iDefault default value
 * \return the value of the parameter
 */
int live_config::getValue(char *pParam, int iDefault)
{
    int suche = findeEintrag(pParam);

    if(suche !=0)
        return atoi(parameter[suche].wert);
    else
        return iDefault;
}

/**
 * look for parameter and return its value. If there is no parameter specified, return the default value.
 * \param pParam pointer to parameter name
 * \param fDefault default value
 * \return the value of the parameter
 */
float live_config::getValue(char *pParam, float fDefault)
{
    int suche = findeEintrag(pParam);

    if(suche != 0)
        return atof(parameter[suche].wert);
    else
        return fDefault;
}

/**
 * look for parameter and return its value. If there is no parameter specified, return the default value.
 * \param pParam pointer to parameter name
 * \param pDefault pointer to default string
 * \return the value of the parameter
 */
char *live_config::getValue(char *pParam, char *pDefault)
{
    int suche = findeEintrag(pParam);

    if(suche != 0)
        return parameter[suche].wert;

    else
        return pDefault;
}

///////////////////////////////////////////////////////////////////////////////
// protected member functions
///////////////////////////////////////////////////////////////////////////////

/**
 * parse the configuration file and store the informations in the internal parameter table.
 * \param pFilename pointer to filename string
 */
void live_config::parse(char* pFilename)
{
    FILE *File=NULL;
    File=fopen(pFilename,"r");
    if (File)
    {
        while (eof_flag != 1)
        {
            eintraege++;
            memcpy(parameter[eintraege].name, getWord(File), 64);
            getWord(File);						// Åberspringe =-Zeichen
            memcpy(parameter[eintraege].wert, getLine(File), 64);
        }
        fclose(File);
    }
}

/**
 * read a single word, until SPACE, EOL or EOF occures
 * \param pFile pointer to FILE
 * \return pointer to the word
 */
char *live_config::getWord(FILE *pFile)
{
    int counter = 0;
    int value;

    // Zuerst mal alle Leerzeichen und Returns ueberspringen
    while ( ((value = getc(pFile)) != EOF) && ((value == 32) || (value == 10))){ }
    buff[counter] = char(value);
    counter++;

    // Jetzt wirklich Zeile einlesen
    if (value != 35)
        while ( ((value = getc(pFile)) != EOF) && (value != 10) && (value != 32) && (value != 35))
        {
            buff[counter] = char(value);
            counter++;
        }

    // Just a comment?
    if  (value == 35)
    {
        // then next line...
        nextLine(pFile);
        return getWord(pFile);
    }
    // End of file?
    if (value == EOF ) eof_flag = 1;
    buff[counter] = '\0';		// End Of String setzen!
    return buff;
}

/**
 * read the rest of the line, until EOL or EOF
 * \param pFile pointer to FILE
 * \return the rest of the line
 */
char *live_config::getLine(FILE *pFile)
{
    int counter = 0;
    int value;

    // Zuerst mal alle Leerzeichen und Returns ueberspringen
    while ( ((value = getc(pFile)) != EOF) && ((value == 32) || (value == 10))){ }
    buff[counter] = char(value);
    counter++;

    // Jetzt wirklich Zeile einlesen
    while ( ((value = getc(pFile)) != EOF) && (value != 10) && (value != 35))
    {
        buff[counter] = char(value);
        counter++;
    }

    // End of file?
    if (value == EOF ) eof_flag = 1;
    buff[counter] = '\0';		// End Of String setzen!
    return buff;
}

/**
 * go to the next line
 * \param pFile pointer to FILE
 */
void live_config::nextLine(FILE *pFile)
{
    int value;
    while ( ((value = getc(pFile)) != EOF) && (value != 10) ) { /** do nothing! */ }
    if (value == EOF ) eof_flag = 1;
}

/**
 * look for a parameter with a certain name and search for it in the internal tables. If the function can find it, it will return the index of this parameter in the table.
 * \param pName pointer to the name of the parameter
 * \return index of the parameter in the internal table or 0 if it cannot find the param.
 */
int live_config::findeEintrag(char *pName)
{
    for (int i=1; i <= eintraege; i++)
        if (compare_string(pName, parameter[i].name))
            return i;

    // couldnt find the parameter
    return 0;
}

/**
 * test if to strings are equal.
 * \param pStr1 pointer to the first string
 * \param pStr2 pointer to the second string
 * \return true if the strings are equal
 */
bool live_config::compare_string(char *pStr1, char* pStr2)
{
    if (strlen(pStr1) != strlen(pStr2) )
        return false;
    else
        for (unsigned int i=0;i<strlen(pStr1);i++)
            if (pStr1[i] != pStr2[i]) return 0;

    return 1;
}
