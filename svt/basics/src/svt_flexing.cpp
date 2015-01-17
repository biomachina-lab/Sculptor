/***************************************************************************
                          svt_flexing
                          -----------
    begin                : 09/10/2006
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_flexing.h>
#include <svt_iostream.h>
#include <svt_point_cloud.h>
#include <svt_point_cloud_atom.h>
#include <svt_vector4.h>
#include <svt_member2.h>
#include <time.h>

#define FLEX cout << svt_trimName("flexing")
using namespace std;

/**
 * Constructor
 */
svt_flexing::svt_flexing(const svt_point_cloud_pdb< svt_vector4<Real64> >   &oOrigConf,
			 const svt_point_cloud< svt_vector4<Real64> >       &oOrigConfCodebooks, 
			 const svt_point_cloud< svt_vector4<Real64> >       &oFinalConfCodebooks,
			 bool bFlexAllAtoms, string pDistType, string pDistInGraphSchema )
{
    //initialize the coordinates
    m_oOrigConf                 = oOrigConf;
    m_oOrigConfCodebooks        = oOrigConfCodebooks;
    m_oFinalConfCodebooks       = oFinalConfCodebooks;
    
    computeCodebooksDisplacements();
    
    //initialize the distance/flexing options
    m_bFlexAllAtoms             = bFlexAllAtoms;
    m_pDistType                 = pDistType;
    m_pDistInGraphSchema        = pDistInGraphSchema;
    
    //initialize the flexing coordinates originally the coordinates of the initial conformation
    for (unsigned int iIndexAtom=0; iIndexAtom < m_oOrigConf.size(); iIndexAtom++)
	m_oFlexedConf.addAtom(*m_oOrigConf.getAtom(iIndexAtom), m_oOrigConf[iIndexAtom]);
    
}
/**
 * Destructor
 */
svt_flexing::~svt_flexing()
{
}

/**
 * Sets the Initial conformation of the structure
 */
void svt_flexing::setOrigConf(const svt_point_cloud_pdb< svt_vector4<Real64> > &oOrigConf)
{
    m_oOrigConf = oOrigConf;
    
    m_oFlexedConf.deleteAllAtoms();
    m_oFlexedConf.deleteAllBonds();
    for (unsigned int iIndexAtom=0; iIndexAtom < m_oOrigConf.size(); iIndexAtom++)
	m_oFlexedConf.addAtom(*m_oOrigConf.getAtom(iIndexAtom), m_oOrigConf[iIndexAtom]);
};


/**
 * Computes the distances between any atom and any codebook vectors in the original strucuture
 */
