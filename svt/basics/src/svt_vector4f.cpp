/***************************************************************************
                          svt_vector4f.cpp  -  description
                          ------------------------------
    begin                : Oct 12 2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_vector4f.h>
// clib includes
#include <svt_iostream.h>

/**
 * Constructor
 * \param fX initial x coordinate
 * \param fY initial y coordinate
 * \param fZ initial z coordinate
 */
svt_vector4f::svt_vector4f(Real32 fX, Real32 fY, Real32 fZ, Real32 fW)
{
    aVec[0] = fX;
    aVec[1] = fY;
    aVec[2] = fZ;
    aVec[3] = fW;
}

svt_vector4f::~svt_vector4f()
{
}

/**
 * post multiply another vector
 * \param rVector reference to another vector
 */
void svt_vector4f::postMult(svt_vector4f &rVector)
{
    aVec[0] = aVec[0] * rVector.getX();
    aVec[1] = aVec[1] * rVector.getY();
    aVec[2] = aVec[2] * rVector.getZ();
    aVec[3] = aVec[3] * rVector.getW();
}

/**
 * pre multiply another vector
 * \param rVector reference to another vector
 */
void svt_vector4f::preMult(svt_vector4f &rVector)
{
    aVec[0] = rVector.getX() * aVec[0];
    aVec[1] = rVector.getY() * aVec[1];
    aVec[2] = rVector.getZ() * aVec[2];
    aVec[3] = rVector.getW() * aVec[3];
}

/**
 * post multiply a matrix
 * \param rMatrix reference to svt_matrix
 */
void svt_vector4f::postMult(svt_matrix4f &rMatrix)
{
    Real32 fTempX = rMatrix(0,0)*aVec[0] + rMatrix(1,0)*aVec[1] + rMatrix(2,0)*aVec[2] + rMatrix(3,0)*aVec[3];
    Real32 fTempY = rMatrix(0,1)*aVec[0] + rMatrix(1,1)*aVec[1] + rMatrix(2,1)*aVec[2] + rMatrix(3,1)*aVec[3];
    Real32 fTempZ = rMatrix(0,2)*aVec[0] + rMatrix(1,2)*aVec[1] + rMatrix(2,2)*aVec[2] + rMatrix(3,2)*aVec[3];
    Real32 fTempW = rMatrix(0,3)*aVec[0] + rMatrix(1,3)*aVec[1] + rMatrix(2,3)*aVec[2] + rMatrix(3,3)*aVec[3];

    aVec[0] = fTempX;
    aVec[1] = fTempY;
    aVec[2] = fTempZ;
    aVec[3] = fTempW;
}

/**
 * pre multiply a matrix
 * \param rMatrix reference to svt_matrix
 */
void svt_vector4f::preMult(svt_matrix4f &rMatrix)
{
    Real32 fTempX = aVec[0]*rMatrix(0,0) + aVec[1]*rMatrix(0,1) + aVec[2]*rMatrix(0,2)+ aVec[3]*rMatrix(0,3);
    Real32 fTempY = aVec[0]*rMatrix(1,0) + aVec[1]*rMatrix(1,1) + aVec[2]*rMatrix(1,2)+ aVec[3]*rMatrix(1,3);
    Real32 fTempZ = aVec[0]*rMatrix(2,0) + aVec[1]*rMatrix(2,1) + aVec[2]*rMatrix(2,2)+ aVec[3]*rMatrix(2,3);
    Real32 fTempW = aVec[0]*rMatrix(3,0) + aVec[1]*rMatrix(3,1) + aVec[2]*rMatrix(3,2)+ aVec[3]*rMatrix(3,3);

    aVec[0] = fTempX;
    aVec[1] = fTempY;
    aVec[2] = fTempZ;
    aVec[3] = fTempW;
}

/**
 * post add a vector
 * \param rVector reference to svt_vector4f
 */
