/***************************************************************************
                          svt_vertinterl
			  --------------
    begin                : 11/21/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_VERT_INTERL_H
#define SVT_VERT_INTERL_H

#include <svt_basics.h>
#include <svt_vector4.h>
#include <math.h>

/**
 * Very simple point class for interleaved OpenGL VARs.
 * Single vertex, four floats.
 */
class DLLEXPORT svt_v4f
{
    Real32 m_oV4[4];

public:

    svt_v4f( Real32 fX = 0.0f, Real32 fY = 0.0f, Real32 fZ = 0.0f, Real32 fW = 1.0f )
    {
	m_oV4[0] = fX;
	m_oV4[1] = fY;
	m_oV4[2] = fZ;
	m_oV4[3] = fW;
    };

    svt_v4f( const svt_v4f& rOther )
    {
        m_oV4[0] = rOther.m_oV4[0];
        m_oV4[1] = rOther.m_oV4[1];
        m_oV4[2] = rOther.m_oV4[2];
	m_oV4[3] = rOther.m_oV4[3];
    };

    inline Real32& operator[](unsigned i)
    {
	return m_oV4[i];
    };
};

/**
 * Very simple point class for interleaved OpenGL VARs.
 * Single vertex, three floats.
 */
class DLLEXPORT svt_v3f
{
    Real32 m_oV3[3];

public:

    svt_v3f( Real32 fX = 0.0f, Real32 fY = 0.0f, Real32 fZ = 0.0f )
    {
	m_oV3[0] = fX;
	m_oV3[1] = fY;
	m_oV3[2] = fZ;
    };

    svt_v3f( const svt_v3f& rOther )
    {
        memcpy( m_oV3, rOther.m_oV3, sizeof(Real32) * 3 );
    };

    Real32& operator[](unsigned i)
    {
	return m_oV3[i];
    };

    const Real32& operator[](unsigned i) const
    {
	return m_oV3[i];
    };

    inline svt_v3f operator+(const svt_v3f& rOther)
    {
        svt_v3f oTmp;

	oTmp.m_oV3[0] = m_oV3[0] + rOther.m_oV3[0];
	oTmp.m_oV3[1] = m_oV3[1] + rOther.m_oV3[1];
	oTmp.m_oV3[2] = m_oV3[2] + rOther.m_oV3[2];

	return oTmp;
    };

    inline svt_v3f operator*(const Real32& fFact)
    {
	svt_v3f oTmp;

	oTmp.m_oV3[0] = fFact * m_oV3[0];
	oTmp.m_oV3[1] = fFact * m_oV3[1];
	oTmp.m_oV3[2] = fFact * m_oV3[2];

	return oTmp;
    };
};

/**
 * Very simple point class for interleaved OpenGL VARs.
 * Normal and vertex, both three floats.
 */
class DLLEXPORT svt_n3f_v3f
{
public:

    Real32 m_oN[3];
    Real32 m_oV[3];

public:

    svt_n3f_v3f( Real32 fNX = 0.0f, Real32 fNY = 0.0f, Real32 fNZ = 0.0f, Real32 fX = 0.0f, Real32 fY = 0.0f, Real32 fZ = 0.0f )
    {
	m_oN[0] = fNX;
	m_oN[1] = fNY;
	m_oN[2] = fNZ;

	m_oV[0] = fX;
	m_oV[1] = fY;
	m_oV[2] = fZ;
    };

    svt_n3f_v3f( const svt_n3f_v3f& rOther )
    {
        memcpy( m_oN, rOther.m_oN, sizeof(Real32) * 6 );
    };

    Real32& operator[](unsigned i)
    {
	return m_oN[i];
    };
};

/**
 * Very simple point class for interleaved OpenGL VARs.
 * Color and vertex, both three floats.
 */
class DLLEXPORT svt_c3f_v3f
{
    Real32 m_oV3[6];

public:

    svt_c3f_v3f( Real32 fR = 1.0f, Real32 fG = 1.0f, Real32 fB = 1.0f, Real32 fX = 0.0f, Real32 fY = 0.0f, Real32 fZ = 0.0f )
    {
	m_oV3[0] = fR;
	m_oV3[1] = fG;
	m_oV3[2] = fB;

	m_oV3[3] = fX;
	m_oV3[4] = fY;
	m_oV3[5] = fZ;
    };

    svt_c3f_v3f( Real32 fR, Real32 fG, Real32 fB, const svt_vector4<Real64>& rVec )
    {
	m_oV3[0] = fR;
	m_oV3[1] = fG;
	m_oV3[2] = fB;

	m_oV3[3] = rVec.x();
	m_oV3[4] = rVec.y();
	m_oV3[5] = rVec.z();
    };

    svt_c3f_v3f( const svt_c3f_v3f& rOther )
    {
        memcpy( m_oV3, rOther.m_oV3, sizeof(Real32) * 6 );
    };

    Real32& operator[](unsigned i)
    {
	return m_oV3[i];
    };

    void set( Real32 fR = 1.0f, Real32 fG = 1.0f, Real32 fB = 1.0f, Real32 fX = 0.0f, Real32 fY = 0.0f, Real32 fZ = 0.0f )
    {
	m_oV3[0] = fR;
	m_oV3[1] = fG;
	m_oV3[2] = fB;

	m_oV3[3] = fX;
	m_oV3[4] = fY;
	m_oV3[5] = fZ;
    };
};