void svt_flexing::computeAtoms2CodebooksDist()
{
    unsigned int iIndex;
    unsigned int iIndexAtom;
    unsigned int iIndexCdbk;
    
    //compute once the distance between the CA and the codebooks; not several times for each atom 
    vector<unsigned int> oIndexCAs;
    vector<bool> oWasDistCA2CdbksComputed;
    vector<Real64> oDist1CA2Cdbks;
    
    m_oAtoms2CodebooksDistances.resize(m_oOrigConf.size(),m_oOrigConfCodebooks.size());

    switch (toupper(m_pDistType[0]))
    {
	case ('C')://cartesian distance
	    for (iIndexAtom=0; iIndexAtom < m_oAtoms2CodebooksDistances.height(); iIndexAtom++)
	    {
		//check if all the atoms are to be flexed (or just the one of the backbone)
		if (m_bFlexAllAtoms) //all atoms 
		{
		    iIndex = iIndexAtom;
		    m_oMoveLike.push_back(iIndex);
                    oWasDistCA2CdbksComputed.push_back(false);
		}
		else// only CA/backbone
		{
		    iIndex = m_oOrigConf.getCA(iIndexAtom);
		    m_oMoveLike.push_back(iIndex);
		    
		    if (oIndexCAs.size()>1)
		    {
			if (oIndexCAs[oIndexCAs.size()-1]!=iIndex) // the previous CA is the same as the one just computed
		          { 
			    oIndexCAs.push_back(iIndex);
			    oWasDistCA2CdbksComputed.push_back(false);
			  }
		    }
		    else
		    {
			oIndexCAs.push_back(iIndex);
			oWasDistCA2CdbksComputed.push_back(false);
		    }
		}
		
		if (!oWasDistCA2CdbksComputed[oWasDistCA2CdbksComputed.size()-1])
		{// if the distances to the CA were not computed 
		    oDist1CA2Cdbks.clear();
		    for (iIndexCdbk = 0; iIndexCdbk < m_oAtoms2CodebooksDistances.width(); iIndexCdbk++)
		    {
		          m_oAtoms2CodebooksDistances[iIndexAtom][iIndexCdbk] = m_oOrigConf[iIndex].distance(m_oOrigConfCodebooks[iIndexCdbk]);
			  oDist1CA2Cdbks.push_back(m_oAtoms2CodebooksDistances[iIndexAtom][iIndexCdbk]);
		    }
		    oWasDistCA2CdbksComputed[oWasDistCA2CdbksComputed.size()-1] = true;
		}
		else //distances already computed; just assign them them 
		    for (iIndexCdbk = 0; iIndexCdbk < m_oAtoms2CodebooksDistances.width(); iIndexCdbk++)
			m_oAtoms2CodebooksDistances[iIndexAtom][iIndexCdbk] = oDist1CA2Cdbks[iIndexCdbk];
		
	    }
            /*
            for (unsigned int i=0; i<m_oMoveLike.size(); i++)
            {
                if (m_oOrigConf.getAtom(i)->getResidueSeq() != m_oOrigConf.getAtom(m_oMoveLike[i])->getResidueSeq())
                svtout << i << "->" << m_oMoveLike[i] << "--"<< m_oOrigConf.getAtom(i)->getResidueSeq() << "-->" << m_oOrigConf.getAtom(m_oMoveLike[i])->getResidueSeq()<<endl;
            }
            */
	    break;
	case ('G'):
	    vector<svt_member2<Real64,unsigned int> > aDistIndexAtom2Cdbks;
	    svt_member2<Real64,unsigned int> oDistIndex(0,0);

	    ((svt_point_cloud_pdb<svt_vector4< Real64> >)m_oOrigConfCodebooks).computePSFGraphDistMat();	    
	    
	    for (iIndexAtom=0; iIndexAtom < m_oAtoms2CodebooksDistances.height(); iIndexAtom++)
	    {
		//get the CA
		iIndex = m_oOrigConf.getCA(iIndexAtom);		
		
		//compute the cartesian dist to all codebook vectors to determine the closest ones
		aDistIndexAtom2Cdbks.clear();
		for (iIndexCdbk = 0; iIndexCdbk < m_oAtoms2CodebooksDistances.width(); iIndexCdbk++)
		{
		    oDistIndex.setNumber(m_oOrigConf[iIndex].distance(m_oOrigConfCodebooks[iIndexCdbk]));
		    oDistIndex.setInfo(iIndexCdbk);
		    aDistIndexAtom2Cdbks.push_back(oDistIndex);
		}
		
		//sort the vector
		sort(aDistIndexAtom2Cdbks.begin(),aDistIndexAtom2Cdbks.end());
		
		
		//getDist2Graph
		for (iIndexCdbk = 0; iIndexCdbk < m_oAtoms2CodebooksDistances.width(); iIndexCdbk++)
		{
		    m_oAtoms2CodebooksDistances[iIndexAtom][iIndexCdbk] = getGraphDistAtom2Cdbk(aDistIndexAtom2Cdbks,iIndexCdbk);
		}

	    } 
	    break;
    }
}


/**
 * Print the distances between the atom (with the index iAtomIndex ) and all the codebook vectors
 * \param iAtomIndex an integer value representing the index of the atom
 */
void svt_flexing::printAtom2CodebooksDist(unsigned int iAtomIndex)
{
    cout << "The distances between the atom " << iAtomIndex << " and the codebook vectors:" << endl;
    for (unsigned int j=0; j < m_oAtoms2CodebooksDistances.width();j++)
    {
	cout << m_oAtoms2CodebooksDistances[iAtomIndex][j] <<'\t';
    }
    cout << endl;
}

/**
 * Compute displacement vectors (vector from the initial to the final position) of the codebook vectors
 */
void svt_flexing::computeCodebooksDisplacements()
{
    m_aCodebookDisplacements.clear();
    for (unsigned int iIndex = 0; iIndex < m_oOrigConfCodebooks.size(); iIndex++)
    {
	m_aCodebookDisplacements.push_back(m_oFinalConfCodebooks[iIndex]-m_oOrigConfCodebooks[iIndex]);
    }
}

/**
 * Print Codebook Displacement
 */
void svt_flexing::printCodebookDisplacements()
{
    cout << "The " << m_aCodebookDisplacements.size() << " codebook vectors displacement" << endl;
    for(unsigned int iIndex = 0; iIndex < m_aCodebookDisplacements.size(); iIndex++)
    {
	cout << iIndex <<":\t";
	cout << m_aCodebookDisplacements[iIndex] << endl;
    }
    
}

/**
 * Compute the distance between an atom and a codebook in a graph
 * \param aDistIndexAtom2Cdbk the vector of couples (distance atom codebook, index codebook) sorted by the distances
 * \param iIndexCdbk an int value - the index of the codebook vectors
 * \return a real value representing the distance between the the atom and the codebook vector
 */
