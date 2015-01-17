/***************************************************************************
                          svt_point_cloud_atom
                          --------------------
    begin                : 02/10/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_basics.h>
#include <svt_types.h>
#include <svt_iostream.h>
#include <svt_point_cloud_atom.h>
#include <svt_point_cloud_bond.h>
// clib includes
#include <stdlib.h>

#ifdef TRUE_WIN32
#define strcasecmp stricmp
#endif

/**
 * Constructor
 */
svt_point_cloud_atom::svt_point_cloud_atom() :
    m_iModel( 0 ),
    m_iPDBIndex( 0 ),
    m_bWater(false),
    m_bHetAtm(false),
    m_aBondList( 0 ),
    m_aBondListI( 0 )
{
    sprintf( m_aName, "QP" );
    // remoteness indicator
    m_cRemoteness = 'D';
    // branch
    m_cBranch = 'B';
    // radius
    m_fRadius = 1.0;
    // alternate location identifier
    m_cAltLoc = ' ';
    // atom resname (i.e. "ALA")
    sprintf( m_aResname, "QPD" );
    // short resname name
    m_cShortResname = '-';
    // chain id
    m_cChainID = 'A';
    // residue sequence number
    m_iResSeq = 0;
    // ordinal residue sequence number
    m_iOrdResSeq = 0;
    // iCode
    m_cICode = ' ';
    // the occupancy
    m_fOccupancy = 1.0;
    // the temperature factor
    m_fTempFact = 1.0;
    // note
    m_pNote[0] = 0;
    // segment id
    m_pSegID[0] = 0;
    // element symbol
    sprintf( m_pElement, "QP" );
    // charge
    m_pCharge[0] = 0;

    // secondary structure information
    // H   Alpha helix
    // G   3-10 helix
    // I   PI-helix
    // E   Extended conformation
    // B   Isolated bridge
    // T   Turn
    // C   Coil (none of the above)
    // N   Information not available
    m_cSecStruct = 'C';

    // number of residues in the secondary structure this atom belongs to
    m_iSecStructNumResidues = 0;

    // relative atomic mass
    m_fMass = 1.0;
    
    m_bIsSelected = false;

    if (m_oAAList.size()==0) // the list was not created
    {
        svt_resname oAA;
        sprintf(oAA.m_a3LetCode, "ALA"); oAA.m_c1LetCode = 'A'; m_oAAList.push_back( oAA );
        sprintf(oAA.m_a3LetCode, "ARG"); oAA.m_c1LetCode = 'R'; m_oAAList.push_back( oAA );
        sprintf(oAA.m_a3LetCode, "ASN"); oAA.m_c1LetCode = 'N'; m_oAAList.push_back( oAA );
        sprintf(oAA.m_a3LetCode, "ASP"); oAA.m_c1LetCode = 'D'; m_oAAList.push_back( oAA );
        sprintf(oAA.m_a3LetCode, "CYS"); oAA.m_c1LetCode = 'C'; m_oAAList.push_back( oAA );
        sprintf(oAA.m_a3LetCode, "GLU"); oAA.m_c1LetCode = 'E'; m_oAAList.push_back( oAA );
        sprintf(oAA.m_a3LetCode, "GLN"); oAA.m_c1LetCode = 'Q'; m_oAAList.push_back( oAA );
        sprintf(oAA.m_a3LetCode, "GLY"); oAA.m_c1LetCode = 'G'; m_oAAList.push_back( oAA );
        sprintf(oAA.m_a3LetCode, "HIS"); oAA.m_c1LetCode = 'H'; m_oAAList.push_back( oAA );
        sprintf(oAA.m_a3LetCode, "ILE"); oAA.m_c1LetCode = 'I'; m_oAAList.push_back( oAA );
        sprintf(oAA.m_a3LetCode, "LEU"); oAA.m_c1LetCode = 'L'; m_oAAList.push_back( oAA );
        sprintf(oAA.m_a3LetCode, "LYS"); oAA.m_c1LetCode = 'K'; m_oAAList.push_back( oAA );
        sprintf(oAA.m_a3LetCode, "MET"); oAA.m_c1LetCode = 'M'; m_oAAList.push_back( oAA );
        sprintf(oAA.m_a3LetCode, "PHE"); oAA.m_c1LetCode = 'F'; m_oAAList.push_back( oAA );
        sprintf(oAA.m_a3LetCode, "PRO"); oAA.m_c1LetCode = 'P'; m_oAAList.push_back( oAA );
        sprintf(oAA.m_a3LetCode, "SER"); oAA.m_c1LetCode = 'S'; m_oAAList.push_back( oAA );
        sprintf(oAA.m_a3LetCode, "THR"); oAA.m_c1LetCode = 'T'; m_oAAList.push_back( oAA );
        sprintf(oAA.m_a3LetCode, "TRP"); oAA.m_c1LetCode = 'W'; m_oAAList.push_back( oAA );
        sprintf(oAA.m_a3LetCode, "TYR"); oAA.m_c1LetCode = 'Y'; m_oAAList.push_back( oAA );
        sprintf(oAA.m_a3LetCode, "VAL"); oAA.m_c1LetCode = 'V'; m_oAAList.push_back( oAA );
    } 

};

