/***************************************************************************
                          svt_cartoon.cpp
			  -------------------
    begin                : 11/16/2006
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_pdb.h>
#include <svt_time.h>
#include <svt_kb_spline.h>
#include <svt_cr_spline.h>
#include <svt_vector3.h>
#include <svt_container_cartoon.h>


void svt_pdb::calcNewTube( vector< int > * aCA )
{
    //unsigned long iTime = svt_getToD();

    int iOrdResSeq;
    int iOrdChainID;
    unsigned int i, j, iLength;
    unsigned int iResidueChange = (unsigned int)  m_iNewTubeSegs/2;
    svt_vector4<Real32> oL1, oL2, oL3, oL4;
    svt_vector4<Real32> aL[m_iNewTubeSegs];
    svt_vector3< Real32 > oN;
    svt_container_cartoon::v4f_c4f_t4f_n3f oV;
    vector<  int >::iterator itrCA    = aCA->begin();
    vector<  int >::iterator pCA      = aCA->begin();
    vector<  int >::iterator itrCAEnd = aCA->end();

    if (aCA->size() < 2)
    {
	//svtout << "NewTube: Cannot display a structure with only one C-alpha atom." << endl;
 	return;
    }

    while (true)
    {

	// arrived at end of molecule?
	if (itrCA == itrCAEnd)
	    break;

	iLength = 1;

	iOrdResSeq  = m_aRenAtoms[*itrCA].getOrdResidueSeq();
	iOrdChainID = m_aRenAtoms[*itrCA].getOrdChainID();
	pCA = itrCA;

	while (true)
	{
	    ++pCA;


	    // arrived at end of molecule?
	    if (pCA == itrCAEnd)
		break;


	    // is the ordinal residue sequence number ok?
	    if (++iOrdResSeq != m_aRenAtoms[*pCA].getOrdResidueSeq())
		break;


	    // are we still in the same chain?
	    if (m_aRenAtoms[*pCA].getOrdChainID() != iOrdChainID)
		break;


	    ++iLength;
	}

	//printf("%i\n", iLength);


	if (iLength < 2)
	{
	    //svtout << "Newtube: Cannot display a structure with only one C-alpha atom." << endl;
	    ++itrCA;
	    continue;
	}


	// simple case, only two c_alphas
	else if (iLength == 2)
	{
	    m_pCartoonCont->beginTube();


	    // set cartoon diameter
	    oV.v[3] = CARTOON_SCALING_FACTOR * m_fNewTubeDiameter;

	    // set color
	    memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);



	    // oL2 will be used as oL1 and oL3 will be used as oL4
	    oL2 = m_aRenAtoms[*itrCA].getGLPos();
	    ++itrCA;
	    oL3 = m_aRenAtoms[*itrCA].getGLPos();

	    m_oCRSpline.calcSplineQMatrix(oL2, oL2, oL3, oL3);
	    m_oCRSpline.calcSplinePoints( aL, m_iNewTubeSegs );


	    // create an arbitrary normal orthogonal to tangent aL[1] - aL[0]
	    oN = crossProduct(svt_vector3< Real32 >(aL[1] - aL[0]), svt_vector3< Real32 >(10.0f, 20.0f, 30.0f));
	    memcpy(oV.n, oN.c_data(), 12);



	    // now add segments
	    for (i=0; i<iResidueChange; ++i)
	    {
		memcpy(oV.v, aL[i].c_data(), 12);
		m_pCartoonCont->addSegment(&oV);
	    }
	    memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);
	    for (i=iResidueChange; i<m_iNewTubeSegs; ++i)
	    {
		memcpy(oV.v, aL[i].c_data(), 12);
		m_pCartoonCont->addSegment(&oV);
	    }


	    m_pCartoonCont->endTube();

	    ++itrCA;
	}


	// chain is longer than 2 residues
	else
	{
	    m_pCartoonCont->beginTube();


	    // set cartoon diameter
	    oV.v[3] = CARTOON_SCALING_FACTOR * m_fNewTubeDiameter;

	    // set color
	    memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);



	    // here, just assign the oLi's so that they can be used in the main loop below
	    oL2 = m_aRenAtoms[*itrCA].getGLPos();
	    oL3 = oL2;
	    oL4 = m_aRenAtoms[*(itrCA+1)].getGLPos();


	    ////////////////// main loop /////////////////////////

	    for (i=2; i<iLength; ++i)
	    {
		oL1 = oL2;
		oL2 = oL3;
		oL3 = oL4;
		oL4 = m_aRenAtoms[*(itrCA+2)].getGLPos();

		m_oCRSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
		m_oCRSpline.calcSplinePoints( aL, m_iNewTubeSegs );


		// if this is the beginning of the chain, create an arbitrary normal orthogonal to
		// tangent aL[1] - aL[0]
		if (i==2)
		{
		    oN = crossProduct(svt_vector3< Real32 >(aL[1] - aL[0]), svt_vector3< Real32 >(10.0f, 20.0f, 30.0f));
		    memcpy(oV.n, oN.c_data(), 12);
		}


		for (j=0; j<iResidueChange; ++j)
		{
		    memcpy(oV.v, aL[j].c_data(), 12);
		    m_pCartoonCont->addSegment(&oV);
		}
		++itrCA;
		memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);
		for (j=iResidueChange; j<m_iNewTubeSegs-1; ++j)
		{
		    memcpy(oV.v, aL[j].c_data(), 12);
		    m_pCartoonCont->addSegment(&oV);
		}
	    }


	    //  last residue has no tangent information at the end:

	    m_oCRSpline.calcSplineQMatrix(oL2, oL3, oL4, oL4);
	    m_oCRSpline.calcSplinePoints( aL, m_iNewTubeSegs );

	    for (j=0; j<iResidueChange; ++j)
	    {
		memcpy(oV.v, aL[j].c_data(), 12);
		m_pCartoonCont->addSegment(&oV);
	    }
	    ++itrCA;
	    memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);
	    for (j=iResidueChange; j<m_iNewTubeSegs; ++j)
	    {
		memcpy(oV.v, aL[j].c_data(), 12);
		m_pCartoonCont->addSegment(&oV);
	    }


	    m_pCartoonCont->endTube();

	    ++itrCA;
	}
    }



    // draw the peptide planes of the residues
    if (m_bCartoonPeptidePlanes)
    {
	svt_vector3<Real32> v1, v2, n;
	svt_c4ub_n3f_v3f p;

	m_pCont->beginTriangles();

	for (itrCA=(aCA->begin()); itrCA != (aCA->end()-1); ++itrCA)
	{
	    if (m_aRenAtoms[*itrCA].getOrdChainID() != m_aRenAtoms[*(itrCA+1)].getOrdChainID() ||
		m_aRenAtoms[*itrCA].getOrdResidueSeq()+1 != m_aRenAtoms[*(itrCA+1)].getOrdResidueSeq())
		continue;
	    
	    v1 = svt_vector3<Real32>(m_aRenAtoms[  *itrCA  ].getGLPos() - m_aRenAtoms[getBackboneO(*itrCA)].getGLPos() );
	    v2 = svt_vector3<Real32>(m_aRenAtoms[*(itrCA+1)].getGLPos() - m_aRenAtoms[getBackboneO(*itrCA)].getGLPos() );
	    n = crossProduct(v1, v2);
	    n.normalize();

            p.setC(m_aRenAtoms[  *itrCA  ].getColor()->getR()*255.0f, m_aRenAtoms[  *itrCA  ].getColor()->getG()*255.0f, m_aRenAtoms[  *itrCA  ].getColor()->getB()*255.0f);
            p.setN(n.x(), n.y(), n.z());
            p.setV(m_aRenAtoms[  *itrCA  ].getGLPos().x(),          m_aRenAtoms[  *itrCA  ].getGLPos().y(),          m_aRenAtoms[  *itrCA  ].getGLPos().z());
            m_pCont->addVertex(p);
            p.setV(m_aRenAtoms[  getBackboneO(*itrCA)   ].getGLPos().x(),          m_aRenAtoms[  getBackboneO(*itrCA)   ].getGLPos().y(),          m_aRenAtoms[  getBackboneO(*itrCA)   ].getGLPos().z());
            m_pCont->addVertex(p);
            p.setV(m_aRenAtoms[  *itrCA  ].getGLPos().x() + v2.x(), m_aRenAtoms[  *itrCA  ].getGLPos().y() + v2.y(), m_aRenAtoms[  *itrCA  ].getGLPos().z() + v2.z());
            m_pCont->addVertex(p);

            p.setC(m_aRenAtoms[*(itrCA+1)].getColor()->getR()*255.0f, m_aRenAtoms[*(itrCA+1)].getColor()->getG()*255.0f, m_aRenAtoms[*(itrCA+1)].getColor()->getB()*255.0f);
            p.setV(m_aRenAtoms[*(itrCA+1)].getGLPos().x(),          m_aRenAtoms[*(itrCA+1)].getGLPos().y(),          m_aRenAtoms[*(itrCA+1)].getGLPos().z());
            m_pCont->addVertex(p);
            p.setV(m_aRenAtoms[  getBackboneO(*itrCA)   ].getGLPos().x(),          m_aRenAtoms[  getBackboneO(*itrCA)   ].getGLPos().y(),          m_aRenAtoms[  getBackboneO(*itrCA)   ].getGLPos().z());
            m_pCont->addVertex(p);
            p.setV(m_aRenAtoms[*(itrCA+1)].getGLPos().x() + v1.x(), m_aRenAtoms[*(itrCA+1)].getGLPos().y() + v1.y(), m_aRenAtoms[*(itrCA+1)].getGLPos().z() + v1.z());
            m_pCont->addVertex(p);
	}
	m_pCont->endTriangles();
    }

    //printf("svt_pdb::calcNewTube() %lu\n", svt_getToD()-iTime);
}

/**
 * Calculate a cartoon
 * \param aCA reference to vector< int > with indices of all C_alpha atoms
 * \param aO reference to vector< int > with indices of all O atoms in the backbone
 * \param aSS reference to vector< char > with the secondary structure (H, S, C) of the C_alpha atoms
 */
