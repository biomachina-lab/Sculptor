/***************************************************************************
                          svt_flexing_inverse_distance_weighting
                          --------------------------------------
    begin                : 09/10/2006
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_flexing_inverse_dist_weighting.h>
#include <svt_iostream.h>
#include <svt_point_cloud.h>
#include <svt_vector4.h>
#include <svt_matrix.h>
#include <math.h>

#define FLEX cout << svt_trimName("flexing")

svt_flexing_inverse_dist_weighting::svt_flexing_inverse_dist_weighting(
    const svt_point_cloud_pdb< svt_vector4<Real64> > &oOrigConf,
    const svt_point_cloud< svt_vector4<Real64> >     &oOrigConfCodebooks, 
    const svt_point_cloud< svt_vector4<Real64> >     &oFinalConfCodebooks,
    bool bFlexAllAtoms, string pDistType, string pDistInGraphSchema,
    string pWeightingMethod, Real64 fWeightPow) : svt_flexing(oOrigConf,oOrigConfCodebooks,oFinalConfCodebooks, bFlexAllAtoms, pDistType, pDistInGraphSchema)
{
    m_pWeightingMethod = pWeightingMethod;
    m_fWeightPow = fWeightPow;
    m_iNbCdbksOfInfluence = oOrigConfCodebooks.size();
}

svt_flexing_inverse_dist_weighting::~svt_flexing_inverse_dist_weighting()
{
    
}

svt_point_cloud_pdb< svt_vector4<Real64> >& svt_flexing_inverse_dist_weighting::interpolate()
{
    computeCodebooks2AtomsDist();
    computeFlexedConformation();

    m_oFlexedConf.addRemark(addParameters2Comments());
 
    return m_oFlexedConf;
}

void svt_flexing_inverse_dist_weighting::computeCodebooks2AtomsDist() 
{
    svt_flexing::computeAtoms2CodebooksDist();
}

/**
 * Compute the positions of the flexed structure...
 */ 
void svt_flexing_inverse_dist_weighting::computeFlexedConformation()
{
    svt_vector4<Real64> oSumWeights2Displacement;
    svt_vector4<Real64> oZeroVector(0);
    svt_vector4<Real64> oWeightXDisplacement;
    float fWeightsSum;
    float fCodebookWeight4Atom;
    
    for (unsigned int iIndexAtom = 0; iIndexAtom < m_oOrigConf.size(); iIndexAtom++)
    {
	oSumWeights2Displacement = oZeroVector;
	fWeightsSum = 0;
	if (toupper(m_pWeightingMethod[0]) == 'L') 
            m_fMaxDist2Cdbks4Atom  = m_oAtoms2CodebooksDistances.nthMaxOnRow(iIndexAtom,m_iNbCdbksOfInfluence);
	
	for(unsigned int iIndexCodebook = 0; iIndexCodebook < m_oOrigConfCodebooks.size(); iIndexCodebook++)
	{
	    if (m_oAtoms2CodebooksDistances[iIndexAtom][iIndexCodebook] > 1e-6)
	    { 
		// compute the weights
		fCodebookWeight4Atom        = getCodebookWeight4Atom(iIndexCodebook,iIndexAtom);
		//compute  weights * displacement
		oWeightXDisplacement        = m_aCodebookDisplacements[iIndexCodebook];
		oWeightXDisplacement       *= fCodebookWeight4Atom;
		
		//add to the sums 
		fWeightsSum                += fCodebookWeight4Atom;
		oSumWeights2Displacement   += oWeightXDisplacement;
                
	    }
	    else //a codebook is superposed with an atom
	    {
		oSumWeights2Displacement    = m_aCodebookDisplacements[iIndexCodebook];
		fWeightsSum                 = 1.0;
		iIndexCodebook              = m_oOrigConfCodebooks.size(); // exit loop
	    } 
	}

	oSumWeights2Displacement *= 1/fWeightsSum;
	m_oFlexedConf[iIndexAtom] += oSumWeights2Displacement;
        
        
        m_oFlexedConf.getAtom(iIndexAtom)->setTempFact(oSumWeights2Displacement.length());
    }
    
    

}

/**
 * Computs the weight of the iIndexCodebook codebook vector for the atom iIndexAtom
 * \param iIndexCodebook the index of the codebook vector
 * \param iIndexAtom the index of the atom
 * \return a float value representing the weight
 */
float svt_flexing_inverse_dist_weighting::getCodebookWeight4Atom(unsigned int iIndexCodebook, unsigned int iIndexAtom)
{
    float fWeight;
    
    switch (toupper(m_pWeightingMethod[0]))
    {
	case 'G':
	    //pure inverse distance weighting
	    fWeight =  1.0/pow(double(m_oAtoms2CodebooksDistances[iIndexAtom][iIndexCodebook]),double(m_fWeightPow)); 
	    break;
	case 'L':
	    float fDist,fR;
	    fR    = m_fMaxDist2Cdbks4Atom;
	    fDist = m_oAtoms2CodebooksDistances[iIndexAtom][iIndexCodebook];
            if (fR > fDist)
	       fWeight = pow(double((fR-fDist)/(fR*fDist)),double(m_fWeightPow));
	    else
		fWeight = 0;
	    break;
	case 'E'://exponential
	    Real64 fPow;
	    fPow = pow(double(m_oAtoms2CodebooksDistances[iIndexAtom][iIndexCodebook]),double(m_fWeightPow));
	    fWeight =  1.0/fPow*exp(-1.0*fPow);
	    break;
	default:
	    fWeight =  1.0/pow(double(m_oAtoms2CodebooksDistances[iIndexAtom][iIndexCodebook]),double(m_fWeightPow)); 
	    break;
    }
    
    return fWeight;

}

/**
 * Include comments/remarks in the output pdb files
 * \return the comment to add to the pdb structures
 * \see svt_point_cloud_pdb:addRemark(...)
 */ 
string svt_flexing_inverse_dist_weighting::addParameters2Comments()
{
    string oRemarks = svt_flexing::addParameters2Comments();
    char oRemark[1000]; 
    
    oRemarks = "Flexing Method: Inverse Distance Weighting\n";
    oRemarks += "Weighting method: ";
    
    switch (toupper(m_pWeightingMethod[0]))
    {
	case 'G':
	    //pure inverse distance weighting
	    oRemarks += "negative power of the distance. ";
	    break;
	case 'L':
	    // Hybrid: also uses the distance to the most distant codebook
	    oRemarks += "power of (1/distance-1/R); where R is the most distant codebook vector. ";
	    break;
	default:
	    oRemarks += "negative power of the distance. ";
	    break;
    }
    
    sprintf(oRemark,"Power of the weight: %g\n", m_fWeightPow);
    oRemarks += oRemark;
    
    return oRemarks;
}