/**
 * Destructor
 */
svt_point_cloud_atom::~svt_point_cloud_atom()
{
};

/**
 * Set the model this atom belongs to
 * \param iModel number of model
 */
void svt_point_cloud_atom::setModel( unsigned int iModel )
{
    m_iModel = iModel;
};

/**
 * Get the model this atom belongs to
 * \return number of model
 */
unsigned int svt_point_cloud_atom::getModel( ) const
{
    return m_iModel;
};

/**
 * Set the atom type. Automatically strips spaces in front and after the atom name.
 * \param pType pointer to the char array with the name information
 */
void svt_point_cloud_atom::setName(const char* pName)
{
    strcpy(m_aName, pName);

    char cElement = m_aName[1];

    // strip spaces in front
    while(m_aName[0] == ' ')
    {
	m_aName[0] = m_aName[1];
	m_aName[1] = m_aName[2];
	m_aName[2] = m_aName[3];
	m_aName[3] = m_aName[4];
    }
    // strip spaces at the end
    for(int i=4; i >= 0; i--)
	if (m_aName[i] == 32)
	    m_aName[i] = 0;

    // simple vdw radii. If this is not exact enough uncomment the "correct" vdw radii below. But you will get much more bonds due to
    // the bigger radii.
    m_fRadius = 1.5f;
    switch(cElement)
    {
    case 'H' : m_fRadius = 1.00f; break;
    case 'C' : m_fRadius = 1.50f; break;
    case 'N' : m_fRadius = 1.40f; break;
    case 'O' : m_fRadius = 1.30f; break;
    case 'F' : m_fRadius = 1.20f; break;
    case 'S' : m_fRadius = 1.90f; break;
    }

    /*
     // standard van der Waals radii are assigned to the common elements. They are taken from Bondi, J.Phys.Chem., 68, 441, 1964. Other elements are assigned van der Waals radii of 2.0A.
     float rad = 2.0f;
     RAD("Ag", 1.72f)
     RAD("Ar", 1.88f)
     RAD("As", 1.85f)
     RAD("Au", 1.66f)
     RAD("Br", 1.85f)
     RAD("C",  1.70f)
     RAD("Cd", 1.58f)
     RAD("Cl", 1.75f)
     RAD("Cu", 1.40f)
     RAD("F",  1.47f)
     RAD("Ga", 1.87f)
     RAD("H",  1.20f)
     RAD("He", 1.40f)
     RAD("Hg", 1.55f)
     RAD("I",  1.98f)
     RAD("In", 1.93f)
     RAD("K",  2.75f)
     RAD("Kr", 2.02f)
     RAD("Li", 1.82f)
     RAD("Mg", 1.73f)
     RAD("N",  1.55f)
     RAD("Na", 2.27f)
     RAD("Ne", 1.54f)
     RAD("Ni", 1.63f)
     RAD("O",  1.52f)
     RAD("P",  1.80f)
     RAD("Pb", 2.02f)
     RAD("Pd", 1.63f)
     RAD("Pt", 1.72f)
     RAD("S",  1.80f)
     RAD("Se", 1.90f)
     RAD("Si", 2.10f)
     RAD("Sn", 2.17f)
     RAD("Te", 2.06f)
     RAD("Tl", 1.96f)
     RAD("U",  1.86f)
     RAD("Xe", 2.16f)
     RAD("Zn", 1.39f)
     */

    m_aName[4] = 0;
};

/**
 * get the atom name
 * \return pointer to the char array with the name information
 */
const char* svt_point_cloud_atom::getName() const
{
    return m_aName;
};

