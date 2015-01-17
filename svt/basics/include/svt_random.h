/********************************************************************
 *                                                                  *
 *  file: svt_random.h                                              *
 *                                                                  *
 *  h-file f�r diverse zufallszahlen-funktionen                     *
 *                                                                  *
 *  f.delonge                                                       *
 *                                                                  *
 ********************************************************************/

/** \file
	Contains the random generator used in SPLUS6 and IBM's RANDO as well as
	random number functions of several distribultions on top of it.
	Alternatively, these random number functions can also be used with
	the native rand() function.
	\author Frank Delonge
*/

#ifndef RANDOM_H
#define RANDOM_H


#include <svt_basics.h>
#include <svt_types.h>

/** enums of supported random number generators.
 */
typedef enum { SPLUS, RANDU, NATIVE } svt_randomGenerator;

void svt_setRandomGenerator( svt_randomGenerator );
DLLEXPORT svt_randomGenerator svt_getRandomGenerator();

DLLEXPORT void svt_ranSeed(unsigned con = 0, unsigned taus = 0);
DLLEXPORT void svt_ranSeedAll(unsigned con = 0, unsigned taus = 0);

DLLEXPORT unsigned svt_ranLargest();

/// Gleichverteilung auf [0, svt_ranLargest()]
DLLEXPORT unsigned svt_rand();                        

/// Gleichverteilung auf [0,1]
DLLEXPORT Real64 svt_ranUni();                        

/// Normalverteilung zu (0,1) nach Box-Muller-Algorithmus
DLLEXPORT Real64 svt_ranNormal();

/// Normalverteilung zu (mu, sigma^2)
DLLEXPORT Real64 svt_ranNormal(Real64 mu, Real64 sigma);

/// Cauchyverteilung
/**
 * generate a random number following a Cauchy Distribution ~ resembles with a normal distribution just wider at the tail
 * the ratio of a two normal distributed variables follow a Cauchy distribution of average 0 and sd 1
 * \return a random number following the Cauchy distribution of average 0 and standard deviation 1
 */
DLLEXPORT Real64 svt_ranCauchy();

/**
 * \return a random number following the Cauchy distribution of average mu and standard deviation sigma
 */
DLLEXPORT Real64 svt_ranCauchy(Real64 mu, Real64 sigma);

/// Weibull-Verteilung
DLLEXPORT Real64 svt_ranWeibull();

/// Hjorth-Verteilung
DLLEXPORT Real64 svt_ranHjorth();

/// Exponential-Verteilung
DLLEXPORT Real64 svt_ranExp();

/// Bernoulli-Verteilung
DLLEXPORT unsigned svt_ranBernoulli();

/// Binomial-Verteilung
DLLEXPORT unsigned svt_ranBinomial();

/// Poisson-Verteilung
DLLEXPORT unsigned svt_ranPoisson();






#endif
