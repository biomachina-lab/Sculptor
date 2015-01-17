/***************************************************************************
                          svt_aliasobj
			  ------------
    begin                : 08/22/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_aliasobj.h>

/**
 * Constructor
 */
svt_aliasobj::svt_aliasobj()
{
    svt_matrix4<Real32> oMat;
    m_aStack.push( oMat );
};

/**
 * Add a mesh
 * \param aVertices the new vertices that should be added
 * \param aNormals the new normal vectors that should be added
 * \param aFaces the new face indices that should be added
 */
void svt_aliasobj::addMesh( vector< svt_vector4< Real32 > > aVertices, vector< svt_vector4< Real32 > > aNormals, vector< svt_face > aFaces )
{
    unsigned int iStartV = m_aVertices.size();
    unsigned int iStartN = m_aNormals.size();
    unsigned int i;

    // the vectors have to be transformed and then added
    for(i=0; i<aVertices.size(); i++)
    {
        aVertices[i][3] = 1.0;
	m_aVertices.push_back( m_aStack.top() * aVertices[i] );
    }

    // normals can be added directly
    m_aNormals.insert( m_aNormals.end(), aNormals.begin(), aNormals.end() );

    // face indices have to be corrected as there already vertices
    svt_face oFace;
    for(i=0; i<aFaces.size(); i++)
    {
	oFace = aFaces[i];
        oFace.setVI_A( oFace[0] + iStartV );
        oFace.setVI_B( oFace[1] + iStartV );
        oFace.setVI_C( oFace[2] + iStartV );
        oFace.setNI_A( oFace.getNI_A() + iStartN );
        oFace.setNI_B( oFace.getNI_B() + iStartN );
        oFace.setNI_C( oFace.getNI_C() + iStartN );

	m_aFaces.push_back( oFace );
    }
};


/**
 * Output the object as a OBJ file (to stdout)
 */
void svt_aliasobj::drawOBJ()
{
    // before streaming out the faces, vertices and normals, we have to remove identical vertices in order to achieve a correct topology of our meshes
    cleanMesh();

    if (m_aColorChangePos.size() == 0 || m_aVertices.size() == 0 || m_aNormals.size() == 0 || m_aFaces.size() == 0)
        return;

    unsigned int i;

    if (m_aFaces.size() > 50000)
        svt_exception::ui()->progressPopup("Exporting OBJ file...", 0, m_aVertices.size());

    try
    {
        for(i=0; i<m_aVertices.size(); i++)
        {
            cout << "v " << m_aVertices[i].x() << " " << m_aVertices[i].y() << " " << m_aVertices[i].z() << endl;

            if (m_aFaces.size() > 50000 && i%1000 == 0)
                svt_exception::ui()->progress( i, m_aVertices.size() );
        }

        for(i=0; i<m_aNormals.size(); i++)
        {
            cout << "vn " << m_aNormals[i].x() << " " << m_aNormals[i].y() << " " << m_aNormals[i].z() << endl;

            if (m_aFaces.size() > 50000 && i%1000 == 0)
                svt_exception::ui()->progress( i, m_aNormals.size() );
        }

        unsigned int iNextCC = m_aColorChangePos[0];
        unsigned int iCC = 0;
        for(i=0; i<m_aFaces.size(); i++)
        {
            if ( i == iNextCC )
            {
                cout << "usemtl svtmat" << m_aColorChange[iCC] << endl;
                iCC++;

                if (iCC < m_aColorChangePos.size())
                    iNextCC = m_aColorChangePos[iCC];
            }

            m_aFaces[i].writeToCout();

            if (m_aFaces.size() > 50000 && i%1000 == 0)
                svt_exception::ui()->progress( i, m_aFaces.size() );
        }

    } catch( svt_userInterrupt& )
    {
    }

    if (m_aFaces.size() > 50000)
        svt_exception::ui()->progressPopdown();
}

/**
 * Output the materials used (to stdout)
 */
void svt_aliasobj::drawMAT()
{
    char oStr[256];
    for(unsigned int i=0; i<m_aColors.size(); i++)
    {
        sprintf( oStr, "svtmat%i", i );
	m_aColors[i].applyOBJ( oStr );
    }
};

/**
 * Set color.
 * The new color will be used from now on for all the outputs until a new color is set.
 */
