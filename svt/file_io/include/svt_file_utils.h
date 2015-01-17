/***************************************************************************
                          svt_file_utils.h
			  -------------------
    begin                : Jul 23 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_FILE_UTILS_H
#define SVT_FILE_UTILS_H

#include <svt_basics.h>
#include <svt_types.h>
#include <svt_stlVector.h>

/**
 * read from file num characters in buffer
 * \param pFile pointer to FILE
 * \param iNum number of bytes in the buffer
 * \param pBuffer pointer to buffer
 */
void DLLEXPORT svt_readBuffer(FILE* pFile, int iNum, char* pBuffer);
/**
 * read Int8 from file
 * \param pFile pointer to FILE
 */
Int8 DLLEXPORT svt_readInt8(FILE* pFile);
/**
 * read Int16 from file
 * \param pFile pointer to FILE
 */
Int16 DLLEXPORT svt_readInt16(FILE* pFile);
/**
 * write Int16 to file
 * \param pFile pointer to FILE
 * \param i Int16 which should be written to disk
 * \return true if write was successful
 */
bool DLLEXPORT svt_writeInt16(FILE* pFile, Int16 i);
/**
 * read Int32 from file
 * \param pFile pointer to FILE
 */
Int32 DLLEXPORT svt_readInt32(FILE* pFile);
/**
 * read Real32 from file
 * \param pFile pointer to FILE
 */
Real32 DLLEXPORT svt_readReal32(FILE* pFile);
/**
 * read Real64 from file
 * \param pFile pointer to FILE
 */
Real32 DLLEXPORT svt_readReal64(FILE* pFile);

/**
 * strip the extension of a filename
 * \param pSource pointer to original filename
 * \param pDestination pointer to new string
 */
void DLLEXPORT svt_stripExtension(char* pSource, char* pDestination);

/**
 * gets the extension of a filename = letters following the last dot
 * \param pSource pointer to original filename
 * \param pDestination pointer to new string
 */
void DLLEXPORT svt_getExtension(char* pSource, char* pDestination);


/**
 * strip the path information of a filename
 * \param pSource pointer to original filename
 * \param pDestination pointer to new filename
 */
void DLLEXPORT svt_stripPath(const char* pSource, char* pDestination);

/**
 * This will interpret an unsigned short (16bit) as a 'half', meaning a 16bit floating point value and return the 32bit version.
 * \param y the 16bit float
 * \return 32bit float value
 */
Real32 DLLEXPORT svt_halfToFloat(unsigned short y);

/**
 * Read-in a series of files and write them into a single archive
 * \param oOutFile stl string with the filename of the archive
 * \param oInFiles stl vector of stl strings with the files to be stored in the archive
 */
void svt_createArchive( string oOutFile, vector< string > oInFiles );
/**
 * Extract an archive to a directory
 * \param oArFile stl string with the filename of the archive
 * \param oDir stl string with the name of the directory where to put it (has to have a directory separator at the end!). Directory needs to exist already.
 */
void svt_extractArchive( string oArFile, string oDir );

#endif