/**
 * Set the PDB-file index of the atom
 * \param iPDBIndex index of the atom
 */
void svt_point_cloud_atom::setPDBIndex( unsigned int iPDBIndex )
{
    m_iPDBIndex = iPDBIndex;
};
/**
 * Get the PDB-file index of the atom
 * \return index of the atom
 */
unsigned int svt_point_cloud_atom::getPDBIndex( ) const
{
    return m_iPDBIndex;
};

/**
 * get the atom remoteness indicator
 * \return char with the remoteness information (greek letters, A=alpha, B=beta,...)
 */
char svt_point_cloud_atom::getRemoteness() const
{
    return m_cRemoteness;
};

/**
 * set the atom remoteness indicator
 * \param cRemoteness char with the remoteness information (greek letters, A=alpha, B=beta,...)
 */
void svt_point_cloud_atom::setRemoteness( char cRemoteness )
{
    m_cRemoteness = cRemoteness;
};

/**
 * get the atom branch information
 * \return char with the branch information
 */
char svt_point_cloud_atom::getBranch() const
{
    return m_cBranch;
};
/**
 * set the atom branch information
 * \param cBranch char with the branch information
 */
void svt_point_cloud_atom::setBranch( char cBranch )
{
    m_cBranch = cBranch;
};

/**
 * set the atom alternate location indicator
 * \param cAltLoc character with the alternate location indicator
 */
void svt_point_cloud_atom::setAltLoc(char cAltLoc)
{
    m_cAltLoc = cAltLoc;
};

/**
 * get the atom alternate location indicator
 * \return character with the alternate location indicator
 */
char svt_point_cloud_atom::getAltLoc() const
{
    return m_cAltLoc;
};

/**
 * set the atom resname
 * \param pResname pointer to the char array with the residue name information
 */
void svt_point_cloud_atom::setResname(const char* pResname)
{
    strcpy(m_aResname, pResname);
    m_aResname[3] = 0;

    if ( strcasecmp(m_aResname,"tip3")==0 || strcasecmp(m_aResname,"hoh")==0 || strcasecmp(m_aResname,"h2o")==0 )
	m_bWater = true;
    else
        m_bWater = false;

    unsigned int iSize = m_oAAList.size();
    for (unsigned int iIndex = 0; iIndex<iSize; iIndex++)
    {
        if ( strcasecmp(m_aResname,m_oAAList[iIndex].m_a3LetCode) == 0 )
        {
            m_cShortResname = m_oAAList[iIndex].m_c1LetCode;
            return;
        } 
    }
    m_cShortResname = '-'; // is empty if nothing found 

};

/**
 * get the atom resname
 * \return pointer to the char array with the residue name information
 */
const char* svt_point_cloud_atom::getResname() const
{
    return m_aResname;
};
 
/**
 * set the short atom resname
 * \param pResname a char with the short "1letter" residue name information
 */
void svt_point_cloud_atom::setShortResname(const char cResname)
{
    m_cShortResname = cResname;
};

/**
 * get the short atom resname
 * \return a char with the short "1letter" residue name information
 */
char svt_point_cloud_atom::getShortResname() const
{
    return m_cShortResname;
};

/**
 * set the chain id
 * \param cChainID character with the chain id
 */
void svt_point_cloud_atom::setChainID(char cChainID)
{
    m_cChainID = cChainID;
};

/**
 * get the chain id
 * \return character with the chain id
 */
char svt_point_cloud_atom::getChainID() const
{
    return m_cChainID;
};

/**
 * set the ordinal chain id
 * \param iOrdChainID character with the chain id
 */
void svt_point_cloud_atom::setOrdChainID(int iOrdChainID)
{
    m_iOrdChainID = iOrdChainID;
};

/**
 * get the ordinal chain id
 * \return int with the chain id
 */
int svt_point_cloud_atom::getOrdChainID() const
{
    return m_iOrdChainID;
};

/**
 * set the residue sequence number
 * \param iResSeq residue sequence number
 */
void svt_point_cloud_atom::setResidueSeq(int iResSeq)
{
    m_iResSeq = iResSeq;
};

/**
 * get the residue sequence number
 * \return residue sequence number
 */
int svt_point_cloud_atom::getResidueSeq() const
{
    return m_iResSeq;
};

/**
 * set the ordinal residue sequence number
 * \param iOrdResSeq ordinal residue sequence number
 */