void svt_pdb::calcCartoon( vector< int > * aCA, vector< char > * aSS  )
{
    //unsigned long iTime = svt_getToD();

    if (aCA->size() < 2)
    {
	//svtout << "Cartoon: Cannot display a structure with only one C-alpha atom." << endl;
 	return;
    }

    unsigned int i, j, iLength, iLen, iNumRemainder, iSkipBegin, iSkipEnd;
    char cStart, cCurrent, cPrev = 'C';
    int iOrdChainID;
    bool bChainBegin = true, bChainEnd = false;
    int iOrdResidueSeq;
    Real32 fW=0, fW1, fW2 = 1.0f;
    unsigned int iResidueChange = (unsigned int) m_iCartoonSegments/2;
    int k, l;
    svt_vector4<Real32> oL1, oL2, oL3, oL4;
    svt_vector4<Real32> aL[m_iCartoonSegments];
    svt_container_cartoon::v4f_c4f_t4f_n3f oV;
    svt_vector3< Real32 > oN, oV1, oV2, oV3, oN1, oN2, oO;

    m_oKBSpline.setTBC(0.0f, 0.0f, 0.0f);

    vector<  int >::iterator itrCA = aCA->begin();
    vector<  int >::iterator pCA;
    vector< char >::iterator itrSS_Start     = aSS->begin();
    vector< char >::iterator itrSS_End       = aSS->begin();
    vector< char >::iterator itrSS           = aSS->begin();
    vector< char >::iterator itrSS_VectorEnd = aSS->end();
    vector< char >::iterator itrSS_VectorBeg = aSS->begin();

    // this loop will set itrCA to the beginning of a secondary structure type.  it determines also
    // the length of the secondary structure, including a check for the minimal length of a helix
   // or a sheet. the resulting length is put into iLength. the structure type is stored in cStart
    while (true)
    {
	itrSS       = itrSS_End;
	itrSS_Start = itrSS_End;
	iLength = 0;
	cStart = *itrSS;
	iOrdChainID = m_aRenAtoms[*itrCA].getOrdChainID();
	pCA = itrCA;

	while (true)
	{
	    ++itrSS;
	    ++iLength;

	    if (itrSS == itrSS_VectorEnd)
	    {
		bChainEnd = true;
		break;
	    }

	    iOrdResidueSeq = m_aRenAtoms[*pCA].getOrdResidueSeq() + 1;
	    ++pCA;

	    if (m_aRenAtoms[*pCA].getOrdResidueSeq() != iOrdResidueSeq || m_aRenAtoms[*pCA].getOrdChainID() != iOrdChainID)
	    {
		bChainEnd = true;
		break;
	    }

	    if (*itrSS != cStart)
		break;
	}

	if ((cStart == 'H' && iLength < m_iMinResiduesPerHelix) || (cStart == 'S' && iLength < m_iMinResiduesPerSheet))
	    cStart = 'C';

	itrSS_End = itrSS;


	if (!bChainEnd)
	{
	    while (true)
	    {
		iLen = 0;
		cCurrent = *itrSS;

		while (true)
		{
		    ++itrSS;
		    ++iLen;

		    if (itrSS == itrSS_VectorEnd)
		    {
			bChainEnd = true;
			break;
		    }

		    iOrdResidueSeq = m_aRenAtoms[*pCA].getOrdResidueSeq() + 1;
		    ++pCA;

		    if (m_aRenAtoms[*pCA].getOrdResidueSeq() != iOrdResidueSeq || m_aRenAtoms[*pCA].getOrdChainID() != iOrdChainID)
		    {
			bChainEnd = true;
			break;
		    }

		    if (*itrSS != cCurrent)
			break;
		}


		if (cStart == 'C' && cCurrent != 'C')
		{
		    if ( (cCurrent == 'H' && iLen < m_iMinResiduesPerHelix) ||
			 (cCurrent == 'S' && iLen < m_iMinResiduesPerSheet)   )
		    {
			itrSS_End = itrSS;
			iLength += iLen;
			if (bChainEnd)
			    break;
			else
			    continue;
		    }
		    else
		    {
			bChainEnd = false;
			break;
		    }
		}

		if (cStart == 'C' && cCurrent == 'C')
		{
		    itrSS_End = itrSS;
		    iLength += iLen;
		    if (bChainEnd)
			break;
		    else
			continue;
		}
		bChainEnd = false;
		break;
	    }
	}


// 	printf("%c %2i: ", cStart, iLength);
// 	pCA = itrCA;
// 	for (itrSS = itrSS_Start; itrSS != itrSS_End; ++itrSS, ++pCA)
// 	{
// 	    printf("%c", m_aRenAtoms[*pCA].getSecStruct());
// 	    //printf("%2i", m_aRenAtoms[*pCA].getOrdResidueSeq());
// 	}
// 	printf("\n");



	// set color according to current residue
	memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);


	switch (cStart)
	{
	    case 'C': // CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
	    {

 		//itrCA += iLength; // skip coil for debugging
 		//break;

// 			glDisable(GL_LIGHTING);
// 			glPointSize(10.0f);
// 			glBegin(GL_POINTS);
// 			glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(oL1.x(), oL1.y(), oL1.z());
// 			glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(oL2.x(), oL2.y(), oL2.z());
// 			glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(oL3.x(), oL3.y(), oL3.z());
// 			glColor3f(1.0f, 1.0f, 1.0f); glVertex3f(oL4.x(), oL4.y(), oL4.z());
// 			glEnd();
// 			glEnable(GL_LIGHTING);


		// this means there is only a single c_alpha --> nothing to do
		if (iLength == 1 && bChainBegin && bChainEnd)
		{
		    ++itrCA;
		    break;
		}


		// set spline tension for random coil
		m_oKBSpline.setTension(0.0f);


		// set diameter of tube
		oV.v[3] = CARTOON_SCALING_FACTOR * m_fCartoonTubeDiameter;


		// begin newtube
		m_pCartoonCont->beginTube();


		if (iLength == 1) // (iLength == 1) (iLength == 1) (iLength == 1) (iLength == 1) (iLength == 1) (iLength == 1)
		{

		    // one-residue tubes look better with less tension
		    m_oKBSpline.setTension(-0.5f);


		    // this means a single c_alpha at the beginning of a chain
		    if (bChainBegin)
		    {

			// don't need to set oL1: calcSplineQMatrix(oL2, oL2, oL3, oL4)
			oL2 = m_aRenAtoms[*itrCA].getGLPos();
			oL3 = 0.5f * (m_aRenAtoms[*itrCA].getGLPos() + m_aRenAtoms[*(itrCA+1)].getGLPos());


			// set oL4 depending on next geometry. Since bChainEnd cannot be true here, this is safe
			if (*itrSS_End == 'S')
			{
			    // since minimum length for a sheet is two, we can safely access itrCA+2
			    oL4 = 0.5f * (m_aRenAtoms[*(itrCA+1)].getGLPos() + m_aRenAtoms[*(itrCA+2)].getGLPos());
			}
			else
			{
			    oL4 = m_aRenAtoms[*(itrCA+1)].getGLPos();
			}

			m_oKBSpline.calcSplineQMatrix(oL2, oL2, oL3, oL4);
			m_oKBSpline.calcSplinePoints( aL, iResidueChange+1 );


			// create an arbitrary normal orthogonal to tangent aL[1] - aL[0]
			oN = crossProduct(svt_vector3< Real32 >(aL[1] - aL[0]), svt_vector3< Real32 >(10.0f, 20.0f, 30.0f));
			memcpy(oV.n, oN.c_data(), 12);


			// add segments
			for (i=0; i<=iResidueChange; ++i)
			{
			    memcpy(oV.v, aL[i].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}


			++itrCA;
		    }
		    // this means a single c_alpha at the end of a longer chain. we can reuse the old oLi's
		    else if (bChainEnd)
		    {

			// just reuse previous oLi's. plus, they do not need to be set for the next
			// geometry because chain ends here
			m_oKBSpline.calcSplineQMatrix(oL2, oL3, oL4, oL4);
			m_oKBSpline.calcSplinePoints( aL, iResidueChange+1 );


			// create an arbitrary normal orthogonal to tangent aL[1] - aL[0]
			oN = crossProduct(svt_vector3< Real32 >(aL[1] - aL[0]), svt_vector3< Real32 >(10.0f, 20.0f, 30.0f));
			memcpy(oV.n, oN.c_data(), 12);


			// add segments
			for (i=0; i<=iResidueChange; ++i)
			{
			    memcpy(oV.v, aL[i].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}


			++itrCA;
		    }
		    else // here, we are in between two structures
		    {

			// reuse old oLi's where possible
			oL1 = oL2;
			oL2 = oL3;
			oL3 = oL4;
			oL4 = 0.5f * (m_aRenAtoms[*itrCA].getGLPos() + m_aRenAtoms[*(itrCA+1)].getGLPos());

			m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			m_oKBSpline.calcSplinePoints( aL, iResidueChange+1 );


			// create an arbitrary normal orthogonal to tangent aL[1] - aL[0]
			oN = crossProduct(svt_vector3< Real32 >(aL[1] - aL[0]), svt_vector3< Real32 >(10.0f, 20.0f, 30.0f));
			memcpy(oV.n, oN.c_data(), 12);

			for (i=0; i<iResidueChange; ++i)
			{
			    memcpy(oV.v, aL[i].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}

			++itrCA;


			// reuse old oLi's where possible. note: next geometry will use them (i.e. assume that they were set correctly)
			oL1 = oL2;
			oL2 = oL3;
			oL3 = oL4;

			if (*itrSS_End == 'S')
			    oL4 = 0.5f * (m_aRenAtoms[*itrCA].getGLPos() + m_aRenAtoms[*(itrCA+1)].getGLPos());
			else
			    oL4 = m_aRenAtoms[*itrCA].getGLPos();



			m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );

			for (i=0; i<m_iCartoonSegments; ++i)
			{
			    memcpy(oV.v, aL[i].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}
		    }
		}
		else if (iLength == 2) // (iLength == 2) (iLength == 2) (iLength == 2) (iLength == 2) (iLength == 2) (iLength == 2) (iLength == 2) (iLength == 2) (iLength == 2)
		{

		    // simplest case, only two c_alphas
		    if (bChainBegin && bChainEnd)
		    {
			// oL2 will be used as oL1 and oL3 will be used as oL4
			oL2 = m_aRenAtoms[*itrCA].getGLPos();
			oL3 = m_aRenAtoms[*(itrCA+1)].getGLPos();


			m_oKBSpline.calcSplineQMatrix(oL2, oL2, oL3, oL3);
			m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );

			// create an arbitrary normal orthogonal to tangent aL[1] - aL[0]
			oN = crossProduct(svt_vector3< Real32 >(aL[1] - aL[0]), svt_vector3< Real32 >(10.0f, 20.0f, 30.0f));
			memcpy(oV.n, oN.c_data(), 12);

			++itrCA;

			// now add segments
			for (i=0; i<iResidueChange; ++i)
			{
			    memcpy(oV.v, aL[i].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}
			memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);
			for (i=iResidueChange; i<m_iCartoonSegments; ++i)
			{
			    memcpy(oV.v, aL[i].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}

			++itrCA;
		    }

		    // this case: the beginning of the chain, and a sheet or helix will follow
		    else if (bChainBegin)
		    {

			// oL2 will be used as oL1, so don' tneed to assign oL1
			oL2 = m_aRenAtoms[*itrCA].getGLPos();
			oL3 = m_aRenAtoms[*(itrCA+1)].getGLPos();
			oL4 = 0.5f * (m_aRenAtoms[*(itrCA+1)].getGLPos() + m_aRenAtoms[*(itrCA+2)].getGLPos());

			m_oKBSpline.calcSplineQMatrix(oL2, oL2, oL3, oL4);
			m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );


			// create an arbitrary normal orthogonal to tangent aL[1] - aL[0]
			oN = crossProduct(svt_vector3< Real32 >(aL[1] - aL[0]), svt_vector3< Real32 >(10.0f, 20.0f, 30.0f));
			memcpy(oV.n, oN.c_data(), 12);


			++itrCA;


			// now add segments
			for (i=0; i<iResidueChange; ++i)
			{
			    memcpy(oV.v, aL[i].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}
			memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);
			for (i=iResidueChange; i<m_iCartoonSegments-1; ++i)
			{
			    memcpy(oV.v, aL[i].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}


			++itrCA;


			oL1 = oL2;
			oL2 = oL3;
			oL3 = oL4;
			if (*itrSS_End == 'S')
			    oL4 = 0.5f * (m_aRenAtoms[*itrCA].getGLPos() + m_aRenAtoms[*(itrCA+1)].getGLPos());
			else
			    oL4 = m_aRenAtoms[*itrCA].getGLPos();

			m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			m_oKBSpline.calcSplinePoints( aL, iResidueChange+1 );


			// now add segments
			for (i=0; i<=iResidueChange; ++i)
			{
			    memcpy(oV.v, aL[i].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}
		    }



                    // this case: the end of the chain
		    else if (bChainEnd)
		    {
			//reuse old oLi's
			oL1 = oL2;
			oL2 = oL3;
			oL3 = oL4;
			oL4 = m_aRenAtoms[*(itrCA+1)].getGLPos();

			m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );


			// create an arbitrary normal orthogonal to tangent aL[1] - aL[0]
			oN = crossProduct(svt_vector3< Real32 >(aL[1] - aL[0]), svt_vector3< Real32 >(10.0f, 20.0f, 30.0f));
			memcpy(oV.n, oN.c_data(), 12);

			for (i=0; i<m_iCartoonSegments-1; ++i)
			{
			    memcpy(oV.v, aL[i].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}

			++itrCA;

			oL1 = oL2;
			oL2 = oL3;
			oL3 = oL4;

			m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );


			for (i=0; i<iResidueChange; ++i)
			{
			    memcpy(oV.v, aL[i].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}
			memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);
			for (i=iResidueChange; i<m_iCartoonSegments; ++i)
			{
			    memcpy(oV.v, aL[i].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}

			++itrCA;
		    }
		    // this case: in between two other parts
 		    else
 		    {

			//reuse old oLi's
			oL1 = oL2;
			oL2 = oL3;
			oL3 = oL4;
			oL4 = m_aRenAtoms[*(itrCA+1)].getGLPos();

			m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			m_oKBSpline.calcSplinePoints( aL, iResidueChange+1 );


			// create an arbitrary normal orthogonal to tangent aL[1] - aL[0]
			oN = crossProduct(svt_vector3< Real32 >(aL[1] - aL[0]), svt_vector3< Real32 >(10.0f, 20.0f, 30.0f));
			memcpy(oV.n, oN.c_data(), 12);

			for (i=0; i<iResidueChange; ++i)
			{
			    memcpy(oV.v, aL[i].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}


			++itrCA;


			oL1 = oL2;
			oL2 = oL3;
			oL3 = oL4;
			oL4 = 0.5f * (m_aRenAtoms[*itrCA].getGLPos() + m_aRenAtoms[*(itrCA+1)].getGLPos());

			m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );

			for (j=0; j<iResidueChange; ++j)
			{
			    memcpy(oV.v, aL[j].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}
			memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);
			for (j=iResidueChange; j<m_iCartoonSegments-1; ++j)
			{
			    memcpy(oV.v, aL[j].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}


			++itrCA;


			oL1 = oL2;
			oL2 = oL3;
			oL3 = oL4;
			if (*itrSS_End == 'S')
			    oL4 = 0.5f * (m_aRenAtoms[*itrCA].getGLPos() + m_aRenAtoms[*(itrCA+1)].getGLPos());
			else
			    oL4 = m_aRenAtoms[*itrCA].getGLPos();

			m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			m_oKBSpline.calcSplinePoints( aL, iResidueChange+1 );

			// now add segments
			for (j=0; j<=iResidueChange; ++j)
			{
			    memcpy(oV.v, aL[j].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}
 		    }
		}
		else// (iLength >= 3) (iLength >= 3) (iLength >= 3) (iLength >= 3)  (iLength >= 3) (iLength >= 3) (iLength >= 3)  (iLength >= 3) (iLength >= 3)
		{

// 		    glDisable(GL_LIGHTING);
// 		    glPointSize(10.0f);
// 		    glBegin(GL_POINTS);
// 		    glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(oL1.x(), oL1.y(), oL1.z());
// 		    glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(oL2.x(), oL2.y(), oL2.z());
// 		    glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(oL3.x(), oL3.y(), oL3.z());
// 		    glColor3f(1.0f, 1.0f, 1.0f); glVertex3f(oL4.x(), oL4.y(), oL4.z());
// 		    glEnd();
// 		    glEnable(GL_LIGHTING);

		    if (bChainBegin)
		    {
			// here, just assign the oLi's so that they can be used in the main loop below
			oL2 = m_aRenAtoms[*itrCA].getGLPos();
			oL3 = oL2;
			oL4 = m_aRenAtoms[*(itrCA+1)].getGLPos();
		    }
		    else
		    {
			// here, a spline needs to be inserted that runs over the first half
			// residue, connected to the previous structure
			oL1 = oL2;
			oL2 = oL3;
			oL3 = oL4;
			oL4 = m_aRenAtoms[*(itrCA+1)].getGLPos();


			m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );


			// create an arbitrary normal orthogonal to tangent aL[1] - aL[0]
			oN = crossProduct(svt_vector3< Real32 >(aL[1] - aL[0]), svt_vector3< Real32 >(10.0f, 20.0f, 30.0f));
			memcpy(oV.n, oN.c_data(), 12);


			// now add segments
			for (i=0; i<iResidueChange; ++i)
			{
			    memcpy(oV.v, aL[i].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}
			memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);
			for (i=iResidueChange; i<m_iCartoonSegments-1; ++i)
			{
			    memcpy(oV.v, aL[i].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}

		    }

		    ++itrCA;


		    ////////////////// main loop /////////////////////////

		    for (i=2; i<iLength; ++i)
		    {
			oL1 = oL2;
			oL2 = oL3;
			oL3 = oL4;
			oL4 = m_aRenAtoms[*(itrCA+1)].getGLPos();

			m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );

			// need to create a normal if this is the beginning of the chain
			if (i==2)
			    if (bChainBegin)
			    {
				// create an arbitrary normal orthogonal to tangent aL[1] - aL[0]
				oN = crossProduct(svt_vector3< Real32 >(aL[1] - aL[0]), svt_vector3< Real32 >(10.0f, 20.0f, 30.0f));
				memcpy(oV.n, oN.c_data(), 12);
			    }

			for (j=0; j<iResidueChange; ++j)
			{
			    memcpy(oV.v, aL[j].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}
			memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);
			for (j=iResidueChange; j<m_iCartoonSegments-1; ++j)
			{
			    memcpy(oV.v, aL[j].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}

			++itrCA;

		    }

		    //////////////// main loop end ///////////////////////


		    // the one before the last residue
		    oL1 = oL2;
		    oL2 = oL3;
		    oL3 = oL4;
  		    if (!bChainEnd)
 			oL4 = 0.5f * (m_aRenAtoms[*itrCA].getGLPos() + m_aRenAtoms[*(itrCA+1)].getGLPos());


		    m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
		    m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );

		    for (j=0; j<iResidueChange; ++j)
		    {
			memcpy(oV.v, aL[j].c_data(), 12);
			m_pCartoonCont->addSegment(&oV);
		    }

		    memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);

		    for (j=iResidueChange; j<m_iCartoonSegments-1; ++j)
		    {
			memcpy(oV.v, aL[j].c_data(), 12);
			m_pCartoonCont->addSegment(&oV);
		    }

		    ++itrCA;


		    // if the chain does not end here, add the last (half a residue long) tube
		    if (!bChainEnd)
		    {
			// the tail of this tube part connecting toi the next structure (over half a residue)
			// need to check the type of the next structure to correctly set tangent information
			oL1 = oL2;
			oL2 = oL3;
			oL3 = oL4;
			if (*itrSS_End == 'S')
			    oL4 = 0.5f * (m_aRenAtoms[*itrCA].getGLPos() + m_aRenAtoms[*(itrCA+1)].getGLPos());
			else
			    oL4 = m_aRenAtoms[*itrCA].getGLPos();


			m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			m_oKBSpline.calcSplinePoints( aL, iResidueChange+1 );

			// since the tail is only one half residue, use only half of the segments
			for (j=0; j<=iResidueChange; ++j)
			{
			    memcpy(oV.v, aL[j].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}
		    }
		    // if the chain ends, just add the last segment
		    else
		    {
			memcpy(oV.v, aL[m_iCartoonSegments-1].c_data(), 12);
			m_pCartoonCont->addSegment(&oV);
		    }
		}

		m_pCartoonCont->endTube();
		break;
	    }
	    case 'H': // HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
	    {
		// we can assume that a helix has always at least three residues ( compare setCartoonParam(...) )

 		//itrCA+=iLength;
 		//break;

		memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);

		// draw helices as cylinders                /////////////////////////////  cylinders  /////////////////////////////
		//
		if (m_bCartoonHelixCylinder)
		{

		    if (iLength <=4)
		    {

			oV.v[3] = CARTOON_SCALING_FACTOR * m_fCartoonHelixWidth * 2.5f;
			memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);

			if (bChainBegin)
			    oL2 = m_aRenAtoms[* itrCA   ].getGLPos();
			else
			    oL2 = 0.5f * (m_aRenAtoms[*(itrCA)].getGLPos() + m_aRenAtoms[*(itrCA-1)].getGLPos());

			if (bChainEnd)
			    oL3 = m_aRenAtoms[*(itrCA+2)].getGLPos();
			else
			    oL3 = 0.5f * (m_aRenAtoms[*(itrCA+iLength-1)].getGLPos() + m_aRenAtoms[*(itrCA+iLength)].getGLPos());

			m_oCRSpline.calcSplineQMatrix(oL2, oL2, oL3, oL3);
			m_oCRSpline.calcSplinePoints( aL, m_iCartoonSegments );


			// create an arbitrary normal orthogonal to tangent aL[1] - aL[0]
			oN = crossProduct(svt_vector3< Real32 >(aL[1] - aL[0]), svt_vector3< Real32 >(10.0f, 20.0f, 30.0f));
			memcpy(oV.n, oN.c_data(), 12);


			m_pCartoonCont->beginTube();


			for (j=0; j<iResidueChange; ++j)
			{
			    memcpy(oV.v, aL[j].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}
			//memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);
			for (j=iResidueChange; j<m_iCartoonSegments; ++j)
			{
			    memcpy(oV.v, aL[j].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}


			m_pCartoonCont->endTube();

			// if some structure is following, we have to set oL2, oL3, and oL4 because they will be reused
			if (!bChainEnd)
			{
			    oL2 = m_aRenAtoms[*(itrCA+iLength-1)].getGLPos();
			    oL3 = 0.5f * (m_aRenAtoms[*(itrCA+iLength-1)].getGLPos() + m_aRenAtoms[*(itrCA+iLength)].getGLPos());
			    if (*itrSS_End == 'S')
				oL4 = 0.5f * (m_aRenAtoms[*(itrCA+iLength)].getGLPos() + m_aRenAtoms[*(itrCA+iLength+1)].getGLPos());
			    else
				oL4 = m_aRenAtoms[*(itrCA+iLength)].getGLPos();
			}


			itrCA += iLength;
		    }
		    // if the helix is shorter than 9 residues, just compute the axis for the cylinder from beginning to end
		    else if (iLength < 9)
		    {

			// save end coordinate of previous structure in oN1
			// (or first C_alpha if chain begins here)
			
			if (!bChainBegin)
			    oN1 = svt_vector3< Real32 >(oL3);
			else
			    oN1 = svt_vector3< Real32 >(m_aRenAtoms[*(itrCA)  ].getGLPos());


			// compute the axis
			oL2 = m_aRenAtoms[*(itrCA)  ].getGLPos()
			    + m_aRenAtoms[*(itrCA+1)].getGLPos()
			    + m_aRenAtoms[*(itrCA+2)].getGLPos();
			oL2 = oL2 / 3.0f;

			oL3 = m_aRenAtoms[*(itrCA+iLength-1)].getGLPos()
			    + m_aRenAtoms[*(itrCA+iLength-2)].getGLPos()
			    + m_aRenAtoms[*(itrCA+iLength-3)].getGLPos();
			oL3 = oL3 / 3.0f;

			oN = svt_vector3< Real32 >(oL3 - oL2);


			// to obtain a nice starting point for the cylinder, project oN1 (see above)
			// orthogonally onto the axis oN
			//P = A + {(AB * AP) / || AB ||^2} AB;

			fW1 = oN * svt_vector3< Real32 >(oN1 - svt_vector3< Real32 >(oL2));
			fW2 = oN.length() * oN.length();

			oL1 = oL2 + svt_vector4< Real32 >(( fW1 / fW2 ) * oN );



			// to obtain a nice END point for the cylinder, project beginning point of
			// next structure (or last C_alpha, if at end of chain) orthogonally onto
			// the axis oN

			if (!bChainEnd)
			    oN1 = 0.5f * svt_vector3< Real32 >(m_aRenAtoms[*(itrCA+iLength-1)].getGLPos() + m_aRenAtoms[*(itrCA+iLength)].getGLPos());
			else
			    oN1 = svt_vector3< Real32 >(m_aRenAtoms[*(itrCA+iLength-1)].getGLPos());


			fW1 = oN * svt_vector3< Real32 >(oN1 - svt_vector3< Real32 >(oL2));

			oL4 = oL2 + svt_vector4< Real32 >(( fW1 / fW2 ) * oN );


// 			glDisable(GL_LIGHTING);
// 			glPointSize(10.0f);
// 			glBegin(GL_POINTS);
// 			glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(oL1.x(), oL1.y(), oL1.z());
// 			glColor3f(1.0f, 1.0f, 1.0f); glVertex3f(oN1.x(), oN1.y(), oN1.z());
// 			glEnd();
// 			glEnable(GL_LIGHTING);



			// FIXME for now, cylinders have 2.5 times the diameter of m_fCartoonHelixWidth
			oV.v[3] = CARTOON_SCALING_FACTOR * m_fCartoonHelixWidth * 2.5f;
			memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);



			m_oCRSpline.calcSplineQMatrix(oL1, oL1, oL4, oL4);
			m_oCRSpline.calcSplinePoints( aL, m_iCartoonSegments );


			// create an arbitrary normal orthogonal to tangent aL[1] - aL[0]
			oN = crossProduct(svt_vector3< Real32 >(aL[1] - aL[0]), svt_vector3< Real32 >(10.0f, 20.0f, 30.0f));
			memcpy(oV.n, oN.c_data(), 12);



			m_pCartoonCont->beginTube();


			for (j=0; j<iResidueChange; ++j)
			{
			    memcpy(oV.v, aL[j].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}
			//memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);
			for (j=iResidueChange; j<m_iCartoonSegments; ++j)
			{
			    memcpy(oV.v, aL[j].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}


			m_pCartoonCont->endTube();



			// if some structure is following, we have to set oL2, oL3, and oL4 because they will be reused
			if (!bChainEnd)
			{
			    oL2 = m_aRenAtoms[*(itrCA+iLength-1)].getGLPos();
			    oL3 = 0.5f * (m_aRenAtoms[*(itrCA+iLength-1)].getGLPos() + m_aRenAtoms[*(itrCA+iLength)].getGLPos());
			    if (*itrSS_End == 'S')
				oL4 = 0.5f * (m_aRenAtoms[*(itrCA+iLength)].getGLPos() + m_aRenAtoms[*(itrCA+iLength+1)].getGLPos());
			    else
				oL4 = m_aRenAtoms[*(itrCA+iLength)].getGLPos();
			}

// 			    glDisable(GL_LIGHTING);
// 			    glPointSize(10.0f);
// 			    glBegin(GL_POINTS);
// 			    //glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(oL1.x(), oL1.y(), oL1.z());
// 			    glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(oL2.x(), oL2.y(), oL2.z());
// 			    glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(oL3.x(), oL3.y(), oL3.z());
// 			    glColor3f(1.0f, 1.0f, 1.0f); glVertex3f(oL4.x(), oL4.y(), oL4.z());
// 			    glEnd();
// 			    glEnable(GL_LIGHTING);

			itrCA+=iLength;
		    }

		    // if the helix is longer, just compute the axis for the cylinder from beginning to end
		    else
		    {

			iNumRemainder = iLength % 3;

			iLen = iLength / 3;

			pCA = itrCA;

			

			// save end coordinate of previous structure in oN1
			// (or first C_alpha if chain begins here)
			
			if (!bChainBegin)
			    oN1 = svt_vector3< Real32 >(oL3);
			else
			    oN1 = svt_vector3< Real32 >(m_aRenAtoms[*(itrCA)  ].getGLPos());


			// compute the axis
			oL2 = m_aRenAtoms[*(itrCA)  ].getGLPos()
			    + m_aRenAtoms[*(itrCA+1)].getGLPos()
			    + m_aRenAtoms[*(itrCA+2)].getGLPos();
			oL2 = oL2 / 3.0f;

			oL3 = m_aRenAtoms[*(itrCA+iLength-1)].getGLPos()
			    + m_aRenAtoms[*(itrCA+iLength-2)].getGLPos()
			    + m_aRenAtoms[*(itrCA+iLength-3)].getGLPos();
			oL3 = oL3 / 3.0f;

			oN = svt_vector3< Real32 >(oL3 - oL2);


			// to obtain a nice starting point for the cylinder, project oN1 (see above)
			// orthogonally onto the axis oN
			//P = A + {(AB * AP) / || AB ||^2} AB;

			fW1 = oN * svt_vector3< Real32 >(oN1 - svt_vector3< Real32 >(oL2));
			fW2 = oN.length() * oN.length();

			oL2 = oL2 + svt_vector4< Real32 >(( fW1 / fW2 ) * oN );
			oL3 = oL2;
			
			if (iNumRemainder > 0)
			    pCA = pCA + 4;
			else
			    pCA = pCA + 3;


			// FIXME for now, cylinders have 2.5 times the diameter of m_fCartoonHelixWidth
			oV.v[3] = CARTOON_SCALING_FACTOR * m_fCartoonHelixWidth * 2.5f;
			memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);




			m_pCartoonCont->beginTube();


			for (i=2; i<iLen; ++i)
			{
			    oL1 = oL2;
			    oL2 = oL3;

			    oL3 = m_aRenAtoms[*(pCA)  ].getGLPos()
				+ m_aRenAtoms[*(pCA+1)].getGLPos()
				+ m_aRenAtoms[*(pCA+2)].getGLPos();
			    oL3 = oL3 / 3.0f;

			    oL4 = m_aRenAtoms[*(pCA+3)].getGLPos()
				+ m_aRenAtoms[*(pCA+4)].getGLPos()
				+ m_aRenAtoms[*(pCA+5)].getGLPos();
			    oL4 = oL4 / 3.0f;

			    m_oCRSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			    m_oCRSpline.calcSplinePoints( aL, m_iCartoonSegments );

			    if (i==2)
			    {
				// create an arbitrary normal orthogonal to tangent aL[1] - aL[0]
				oN = crossProduct(svt_vector3< Real32 >(aL[1] - aL[0]), svt_vector3< Real32 >(10.0f, 20.0f, 30.0f));
				memcpy(oV.n, oN.c_data(), 12);
			    }

			    if (i == iLen-2)
			    {
				if (iNumRemainder == 2)
				    ++pCA;
			    }


			    for (j=0; j<iResidueChange; ++j)
			    {
				memcpy(oV.v, aL[j].c_data(), 12);
				m_pCartoonCont->addSegment(&oV);
			    }
			    //memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);
			    for (j=iResidueChange; j<m_iCartoonSegments-1; ++j)
			    {
				memcpy(oV.v, aL[j].c_data(), 12);
				m_pCartoonCont->addSegment(&oV);
			    }

			    pCA = pCA + 3;
			}


			oL1 = oL2;
			oL2 = oL3;
			oL3 = oL4;

			oN = svt_vector3< Real32 >(oL3 - oL2);

			// to obtain a nice END point for the cylinder, project beginning point of
			// next structure (or last C_alpha, if at end of chain) orthogonally onto
			// the axis oN

			if (!bChainEnd)
			    oN1 = 0.5f * svt_vector3< Real32 >(m_aRenAtoms[*(itrCA+iLength-1)].getGLPos() + m_aRenAtoms[*(itrCA+iLength)].getGLPos());
			else
			    oN1 = svt_vector3< Real32 >(m_aRenAtoms[*(itrCA+iLength-1)].getGLPos());


			fW1 = oN * svt_vector3< Real32 >(oN1 - svt_vector3< Real32 >(oL3));
			fW2 = oN.length() * oN.length();

			oL3 = oL3 + svt_vector4< Real32 >(( fW1 / fW2 ) * oN );


// 			glDisable(GL_LIGHTING);
// 			glPointSize(10.0f);
// 			glBegin(GL_POINTS);
// 			glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(oL1.x(), oL1.y(), oL1.z());
// 			glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(oL2.x(), oL2.y(), oL2.z());
// 			glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(oL3.x(), oL3.y(), oL3.z());
// 			//glColor3f(1.0f, 1.0f, 1.0f); glVertex3f(oL4.x(), oL4.y(), oL4.z());
// 			glEnd();
// 			glEnable(GL_LIGHTING);


			m_oCRSpline.calcSplineQMatrix(oL1, oL2, oL3, oL3);
			m_oCRSpline.calcSplinePoints( aL, m_iCartoonSegments );


			for (j=0; j<iResidueChange; ++j)
			{
			    memcpy(oV.v, aL[j].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}
			//memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);
			for (j=iResidueChange; j<m_iCartoonSegments; ++j)
			{
			    memcpy(oV.v, aL[j].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}


			m_pCartoonCont->endTube();


			// if some structure is following, we have to set oL2, oL3, and oL4 because they will be reused
			if (!bChainEnd)
			{
			    oL2 = m_aRenAtoms[*(itrCA+iLength-1)].getGLPos();
			    oL3 = 0.5f * (m_aRenAtoms[*(itrCA+iLength-1)].getGLPos() + m_aRenAtoms[*(itrCA+iLength)].getGLPos());
			    if (*itrSS_End == 'S')
				oL4 = 0.5f * (m_aRenAtoms[*(itrCA+iLength)].getGLPos() + m_aRenAtoms[*(itrCA+iLength+1)].getGLPos());
			    else
				oL4 = m_aRenAtoms[*(itrCA+iLength)].getGLPos();
			}

			itrCA+=iLength;
		    }
		}

		// draw helices as helices (not cylinders) /////////////////////////////////      helices      ////////////////////////
		else
		{

		    m_oKBSpline.setTension(-1.0f);
		    oV.v[3] = CARTOON_SCALING_FACTOR * m_fCartoonHelixWidth;

		    if (iLength < 9)
		    {
			m_pCartoonCont->beginHelix();

			if (iLength > 5)
			{
			    oV1 = svt_vector3< Real32 >(  m_aRenAtoms[*(itrCA)  ].getGLPos()
							+ m_aRenAtoms[*(itrCA+1)].getGLPos()
							+ m_aRenAtoms[*(itrCA+2)].getGLPos());

			    oV2 = svt_vector3< Real32 >(  m_aRenAtoms[*(itrCA+iLength-1)].getGLPos()
							+ m_aRenAtoms[*(itrCA+iLength-2)].getGLPos()
							+ m_aRenAtoms[*(itrCA+iLength-3)].getGLPos());
		    
			    oN = oV2 - oV1;
			}
			else
			{
			    if (bChainBegin)
				oV1 = svt_vector3< Real32 >(m_aRenAtoms[* itrCA   ].getGLPos());
			    else
				oV1 = 0.5f * svt_vector3< Real32 >(m_aRenAtoms[*(itrCA)].getGLPos() + m_aRenAtoms[*(itrCA-1)].getGLPos());

			    if (bChainEnd)
				oV2 = svt_vector3< Real32 >(m_aRenAtoms[*(itrCA+iLength-1)].getGLPos());
			    else
				oV2 = 0.5f * svt_vector3< Real32 >(m_aRenAtoms[*(itrCA+iLength-1)].getGLPos() + m_aRenAtoms[*(itrCA+iLength)].getGLPos());

			    oN = svt_vector3< Real32 >(oV2 - oV1);
			}

			memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);

			memcpy(oV.n, oN.c_data(), 12);


			// glue to a previous structure
			//if (cPrev == 'S' && !bChainBegin)
			if (!bChainBegin)
			{
			    oL1 = oL2;
			    oL2 = oL3;
			    oL3 = oL4;
			    oL4 = m_aRenAtoms[*(itrCA+1)].getGLPos();

			    m_oKBSpline.setTBC(-0.25f, 0.0f, -0.5f);
			    m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			    m_oKBSpline.calcSplinePoints( aL, iResidueChange+1 );

			    m_oKBSpline.setTBC(-1.0f, 0.0f, 0.0f);

			    for (i=0; i<iResidueChange; ++i)
			    {
				memcpy(oV.v, aL[i].c_data(), 12);
				m_pCartoonCont->addSegment(&oV);
			    }
			}

			// first residue
			oL2 = m_aRenAtoms[*itrCA].getGLPos();
			if (bChainBegin)
			    oL1 = oL2;
			else
			    oL1 = m_aRenAtoms[*(itrCA-1)].getGLPos();
			++itrCA;
			oL3 = m_aRenAtoms[*itrCA].getGLPos();
			oL4 = m_aRenAtoms[*(itrCA+1)].getGLPos();

			m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );

			for (i=0; i<iResidueChange; ++i)
			{
			    memcpy(oV.v, aL[i].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}
			memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);
			for (i=iResidueChange; i<m_iCartoonSegments-1; ++i)
			{
			    memcpy(oV.v, aL[i].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}


			// middle residue(s)
			for (i = 1; i < iLength-2; ++i)
			{
			    oL1 = m_aRenAtoms[*(itrCA-1)].getGLPos();
			    oL2 = m_aRenAtoms[  *itrCA  ].getGLPos();
			    ++itrCA;
			    oL3 = m_aRenAtoms[  *itrCA  ].getGLPos();
			    oL4 = m_aRenAtoms[*(itrCA+1)].getGLPos();

			    m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			    m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );

			    for (j=0; j<iResidueChange; ++j)
			    {
				memcpy(oV.v, aL[j].c_data(), 12);
				m_pCartoonCont->addSegment(&oV);
			    }
			    memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);
			    for (j=iResidueChange; j<m_iCartoonSegments-1; ++j)
			    {
				memcpy(oV.v, aL[j].c_data(), 12);
				m_pCartoonCont->addSegment(&oV);
			    }
			}


			// third residue
			oL1 = m_aRenAtoms[*(itrCA-1)].getGLPos();
			oL2 = m_aRenAtoms[  *itrCA  ].getGLPos();
			++itrCA;
			oL3 = m_aRenAtoms[  *itrCA  ].getGLPos();
			if (bChainEnd)
			    oL4 = oL3;
			else
			    oL4 = 0.5f * (m_aRenAtoms[  *itrCA  ].getGLPos() + m_aRenAtoms[  *(itrCA+1)  ].getGLPos());

			m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );

			for (i=0; i<iResidueChange; ++i)
			{
			    memcpy(oV.v, aL[i].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}
			memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);

			// if the chain does not end here and needs an arrowhead, we need to add an additional segment with a greated width where the arrowhead begins
			if (m_bCartoonHelixArrowhead && bChainEnd)
			{
			    // compute fW, that is substracted repeatedly from the helix width to create an arrowhead
			    fW = (oV.v[3] * 1.5f) / (m_iCartoonSegments-iResidueChange);
			    oV.v[3] *= 1.5f;

			    // add an intermediate vertex; here the arrowhead begins (the width is greater here)
			    oV.v[0] = 0.99f * aL[iResidueChange-1].x() + 0.01f * aL[iResidueChange].x();
			    oV.v[1] = 0.99f * aL[iResidueChange-1].y() + 0.01f * aL[iResidueChange].y();
			    oV.v[2] = 0.99f * aL[iResidueChange-1].z() + 0.01f * aL[iResidueChange].z();

			    m_pCartoonCont->addSegment(&oV);
			}

			for (i=iResidueChange; i<m_iCartoonSegments-1; ++i)
			{
			    if (m_bCartoonHelixArrowhead && bChainEnd)
				oV.v[3] = oV.v[3] - fW;

			    memcpy(oV.v, aL[i].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}



			if (bChainEnd)
			{
			    if (m_bCartoonHelixArrowhead)
				oV.v[3] = oV.v[3] - fW;

			    memcpy(oV.v, aL[i].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}
			else
			{
			    oL1 = oL2;
			    oL2 = oL3;
			    oL3 = oL4;
			    oL4 = m_aRenAtoms[*(itrCA+1)].getGLPos();


			    m_oKBSpline.setTBC(-0.5f, 0.0f, -0.5f);

			    m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			    m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );

			    m_oKBSpline.setContinuity(0.0f);

			    if (m_bCartoonHelixArrowhead)
			    {
				// add this first segment here, because for an arrowhead beginning, there will be a second vertex with greater width
				memcpy(oV.v, aL[0].c_data(), 12);
				m_pCartoonCont->addSegment(&oV);


				// compute fW, that is substracted repeatedly from the helix width to create an arrowhead
				fW = (oV.v[3] * 1.5f) / (m_iCartoonSegments-1);
				oV.v[3] *= 1.5f;

				// add an intermediate vertex; here the arrowhead begins (the width is greater here)
				oV.v[0] = 0.99f * aL[0].x() + 0.01f * aL[1].x();
				oV.v[1] = 0.99f * aL[0].y() + 0.01f * aL[1].y();
				oV.v[2] = 0.99f * aL[0].z() + 0.01f * aL[1].z();

				m_pCartoonCont->addSegment(&oV);

				for (j=1; j<m_iCartoonSegments; ++j)
				{
				    memcpy(oV.v, aL[j].c_data(), 12);
				    oV.v[3] = oV.v[3] - fW;
				    m_pCartoonCont->addSegment(&oV);
				}
			    }
			    else
			    {
				for (j=0; j<m_iCartoonSegments; ++j)
				{
				    memcpy(oV.v, aL[j].c_data(), 12);
				    m_pCartoonCont->addSegment(&oV);
				}
			    }
			}

			m_pCartoonCont->endHelix();

			++itrCA;
		    }
		    else // (iLength >= 9)
		    {
			// normals are computed by computing vectors from the "average" positions of
			// adjacent C_alpha triples. if there are one or two additional C_alphas
			// they are ignored (normal doesn't change over these residues)

			iNumRemainder = iLength % 3;

			if (iNumRemainder == 1)
			{
			    iSkipBegin = 1;
			    iSkipEnd = 0;
			}
			else if (iNumRemainder == 2)
			{
			    iSkipBegin = 1;
			    iSkipEnd = 1;
			}
			else
			{
			    iSkipBegin = 0;
			    iSkipEnd = 0;
			}


			oV1 = svt_vector3< Real32 >(  m_aRenAtoms[*(itrCA  )].getGLPos()
						    + m_aRenAtoms[*(itrCA+1)].getGLPos()
						    + m_aRenAtoms[*(itrCA+2)].getGLPos());

//#define HELIX_AXIS
#ifdef HELIX_AXIS
			glDisable(GL_LIGHTING);
			glLineWidth(4.0f);
			glBegin(GL_LINE_STRIP);
			glColor3f(1.0f, 0.0f, 0.0f);

			oV1 = oV1 / 3.0f;

			glVertex3f(oV1.x(), oV1.y(), oV1.z());
			glVertex3f(m_aRenAtoms[*(itrCA  )].getGLPos().x(), m_aRenAtoms[*(itrCA  )].getGLPos().y(), m_aRenAtoms[*(itrCA  )].getGLPos().z());
			glVertex3f(oV1.x(), oV1.y(), oV1.z());
			glVertex3f(m_aRenAtoms[*(itrCA+1)].getGLPos().x(), m_aRenAtoms[*(itrCA+1)].getGLPos().y(), m_aRenAtoms[*(itrCA+1)].getGLPos().z());
			glVertex3f(oV1.x(), oV1.y(), oV1.z());
			glVertex3f(m_aRenAtoms[*(itrCA+2)].getGLPos().x(), m_aRenAtoms[*(itrCA+2)].getGLPos().y(), m_aRenAtoms[*(itrCA+2)].getGLPos().z());
			glVertex3f(oV1.x(), oV1.y(), oV1.z());
#endif

			if (iLength == 11)
			{
			    oV2 = svt_vector3< Real32 >(  m_aRenAtoms[*(itrCA+4)].getGLPos()
							+ m_aRenAtoms[*(itrCA+5)].getGLPos()
							+ m_aRenAtoms[*(itrCA+6)].getGLPos());

			    oV3 = svt_vector3< Real32 >(  m_aRenAtoms[*(itrCA+8)].getGLPos()
							+ m_aRenAtoms[*(itrCA+9)].getGLPos()
							+ m_aRenAtoms[*(itrCA+10)].getGLPos());
			}
			else
			{
			    oV2 = svt_vector3< Real32 >(  m_aRenAtoms[*(itrCA + (3+iSkipBegin))].getGLPos()
							+ m_aRenAtoms[*(itrCA + (4+iSkipBegin))].getGLPos()
							+ m_aRenAtoms[*(itrCA + (5+iSkipBegin))].getGLPos());

			    oV3 = svt_vector3< Real32 >(  m_aRenAtoms[*(itrCA + (6+iSkipBegin))].getGLPos()
							+ m_aRenAtoms[*(itrCA + (7+iSkipBegin))].getGLPos()
							+ m_aRenAtoms[*(itrCA + (8+iSkipBegin))].getGLPos());
			}

#ifdef HELIX_AXIS
			oV2 = oV2 / 3.0f;
			oV3 = oV3 / 3.0f;

			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(oV2.x(), oV2.y(), oV2.z());
			glVertex3f(m_aRenAtoms[*(itrCA+(3+iSkipBegin))].getGLPos().x(), m_aRenAtoms[*(itrCA+(3+iSkipBegin))].getGLPos().y(), m_aRenAtoms[*(itrCA+(3+iSkipBegin))].getGLPos().z());
			glVertex3f(oV2.x(), oV2.y(), oV2.z());
			glVertex3f(m_aRenAtoms[*(itrCA+(4+iSkipBegin))].getGLPos().x(), m_aRenAtoms[*(itrCA+(4+iSkipBegin))].getGLPos().y(), m_aRenAtoms[*(itrCA+(4+iSkipBegin))].getGLPos().z());
			glVertex3f(oV2.x(), oV2.y(), oV2.z());
			glVertex3f(m_aRenAtoms[*(itrCA+(5+iSkipBegin))].getGLPos().x(), m_aRenAtoms[*(itrCA+(5+iSkipBegin))].getGLPos().y(), m_aRenAtoms[*(itrCA+(5+iSkipBegin))].getGLPos().z());
			glVertex3f(oV2.x(), oV2.y(), oV2.z());

			if (iLength == 11) ++iSkipBegin;
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3f(oV3.x(), oV3.y(), oV3.z());
			glVertex3f(m_aRenAtoms[*(itrCA+(6+iSkipBegin))].getGLPos().x(), m_aRenAtoms[*(itrCA+(6+iSkipBegin))].getGLPos().y(), m_aRenAtoms[*(itrCA+(6+iSkipBegin))].getGLPos().z());
			glVertex3f(oV3.x(), oV3.y(), oV3.z());
			glVertex3f(m_aRenAtoms[*(itrCA+(7+iSkipBegin))].getGLPos().x(), m_aRenAtoms[*(itrCA+(7+iSkipBegin))].getGLPos().y(), m_aRenAtoms[*(itrCA+(7+iSkipBegin))].getGLPos().z());
			glVertex3f(oV3.x(), oV3.y(), oV3.z());
			glVertex3f(m_aRenAtoms[*(itrCA+(8+iSkipBegin))].getGLPos().x(), m_aRenAtoms[*(itrCA+(8+iSkipBegin))].getGLPos().y(), m_aRenAtoms[*(itrCA+(8+iSkipBegin))].getGLPos().z());
			glVertex3f(oV3.x(), oV3.y(), oV3.z());
			glColor3f(1.0f, 1.0f, 1.0f);
			if (iLength == 11) --iSkipBegin;
#endif


			oN1 = oV2 - oV1;
			oN2 = oV3 - oV2;


			m_pCartoonCont->beginHelix();


			// copy the normal information for the beginning of the chain
			memcpy(oV.n, oN1.c_data(), 12);


			// glue to a previous structure
			if (!bChainBegin)
			{
			    oL1 = oL2;
			    oL2 = oL3;
			    oL3 = oL4;
			    oL4 = m_aRenAtoms[*(itrCA+1)].getGLPos();

			    m_oKBSpline.setTBC(-0.25f, 0.0f, -0.5f);

			    m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			    m_oKBSpline.calcSplinePoints( aL, iResidueChange+1 );

			    m_oKBSpline.setTBC(-1.0f, 0.0f, 0.0f);

			    for (i=0; i<iResidueChange; ++i)
			    {
				memcpy(oV.v, aL[i].c_data(), 12);
				m_pCartoonCont->addSegment(&oV);
			    }
			}
			else
			{
			    // these will be used in the following loop. if at the beginning of a chain,
			    // no tangent information. if not, use the previous c_alpha
			    oL3 = m_aRenAtoms[*itrCA].getGLPos();

			    if (bChainBegin)
				oL2 = oL3;
			    else
				oL2 = m_aRenAtoms[*(itrCA-1)].getGLPos();

			    oL4 = m_aRenAtoms[*(itrCA+1)].getGLPos();
			}


			// first of the first three (or four) residues
			for (i=0; i<(1+iSkipBegin); ++i)
			{
			    oL1 = oL2;
			    oL2 = oL3;
			    oL3 = oL4;
			    ++itrCA;
			    oL4 = m_aRenAtoms[*(itrCA+1)].getGLPos();

			    m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			    m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );

			    for (j=0; j<iResidueChange; ++j)
			    {
				memcpy(oV.v, aL[j].c_data(), 12);
				m_pCartoonCont->addSegment(&oV);
			    }
			    memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);
			    for (j=iResidueChange; j<m_iCartoonSegments-1; ++j)
			    {
				memcpy(oV.v, aL[j].c_data(), 12);
				m_pCartoonCont->addSegment(&oV);
			    }
			}
			// rest of the first three (or four) residues
			fW1 = 3.0f * (((Real32)m_iCartoonSegments) - 1.0f);
			fW2 = 1.0f;
			for (i=0; i<2; ++i)
			{
			    oL1 = oL2;
			    oL2 = oL3;
			    oL3 = oL4;
			    ++itrCA;
			    oL4 = m_aRenAtoms[*(itrCA+1)].getGLPos();

			    m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			    m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );

			    for (j=0; j<iResidueChange; ++j)
			    {
				if (i>0)
				{
				    //printf("fW1: %3.0f fW2: %3.0f\n", fW1, fW2);
				    oN = fW1-- * oN1 + fW2++ * oN2;
				    memcpy(oV.n, oN.c_data(), 12);
				}
				memcpy(oV.v, aL[j].c_data(), 12);
				m_pCartoonCont->addSegment(&oV);
			    }
			    memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);
			    for (j=iResidueChange; j<m_iCartoonSegments-1; ++j)
			    {
				//printf("fW1: %3.0f fW2: %3.0f\n", fW1, fW2);
				oN = fW1-- * oN1 + fW2++ * oN2;
				memcpy(oV.n, oN.c_data(), 12);
				memcpy(oV.v, aL[j].c_data(), 12);
				m_pCartoonCont->addSegment(&oV);
			    }
			}


		    
			// k is the number of residues whose geometry will be computed in the following loop
			k = (int)(iLength - iNumRemainder) - 9;

			for (l = 0; l < k; ++l)
			{
			    oL1 = oL2;
			    oL2 = oL3;
			    oL3 = oL4;
			    ++itrCA;
			    oL4 = m_aRenAtoms[*(itrCA+1)].getGLPos();

			    m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			    m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );

			    for (j=0; j<iResidueChange; ++j)
			    {
				//printf("fW1: %3.0f fW2: %3.0f\n", fW1, fW2);
				oN = fW1-- * oN1 + fW2++ * oN2;
				memcpy(oV.n, oN.c_data(), 12);
				memcpy(oV.v, aL[j].c_data(), 12);
				m_pCartoonCont->addSegment(&oV);
			    }
			    memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);

			    // in the middle between two averaged c_alpha triples, the previous
			    // normal is forgotten and the next normal comes into play
			    if ((l+2)%3 == 0)
			    {
				oN1 = oN2;
				oV2 = oV3;

				if (l==k-2)
				    i=iSkipEnd;
				else
				    i=0;

				oV3 = svt_vector3< Real32 >(  m_aRenAtoms[*(itrCA+4+i)].getGLPos()
							    + m_aRenAtoms[*(itrCA+5+i)].getGLPos()
							    + m_aRenAtoms[*(itrCA+6+i)].getGLPos());
#ifdef HELIX_AXIS
				oV3 = oV3 / 3.0f;
				
				glVertex3f(oV3.x(), oV3.y(), oV3.z());
				glVertex3f(m_aRenAtoms[*(itrCA+4+i)].getGLPos().x(), m_aRenAtoms[*(itrCA+4+i)].getGLPos().y(), m_aRenAtoms[*(itrCA+4+i)].getGLPos().z());
				glVertex3f(oV3.x(), oV3.y(), oV3.z());
				glVertex3f(m_aRenAtoms[*(itrCA+5+i)].getGLPos().x(), m_aRenAtoms[*(itrCA+5+i)].getGLPos().y(), m_aRenAtoms[*(itrCA+5+i)].getGLPos().z());
				glVertex3f(oV3.x(), oV3.y(), oV3.z());
				glVertex3f(m_aRenAtoms[*(itrCA+6+i)].getGLPos().x(), m_aRenAtoms[*(itrCA+6+i)].getGLPos().y(), m_aRenAtoms[*(itrCA+6+i)].getGLPos().z());
				glVertex3f(oV3.x(), oV3.y(), oV3.z());
#endif
				oN2 = oV3 - oV2;

				fW1 = 3.0f * (((Real32)m_iCartoonSegments) - 1.0f);
				fW2 = 1.0f;
			    }

			    for (j=iResidueChange; j<m_iCartoonSegments-1; ++j)
			    {
				//printf("fW1: %3.0f fW2: %3.0f\n", fW1, fW2);
				oN = fW1-- * oN1 + fW2++ * oN2;
				memcpy(oV.n, oN.c_data(), 12);
				memcpy(oV.v, aL[j].c_data(), 12);
				m_pCartoonCont->addSegment(&oV);
			    }
			}


			for (i=0; i<2; ++i)
			{
			    oL1 = oL2;
			    oL2 = oL3;
			    oL3 = oL4;
			    ++itrCA;
			    oL4 = m_aRenAtoms[*(itrCA+1)].getGLPos();

			    m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			    m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );

			    for (j=0; j<iResidueChange; ++j)
			    {
				//printf("fW1: %3.0f fW2: %3.0f\n", fW1, fW2);
				oN = fW1-- * oN1 + fW2++ * oN2;
				memcpy(oV.n, oN.c_data(), 12);
				memcpy(oV.v, aL[j].c_data(), 12);
				m_pCartoonCont->addSegment(&oV);
			    }
			    memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);
			    for (j=iResidueChange; j<m_iCartoonSegments-1; ++j)
			    {
				if (i==0)
				{
				    //printf("fW1: %3.0f fW2: %3.0f\n", fW1, fW2);
				    oN = fW1-- * oN1 + fW2++ * oN2;
				    memcpy(oV.n, oN.c_data(), 12);
				}
				else
				    memcpy(oV.n, oN2.c_data(), 12);

				memcpy(oV.v, aL[j].c_data(), 12);
				m_pCartoonCont->addSegment(&oV);
			    }
			}

			// from now on, normal isn't change anymore
			memcpy(oV.n, oN2.c_data(), 12);

			if (iSkipEnd)
			{
			    oL1 = m_aRenAtoms[*(itrCA-1)].getGLPos();
			    oL2 = m_aRenAtoms[*itrCA].getGLPos();
			    ++itrCA;
			    oL3 = m_aRenAtoms[*itrCA].getGLPos();
			    oL4 = m_aRenAtoms[*(itrCA+1)].getGLPos();

			    m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			    m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );

			    for (j=0; j<iResidueChange; ++j)
			    {
				memcpy(oV.v, aL[j].c_data(), 12);
				m_pCartoonCont->addSegment(&oV);
			    }
			    memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);
			    for (j=iResidueChange; j<m_iCartoonSegments-1; ++j)
			    {
				memcpy(oV.v, aL[j].c_data(), 12);
				m_pCartoonCont->addSegment(&oV);
			    }
			}


			// third-last and second-last residues
			for (i=0; i<2; ++i)
			{
 			    if (i == 1)
			    {
				fW1 = 3.0f * (((Real32)m_iCartoonSegments)-1.0f);
				fW2 = 1.0f;
			    }

			    oL1 = oL2;
			    oL2 = oL3;
			    oL3 = oL4;
			    ++itrCA;
			    oL4 = m_aRenAtoms[*(itrCA+1)].getGLPos();

			    m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			    m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );

			    for (j=0; j<iResidueChange; ++j)
			    {
				memcpy(oV.v, aL[j].c_data(), 12);
				m_pCartoonCont->addSegment(&oV);
			    }
			    memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);
			    for (j=iResidueChange; j<m_iCartoonSegments-1; ++j)
			    {
				memcpy(oV.v, aL[j].c_data(), 12);
				m_pCartoonCont->addSegment(&oV);
			    }
			}


			// last residue
			oL1 = oL2;
			oL2 = oL3;
			oL3 = oL4;

			++itrCA;

			if (bChainEnd)
			    oL4 = oL3;
			else
			    oL4 = 0.5f * (m_aRenAtoms[  *itrCA  ].getGLPos() + m_aRenAtoms[  *(itrCA+1)  ].getGLPos());

			m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );

			for (j=0; j<iResidueChange; ++j)
			{
			    memcpy(oV.v, aL[j].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}
			memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);

			// if the chain does not end here and needs an arrowhead, we need to add an additional segment with a greated width where the arrowhead begins
			if (m_bCartoonHelixArrowhead && bChainEnd)
			{
			    // compute fW, that is substracted repeatedly from the helix width to create an arrowhead
			    fW = (oV.v[3] * 1.5f) / (m_iCartoonSegments-iResidueChange);
			    oV.v[3] *= 1.5f;

			    // add an intermediate vertex; here the arrowhead begins (the width is greater here)
			    oV.v[0] = 0.99f * aL[iResidueChange-1].x() + 0.01f * aL[iResidueChange].x();
			    oV.v[1] = 0.99f * aL[iResidueChange-1].y() + 0.01f * aL[iResidueChange].y();
			    oV.v[2] = 0.99f * aL[iResidueChange-1].z() + 0.01f * aL[iResidueChange].z();

			    m_pCartoonCont->addSegment(&oV);
			}

			for (j=iResidueChange; j<m_iCartoonSegments-1; ++j)
			{
			    if (m_bCartoonHelixArrowhead && bChainEnd)
				oV.v[3] = oV.v[3] - fW;

			    memcpy(oV.v, aL[j].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}



			// if the chain ends here, just add the last segment, that coincides with last C_alpha
			if (bChainEnd)
			{
			    if (m_bCartoonHelixArrowhead)
				oV.v[3] = oV.v[3] - fW;

			    memcpy(oV.v, aL[j].c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}
			// if not, create a tail that ends in the middle of a peptide plane (i.e. at the end of the residue)
			else
			{
			    oL1 = oL2;
			    oL2 = oL3;
			    oL3 = oL4;
			    if (*itrSS_End == 'S')
				oL4 = 0.5f * (m_aRenAtoms[*(itrCA+1)].getGLPos() + m_aRenAtoms[*(itrCA+2)].getGLPos());
			    else
				oL4 = m_aRenAtoms[*(itrCA+1)].getGLPos();


			    m_oKBSpline.setTBC(-0.5f, 0.0f, -0.5f);
			    m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			    m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );

			    m_oKBSpline.setContinuity(0.0f);


			    if (m_bCartoonHelixArrowhead)
			    {
				// add this first segment here, because for an arrowhead beginning, there will be a second vertex with greater width
				memcpy(oV.v, aL[0].c_data(), 12);
				m_pCartoonCont->addSegment(&oV);


				// compute fW, that is substracted repeatedly from the helix width to create an arrowhead
				fW = (oV.v[3] * 1.5f) / (m_iCartoonSegments-1);
				oV.v[3] *= 1.5f;

				// add an intermediate vertex; here the arrowhead begins (the width is greater here)
				oV.v[0] = 0.99f * aL[0].x() + 0.01f * aL[1].x();
				oV.v[1] = 0.99f * aL[0].y() + 0.01f * aL[1].y();
				oV.v[2] = 0.99f * aL[0].z() + 0.01f * aL[1].z();

				m_pCartoonCont->addSegment(&oV);

				for (j=1; j<m_iCartoonSegments; ++j)
				{
				    memcpy(oV.v, aL[j].c_data(), 12);
				    oV.v[3] = oV.v[3] - fW;
				    m_pCartoonCont->addSegment(&oV);
				}
			    }
			    else
			    {
				for (j=0; j<m_iCartoonSegments; ++j)
				{
				    memcpy(oV.v, aL[j].c_data(), 12);
				    m_pCartoonCont->addSegment(&oV);
				}
			    }
			}

			m_pCartoonCont->endHelix();

#ifdef HELIX_AXIS
			glEnd();
#endif
			++itrCA;
		    }
		}
		break;
	    }
	    case 'S': // SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS
	    {

 		//itrCA += iLength;
 		//break;

		m_oKBSpline.setTension(0.0f);
		oV.v[3] = CARTOON_SCALING_FACTOR * m_fCartoonSheetWidth;
		memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);

		m_pCartoonCont->beginSheet();

		if (iLength == 2 && bChainBegin && bChainEnd)
		{

		    oO = m_aRenAtoms[getBackboneO(*itrCA)].getGLPos();
		    oN1 = svt_vector3< Real32 >(m_aRenAtoms[*(itrCA+1)].getGLPos() + m_aRenAtoms[*itrCA].getGLPos() - oO);
		    oN1 = oN1 - svt_vector3< Real32 >(oO);
		    memcpy(oV.n, oN1.c_data(), 12);

		    oL1 = svt_vector3< Real32 >(m_aRenAtoms[*itrCA].getGLPos());
		    oL2 = oL1;
		    ++itrCA;
		    oL3 = svt_vector3< Real32 >(m_aRenAtoms[*itrCA].getGLPos());
		    oL4 = oL3;

		    m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
		    m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );

		    for (j=0; j<iResidueChange; ++j)
		    {
			memcpy(oV.v, aL[j].c_data(), 12);
			m_pCartoonCont->addSegment(&oV);
		    }

		    fW = (oV.v[3] * 1.5f) / (m_iCartoonSegments - iResidueChange);
		    oV.v[3] *= 1.5f;

		    oV.v[0] = 1.01f * aL[iResidueChange-1].x() - 0.01f * aL[iResidueChange-2].x();
		    oV.v[1] = 1.01f * aL[iResidueChange-1].y() - 0.01f * aL[iResidueChange-2].y();
		    oV.v[2] = 1.01f * aL[iResidueChange-1].z() - 0.01f * aL[iResidueChange-2].z();

		    m_pCartoonCont->addSegment(&oV);

		    memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);

		    for (j=iResidueChange; j<m_iCartoonSegments; ++j)
		    {
			memcpy(oV.v, aL[j].c_data(), 12);
			oV.v[3] -= fW;
			m_pCartoonCont->addSegment(&oV);
		    }
		    ++itrCA;
		}
		else
		{
		    // at the beginning of a chain, no tangent information (i.e. oL1=oL2)
		    if (bChainBegin)
		    {
			oL1 = m_aRenAtoms[*itrCA].getGLPos();
			oL2 = oL1;

			oO = m_aRenAtoms[getBackboneO(*itrCA)].getGLPos();
			oN1 = svt_vector3< Real32 >(m_aRenAtoms[*(itrCA+1)].getGLPos() + m_aRenAtoms[*itrCA].getGLPos() - oO);
			oN1 = oN1 - svt_vector3< Real32 >(oO);
		    }
		    // otherwise, tangent information points to previous C_alpha
		    else
		    {
			oL1 = m_aRenAtoms[*(itrCA-1)].getGLPos();
			oL2 = 0.5f * (m_aRenAtoms[*(itrCA-1)].getGLPos() + m_aRenAtoms[*itrCA].getGLPos());

			oO = m_aRenAtoms[getBackboneO(*(itrCA-1))].getGLPos();
			oN1 = svt_vector3< Real32 >(m_aRenAtoms[*itrCA].getGLPos() + m_aRenAtoms[*(itrCA-1)].getGLPos() - oO);
			oN1 = oN1 - svt_vector3< Real32 >(oO);
		    }

		    oL3 = 0.5f * (m_aRenAtoms[*itrCA].getGLPos() + m_aRenAtoms[*(itrCA+1)].getGLPos());
		    ++itrCA;

		    if (iLength == 2 && bChainEnd)
			oL4 = m_aRenAtoms[*itrCA].getGLPos();
		    else
			oL4 = 0.5f * (m_aRenAtoms[*itrCA].getGLPos() + m_aRenAtoms[*(itrCA+1)].getGLPos());

		    m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
		    m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );

 		    oO = m_aRenAtoms[getBackboneO(*(itrCA-1))].getGLPos();
		    oN2 = svt_vector3< Real32 >(m_aRenAtoms[*itrCA].getGLPos() + m_aRenAtoms[*(itrCA-1)].getGLPos() - oO);
 		    oN2  = oN2 - svt_vector3< Real32 >(oO);
		    // check if two adjacent peptide plane orientations are twisted more than 90
		    // degrees to each other. although this should be the case in sheets, correct it
		    // because at the beginning and at the end of the sheet, it will look strange
		    if (oN2 * oN1 < 0)
			oN2 = oN2 * -1.0f;

		    memcpy(oV.n, oN1.c_data(), 12);


		    fW1 = (Real32)(m_iCartoonSegments-1);
		    fW2 = 0.0f;

		    for (j=0; j<m_iCartoonSegments-1; ++j)
		    {
			memcpy(oV.v, aL[j].c_data(), 12);
			oN = fW1-- * oN1 + fW2++ * oN2;
			memcpy(oV.n, oN.c_data(), 12);
			m_pCartoonCont->addSegment(&oV);
		    }


		    ////////////////////////////////////////////////////////////////////////////////////
		    for (i=3; i<iLength; ++i)
		    {
 			oL1 = oL2;
			oL2 = oL3;
			oL3 = oL4;
			memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);
			++itrCA;
			oL4 = 0.5f * (m_aRenAtoms[*itrCA].getGLPos() + m_aRenAtoms[*(itrCA+1)].getGLPos());


			m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );

			oO = m_aRenAtoms[getBackboneO(*(itrCA-1))].getGLPos();
			oN1 = oN2;
			oN2 = svt_vector3< Real32 >(m_aRenAtoms[*itrCA].getGLPos() + m_aRenAtoms[*(itrCA-1)].getGLPos() - oO);

			// rely on the fact that two adjacent peptide planes are twisted by 180
			// degrees to each other, or
			if (!(i%2))
			    oN2  = svt_vector3< Real32 >(oO) - oN2;
			else
			    oN2  = oN2 - svt_vector3< Real32 >(oO);

			// check for it:

// 			oN2  = oN2 - svt_vector3< Real32 >(oO);
//  			if (oN1 * oN2 < 0)
//  			    oN2 = oN2 * -1.0f;

			fW1 = (Real32)(m_iCartoonSegments-1);
			fW2 = 1.0f;

			for (j=0; j<m_iCartoonSegments-1; ++j)
			{
			    memcpy(oV.v, aL[j].c_data(), 12);
			    oN = fW1-- * oN1 + fW2++ * oN2;
			    memcpy(oV.n, oN.c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}
		    }
		    ////////////////////////////////////////////////////////////////////////////////////


		    memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);


		    if (iLength > 2)
		    {
			oL1 = oL2;
			oL2 = oL3;
			oL3 = oL4;

			++itrCA;

			if (!bChainEnd)
			    oL4 = 0.5f * (m_aRenAtoms[*itrCA].getGLPos() + m_aRenAtoms[*(itrCA+1)].getGLPos());
			else
			    oL4 = m_aRenAtoms[*itrCA].getGLPos();

			m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
			m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );

			oO = m_aRenAtoms[getBackboneO(*(itrCA-1))].getGLPos();
			oN1 = oN2;
			oN2 = svt_vector3< Real32 >(m_aRenAtoms[*itrCA].getGLPos() + m_aRenAtoms[*(itrCA-1)].getGLPos() - oO);
			oN2  = oN2 - svt_vector3< Real32 >(oO);
 			if (oN2 * oN1 < 0)
 			    oN2 = oN2 * -1.0f;

			fW1 = (Real32)(m_iCartoonSegments-1);
			fW2 = 1.0f;

			for (j=0; j<m_iCartoonSegments-1; ++j)
			{
			    memcpy(oV.v, aL[j].c_data(), 12);
			    oN = fW1-- * oN1 + fW2++ * oN2;
			    memcpy(oV.n, oN.c_data(), 12);
			    m_pCartoonCont->addSegment(&oV);
			}
		    }


		    oL1 = oL2;
		    oL2 = oL3;
		    oL3 = oL4;

		    if (!bChainEnd)
			oL4 = m_aRenAtoms[*(itrCA+1)].getGLPos();
		    else
			oL4 = oL3;

		    m_oKBSpline.calcSplineQMatrix(oL1, oL2, oL3, oL4);
		    m_oKBSpline.calcSplinePoints( aL, m_iCartoonSegments );


		    memcpy(oV.c, m_aRenAtoms[*itrCA].getColor()->getDiffuse(), 16);

		    ++itrCA;

		    if (!bChainEnd)
		    {
			fW1 = (Real32)(m_iCartoonSegments-1);
			fW2 = 1.0f;

			oO = m_aRenAtoms[getBackboneO(*(itrCA-1))].getGLPos();
			oN1 = oN2;
			oN2 = svt_vector3< Real32 >(m_aRenAtoms[*itrCA].getGLPos() + m_aRenAtoms[*(itrCA-1)].getGLPos() - oO);
			oN2  = oN2 - svt_vector3< Real32 >(oO);
			if (oN2 * oN1 < 0)
			    oN2 = oN2 * -1.0f;
		    }

		    memcpy(oV.v, aL[0].c_data(), 12);
		    m_pCartoonCont->addSegment(&oV);

 		    fW = (oV.v[3] * 1.5f) / (m_iCartoonSegments-1);
 		    oV.v[3] *= 1.5f;

 		    oV.v[0] = 0.99f * aL[0].x() + 0.01f * aL[1].x();
 		    oV.v[1] = 0.99f * aL[0].y() + 0.01f * aL[1].y();
 		    oV.v[2] = 0.99f * aL[0].z() + 0.01f * aL[1].z();

		    m_pCartoonCont->addSegment(&oV);

		    for (j=1; j<m_iCartoonSegments; ++j)
		    {
			memcpy(oV.v, aL[j].c_data(), 12);
			if (!bChainEnd)
			{
			    oN = fW1-- * oN1 + fW2++ * oN2;
			    memcpy(oV.n, oN.c_data(), 12);
			}
			oV.v[3] -= fW;
			m_pCartoonCont->addSegment(&oV);
		    }
		}

		m_pCartoonCont->endSheet();

		break;
	    }
	    default:
	    {
		// only to avoid compiler warning
		break;
	    }
	} // switch close


	if (itrSS_End == itrSS_VectorEnd)
	    break;


	if (bChainBegin && bChainEnd)
	{
	    bChainEnd = false;
	}
	else if (bChainBegin)
	{
	    bChainBegin = false;
	}
	else if (bChainEnd)
	{
	    bChainEnd = false;
	    bChainBegin = true;
	}

	cPrev = cStart;
    }


    // draw the peptide planes of the residues
    if (m_bCartoonPeptidePlanes)
    {
	svt_vector3<Real32> v1, v2, n;
	svt_c4ub_n3f_v3f p;

	m_pCont->beginTriangles();

	for (itrCA=(aCA->begin()); itrCA != (aCA->end()-1); ++itrCA)
	{
	    if (m_aRenAtoms[*itrCA].getOrdChainID() != m_aRenAtoms[*(itrCA+1)].getOrdChainID() ||
		m_aRenAtoms[*itrCA].getOrdResidueSeq()+1 != m_aRenAtoms[*(itrCA+1)].getOrdResidueSeq())
		continue;
	    
	    v1 = svt_vector3<Real32>(m_aRenAtoms[  *itrCA  ].getGLPos() - m_aRenAtoms[getBackboneO(*itrCA)].getGLPos() );
	    v2 = svt_vector3<Real32>(m_aRenAtoms[*(itrCA+1)].getGLPos() - m_aRenAtoms[getBackboneO(*itrCA)].getGLPos() );
	    n = crossProduct(v1, v2);
	    n.normalize();

            p.setC(m_aRenAtoms[  *itrCA  ].getColor()->getR()*255.0f, m_aRenAtoms[  *itrCA  ].getColor()->getG()*255.0f, m_aRenAtoms[  *itrCA  ].getColor()->getB()*255.0f);
            p.setN(n.x(), n.y(), n.z());
            p.setV(m_aRenAtoms[  *itrCA  ].getGLPos().x(),          m_aRenAtoms[  *itrCA  ].getGLPos().y(),          m_aRenAtoms[  *itrCA  ].getGLPos().z());
            m_pCont->addVertex(p);
            p.setV(m_aRenAtoms[  getBackboneO(*itrCA)   ].getGLPos().x(),          m_aRenAtoms[  getBackboneO(*itrCA)   ].getGLPos().y(),          m_aRenAtoms[  getBackboneO(*itrCA)   ].getGLPos().z());
            m_pCont->addVertex(p);
            p.setV(m_aRenAtoms[  *itrCA  ].getGLPos().x() + v2.x(), m_aRenAtoms[  *itrCA  ].getGLPos().y() + v2.y(), m_aRenAtoms[  *itrCA  ].getGLPos().z() + v2.z());
            m_pCont->addVertex(p);

            p.setC(m_aRenAtoms[*(itrCA+1)].getColor()->getR()*255.0f, m_aRenAtoms[*(itrCA+1)].getColor()->getG()*255.0f, m_aRenAtoms[*(itrCA+1)].getColor()->getB()*255.0f);
            p.setV(m_aRenAtoms[*(itrCA+1)].getGLPos().x(),          m_aRenAtoms[*(itrCA+1)].getGLPos().y(),          m_aRenAtoms[*(itrCA+1)].getGLPos().z());
            m_pCont->addVertex(p);
            p.setV(m_aRenAtoms[  getBackboneO(*itrCA)   ].getGLPos().x(),          m_aRenAtoms[  getBackboneO(*itrCA)   ].getGLPos().y(),          m_aRenAtoms[  getBackboneO(*itrCA)   ].getGLPos().z());
            m_pCont->addVertex(p);
            p.setV(m_aRenAtoms[*(itrCA+1)].getGLPos().x() + v1.x(), m_aRenAtoms[*(itrCA+1)].getGLPos().y() + v1.y(), m_aRenAtoms[*(itrCA+1)].getGLPos().z() + v1.z());
            m_pCont->addVertex(p);
	}

	m_pCont->endTriangles();
    }

    //printf("svt_pdb::calcCartoon() %lu\n", svt_getToD()-iTime);
}

