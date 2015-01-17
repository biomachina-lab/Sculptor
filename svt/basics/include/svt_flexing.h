/***************************************************************************
                          svt_flexing
                          -----------
    begin                : 09/10/2006
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_iostream.h>
#include <svt_point_cloud_pdb.h>
#include <svt_vector4.h>
#include <svt_member2.h>

#ifndef SVT_FLEXING_H
#define SVT_FLEXING_H
/**
 * A class to flex molecular structures based on positions of feature vectors.
 * Two sets of features are necessary, one that describes the current state and another
 * one that represents the new conformation. The class provides methods to generate the
 * full atomic structure of the new conformation described by the second feature-point set.
 * This is only a base-class, several derived classes implement a variety of global interpolation
 * strategies to deform the molecular structure.
 *
 * \author Mirabela Rusu
 */


class DLLEXPORT svt_flexing
{

protected:

    svt_point_cloud_pdb< svt_vector4<Real64> > m_oOrigConf;
    svt_point_cloud_pdb< svt_vector4<Real64> > m_oFlexedConf;
    svt_point_cloud< svt_vector4<Real64> >     m_oOrigConfCodebooks;
    svt_point_cloud< svt_vector4<Real64> >     m_oFinalConfCodebooks;
    svt_matrix<Real32>                         m_oAtoms2CodebooksDistances;
     
    vector< svt_vector4<Real64> >              m_aCodebookDisplacements;
    string                                     m_pDistType; //Cartesian,  Skeleton
    string                                     m_pDistInGraphSchema; // Minimum, Average used only if Skeleton used for m_pDistType
    
    //flag: if true then the all atoms are flexed; false to indicate the only coordinates of the atoms on the backbone 
    //are flexed and the side chain moves the same way as the CA; 
    bool m_bFlexAllAtoms; 
    
    //the side chain atoms move like the Calpha, the backbone is computed using the interpolation method
    // index = atom index; value index of the CA
    vector<int> m_oMoveLike;
    
    // store the index of Ca corresponding to each residue
    vector<int> m_oIndexCAs;
    
public:

    /**
     * Constructor
     */
    svt_flexing(const svt_point_cloud_pdb< svt_vector4<Real64> >    &oOrigConf,
		const svt_point_cloud< svt_vector4<Real64> >        &oOrigConfCodebooks,
		const svt_point_cloud< svt_vector4<Real64> >        &oFinalConfCodebooks,
		bool m_bFlexAllAtoms = false, string pDistType = "Cartesian", string pDistInGraphSchema = "Minimum" );
    
    /**
     * Destructor
     */
    virtual ~svt_flexing();
    
    /**
     */
    virtual svt_point_cloud_pdb< svt_vector4<Real64> >& interpolate() = 0;
    
    /**
     * Sets the Initial conformation of the structure
     */
    void setOrigConf(const svt_point_cloud_pdb< svt_vector4<Real64> > &oOrigConf);
    
    /**
     * Sets the Codebook vectors of the Initial conformation of the structure
     */
    void setOrigConfCodebooks(svt_point_cloud< svt_vector4<Real64> > &oOrigConfCodebooks){m_oOrigConfCodebooks = oOrigConfCodebooks;};
    
    /**
     * Sets the codebook vectors of the FinalInitial conformation of the structure
     */
    void setFinalConfCodebooks(svt_point_cloud< svt_vector4<Real64> > &oFinalConfCodebooks){m_oFinalConfCodebooks = oFinalConfCodebooks;};
    
    /**
     * Computes the distances between any atom and any codebook vectors in the original strucuture
     */
    virtual void computeAtoms2CodebooksDist();
    
    /**
     * Print the distances between the atom (with the index iAtomIndex ) and all the codebook vectors
     * \param iAtomIndex an integer value representing the index of the atom
     */
    void printAtom2CodebooksDist(unsigned int iAtomIndex);
    
     /**
      */
    virtual void computeFlexedConformation() = 0;
    
    /**
     * Compute displacement vectors (vector from the initial to the final position) of the codebook vectors
     */
    void computeCodebooksDisplacements();
    
    /**
     * Print Codebook Displacement
     */
    void printCodebookDisplacements();
    
    /**
     * set the distance type : "cartesian" or "graph"
     * \param pDistType a string starting with c for Cartesian distance or starting with g for Graph distance 
     */
    virtual void setDistType(string pDistType);
    
    /**
     * gets the distance type
     * \return a string representing the distance type 
     */
    virtual string getDistType() const{return m_pDistType;}
    
    /**
    * sets the DistInGraphSchema
    * \param iDistInGraphSchema
    */
    void setDistInGraphSchema(string pDistInGraphSchema){m_pDistInGraphSchema = pDistInGraphSchema;}
    
    /**
     * Set the original and final configuration for the codebook, and computes the displacement vector
     * \param oCodebookInitial the vector containing the coordinates to assign to the initial Configuration
     * \param oCodebookFinal the vector containing the coordinates to assign to the final Configuration
     * \param oFilename if not NULL is used to append the Initial state of the codebooks vectors 
     * (Ideal when the interpolation is done in several steps) 
     */
    void setCodebooks(const vector<svt_vector4<Real64> > &oCodebookInitial, 
		      const vector<svt_vector4<Real64> > &oCodebookFinal   ,
		      char* oFilename = NULL);
  
    
    protected:
    
	/**
     * Compute the distance between an atom and a codebook in a graph
     * \param aDistIndexAtom2Cdbk the vector of couples (distance atom codebook, index codebook) sorted by the distances
     * \param iIndexCdbk an int value - the index of the codebook vectors
     * \return a real value representing the distance between the the atom and the codebook vector
     */
    //Real64 getGraphDistAtom2Cdbk(vector<distIndexAtom2Cdbk> aDistIndexAtom2Cdbks, unsigned int iIndexCdbk);
    
    /**
     * Compute the distance between an atom and a codebook in a graph
     * \param aDistIndexAtom2Cdbk the vector of couples (distance atom codebook, index codebook) sorted by the distances
     * \param iIndexCdbk an int value - the index of the codebook vectors
     * \return a real value representing the distance between the the atom and the codebook vector
     */
    Real64 getGraphDistAtom2Cdbk(vector<svt_member2<Real64,unsigned int> > aDistIndexAtom2Cdbks, unsigned int iIndexCdbk);
    
    /**
     * Include comments/remarks in the output pdb files
     * \return the comment to add to the pdb structures
     * \see svt_point_cloud_pdb:addRemark(...)
     */ 
    virtual string addParameters2Comments();
    
    /**
     * Set the codebook in the initial conformation 
     */
    void setInitialConfCodebook(const vector<svt_vector4<Real64> > &oCodebook);
    
    /**
     * Set the codebook in the final conformation
     */
    void setFinalConfCodebook(const vector<svt_vector4<Real64> > &oCodebook);
    
    
    /**
     * print pdb parameter
     * \param iIndexParam integer value representing which param to be print
     */
    void printPdbCoords(unsigned int iIndexParam);
};

#endif//INTERPOLATION_H
