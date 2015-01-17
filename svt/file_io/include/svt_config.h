/***************************************************************************
                          svt_config.h  -  description
                             -------------------
    begin                : Tue June 13 2000
    author               : Jan Deiterding
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_CONFIG_H
#define SVT_CONFIG_H

#include <svt_basics.h>
#include <stdio.h>

#define MAXPARAM 100

/**
 * This class stores the application and library configuration.
 * The basic idea is that the class stores a list of keywords and values. Each keyword can only have just one value.
 * The application just ask with the functions getValue for one keyword. If no value is stored for this keyword, the class
 * returns the default value (provided by the application itself).

 * @author Stefan Birmanns
 */
class DLLEXPORT svt_config
{
protected:

    struct parameter
    {
	char name[64];
	char wert[256];
    } m_aParameter[MAXPARAM];

    int m_iItems;

    void parse(const char* pFilename);
    int findItem(const char *pName);

public:
    /**
     * Constructor
     */
    svt_config(const char* pFname = NULL);
    ~svt_config(void);

    /**
     * get boolean value
     */
    bool  getValue(const char *para, bool  default_value);
    /**
     * get int value
     */
    int   getValue(const char *para, int   default_value);
    /**
     * get float value
     */
    float getValue(const char *para, float default_value);
    /**
     * get double value
     */
    double getValue(const char *para, double default_value);
    /**
     * get a string
     */
    const char* getValue(const char *para, const char* default_value);

    /**
     * set boolean value
     */
    void setValue(const char *para, bool new_value);
    /**
     * set int value
     */
    void setValue(const char *para, int new_value);
    /**
     * set float value
     */
    void setValue(const char *para, float new_value);
    /**
     * set double value
     */
    void setValue(const char *para, double new_value);
    /**
     * set a string
     */
    void setValue(const char *para, const char* new_value);
};

#endif