Real64 svt_flexing::getGraphDistAtom2Cdbk(vector<svt_member2<Real64, unsigned int> > aDistIndexAtom2Cdbks, unsigned int iIndexCdbk)
{

    unsigned int iIndex;
    Real64 fDistAtom2Cdbk;
    
    Real64 distTroughClosestCdbk, distTrough2ndClosestCdbk;
    Real64 weightClosestCdbk, weight2ndClosestCdbk;
	    
    
    switch(toupper(m_pDistInGraphSchema[0]))
    {
	case 'A'://the adverage    
        // Closest codebook vector
	    iIndex = 0;
	    distTroughClosestCdbk = aDistIndexAtom2Cdbks[iIndex].getNumber()+
		    ((svt_point_cloud_pdb<svt_vector4< Real64> >)m_oOrigConfCodebooks).getGraphDists()[aDistIndexAtom2Cdbks[iIndex].getInfo()][iIndexCdbk];
	    weightClosestCdbk = 1/aDistIndexAtom2Cdbks[iIndex].getNumber();
    // 2nd closest codebook vector
	    iIndex = 1;
	    distTrough2ndClosestCdbk = aDistIndexAtom2Cdbks[iIndex].getNumber()+
		    ((svt_point_cloud_pdb<svt_vector4< Real64> >)m_oOrigConfCodebooks).getGraphDists()[aDistIndexAtom2Cdbks[iIndex].getInfo()][iIndexCdbk];
	    weight2ndClosestCdbk = 1/aDistIndexAtom2Cdbks[iIndex].getNumber();

    //compute the distance to the codebook iIndexCdbk
	    fDistAtom2Cdbk = (weightClosestCdbk*distTroughClosestCdbk+
		    weight2ndClosestCdbk*distTrough2ndClosestCdbk)/(weightClosestCdbk+weight2ndClosestCdbk);
	    break;
	case 'M'://min
	    iIndex = 0;
	    distTroughClosestCdbk = aDistIndexAtom2Cdbks[iIndex].getNumber()+
		    ((svt_point_cloud_pdb<svt_vector4< Real64> >)m_oOrigConfCodebooks).getGraphDists()[aDistIndexAtom2Cdbks[iIndex].getInfo()][iIndexCdbk];
	    
            // 2nd closest codebook vector
	    iIndex = 1;
	    distTrough2ndClosestCdbk = aDistIndexAtom2Cdbks[iIndex].getNumber()+
		    ((svt_point_cloud_pdb<svt_vector4< Real64> >)m_oOrigConfCodebooks).getGraphDists()[aDistIndexAtom2Cdbks[iIndex].getInfo()][iIndexCdbk];
	    
	    fDistAtom2Cdbk = svt_min(distTroughClosestCdbk,distTrough2ndClosestCdbk);
	    break;
	default:
	    //min
	    iIndex = 0;
	    distTroughClosestCdbk = aDistIndexAtom2Cdbks[iIndex].getNumber()+
		    ((svt_point_cloud_pdb<svt_vector4< Real64> >)m_oOrigConfCodebooks).getGraphDists()[aDistIndexAtom2Cdbks[iIndex].getInfo()][iIndexCdbk];
	    
            // 2nd closest codebook vector
	    iIndex = 1;
	    distTrough2ndClosestCdbk = aDistIndexAtom2Cdbks[iIndex].getNumber()+
		    ((svt_point_cloud_pdb<svt_vector4< Real64> >)m_oOrigConfCodebooks).getGraphDists()[aDistIndexAtom2Cdbks[iIndex].getInfo()][iIndexCdbk];
	    
	    fDistAtom2Cdbk = svt_min(distTroughClosestCdbk,distTrough2ndClosestCdbk);
	    break;
    
    }
    return fDistAtom2Cdbk;
    

}

/**
 * set the distance type : "cartesian" or "graph"
 * \param pDistType a string starting with c for Cartesian distance or starting with g for Graph distance 
 */
 void svt_flexing::setDistType(string pDistType)
{
    m_pDistType = pDistType;
}

 /**
 * Include comments/remarks in the output pdb files
 * \return the comment to add to the pdb structures
 * \see svt_point_cloud_pdb:addRemark(...)
 */ 
string svt_flexing::addParameters2Comments()
{
    char oRemark[1000];
    string oRemarks;
    
    oRemarks = "Flexing Method by sculptor@biomachina.org\nOrganisation: School of Health Informatics Sciences, Houston\n";
    
    //Insert today's date and hour
    time_t rawtime;
    time ( &rawtime );
    ctime (&rawtime);
    sprintf(oRemark, "Created on %s",ctime (&rawtime));
    oRemarks += oRemark;
    
    //Insert parameters
    switch (toupper(m_pDistType[0])){
	case 'C':
	    oRemarks += "Distance Type: Cartesian; "; 
	    break;
	case 'G':
	    oRemarks += "Distance Type: Graph; ";
	    oRemarks += "The distance from the atom to the graph is based on the";
	    switch(toupper(m_pDistInGraphSchema[0]))
	    {
		case 'A'://the adverage    
		    oRemarks += " average.";
		    break;
		case 'M':
		    oRemarks += " minimum.";
		    break;    
		default:
		    oRemarks += " minimum."; 
		    break; 
	    }
	    break;
	    
	default:
	    oRemarks += "Distance Type: Cartesian; ";
	    break;    
    }
    
    oRemarks += "\n";
    
    return oRemarks;
}

 /**
 * Set the codebook in the initial conformation 
 */