void svt_point_cloud_atom::setOrdResidueSeq(int iOrdResSeq)
{
    m_iOrdResSeq = iOrdResSeq;
};
/**
 * get the ordinal residue sequence number
 * \return ordinal residue sequence number
 */
int svt_point_cloud_atom::getOrdResidueSeq() const
{
    return m_iOrdResSeq;
};

/**
 * set the iCode (code for insertion of residues)
 * \param cICode char with the iCode
 */
void svt_point_cloud_atom::setICode(char cICode)
{
    m_cICode = cICode;
};

/**
 * get the iCode (code for insertion of residues)
 * \return char with the iCode
 */
char svt_point_cloud_atom::getICode() const
{
    return m_cICode;
};

/**
 * set the occupancy
 * \param fOccupancy the occupancy
 */
void svt_point_cloud_atom::setOccupancy(float fOccupancy)
{
    m_fOccupancy = fOccupancy;
};

/**
 * get the occupancy
 * \return the occupancy
 */
float svt_point_cloud_atom::getOccupancy() const
{
    return m_fOccupancy;
};

/**
 * set the temperature factor
 * \param fTempFact the temperature factor
 */
void svt_point_cloud_atom::setTempFact(float fTempFact)
{
    m_fTempFact = fTempFact;
};

/**
 * get the temperature factor
 * \return the temperature factor
 */
float svt_point_cloud_atom::getTempFact() const
{
    return m_fTempFact;
};

/**
 * set the note
 * \param pNote pointer to char array with at least 3 chars for the note
 */
void svt_point_cloud_atom::setNote(const char *pNote)
{
    m_pNote[0] = pNote[0];
    m_pNote[1] = pNote[1];
    m_pNote[2] = pNote[2];
    m_pNote[3] = 0;

    if (m_pNote[2] == 0)
        m_pNote[2] = ' ';
    if (m_pNote[1] == 0)
        m_pNote[1] = ' ';
    if (m_pNote[0] == 0)
        m_pNote[0] = ' ';
};

/**
 * get the note
 * \return pointer to char array with the note
 */
const char* svt_point_cloud_atom::getNote() const
{
    return m_pNote;
};

/**
 * set the segment id
 * \param pSegID pointer to char array with at least 4 chars for the segment id
 */
void svt_point_cloud_atom::setSegmentID(const char *pSegID)
{
    m_pSegID[0] = pSegID[0];
    m_pSegID[1] = pSegID[1];
    m_pSegID[2] = pSegID[2];
    m_pSegID[3] = pSegID[3];
    m_pSegID[4] = 0;

    if (m_pSegID[3] == 0)
        m_pSegID[3] = ' ';
    if (m_pSegID[2] == 0)
        m_pSegID[2] = ' ';
    if (m_pSegID[1] == 0)
        m_pSegID[1] = ' ';
    if (m_pSegID[0] == 0)
        m_pSegID[0] = ' ';
};

/**
 * get the segment id
 * \return pointer to char array with the segment id
 */
const char* svt_point_cloud_atom::getSegmentID() const
{
    return m_pSegID;
};

/**
 * set the element symbol
 * \param pElement pointer to char array with at least 2 chars for the element symbol
 */
void svt_point_cloud_atom::setElement(const char *pElement)
{
    m_pElement[0] = pElement[0];
    m_pElement[1] = pElement[1];
    m_pElement[2] = 0;

    //Mirabela added = 13 Carrige return
    if (m_pElement[1] == 0 || m_pElement[1] == 10 || m_pElement[1] == 13)
        m_pElement[1] = ' ';
    if (m_pElement[0] == 0 || m_pElement[0] == 10 || m_pElement[0] == 13)
        m_pElement[0] = ' ';
     
};

/**
 * get the element symbol
 * \return pointer to char array with the element symbol
 */
const char* svt_point_cloud_atom::getElement() const
{
    return m_pElement;
};

/**
 * set the charge
 * \param pCharge pointer to char array with at least 2 chars for the charge
 */
void svt_point_cloud_atom::setCharge(const char *pCharge)
{
    m_pCharge[0] = pCharge[0];
    m_pCharge[1] = pCharge[1];
    m_pCharge[2] = 0;

    if (m_pCharge[1] == 0 || m_pCharge[1] == 10 || m_pCharge[1] == 13 )
        m_pCharge[1] = ' ';
    if (m_pCharge[0] == 0 || m_pCharge[0] == 10 || m_pCharge[0] == 13 )
        m_pCharge[0] = ' ';
};