int svt_pdb::getBackboneO(int i)
{
    int j = (int)i;
    int k = m_aRenAtoms[i].getOrdResidueSeq();

    while (true)
    {
	--j;

	if (j < 0)
	    break;

	else if (m_aRenAtoms[j].getOrdResidueSeq() != k)
	    break;

	else if (m_aRenAtoms[j].getName()[0] == 'O' && m_aRenAtoms[j].getRemoteness() == ' ')
	    return j;
    }

    j = i;

    i = m_aRenAtoms.size();

    while (true)
    {

	++j;

	if (j == i)
	    return -1;

	else if (m_aRenAtoms[j].getOrdResidueSeq() != k)
	    return -1;

	else if (m_aRenAtoms[j].getName()[0] == 'O' && m_aRenAtoms[j].getRemoteness() == ' ')
	    return j;
    }
    return 0;
}

void svt_pdb::setCartoonParam(EnumCartoonParam eParameter, float fValue)
{
    switch (eParameter)
    {
	case CARTOON_SHEET_HELIX_PROFILE_CORNERS:
	{
	    // possible values: 2 for flat sheets/helices
	    //                  4 for rectangular sheets/helices
	    //                  >=6 for round sheets/helices


	    // if previous value is 4, and now setting to 3, user is decreasing value, so set to 2
	    if (m_iCartoonSheetHelixProfileCorners == 4 && fValue == 3.0f)
		m_iCartoonSheetHelixProfileCorners = 2;

	    // if previous value is 2, and now setting to 3, user is increasing value, so set to 4
	    else if (m_iCartoonSheetHelixProfileCorners == 2 && fValue == 3.0f)
		m_iCartoonSheetHelixProfileCorners = 4;

	    // if previous value is 6, and now setting to 5, user is decreasing value, so set to 4
	    else if (m_iCartoonSheetHelixProfileCorners == 6 && fValue == 5.0f)
		m_iCartoonSheetHelixProfileCorners = 4;

	    // if previous value is 4, and now setting to 5, user is increasing value, so set to 6
	    else if (m_iCartoonSheetHelixProfileCorners == 4 && fValue == 5.0f)
		m_iCartoonSheetHelixProfileCorners = 6;

	    else
	    {
		if (fValue <= 2.0f)
		    fValue = 2.0f;

		m_iCartoonSheetHelixProfileCorners = (unsigned int) fValue;
	    }

	    rebuildDL();
	    break;
	}
	case CARTOON_USE_SHADER:
	{
            m_bCartoonUseShader = (bool)fValue;

            if ( fValue == 1.0f && getShaderSupported() && svt_getAllowShaderPrograms() )
            {
                m_pCartoonCont_Dynamic->setVisible(true);
                m_pCartoonCont_Static ->setVisible(false);
                m_pCartoonCont = m_pCartoonCont_Dynamic;
            }
            else
            {
                m_pCartoonCont_Dynamic->setVisible(false);
                m_pCartoonCont_Static ->setVisible(true);
                m_pCartoonCont = m_pCartoonCont_Static;
            }

            rebuildDL();
	    break;
	}
	case CARTOON_PER_PIXEL_LIGHTING:
	{
            m_pCartoonCont_Dynamic->setPerPixelLighting((bool)fValue);

	    rebuildDL();
	    break;
	}
	case CARTOON_PEPTIDE_PLANES:
	{
	    (fValue == 0.0f) ? m_bCartoonPeptidePlanes = false : m_bCartoonPeptidePlanes = true;

	    rebuildDL();
	    break;
	}
	case CARTOON_SHEET_MIN_RESIDUES:
	{
	    // a sheet must have at least two residues
	    if (fValue < 2.0f)
		fValue = 2.0f;
	    m_iMinResiduesPerSheet = (unsigned int) fValue;

	    rebuildDL();
	    break;
	}
	case CARTOON_SHEET_WIDTH:
	{
	    if (fValue < 0.0f) 
		fValue = 0.0f;
	    m_fCartoonSheetWidth = fValue;

	    rebuildDL();
	    break;
	}
	case CARTOON_SHEET_HEIGHT:
	{
	    if (fValue < 0.0f) 
		fValue = 0.0f;
	    m_fCartoonSheetHeight = fValue;

	    rebuildDL();
	    break;
	}
	case CARTOON_HELIX_MIN_RESIDUES:
	{
	    // a helix must have at least three residues
	    if (fValue < 3.0f)
		fValue = 3.0f;
	    m_iMinResiduesPerHelix = (unsigned int) fValue;

	    rebuildDL();
	    break;
	}
	case CARTOON_HELIX_WIDTH:
	{
	    if (fValue < 0.0f) 
		fValue = 0.0f;
	    m_fCartoonHelixWidth = fValue;

	    rebuildDL();
	    break;
	}
	case CARTOON_HELIX_HEIGHT:
	{
	    if (fValue < 0.0f) 
		fValue = 0.0f;
	    m_fCartoonHelixHeight = fValue;

	    rebuildDL();
	    break;
	}
	case CARTOON_TUBE_DIAMETER:
	{
	    if (fValue < 0.0f) 
		fValue = 0.0f;
	    m_fCartoonTubeDiameter = fValue;

	    rebuildDL();
	    break;
	}
	case CARTOON_HELIX_CYLINDER:
	{
	    (fValue == 0.0f) ? m_bCartoonHelixCylinder = false : m_bCartoonHelixCylinder = true;

	    rebuildDL();
	    break;
	}
	case CARTOON_HELIX_ARROWHEADS:
	{
	    (fValue == 0.0f) ? m_bCartoonHelixArrowhead = false : m_bCartoonHelixArrowhead = true;

	    rebuildDL();
	    break;
	}
	case CARTOON_SPLINE_SEGMENTS:
	{
	    // at least four segments per residue
	    if (fValue < 4.0f)
		fValue = 4.0f;

	    m_iCartoonSegments = (unsigned int) fValue;

	    rebuildDL();
	    break;
	}
	default:
	    break;
    }
}

