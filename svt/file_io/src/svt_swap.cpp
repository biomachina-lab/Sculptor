/***************************************************************************
                          svt_swap.h
			  ----------
    begin                : 11/28/2001
    author               : Stefan Birmanns, Frank Kehren
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_swap.h>

short int swaptest=0x0102;

/**
 * are the CPU registers in Big-Endian-Format?
 * \return true if big endian cpu
 */
bool DLLEXPORT svt_bigEndianMachine(void)
{
    char *p;
 
    p=(char *)&swaptest;                            /* Zeiger auf Wert 0x0102 */
    return (*p == 0x01);              /* erstes Byte 0x01 => Big-Endian-Format */
}

/**
 * swap a double value
 * \param pValue pointer to the double variable
 */
void DLLEXPORT svt_swapDouble(double *pValue)
{
    char *cp, temp;

    cp=(char *)pValue;
    svt_swap(cp, cp+7);
    cp++;
    svt_swap(cp, cp+5);
    cp++;
    svt_swap(cp, cp+3);
    cp++;
    svt_swap(cp, cp+1);
}

/**
 * swap a Real32 value
 * \param pValue pointer to the Real32 variable
 */
void DLLEXPORT svt_swapReal32(Real32 *pValue)
{
    char *cp, temp;

    cp=(char *)pValue;
    svt_swap(cp, cp+3);
    cp++;
    svt_swap(cp, cp+1);
}

/**
 * swap a Int16 value
 * \param pValue pointer to the Int16 variable
 */
void DLLEXPORT svt_swapInt16(Int16 *pValue)
{
    char *cp, temp;

    cp=(char *)pValue;
    svt_swap(cp, cp+1);
}

/**
 * swap a Int32 value
 * \param pValue pointer to the Int32 variable
 */
void DLLEXPORT svt_swapInt32(Int32 *pValue)
{
    char *cp, temp;

    cp=(char *)pValue;
    svt_swap(cp, cp+3);
    cp++;
    svt_swap(cp, cp+1);
}