/**
 * get the charge
 * \return pointer to char array with the charge
 */
const char* svt_point_cloud_atom::getCharge() const
{
    return m_pCharge;
};

#define ISNUMBER(X) (X > 47 && X < 58)

/**
 * is the atom a hydrogen atom?
 * \return true if the the atom is an hydrogen atom
 */
bool svt_point_cloud_atom::isHydrogen() const
{
    if (m_aName[0] == 'H' || (ISNUMBER(m_aName[0]) && m_cRemoteness == 'H') || (ISNUMBER(m_aName[0]) && ISNUMBER(m_cRemoteness) && m_cBranch == 'H'))
	return true;
    else
        return false;
}
/**
 * is the atom a QPDB codebook vector?
 * \return true if the the atom is a CV
 */
bool svt_point_cloud_atom::isQPDB() const
{
    if (m_aName[0] == 'Q' && m_cRemoteness == 'P' && m_cBranch == 'D')
	return true;
    else
        return false;
}

/**
 * is the atom part of a water molecule?
 * \return true if the atom os part of a water molecule
 */
bool svt_point_cloud_atom::isWater() const
{
    return m_bWater;
}

/**
 * is the atom a carbon alpha?
 * \return true if the atom a CA
 */
bool svt_point_cloud_atom::isCA() const
{
    if (!m_bHetAtm && (m_aName[0] == 'C' && m_cRemoteness == 'A'))
	return true;
    else
        return false;
}

/**
 * is the atom on the Backbone? (def Backbone: CAlpha (CA), Carbon(C) witout Remoteness with his Oxygen, Nitrogen(N) witout Remoteness and the Oxygen (O)  of the previous 
 * \return true if the atom on the Backbone
 */
bool svt_point_cloud_atom::isBackbone() const
{
   
    if (!m_bHetAtm && (    (m_aName[0] == 'C' && m_cRemoteness == 'A')
	                || (m_aName[0] == 'C' && m_cRemoteness == ' ')
			|| (m_aName[0] == 'N' && m_cRemoteness == ' ')
			|| (m_aName[0] == 'O' && m_cRemoteness == ' ')
                        || (m_aName[0] == 'O' && m_cRemoteness == 'X') // COOH of the C terminus 
                        || (m_aName[0] == 'P' && m_cRemoteness == ' ') //nucleotide
                        || (m_aName[0] == 'O' && m_cRemoteness == '5' && m_cBranch == '\'')
                        || (m_aName[0] == 'C' && m_cRemoteness == '5' && m_cBranch == '\'')
                        || (m_aName[0] == 'C' && m_cRemoteness == '4' && m_cBranch == '\'')
                        || (m_aName[0] == 'O' && m_cRemoteness == '4' && m_cBranch == '\'')
                        || (m_aName[0] == 'C' && m_cRemoteness == '3' && m_cBranch == '\'')
                        || (m_aName[0] == 'O' && m_cRemoteness == '3' && m_cBranch == '\'')
                        || (m_aName[0] == 'C' && m_cRemoteness == '2' && m_cBranch == '\'')
                        || (m_aName[0] == 'O' && m_cRemoteness == '2' && m_cBranch == '\'')
                        || (m_aName[0] == 'C' && m_cRemoteness == '1' && m_cBranch == '\'')
                      ) ) 
	return true;
    else
	return false;
}

/**
 * is the atom part of a nucleotide?
 * \return true if the atom is part of a nucleotide
 */
bool svt_point_cloud_atom::isNucleotide() const
{
    return (m_aResname[0] == ' ' && m_aResname[1] == ' ');
}

/**
 * set the relative atomic mass
 * \param fMass relative atomic mass
 */
void svt_point_cloud_atom::setMass(Real64 fMass)
{
    m_fMass = fMass;
}
/**
 * get the relative atomic mass
 * \return relative atomic mass
 */
Real64 svt_point_cloud_atom::getMass() const
{
    return m_fMass;
}

/**
 * adjust the atomic mass based on a (simple) periodic table.
 * ToDo: Full periodic table
 */