float svt_pdb::getCartoonParam(EnumCartoonParam eParameter)
{
    switch (eParameter)
    {
	case CARTOON_SHEET_HELIX_PROFILE_CORNERS:
	{
	    return m_iCartoonSheetHelixProfileCorners;
	}
	case CARTOON_USE_SHADER:
	{
	    return (float)m_bCartoonUseShader;
	}
	case CARTOON_PER_PIXEL_LIGHTING:
	{
            return (float)(m_pCartoonCont_Dynamic->getPerPixelLighting());
	}
	case CARTOON_PEPTIDE_PLANES:
	{
            return (float)m_bCartoonPeptidePlanes;
	}
	case CARTOON_SHEET_MIN_RESIDUES:
	{
	    return (float)m_iMinResiduesPerSheet;
	}
	case CARTOON_SHEET_WIDTH:
	{
	    return m_fCartoonSheetWidth;
	}
	case CARTOON_SHEET_HEIGHT:
	{
	    return m_fCartoonSheetHeight;
	}
	case CARTOON_HELIX_MIN_RESIDUES:
	{
	    return (float)m_iMinResiduesPerHelix;
	}
	case CARTOON_HELIX_WIDTH:
	{
	    return m_fCartoonHelixWidth;
	}
	case CARTOON_HELIX_HEIGHT:
	{
	    return m_fCartoonHelixHeight;
	}
	case CARTOON_TUBE_DIAMETER:
	{
	    return m_fCartoonTubeDiameter;
	}
	case CARTOON_HELIX_CYLINDER:
	{
            return (float)m_bCartoonHelixCylinder;
	}
	case CARTOON_HELIX_ARROWHEADS:
	{
            return (float)m_bCartoonHelixArrowhead;
	}
	case CARTOON_SPLINE_SEGMENTS:
	{
	    return (float)m_iCartoonSegments;
	}
	default:
	{
	    return 0.0f;
	}
    }
}