/**
 * Very simple point class for interleaved OpenGL VARs.
 * Color, normal and vertex. Color is stored in four bytes, normal and vertex are both three floats.
 */
class DLLEXPORT svt_c4ub_n3f_v3f
{
public:

    unsigned char m_oC[4];
    unsigned char m_aDummy[4];
    svt_v3f       m_oN;
    svt_v3f       m_oV;

    svt_c4ub_n3f_v3f( Real32 fR = 0.0f, Real32 fG = 0.0f, Real32 fB = 0.0f, Real32 fNX = 0.0f, Real32 fNY = 0.0f, Real32 fNZ = 0.0f, Real32 fX = 0.0f, Real32 fY = 0.0f, Real32 fZ = 0.0f )
    {
	m_oC[0] = (unsigned char)(fR);
	m_oC[1] = (unsigned char)(fG);
	m_oC[2] = (unsigned char)(fB);
	m_oC[3] = 255;

	m_oN[0] = fNX;
	m_oN[1] = fNY;
	m_oN[2] = fNZ;

	m_oV[0] = fX;
	m_oV[1] = fY;
	m_oV[2] = fZ;
    };

    svt_c4ub_n3f_v3f( const svt_vector4<Real32>& rOther )
    {
        m_oV[0] = rOther.x();
        m_oV[1] = rOther.y();
        m_oV[2] = rOther.z();
    };

    svt_c4ub_n3f_v3f( const svt_c4ub_n3f_v3f& rOther )
    {
        memcpy( m_oC, rOther.m_oC, (sizeof(svt_c4ub_n3f_v3f) ) );
    };

    void setV( Real32 fX, Real32 fY, Real32 fZ )
    {
        m_oV[0] = fX;
        m_oV[1] = fY;
	m_oV[2] = fZ;
    }
    void setN( Real32 fNX, Real32 fNY, Real32 fNZ )
    {
        m_oN[0] = fNX;
        m_oN[1] = fNY;
	m_oN[2] = fNZ;
    }
    void setC( Real32 fR, Real32 fG, Real32 fB, Real32 fA = 255.0f )
    {
        m_oC[0] = (unsigned char)(fR);
        m_oC[1] = (unsigned char)(fG);
        m_oC[2] = (unsigned char)(fB);
        m_oC[3] = (unsigned char)(fA);
    }
    void setC( unsigned char cR, unsigned char cG, unsigned char cB )
    {
        m_oC[0] = cR;
        m_oC[1] = cG;
	m_oC[2] = cB;
    }

    void normalize()
    {
	Real32 fLength =  sqrt( m_oV[0]*m_oV[0] + m_oV[1]*m_oV[1] + m_oV[2]*m_oV[2] );

	if (fLength > 0 )
	{
	    m_oV[0] /= fLength;
	    m_oV[1] /= fLength;
	    m_oV[2] /= fLength;
	}
    };

    void normalizeNorm()
    {
	Real32 fLength =  sqrt( m_oN[0]*m_oN[0] + m_oN[1]*m_oN[1] + m_oN[2]*m_oN[2] );

	if (fLength > 0 )
	{
	    m_oN[0] /= fLength;
	    m_oN[1] /= fLength;
	    m_oN[2] /= fLength;
	}
    };

    inline svt_c4ub_n3f_v3f operator-(const svt_c4ub_n3f_v3f& rOther)
    {
        svt_c4ub_n3f_v3f oTmp;

	oTmp.m_oV[0] = m_oV[0] - rOther.m_oV[0];
	oTmp.m_oV[1] = m_oV[1] - rOther.m_oV[1];
	oTmp.m_oV[2] = m_oV[2] - rOther.m_oV[2];

	return oTmp;
    };

    inline svt_c4ub_n3f_v3f operator+(const svt_c4ub_n3f_v3f& rOther)
    {
        svt_c4ub_n3f_v3f oTmp;

	oTmp.m_oV[0] = m_oV[0] + rOther.m_oV[0];
	oTmp.m_oV[1] = m_oV[1] + rOther.m_oV[1];
	oTmp.m_oV[2] = m_oV[2] + rOther.m_oV[2];

	return oTmp;
    };

    inline svt_c4ub_n3f_v3f operator+=(const svt_c4ub_n3f_v3f& rOther)
    {
	m_oV[0] += rOther.m_oV[0];
	m_oV[1] += rOther.m_oV[1];
	m_oV[2] += rOther.m_oV[2];

	return *this;
    };

    inline svt_c4ub_n3f_v3f operator*(const Real32& fFact)
    {
	svt_c4ub_n3f_v3f oTmp;

	oTmp.m_oV[0] = fFact * m_oV[0];
	oTmp.m_oV[1] = fFact * m_oV[1];
	oTmp.m_oV[2] = fFact * m_oV[2];

	return oTmp;
    };

    inline svt_c4ub_n3f_v3f operator*(const svt_c4ub_n3f_v3f& rOther)
    {
        svt_c4ub_n3f_v3f oTmp;

	oTmp.m_oV[0] = m_oV[0] * rOther.m_oV[0];
	oTmp.m_oV[1] = m_oV[1] * rOther.m_oV[1];
	oTmp.m_oV[2] = m_oV[2] * rOther.m_oV[2];

	return oTmp;
    };

};

#endif
