#include <svt_gacylinder_ind.h>
/**
 * the constructor
 */ 
svt_gacylinder_identifySse_ind::svt_gacylinder_identifySse_ind():svt_ga_ind(),
m_iSseCount(0)
{
};

/**
 * the destructor
 */ 
svt_gacylinder_identifySse_ind::~svt_gacylinder_identifySse_ind(){};

/**
 * set the number of sse
 */
void svt_gacylinder_identifySse_ind::setSseCount(int iSseCount)
{
    m_iSseCount = iSseCount;
};

/**
 * get the number of sse
 */
int svt_gacylinder_identifySse_ind::getSseCount()
{
    return m_iSseCount;
};

/**
 * set the number of intersections
 */
void svt_gacylinder_identifySse_ind::setIntersectionCount(int iIntersectionCount)
{
    m_iIntersectionCount = iIntersectionCount;
};

/**
 * get the number of intersections
 */
int svt_gacylinder_identifySse_ind::getIntersectionCount()
{
    return m_iIntersectionCount;
};

/**
 * set the mapping
 */
void svt_gacylinder_identifySse_ind::setMapping(vector<int>& oMappingID)
{
    m_oMappingID = oMappingID;
};

/**
 * get the mapping
 */
vector<int>& svt_gacylinder_identifySse_ind::getMapping()
{
    return m_oMappingID;
};
/**
 * set the cylinders
 */
void svt_gacylinder_identifySse_ind::setCylinders(vector< svt_point_cloud_pdb<svt_ga_vec> >& oCylinders)
{
    m_oCylinders = oCylinders;
};
/**
 * get the cylinders
 */
vector< svt_point_cloud_pdb<svt_ga_vec> >&  svt_gacylinder_identifySse_ind::getCylinders()
{
    return m_oCylinders;
};
/**
 * get the cylinders into a pdb model
 */
void svt_gacylinder_identifySse_ind::getCylinders2Model( svt_point_cloud_pdb<svt_ga_vec> * pModel)
{
    pModel->deleteAllAtoms();
    for (unsigned int iIndex=0; iIndex < m_oCylinders.size(); iIndex++)
    {
        if (pModel->size()==0)
            *pModel = m_oCylinders[iIndex];
        else
            pModel->append(m_oCylinders[iIndex], iIndex);
    }
    pModel->calcAtomModels();
};

/**
 * compute the number of Sse in the individual
 */
void svt_gacylinder_identifySse_ind::computeSseCount()
{
    unsigned int iSize = getGeneCount();
    m_iSseCount = 0;
    for (unsigned int iIndex=0; iIndex<iSize; iIndex++)
    {
        if (getGene(iIndex)==1)
            m_iSseCount++;
    }
}; 
/**
 * print genes to cout
 */
void svt_gacylinder_identifySse_ind::printGenes()
{
    char pOut[1024], pOutTmp[256];

    sprintf(pOut, "%8.6f %2d %3d %3d - ", getFitness(), getOrigin(),  getSseCount(), getIntersectionCount());
    
    for (int i=0; i< getGeneCount(); i++)
    {
        sprintf(pOutTmp, "%3.1f", getGene(i));
        strcat(pOut, pOutTmp);
    }
    cout << pOut << endl;
};


///////////////////////////////////////////////////////////////////////////////
// svt_gacylinder_ind class
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 */
svt_gacylinder_ind::svt_gacylinder_ind():svt_ga_ind(),
m_bWrote(false),
m_iTurns( 10 ),
m_fHeightTurn(5.1/4.0)
{
    m_oTrans.loadIdentity();
}

/**
 * create the coarse phenotype (equivalent simple pdb) but don't fill yet atomic coordinates
 * \param number of units
 */
void svt_gacylinder_ind::buildCoarsePhenotype( )
{
    m_oCoarsePhenotype.clear();
    svt_vector4<Real64> oVec;

    //oVec.x(0.0f); oVec.y(0.0f); oVec.z( -(Real64)m_iTurns*m_fHeightTurn);
    oVec.x(0.0f); oVec.y(0.0f); oVec.z( 0.0 );
    m_oCoarsePhenotype.push_back( oVec );
 
    //oVec.x(0.0f); oVec.y(0.0f); oVec.z( (Real64)m_iTurns*m_fHeightTurn);
    oVec.x(0.0f); oVec.y(0.0f); oVec.z( 1.0 );
    m_oCoarsePhenotype.push_back( oVec );
};

/**
 * update the coarse phenotype for the given unit 
 * \param transformation matrix for that unit
 */
void svt_gacylinder_ind::updateCoarsePhenotype(svt_ga_mat oMat)
{
    buildCoarsePhenotype();

    for (unsigned int iPoint=0; iPoint < m_oCoarsePhenotype.size(); iPoint++)
    	m_oCoarsePhenotype[iPoint] = oMat * m_oCoarsePhenotype[iPoint]  ;
    setTrans( oMat );
}


