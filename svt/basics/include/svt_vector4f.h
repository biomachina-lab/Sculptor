/***************************************************************************
                          svt_vector4f.h  -  description
                          ------------------------------
    begin                : Oct 12 2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_VECTOR_H
#define SVT_VECTOR_H

#include <svt_basics.h>
#include <svt_types.h>
#include <math.h>
#include <svt_matrix4f.h>

/** A four float vector
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_vector4f
{
protected:
    Vector4f aVec;

public:
    /**
     * Constructor
     * \param fX initial x coordinate
     * \param fY initial y coordinate
     * \param fZ initial z coordinate
     * \param fW initial w coordinate
     */
    svt_vector4f(Real32 fX =0.0f, Real32 fY =0.0f, Real32 fZ =0.0f, Real32 fW =1.0f);
    ~svt_vector4f();

    /**
     * post multiply another vector
     * \param rVector reference to another vector
     */
    void postMult(svt_vector4f &rVector);
    /**
     * pre multiply another vector
     * \param rVector reference to another vector
     */
    void preMult(svt_vector4f &rVector);

    /**
     * post multiply a matrix
     * \param rMatrix reference to svt_matrix
     */
    void postMult(svt_matrix4f &rMatrix);
    /**
     * pre multiply a matrix
     * \param rMatrix reference to svt_matrix
     */
    void preMult(svt_matrix4f &rMatrix);

    /**
     * multiply vector with scalar
     * \param fScalar the scalar to multiply with
     */
    void multScalar(Real32 fScalar);

    /**
     * post add a vector
     * \param rVector reference to svt_vector4f
     */
    void postAdd(svt_vector4f &rVector);
    /**
     * pre add a vector
     * \param rVector reference to svt_vector4f
     */
    void preAdd(svt_vector4f &rVector);
    /**
     * post sub a vector
     * \param rVector reference to svt_vector4f
     */
    void postSub(svt_vector4f &rVector);
    /**
     * pre sub a vector
     * \param rVector reference to svt_vector4f
     */
    void preSub(svt_vector4f &rVector);

    /**
     * post cross product another vector
     * \param rVector reference to svt_vector4f
     */
    void postCross(svt_vector4f &rVector);
    /**
     * pre cross product another vector
     * \param rVector reference to svt_vector4f
     */
    void preCross(svt_vector4f &rVector);

    /**
     * get the x coord of the vector
     * \return x coord
     */
    Real32 getX() const;
    /**
     * set the x coord of the vector
     * \param fX the new x coord
     */
    void setX(Real32 fX);

    /**
     * get the y coord of the vector
     * \return y coord
     */
    Real32 getY() const;
    /**
     * set the y coord of the vector
     * \param fY the new y coord
     */
    void setY(Real32 fY);

    /**
     * get the z coord of the vector
     * \return z coord
     */
    Real32 getZ() const;
    /**
     * set the z coord of the vector
     * \param fZ the new z coord
     */
    void setZ(Real32 fZ);

    /**
     * get the w coord of the vector
     * \return w coord
     */
    Real32 getW() const;
    /**
     * set the w coord of the vector
     * \param fW the new w coord
     */
    void setW(Real32 fW);

    /**
     * get the x coord of the vector
     * \return x coord
     */
    Real32 x() const;
    /**
     * set the x coord of the vector
     * \param fX the new x coord
     */
    void x(Real32 fX);

    /**
     * get the y coord of the vector
     * \return y coord
     */
    Real32 y() const;
    /**
     * set the y coord of the vector
     * \param fY the new y coord
     */
    void y(Real32 fY);

    /**
     * get the z coord of the vector
     * \return z coord
     */
    Real32 z() const;
    /**
     * set the z coord of the vector
     * \param fZ the new z coord
     */
    void z(Real32 fZ);

    /**
     * get the w coord of the vector
     * \return w coord
     */
    Real32 w() const;
    /**
     * set the w coord of the vector
     * \param fW the new w coord
     */
    void w(Real32 fW);

    /**
     * set all three coords of the vector at once
     * \param fX x coord
     * \param fY y coord
     * \param fZ z coord
     */
    void setPos(Real32 fX, Real32 fY, Real32 fZ);

    /**
     * set from a Vector4f
     * \param aNewVec new Vector4f to set the svt_vector4f to
     */
    void set(Vector4f& aNewVec);

    /**
     * values are set to the absolute values
     */
    void abs();

    /**
     * get the length of the vector
     * \return length
     */
    Real32 length();
    /**
     * calculate the distance between two vectors
     * \param oVec the other vector
     * \return distance
     */
    Real32 distance(svt_vector4f oVec);
    /**
     * calculate the squared distance between two vectors
     * \param oVec the other vector
     * \return distance
     */
    Real32 distanceSq(svt_vector4f oVec);

    /**
     * normalize the vector
     */
    void normalize();

    /**
     * point product
     */
    Real32 pointProduct(svt_vector4f oVecB);

    /**
     * dump the content of the vector to cout
     */
    void dump();
};

#endif
