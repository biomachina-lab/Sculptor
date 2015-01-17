/***************************************************************************
                          svt_multivec
                          ------------
    begin                : 10/22/2004
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_MULTIVEC
#define __SVT_MULTIVEC

#include <svt_types.h>
#include <math.h>
#include <svt_matrix.h>
#include <svt_stlVector.h>

/**
 * vector class for multiple dimensions (not only 2 or 3)
 */
template<class T> class svt_multivec : public svt_matrix<T>
{
public:

    /**
     * Vector class. The vector can have any dimension, not only 3!
     * \param iDim number of dimensions for the new vector
     */
    svt_multivec(int iDim) : svt_matrix<T>(iDim,1)
    {
	for (int i=0; i<iDim; i++)
	    (*this)[i] = (T)(0.0);
    };

    /**
     * get x coordinate (index 0)
     * \return coordinate
     */
    inline T x() const
    {
        return (*this)[0];
    };
    /**
     * set x coordinate (index 0)
     * \param fX new value for x
     */
    inline void x(T fX)
    {
        (*this)[0] = fX;
    };
    /**
     * get y coordinate (index 1)
     * \return coordinate
     */
    inline T y() const
    {
        return (*this)[1];
    };
    /**
     * set y coordinate (index 1)
     * \param fY new value for y
     */
    inline void y(T fY)
    {
        (*this)[1] = fY;
    };

    /**
     * distance to another vector (better distance between two points, not vectors)
     * \param oVec the other vector
     * \return distance to second vector
     */
    virtual T distance(svt_multivec<T>& oVec)
    {
	return (T)(sqrt((double)distanceSq( oVec)));
    };

    /**
     * squared distance to another vector (better distance between two points, not vectors)
     * \param oVec the other vector
     * \return squared distance to second vector
     */
    T distanceSq(svt_multivec<T>& oVec)
    {
        T fResult = 0.0;

	if (svt_matrix<T>::height() != oVec.height())
	    return fResult;

	for(unsigned int i=0; i<svt_matrix<T>::height(); i++)
            fResult += (oVec[i] - (*this)[i]) * (oVec[i] - (*this)[i]);

	return fResult;
    };

    /**
     * square brackets
     * \param iRow the row
     */
    inline T& operator[](unsigned int iRow) const
    {
        //return (*(svt_matrix<T>*)(this))[iRow][0];
        return (T&)(svt_matrix<T>::at(iRow,0));
    };

    /**
     * get the length of the vector
     * \return length
     */
    inline T length() const
    {
        T fResult = 0.0;

	for(unsigned int i=0; i<svt_matrix<T>::height(); i++)
	    fResult += (*this)[i] * (*this)[i];

        return fResult;
    };
};

/**
 * add two vectors
 */
template<class T> 
inline svt_multivec<T> operator+(const svt_multivec<T>& rVecA, const svt_multivec<T>& rVecB)
{
    if (rVecA.height() != rVecB.height())
	return rVecA;

    svt_multivec<T> oRes( rVecA.height() );

    for(unsigned int i=0; i<rVecA.height(); i++)
	oRes[i] = rVecA[i] + rVecB[i];

    return oRes;
};

/**
 * subtract two vectors
 */
template<class T> 
inline svt_multivec<T> operator-(const svt_multivec<T>& rVecA, const svt_multivec<T>& rVecB)
{
    if (rVecA.height() != rVecB.height())
	return rVecA;

    svt_multivec<T> oRes( rVecA.height() );

    for(unsigned int i=0; i<rVecA.height(); i++)
	oRes[i] = rVecA[i] - rVecB[i];

    return oRes;
};

/**
 * dot product
 */
template<class T> 
inline svt_multivec<T> operator*(const svt_multivec<T>& rVecA, const svt_multivec<T>& rVecB)
{
    if (rVecA.height() != rVecB.height())
	return rVecA;

    svt_multivec<T> oRes( rVecA.height() );

    for(unsigned int i=0; i<rVecA.height(); i++)
	oRes[i] = rVecA[i] * rVecB[i];

    return oRes;
};

/**
 * multiply with scalar
 */
template<class T> 
inline svt_multivec<T> operator*(const svt_multivec<T>& rVecA, const T& fScalar)
{
    svt_multivec<T> oRes( rVecA.height() );

    for(unsigned int i=0; i<rVecA.height(); i++)
	oRes[i] = fScalar * rVecA[i];

    return oRes;
};

/**
 * multiply with scalar
 */
template<class T> 
inline svt_multivec<T> operator*(const T& fScalar, const svt_multivec<T>& rVecA)
{
    svt_multivec<T> oRes( rVecA.height() );

    for(unsigned int i=0; i<rVecA.height(); i++)
	oRes[i] = fScalar * rVecA[i];

    return oRes;
};

/**
 * divide by scalar
 */
template<class T> 
inline svt_multivec<T> operator/(const svt_multivec<T>& rVecA, const T& fScalar)
{
    svt_multivec<T> oRes( rVecA.height() );

    for(unsigned int i=0; i<rVecA.height(); i++)
	oRes[i] = rVecA[i] / fScalar;

    return oRes;
};

/**
 * divide by scalar
 */
template<class T> 
inline svt_multivec<T> operator/(const T& fScalar, const svt_multivec<T>& rVecA)
{
    svt_multivec<T> oRes( rVecA.height() );

    for(unsigned int i=0; i<rVecA.height(); i++)
	oRes[i] = rVecA[i] / fScalar;

    return oRes;
};

#endif
