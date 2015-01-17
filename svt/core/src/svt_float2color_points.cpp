/***************************************************************************
                          svt_float2color_points
                          ----------------------
    begin                : 03/04/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_float2color_points.h>

/**
 * Constructor
 */
svt_float2color_points::svt_float2color_points( float fMin, float fMax ) : svt_float2color( fMin, fMax )
{
}

/**
 * get the red color value of the float
 * \param fX the float value
 */
char svt_float2color_points::getR(float fX)
{
    unsigned int i;

    if (m_aPointsR.size() == 0 || m_aPointsR.size() == 1)
        return 0;

    for(i=1; i<m_aPointsR.size(); i++)
	if (m_aPointsR[i].x() > fX) break;

    if (i == m_aPointsR.size())
        i--;

    if (fX < m_aPointsR[i-1].x() || fX > m_aPointsR[i].x())
        return 0;

    if (m_aPointsR[i].y() + ((m_aPointsR[i].y() - m_aPointsR[i-1].y()) / (m_aPointsR[i].x() - fX)) + 0.5 > 255.0)
        return 0xff;
    if (m_aPointsR[i].y() + ((m_aPointsR[i].y() - m_aPointsR[i-1].y()) / (m_aPointsR[i].x() - fX)) + 0.5 < 0.0)
        return 0x00;

    return (unsigned char)(m_aPointsR[i].y() + ((m_aPointsR[i].y() - m_aPointsR[i-1].y()) / (m_aPointsR[i].x() - fX)) + 0.5);
}
/**
 * get the green color value of the float
 * \param fX the float value
 */
char svt_float2color_points::getG(float fX)
{
    unsigned int i;

    if (m_aPointsG.size() == 0 || m_aPointsG.size() == 1)
        return 0;

    for(i=1; i<m_aPointsG.size(); i++)
	if (m_aPointsG[i].x() > fX) break;

    if (i == m_aPointsG.size())
        i--;

    if (fX < m_aPointsG[i-1].x() || fX > m_aPointsG[i].x())
        return 0;

    if (m_aPointsG[i].y() + ((m_aPointsG[i].y() - m_aPointsG[i-1].y()) / (m_aPointsG[i].x() - fX)) + 0.5 > 255.0)
        return 0xff;
    if (m_aPointsG[i].y() + ((m_aPointsG[i].y() - m_aPointsG[i-1].y()) / (m_aPointsG[i].x() - fX)) + 0.5 < 0.0)
        return 0x00;

    return (unsigned char)(m_aPointsG[i].y() + ((m_aPointsG[i].y() - m_aPointsG[i-1].y()) / (m_aPointsG[i].x() - fX)) + 0.5);
}
/**
 * get the blue color value of the float
 * \param fX the float value
 */
char svt_float2color_points::getB(float fX)
{
    unsigned int i;

    if (m_aPointsB.size() == 0 || m_aPointsB.size() == 1)
        return 0;

    for(i=1; i<m_aPointsB.size(); i++)
	if (m_aPointsB[i].x() > fX) break;

    if (i == m_aPointsB.size())
        i--;

    if (fX < m_aPointsB[i-1].x() || fX > m_aPointsB[i].x())
	return 0;

    if (m_aPointsB[i].y() + ((m_aPointsB[i].y() - m_aPointsB[i-1].y()) / (m_aPointsB[i].x() - fX)) + 0.5 > 255.0)
        return 0xff;
    if (m_aPointsB[i].y() + ((m_aPointsB[i].y() - m_aPointsB[i-1].y()) / (m_aPointsB[i].x() - fX)) + 0.5 < 0.0)
        return 0x00;

    return (unsigned char)(m_aPointsB[i].y() + ((m_aPointsB[i].y() - m_aPointsB[i-1].y()) / (m_aPointsB[i].x() - fX)) + 0.5);
}

/**
 * add a point
 * \param oPoint svt_vector3<double> object - z is always 0!
 */
void svt_float2color_points::addPointR(svt_vector3<double>oPoint)
{
    if (m_aPointsR.size() == 0)
    {
	m_aPointsR.push_back( oPoint );
	return;
    }

    for(std::vector< svt_vector3<double> >::iterator iIter = m_aPointsR.begin(); iIter != m_aPointsR.end(); iIter++)
    {
	if (oPoint.x() < iIter->x())
	{
	    m_aPointsR.insert( iIter, oPoint );
	    return;
	}
    }

    m_aPointsR.push_back( oPoint );
};
/**
 * add a point
 * \param oPoint svt_vector3<double> object - z is always 0!
 */
void svt_float2color_points::addPointG(svt_vector3<double>oPoint)
{
    if (m_aPointsG.size() == 0)
    {
	m_aPointsG.push_back( oPoint );
	return;
    }

    for(std::vector< svt_vector3<double> >::iterator iIter = m_aPointsG.begin(); iIter != m_aPointsG.end(); iIter++)
    {
	if (oPoint.x() < iIter->x())
	{
	    m_aPointsG.insert( iIter, oPoint );
	    return;
	}
    }

    m_aPointsG.push_back( oPoint );
};
/**
 * add a point
 * \param oPoint svt_vector3<double> object - z is always 0!
 */
void svt_float2color_points::addPointB(svt_vector3<double>oPoint)
{
    if (m_aPointsB.size() == 0)
    {
	m_aPointsB.push_back( oPoint );
	return;
    }

    for(std::vector< svt_vector3<double> >::iterator iIter = m_aPointsB.begin(); iIter != m_aPointsB.end(); iIter++)
    {
	if (oPoint.x() < iIter->x())
	{
	    m_aPointsB.insert( iIter, oPoint );
	    return;
	}
    }

    m_aPointsB.push_back( oPoint );
};
