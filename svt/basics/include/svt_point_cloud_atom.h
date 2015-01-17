/***************************************************************************
                          svt_point_cloud_atom
                          --------------------
    begin                : 02/10/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_POINT_CLOUD_ATOM_H
#define __SVT_POINT_CLOUD_ATOM_H

#include <svt_stlVector.h>

class svt_point_cloud_bond;

typedef struct 
{
    //the three letter codes for the amino acids
    char m_a3LetCode[5];
    // the one letter code
    char m_c1LetCode;

} svt_resname;

// the list with the aa long and short names
static vector<svt_resname> m_oAAList;

/**
* basic class containing the atom information 
*/
class DLLEXPORT svt_point_cloud_atom
{
protected:

    // to which model does this atom belong
    unsigned int m_iModel;
    // PDB-file index of the atom
    unsigned int m_iPDBIndex;

    // atom name (i.e. "C")
    char   m_aName[5];
    // remoteness indicator
    char   m_cRemoteness;
    // branch
    char   m_cBranch;
    // radius
    Real32 m_fRadius;
    // alternate location identifier
    char   m_cAltLoc;
    // atom resname (i.e. "ALA")
    char   m_aResname[5];
    // atom short resname (i.e. "A")
    char   m_cShortResname;
    // chain id
    char   m_cChainID;
    // ordinal chain id (note it's an integer!)
    int    m_iOrdChainID;
    // residue sequence number
    int    m_iResSeq;
    // ordinal res sequence number
    int    m_iOrdResSeq;
    // iCode
    char   m_cICode;
    // the occupancy
    Real32 m_fOccupancy;
    // the temperature factor
    Real32 m_fTempFact;
    // note
    char   m_pNote[4];
    // segment id
    char   m_pSegID[5];
    // element symbol
    char   m_pElement[3];
    // charge
    char   m_pCharge[3];

    // secondary structure information
    // H   Alpha helix
    // G   3-10 helix
    // I   PI-helix
    // E   Extended conformation
    // B   Isolated bridge
    // T   Turn
    // C   Coil (none of the above)
    // N   Information not available
    char   m_cSecStruct;

    // number of residues in the secondary structure this atom belongs to
    int    m_iSecStructNumResidues;

    // relative atomic mass
    Real64 m_fMass;

    // is this atom part of a water molecule?
    bool   m_bWater;
    
    //is this atom an "hetatm" record in the pdb
    bool m_bHetAtm; 

    // bond list
    vector< svt_point_cloud_bond* > m_aBondList;
    // index-based bond list
    vector< unsigned int > m_aBondListI;
    
    // Is Atom selected
    bool m_bIsSelected;


public:

    /**
     * Constructor
     */
    svt_point_cloud_atom();
    /**
     * Destructor
     */
    virtual ~svt_point_cloud_atom();

    /**
     * Set the model this atom belongs to
     * \param iModel number of model
     */
    void setModel( unsigned int iModel );
    /**
     * Get the model this atom belongs to
     * \return number of model
     */
    unsigned int getModel( ) const;

    /**
     * Set the PDB-file index of the atom
     * \param iPDBIndex index of the atom
     */
    void setPDBIndex( unsigned int iPDBIndex );
    /**
     * Get the PDB-file index of the atom
     * \return index of the atom
     */
    unsigned int getPDBIndex( ) const;

