/***************************************************************************
                          svt_float2opacity_points
                          ------------------------
    begin                : 03/04/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// clib includes
#include <math.h>
// svt includes
#include <svt_core.h>
#include <svt_float2opacity_points.h>

#include <stdio.h>

/**
 * Constructor
 */
svt_float2opacity_points::svt_float2opacity_points( float fMin, float fMax) : svt_float2opacity( fMin, fMax )
{
    calcFact();
};

/**
 * get the opacity value of the float
 * \param fX the float value
 */
char svt_float2opacity_points::getOpacity(float fX)
{
    unsigned int i;

    if (m_aPointsO.size() == 0 || m_aPointsO.size() == 1)
        return 0;

    for(i=1; i<m_aPointsO.size(); i++)
	if (m_aPointsO[i].x() > fX) break;

    if (i == m_aPointsO.size())
        i--;

    if (fX < m_aPointsO[i-1].x() || fX > m_aPointsO[i].x())
	return 0;

    if (m_aPointsO[i].y() + ((m_aPointsO[i].y() - m_aPointsO[i-1].y()) / (m_aPointsO[i].x() - fX)) + 0.5 > 255.0)
        return 0xff;
    if (m_aPointsO[i].y() + ((m_aPointsO[i].y() - m_aPointsO[i-1].y()) / (m_aPointsO[i].x() - fX)) + 0.5 < 0.0)
        return 0x00;

    return (unsigned char)(m_aPointsO[i].y() + ((m_aPointsO[i].y() - m_aPointsO[i-1].y()) / (m_aPointsO[i].x() - fX)) + 0.5);
}

/**
 * add a point
 * \param oPoint svt_vector3<double> object - z is always 0!
 */
void svt_float2opacity_points::addPointO(svt_vector3<double>oPoint)
{
    if (m_aPointsO.size() == 0)
    {
	m_aPointsO.push_back( oPoint );
	return;
    }

    for(std::vector< svt_vector3<double> >::iterator iIter = m_aPointsO.begin(); iIter != m_aPointsO.end(); iIter++)
    {
	if (oPoint.x() < iIter->x())
	{
	    m_aPointsO.insert( iIter, oPoint );
	    return;
	}
    }

    m_aPointsO.push_back( oPoint );
};
