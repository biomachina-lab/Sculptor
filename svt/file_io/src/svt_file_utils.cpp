/***************************************************************************
                          svt_file_utils.cpp
			  -------------------
    begin                : Jul 23 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_basics.h>
#include <svt_file_utils.h>
#include <svt_swap.h>
#include <string.h>

#define FTL cout << svt_trimName("svt_file_utils")

/**
 * read from file num characters in buffer
 * \param pFile pointer to FILE
 * \param iNum number of bytes in the buffer
 * \param pBuffer pointer to buffer
 */
DLLEXPORT void svt_readBuffer(FILE* pFile, int iNum, char* pBuffer)
{
    for(int i=0;i<iNum;i++)
        pBuffer[i] = fgetc(pFile);
};

/**
 * read Int8 from file
 * \param pFile pointer to FILE
 */
Int8 DLLEXPORT svt_readInt8(FILE* pFile)
{
    return fgetc(pFile);
};

/**
 * read Int16 from file
 * \param pFile pointer to FILE
 */
Int16 DLLEXPORT svt_readInt16(FILE* pFile)
{
    Int16 i = 0;
    size_t t;

    t = fread(&i,sizeof(char),2,pFile);

    if(svt_bigEndianMachine())
        svt_swapInt16(&i);

    return i;
};

/**
 * write Int16 to file
 * \param pFile pointer to FILE
 * \param i Int16 which should be written to disk
 * \return true if write was successful
 */
bool DLLEXPORT svt_writeInt16(FILE* pFile, Int16 i)
{
    //if(svt_bigEndianMachine())
    //  svt_swapInt16(&i);

    if (fwrite(&i, 1, 2, pFile)==2)
        return true;
    else
        return false;
};

/**
 * read Int32 from file
 * \param pFile pointer to FILE
 */
Int32 DLLEXPORT svt_readInt32(FILE* pFile)
{
    Int32 i = 0;

    size_t t;
    t = fread(&i,sizeof(char),4,pFile);

    if(svt_bigEndianMachine())
        svt_swapInt32(&i);

    return i;
};

/**
 * read Real32 from file
 * \param pFile pointer to FILE
 */
Real32 DLLEXPORT svt_readReal32(FILE* pFile)
{
    Real32 i = 0.0f;

    size_t t;
    t = fread(&i,sizeof(char),4,pFile);

    if(svt_bigEndianMachine())
        svt_swapReal32(&i);

    return i;
};

/**
 * read Real64 from file
 * \param pFile pointer to FILE
 */
Real32 DLLEXPORT svt_readReal64(FILE* pFile)
{
    Real64 i = 0.0f;

    size_t t;
    t = fread(&i,sizeof(char),8,pFile);

    if(svt_bigEndianMachine())
        svt_swapDouble(&i);

    return i;
};

/**
 * strip the extension of a filename
 * \param pSource pointer to original filename
 * \param pDestination pointer to new string
 */
void DLLEXPORT svt_stripExtension(char* pSource, char* pDestination)
{
    int iEnd = strlen( pSource );

    int i;
    for(i=iEnd; i>=0; i--)
        if (pSource[i] == '.')
            break;

    iEnd = i;

    if (iEnd == 0)
        iEnd = strlen(pSource);

    for(i=0; i<iEnd; i++)
        pDestination[i] = pSource[i];

    pDestination[iEnd] = 0;
}

/**
 * gets the extension of a filename = letters following the last dot
 * \param pSource pointer to original filename
 * \param pDestination pointer to new string
 */
void DLLEXPORT svt_getExtension(char* pSource, char* pDestination)
{
    char pTmp[1256];
    
    unsigned int iEnd = strlen(pSource);
    int iIndex = iEnd-1;
    while (iIndex>0)
    {
        if (pSource[iIndex]=='.')// the dot was found exit here
            iIndex = 0;
        else
            if (pSource[iIndex]!=' ')
            {
                strcpy(pTmp, pDestination);
                sprintf(pDestination, "%c%s", pSource[iIndex], pTmp);
            }
        iIndex--;
    }
}

/**
 * strip the path information of a filename
 * \param pSource pointer to original filename
 * \param pDestination pointer to new filename
 */
void DLLEXPORT svt_stripPath(const char* pSource, char* pDestination)
{
    unsigned int iStart = 0;
    unsigned int iEnd = strlen( pSource );

    unsigned int i;
    for(i=0; i<iEnd; i++)
        if (pSource[i] == '/' || pSource[i] == '\\')
            iStart = i+1;

    if (iStart >= strlen(pSource) || iStart >= iEnd)
        iStart = 0;

    for(i=iStart; i<iEnd; i++)
        pDestination[i-iStart] = pSource[i];

    pDestination[i-iStart] = 0;
}

/**
 * This will interpret an unsigned short (16bit) as a 'half', meaning a 16bit floating point value and return the 32bit version.
 * \param y the 16bit float
 * \return 32bit float value
 */