void svt_flexing::setInitialConfCodebook(const vector<svt_vector4<Real64> > &oCodebook)
{
    if (m_oOrigConfCodebooks.size() != oCodebook.size())
    {
	svtout << "ERROR: The source and destination codebook should have the same size." << endl;
	return;
    }
    
    for (unsigned int iIndex=0; iIndex < m_oOrigConfCodebooks.size(); iIndex ++)
    {
	m_oOrigConfCodebooks[iIndex][0] = oCodebook[iIndex].x();
	m_oOrigConfCodebooks[iIndex][1] = oCodebook[iIndex].y();
	m_oOrigConfCodebooks[iIndex][2] = oCodebook[iIndex].z();
    }
};

    
 /**
  * Set the codebook in the final conformation
  */
void svt_flexing::setFinalConfCodebook(const vector<svt_vector4<Real64> > &oCodebook)
{
    
    if (m_oFinalConfCodebooks.size() != oCodebook.size())
    {
	svtout << "ERROR: The source and destination codebook should have the same size." << endl;
	return;
    }
    
    for (unsigned int iIndex=0; iIndex < m_oFinalConfCodebooks.size(); iIndex ++)
    {
	m_oFinalConfCodebooks[iIndex][0] = oCodebook[iIndex].x();
	m_oFinalConfCodebooks[iIndex][1] = oCodebook[iIndex].y();
	m_oFinalConfCodebooks[iIndex][2] = oCodebook[iIndex].z();
    }

};

/**
 * Set the original and final configuration for the codebook, and computes the displacement vector
 * \param oCodebookInitial the vector containing the coordinates to assign to the initial Configuration
 * \param oCodebookFinal the vector containing the coordinates to assign to the final Configuration
 */
void svt_flexing::setCodebooks( const vector<svt_vector4<Real64> > &oCodebookInitial, 
				const vector<svt_vector4<Real64> > &oCodebookFinal   ,
			        char * oFilename                                         )
{
    setInitialConfCodebook          (oCodebookInitial);
    setFinalConfCodebook            (oCodebookFinal  );
    computeCodebooksDisplacements   ();
    
    if (oFilename!=NULL) 
	m_oFinalConfCodebooks.writePDB(oFilename,true);
};


/**
 * print pdb parameter
 * \param iIndexParam integer value representing which param to be print
 */
void svt_flexing::printPdbCoords(unsigned int iIndexParam)
{
    unsigned int iIndexAtom;
    switch(iIndexParam)
    {
	case 0:// Original Conformation structure
	    for (iIndexAtom = 0; iIndexAtom < m_oOrigConf.size(); iIndexAtom++)
	    {
		svtout << m_oOrigConf[iIndexAtom].x() << "\t" 
		       << m_oOrigConf[iIndexAtom].y() << "\t"
		       << m_oOrigConf[iIndexAtom].z() << endl;
	    }
	    break;
	    case 1://Final Conformation(Flexed) structure
	    for (iIndexAtom = 0; iIndexAtom < m_oFlexedConf.size(); iIndexAtom++)
	    {
		svtout << m_oFlexedConf[iIndexAtom].x() << "\t" 
		       << m_oFlexedConf[iIndexAtom].y() << "\t"
		       << m_oFlexedConf[iIndexAtom].z() << endl;
	    }
	    break;
	case 2: //Original conformation codebook
	    for (iIndexAtom = 0; iIndexAtom < m_oOrigConfCodebooks.size(); iIndexAtom++)
	    {
		svtout  << m_oOrigConfCodebooks[iIndexAtom].x() << "\t" 
			<< m_oOrigConfCodebooks[iIndexAtom].y() << "\t"
			<< m_oOrigConfCodebooks[iIndexAtom].z() << endl;
	    }
	    break;
	case 3: //Final Conformation codebook
	    for (iIndexAtom = 0; iIndexAtom < m_oFinalConfCodebooks.size(); iIndexAtom++)
	    {
		svtout  << m_oFinalConfCodebooks[iIndexAtom].x() << "\t" 
			<< m_oFinalConfCodebooks[iIndexAtom].y() << "\t"
			<< m_oFinalConfCodebooks[iIndexAtom].z() << endl;
	    }
	    break;
    }
};