void svt_aliasobj::setColor( svt_color& rColor )
{
    unsigned int i;

    // was the previous color actually used?
    if (m_aColorChangePos.size() > 0)
	if (m_aColorChangePos[ m_aColorChangePos.size() -1 ] == m_aFaces.size())
	{
	    m_aColors.pop_back();
	    m_aColorChange.pop_back();
	    m_aColorChangePos.pop_back();
	}

    for(i=0; i<m_aColors.size(); i++)
	if (m_aColors[i] == rColor)
	    break;

    // not found...
    if (i >= m_aColors.size())
    {
	m_aColors.push_back( rColor );
	m_aColorChange.push_back( m_aColors.size()-1 );
        m_aColorChangePos.push_back( m_aFaces.size() );

    } else {

	m_aColorChange.push_back( i );
        m_aColorChangePos.push_back( m_aFaces.size() );
    }
};

/**
 * Set color.
 * The new color will be used from now on for all the outputs until a new color is set.
 */
void svt_aliasobj::setColor( svt_color& rColor, unsigned int iFacePos )
{
    unsigned int i;

    for(i=0; i<m_aColors.size(); i++)
	if (m_aColors[i] == rColor)
	    break;

    // not found...
    if (i >= m_aColors.size())
    {
	m_aColors.push_back( rColor );
	m_aColorChange.push_back( m_aColors.size()-1 );
        m_aColorChangePos.push_back( m_aFaces.size() + iFacePos );

    } else {

	m_aColorChange.push_back( i );
        m_aColorChangePos.push_back( m_aFaces.size() + iFacePos );
    }
};


/**
 * Push a new transformation matrix on the stack
 */
void svt_aliasobj::pushMatrix( svt_matrix4<Real32> oMatrix )
{
    svt_matrix4<Real32> oTop = m_aStack.top();
    oMatrix = oTop * oMatrix;

    m_aStack.push( oMatrix );
};
/**
 * Pop the last transformation matrix from the stack
     */
void svt_aliasobj::popMatrix( )
{
    m_aStack.pop();
};

/**
 * Get the current transformation matrix
 */
svt_matrix4<Real32> svt_aliasobj::getMatrix()
{
    return m_aStack.top();
}

/**
 * Clean mesh - before streaming out the faces, vertices and normals, we have to remove identical vertices in order to obtain a correct topology of our meshes.
 */
void svt_aliasobj::cleanMesh()
{
    // vertex array
    vector< svt_vector4<Real32> > aNewVertices;
    // has changed array?
    vector< unsigned int > aHasChanged;
    // face array
    vector< svt_face > aNewFaces;
    // index variables
    unsigned int i;

    //
    // first find the identical vertices using a space partitioning technique
    //
    vector< unsigned int > aVertices = findIdenticals(true);

    //
    // now only the coordinates of the unique vertices have to be added to a new, consolidated list. All the doubles need to point to the new, unique vectors.
    //
    svt_exception::ui()->progressPopup("Cleaning Mesh...", 0, aVertices.size());
    unsigned int iTmp;
    try
    {
	for(i=0; i<aVertices.size(); i++)
	{
	    if (aVertices[i] == i)
	    {
		aNewVertices.push_back( m_aVertices[i] );
                aVertices[i] = aNewVertices.size() - 1;
            } else {
                iTmp = aVertices[i];
                aVertices[i] = aVertices[iTmp];
            }

	    if (i%1000 == 0)
	        svt_exception::ui()->progress( i, aVertices.size() );
	}
    } catch( svt_userInterrupt& )
    {
	svt_exception::ui()->progressPopdown();
        return;
    }
    svt_exception::ui()->progressPopdown();

    //
    // Now that we have a correct vertex list, we now also need to adjust the vertex-indices of the faces
    //
    svt_face oFace;
    svt_exception::ui()->progressPopup("Cleaning Mesh...", 0, m_aFaces.size());
    try
    {
	for(i=0; i<m_aFaces.size(); i++)
        {
            oFace = m_aFaces[i];

	    oFace.setVI_A( aVertices[ m_aFaces[i][0]-1 ] +1);
	    oFace.setVI_B( aVertices[ m_aFaces[i][1]-1 ] +1);
            oFace.setVI_C( aVertices[ m_aFaces[i][2]-1 ] +1);

	    aNewFaces.push_back( oFace );

	    if (i%1000 == 0)
	        svt_exception::ui()->progress( i, m_aFaces.size() );
	}
    } catch( svt_userInterrupt& )
    {
	svt_exception::ui()->progressPopdown();
        return;
    }
    svt_exception::ui()->progressPopdown();

    //
    // Now we need to copy the corrected data back into the internal vertex and face lists
    //
    m_aVertices.clear();
    m_aFaces.clear();

    m_aVertices = aNewVertices;
    m_aFaces = aNewFaces;
}

