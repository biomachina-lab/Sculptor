/***************************************************************************
                          svt_column_reader
                          -----------------
    begin                : 02/10/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_COLUMN_READER_H
#define __SVT_COLUMN_READER_H

#include <svt_basics.h>
#include <svt_types.h>
#include <svt_iostream.h>

#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////
// Column-based file reader
///////////////////////////////////////////////////////////////////////////////

#define MAXLINE 2048

/**
 * Column-based file format reader.
 */
class svt_column_reader
{
protected:

    char m_pLine[MAXLINE];
    FILE* m_pFile;

    char m_pString[MAXLINE];

public:

    /**
     * Constructor
     */
    svt_column_reader(const char* pFilename) :
        m_pFile(NULL)
    {
        m_pFile = fopen(pFilename,"r");
    };
    ~svt_column_reader()
    {
        if (m_pFile != NULL)
	    fclose( m_pFile );
    };

    /**
     * Read next line.
     */
    inline bool readLine()
    {
	if (m_pFile == NULL)
	    return false;

	// read the complete line. return if EOF.
	if (fgets(m_pLine, MAXLINE, m_pFile) == NULL)
	    return false;

        return true;
    };

    /**
     * Extract a string.
     * \param iStart first column (starts with 0!)
     * \param iEnd last column (this column is still read!)
     * \return string - please delete[] after use!
     */
    inline char* extractString(unsigned int iStart, unsigned int iEnd)
    {
        unsigned int i=0;
        unsigned int iLength = iEnd-iStart;

        if ( iEnd > strlen( m_pLine ) )
            iLength = strlen( m_pLine ) - iStart - 1;

        if (iStart >= strlen( m_pLine) || iLength <= 0)
        {
            m_pString[0] = 0;
            return m_pString;
        }

        for(i=0; i<=iLength; i++)
            m_pString[i] = m_pLine[i+iStart];

        m_pString[i] = 0;
        return m_pString;
    };

    /**
     * Extract a char.
     * \param iCol column where the char resides.
     * \return the char.
     */
    inline char extractChar( unsigned int iCol )
    {
        if (iCol < strlen( m_pLine ))
            return m_pLine[iCol];
        else
            return ' ';
    };

    /**
     * Extract a real32.
     * \param iStart first column (starts with 0!)
     * \param iEnd last column (this column is still read!)
     * \return Real32 value
     */
    inline Real32 extractReal32(unsigned int iStart, unsigned int iEnd)
    {
        Real32 fVal = 0.0f;

        char* pString = extractString(iStart, iEnd);
        fVal = atof( pString );

        return fVal;
    };

    /**
     * Extract an int.
     * \param iStart first column (starts with 0!)
     * \param iEnd last column (this column is still read!)
     * \return Int value
     */
    inline int extractInt(unsigned int iStart, unsigned int iEnd)
    {
        int iVal = 0;

        char* pString = extractString(iStart, iEnd);
        iVal = atoi( pString );

        return iVal;
    };

    /**
     * Get length of line
     */
    inline unsigned int getLength() const
    {
	return strlen( m_pLine );
    };

    /**
     * EOF test.
     */
    inline bool eof()
    {
        if (m_pFile != NULL)
	    return (bool)(feof(m_pFile));
	else
            return true;
    };

};

#endif
