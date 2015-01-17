/***************************************************************************
                          live_config.h  -  description
                             -------------------
    begin                : 19.04.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef LIVE_CONFIG_H
#define LIVE_CONFIG_H

#include <live_win.h>
#include <stdio.h>

/**
 * This class stores the device driver configuration.
 * The basic idea is that the class stores a list of keywords and values. Each keyword can only have just one value.
 * The application just ask with the functions getValue for one keyword. If no value is stored for this keyword, the class
 * returns the default value (provided by the application itself).
 *
 * @author Stefan Birmanns, Jan Deiterding
 */
class DLLEXPORT live_config
{
protected:
    int eof_flag;
    char buffer[64];
    char buff[256];
    struct para{char name[64];	char wert[256];} parameter[100];
    int eintraege;

    /**
     * parse the configuration file and store the informations in the internal parameter table.
     * \param pFilename pointer to filename string
     */
    void parse(char* pFilename);
    /**
     * read a single word, until SPACE, EOL or EOF occures
     * \param pFile pointer to FILE
     * \return pointer to the word
     */
    char *getWord(FILE *pFile);
    /**
     * read the rest of the line, until EOL or EOF
     * \param pFile pointer to FILE
     * \return the rest of the line
     */
    char *getLine(FILE *pFile);
    /**
     * go to the next line
     * \param pFile pointer to FILE
     */
    void nextLine(FILE *pFile);
    /**
     * look for a parameter with a certain name and search for it in the internal tables. If the function can find it, it will return the index of this parameter in the table.
     * \param pName pointer to the name of the parameter
     * \return index of the parameter in the internal table or 0 if it cannot find the param.
     */
    int findeEintrag(char *pName);
    /**
     * test if to strings are equal.
     * \param pStr1 pointer to the first string
     * \param pStr2 pointer to the second string
     * \return true if the strings are equal
     */
    bool compare_string(char *pStr1, char* pStr2);

public:
    /**
     * Constructor.
     * Create a new configuration object. You should create the object with the configuration filename
     * \param pName name of the configuration file
     */
    live_config(char* pname = NULL);
    /**
     * Destructor
     */
    virtual ~live_config(void) {};

    /**
     * look for a boolean type parameter and return its value. If there is no parameter specified, return the default value.
     * \param pParam pointer to parameter name
     * \param bDefault default value
     * \return the value of the parameter
     */
    bool getValue(char *para, bool  default_value);
    /**
     * look for an integer type parameter and return its value. If there is no parameter specified, return the default value.
     * \param pParam pointer to parameter name
     * \param iDefault default value
     * \return the value of the parameter
     */
    int getValue(char *para, int   default_value);
    /**
     * look for a float type parameter and return its value. If there is no parameter specified, return the default value.
     * \param pParam pointer to parameter name
     * \param fDefault default value
     * \return the value of the parameter
     */
    float getValue(char *para, float default_value);
    /**
     * look for string type parameter and return its value. If there is no parameter specified, return the default value.
     * \param pParam pointer to parameter name
     * \param pDefault pointer to default string
     * \return the value of the parameter
     */
    char* getValue(char *para, char* default_value);
};

#endif