Real32 DLLEXPORT svt_halfToFloat(unsigned short y)
{
    int s = (y >> 15) & 0x00000001;
    int e = (y >> 10) & 0x0000001f;
    int m =  y        & 0x000003ff;

    if (e == 0)
    {
	if (m == 0)
	{
	    //
	    // Plus or minus zero
	    //

	    return s << 31;
	}
	else
	{
	    //
	    // Denormalized number -- renormalize it
	    //

	    while (!(m & 0x00000400))
	    {
		m <<= 1;
		e -=  1;
	    }

	    e += 1;
	    m &= ~0x00000400;
	}
    }
    else if (e == 31)
    {
	if (m == 0)
	{
	    //
	    // Positive or negative infinity
	    //

	    return (s << 31) | 0x7f800000;
	}
	else
	{
	    //
	    // Nan -- preserve sign and significand bits
	    //

	    return (s << 31) | 0x7f800000 | (m << 13);
	}
    }

    //
    // Normalized number
    //

    e = e + (127 - 15);
    m = m << 13;

    //
    // Assemble s, e and m.
    //

    unsigned int i = (s << 31) | (e << 23) | m;

    float f;

    memcpy( &f, &i, sizeof(float) );

    return f;
}

/**
 * Read-in a series of files and write them into a single archive
 * \param oOutFile stl string with the filename of the archive
 * \param oInFiles stl vector of stl strings with the files to be stored in the archive
 */
void svt_createArchive( string oOutFile, vector< string > oInFiles )
{
    // create output file header
    FILE* pInFile;
    FILE* pOutFile;
    char pBuffer[4096];
 
    FTL << "Creating archive: " << oOutFile << endl;
    pOutFile = fopen( oOutFile.c_str(), "wb");
    sprintf(pBuffer,"SPF%07i", (unsigned int)(oInFiles.size()));
    // write header
    fwrite( pBuffer, 1, 11, pOutFile );

    // now load all the individual files
    size_t iRead;
    for(unsigned int i=0; i<oInFiles.size(); i++)
    {
        FTL << "  Reading file " << i+1 << " - " << oInFiles[i] << endl;

        pInFile = fopen(oInFiles[i].c_str(), "rb");

        if (pInFile)
        {
            // determine the filename
            char oFilename[4096];
            svt_stripPath( oInFiles[i].c_str(), oFilename );

            // Determine total size of file
            long   iSize = 0;
            fseek (pInFile, 0, SEEK_END);
            iSize = ftell(pInFile);
            rewind(pInFile);

            // write header for this particular file
            sprintf(pBuffer,"FIL%07i", i+1);
            fwrite( pBuffer, 1, 11, pOutFile );
            sprintf(pBuffer,"%s ", oFilename);
            fwrite( pBuffer, 1, strlen(oFilename)+2, pOutFile );
            sprintf(pBuffer,"SIZE%li", iSize);
            fwrite( pBuffer, 1, strlen(pBuffer)+1, pOutFile );

            while(!feof(pInFile))
            {
                iRead = fread( pBuffer, 1, 4096, pInFile );
                fwrite( pBuffer, 1, iRead, pOutFile );
            }
        }

        fclose( pInFile );
    }

    fclose( pOutFile );

    FTL << "Done." << endl;
};

/**
 * Extract an archive to a directory
 * \param oArFile stl string with the filename of the archive
 * \param oDir stl string with the name of the directory where to put it (has to have a directory separator at the end!)
 */
void svt_extractArchive( string oArFile, string oDir )
{
    FILE* pArFile;
    char pBuffer[4096];
    char cDummy;

    unsigned int iFiles;

    // read in the header
    pArFile = fopen( oArFile.c_str(), "rb");
    FTL << "Extracting archive from file: " << oArFile << endl;
    // read header
    fread( pBuffer, 1, 11, pArFile );
    // read number of files
    sscanf( pBuffer, "SPF%d", &iFiles );
    FTL << "  Have to read " << iFiles << endl;

    // now load all the individual files
    size_t iRead;
    int iSize;

    for(unsigned int i=0; i< iFiles; i++)
    {
        // header
        fread( pBuffer, 1, 11, pArFile );
        // filename
        fscanf( pArFile, "%s", pBuffer );
        FTL << "  Reading file " << i+1 << " - " << pBuffer << endl;

        string oFilename( oDir );
        oFilename = oFilename + string( pBuffer );
        FILE* pOutFile = fopen( oFilename.c_str(), "wb" );

        // size
        cDummy = fgetc( pArFile );
        cDummy = fgetc( pArFile );
        fscanf( pArFile, "SIZE%d", &iSize );
        cDummy = fgetc( pArFile );

        // read and write the data
        while(!feof(pArFile) && iSize > 0)
        {
            if (iSize > 4096)
                iRead = fread( pBuffer, 1, 4096, pArFile );
            else
                iRead = fread( pBuffer, 1, iSize, pArFile );

            fwrite( pBuffer, 1, iRead, pOutFile );

            iSize -= iRead;
        }

        fclose( pOutFile );
    }

    fclose( pArFile );

    FTL << "Done." << endl;
};