void svt_vector4f::postAdd(svt_vector4f &rVector)
{
    aVec[0] = aVec[0] + rVector.getX();
    aVec[1] = aVec[1] + rVector.getY();
    aVec[2] = aVec[2] + rVector.getZ();
    aVec[3] = aVec[3] + rVector.getW();
}

/**
 * pre add a vector
 * \param rVector reference to svt_vector4f
 */
void svt_vector4f::preAdd(svt_vector4f &rVector)
{
    aVec[0] = rVector.getX() + aVec[0];
    aVec[1] = rVector.getY() + aVec[1];
    aVec[2] = rVector.getZ() + aVec[2];
    aVec[3] = rVector.getW() + aVec[3];
}

/**
 * post sub a vector
 * \param rVector reference to svt_vector4f
 */
void svt_vector4f::postSub(svt_vector4f &rVector)
{
    aVec[0] = aVec[0] - rVector.getX();
    aVec[1] = aVec[1] - rVector.getY();
    aVec[2] = aVec[2] - rVector.getZ();
    aVec[3] = aVec[3] - rVector.getW();
}

/**
 * pre sub a vector
 * \param rVector reference to svt_vector4f
 */
void svt_vector4f::preSub(svt_vector4f &rVector)
{
    aVec[0] = rVector.getX() - aVec[0];
    aVec[1] = rVector.getY() - aVec[1];
    aVec[2] = rVector.getZ() - aVec[2];
    aVec[3] = rVector.getW() - aVec[3];
}

/**
 * post cross product another vector
 * \param rVector reference to svt_vector4f
 */
void svt_vector4f::postCross(svt_vector4f &rVector)
{
    Real32 fTempX = (aVec[1] * rVector.getZ()) - (aVec[2] * rVector.getY());
    Real32 fTempY = (aVec[2] * rVector.getX()) - (aVec[0] * rVector.getZ());
    Real32 fTempZ = (aVec[0] * rVector.getY()) - (aVec[1] * rVector.getX());

    aVec[0] = fTempX;
    aVec[1] = fTempY;
    aVec[2] = fTempZ;
}
/**
 * pre cross product another vector
 * \param rVector reference to svt_vector4f
 */
void svt_vector4f::preCross(svt_vector4f &rVector)
{
    Real32 fTempX = (rVector.getZ() * aVec[1]) - (rVector.getY() * aVec[2]);
    Real32 fTempY = (rVector.getX() * aVec[2]) - (rVector.getZ() * aVec[0]);
    Real32 fTempZ = (rVector.getY() * aVec[0]) - (rVector.getX() * aVec[1]);

    aVec[0] = fTempX;
    aVec[1] = fTempY;
    aVec[2] = fTempZ;
}

/**
 * multiply vector with scalar
 * \param fScalar the scalar to multiply with
 */
void svt_vector4f::multScalar(Real32 fScalar)
{
    aVec[0] *= fScalar;
    aVec[1] *= fScalar;
    aVec[2] *= fScalar;
    aVec[3] *= fScalar;
}

/**
 * get the x coord of the vector
 * \return x coord
 */
Real32 svt_vector4f::getX() const
{
    return aVec[0];
}

/**
 * set the x coord of the vector
 * \param fX the new x coord
 */
void svt_vector4f::setX(Real32 fX)
{
    aVec[0] = fX;
}

/**
 * get the y coord of the vector
 * \return y coord
 */
Real32 svt_vector4f::getY() const
{
    return aVec[1];
}

/**
 * set the y coord of the vector
 * \param fY the new y coord
 */
void svt_vector4f::setY(Real32 fY)
{
    aVec[1] = fY;
}

/**
 * get the z coord of the vector
 * \return z coord
 */
Real32 svt_vector4f::getZ() const
{
    return aVec[2];
}

/**
 * set the z coord of the vector
 * \param fZ the new z coord
 */
void svt_vector4f::setZ(Real32 fZ)
{
    aVec[2] = fZ;
}

/**
 * get the w coord of the vector
 * \return w coord
 */
Real32 svt_vector4f::getW() const
{
    return aVec[3];
}