void svt_pdb::setNewTubeSegs(int iTubeSegs)
{
    m_iNewTubeSegs = (unsigned int) iTubeSegs;
    rebuildDL();
}

int svt_pdb::getNewTubeSegs()
{
    return (int)m_iNewTubeSegs;
}

void svt_pdb::setNewTubeProfileCorners( int iNumCorners )
{
    m_iNewTubeProfileCorners = (unsigned int) iNumCorners;
    rebuildDL();
}

int svt_pdb::getNewTubeProfileCorners()
{
    return (int)m_iNewTubeProfileCorners;
}

void svt_pdb::setNewTubeDiameter(float fNewTubeDiameter)
{
    m_fNewTubeDiameter = fNewTubeDiameter;
    rebuildDL();
}

float svt_pdb::getNewTubeDiameter()
{
    return m_fNewTubeDiameter;
}

bool svt_pdb::getShaderSupported()
{
    return m_pCartoonCont_Dynamic->checkShaderSupport();
}


/**
 * this function adds the cartoon container and initializes member variables
 */
void svt_pdb::initCartoon()
{
    m_iNewTubeSegs = 6;
    m_iNewTubeProfileCorners = 6;
    m_fNewTubeDiameter = 1.0f;

    m_bCartoonPeptidePlanes = false;
    m_bCartoonHelixArrowhead = false;
    m_bCartoonHelixCylinder = false;
    m_fCartoonHelixWidth = 1.5f;
    m_fCartoonHelixHeight = 0.5;
    m_fCartoonSheetWidth = 1.5f;
    m_fCartoonSheetHeight = 0.5f;
    m_fCartoonTubeDiameter = 0.75f;
    m_iCartoonSegments = 6;
    // in the geometry generation code, a lot of times it is assumed that the minimum length for a
    // sheet is two. this saves a ton of special cases
    m_iMinResiduesPerSheet = 2;
    // for helices, the minimum is three. saves another ton of special cases
    m_iMinResiduesPerHelix = 3;
    m_iCartoonSheetHelixProfileCorners = 4;

    m_pCartoonCont_Dynamic = new svt_container_cartoon_dynamic(this);
    m_pCartoonCont_Dynamic->setName("(svt_pdb)m_pCartoonCont_Dynamic");
    add(m_pCartoonCont_Dynamic);
    m_pCartoonCont_Static = new svt_container_cartoon_static(this);
    m_pCartoonCont_Static->setName("(svt_pdb)m_pCartoonCont_Static");
    add(m_pCartoonCont_Static);

    // default shader use to false
    m_bCartoonUseShader = false;
    // but per pixel lighting should be used (when shaders are switched on)
    m_pCartoonCont_Dynamic->setPerPixelLighting(true);

    if ( svt_getAllowShaderPrograms() && m_bCartoonUseShader )
    {
	m_pCartoonCont = m_pCartoonCont_Dynamic;
	m_pCartoonCont_Static->setVisible(false);
    }
    else
    {
	m_pCartoonCont = m_pCartoonCont_Static;
	m_pCartoonCont_Dynamic->setVisible(false);
    }
}