/**
 * Get the minimal coordinates of the point cloud - it will return a vector that has in each dimension the information about the minimal
 * coordinate it has found in the cloud.
 */
svt_vector4<Real64> svt_aliasobj::getMinCoord()
{
    Real64 fMinX = 1.0E10;
    Real64 fMinY = 1.0E10;
    Real64 fMinZ = 1.0E10;

    unsigned int i;
    unsigned int iSize = m_aVertices.size();

    for(i=0; i<iSize; i++)
    {
	if (m_aVertices[i].x() < fMinX)
            fMinX = m_aVertices[i].x();
	if (m_aVertices[i].y() < fMinY)
            fMinY = m_aVertices[i].y();
	if (m_aVertices[i].z() < fMinZ)
	    fMinZ = m_aVertices[i].z();
    }

    svt_vector4<Real64> oVec;
    oVec.x( fMinX );
    oVec.y( fMinY );
    oVec.z( fMinZ );

    return oVec;
};

/**
 * Get the maximal coordinates of the point cloud - it will return a vector that has in each dimension the information about the maximal
 * coordinate it has found in the cloud.
 */
svt_vector4<Real64> svt_aliasobj::getMaxCoord()
{
    Real64 fMaxX = -1.0E10;
    Real64 fMaxY = -1.0E10;
    Real64 fMaxZ = -1.0E10;

    unsigned int i;
    unsigned int iSize = m_aVertices.size();

    for(i=0; i<iSize; i++)
    {
	if (m_aVertices[i].x() > fMaxX)
            fMaxX = m_aVertices[i].x();
	if (m_aVertices[i].y() > fMaxY)
            fMaxY = m_aVertices[i].y();
	if (m_aVertices[i].z() > fMaxZ)
	    fMaxZ = m_aVertices[i].z();
    }

    svt_vector4<Real64> oVec;
    oVec.x( fMaxX );
    oVec.y( fMaxY );
    oVec.z( fMaxZ );

    return oVec;
};

/**
 * Find identical vertices.
 * \return Return a vector of indices. If the value/index at position i is i, then this is a unique vertex. Otherwise the value/index will point to the identical twin.
 */
