/********************************************************************
 *                                                                  *
 *  file: svt_random.c                                              *
 *                                                                  *
 *  Implementierunf diverser zufallszahlen-funktionen               *
 *                                                                  *
 *  f.delonge                                                       *
 *                                                                  *
 ********************************************************************/

// 
// system includes
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>


//
// user includes
//
#include <svt_random.h>

//
// some static stuff
//
static svt_randomGenerator s_eRandomGenerator = SPLUS;

static int s_bNativeInitialized = 0;
static int s_bRanduInialized = 0;
static int s_bSPlusInitialized = 0;

static unsigned s_uLastCon_SPLUS = 1803752341u;
static unsigned s_uLastTaus_SPLUS = 3697165728u;
static unsigned s_uLast_RANDU = 1803752341u;


//
// constants
//
const unsigned c_uLargest_SPLUS = 0xffffffff;
const unsigned c_uLargest_RANDU = 0x8fffffff;
const Real64   c_dPi =3.141592654;

//
// internal prototypes
//
static unsigned svt_rand_SPLUS();
static unsigned svt_rand_RANDU();
static unsigned svt_rand_NATIVE();

static void svt_ranSeed_SPLUS(unsigned=0, unsigned=0);
static void svt_ranSeed_RANDU(unsigned=0);
static void svt_ranSeed_NATIVE(unsigned=0);


//
// Implementation
//
void svt_setRandomGenerator( svt_randomGenerator e )
{
  s_eRandomGenerator = e;
}


svt_randomGenerator svt_getRandomGenerator()
{
  return s_eRandomGenerator;

}

void svt_ranSeed(unsigned con, unsigned taus)
{
  switch (svt_getRandomGenerator())
	{
	case(SPLUS): svt_ranSeed_SPLUS(con, taus); break;
	case(RANDU): svt_ranSeed_RANDU(con); break;
	default:     svt_ranSeed_NATIVE(con); break;
	} 
}

void svt_ranSeedAll(unsigned con, unsigned taus)
{
  svt_ranSeed_SPLUS(con, taus);
  svt_ranSeed_RANDU(con);
  svt_ranSeed_NATIVE(con);
}


unsigned svt_ranLargest()
{
  switch (svt_getRandomGenerator())
	{
	case(SPLUS): return c_uLargest_SPLUS;
	case(RANDU): return c_uLargest_RANDU;
	default:     return RAND_MAX;
	} 
}



unsigned svt_rand_SPLUS()
{

  const unsigned lambda=69069;

  if ( !s_bSPlusInitialized )
	svt_ranSeed_SPLUS();

  s_uLastCon_SPLUS = s_uLastCon_SPLUS*lambda;
  s_uLastTaus_SPLUS ^= s_uLastTaus_SPLUS >> 15;
  s_uLastTaus_SPLUS ^= s_uLastTaus_SPLUS << 17;
  
  return s_uLastTaus_SPLUS ^ s_uLastCon_SPLUS;

}


unsigned svt_rand_RANDU()
{
  if ( !s_bRanduInialized )
	svt_ranSeed_RANDU();

  unsigned v = ((1<<16)+3) * s_uLast_RANDU;
  v = v & 0x8fffffffu;
  s_uLast_RANDU = v;

  return s_uLast_RANDU;

}

unsigned svt_rand_NATIVE()
{

  if ( !s_bNativeInitialized )
	{
	svt_ranSeed_NATIVE();
	}
  return rand();

}

unsigned svt_rand()
{
  switch (svt_getRandomGenerator())
	{
	case(SPLUS): return svt_rand_SPLUS();
	case(RANDU): return svt_rand_RANDU();
	default:     return svt_rand_NATIVE();
	} 

}


Real64 svt_ranUni()
{
  return svt_rand()/Real64(svt_ranLargest());
}



Real64 svt_ranNormal()
{

  Real64 r1,r2;
  static int s_iReserve=0;
  static Real64 s_dReserveValue=0;

  if ( s_iReserve )
	{
	s_iReserve=0;
	return s_dReserveValue;
	}

  r1=sqrt(-2*log(svt_ranUni()));
  r2=svt_ranUni();

  s_iReserve=1;
  s_dReserveValue=r1*sin(2*c_dPi*r2);
  
  return r1*cos(2*c_dPi*r2);

}

Real64 svt_ranNormal(Real64 mu, Real64 sigma)
{
  return mu + svt_ranNormal() * sigma;

}

/**
 * generate a random number following a Cauchy Distribution ~ resembles with a normal distribution just wider at the tail
 * the ratio of a two normal distributed variables follow a Cauchy distribution of average 0 and sd 1
 * \return a random number following the Cauchy distribution of average 0 and standard deviation 1
 */
Real64 svt_ranCauchy()
{
  Real64 fN1 = svt_ranNormal();
  Real64 fN2 = svt_ranNormal();
  
  Real64 fRatio = fN1/fN2;
  
  return  fRatio;
}

/**
 * \return a random number following the Cauchy distribution of average mu and standard deviation sigma
 */
Real64 svt_ranCauchy(Real64 mu, Real64 sigma)
{
  return mu + svt_ranCauchy() * sigma;
}

Real64 svt_ranWeibull()
{
  printf("svt_ranWeibull: Not implemented yet!\n");
  return 0;
}


Real64 svt_ranHjorth()
{
  printf("svt_ranHjorth: Not implemented yet!\n");
  return 0;
}



Real64 svt_ranExp()
{
  printf("svt_ranExp: Not implemented yet!\n");
  return 0;
}



unsigned svt_ranBernoulli()
{
  printf("svt_ranBernoulli: Not implemented yet!\n");
  return 0;
}



unsigned svt_ranBinomial()
{
  printf("svt_ranBinomial: Not implemented yet!\n");
  return 0;
}



unsigned svt_ranPoisson()
{
  printf("svt_ranPoisson: Not implemented yet!\n");
  return 0;
}



static void svt_ranSeed_SPLUS(unsigned con, unsigned taus)
{
  
  if ( con==0)
	con=time(NULL);

  taus = con*2;

  if ( !(con%2) )
	con++;

  s_uLastCon_SPLUS = con;
  s_uLastTaus_SPLUS = taus;
  //printf("Initialized SPLUS with %d, %d\n", con, taus);

  s_bSPlusInitialized = 1;
}


static void svt_ranSeed_RANDU(unsigned start)
{
  if (start==0)
	start = time(NULL);

  s_uLast_RANDU = start;

  //printf("Initialized RANDU with %d\n", start);
  s_bRanduInialized = 1;
}


static void svt_ranSeed_NATIVE(unsigned start)
{
  if ( start==0)
	start = time(NULL);

  srand(start);
  
  //printf("Initialized NATIVE with %d\n", start);

  s_bNativeInitialized = 1;
}