/**
 * set the w coord of the vector
 * \param fW the new w coord
 */
void svt_vector4f::setW(Real32 fW)
{
    aVec[3] = fW;
}

/**
 * get the x coord of the vector
 * \return x coord
 */
Real32 svt_vector4f::x() const
{
    return aVec[0];
}

/**
 * set the x coord of the vector
 * \param fX the new x coord
 */
void svt_vector4f::x(Real32 fX)
{
    aVec[0] = fX;
}

/**
 * get the y coord of the vector
 * \return y coord
 */
Real32 svt_vector4f::y() const
{
    return aVec[1];
}

/**
 * set the y coord of the vector
 * \param fY the new y coord
 */
void svt_vector4f::y(Real32 fY)
{
    aVec[1] = fY;
}

/**
 * get the z coord of the vector
 * \return z coord
 */
Real32 svt_vector4f::z() const
{
    return aVec[2];
}

/**
 * set the z coord of the vector
 * \param fZ the new z coord
 */
void svt_vector4f::z(Real32 fZ)
{
    aVec[2] = fZ;
}

/**
 * get the w coord of the vector
 * \return w coord
 */
Real32 svt_vector4f::w() const
{
    return aVec[3];
}

/**
 * set the w coord of the vector
 * \param fW the new w coord
 */
void svt_vector4f::w(Real32 fW)
{
    aVec[3] = fW;
}

/**
 * set all three coords of the vector at once
 */
void svt_vector4f::setPos(Real32 fX, Real32 fY, Real32 fZ)
{
    aVec[0] = fX;
    aVec[1] = fY;
    aVec[2] = fZ;
}

/**
 * set from a Vector4f
 * \param aNewVec new Vector4f to set the svt_vector4f to
 */
void svt_vector4f::set(Vector4f& aNewVec)
{
    aVec[0] = aNewVec[0];
    aVec[1] = aNewVec[1];
    aVec[2] = aNewVec[2];
    aVec[3] = aNewVec[3];
}

/**
 * values are set to the absolute values
 */
void svt_vector4f::abs()
{
    aVec[0] = (Real32)fabs(aVec[0]);
    aVec[1] = (Real32)fabs(aVec[1]);
    aVec[2] = (Real32)fabs(aVec[2]);
    aVec[3] = (Real32)fabs(aVec[3]);
}
/**
 * get the length of the vector
 * \return length
 */
Real32 svt_vector4f::length()
{
    return (Real32)sqrt(aVec[0]*aVec[0] + aVec[1]*aVec[1] + aVec[2]*aVec[2]);
}
/**
 * calculate the distance between two vectors
 * \param oVec the other vector
 * \return distance
 */
Real32 svt_vector4f::distance(svt_vector4f oVec)
{
    svt_vector4f oTempVec = oVec;
    oTempVec.postSub(*this);
    return oTempVec.length();
}
/**
 * calculate the squared distance between two vectors
 * \param oVec the other vector
 * \return distance
 */
Real32 svt_vector4f::distanceSq(svt_vector4f oVec)
{
    svt_vector4f oTempVec = oVec;
    oTempVec.postSub(*this);
    return oTempVec.x()*oTempVec.x() + oTempVec.y()*oTempVec.y() + oTempVec.z()*oTempVec.z();
}

/**
 * normalize the vector
 */
void svt_vector4f::normalize()
{
    Real32 fLength = length();

    if (fLength != 0.0f)
    {
        fLength = 1 / fLength;

        x( x() * fLength);
        y( y() * fLength);
        z( z() * fLength);
    }
}

/**
 * point product operator
 */
Real32 svt_vector4f::pointProduct(svt_vector4f oVecB)
{
    return ((x()*oVecB.x()) + (y()*oVecB.y()) + (z()*oVecB.z()));
}

/**
 * dump the content of the vector to cout
 */
void svt_vector4f::dump()
{
    cout << "svt_vector4f: (" << aVec[0] << "," << aVec[1] << "," << aVec[2] << "," << aVec[3] << ")" << endl;
}