    /**
     * set the atom name
     * \param pName pointer to the char array with the name information
     */
    void setName(const char* pType);
    /**
     * get the atom name
     * \return pointer to the char array with the name information
     */
    const char* getName() const;
    /**
     * get the atom remoteness indicator
     * \return char with the remoteness information (greek letters, A=alpha, B=beta,...)
     */
    char getRemoteness() const;
    /**
     * set the atom remoteness indicator
     * \param cRemoteness char with the remoteness information (greek letters, A=alpha, B=beta,...)
     */
    void setRemoteness( char cRemoteness );
    /**
     * get the atom branch information
     * \return char with the branch information
     */
    char getBranch() const;
    /**
     * set the atom branch information
     * \param cBranch char with the branch information
     */
    void setBranch( char cBranch );
    /**
     * set the atom alternate location indicator
     * \param cAltLoc character with the alternate location indicator
     */
    void setAltLoc(char cAltLoc);
    /**
     * get the atom alternate location indicator
     * \return character with the alternate location indicator
     */
    char getAltLoc() const;
    /**
     * set the atom resname
     * \param pResname pointer to the char array with the residue name information
     */
    void setResname(const char* pResname);
    /**
     * get the atom resname
     * \return pointer to the char array with the residue name information
     */
    const char* getResname() const;
     /**
     * set the short atom resname
     * \param pResname a char with the short "1letter" residue name information
     */
    void setShortResname(const char cResname);
    /**
     * get the short atom resname
     * \return a char with the short "1letter" residue name information
     */
    char getShortResname() const;
   /**
     * set the chain id
     * \param cChainID character with the chain id
     */
    void setChainID(char cChainID);
    /**
     * get the chain id
     * \return character with the chain id
     */
    char getChainID() const;
    /**
     * set the chain id
     * \param cOrdChainID character with the chain id
     */
    void setOrdChainID(int iOrdChainID);
    /**
     * get the chain id
     * \return int with the chain id
     */
    int getOrdChainID() const;
    /**
     * set the residue sequence number
     * \param iResSeq residue sequence number
     */
    void setResidueSeq(int iResSeq);
    /**
     * get the residue sequence number
     * \return residue sequence number
     */
    int getResidueSeq() const;
    /**
     * set the ordinal residue sequence number
     * \param iOrdResSeq ordinal residue sequence number
     */
    void setOrdResidueSeq(int iOrdResSeq);
    /**
     * get the ordinal residue sequence number
     * \return ordinal residue sequence number
     */
    int getOrdResidueSeq() const;
    /**
     * set the iCode (code for insertion of residues)
     * \param cICode char with the iCode
     */
    void setICode(char cICode);
    /**
     * get the iCode (code for insertion of residues)
     * \return char with the iCode
     */
    char getICode() const;
    /**
     * set the occupancy
     * \param fOccupancy the occupancy
     */
    void setOccupancy(float fOccupancy);
    /**
     * get the occupancy
     * \return the occupancy
     */
     float getOccupancy() const;
     /**
      * set the temperature factor
      * \param fTempFact the temperature factor
      */
     void setTempFact(float fTempFact);
     /**
      * get the temperature factor
      * \return the temperature factor
      */
     float getTempFact() const;
     /**
      * set the note
      * \param pNote pointer to char array with at least 3 chars for the note
      */
     void setNote(const char *pNote);
     /**
      * get the note
      * \return pointer to char array with the note
      */
     const char* getNote() const;
     /**
      * set the segment id
      * \param pSegID pointer to char array with at least 4 chars for the segment id
      */
     void setSegmentID(const char *pSegID);
     /**
      * get the segment id
      * \return pointer to char array with the segment id
      */
     const char* getSegmentID() const;
     /**
      * set the element symbol
      * \param pElement pointer to char array with at least 2 chars for the element symbol
      */
     void setElement(const char *pElement);
     /**
      * get the element symbol
      * \return pointer to char array with the element symbol
      */
     const char* getElement() const;
     /**
      * set the charge
      * \param pCharge pointer to char array with at least 2 chars for the charge
      */
     void setCharge(const char *pCharge);
     /**
      * get the charge
      * \return pointer to char array with the charge
      */
     const char* getCharge() const;

     /**
      * is the atom a hydrogen atom?
      * \return true if the the atom is an hydrogen atom
      */
     bool isHydrogen() const;
     /**
      * is the atom a QPDB codebook vector?
      * \return true if the the atom is a CV
      */
     bool isQPDB() const;
     /**
      * is the atom part of a water molecule?
      * \return true if the atom os part of a water molecule
      */
     bool isWater() const;
     /**
      * is the atom a carbon alpha?
      * \return true if the atom a CA
      */
     bool isCA() const;
     /**
      * is the atom on the Backbone?
      * \return true if the atom on the Backbone
      */
     bool isBackbone() const;
     /**
      * is the atom part of a nucleotide?
      * \return true if the atom is part of a nucleotide
      */
     bool isNucleotide() const;

     /**
      * set the relative atomic mass
      * \param fMass relative atomic mass
      */
     void setMass(Real64 fMass);
     /**
      * get the relative atomic mass
      * \return relative atomic mass
      */
     Real64 getMass() const;

     /**
      * adjust the atomic mass based on a (simple) periodic table.
      * ToDo: Full periodic table
      */
     void adjustMass();

     /**
      * set the secondary structure information for this atom
      * \param cSecStruct secondary structure information
      */
     void setSecStruct(char cSecStruct);

     /**
      * get the secondary structure information for this atom
      * \return secondary structure information
      */
     char getSecStruct();

     /**
      * set the length in number of residues of the secondary structure element to which this atom belongs
      * \param iSecStructNumResidues number of residues
      */
     void setSecStructNumResidues(int iSecStructNumResidues);

     /**
      * get the length in number of residues of the secondary structure element to which this atom belongs
      * \return number of residues
      */
     int getSecStructNumResidues();

     /**
      * Get the van der waals radius
      * \return van der waals radius value (in Angstroem)
      */
     Real64 getVDWRadius() const;

     /**
      * Add a bond to the bond list of this atom
      * \param rBond pointer to the svt_point_cloud_bond object which should be added to the bond list
      */
     void addToBondList(svt_point_cloud_bond* pBond, unsigned int iIndex);
     /**
      * Remove a bond from the bond list of this atom
      * \param iIndex index of the svt_point_cloud_bond object which should be deleted from the bond list
      */
     void delFromBondList(unsigned int iIndex);

     /**
      * Get bond list
      */
     vector< unsigned int > getBondList();
     /**
      * Adjust bond list - a bond was erased from the global list and now all bond indexes higher than a certain number have to be decresed by one
      */
     void adjustBondList( unsigned int iIndex );
     
     /**
      * deletes all bonds in the bond list
      */
     void delBondList();
     
     
     /**
      * set Selected
      */
     void setSelected(bool bSelected);
     
     /**
      * get Selected
      */
     bool getSelected();
     
     /**
      * set the record name : is atom of class hetatm
      */
     void setHetAtm(bool bHetAtm);
     
     /**
      * get the record name : is atom of class hetatm
      */
     bool getHetAtm();
     
     
};

#endif