/**
 * get the coarse phenotype
 */
svt_point_cloud_pdb< svt_vector4<Real64 > > svt_gacylinder_ind::getCoarsePhenotype()
{
    svt_point_cloud_pdb<svt_vector4<Real64> > oCoarsePhenotype;    
    svt_point_cloud_atom oAtom;

    for (unsigned int iIndex=0; iIndex < (Real64)m_oCoarsePhenotype.size(); iIndex++ ) 
	    oCoarsePhenotype.addAtom(oAtom, m_oCoarsePhenotype[iIndex] );

    return oCoarsePhenotype;
};

/**
 * calculate the distance between two individuals
 * \param rOther reference to the other individual
 * \return vector distance between the two gene-vectors
 */
Real64 svt_gacylinder_ind::distance( svt_gacylinder_ind& rOther )
{ 
    Real64 fDist = 0.0;
    if (m_oCoarsePhenotype.size()!= rOther.m_oCoarsePhenotype.size() || m_oCoarsePhenotype.size()==0)
    {
	svtout << "Can not compute distance" << m_oCoarsePhenotype.size() << " " << rOther.m_oCoarsePhenotype.size() << " " <<  m_oCoarsePhenotype.size() <<  endl;
	return 0.0;
    }
/*
    svt_vector4< Real64 > oDiffThis  = m_oCoarsePhenotype[1]-m_oCoarsePhenotype[0];
    svt_vector4< Real64 > oDiffOther = rOther.m_oCoarsePhenotype[1]-rOther.m_oCoarsePhenotype[0];
    svt_vector4< Real64 > oDiff = m_oCoarsePhenotype[0] - rOther.m_oCoarsePhenotype[0];

    svt_vector4< Real64 > oCrossProd;
    oCrossProd.x( oDiffThis.y()*oDiffOther.z() - oDiffThis.z()*oDiffOther.y() );
    oCrossProd.y( oDiffThis.z()*oDiffOther.x() - oDiffThis.x()*oDiffOther.z() );
    oCrossProd.z( oDiffThis.x()*oDiffOther.y() - oDiffThis.y()*oDiffOther.x() );

    fDist = (oDiff.x()*oCrossProd.x()+oDiff.y()*oCrossProd.y()+oDiff.z()*oCrossProd.z() )/oCrossProd.length();

    return fDist;
*/

    
    for(unsigned int i=0; i<1/*m_oCoarsePhenotype.size()*/; i++)
        fDist += m_oCoarsePhenotype[i].distanceSq(rOther.m_oCoarsePhenotype[i]);

    //return sqrt( fDist/(Real64)((Real64)m_oCoarsePhenotype.size()/4.0f) );
    return sqrt( fDist );

};

/**
 * set Wrote on disk
 * \param bWrote whether it was already wrote on disk
 */
void svt_gacylinder_ind::setWrote( bool bWrote )
{
    m_bWrote = bWrote;
};

/**
 * get Wrote on disk
 * \return bWrote whether it was already wrote on disk
 */
bool svt_gacylinder_ind::getWrote()
{
    return m_bWrote;
};

/**
 * get the number of turns
 * \return the number of turns
 */
unsigned int svt_gacylinder_ind::getTurns()
{
    return m_iTurns;
};

/**
* get the number of turns
* \param the number of turns
*/
void svt_gacylinder_ind::setTurns( unsigned int iTurns)
{
    m_iTurns = iTurns;
};

 
/**
 * get the height of a turn
 * \return the height
 */
Real64 svt_gacylinder_ind::getHeightTurn()
{
    return m_fHeightTurn;
};

/**
* set the height of a turn
* \param the height
*/
void svt_gacylinder_ind::setHeightTurn( Real64 fHeightTurn)
{
    m_fHeightTurn = fHeightTurn;
};

/**
 * get the Transformation
 * \return the transformation
 */	
svt_ga_mat svt_gacylinder_ind::getTrans()
{
    return m_oTrans;
};

/**
 * set the Transformation
 * \param the transformation
 */	
void svt_gacylinder_ind::setTrans(svt_ga_mat& rTrans)
{
    m_oTrans = rTrans;
};


/**
 * set Fitness Top
 * \param the new fitness
 */ 
void svt_gacylinder_ind::setFitnessTop( Real64 fFitness)
{
    m_fFitnessTop = fFitness;
};

/**
 * get Fitness Top
 * \return the new fitness
 */ 
Real64 svt_gacylinder_ind::getFitnessTop()
{
    return m_fFitnessTop;
};

/**
 * set Fitness Bot
 * \param the new fitness
 */ 
void svt_gacylinder_ind::setFitnessBot( Real64 fFitness)
{
    m_fFitnessBot = fFitness;
};

/**
 * get Fitness Top
 * \return the new fitness
 */ 
Real64 svt_gacylinder_ind::getFitnessBot()
{
    return m_fFitnessBot;
};