void svt_point_cloud_atom::adjustMass()
{
    static const char *atom_name[] =
    {
	"H", "HE", "LI", "BE",  "B",  "C",  "N",  "O",  "F", "NE",
	"NA", "MG", "AL", "SI",  "P",  "S", "CL", "AR",  "K", "CA",
	"SC", "TI",  "V", "CR", "MN", "FE", "CO", "NI", "CU", "ZN"
    };
    static float atom_mass[30] =
    {
	1.008 ,  4.003,  6.940,  9.012, 10.810, 12.011, 14.007, 16.000,  18.998, 20.170,
	22.989, 24.305, 26.982, 28.086, 30.974, 32.060, 35.453, 39.948,  39.098, 40.078,
	44.956, 47.867, 50.942, 51.996, 54.938, 55.847, 58.933, 58.710,  63.546, 65.380
    };

    for(int i=0; i<30; i++)
    {
	if (strcmp(getName(), atom_name[i])==0)
	{
	    setMass( atom_mass[i] );

            //svtout << getName() << " - " << atom_mass[i] << endl;

	    return;
	}
    }

    //svtout << getName() << " - NOT FOUND, NO MASS ASSIGNED!" << endl;

};

/**
 * set the secondary structure information for this atom
 * \param cSecStruct secondary structure information
 *
 */
void svt_point_cloud_atom::setSecStruct(char cSecStruct)
{
    m_cSecStruct = cSecStruct;
}

/**
 * get the secondary structure information for this atom
 * \return secondary structure information
 *
 */
char svt_point_cloud_atom::getSecStruct()
{
    return m_cSecStruct;
}

/**
 * set the number of residues in the secondary structure this atom belongs to
 * \param iSecStructNumResidues number of residues
 *
 */
void svt_point_cloud_atom::setSecStructNumResidues(int iSecStructNumResidues)
{
    m_iSecStructNumResidues = iSecStructNumResidues;
}

/**
 * get the number of residues in the secondary structure this atom belongs to
 * \return number of residues
 *
 */
int svt_point_cloud_atom::getSecStructNumResidues()
{
    return m_iSecStructNumResidues;
}

/**
 * Get the van der waals radius
 * \return van der waals radius value (in Angstroem)
 */
Real64 svt_point_cloud_atom::getVDWRadius() const
{
    return m_fRadius;
}

/**
 * Add a bond to the bond list of this atom
 * \param rBond pointer to the svt_point_cloud_bond object which should be added to the bond list
 */
void svt_point_cloud_atom::addToBondList(svt_point_cloud_bond* pBond, unsigned int iIndex)
{
    m_aBondList.push_back( pBond );
    m_aBondListI.push_back( iIndex );
};
/**
 * Remove a bond from the bond list of this atom
 * \param rBond pointer to the svt_point_cloud_bond object which should be added to the bond list
 */
void svt_point_cloud_atom::delFromBondList(unsigned int iIndex)
{
    vector< unsigned int >::iterator pI = find(m_aBondListI.begin(), m_aBondListI.end(), iIndex);
    if (pI == m_aBondListI.end())
    {
        return;
    } else
    {
        m_aBondListI.erase( pI );
        int iIndex = m_aBondListI.end() - pI;
        m_aBondList.erase( m_aBondList.begin() + iIndex );
    }
    return;
};

/**
 * Get bond list
 */
vector< unsigned int > svt_point_cloud_atom::getBondList()
{
    return m_aBondListI;
}

/**
 * Adjust bond list - a bond was erased from the global list and now all bond indexes higher than a certain number have to be decresed by one
 */
void svt_point_cloud_atom::adjustBondList( unsigned int iIndex )
{
    for(unsigned int i=0;i<m_aBondListI.size();i++)
        if (m_aBondListI[i] > iIndex)
            m_aBondListI[i] = m_aBondListI[i] -1;
}

/**
 * deletes all bonds in the bond list
 */
void svt_point_cloud_atom::delBondList()
{
    m_aBondList.clear();
    m_aBondListI.clear();
};

/**
 * set Selected
 */
void svt_point_cloud_atom::setSelected(bool bSelected)
{
    m_bIsSelected = bSelected;
};

/**
* get Selected
*/
bool svt_point_cloud_atom::getSelected()
{
    return m_bIsSelected;
};

/**
 * set the record name : is atom of class hetatm
 */
void svt_point_cloud_atom::setHetAtm(bool bHetAtm)
{
    m_bHetAtm = bHetAtm;
};

/**
 * get the record name : is atom of class hetatm
 */
bool svt_point_cloud_atom::getHetAtm()
{
    return m_bHetAtm;
};
