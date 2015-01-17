/***************************************************************************
                          svt_float2mat_points
                          --------------------
    begin                : 03/04/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_float2mat_points.h>

/**
 * Constructor
 */
svt_float2mat_points::svt_float2mat_points( float fMin, float fMax ) : svt_float2mat( fMin, fMax ),
    m_bUninitialized(true)
{
    m_aPointsR.push_back( svt_vector3<double>(fMin, 0.0, 0.0) );
    m_aPointsR.push_back( svt_vector3<double>(fMax, 255.0, 0.0) );
    m_aPointsG.push_back( svt_vector3<double>(fMin, 0.0, 0.0) );
    m_aPointsG.push_back( svt_vector3<double>(fMax, 255.0, 0.0) );
    m_aPointsB.push_back( svt_vector3<double>(fMin, 0.0, 0.0) );
    m_aPointsB.push_back( svt_vector3<double>(fMax, 255.0, 0.0) );
    m_aPointsO.push_back( svt_vector3<double>(fMin, 0.0, 0.0) );
    m_aPointsO.push_back( svt_vector3<double>(fMax, 255.0, 0.0) );
}

/**
 * get the red color value of the float
 * \param fX the float value
 */
unsigned char svt_float2mat_points::getR(float fX)
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

    double fM = (((m_aPointsR[i].y() - m_aPointsR[i-1].y()) / (m_aPointsR[i].x() - m_aPointsR[i-1].x())) * (fX - m_aPointsR[i-1].x())) + m_aPointsR[i-1].y();

    if (fM + 0.5 > 255.0)
	return 0xff;
    if (fM < 0.0)
        return 0x00;

    return (unsigned char)(fM + 0.5);
}
/**
 * get the green color value of the float
 * \param fX the float value
 */
unsigned char svt_float2mat_points::getG(float fX)
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

    double fM = (((m_aPointsG[i].y() - m_aPointsG[i-1].y()) / (m_aPointsG[i].x() - m_aPointsG[i-1].x())) * (fX - m_aPointsG[i-1].x())) + m_aPointsG[i-1].y();

    if (fM + 0.5 > 255.0)
	return 0xff;
    if (fM < 0.0)
        return 0x00;

    return (unsigned char)(fM + 0.5);

}
/**
 * get the blue color value of the float
 * \param fX the float value
 */
unsigned char svt_float2mat_points::getB(float fX)
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

    double fM = (((m_aPointsB[i].y() - m_aPointsB[i-1].y()) / (m_aPointsB[i].x() - m_aPointsB[i-1].x())) * (fX - m_aPointsB[i-1].x())) + m_aPointsB[i-1].y();

    if (fM + 0.5 > 255.0)
	return 0xff;
    if (fM < 0.0)
        return 0x00;

    return (unsigned char)(fM + 0.5);
}

/**
 * get the opacity value of the float
 * \param fX the float value
 */
unsigned char svt_float2mat_points::getOpacity(float fX)
{
    unsigned int i;

    if (fX == 0.0 && m_bZero == true)
        return 0x00;

    if (m_aPointsO.size() == 0 || m_aPointsO.size() == 1)
        return 0x00;

    for(i=1; i<m_aPointsO.size(); i++)
	if (m_aPointsO[i].x() > fX) break;

    if (i == m_aPointsO.size())
        i--;

    if (fX < m_aPointsO[i-1].x() || fX > m_aPointsO[i].x())
	return 0;

    double fM = (((m_aPointsO[i].y() - m_aPointsO[i-1].y()) / (m_aPointsO[i].x() - m_aPointsO[i-1].x())) * (fX - m_aPointsO[i-1].x())) + m_aPointsO[i-1].y();

    //cout << "fM+0.5:" << fM + 0.5 << " char:" << (int)((unsigned char)(fM + 0.5)) << endl;

    if (fabs(fM + 0.5 - (float)((unsigned char)(fM + 0.5))) > 1)
        cout << "svt_float2mat_points> Warning!!!!!!!!!!!!!!!!!!!" << endl;

    if (fM + 0.5 > 255.0)
	return 0xff;
    if (fM < 0.0)
        return 0x00;

    return (unsigned char)(fM + 0.5);
}

/**
 * add a point
 * \param oPoint svt_vector3<double> object - z is always 0!
 */
