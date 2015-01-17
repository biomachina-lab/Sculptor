/***************************************************************************
                          svt_swap.h
			  ----------
    begin                : 11/28/2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_SWAP_H
#define SVT_SWAP_H

#include <svt_types.h>

#define svt_swap(a, b) (temp)=*(a);\
                   *(a)=*(b);\
                   *(b)=(temp);

#define SwapInt(a)  SwapFloat((float *)(a))
#define SwapLong(a) SwapFloat((float *)(a))

/**
 * are the CPU registers in Big-Endian-Format?
 * \return true if big endian cpu
 */
bool DLLEXPORT svt_bigEndianMachine(void);
/**
 * swap a double value
 * \param pValue pointer to the double variable
 */
void DLLEXPORT svt_swapDouble(double *pValue);
/**
 * swap a Real32 value
 * \param pValue pointer to the Real32 variable
 */
void DLLEXPORT svt_swapReal32(Real32 *pValue);
/**
 * swap a Int16 value
 * \param pValue pointer to the Int16 variable
 */
void DLLEXPORT svt_swapInt16(Int16 *value);
/**
 * swap a Int32 value
 * \param pValue pointer to the Int32 variable
 */
void DLLEXPORT svt_swapInt32(Int32 *value);

#endif