vector< unsigned int > svt_aliasobj::findIdenticals(bool bShowProgress)
{
    printf("Merging identical vertices...\n");

    unsigned int iDoubles = 0;
    unsigned int i;
    vector< unsigned int > aVertices( m_aVertices.size() );
    for(i=0;i<m_aVertices.size();i++)
        aVertices[i] = i;

    if (m_aVertices.size() == 0)
    {
        printf("  No vertices found...\n");
        return aVertices;
    }

    // cutoff
    Real32 fCutoff = 1.90f * 2.5f;
    Real32 fCutoffSq = fCutoff*fCutoff;

    printf("  Space Partitioning...\n");

    // calculate the minimum and maximum coordinates
    svt_vector4<Real64> oMin = getMinCoord();
    svt_vector4<Real64> oMax = getMaxCoord();

    Real32 fMinX = oMin.x();
    Real32 fMaxX = oMax.x();
    Real32 fMinY = oMin.y();
    Real32 fMaxY = oMax.y();
    Real32 fMinZ = oMin.z();
    Real32 fMaxZ = oMax.z();

    printf("  Max: ( %f, %f, %f ) - Min: ( %f, %f, %f )\n",fMaxX,fMaxY,fMaxZ,fMinX,fMinY,fMinZ);

    // pre-sorting of all vertices into boxes
    // how many boxes do we need?
    unsigned int iBoxesX = (unsigned int)(((fMaxX - fMinX) / fCutoff) + 0.5f) +1;
    unsigned int iBoxesY = (unsigned int)(((fMaxY - fMinY) / fCutoff) + 0.5f) +1;
    unsigned int iBoxesZ = (unsigned int)(((fMaxZ - fMinZ) / fCutoff) + 0.5f) +1;
    unsigned int iBoxesXY = iBoxesX * iBoxesY;

    printf("  Boxes: (%i,%i,%i)\n",iBoxesX,iBoxesY,iBoxesZ);

    // allocate memory
    vector< vector<unsigned int> > aBox( (iBoxesX+1)*(iBoxesY+1)*(iBoxesZ+1) );
    int iBoxX, iBoxY, iBoxZ, iBoxIndex;
    // now sort all vertices into these boxes
    for(i=0;i<m_aVertices.size();i++)
    {
        iBoxX = (int)(((m_aVertices[i].x() - fMinX) / fCutoff) + 0.5f);
        iBoxY = (int)(((m_aVertices[i].y() - fMinY) / fCutoff) + 0.5f);
        iBoxZ = (int)(((m_aVertices[i].z() - fMinZ) / fCutoff) + 0.5f);
        iBoxIndex = iBoxX + (iBoxY * iBoxesX) + (iBoxZ * iBoxesXY);
	aBox[iBoxIndex].push_back(i);
    }

    // now look for neighbors in these cubes
    vector< vector<unsigned int > > aNeighbor( m_aVertices.size() );
    unsigned int iIndex, iIndexNext, iVert, iNumElements;
    unsigned int j,x,y,z;

    try
    {
        if (bShowProgress)
            svt_exception::ui()->progressPopup("Cleaning mesh", 0, iBoxesZ-1);

        for(z=0;z<iBoxesZ;z++)
        {
            for(y=0;y<iBoxesY;y++)
                for(x=0;x<iBoxesX;x++)
                {
                    // calculate the index of the current block
                    iIndex = x+(y*iBoxesX)+(z*iBoxesXY);
                    // loop over all vertices in that block
                    for(i=0;i<aBox[iIndex].size();i++)
                    {
                        iVert = aBox[iIndex][i];

                        // search in same block for neighbors
                        for(j=i+1;j<aBox[iIndex].size();j++)
                        {
                            if (m_aVertices[iVert].distanceSq( m_aVertices[aBox[iIndex][j]] ) > fCutoffSq)
                                continue;
                            aNeighbor[iVert].push_back( aBox[iIndex][j] );
                        }
                        // search for neighbors in block x+1
                        if (x < iBoxesX-1)
                        {
                            iIndexNext = (x+1)+(y*iBoxesX)+(z*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if (m_aVertices[iVert].distanceSq( m_aVertices[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iVert].push_back( aBox[iIndexNext][j] );
                            }
                        }
                        // search for neighbors in block y+1
                        if (y < iBoxesY-1)
                        {
                            iIndexNext = x+((y+1)*iBoxesX)+(z*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( m_aVertices[iVert].distanceSq( m_aVertices[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iVert].push_back( aBox[iIndexNext][j] );
                            }
                        }
                        // search for neighbors in block z+1 block
                        if (z < iBoxesZ-1)
                        {
                            iIndexNext = x+(y*iBoxesX)+((z+1)*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( m_aVertices[iVert].distanceSq( m_aVertices[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iVert].push_back( aBox[iIndexNext][j] );
                            }
                        }
                        // search for neighbors in block x+1 and y+1
                        if (x < iBoxesX-1 && y < iBoxesY-1)
                        {
                            iIndexNext = (x+1)+((y+1)*iBoxesX)+(z*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( m_aVertices[iVert].distanceSq( m_aVertices[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iVert].push_back( aBox[iIndexNext][j] );
                            }
                        }
                        // search for neighbors in block x+1 and z+1
                        if (x < iBoxesX-1 && z < iBoxesZ-1)
                        {
                            iIndexNext = (x+1)+(y*iBoxesX)+((z+1)*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( m_aVertices[iVert].distanceSq( m_aVertices[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iVert].push_back( aBox[iIndexNext][j] );
                            }
                        }
                        // search for neighbors in block y+1 and z+1
                        if (y < iBoxesY-1 && z < iBoxesZ-1)
                        {
                            iIndexNext = x+((y+1)*iBoxesX)+((z+1)*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( m_aVertices[iVert].distanceSq( m_aVertices[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iVert].push_back( aBox[iIndexNext][j] );
                            }
                        }
                        // search for neighbors in block x+1, y-1
                        if (x < iBoxesX-1 && y > 0)
                        {
                            iIndexNext = (x+1)+((y-1)*iBoxesX)+(z*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( m_aVertices[iVert].distanceSq( m_aVertices[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iVert].push_back( aBox[iIndexNext][j] );
                            }
                        }

                        // search for neighbors in block x-1, z+1
                        if (x > 0 && z < iBoxesZ-1)
                        {
                            iIndexNext = (x-1)+(y*iBoxesX)+((z+1)*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( m_aVertices[iVert].distanceSq( m_aVertices[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iVert].push_back( aBox[iIndexNext][j] );
                            }
                        }
                        // search for neighbors in block z+1, y-1
                        if (y > 0 && z < iBoxesZ-1)
                        {
                            iIndexNext = x+((y-1)*iBoxesX)+((z+1)*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( m_aVertices[iVert].distanceSq( m_aVertices[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iVert].push_back( aBox[iIndexNext][j] );
                            }
                        }
                        // search for neighbors in block z+1, y+1, x+1 block
                        if (x < iBoxesX-1 && y < iBoxesY-1 && z < iBoxesZ-1)
                        {
                            iIndexNext = (x+1)+((y+1)*iBoxesX)+((z+1)*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( m_aVertices[iVert].distanceSq( m_aVertices[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iVert].push_back( aBox[iIndexNext][j] );
                            }
                        }
                        // search for neighbors in block x-1, y+1, z+1
                        if (x > 0 && y < iBoxesY-1 && z < iBoxesZ-1)
                        {
                            iIndexNext = (x-1)+((y+1)*iBoxesX)+((z+1)*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( m_aVertices[iVert].distanceSq( m_aVertices[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iVert].push_back( aBox[iIndexNext][j] );
                            }
                        }
                        // search for neighbors in block x+1, y-1, z+1
                        if (x < iBoxesX-1 && y > 0 && z < iBoxesZ-1)
                        {
                            iIndexNext = (x+1)+((y-1)*iBoxesX)+((z+1)*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( m_aVertices[iVert].distanceSq( m_aVertices[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iVert].push_back( aBox[iIndexNext][j] );
                            }
                        }
                        // search for neighbors in block x-1, y-1, z+1
                        if (x > 0 && y > 0 && z < iBoxesZ-1)
                        {
                            iIndexNext = (x-1)+((y-1)*iBoxesX)+((z+1)*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( m_aVertices[iVert].distanceSq( m_aVertices[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iVert].push_back( aBox[iIndexNext][j] );
                            }
                        }
                    }

                }

            if (bShowProgress)
                svt_exception::ui()->progress(z, iBoxesZ-1);
        }
        if (bShowProgress)
            svt_exception::ui()->progressPopdown();

    } catch (svt_userInterrupt&)
    {
    }

    try
    {
        if (bShowProgress)
            svt_exception::ui()->progressPopup("Cleaning mesh", 0, m_aVertices.size()-1);


        // now we have an neighbor array with all corresponding vertex pairs within a certain distance.
        Real32 fDist;
        int k, iNeighborNum;
        unsigned int iNeighborSum = 0;

        for(i=0;i<m_aVertices.size();i++)
        {
            iNeighborNum = aNeighbor[i].size();
            iNeighborSum += iNeighborNum;

            for(k=0;k<iNeighborNum;k++)
            {
                j = aNeighbor[i][k];

                fDist = m_aVertices[i].distance( m_aVertices[j] );

                // OK, is this now the vertex that we want to keep, or the one with the higher index, that should get removed?
                if (fDist < 0.000001 && i<j && aVertices[i] == i && aVertices[j] == j)
                {
                    aVertices[j] = i;
                    iDoubles++;
                }
            }

            if (i%1000 == 0)
                if (bShowProgress)
                    svt_exception::ui()->progress(i, m_aVertices.size()-1);
        }

        printf("  Found %i identical vertices (of %i)\n", iDoubles, (int)m_aVertices.size());

    } catch( svt_userInterrupt&) { }

    if (bShowProgress)
        svt_exception::ui()->progressPopdown();

    return aVertices;
}


/**
 * Reset the alias object - delete all triangles, vertices, normals and the matrix stack
 */
void svt_aliasobj::reset()
{
    while(!m_aStack.empty())
        m_aStack.pop();

    svt_matrix4<Real32> oMat;
    m_aStack.push( oMat );

    m_aVertices.clear();
    m_aFaces.clear();
    m_aNormals.clear();

    m_aColors.clear();
    m_aColorChange.clear();
    m_aColorChangePos.clear();
};
