/***************************************************************************
                          svt_tube.cpp
			  -------------------
    begin                : 10/07/2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_pdb.h>

/**
 * local function to correct the length of the cylinders
 */
void svt_pdb::correctCylinder( svt_vector4<Real32>& rPrev, svt_vector4<Real32>& rStart, svt_vector4<Real32>& rEnd, svt_vector4<Real32> rNext)
{
    svt_vector4<Real32> oVecP = rStart - rPrev; oVecP.normalize();
    svt_vector4<Real32> oVecT = rEnd - rStart; oVecT.normalize();
    svt_vector4<Real32> oVecN = rNext - rEnd; oVecN.normalize();

    Real32 fAlpha = fabs(oVecP * oVecT);
    Real32 fAdj = 0.0f;
    if (fAlpha > 0.0f && fAlpha < 0.9999f)
        fAdj = (m_fTubeDiameter * 0.001f) * sqrtf((1.0f-fAlpha) / (1.0f+fAlpha));

    rStart = rStart - (oVecT * fAdj);

    fAlpha = oVecT * oVecN;
    fAdj = 0.0f;
    if (fAlpha > 0.0f && fAlpha < 0.9999f)
        fAdj = (m_fTubeDiameter * 0.001f) * sqrtf((1.0f-fAlpha) / (1.0f+fAlpha));

    rEnd = rEnd + (oVecT * fAdj);
};

/**
 * calculate a tube
 * \param rAtoms reference to vector<int> with the indizes of the atoms
 */
void svt_pdb::calcTube( vector<int>& rAtoms )
{
    if (rAtoms.size() < 4)
        return;

    // trace mode = no splines?
    unsigned int i,j;
    unsigned int iTubePoints = m_iTubeSegs + 1;

    if (m_iTubeSegs < 2)
    {
        svt_vector4<Real32> oAtomA;
        svt_vector4<Real32> oAtomB;
        for(i=0; i<rAtoms.size()-2; i++)
        {
            oAtomA = svt_vector4<Real32>( m_aRenAtoms[rAtoms[i]].getGLPos() );
            oAtomB = svt_vector4<Real32>( m_aRenAtoms[rAtoms[i+1]].getGLPos() );

            m_pCont->addCylinder(
				 oAtomA,
				 oAtomB,
				 m_fTubeDiameter * 0.001f,
				 *m_aRenAtoms[rAtoms[i]].getColor()
				);
        }
        return;
    }

    // ok, tube mode, we have to calculate the splines
    svt_vector4<Real32> *pTmp = new svt_vector4<Real32>[iTubePoints];
    svt_vector4<Real32> oVecA;
    svt_vector4<Real32> oVecB;
    svt_vector4<Real32> oVecC;
    svt_vector4<Real32> oVecD;
    svt_vector4<Real32> oVecOldA;
    svt_vector4<Real32> oVecOldB;
    svt_vector4<Real32> oVecOldC;
    svt_vector4<Real32> oVecOldD;

    svt_vector4<Real32> oAtom0;
    svt_vector4<Real32> oAtom1;
    svt_vector4<Real32> oAtom2;
    svt_vector4<Real32> oAtom3;

    bool bFirstSeg = true;

    for(i=0; i<=rAtoms.size()-4; i++)
    {

        oAtom0 = svt_vector4<Real32>(m_aRenAtoms[rAtoms[i]].getGLPos());
        oAtom1 = svt_vector4<Real32>(m_aRenAtoms[rAtoms[i+1]].getGLPos());
        oAtom2 = svt_vector4<Real32>(m_aRenAtoms[rAtoms[i+2]].getGLPos());
        oAtom3 = svt_vector4<Real32>(m_aRenAtoms[rAtoms[i+3]].getGLPos());

        m_oCRSpline.calcSplineQMatrix(
                                      oAtom0,
                                      oAtom1,
                                      oAtom2,
                                      oAtom3
                                     );
        m_oCRSpline.calcSplinePoints( pTmp, iTubePoints );

        // do we have to draw cylinders or lines?
        if (m_fTubeDiameter > 0.8f)
        {
            // draw the last part of the previous segment
            if (!bFirstSeg)
            {
                oVecA = oVecOldB;
                oVecB = oVecOldC;
                oVecC = oVecOldD;
                oVecD = pTmp[1];
                correctCylinder( oVecA, oVecB, oVecC, oVecD );
                m_pCont->addCylinder(
				     oVecB,
				     oVecC,
				     m_fTubeDiameter * 0.001f,
				     *m_aRenAtoms[rAtoms[i]].getColor()
				    );

            }

            // draw the first part of the current segment
            if (bFirstSeg)
                oVecOldC = pTmp[0];
            oVecA = oVecOldC;
            oVecB = pTmp[0];
            oVecC = pTmp[1];
            oVecD = pTmp[2];
            correctCylinder( oVecA, oVecB, oVecC, oVecD );
            if (i==0)
                m_pCont->addCylinder(
				     oVecB,
				     oVecC,
				     m_fTubeDiameter * 0.001f,
				     *m_aRenAtoms[rAtoms[i]].getColor()
				    );
            else
                m_pCont->addCylinder(
				     oVecB,
				     oVecC,
				     m_fTubeDiameter * 0.001f,
				     *m_aRenAtoms[rAtoms[i]].getColor()
				    );

            // draw everything, except the first and last segment
            for(j=0; j<iTubePoints-3; j++)
            {
                oVecA = pTmp[j];
                oVecB = pTmp[j+1];
                oVecC = pTmp[j+2];
                oVecD = pTmp[j+3];

                // regular segment
                correctCylinder( oVecA, oVecB, oVecC, oVecD );
                if (j<floor(float(iTubePoints)/2))
                    m_pCont->addCylinder(
					 oVecB,
					 oVecC,
					 m_fTubeDiameter * 0.001f,
					 *m_aRenAtoms[rAtoms[i]].getColor()
					);
                else
                    m_pCont->addCylinder(
					 oVecB,
					 oVecC,
					 m_fTubeDiameter * 0.001f,
					 *m_aRenAtoms[rAtoms[i+1]].getColor()
					);
            }

            oVecOldA = pTmp[j-1];
            oVecOldB = pTmp[j];
            oVecOldC = pTmp[j+1];
            oVecOldD = pTmp[j+2];
            bFirstSeg = false;

        } /*else {
            for(j=0; j<iTubePoints-1; j++)
            {
                if (j<floor(float(iTubePoints)/2))
                    m_pCont->addLine(
				     pTmp[j],
				     pTmp[j+1],
				     m_aRenAtoms[rAtoms[i]].getColor()->getR(),
				    );
                else
                    (*m_pCont) << new svt_basic_line(
                                                  pTmp[j],
                                                  pTmp[j+1],
                                                  m_aRenAtoms[rAtoms[i+1]].getColor()->getR(),
                                                  m_aRenAtoms[rAtoms[i+1]].getColor()->getG(),
                                                  m_aRenAtoms[rAtoms[i+1]].getColor()->getB()
                                                 );
            }
        } */
    }

    // draw the last part of the last segment
    if (m_fTubeDiameter > 0.8f)
    {
        if (!bFirstSeg)
        {
            oVecA = oVecOldB;
            oVecB = oVecOldC;
            oVecC = oVecOldD;
            oVecD = oVecOldD;
            correctCylinder( oVecA, oVecB, oVecC, oVecD );
            m_pCont->addCylinder(
				 oVecB,
				 oVecC,
				 m_fTubeDiameter * 0.001f,
				 *m_aRenAtoms[rAtoms[i]].getColor()
				);

        }
    }

    delete[] pTmp;
}
