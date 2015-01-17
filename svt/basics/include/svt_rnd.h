/***************************************************************************
                          svt_rnd
                          -------
    begin                : 12/15/2004
    author               : Makoto Matsumoto, Takuji Nishim
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_RND_H
#define SVT_RND_H

#include <svt_basics.h>

/**
 * seed for the random number generator
 * \param iSeed seed value
 */
void DLLEXPORT svt_sgenrand (unsigned long);

/**
 * generate a random number
 * \return the new random number
 */
double DLLEXPORT svt_genrand ();

#endif
