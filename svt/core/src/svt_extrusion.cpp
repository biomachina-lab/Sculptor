/***************************************************************************
                          svt_extrusion.cpp
			  -----------------
    begin                : 9/26/03
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_extrusion.h>

/**
 * Constructor
 */
svt_extrusion::svt_extrusion()
{
    add(&m_oCont);
}

/**
 * set path
 * \param aPath svt_array object with the points that form the line along the object is extruded.
 */
void svt_extrusion::setPath(svt_array<svt_vec4real32> aPath)
{
    m_aPath = aPath;
}
/**
 * get path
 * \return svt_array object with the points that form the line along the object is extruded.
 */
svt_array<svt_vec4real32> svt_extrusion::getPath()
{
    return m_aPath;
}

/**
 * set shape
 * \param aPoints svt_array with the points that form the shape of the object that gets extruded
 */
void svt_extrusion::setShape(svt_array<svt_vec4real32> aPoints)
{
    m_aPoints = aPoints;
}
/**
 * get shape
 * \return svt_array with the points that form the shape of the object that gets extruded
 */
svt_array<svt_vec4real32> svt_extrusion::getShape()
{
    return m_aPoints;
}

/**
 * extrude
 */
void svt_extrusion::extrude()
{
    m_oCont.delAll();

    svt_matrix4<Real32> oMat;
    svt_matrix4<Real32> oOldMat;
    svt_vec4real32 oVecA;
    svt_vec4real32 oVecB;
    svt_vec4real32 oVecC;
    svt_vec4real32 oVecD;
    svt_vec4real32 oVecN;
    svt_vec4real32 oVecN1;
    svt_vec4real32 oVecN2;
    svt_vec4real32 oVecX;
    svt_vec4real32 oVecY;
    int iFirst, iSec;

    for(int i=1;i<m_aPath.numElements()-1;i++)
    {
	// calculate transformation matrix
        oVecA = m_aPath[i-1];
        oVecB = m_aPath[i];
	oVecC = m_aPath[i+1];
	oVecN = oVecB - oVecA; oVecN.normalize();
        oVecX = vectorProduct(oVecN, svt_vec4real32(oVecC - oVecB)); oVecX.normalize();
        oVecY = vectorProduct(oVecN, oVecX); oVecY.normalize();

        oMat.loadIdentity();
        oMat[0][0] = oVecX[0];
        oMat[1][0] = oVecX[1];
        oMat[2][0] = oVecX[2];

	oMat[0][1] = oVecY[0];
        oMat[1][1] = oVecY[1];
        oMat[2][1] = oVecY[2];

	oMat[0][2] = oVecN[0];
        oMat[1][2] = oVecN[1];
        oMat[2][2] = oVecN[2];

        oMat.translate(m_aPath[i]);

	// ok, now apply that matrix to the object
        if (i!=1)
	    for(int j=0; j<m_aPoints.numElements();j++)
	    {
		if (j!=m_aPoints.numElements()-1)
		{
		    iFirst = j;
		    iSec = j+1;
		} else {
		    iFirst = j;
                    iSec = 0;
		}

		oVecA = m_aPoints[iFirst] * oOldMat;
		oVecB = m_aPoints[iSec] * oOldMat;
		oVecC = m_aPoints[iFirst] * oMat;
		oVecD = m_aPoints[iSec] * oMat;

                oVecN = vectorProduct( oVecB - oVecA, oVecC - oVecA ); oVecN.normalize();

		/*m_oCont << new svt_basic_triangle(
						  oVecA,
						  oVecB,
						  oVecC,
						  oVecN,
						  oVecN,
                                                  oVecN
						 );

		m_oCont << new svt_basic_triangle(
						  oVecD,
						  oVecC,
						  oVecB,
						  oVecN,
						  oVecN,
                                                  oVecN
					      );*/
	    }

        oOldMat = oMat;
    }
}