void svt_float2mat_points::addPointR(svt_vector3<double>oPoint)
{
    m_bUninitialized = false;

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
void svt_float2mat_points::addPointG(svt_vector3<double>oPoint)
{
    m_bUninitialized = false;

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
void svt_float2mat_points::addPointB(svt_vector3<double>oPoint)
{
    m_bUninitialized = false;

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
/**
 * add a point
 * \param oPoint svt_vector3<double> object - z is always 0!
 */
void svt_float2mat_points::addPointO(svt_vector3<double>oPoint)
{
    m_bUninitialized = false;

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

/**
 * update the object, if the minimum or the maximum value have changed
 */
void svt_float2mat_points::update()
{
    if (m_bUninitialized == true)
    {
	m_aPointsR[0] = svt_vector3<double>(m_fMin, 0.0, 0.0);
	m_aPointsR[1] = svt_vector3<double>(m_fMax, 255.0, 0.0);
	m_aPointsG[0] = svt_vector3<double>(m_fMin, 0.0, 0.0);
	m_aPointsG[1] = svt_vector3<double>(m_fMax, 255.0, 0.0);
	m_aPointsB[0] = svt_vector3<double>(m_fMin, 0.0, 0.0);
	m_aPointsB[1] = svt_vector3<double>(m_fMax, 255.0, 0.0);
	m_aPointsO[0] = svt_vector3<double>(m_fMin, 0.0, 0.0);
	m_aPointsO[1] = svt_vector3<double>(m_fMax, 255.0, 0.0);
    }
}

/**
 * save to file
 * \param oFilename std string with filename
 */
void svt_float2mat_points::saveToFile(string oFilename)
{
    FILE* pFile = fopen(oFilename.c_str(), "w");

    if (pFile != NULL)
    {
        unsigned int i;

	fprintf(pFile, "<!-- SVT transfer function -->\n");

        fprintf(pFile, "%i\n", (int)(m_aPointsR.size()) );
	for(i=0; i<m_aPointsR.size(); i++)
            fprintf(pFile, "<%lf,%lf>\n", m_aPointsR[i].x(), m_aPointsR[i].y() );

	fprintf(pFile, "%i\n", (int)(m_aPointsG.size()) );
	for(i=0; i<m_aPointsG.size(); i++)
	    fprintf(pFile, "<%lf,%lf>\n", m_aPointsG[i].x(), m_aPointsG[i].y() );

        fprintf(pFile, "%i\n", (int)(m_aPointsB.size()) );
	for(i=0; i<m_aPointsB.size(); i++)
            fprintf(pFile, "<%lf,%lf>\n", m_aPointsB[i].x(), m_aPointsB[i].y() );

	fprintf(pFile, "%i\n", (int)(m_aPointsO.size()) );
	for(i=0; i<m_aPointsO.size(); i++)
            fprintf(pFile, "<%lf,%lf>\n", m_aPointsO[i].x(), m_aPointsO[i].y() );

	fclose(pFile);
    }
}
/**
 * load from file
 * \param oFilename std string with filename
 */
void svt_float2mat_points::loadFromFile(string oFilename)
{
    FILE* pFile = fopen(oFilename.c_str(), "r");

    if (pFile != NULL)
    {
	if (fscanf(pFile, "<!-- SVT transfer function -->\n") == 0)
	    svtout << "Warning: Did not find the header expected for a transfer function file..." << endl;

        unsigned int iCount;
        unsigned int i;

        m_aPointsR.clear();
        m_aPointsG.clear();
        m_aPointsB.clear();
        m_aPointsO.clear();

        double fRx, fRy, fGx, fGy, fBx, fBy, fOx, fOy;

	if (fscanf(pFile,"%i\n", &iCount ) == 0) { fclose(pFile); svtout << "Warning: premature end of file while reading transfer function..." << endl; return; }
        cout << iCount << endl;
	for(i=0; i<iCount; i++)
	{
	    if ( fscanf(pFile, "<%lf,%lf>\n", &fRx, &fRy ) < 2 ) { fclose(pFile); svtout << "Warning: premature end of file while reading transfer function..." << endl; return; }
            addPointR(svt_vector3<double>(fRx, fRy, 0.0));
	}

	if (fscanf(pFile,"%i\n", &iCount ) == 0) { fclose(pFile); svtout << "Warning: premature end of file while reading transfer function..." << endl; return; }
        cout << iCount << endl;
	for(i=0; i<iCount; i++)
	{
	    if ( fscanf(pFile, "<%lf,%lf>\n", &fGx, &fGy ) < 2 ) { fclose(pFile); svtout << "Warning: premature end of file while reading transfer function..." << endl; return; }
            addPointG(svt_vector3<double>(fGx, fGy, 0.0));
	}

	if (fscanf(pFile,"%i\n", &iCount ) == 0) { fclose(pFile); svtout << "Warning: premature end of file while reading transfer function..." << endl; return; }
        cout << iCount << endl;
	for(i=0; i<iCount; i++)
	{
	    if ( fscanf(pFile, "<%lf,%lf>\n", &fBx, &fBy ) < 2 ) { fclose(pFile); svtout << "Warning: premature end of file while reading transfer function..." << endl; return; }
            addPointB(svt_vector3<double>(fBx, fBy, 0.0));
	}

	if (fscanf(pFile,"%i\n", &iCount ) == 0) { fclose(pFile); svtout << "Warning: premature end of file while reading transfer function..." << endl; return; }
        cout << iCount << endl;
	for(i=0; i<iCount; i++)
	{
	    if ( fscanf(pFile, "<%lf,%lf>\n", &fOx, &fOy ) < 2 ) { fclose(pFile); svtout << "Warning: premature end of file while reading transfer function..." << endl; return; }
            addPointO(svt_vector3<double>(fOx, fOy, 0.0));
	}

	fclose(pFile);
    }
}
