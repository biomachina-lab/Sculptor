/***************************************************************************
                          svt_point_cloud_pdb
                          -------------------
    begin                : 02/10/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_POINT_CLOUD_PDB_H
#define __SVT_POINT_CLOUD_PDB_H

#include <stdlib.h>
#include <svt_stlVector.h>
#include <svt_stlMap.h>
#include <svt_column_reader.h>
#include <svt_file_utils.h>
#include <svt_point_cloud.h>
#include <svt_point_cloud_bond.h>
#include <svt_point_cloud_link.h>
#include <svt_point_cloud_atom.h>
#include <svt_matrix4.h>
#include <limits.h>
#include <svt_stride.h>
//added libraries for fetchPDB
#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif
//#include <svt_messages.h>
#include <svt_exception.h>

#ifdef WIN32
#include <string.h>
#include <windows.h>
#include <winsock.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#define strcasecmp _stricmp
#endif

#ifdef MACOSX
#include <CFURL.h>
#endif

#define MAXLINE 2048

///////////////////////////////////////////////////////////////////////////////
// svt_point_cloud_pdb
///////////////////////////////////////////////////////////////////////////////

enum SymmetryType
{
    SYMMETRY_C,
    SYMMETRY_D,
    SYMMETRY_H
};

typedef enum {
    SSE_NOTAVAILABLE,
    SSE_PDB, // from the pdb
    SSE_STRIDE, //stride
    SSE_OTHER
} svt_secStructSource;

enum Select {
    ALL,
    BACKBONE,
    TRACE
};

/**
 * class containing secondary structure information as read from Pdb 
 */
class svt_sse_pdb
{
public:
    char m_aType[7];
    int m_iNum;
    
    char m_aID[4];

    char m_aInitialResname[5];
    char m_cInitialResChainID;
    int m_iInitialResSeq;
    char m_cInitialICode;

    char m_aTerminalResname[5];
    char m_cTerminalResChainID;
    int m_iTerminalResSeq;
    char m_cTerminalICode;

    //helix information
    int m_iClass;
    char m_aComment[31];
    int m_iLen;

    // sheet information 
    int m_iNumStrands;
    int m_iSense;
    
    char m_aCurAtom[5];
    char m_aCurResname[5];
    char m_cCurResChainID;
    char m_aCurResSeq[5];
    char m_cCurICode;

    char m_aPrevAtom[5];
    char m_aPrevResname[5];
    char m_cPrevResChainID;
    char m_aPrevResSeq[5];
    char m_cPrevICode;

};

/**
 * This is a special form of a point cloud that encapsulates a molecular pdb file
 * \author Stefan Birmanns
 */
template<class T> class svt_point_cloud_pdb : public svt_point_cloud<T>
{
protected:

    vector< svt_point_cloud_atom > m_aAtoms;
    vector< svt_point_cloud_bond > m_aBonds;
    vector< svt_point_cloud_link > m_aLinks;


    vector< const char* >  m_aAtomNames;
    vector< const char* >  m_aAtomResnames;
    vector< char >         m_aAtomChainIDs;
    vector< char >         m_aAtomSecStructureIDs;
    vector< const char* >  m_aAtomSegmentIDs;
    vector< int >          m_aAtomResSeqs;
    vector< unsigned int > m_aAtomModels;

    // should the atoms belonging to a water molecule be ignored in clustering this structure?
    bool m_bIgnoreWater;

    // shall we load HETATM records?
    bool m_bLoadHetAtm;

    // enumeration functions;
    unsigned int m_iAtomEnum;
    unsigned int m_iBondEnum;

    //the number of carbon alpha in the molecule, the number of backbone atoms in the molecule
    unsigned int m_iNumCAAtoms;
    unsigned int m_iNumBBAtoms;

    // was the secondary structure information for this molecule already specified?
    bool m_bSecStructAvailable;

    // were does the secondary structure come from: 0 not available (not yet computed/read); 1, from pdb, 2, from stride 
    svt_secStructSource m_eSecStructSource;

    // distance cutoff value that determines the maximum distance for which consecutive residues are connected
    float m_fTraceCutoff;

    // filename of pdb structure
    string m_pFilename;

    //graph distance in the psf file
    //at position i,j is placed the graph distance between atom i and atom j
    svt_matrix < Real64 > m_oGraphDists;

    //was the psf file been read? without the bonds information the m_oGraphDist cannot be computed
    bool m_bPSFRead;

    // Remarks relative to the pdb file (read form the original file, also added durring execution)
    vector< string > m_aRemarks;

    // store transformation that allow the creating of the 
    vector< svt_matrix4 <Real64> > m_aSymmMats;

    //list of modified residues
    vector< vector<string> > m_aModResidues;

    // list of all the secondary Structure elements
    vector <svt_sse_pdb> m_aSsePdb;

    int CoordPrecisionPDB (double fNum) {
      if (fNum < 9999.9995 && fNum >= -999.9995) return 3;
      else if (fNum < 99999.995 && fNum >= -9999.995) return 2;
      else if (fNum < 999999.95 && fNum >= -99999.95) return 1;
      else if (fNum < 99999999.5 && fNum >= -9999999.5) return 0;
      else {
	svt_exception::ui()->error("Error: PDB coordinate out of range, PDB outout aborted");
        exit(0);
      }
    }

public:

    /**
     * Constructor
     */
    svt_point_cloud_pdb();
    /**
     * Copy Constructor for svt_point_cloud (not pdb) object
     */
    svt_point_cloud_pdb( svt_point_cloud<T>& rOther);
    /**
     * Destructor
     */
    ~svt_point_cloud_pdb();



    /**
     * \name File format functions
     */
    //@{
    
   
    /**
     * Fetch PDB data from www.rcsb.org
     * \param pPdbID pointer to array of 4 char PDBID
     */
    bool fetchPDB(const char* pPdbID);

    /**
     * Load a pdb file.
     * \param pointer to array of char with the filename
     */
    void loadPDB(const char* pFilename);

    /**
     * Write pdb file.
     * \param pFilename pointer to array of char with the filename
     * \param bAppend if true, the pdb structure as append at the end of an existing structure file
     * \param bFirst if true only the first frame is written to the pdb file
     */
    void writePDB(const char* pFilename, bool bAppend = false, bool bAddRemarks = true, bool bFirst = false);

    /**
     * Opens and reads a psf file.
     * \param pFilename pointer to array of char with the filename
     */
    bool loadPSF(const char* pFilename);

    /**
     * Write psf file
     * \param pFilename pointer to array of char with the filename
     */
    void writePSF(const char* pFilename);

    /**
     * loads a dcd file
     * \param oFilename filename
     */
    bool loadDCD(const char* pFilename);

    //@}

    /**
     * \name File Format Helper Functions
     */
    //@{

    /**
     * setupHTTPGet is a method that constructs the HTTP/1.1 GET command from it's parameters.
     * \param cDirectory the exact directory from the server
     * \param cFilename the file to retrieve 
     * \param cHost the server to connect to
     * \param cGetstr the string that will contain the entire GET command  
     */
    void setupHTTPGet(const char* cDirectory, const char* cFilename, const char* cHost, char* cGetstr);

    /**
     * hexValue is a method that converts a single hex digit into decimal.  
     * \param cHexchar the character to convert  
     * \return decimal base 10 of hex character
     */
    int hexValue(char cHexchar);
    
    /**
     * hextoint is a method that converts an entire hex string into decimal.  
     * \param pHexstr pointer to the string to convert 
     * \return decimal base 10 of entire hex string
     */
    int hextoint(char* pHexstr);

    /**
     * peekPDB is a method that retrieves the HTTP header of the chosen PDBID. It checks the first 
     * line of the HTTP header to determine whether the PDBID is valid.  If it is not, it closes the 
     * connection, and returns false.  Otherwise, the method retrieves the entire header and returns true.
     * \param iSocket the socket used to communicate/recv with the server  
     */
    bool peekPDB(int iSocket);
    
    /**
     * parseLine is a method that reads one line from pdb data, parses the information, and 
     * integrates it into the pdb object.
     * \param pLine pointer to one line of data
     * \param iModel integer of which molecular model
     * \param iCount integer of number of atoms
     * \param iPrevSeq integer of previous residue sequence
     * \param iOrdSeq integer of ordinal sequence
     * \param cPrevChain character of previous chain ID
     */
    void parseLine(const char* pLine, unsigned int& iModel, unsigned int& iCount, int& iPrevSeq, int& iOrdSeq, char& cPrevChain); 	
   
     //@}

    /**
     * \name Data management
     * With this functions you can add or delete atoms, etc.
     */
    //@{

    /**
     * add atom
     */
    void addAtom( svt_point_cloud_atom& rAtom, svt_vector4<Real64>& rVec);

    /**
     * get atom with a special index (attention this is not the index in the pdb file, because this index is not unique in a lot of pdb files. svt_pdb simply renumbers all the atoms to get a unique index for each atom).
     * \param iIndex index of the atom you want to get
     * \return reference to the svt_atom object you want to get.
     */
    svt_point_cloud_atom& atom( unsigned int i );
    /**
     * get atom with a special index (attention this is not the index in the pdb file, because this index is not unique in a lot of pdb files. svt_pdb simply renumbers all the atoms to get a unique index for each atom).
     * \param iIndex index of the atom you want to get
     * \return pointer to the svt_atom object you want to get.
     */
    svt_point_cloud_atom* getAtom( unsigned int i );

    /**
     * Delete all atoms.
     * Attention: Please keep in mind that the internal data structures are using stl vectors. In most stl implementations the vectors cannot shrink - they will increase
     * the memory allocation every time one stores an element, but will not shrink if one deletes elements. In addition, to keep the operating system calls for the
     * memory management at a minimum, they will allocate more memory than what they would actually need to store the current elements. Most implementations will double the amount
     * of memory that they currently hold, whenever they exhaust their current capacity.
     * For the function here, that means that if you store 2mio atoms in the point_cloud_pdb and then delete all atoms, the memory consumption will still be for 2mio atoms. If you
     * from now on store only 30 atoms, it does not matter, the memory consumption will not go down. If you would like to store fluctuating numbers of atoms, please dynamically
     * create new objects on the heap with new and delete them again.
     */
    void deleteAllAtoms();

    /**
     * add a bond
     * \param iAtomA index of atom A
     * \param iAtomB index of atom B
     */
    void addBond( unsigned int iAtomA, unsigned int iAtomB );
    /**
     * is there a bond between two atoms?
     * \param iAtomA index of atom A
     * \param iAtomB index of atom B
     */
    bool isBond( unsigned int iAtomA, unsigned int iAtomB );
    /**
     * remove bond between two atoms?
     * \param iAtomA index of atom A
     * \param iAtomB index of atom B
     */
    void delBond( unsigned int iAtomA, unsigned int iAtomB );


    /**
     * find the bond object connecting two atoms
     * \param pA pointer to atom a
     * \param pB pointer to atom b
     * \return pointer to svt_point_cloud_bond object. If there is no such bond between these atoms, the return value will be NULL
     */
    svt_point_cloud_bond* findBond(svt_point_cloud_atom* pA, svt_point_cloud_atom* pB);

    /**
     * get number of bonds
     * \return number of bonds
     */
    unsigned int getBondsNum();
    /**
     * get bonds
     * \return pointer to array of bonds
     */
    vector< svt_point_cloud_bond >& getBonds();


    /**
     * delete all bonds
     */
    void deleteAllBonds();

    /**
     * add a link
     * \param iAtomA index of atom A
     * \param iAtomB index of atom B
     */
    void addLink( int iAtomA, int iAtomB );


    /**
     * rotate
     * \param iIndexLink the index of the link in the list
     * \param iIndexAxis = (0,1,2)=(x,y,z)
     * \param fAngle the angle in rad
     */
    void rotateLink( int iIndeLink, int iIndexAxis, Real64 fAngle );

    /**
     * reset links rotations
     */
    void resetLinksRotation();

    /**
     * adjust the Length with
     *\param iIndexLink the index of the link in the list
     * \param fLen the length to be added (flen can be positive but also negative)
     */
    void setLinkLength( int iIndeLink, Real64 fLen );


    /**
     * get number of links
     * \return number of links
     */
    unsigned int getLinksNum();

    /**
     * get the Psi angles of the Link
     * \param iIndexLink the index of the link
     * \return the angle
     */
    Real64 getLinkPsi( int iIndexLink);

    /**
     * get the Phi angles of the Link
     * \param iIndexLink the index of the link
     * \return the angle
     */
    Real64 getLinkPhi( int iIndexLink);

    /**
     * get the Theta angles of the Link
     * \param iIndexLink the index of the link
     * \return the angle
     */
    Real64 getLinkTheta( int iIndexLink);

    /**
     * get the length of the Link
     * \param iIndexLink the index of the link
     * \return the angle
     */
    Real64 getLinkLen( int iIndexLink);

    /**
     * delete all links
     */
    void deleteAllLinks();

    /**
     * Append another pdb structure
     * \param rOther reference to other pdb point cloud
     * \param iModel the model number of the other, appended pdb structure (default: 0)
     * \return reference to point_cloud_pdb object
     */
    svt_point_cloud_pdb<T>& append( svt_point_cloud_pdb<T>& rOther, int iModel = -1 );

    /**
     * Add a remark
     * \param oRemark the remark
     */
    void addRemark(string oRemark);

    //@}

    /**
     * \name Enumeration functions
     * With this functions you can search the svt_pdb object for atoms or bonds with certain properties.
     */
    //@{

    /**
     * reset atom enumeration functions. This functions set the atom enumeration index back to the first atom.
     */
    void resetAtomEnum();
    /**
     * look for all atoms of a certain type. This function will return the next atom of this type, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetAtomEnum().
     * \param pType pointer to an char array with the name of the atom
     */
    int enumAtomType(const char* pType);
    /**
     * look for all atoms of a certain residue name. This function will return the next atom with this residue name, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetAtomEnum().
     * \param pResname pointer to an char array with the residue name
     */
    int enumAtomResname(const char* pResname);
    /**
     * look for all atoms with a certain chain id. This function will return the next atom of this type, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetAtomEnum().
     * \param pType pointer to an char array with the name of the atom
     */
    int enumAtomChainID(char cChainID);
    /**
     * look for all atoms with a certain segment id. This function will return the next atom of this type, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetAtomEnum().
     * \param pType pointer to an char array with the name of the atom
     */
    int enumAtomSegmentID(const char* pSegmentID);
    /**
     * look for all atoms with a certain residue sequence number. This function will return the next atom of this type, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetAtomEnum().
     * \param pType pointer to an char array with the name of the atom
     */
    int enumAtomResidueSeq(int iResidueSeq);
    /**
     * look for all atoms with a certain model number. This function will return the next atom of this type, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetAtomEnum().
     * \param int with the model number
     */
    int enumAtomModel(unsigned int iModel);
    /**
     * look for all atoms. This function will return the next atom, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetAtomEnum().
     */
    int enumAtomAll();
    /**
     * look for all atoms part of a nucleotide. This function will return the next atom, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetAtomEnum().
     */
    int enumAtomNucleotide();
    /**
     * look for all HET atoms. This function will return the next atom, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetAtomEnum().
     */
    int enumAtomHet();
    /**
     * look for all water atoms. This function will return the next atom, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetAtomEnum().
     */
    int enumAtomWater();
    /**
     * reset bond enumeration functions. This functions set the bond enumeration index back to the first bond.
     */
    void resetBondEnum();
    /**
     * look for all bonds. This function will return the next bond, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetBondEnum().
     */
    int enumBondAll();
    /**
     * look for all bonds which connect a certain atom with other atoms. This function will return the next bond connecting this atom, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetBondEnum().
     * \param pAtom pointer to an svt_point_cloud_atom object
     */
    int enumBondAtom(svt_point_cloud_atom* pAtom);

    //@}

    /**
     * \name Information
     * With these functions one can get basic statistics about the atoms in the pdb file.
     */
    //@{

    /**
     * get an array with all the different atom types (names) in the pdb file
     * \return pointer to an vector with strings of all different atom types (names) used in the pdb. Dont delete this pointer! svt_pdb will take care of it.
     */
    vector<const char*>& getAtomNames();
    /**
     * get an array with all the different residue names in the pdb file
     * \return pointer to an vector with strings of all different residue names used in the pdb. Dont delete this pointer! svt_pdb will take care of it.
     */
    vector<const char*>& getAtomResnames();
    /**
     * get an array with all the different chain id's in the pdb file
     * \return pointer to an vector with strings of all different residue names used in the pdb. Dont delete this pointer! svt_pdb will take care of it.
     */
    vector<char>& getAtomChainIDs();
    /**
     * get an array with all the different segment id's in the pdb file
     * \return pointer to an vector with strings of all different residue names used in the pdb. Dont delete this pointer! svt_pdb will take care of it.
     */
    vector<const char*>& getAtomSegmentIDs();
    /**
     * get an array with all the different residue sequence numbers in the pdb file
     * \return pointer to an vector with strings of all different residue names used in the pdb. Dont delete this pointer! svt_pdb will take care of it.
     */
    vector<int>& getAtomResidueSeqs();
    /**
     * get an array with all the different secondary structure ids in the pdb file
     * \return pointer to an vector with strings of all different secondary structure ids used in the pdb. Dont delete this pointer! svt_pdb will take care of it.
     */
    vector<char>& getAtomSecStructIDs();
    /**
     * get an array with all the different models
     * \return reference to vector with ints
     */
    vector<unsigned int>& getAtomModels();
    /**
     * get a copy of a certain model
     * \param iModel number of model
     */
    svt_point_cloud_pdb<T>* getModel( unsigned int iModel );

    /**
     * get the number of Carbon Alpha in the pdb
     * \return the number of CA in the pdb
     */
    unsigned int getNumCAAtoms();

    /**
     * get the number of Backbone atoms in the pdb
     * \return the number of backbone atoms in the pdb
     */
    unsigned int getNumBBAtoms();

    /**
     * get the minimum x coord of all atoms
     * \return minimum x coord
     */
    Real32 getMinXCoord();
    /**
     * get the minimum y coord of all atoms
     * \return minimum y coord
     */
    Real32 getMinYCoord();
    /**
     * get the minimum z coord of all atoms
     * \return minimum z coord
     */
    Real32 getMinZCoord();
    /**
     * get the maximum x coord of all atoms
     * \return maximum x coord
     */
    Real32 getMaxXCoord();
    /**
     * get the maximum y coord of all atoms
     * \return maximum y coord
     */
    Real32 getMaxYCoord();
    /**
     * get the maximum z coord of all atoms
     * \return maximum z coord
     */
    Real32 getMaxZCoord();

    /**
     * get the maximum bfactor of all atoms
     * \return maximum bfactor
     */
    Real32 getMaxTempFact();
    /**
     * get the minimum bfactor of all atoms
     * \return minimum bfactor
     */
    Real32 getMinTempFact();

    /**
     * indicates if the psf_file has been reads
     * \return a bool true(1) if the psf has been read; false(0)  in the other case
     */
    bool getPSFRead();

    /**
     * Get the distance in the graph between 2 atoms(codebook vectors)
     * \param iIndex1 an int representing the row
     * \param iIndex2 an int representing the column
     * \return a float - the distance (inside the graph) between the atom iIndex1, iIndex2
     */
    Real64 getGraphDist(unsigned int iIndex1, unsigned int iIndex2);

    /**
     * Get the pointer to the graph distance matrix
     * \return svt_matrix& object m_oGraphDists = distances (inside the graph) between any atoms
     */
     svt_matrix<Real64>& getGraphDists();

    /**
     * calculate the sphericity of the pdb
     */
    Real32 getSphericity();

    /**
     * calculate the bonds between the atoms (according to their distance the bonds are guessed)
     * \param bShowProgress indicates rather the progress bar is shown
     */
    void calcBonds(bool bShowProgress = true);
    /**
     * calculate all lists (atom types, residues, ...)
     */
    void calcAllLists();
    /**
     * calculate array with the different atom type
     */
    void calcAtomNames();
    /**
     * calculate array with the different atom resnames
     */
    void calcAtomResnames();
    /**
     * calculate array with the different atom chain ids
     */
    void calcAtomChainIDs();
    /**
     * calculate array with the different atom secondary structure ids
     */
    void calcAtomSecStructureIDs();
    /**
     * calculate array with the different atom segment ids
     */
    void calcAtomSegmentIDs();
    /**
     * calculate array with the different atom residue sequence numbers
     */
    void calcAtomResidueSeqs();
    /**
     * calculate array with the different atom model numbers (Attention: This function is NOT CALLED in calcAllLists as typically this array is automatically build during loadPDB. If a pdb is build by hand, call this function after assembly of the structure!).
     */
    void calcAtomModels();

    /**
    * set the coordinate to the first atom in the links list
    * \param oCoord the new coordinates
    */
    void setLinksOrigin(svt_vector4<Real64> oCoord);

    /**
     * update the cartesian coordinates using the internal coordinates/link information
     */
    void updateCoords();

    /**
     * get Internal coordiantes as vector. first translation, then lenght + angles for each link
     * \return vector with internal coordinates
     */
    vector<Real64>& getInternalCoords();

    //@}

    /**
     * \name Secondary structure information
     */
    //@{
    /**
     * compute the ordinal chainIDs. Those identify chains in the pdb based only on a C-alpha to C-alpha
     * distance criterion and are supposed to be calculated independently from the pdb chainIDs
     */
    void calcOrdinalChainIDs();
    /**
     * calculate the secondary structure information
     */
    void calcSecStruct();
    /**
     * runs stride on one pdb containing one model or chain
     */
    void runStride();
     /**
     * Compress the secondary structure information into the ssePdb object
     * \ATTENTION: erases the existant compressed information and recompresses it from the pdb atom entry 
     */
    void compressSecStruct();
    /**
     * compute and set the length of the secondary structure elements
     */
    void calcSecStructLengths();
    /**
     * is the secondary structure information already specified?
     * \param bSecStruct true if the atoms know their secondary structure membership
     */
    void setSecStructAvailable( bool bSecStruct );
    /**
     * is the secondary structure information already specified?
     * return true if the atoms know their secondary structure membership
     */
    bool getSecStructAvailable();
     /**
     * set the secondary structure source
     * \param 0 - not available, 1 - pdb, 2 - stride 
     */
    void setSecStructSource( int eSecStructSource );
    /**
     * set the secondary structure source
     * \return 0 - not available, 1 - pdb, 2 - stride 
     */
    int getSecStructSource();
    /**
     * get the compressed list of secondary structure- the one used to hold the information wrote in pdb in HELIX and SHEET entry
     */ 
    vector <svt_sse_pdb>& getSecStructCompressedList();
     /**
     * set the compressed list of secondary structure- the one used to hold the information wrote in pdb in HELIX and SHEET entry
     */ 
    void setSecStructCompressedList( vector <svt_sse_pdb> aSse );
    /**
     * get the sse from the list for this atom 
     * \param oAtom the atom 
     */ 
    char getSecStructFromCompressedList( svt_point_cloud_atom oAtom );
    /**
     * set the distance cutoff value that determines the maximum distance for which consecutive residues are connected
     * \param fTubeCutoff the distance cutoff value
     */
    void setTraceCutoff(float fTraceCutoff);
    /**
     * set the distance cutoff value that determines the maximum distance for which consecutive residues are connected
     * \return the distance cutoff value
     */
    float getTraceCutoff();
    /**
     * get the secondary structure
     */ 

    //@}

    /**
     * \name Misc functions
     */
    //@{

    /**
     * Get the maximum b-factor
     */
    Real32 getMaxTempFact() const;

    /**
     * Get the average b-factor
     */
    Real32 getAvgTempFact() const;

    /**
     * should the atoms belonging to a water molecule be ignored in clustering this structure? Default: false.
     */
    void setIgnoreWater( bool bIgnoreWater );
    /**
     * shall we read HETATM records?
     * \param bHetAtm if true hetatm atoms are read (default: true)
     */
    void setHetAtm( bool bHetAtm );


    /**
     * sample the object randomly and return a vector that refrects the probability distribution of the object
     */
    T sample( );

    /**
     * Project mass
     * \param fWidth voxel width of the target map
     * \param bCA if true, only the CA atoms are evaluated
     */
    svt_volume<Real64>* projectMass(Real64 fWidth, bool bCA=false);

    /**
     * Project mass
     * \param pMap pointer to map that will hold projected structure
     * \param pTransformation pointer to transformation to apply (in Sculptor coordinates)
     * \param bCA if true, only the CA atoms are evaluated
     */
    void projectMass(svt_volume<Real64>* pMap, svt_matrix4<Real64> oTransformation = svt_matrix4<Real64>(), bool bCA=false, int iAtoms = -1);

    /**
     * Project distance
     * \param pMap pointer to map that will hold projected distances
     * \param pTransformation pointer to transformation to apply
     */
    void projectDistance(svt_volume<Real64>* pMap, svt_matrix4<Real64> oTransformation, bool bBackbone);
    /**
     * blur the pdb structure and thereby create an artificial low-resolution map
     * \param fWidth voxel width of the target map
     * \param fResolution resolution of the target map
     * \param fAdjX adjust x coordinates by this value (e.g. to uncenter) default: 0
     * \param fAdjY adjust y coordinates by this value (e.g. to uncenter) default: 0
     * \param fAdjZ adjust z coordinates by this value (e.g. to uncenter) default: 0
     * \param bProgress if true a progress bar is shown
     */
    svt_volume<Real64>* blur(Real64 fWidth, Real64 fResolution, Real64 fAdjX =0.0, Real64 fAdjY =0.0, Real64 fAdjZ =0.0, bool bProgress =false);
    /**
     * blur the pdb structure and thereby create an artificial low-resolution map
     * \param fWidth voxel width of the target map
     * \param fResolution resolution of the target map
     * \param fAdjX adjust x coordinates by this value (e.g. to uncenter) default: 0
     * \param fAdjY adjust y coordinates by this value (e.g. to uncenter) default: 0
     * \param fAdjZ adjust z coordinates by this value (e.g. to uncenter) default: 0
     * \param bProgress if true a progress bar is shown
     */
    svt_volume<Real64>* blur1D(Real64 fWidth, Real64 fResolution, Real64 fAdjX =0.0, Real64 fAdjY =0.0, Real64 fAdjZ =0.0, bool bProgress =false);

    /**
     * Project mass and correlation - no new volume will get created, the correlation is calculated on the fly
     * \param pMap pointer to map that holds the target map
     * \param pTransformation pointer to transformation to apply
     * \param bCA if true, only the CA atoms are evaluated
     */
    Real64 projectMassCorr(svt_volume<Real64>* pMap, svt_matrix4<Real64> oTransformation, bool bCA);

    /**
     * Blur and correlation - no new volume will get created, the correlation is calculated on the fly
     * \param pMap pointer to map that holds the target map
     * \param pTransformation pointer to transformation to apply
     * \param bCA if true, only the CA atoms are evaluated
     */
    Real64 blurCorr( svt_volume<Real64>* pKernel, svt_volume<Real64>* pMap, svt_matrix4<Real64> oTransformation, bool bCA);

    /**
     * Calculate rmsd between this and another structure. The other structure should have equal or larger size. If larger, an oligomer is assumed,
     * and the overlap with a chain is determined. The most overlapping chain is then used as reference and the rmsd is computed.
     * \param rOlig reference to other structure
     * \param bAlign align the two structures first (default: false) - valid only for the case if the two structures have the same size, ignored otherwise!
     */
    Real64 rmsd( svt_point_cloud_pdb<T>& rOlig, bool bAlign = false, Select iSelection=ALL, bool bShowProgress = true );

    /**
    * Calculate dRMSD (distance RMSD - intramolecular distances) between this and another structure.
    * \param rOlig reference to other structure
    */
    Real64 drmsd( svt_point_cloud_pdb<T>& rOlig );


    /**
     * Get a chain
     * \param cChainID the chain ID
     * \return returns the atoms of the chain as pointer to another svt_point_cloud_pdb object
     */
    svt_point_cloud_pdb<T> getChain(char cChainID);

    /**
    * Get the trace
    * \return returns the CA as svt_point_cloud_pdb object
    */
    svt_point_cloud_pdb<T> getTrace(unsigned int iSkip = 0);

    /**
     * gets the backbone of the structure
     * \return svt_point_cloud_pdb with the backbone
     */
    svt_point_cloud_pdb<T> getBackbone();

    /**
     * create a cylinder based on a circle of radius=fRadius with iPoints
     * The circle is copied on z axis iCircleCopies times using fDist as separation
     */
    void makeCylinder(unsigned int iPoints, Real64 fRadius, unsigned int iCircleCopies, Real64 fDist );

    /**
     * Get the number of atoms in
     * \param iSelection can be ALL, BACKBONE, or CA; with ALL- function equivalent with this->size()
     */
    unsigned int getAtomsNumber(Select iSelection);

    /**
     * Calculates and returns the Carbon Alpha (CA) of the atom's residue
     * \param iIndexAtom an integer representing the atom's index in the pdb structure
     * \return unsigned int - the index of the Carbon Alpha
     */
    unsigned int getCA(unsigned int iIndexAtom);

    /**
     * Determine it iIndexAtom is on the backbone
     * \param iIndexAtom an integer representing the atom's index in the pdb structure
     * \return unsigned int - iIndexAtom if the atom is on the backbone or the CA if not
     */
    unsigned int getBackbone(unsigned int iIndexAtom);

    /**
     * Compute the Distances between any atom of the structure using the topology - graph connections given in the PSF file
     */
    bool computePSFGraphDistMat();

    /**
     * \param bPSFRead a bool indicating if the psf files has been read;
     */
    void setPSFRead(bool bPSFRead);

    /**
     * get the symmetry Matrix transformations
     */
    vector<svt_matrix4<Real64> >& getSymmMats();
    //@}


     /**
     * \name Select functions
     */
    //@{

    /**
     * select atoms that are in the residue
     * \param iResidueSeq the number of the residue
     */
    void selectAtomResidueSeq( int iResidueSeq );

    /**
     * get the index of the selected/deselected atoms
     * \param bIsSelected the selection status = true - is selected; false  - is not selected
     * \return the index of the atoms with selected status indicated by bIsSelected
     */
    vector<unsigned int> getSelection(bool bIsSelected=true);

    //@}

    /**
     * \name Pair Distance Functions
     */
    //@{

    /**
     * Calculate the Pair Distribution Function of a part A of the molecule to another part B
     * \param fWidth is width of a bin (held constant between the range Dmin and Dmax)
     * \param bIsSelectedA represent the selection status of the part A (if true ~ the selected atoms are used)
     * \param bIsSelectedB represent the selection status of the part B (if true ~ the selected atoms are used)
     * \return the computed pair Distribution
     */
    svt_pair_distribution getSelectionPDF(Real64 fWidth, bool bIsSelectedA, bool bIsSelectedB);

    //@}

    /**
     *\name Symmetry
     */
    //@{
    /**
     * create symmetric oligomers
     * \param order is equivalent with the number of monomers in the symmetric unit
     * \param symmetry type
     * \param symmetry axis
     * \param fOffsetAxis1 is the offset form the first axis
     * \param fOffsetAxis2 is the offset form the second axis
     * \return pdb containing the symmetric oligomere
     */
    svt_point_cloud_pdb<T> applySymmetry(unsigned int iOrder=6, const SymmetryType eType=SYMMETRY_C, char cAxis='z', Real64 fOffsetAxis1=0, Real64 fOffsetAxis2=0);
    //@}

};

///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 */
template<class T>
svt_point_cloud_pdb<T>::svt_point_cloud_pdb() : svt_point_cloud<T>(),
m_bIgnoreWater( false ),
m_bLoadHetAtm( true ),
m_iAtomEnum( 0 ),
m_iBondEnum( 0 ),
m_iNumCAAtoms( 0 ),
m_iNumBBAtoms( 0 ),
m_bSecStructAvailable( false ),
m_eSecStructSource( SSE_NOTAVAILABLE ),
m_fTraceCutoff( 8.0f ),
m_pFilename( "" ),
m_bPSFRead ( false ){
};
/**
 * Copy Constructor for svt_point_cloud (not pdb) object
 */
template<class T>
svt_point_cloud_pdb<T>::svt_point_cloud_pdb( svt_point_cloud<T>& rOther) : svt_point_cloud<T>(),
m_bIgnoreWater( false ),
m_bLoadHetAtm( true ),
m_iAtomEnum( 0 ),
m_iBondEnum( 0 ),
m_iNumCAAtoms( 0 ),
m_iNumBBAtoms( 0 ),
m_bSecStructAvailable( false ),
m_eSecStructSource( SSE_NOTAVAILABLE ),
m_fTraceCutoff( 8.0f ),
m_pFilename( "" ),
m_bPSFRead ( false )
{
    svt_point_cloud_atom oAtom;

    for(unsigned int i=0; i<rOther.size(); i++)
        addAtom( oAtom, rOther[i] );
};


/**
 * Destructor
 */
template<class T>
svt_point_cloud_pdb<T>::~svt_point_cloud_pdb()
{
};


/**
 * product of matrix and point cloud
 */
template<class T>
inline svt_point_cloud_pdb<T> operator*(const svt_matrix4<Real64>& rM, svt_point_cloud_pdb<T>& rPC)
{
    svt_point_cloud_pdb<T> oPC( rPC );

    for(unsigned int i=0; i<rPC.size(); i++)
        oPC[i] = rM * rPC[i];

    return oPC;
};

///////////////////////////////////////////////////////////////////////////////
// File format functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Fetch a pdb file from www.rcsb.org
 * \param pPdbID pointer to array of complete file name to fetch
 */
template<class T>
bool svt_point_cloud_pdb<T>::fetchPDB(const char* pPdbID)
{
    svtout << "Fetching " << pPdbID << endl;

    //used to hold the HTTP GET command
    char cHTTPGetstr[60] = {};
    //local variable that holds whether PDB exists
    bool bPdbExist;
    //variables for the socket and data transmission 
#ifdef WIN32
    SOCKET iSockfd;
#else
    int iSockfd;
#endif
    int iHexval, iStatus, iNumbytes, iVal, iNum;
    //addrinfo sets variables given by the server
    struct addrinfo oHints, *pRes;
    //hexarray stores the number of bytes (in hex format) of each chunk to be received
    char cHexarray[9] = {};
    //one character buffer used in recv commands
    char cEachchar[1];
    //buffer gap to eliminate new lines and display correct hex values and data
    char cGap[2];
    //server to connect to
    const char cHost[] = "www.rcsb.org";

    // clear all old atoms
    deleteAllAtoms();
    //variables used for parseLine and passing parsestr into parseLine
    unsigned int iModel = 0;
    unsigned int iCount = 0;
    int iPrevResSeq = -1;
    int iOrdResSeq = 0;
    char cPrevChainID = '-';
    char cParsestr[MAXLINE] = {};

    //setup socket info
    memset(&oHints, 0, sizeof oHints);
    oHints.ai_family = AF_UNSPEC;
    oHints.ai_socktype = SOCK_STREAM;
    oHints.ai_protocol = 0;
    try
    {
        svt_exception::ui()->busyPopup("Fetch PDBID in progress, please wait...");

   #ifdef WIN32
        WSADATA wsadata;
        if (WSAStartup( MAKEWORD( 1,1 ), &wsadata ) != 0)
        {
	    svtout << "fetchPDB: couldn't initialize winsockets" << endl;
   	    return false;
        }
#endif
   
        //get addr info and setup/connect to socket with checks
        if ((iStatus = getaddrinfo(cHost, "80", &oHints, &pRes)) != 0)
        {
 	    svtout << "fetchPDB: getaddrinfo failure: " << gai_strerror(iStatus) << endl;
 	    return false;
        }

        iSockfd = socket(pRes->ai_family, pRes->ai_socktype, pRes->ai_protocol);
#ifdef WIN32
        if (iSockfd == INVALID_SOCKET)
        {
    	    svtout << "WSAGetLastError(): " << WSAGetLastError() << endl;
	    svtout << "fetchPDB: error during socket() call." << endl;
	    return false;
        }
#else
        if (iSockfd < 0)
        {
	    svtout << "fetchPDB: error during socket() call." << endl;
	    return false;
        }
#endif
   
        if ((iStatus = connect(iSockfd, pRes->ai_addr, pRes->ai_addrlen)) != 0)
        {
	    svtout << "fetchPDB: connect error." << endl;
	    return false;
        }

        freeaddrinfo(pRes);

        //setup string for the get command using HTTP/1.1 format, ie: GET <directory><filename> HTTP/1.1\nHost: <host>\n\n
        setupHTTPGet("/pdb/files/", pPdbID, cHost, cHTTPGetstr);
  
        //send the GET command to the server	
        iStatus = send(iSockfd, cHTTPGetstr, sizeof cHTTPGetstr, 0);
    
        //method to determine if PDB exists, sets global variable to false if it does not, otherwise eliminates header
        bPdbExist = peekPDB(iSockfd);
    
        //if PDB does not exist, this is the end of the fetchPDB method.  Report failure.
        if (!bPdbExist)
        {
   	    svtout << pPdbID << " is not in the database.  Connection closing." << endl;
            svt_exception::ui()->busyPopdown();
	    return false;
        }
    
        //loop to iteratively retrieve data if the PDBID exists
        while(true)
        {	
            svt_exception::ui()->busyRotate();

            //loop counter for hexarray location and termination of loop
	    iVal = 0;	
	    //get the hex value to serve as counter in data transmission - eliminate hex number in data
	    while(true)
	    {
	    
	        iNumbytes = recv(iSockfd, cEachchar, sizeof cEachchar, 0);
	        //check to see what is received.  If it is a letter or number, store it.  Otherwise, its the end of of the hex chunk
	        if (isalnum(*cEachchar))
	        {
		    //if the first number retrieved is 0, we're finished, otherwise store the variable and keep going.
		    if (iVal == 0 && cEachchar[0] == '0')
		    {
#ifdef WIN32
		        WSACleanup();
#else				
		        close(iSockfd);
#endif
			svt_exception::ui()->busyPopdown();
		        svtout << pPdbID << " fetched." << endl;
		        if (iCount == 0 && this->getMaxTimestep() > 1)
                            this->m_aPoints.pop_back();

		        if (m_aAtomModels.size() == 0)
		            m_aAtomModels.push_back( 0 );
              	        else
		            sort(m_aAtomModels.begin(), m_aAtomModels.end());
       
                        calcSecStructLengths();
                        svtout << "Loaded " << m_aSsePdb.size() << " secondary structure elements." << endl; 
		        svtout << "Loaded " << this->size() << " atoms from " << pPdbID << "." << endl;
		        this->setTimestep( 0 );
		        return true;
		    }
		    else
		        cHexarray[iVal] = cEachchar[0];
			
	        }
	        else 
	        {
	            cHexarray[iVal] = '\0';
		    break;
	        }
	        iVal++;
            }

	    //gap of one after hex number for proper formatting of pdb data
	    recv(iSockfd, cEachchar, sizeof cEachchar, 0);
	
	    //convert hexarray (which stores the 4 or less digit hex number) into decimal and use as counter.
	    iHexval = hextoint(cHexarray);
	
	    //Determine where in the line we are.  If this is a new line, then setup new line with iNum = 0. Otherwise, 
	    //the previous chunk ended midline, so we must resume finishing the line from the next chunk.  This correctly
	    //sets the placeholder (iNum) for the next data value in the line (cParsestr).
	    if (strlen(cParsestr) == 0)
	        iNum = 0;
	    else
	        iNum = strlen(cParsestr);
	
	    //Using the counter (iHexval) - ie, the size of the chunk, continue receiving data until the chunk is finished.
	    //Store data into a line (cParsestr).  When we finish the line, send to parseLine and reset placeholder (iNum) and string.  If
	    //the chunk is ended, add a null character to the end of the string.  This will be used as indicated above to 
	    //determine the length and therefore the placeholder of the line.
	    while (iHexval > 0)
	    {
	        recv(iSockfd, cEachchar, sizeof cEachchar, 0);
	        iHexval--;
	        if (*cEachchar == '\n')
	        {
	            cParsestr[iNum] = '\n';
		    parseLine(cParsestr, iModel, iCount, iPrevResSeq, iOrdResSeq, cPrevChainID);
		    iNum = 0;
		    cParsestr[0] = '\0';
	        }
	        else
	        {
	            cParsestr[iNum] = *cEachchar;
		    iNum++;
		    if (iHexval == 0)
		        cParsestr[iNum] = '\0';
	        }
	    }
	    //space gap after chunk has been sent for proper formatting of the pdb data
	    recv(iSockfd, cGap, sizeof cGap, 0);
        }
 
    }
    catch (svt_userInterrupt&)
    {
	svt_exception::ui()->busyPopdown();
	deleteAllAtoms();
#ifdef WIN32
        WSACleanup();
#else				
	close(iSockfd);
#endif
        return true;
    }
    return true;
};

/**
 * Load a pdb file.
 * \param pFilename pointer to array of char with the filename
 */
template<class T>
void svt_point_cloud_pdb<T>::loadPDB(const char* pFilename)
{
    char m_pLine[MAXLINE];
    FILE* m_pFile;
    bool bEof;

    svtout << "Opening " << pFilename << endl;
    //open file
    m_pFile = fopen(pFilename, "r");

    if (m_pFile != NULL)
        bEof = (bool)(feof(m_pFile));
    else
    	bEof = true;

    if (!bEof)
	    m_pFilename.assign(pFilename);
    else
    { 
	    svtout << "Error opening file: " << pFilename << endl;
	    return;
    }

    // clear all old atoms
    deleteAllAtoms();
    unsigned int iModel = 0;
    unsigned int iCount = 0;
    int iPrevResSeq = -1;
    int iOrdResSeq = 0;
    char cPrevChainID = '-';

    // read until next atom record reached
    while(true)
    {

        if (fgets(m_pLine, MAXLINE, m_pFile) == NULL)
        {
            if (iCount == 0 && this->getMaxTimestep() > 1)
                this->m_aPoints.pop_back();

	        if (m_aAtomModels.size() == 0)
	            m_aAtomModels.push_back( 0 );
            else
	            sort(m_aAtomModels.begin(), m_aAtomModels.end());
	    
            this->setTimestep( 0 );

            calcOrdinalChainIDs();
            calcSecStructLengths(); 

	        svtout << "Loaded " << m_aSsePdb.size() << " secondary structure elements." << endl;
            svtout << "Loaded " << this->size() << " atoms from file " << pFilename << "." << endl;
	        return;
	    }
        else 
 	        parseLine(m_pLine, iModel, iCount, iPrevResSeq, iOrdResSeq, cPrevChainID);	 
	  
    }

};


/**
 * Write pdb file.
 * \param pFilename pointer to array of char with the filename
 * \param bAppend if true, the pdb structure as append at the end of an existing structure file
 * \param bFirst if true only the first frame is written to the pdb file
 */
template<class T>
void svt_point_cloud_pdb<T>::writePDB(const char* pFilename, bool bAppend, bool bAddRemarks, bool bFirst)
{
    unsigned int iNum = m_aAtoms.size();

    FILE* pFile = NULL;
    if (!bAppend)
	pFile = fopen(pFilename, "wt");
    else {
	pFile = fopen(pFilename, "a");
    }

    if (pFile == NULL)
    {
        svtout << "Error: Unable to write to file " << pFilename << "! Please check permissions, disk space, etc." << endl;
        return;
    }

    if (bAddRemarks)
    {
        for (unsigned int iIndexRemark = 0; iIndexRemark < m_aRemarks.size(); iIndexRemark++)
        {
	   fprintf(pFile, "REMARK %s",m_aRemarks[iIndexRemark].c_str());
        }
    }
    unsigned int iModel = m_aAtoms[0].getModel();

 //code could be used if the modified res are to be outputed into the PDB
//     for (unsigned int iModRes=0; iModRes<m_aModResidues.size(); iModRes++ )
//     {
// 	fprintf(pFile, "MODRES %s %s %s%s%s%s  %s\n",m_aModResidues[iModRes][0].c_str(),m_aModResidues[iModRes][1].c_str(), m_aModResidues[iModRes][2].c_str(),
// 		m_aModResidues[iModRes][3].c_str(),m_aModResidues[iModRes][4].c_str(),m_aModResidues[iModRes][5].c_str(),m_aModResidues[iModRes][6].c_str() );
//     }
//

    if (m_eSecStructSource==SSE_OTHER)
        compressSecStruct(); 

    int iNoHelices = 0;
    int iNoSheets = 0;
    for (unsigned int iSse=0; iSse< m_aSsePdb.size(); iSse++)
    {
        if (strcmp(m_aSsePdb[iSse].m_aType,"HELIX ")==0) // they are the same
        {
            iNoHelices++;
            fprintf(pFile, "%s %3i %3s %3s %c %4i%c %3s %c %4i%c%2i%30s %5i%4s\n",
                m_aSsePdb[iSse].m_aType,
                m_aSsePdb[iSse].m_iNum%1000,
                m_aSsePdb[iSse].m_aID, 
                m_aSsePdb[iSse].m_aInitialResname,
                m_aSsePdb[iSse].m_cInitialResChainID,
                m_aSsePdb[iSse].m_iInitialResSeq,
                m_aSsePdb[iSse].m_cInitialICode,
                m_aSsePdb[iSse].m_aTerminalResname,
                m_aSsePdb[iSse].m_cTerminalResChainID,
                m_aSsePdb[iSse].m_iTerminalResSeq,
                m_aSsePdb[iSse].m_cTerminalICode,
                m_aSsePdb[iSse].m_iClass,
                m_aSsePdb[iSse].m_aComment,
                m_aSsePdb[iSse].m_iLen,
                " "
              );
        }

    }
    
    for (unsigned int iSse=0; iSse< m_aSsePdb.size(); iSse++)
    {
        if (strcmp(m_aSsePdb[iSse].m_aType,"SHEET ")==0) // they are the same
        {
            iNoSheets++;
            fprintf(pFile, "%s %3i %3s%2i %3s %c%4i%c %3s %c%4i%c%2d %4s%3s %c%4s%c %4s%3s %c%4s%c%10s\n",
                m_aSsePdb[iSse].m_aType,
                m_aSsePdb[iSse].m_iNum,
                m_aSsePdb[iSse].m_aID,
                m_aSsePdb[iSse].m_iNumStrands,
                m_aSsePdb[iSse].m_aInitialResname,
                m_aSsePdb[iSse].m_cInitialResChainID,
                m_aSsePdb[iSse].m_iInitialResSeq,
                m_aSsePdb[iSse].m_cInitialICode,
                m_aSsePdb[iSse].m_aTerminalResname,
                m_aSsePdb[iSse].m_cTerminalResChainID,
                m_aSsePdb[iSse].m_iTerminalResSeq,
                m_aSsePdb[iSse].m_cTerminalICode,  
                m_aSsePdb[iSse].m_iSense,
                m_aSsePdb[iSse].m_aCurAtom,
                m_aSsePdb[iSse].m_aCurResname,
                m_aSsePdb[iSse].m_cCurResChainID,
                m_aSsePdb[iSse].m_aCurResSeq,
                m_aSsePdb[iSse].m_cCurICode,
                m_aSsePdb[iSse].m_aPrevAtom,
                m_aSsePdb[iSse].m_aPrevResname,
                m_aSsePdb[iSse].m_cPrevResChainID,
                m_aSsePdb[iSse].m_aPrevResSeq,
                m_aSsePdb[iSse].m_cPrevICode,
                " "
              );
        }
    }


    int iTS;
    char cChainID;
    char pRecordName[7];

    for(iTS = 0; iTS < this->getMaxTimestep(); iTS++)
    {
    //	svtout << "iTS:  " << iTS  << endl;

	if (this->getMaxTimestep() > 1)
            svtout << "  Writing frame " << iTS << " / " << this->getMaxTimestep() << endl;

        this->setTimestep( iTS );

	if (m_aAtomModels.size() > 1)
	    fprintf( pFile, "MODEL        %i\n", iModel);


	for(unsigned int i=0; i<iNum; i++)
	{

		 if (m_aAtoms[i].getModel() != iModel)
	    {
		fprintf( pFile, "ENDMDL\n");
		fprintf( pFile, "MODEL        %i\n", m_aAtoms[i].getModel());
		iModel = m_aAtoms[i].getModel();
	    }

            cChainID = m_aAtoms[i].getChainID();
	    if (cChainID == '-')
		cChainID = ' ';

        if (m_aAtoms[i].getHetAtm())
           sprintf(pRecordName,"%s","HETATM\0");
        else
           sprintf(pRecordName,"%s","ATOM  \0");

           fprintf(pFile, "%s%5i %2s%c%c%c%-2s %c%4i%c   %8.*f%8.*f%8.*f%6.2f%6.2f %3s  %4s%2s%2s\n",
		   pRecordName,
		   m_aAtoms[i].getPDBIndex(),
		   m_aAtoms[i].getName(),
		   m_aAtoms[i].getRemoteness(),
		   m_aAtoms[i].getBranch(),
		   m_aAtoms[i].getAltLoc(),
		   m_aAtoms[i].getResname(),
		   cChainID,
		   m_aAtoms[i].getResidueSeq(),
		   m_aAtoms[i].getICode(),
		   CoordPrecisionPDB(this->getPoint(i).x()), this->getPoint(i).x(),
		   CoordPrecisionPDB(this->getPoint(i).y()), this->getPoint(i).y(),
		   CoordPrecisionPDB(this->getPoint(i).z()), this->getPoint(i).z(),
		   m_aAtoms[i].getOccupancy(),
		   m_aAtoms[i].getTempFact(),
		   m_aAtoms[i].getNote(),
		   m_aAtoms[i].getSegmentID(),
		   m_aAtoms[i].getElement(),
		   m_aAtoms[i].getCharge()
		   );

	}

	if (m_aAtomModels.size() > 1)
	{
	    fprintf( pFile, "ENDMDL\n");
	}

	if (bFirst)
            break;

        if (this->getMaxTimestep() > 0)
	    fprintf( pFile, "END\n");
    }


    this->setTimestep( 0 );

    fclose(pFile);
};

/**
 * opens and reads a psf file
 */
template<class T>
bool svt_point_cloud_pdb<T>::loadPSF(const char* pFilename)
{
    unsigned int iNrAtoms, iNrBonds;
    unsigned int i, j;

     //yes the psf file has been read
    m_bPSFRead = true;


    // do we have to create also new coordinate entries?
    bool bNewAtoms;
    if (m_aAtoms.size() > 0)
	bNewAtoms = false;
    else
        bNewAtoms = true;

    // open file
    svt_column_reader oReader( pFilename );
    if (oReader.eof())
    {
        cout << "loadPSF: cannot load psf file!" << endl;
        return false;
    }

    // load the header
    oReader.readLine();
    char* pBuffer = oReader.extractString(0,3);
    if( pBuffer[0] != 'P' && pBuffer[1] != 'S' && pBuffer[2] != 'F' )
    {
        svtout << "loadPSF: cannot load psf file - first string != PSF " << endl;
        return false;
    }
    // read empty line after PSF
    oReader.readLine();

    // number of lines in the header
    unsigned int iNrLines;
    oReader.readLine();
    iNrLines = oReader.extractInt( 0, 7 );
    for(i = 0; i < iNrLines; i++)
	oReader.readLine();
    // read empty line after header
    oReader.readLine();

    // read the number of atoms in the file
    oReader.readLine();
    iNrAtoms = oReader.extractInt( 0, 7 );
    unsigned int iAtomNr = 0;
    char pName[3]; pName[0] = 0; pName[1] = 0; pName[2] = 0;
    // read the atoms
    for (i = 0; i < iNrAtoms; ++i)
    {
	oReader.readLine();

	svt_point_cloud_atom oAtom;

	// atom number
	iAtomNr = oReader.extractInt( 0, 7 );
        oAtom.setPDBIndex( iAtomNr );
	// segment id
        oAtom.setSegmentID( oReader.extractString( 9, 12 ) );
	// residue number
	oAtom.setResidueSeq( oReader.extractInt( 14, 17 ) );
	// residue name
	oAtom.setResname( oReader.extractString( 19, 21 ) );
        // atom name
        pName[0] = oReader.extractChar( 23 );
        pName[1] = oReader.extractChar( 24 );
        oAtom.setName( pName );
        // atom remoteness
	oAtom.setRemoteness( oReader.extractChar( 25 ) );
        oAtom.setBranch( oReader.extractChar( 26 ) );
	// atom type
	//oReader.extractString( 30, 32 );
	// atom charge
	oAtom.setCharge( oReader.extractString( 35, 48 ) );
	// atom mass
	oAtom.setMass( oReader.extractReal32( 49, 59 ) );

        // some defaults
	oAtom.setAltLoc(' ');
	oAtom.setChainID('A');
	oAtom.setICode(' ');
	oAtom.setOccupancy( 1.0 );
	oAtom.setTempFact( 1.0 );

	//create atom
	if (bNewAtoms)
	{
	    svt_vector4<Real64> oNull(0.0, 0.0, 0.0);

	    // no coordinates in a psf file
	    addAtom( oAtom, oNull );

	} else
        {
            //do not update information cause the pdb already has an entry relative to this atom
        }
    }
    // cut the blank line after the atom record
    oReader.readLine();

    // get the number of bonds
    oReader.readLine();
    iNrBonds = oReader.extractInt( 0, 7 );

    unsigned int iA, iB, iBonds;
    iBonds = 0;
    while( iBonds < iNrBonds )
    {

	oReader.readLine();

	for(j=0; j < oReader.getLength()-16; j+=16)
	{
	    iA = oReader.extractInt( j+0, j+7 );
	    iB = oReader.extractInt( j+8, j+15 );

	    addBond( iA-1, iB-1 );

            iBonds++;
	}
    }




    return true;
};

/**
 * Write psf file
 * \param pFilename pointer to array of char with the filename
 */
template<class T>
void svt_point_cloud_pdb<T>::writePSF(const char* pFilename)
{
    unsigned int i,k = 0;

    FILE* pFout = fopen(pFilename, "w");

    fprintf(pFout, "PSF \n");
    fprintf(pFout, " \n");
    fprintf(pFout, "       1 !NTITLE\n");
    fprintf(pFout, " REMARK Connectivity file created by SVT/Sculptor - Attention: Atm, only the connectivity information is valid in this file!\n");
    fprintf(pFout, " \n");
    fprintf(pFout, "%8d !NATOM\n", this->size());

    for( i=0; i<this->size(); i++ )
	fprintf(pFout, "%8d %4s %-4d %4s QVOL QVOL   0.000000       0.00000           0\n", this->atom(i).getPDBIndex(), this->atom(i).getSegmentID(), this->atom(i).getResidueSeq(), this->atom(i).getResname() );

    //fprintf(pFout, "%8d QVOL %-4d QVOL QVOL QVOL   0.000000       0.00000           0\n", i, i+1);

    fprintf(pFout, " \n");

    fprintf(pFout, "%8d !NBOND: bonds\n", (int)m_aBonds.size());

    for (i=0; i<m_aBonds.size(); i++)
    {
	if (k == 4)
	{
	    k = 0;
	    fprintf(pFout, " \n");
	}
	k++;
	fprintf( pFout, "%8d%8d", m_aBonds[i].getIndexA()+1, m_aBonds[i].getIndexB()+1 );
    }
    if (k > 0) fprintf(pFout, "\n");

    fprintf(pFout, "\n");
    fprintf(pFout, "       0 !NTHETA: angles\n");
    fprintf(pFout, "\n");
    fprintf(pFout, "       0 !NPHI: dihedrals\n");
    fprintf(pFout, "\n");
    fprintf(pFout, "       0 !NIMPHI: impropers\n");
    fprintf(pFout, "\n");
    fprintf(pFout, "       0 !NDON: donors\n");
    fprintf(pFout, "\n");
    fprintf(pFout, "       0 !NACC: acceptors\n");
    fprintf(pFout, "\n");
    fprintf(pFout, "       0 !NNB\n");
    fprintf(pFout, "\n");
    fprintf(pFout, "       0       0 !NGRP\n");
    fprintf(pFout, "\n");

    fclose (pFout);

    svtout << "Connectivity data written to PSF file " << pFilename << endl;

};


/**
 * loads a dcd file
 * \param oFilename filename
 */
template<class T>
bool svt_point_cloud_pdb<T>::loadDCD(const char* pFilename)
{
    int j;
    int i=0;

    //
    // Open file
    //
    FILE* pFile = fopen( pFilename, "rb" );
    if (pFile == NULL)
    {
        svtout << "Cannot open dcd file: " << pFilename << endl;
        return false;
    }
    m_pFilename = pFilename;

    svtout << "Load DCD: opening " << pFilename << " for binary reading" << endl;

    //
    // First Block
    //
    Int32 iBlockSize = svt_readInt32( pFile );
    if (iBlockSize != 84)
    {
        svtout << "Load DCD:" << "\n\tDCD file may be corrupt" << "\n\tThe size of the first block is not 84" << endl;
        return false;
    }

    // CORD
    if (svt_readInt8( pFile ) != 'C' || svt_readInt8( pFile ) != 'O' || svt_readInt8( pFile ) != 'R' || svt_readInt8( pFile ) != 'D')
    {
        svtout << "Load DCD: This is not a coordinate DCD file! Skipping..." << endl;
        return false;
    }

    // HEADER
    Int32 iNrSets = svt_readInt32( pFile );
    svtout << "  NSET    = " << iNrSets << endl;
    svtout << "  ISTRT   = " << svt_readInt32( pFile ) << endl;
    svtout << "  NSAVC   = " << svt_readInt32( pFile ) << endl;
    svtout << "  5-ZEROS = " << svt_readInt32( pFile ) << ", " << svt_readInt32(pFile) << ", " << svt_readInt32(pFile) << ", " << svt_readInt32(pFile) << ", " << svt_readInt32(pFile) << endl;
    svtout << "  NAMNF   = " << svt_readInt32( pFile ) << endl;
    svtout << "  DELTA   = " << svt_readReal64( pFile ) << endl;
    svtout << "  9-ZEROS = " << svt_readInt32( pFile ) << ", " << svt_readInt32(pFile) << ", " << svt_readInt32(pFile) << ", " << svt_readInt32(pFile) << ", " << svt_readInt32(pFile) << ", " << svt_readInt32(pFile) << ", " << svt_readInt32(pFile) << ", " << svt_readInt32(pFile) << ", " << svt_readInt32(pFile) << endl;

    // End blocksize of first block
    //fseek(pFile, (11*sizeof(Int32) + sizeof(Real64) + 9*sizeof(Int32)), SEEK_SET);
    iBlockSize = svt_readInt32( pFile );
    if (iBlockSize != 84)
    {
        svtout << "Load DCD: DCD file may be corrupt, the end-size of the first block is not 84" << endl;
        return false;
    }


    //
    // Second block, title block
    //
    iBlockSize = svt_readInt32( pFile );

    // number of strings in title block
    Int32 iNrStrings = svt_readInt32( pFile );
    svtout << "  NTITLE  = " << iNrStrings << endl;
    if (iBlockSize != (4 + (80 * iNrStrings) ) )
    {
	svtout << "Load DCD: DCD file may be corrupt, the size of the title record is not (4 + 80*n)" << endl;
	return false;
    }

    char pTitle[100];
    for(i=0; i<iNrStrings; i++)
    {
	svt_readBuffer( pFile, 80, pTitle );
        pTitle[80] = 0;
	svtout << "            " << i << " : " << pTitle << endl;
    }

    //fseek( pFile, (91 + 8 + (iNrStrings * 80) + 1), SEEK_SET );
    //
    // End blocksize of second block
    iBlockSize = svt_readInt32( pFile );
    if (iBlockSize != (4 + (80 * iNrStrings) ) )
    {
        svtout << "Load DCD: DCD file may be corrupt, the end-size of the title record is not (4 + 80*n)" << endl;
        return false;
    }


    //
    // Third block, number of atoms
    //
    iBlockSize = svt_readInt32( pFile );
    if (iBlockSize != 4)
    {
        svtout << "Load DCD: DCD file may be corrupt" << endl;
        return false;
    }

    // read the number of atoms
    int iAtomNr = svt_readInt32( pFile );
    svtout << "  NATM    = " << iAtomNr << endl;
    if (iAtomNr != (int)(m_aAtoms.size()))
    {
        svtout << "Load DCD: DCD file has different number of atoms compared to current molecule?" << endl;
        return false;
    }

    // End blocksize of third block
    iBlockSize = svt_readInt32( pFile );
    if (iBlockSize != 4)
    {
        svtout << "Load DCD: DCD file may be corrupt" << endl;
        return false;
    }


    //
    // Next blocks, coordinates
    //
    for(j = 0; j < iAtomNr; j++)
    {
	this->m_aPoints[0][j].x( 0 );
	this->m_aPoints[0][j].y( 0 );
	this->m_aPoints[0][j].z( 0 );
    }

    i=0;

    // load in the coords
    while(!feof(pFile))
    {
	svtout << "  Reading timestep: " << i << "\r";

        //
	// X column
        //
	iBlockSize = svt_readInt32( pFile );
	if (iBlockSize == 0)
            break;
	if (iBlockSize != 4*iAtomNr)
	{
	    svtout << "Load DCD: DCD file may be corrupt" << endl;
	    svtout << "  Blocksize start of X column: " << iBlockSize << endl;
	    return false;
	}
	for(j = 0; j < iAtomNr; j++)
	    this->m_aPoints[i][j].x( svt_readReal32( pFile ) );
	iBlockSize = svt_readInt32( pFile );
	if (iBlockSize != 4*iAtomNr)
	{
	    svtout << "Load DCD: DCD file may be corrupt" << endl;
	    svtout << "  Blocksize end of X column: " << iBlockSize << endl;
	    return false;
	}

	//
	// Y column
        //
        iBlockSize = svt_readInt32( pFile );
	if (iBlockSize != 4*iAtomNr)
	{
	    svtout << "Load DCD: DCD file may be corrupt" << endl;
	    svtout << "  Blocksize start of Y column: " << iBlockSize << endl;
	    return false;
	}
        for(j = 0; j < iAtomNr; j++)
	    this->m_aPoints[i][j].y( svt_readReal32( pFile ) );
	iBlockSize = svt_readInt32( pFile );
	if (iBlockSize != 4*iAtomNr)
	{
	    svtout << "Load DCD: DCD file may be corrupt" << endl;
	    svtout << "  Blocksize end of Y column: " << iBlockSize << endl;
	    return false;
	}

	//
	// Z column
        //
	iBlockSize = svt_readInt32( pFile );
	if (iBlockSize != 4*iAtomNr)
	{
	    svtout << "Load DCD: DCD file may be corrupt" << endl;
	    svtout << "  Blocksize start of Z column: " << iBlockSize << endl;
	    return false;
	}
	for(j = 0; j < iAtomNr; j++)
	    this->m_aPoints[i][j].z( svt_readReal32( pFile ) );
	iBlockSize = svt_readInt32( pFile );
	if (iBlockSize != 4*iAtomNr)
	{
	    svtout << "Load DCD: DCD file may be corrupt" << endl;
	    svtout << "  Blocksize end of Z column: " << iBlockSize << endl;
	    return false;
	}

        // next timestep
	this->addTimestep();
        i++;
    }

    this->setTimestep( 0 );

    fclose( pFile );

    cout << endl;
    svtout << "Load DCD: Finished loading " << i << " timesteps" << endl;

    return true;
};

///////////////////////////////////////////////////////////////////////////////
// File Format Helper Functions
///////////////////////////////////////////////////////////////////////////////

/**
 * setupHTTPGet is a method that constructs the HTTP/1.1 GET command from it's parameters.
 * \param cDirectory the exact directory from the server
 * \param cFilename the file to retrieve 
 * \param cHost the server to connect to
 * \param cGetstr the string that will contain the entire GET command  
 */
template<class T>
void svt_point_cloud_pdb<T>::setupHTTPGet(const char* cDirectory, const char* cFilename, const char* cHost, char* cGetstr)
{
    strcat(cGetstr, "GET ");
    strcat(cGetstr, cDirectory);
    strcat(cGetstr, cFilename);
    strcat(cGetstr, " HTTP/1.1\nHost: ");
    strcat(cGetstr, cHost);
    strcat(cGetstr, "\n\n");
};

/**
 * hexValue is a method that converts a single hex digit into decimal.  
 * \param cHexchar the character to convert  
 * \return decimal base 10 of hex character
 */
template<class T>
int svt_point_cloud_pdb<T>::hexValue(char cHexchar)
{
    const char *pDigits = "0123456789ABCDEF";
    int iNum;
    int iLen = strlen(pDigits);
    for (iNum = 0; iNum < iLen; iNum++)
    {
	if (toupper ( (unsigned char) cHexchar) == pDigits[iNum])
		break;
    }

    return iNum;
};

/**
 * hextoint is a method that converts an entire hex string into decimal.  
 * \param pHexstr pointer to the string to convert 
 * \return decimal base 10 of entire hex string
 */
template<class T>
int svt_point_cloud_pdb<T>::hextoint(char *pHexstr)
{
    int iLen = strlen(pHexstr);
    int iPower = 0;
    int iResult = 0;
    int iNum;
    for (iNum = iLen-1; iNum >= 0; iNum--)
    {
	iResult = iResult + (int)pow(16.0, iPower) * hexValue(pHexstr[iNum]);
	iPower++;
    }
    return iResult;
};

/**
 * peekPDB is a method that retrieves the HTTP header of the chosen PDBID. It checks the first 
 * line of the HTTP header to determine whether the PDBID is valid.  If it is not, it closes the 
 * connection, and returns false.  Otherwise, the method retrieves the entire header and returns true.
 * \param iSocket the socket used to communicate/recv with the server  
 */
template<class T>
bool svt_point_cloud_pdb<T>::peekPDB(int iSocket)
{
    char cSingle[1];
    int iReturnChar = 0;
    //do while loop to check the first line of http header - specifically checks for any 4's in the first line
    do
    {
        recv(iSocket, cSingle, sizeof cSingle, 0);
	//first line contains a 4, indicative of any number of 400 server commands (all failures) so return false
	if (*cSingle == '4')
	{
#ifdef WIN32
	    WSACleanup();
#else				
            close(iSocket);
#endif
	    return false;
	}
		
    } while(*cSingle != '\r' && *cSingle != '\n'); 
    //if we made it this far, then the PDB exists, so let's remove the rest of the http header before passing it on by checking for 2
    //consecutive '\r''\n' chars  - indicating the end of the header and return true.
    while (iReturnChar < 4)
    {
        recv(iSocket, cSingle, sizeof cSingle, 0);
	if (*cSingle == '\r' || *cSingle == '\n')
	    iReturnChar++;
	else
	    iReturnChar = 0;
    }
    return true;
};

/**
 * parseLine is a method that reads one line from pdb data, parses the information, and 
 * integrates it into the pdb object.
 * \param pLine pointer to one line of data
 * \param iModel integer of which molecular model
 * \param iCount integer of number of atoms
 * \param iPrevSeq integer of previous residue sequence
 * \param iOrdSeq integer of ordinal sequence
 * \param cPrevChain character of previous chain ID
 */
template<class T>    
void svt_point_cloud_pdb<T>::parseLine(const char* pLine, unsigned int& iModel, unsigned int& iCount, int& iPrevSeq, int& iOrdSeq, char& cPrevChain)
{
    string oRemark;
    Real64 fX, fY, fZ;
    string oBuffer = pLine;

    //if it does not have 80 characters just add empty spaces
    while (oBuffer.size()<80)
        oBuffer += " ";

    //make sure input line is long enough for substring commands...if not, tell user.
    if (oBuffer.length() < 3)
    {
	svtout << "PDB file corrupt or missing data." << endl;
	return;
    }

    string pBuffer = oBuffer.substr(0,6);
    string oEndframeA = oBuffer.substr(0,3);
    string oEndframeB = oBuffer.substr(3,1);
    
    //is it an ATOM record?
    if (pBuffer == "ATOM  " || (pBuffer == "HETATM" && m_bLoadHetAtm))
    {
	svt_point_cloud_atom oAtom;

      	// index
	int index = atoi((oBuffer.substr( 6,5 )).c_str());
	oAtom.setPDBIndex( index );
	// model number
       	oAtom.setModel( iModel );

	//now parse the coordinates
	fX = atof((oBuffer.substr( 30, 8 )).c_str());
	fY = atof((oBuffer.substr( 38, 8 )).c_str());
	fZ = atof((oBuffer.substr( 46, 8 )).c_str());

     	if ( this->getMaxTimestep() == 1 )
	{
	    this->addPoint( fX, fY, fZ );

	    //type information - str
	    oAtom.setName( (oBuffer.substr( 12, 2 )).c_str() );
	    //type char
	    oAtom.setRemoteness( (oBuffer.substr( 14, 1)).c_str()[0]  );
	    //type char
	    oAtom.setBranch( (oBuffer.substr( 15, 1)).c_str()[0]  );
	    //alternate location identifier information type char
	    oAtom.setAltLoc( (oBuffer.substr( 16, 1)).c_str()[0]  );
	    //resname information type str
	    oAtom.setResname( (oBuffer.substr( 17, 4 )).c_str());
	    //chain id type char
	    char cChainID = (oBuffer.substr( 21, 1)).c_str()[0]; 
	    if (cChainID == ' ')
		cChainID = '-';
            oAtom.setChainID( cChainID );
            if (cChainID != cPrevChain)
                iOrdSeq = 0;
            cPrevChain = cChainID;
	    // residue sequence number type int
            oAtom.setResidueSeq( atoi((oBuffer.substr( 22,4 )).c_str()) );
            // ordinal sequence number
            if (iPrevSeq != oAtom.getResidueSeq() )
                iOrdSeq++;
            iPrevSeq = oAtom.getResidueSeq();
            oAtom.setOrdResidueSeq( iOrdSeq );
            // iCode type char
	    oAtom.setICode((oBuffer.substr( 26, 1 )).c_str()[0]);
	    // fOccupancy type Real32
	    oAtom.setOccupancy( atof((oBuffer.substr( 54, 6 )).c_str()));
	    // temperature b factor type Real32
	    oAtom.setTempFact( atof((oBuffer.substr( 60, 6 )).c_str()));
	    // note type string
	    oAtom.setNote( (oBuffer.substr( 67, 3 )).c_str() );
	    // segment id type string
	    oAtom.setSegmentID((oBuffer.substr( 72, 4 )).c_str() );
	    // element symbol type string
	    oAtom.setElement((oBuffer.substr( 76, 2 )).c_str()  );
	    // charge type string
	    oAtom.setCharge( (oBuffer.substr( 78, 2 )).c_str() );

	    // adjust mass
	    oAtom.adjustMass();

	    m_aAtoms.push_back( oAtom );

            if (oAtom.isCA())
                m_iNumCAAtoms++;

            if (oAtom.isBackbone())
                m_iNumBBAtoms++;
	} 
	else 
	{

            if ( iCount >= m_aAtoms.size() )
            {
                svtout << "Error, file contains a trajectory with frames consisting of a varying number of atoms. Loading aborted!" << endl;
                return;
            }

	    T oVec;
	    oVec.x( fX );
	    oVec.y( fY );
	    oVec.z( fZ );

	    this->setPoint( iCount, oVec );

	    // increase local counter
	    iCount++;

	}

	// is it an HETATM record?
        if (m_bLoadHetAtm && pBuffer == "HETATM" )
	{
	    int iModResNo;
	    bool bIsModRes = false;
	    int iIndexModRes = 0;
	    for (unsigned int iModRes=0; iModRes < m_aModResidues.size(); iModRes++ )
	    {
		sscanf(m_aModResidues[iModRes][3].c_str(),"%i", &iModResNo);
		if (m_aAtoms[ m_aAtoms.size() - 1].getResname() == m_aModResidues[iModRes][1] && m_aAtoms[ m_aAtoms.size() - 1].getChainID() == m_aModResidues[iModRes][2][0] &&
		    m_aAtoms[ m_aAtoms.size() - 1].getResidueSeq() == iModResNo)
		{
		    bIsModRes = true;
		    iIndexModRes = iModRes;
		}
	    }
            m_aAtoms[ m_aAtoms.size() - 1].setHetAtm( !bIsModRes );

	    if (bIsModRes)
		m_aAtoms[ m_aAtoms.size() - 1].setResname( m_aModResidues[iIndexModRes][5].c_str() );
	}
    
        // see if atom is in secondary structure element
        if (m_aSsePdb.size()>0)
        {
            //default secondary structure element is coil
            char cSse = getSecStructFromCompressedList( m_aAtoms[ m_aAtoms.size() - 1] );  
            m_aAtoms[ m_aAtoms.size() - 1].setSecStruct(cSse);
        }
    }
	

    // new frame?
    if (oEndframeA == "END" && oEndframeB != "M")
    {
	iCount = 0;
	this->addTimestep();

	svtout << "  Found frame " << this->getMaxTimestep() << endl;
    }

    // each model is treated as chain!
    if (pBuffer == "MODEL ")
    {
	string pBufferInt = oBuffer.substr(12,4);
	iModel = (unsigned int)(atoi(pBufferInt.c_str()));

	svtout << "  Found Model: " << iModel << endl;

	if (find(m_aAtomModels.begin(), m_aAtomModels.end(), iModel) == m_aAtomModels.end())
	    m_aAtomModels.push_back( iModel );
    }

    if (pBuffer == "REMARK")
    {
        string oRemark = oBuffer.substr(7, MAXLINE - 7);
        addRemark( oRemark );

        string pBufferExt = oBuffer.substr(0,18);
        if (pBufferExt == "REMARK 350   BIOMT") // found multiple copies 
        {
            int iLineId = atoi((oBuffer.substr(18,1)).c_str());
            int iMatId  = atoi((oBuffer.substr(19,4)).c_str());
            //cout << oBuffer;
            while ((int)m_aSymmMats.size()<iMatId)
            {
              svt_matrix4<Real64> oMat;
              m_aSymmMats.push_back(oMat);
            }
            m_aSymmMats[iMatId-1][iLineId-1][0] = atof((oBuffer.substr(24,10)).c_str());
            m_aSymmMats[iMatId-1][iLineId-1][1] = atof((oBuffer.substr(34,10)).c_str());
            m_aSymmMats[iMatId-1][iLineId-1][2] = atof((oBuffer.substr(44,10)).c_str());
            m_aSymmMats[iMatId-1][iLineId-1][4] = atof((oBuffer.substr(54,15)).c_str());

            //if (iLineId==3)
            //  cout << m_aSymmMats[iMatId-1] << endl;

        }

    }

    if (pBuffer == "MODRES")
    {
	vector<string> oModResidue;
	oModResidue.push_back( (oBuffer.substr( 7, 4)).c_str() );//0 - idcode
	oModResidue.push_back( (oBuffer.substr(12, 3)).c_str() );//1 - residue name
	oModResidue.push_back( (oBuffer.substr(16, 2)).c_str() );//2 - chainid
	oModResidue.push_back( (oBuffer.substr(18, 4)).c_str() );//3 - sequence number
	oModResidue.push_back( (oBuffer.substr(22, 2)).c_str() );//4 -
	oModResidue.push_back( (oBuffer.substr(24, 3)).c_str() );//5 - standard residues name
	oModResidue.push_back( (oBuffer.substr(29, 41)).c_str() );

	m_aModResidues.push_back(oModResidue);
    }

    if (pBuffer =="HELIX ")
    {
        svt_sse_pdb oHelix;

        strcpy(oHelix.m_aType,  (oBuffer.substr( 0, 6)).c_str());

        oHelix.m_iNum = atoi((oBuffer.substr( 7, 3)).c_str()); 
        strcpy(oHelix.m_aID,  (oBuffer.substr(11 , 3)).c_str());

        strcpy(oHelix.m_aInitialResname,  (oBuffer.substr( 15, 3)).c_str());
        oHelix.m_cInitialResChainID = (oBuffer.substr( 19, 1)).c_str()[0];
        oHelix.m_iInitialResSeq = atoi( (oBuffer.substr( 21, 4)).c_str()  );
        oHelix.m_cInitialICode = (oBuffer.substr( 25, 1)).c_str()[0];

        strcpy(oHelix.m_aTerminalResname,  (oBuffer.substr( 27, 3)).c_str());
        oHelix.m_cTerminalResChainID = (oBuffer.substr( 31, 1)).c_str()[0];
        oHelix.m_iTerminalResSeq = atoi( (oBuffer.substr( 33, 4)).c_str()  );
        oHelix.m_cTerminalICode = (oBuffer.substr( 37, 1)).c_str()[0];
            
        oHelix.m_iClass = atoi( (oBuffer.substr( 38, 2)).c_str()  );
        strcpy(oHelix.m_aComment,  (oBuffer.substr( 40, 30)).c_str());
        oHelix.m_iLen = atoi( (oBuffer.substr( 71, 5)).c_str()  );

        m_aSsePdb.push_back( oHelix );
    
        //also set the secStructureAvailable(true);
        setSecStructSource(SSE_PDB);
    }

    if (pBuffer =="SHEET ")
    {
        svt_sse_pdb oSheet;

        sprintf(oSheet.m_aType, "%s", (oBuffer.substr(0, 6)).c_str());

        oSheet.m_iNum = atoi((oBuffer.substr( 7, 3)).c_str()); 
        strcpy(oSheet.m_aID,  (oBuffer.substr(11 , 3)).c_str());

        oSheet.m_iNumStrands = atoi((oBuffer.substr( 14, 2)).c_str()); 

        strcpy(oSheet.m_aInitialResname,  (oBuffer.substr( 17, 3)).c_str());
        oSheet.m_cInitialResChainID = (oBuffer.substr(21 , 1)).c_str()[0];
        oSheet.m_iInitialResSeq = atoi( (oBuffer.substr( 22, 4)).c_str()  );
        oSheet.m_cInitialICode = (oBuffer.substr( 26, 1)).c_str()[0];

        strcpy(oSheet.m_aTerminalResname,  (oBuffer.substr( 28, 3)).c_str());
        oSheet.m_cTerminalResChainID = (oBuffer.substr( 32, 1)).c_str()[0];
        oSheet.m_iTerminalResSeq = atoi( (oBuffer.substr( 33, 4)).c_str()  );
        oSheet.m_cTerminalICode = (oBuffer.substr( 37, 1)).c_str()[0];
     
        oSheet.m_iSense = atoi((oBuffer.substr( 38, 2)).c_str()); 
        
        strcpy(oSheet.m_aCurAtom,  (oBuffer.substr( 41, 4)).c_str() );
        strcpy(oSheet.m_aCurResname,  (oBuffer.substr( 45, 3)).c_str());
        oSheet.m_cCurResChainID = (oBuffer.substr(49 , 1)).c_str()[0];
        strcpy(oSheet.m_aCurResSeq, (oBuffer.substr( 50, 4)).c_str()  );
        oSheet.m_cCurICode = (oBuffer.substr( 54, 1)).c_str()[0];  

        strcpy(oSheet.m_aPrevAtom,  (oBuffer.substr( 56, 4)).c_str() );
        strcpy(oSheet.m_aPrevResname,  (oBuffer.substr( 60, 3)).c_str());
        oSheet.m_cPrevResChainID = (oBuffer.substr(64 , 1)).c_str()[0];
        strcpy(oSheet.m_aPrevResSeq, (oBuffer.substr( 65, 4)).c_str()  );
        oSheet.m_cPrevICode = (oBuffer.substr( 69, 1)).c_str()[0];  

        m_aSsePdb.push_back( oSheet );
        
        //also set the secStructureAvailable(true);
        setSecStructSource(SSE_PDB);
    }


};


///////////////////////////////////////////////////////////////////////////////
// Data management functions
///////////////////////////////////////////////////////////////////////////////

/**
 * add atom
 */
template<class T>
void svt_point_cloud_pdb<T>::addAtom( svt_point_cloud_atom& rAtom, svt_vector4<Real64>& rVec)
{
    this->addPoint( rVec.x(), rVec.y(), rVec.z() );
    m_aAtoms.push_back( rAtom );
    if ( m_aAtoms[m_aAtoms.size()-1].getPDBIndex()==0)
    {
        m_aAtoms[m_aAtoms.size()-1].setPDBIndex( m_aAtoms.size() );
    }

    if (rAtom.isCA())
        m_iNumCAAtoms++;

    if (rAtom.isBackbone())
        m_iNumBBAtoms++;

};

/**
 * get atom with a special index (attention this is not the index in the pdb file, because this index is not unique in a lot of pdb files. svt_pdb simply renumbers all the atoms to get a unique index for each atom).
 * \param iIndex index of the atom you want to get
 * \return reference to the svt_atom object you want to get.
 */
template<class T>
svt_point_cloud_atom& svt_point_cloud_pdb<T>::atom( unsigned int i )
{
    return m_aAtoms[i];
};

/**
 * get atom with a special index (attention this is not the index in the pdb file, because this index is not unique in a lot of pdb files. svt_pdb simply renumbers all the atoms to get a unique index for each atom).
 * \param iIndex index of the atom you want to get
 * \return pointer to the svt_atom object you want to get.
 */
template<class T>
svt_point_cloud_atom* svt_point_cloud_pdb<T>::getAtom( unsigned int i )
{
    return &m_aAtoms[i];
};

/**
 * delete all atoms
 */
template<class T>
void svt_point_cloud_pdb<T>::deleteAllAtoms()
{
    this->delAllPoints();
    m_aAtoms.clear();
    m_aBonds.clear();
    m_aLinks.clear();

    m_aAtomNames.clear();
    m_aAtomResnames.clear();
    m_aAtomChainIDs.clear();
    m_aAtomSecStructureIDs.clear();
    m_aAtomSegmentIDs.clear();
    m_aAtomResSeqs.clear();
    m_aAtomModels.clear();

    m_aRemarks.clear();
    m_aModResidues.clear();

    m_iNumCAAtoms = 0;
    m_iNumBBAtoms = 0;

    m_oGraphDists.resize(0,0);
};

/**
 * add a bond
 * \param iAtomA index of atom A
 * \param iAtomB index of atom B
 */
template<class T>
void svt_point_cloud_pdb<T>::addBond( unsigned int iAtomA, unsigned int iAtomB )
{
    // is there already a bond?
    vector< unsigned int > aList = m_aAtoms[iAtomA].getBondList();
    for(unsigned int i=0; i<aList.size(); i++)
        if ( (m_aBonds.size() > aList[i] && ( m_aBonds[aList[i]].getIndexB() == (int)(iAtomB) || m_aBonds[aList[i]].getIndexA() == (int)(iAtomB)) ))
        {
            svtout << "DEBUG: addBond called, but there was already a bond in place?!" << endl;
            svtout << iAtomA << "\t" << iAtomB << "\t" << aList.size() << "\t" << aList[i] << "\t" << m_aBonds[aList[i]].getIndexB() << "\t" << m_aBonds[aList[i]].getIndexA()  << endl;
            //return;
        }

    m_aBonds.push_back( svt_point_cloud_bond( &m_aAtoms[iAtomA], &m_aAtoms[iAtomB], iAtomA, iAtomB ) );
    m_aAtoms[iAtomA].addToBondList( &m_aBonds.back(), m_aBonds.size()-1 );
    m_bPSFRead = true;
};
/**
 * is there a bond between two atoms?
 * \param iAtomA index of atom A
 * \param iAtomB index of atom B
 */
template<class T>
bool svt_point_cloud_pdb<T>::isBond( unsigned int iAtomA, unsigned int iAtomB )
{
    vector< unsigned int > aList = m_aAtoms[iAtomA].getBondList();

    if (aList.size() == 0)
        return false;

    for(unsigned int i=0; i<aList.size(); i++)
        if (m_aBonds[aList[i]].getIndexB() == (int)(iAtomB) || m_aBonds[aList[i]].getIndexA() == (int)(iAtomB))
            return true;

    return false;
};
/**
 * remove bond between two atoms?
 * \param iAtomA index of atom A
 * \param iAtomB index of atom B
 */
template<class T>
void svt_point_cloud_pdb<T>::delBond( unsigned int iAtomA, unsigned int iAtomB )
{
    vector< unsigned int > aList = m_aAtoms[iAtomA].getBondList();

    if (aList.size() == 0)
    {
        svtout << "DEBUG: delBond was called, but there are no bonds there?!" << endl;
        return;
    }
    //for(unsigned int i=0; i<aList.size(); i++)
    //    svtout << "DEBUG: CV: " << iAtomA << " has a bond: " << m_aBonds[aList[i]].getIndexA() << " - " << m_aBonds[aList[i]].getIndexB() << endl;

    for(unsigned int i=0; i<aList.size(); i++)
    {
        if (m_aBonds[aList[i]].getIndexB() == (int)(iAtomB) || m_aBonds[aList[i]].getIndexA() == (int)(iAtomB))
        {
            m_aAtoms[iAtomA].delFromBondList( aList[i] );

            //vector< unsigned int > aNewList = m_aAtoms[iAtomA].getBondList();
            //for(unsigned int j=0; j<aNewList.size(); j++)
            //    svtout << "DEBUG: After CV: " << iAtomA << " has a bond: " << m_aBonds[aNewList[j]].getIndexA() << " - " << m_aBonds[aNewList[j]].getIndexB() << endl;

            // now correct the other bonds
            for(unsigned int j=0;j<m_aAtoms.size(); j++)
                m_aAtoms[j].adjustBondList(aList[i]);

            m_aBonds.erase( m_aBonds.begin() + aList[i]);

            return;
        }
    }

    svtout << "DEBUG: delBond was called, but I cannot find that bond?!" << endl;
    return;
};

/**
 * find the bond object connecting two atoms
 * \param pA pointer to atom a
 * \param pB pointer to atom b
 * \return pointer to svt_point_cloud_bond object. If there is no such bond between these atoms, the return value will be NULL
 */
template<class T>
svt_point_cloud_bond* svt_point_cloud_pdb<T>::findBond(svt_point_cloud_atom* pA, svt_point_cloud_atom* pB)
{
    for(unsigned int i=0;i<m_aBonds.size();i++)
	if ((m_aBonds[i].getAtomA() == pA && m_aBonds[i].getAtomB() == pB) || (m_aBonds[i].getAtomA() == pB && m_aBonds[i].getAtomB() == pA))
	    return &(m_aBonds[i]);

    return NULL;
};

/**
 * get number of bonds
 * \return number of bonds
 */
template<class T>
unsigned int svt_point_cloud_pdb<T>::getBondsNum()
{
    return m_aBonds.size();
}

/**
 * get bonds
 * \return pointer to array of bonds
 */
template<class T>
vector< svt_point_cloud_bond >& svt_point_cloud_pdb<T>::getBonds()
{
    return m_aBonds;
}

/**
 * delete all bonds
 */
template<class T>
void svt_point_cloud_pdb<T>::deleteAllBonds()
{
    m_aBonds.clear();

    unsigned int iAtomsNum = m_aAtoms.size();
    for (unsigned int iIndex=0; iIndex<iAtomsNum; iIndex++)
    {
        //printf("Atom: %d delete %d bonds\n", iIndex, m_aAtoms[iIndex].getBondList().size() );
        m_aAtoms[iIndex].delBondList();
    }
};

/**
 * add a link
 * \param iAtomA index of atom A
 * \param iAtomB index of atom B
 */
template<class T>
void svt_point_cloud_pdb<T>::addLink( int iAtomA, int iAtomB )
{
    svt_vector4<Real64> oVec = (*this)[iAtomB]-(*this)[iAtomA];
    svt_point_cloud_link oLink( &m_aAtoms[iAtomA], &m_aAtoms[iAtomB], iAtomA, iAtomB, new svt_vector4<Real64>(oVec));

    m_aLinks.push_back( oLink );
};

/**
 * reset links rotations
 */
template<class T>
void svt_point_cloud_pdb<T>::resetLinksRotation()
{

    for (unsigned int iIndex=0; iIndex < getLinksNum(); iIndex++)
    {
        m_aLinks[iIndex].resetTransformation();
    }

};


/**
 * rotate
 * \param iIndexLink the index of the link in the list
 * \param iIndexAxis = (1,2)=(Theta,phi)
 * \param fAngle the angle in rad
 */
template<class T>
void svt_point_cloud_pdb<T>::rotateLink( int iIndexLink, int iIndexAxis, Real64 fAngle )
{
    if (iIndexLink>=0 && iIndexLink< (int)getLinksNum() )
	m_aLinks[iIndexLink].rotate(iIndexAxis, fAngle );
    else
        svtout << "Out of Bounderies" << endl;
};


/**
 * get number of links
 * \return number of links
 */
template<class T>
unsigned int svt_point_cloud_pdb<T>::getLinksNum()
{
    return m_aLinks.size();
};

/**
 * get the Psi angles of the Link
 * \param iIndexLink the index of the link
 * \return the angle
 */
template<class T>
Real64 svt_point_cloud_pdb<T>::getLinkPsi( int iIndexLink)
{

    if (iIndexLink>=0 && iIndexLink< (int)getLinksNum() )
    {
        return m_aLinks[iIndexLink].getPsi();
    }
    else
    {
        svtout << "Out of Bounderies" << endl;
        return -1;
    }
};


/**
 * get the Phi angles of the Link
 * \param iIndexLink the index of the link
 * \return the angle
 */
template<class T>
Real64 svt_point_cloud_pdb<T>::getLinkPhi( int iIndexLink)
{

    if (iIndexLink>=0 && iIndexLink< (int)getLinksNum() )
    {
        return m_aLinks[iIndexLink].getPhi();
    }
    else
    {
        svtout << "Out of Bounderies" << endl;
        return -1;
    }
};

/**
 * get the theta angles of the Link
 * \param iIndexLink the index of the link
 * \return the angle
 */
template<class T>
Real64 svt_point_cloud_pdb<T>::getLinkTheta( int iIndexLink)
{

    if (iIndexLink>=0 && iIndexLink< (int)getLinksNum() )
    {
        return m_aLinks[iIndexLink].getTheta();
    }
    else
    {
        svtout << "Out of Bounderies" << endl;
        return -1;
    }
};

/**
 * get the lenght of the Link
 * \param iIndexLink the index of the link
 * \return the angle
 */
template<class T>
Real64 svt_point_cloud_pdb<T>::getLinkLen( int iIndexLink)
{

    if (iIndexLink>=0 && iIndexLink< (int)getLinksNum() )
    {
        return m_aLinks[iIndexLink].length();
    }
    else
    {
        svtout << "Out of Bounderies" << endl;
        return -1;
    }
};

/**
 * adjust the Length with
 *\param iIndexLink the index of the link in the list
 * \param fLen the length to be added (flen can be positive but also negative)
 */
template<class T>
void svt_point_cloud_pdb<T>::setLinkLength( int iIndexLink, Real64 fLen )
{
    if (iIndexLink>=0 && iIndexLink< (int)getLinksNum() )
    {
        m_aLinks[iIndexLink].setLength(fLen);
    }
    else
    {
        svtout << "Out of Bounderies" << endl;
    }
};

/**
 * delete all links
 */
template<class T>
void svt_point_cloud_pdb<T>::deleteAllLinks()
{
    m_aLinks.clear();
};

/**
 * Append another pdb structure
 * \param rOther reference to other pdb point cloud
 * \param iModel the model number of the other, appended pdb structure (default: 0)
 * \return reference to point_cloud_pdb object
 */
template<class T>
svt_point_cloud_pdb<T>& svt_point_cloud_pdb<T>::append( svt_point_cloud_pdb<T>& rOther, int iModel )
{
    unsigned int iSize = this->size();
    unsigned int iAtomStart = m_aAtoms.size();

    for(unsigned int i=0; i<rOther.size(); i++)
    {
	this->addAtom( rOther.atom(i), rOther[i] );
        if (iModel >=0)
            this->atom( iSize + i ).setModel( iModel );
    }

    for(unsigned int i=0; i<rOther.m_aBonds.size(); i++)
        addBond( rOther.m_aBonds[i].getIndexA()+iAtomStart, rOther.m_aBonds[i].getIndexB()+iAtomStart );

    return *this;
};

/**
 * Add a remark
 * \param oRemark the remark as a string
 */
template<class T>
void svt_point_cloud_pdb<T>::addRemark(string oRemark)
{
    m_aRemarks.push_back(oRemark);
}


///////////////////////////////////////////////////////////////////////////////
// Enumeration functions
///////////////////////////////////////////////////////////////////////////////

/**
 * reset atom enumeration functions. This functions set the atom enumeration index back to the first atom.
 */
template<class T>
void svt_point_cloud_pdb<T>::resetAtomEnum()
{
    m_iAtomEnum = 0;
}

/**
 * look for all atoms of a certain type. This function will return the next atom of this type, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetAtomEnum().
 * \param pType pointer to an char array with the name of the atom
 */
template<class T>
int svt_point_cloud_pdb<T>::enumAtomType(const char* pType)
{
    for(unsigned int i=m_iAtomEnum;i<m_aAtoms.size();i++)
        if (strcmp(pType, m_aAtoms[i].getName()) == 0)
        {
            m_iAtomEnum = i+1;
            return i;
        }

    return -1;
}

/**
 * look for all atoms of a certain residue name. This function will return the next atom with this residue name, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetAtomEnum().
 * \param pResname pointer to an char array with the residue name
 */
template<class T>
int svt_point_cloud_pdb<T>::enumAtomResname(const char* pResname)
{
    for(unsigned int i=m_iAtomEnum;i<m_aAtoms.size();i++)
        if (strcmp(pResname, m_aAtoms[i].getResname()) == 0)
        {
            m_iAtomEnum = i+1;
            return i;
        }

    return -1;
}

/**
 * look for all atoms with a certain chain id. This function will return the next atom of this type, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetAtomEnum().
 * \param pType pointer to an char array with the name of the atom
 */
template<class T>
int svt_point_cloud_pdb<T>::enumAtomChainID(char cChainID)
{
    for(unsigned int i=m_iAtomEnum;i<m_aAtoms.size();i++)
        if (m_aAtoms[i].getChainID() == cChainID)
        {
            m_iAtomEnum = i+1;
            return i;
        }

    return -1;
}

/**
 * look for all atoms with a certain segment id. This function will return the next atom of this type, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetAtomEnum().
 * \param pType pointer to an char array with the name of the atom
 */
template<class T>
int svt_point_cloud_pdb<T>::enumAtomSegmentID(const char* pSegmentID)
{
    for(unsigned int i=m_iAtomEnum;i<m_aAtoms.size();i++)
        if (strcmp(pSegmentID, m_aAtoms[i].getSegmentID()) == 0)
        {
            m_iAtomEnum = i+1;
            return i;
        }

    return -1;
}
/**
 * look for all atoms with a certain model number. This function will return the next atom of this type, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetAtomEnum().
 * \param int with the model number
 */
template<class T>
int svt_point_cloud_pdb<T>::enumAtomModel(unsigned int iModel)
{
    for(unsigned int i=m_iAtomEnum;i<m_aAtoms.size();i++)
        if (m_aAtoms[i].getModel() == iModel)
        {
            m_iAtomEnum = i+1;
            return i;
        }

    return -1;
};

/**
 * look for all atoms with a certain residue sequence number. This function will return the next atom of this type, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetAtomEnum().
 * \param pType pointer to an char array with the name of the atom
 */
template<class T>
int svt_point_cloud_pdb<T>::enumAtomResidueSeq(int iResidueSeq)
{
    for(unsigned int i=m_iAtomEnum;i<m_aAtoms.size();i++)
        if (m_aAtoms[i].getResidueSeq() == iResidueSeq)
        {
            m_iAtomEnum = i+1;
            return i;
        }

    return -1;
}

/**
 * look for all atoms. This function will return the next atom, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetAtomEnum().
 */
template<class T>
int svt_point_cloud_pdb<T>::enumAtomAll()
{
    if (m_iAtomEnum < m_aAtoms.size())
    {
        m_iAtomEnum++;
        return m_iAtomEnum-1;
    } else
        return -1;
}

/**
 * look for all atoms part of a nucleotide. This function will return the next atom, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetAtomEnum().
 */
template<class T>
int svt_point_cloud_pdb<T>::enumAtomNucleotide()
{
    for(unsigned int i=m_iAtomEnum;i<m_aAtoms.size();i++)
    {
        if (m_aAtoms[i].isNucleotide())
	{
	    m_iAtomEnum = i+1;
	    return (int) i;
	}
    }

    return -1;
}

/**
 * look for all HET atoms. This function will return the next atom, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetAtomEnum().
 */
template<class T>
int svt_point_cloud_pdb<T>::enumAtomHet()
{
    for(unsigned int i=m_iAtomEnum;i<m_aAtoms.size();i++)
    {
        if (m_aAtoms[i].getHetAtm())
	{
	    m_iAtomEnum = i+1;
	    return (int) i;
	}
    }

    return -1;
}

/**
 * look for all water atoms. This function will return the next atom, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetAtomEnum().
 */
template<class T>
int svt_point_cloud_pdb<T>::enumAtomWater()
{
    for(unsigned int i=m_iAtomEnum;i<m_aAtoms.size();i++)
    {
        if (m_aAtoms[i].isWater())
	{
	    m_iAtomEnum = i+1;
	    return (int) i;
	}
    }

    return -1;
}

/**
 * reset bond enumeration functions. This functions set the bond enumeration index back to the first bond.
 */
template<class T>
void svt_point_cloud_pdb<T>::resetBondEnum()
{
    m_iBondEnum = 0;
}

/**
 * look for all bonds. This function will return the next bond, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetBondEnum().
 */
template<class T>
int svt_point_cloud_pdb<T>::enumBondAll()
{
    if (m_iBondEnum < m_aBonds.size())
    {
        m_iBondEnum++;
        return m_iBondEnum-1;
    } else
        return -1;
}

/**
 * look for all bonds which connect a certain atom with other atoms. This function will return the next bond connecting this atom, starting by a enumeration index. This index will move on every time you use an enumeration function. You can reset the index with resetBondEnum().
 * \param pAtom pointer to an svt_point_cloud_atom object
 */
template<class T>
int svt_point_cloud_pdb<T>::enumBondAtom(svt_point_cloud_atom* pAtom)
{
    for(unsigned int i=m_iBondEnum;i<m_aBonds.size();i++)
        if (m_aBonds[i].getAtomA() == pAtom || m_aBonds[i].getAtomB() == pAtom)
        {
            m_iBondEnum = i+1;
            return i;
        }

    return -1;
}

///////////////////////////////////////////////////////////////////////////////
// Info functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Get an array with all the different atom types (names) in the pdb file
 * \return pointer to an vector with strings of all different atom types (names) used in the pdb. Dont delete this pointer! svt_pdb will take care of it.
 */
template<class T>
vector<const char*>& svt_point_cloud_pdb<T>::getAtomNames()
{
    return m_aAtomNames;
}
/**
 * Get an array with all the different residue names in the pdb file
 * \return pointer to an vector with strings of all different residue names used in the pdb. Dont delete this pointer! svt_pdb will take care of it.
 */
template<class T>
vector<const char*>& svt_point_cloud_pdb<T>::getAtomResnames()
{
    return m_aAtomResnames;
}
/**
 * Get an array with all the different chain id's in the pdb file
 * \return pointer to an vector with strings of all different residue names used in the pdb. Dont delete this pointer! svt_pdb will take care of it.
 */
template<class T>
vector<char>& svt_point_cloud_pdb<T>::getAtomChainIDs()
{
    return m_aAtomChainIDs;
}
/**
 * Get an array with all the different segment id's in the pdb file
 * \return pointer to an vector with strings of all different residue names used in the pdb. Dont delete this pointer! svt_pdb will take care of it.
 */
template<class T>
vector<const char*>& svt_point_cloud_pdb<T>::getAtomSegmentIDs()
{
    return m_aAtomSegmentIDs;
}
/**
 * Get an array with all the different residue sequence numbers in the pdb file
 * \return pointer to an vector with strings of all different residue names used in the pdb. Dont delete this pointer! svt_pdb will take care of it.
 */
template<class T>
vector<int>& svt_point_cloud_pdb<T>::getAtomResidueSeqs()
{
    return m_aAtomResSeqs;
}
/**
 * Get an array with all the different secondary structure ids in the pdb file
 * \return pointer to an vector with strings of all different secondary structure ids used in the pdb. Dont delete this pointer! svt_pdb will take care of it.
 */
template<class T>
vector<char>& svt_point_cloud_pdb<T>::getAtomSecStructIDs()
{
    return m_aAtomSecStructureIDs;
}
/**
 * get an array with all the different models
 * \return reference to vector with ints
 */
template<class T>
vector<unsigned int>& svt_point_cloud_pdb<T>::getAtomModels()
{
    return m_aAtomModels;
};

/**
 * get a copy of a certain model
 * \param iModel number of model
 */
template<class T>
svt_point_cloud_pdb<T>* svt_point_cloud_pdb<T>::getModel( unsigned int iModel )
{
    svt_point_cloud_pdb<T>* pNew = new svt_point_cloud_pdb<T>();

    for(unsigned int i=0; i<this->size(); i++)
    {
	if (this->atom(i).getModel() == iModel)
	    pNew->addAtom( this->atom(i), (*this)[i] );
    }

    return pNew;
};

/**
 * get the number of Carbon Alpha in the pdb
 * \return the number of CA in the pdb
 */
template<class T>
unsigned int svt_point_cloud_pdb<T>::getNumCAAtoms()
{
    return m_iNumCAAtoms;
};

/**
 * get the number of Backbone atoms in the pdb
 * \return the number of backbone atoms in the pdb
 */
template<class T>
unsigned int svt_point_cloud_pdb<T>::getNumBBAtoms()
{
    return m_iNumBBAtoms;
};

/**
 * Get the minimum x coord of all atoms
 * \return minimum x coord
 */
template<class T>
Real32 svt_point_cloud_pdb<T>::getMinXCoord()
{
    Real32 min_x = 1000000.0f;
    unsigned int i;
    for(i=0;i<m_aAtoms.size();i++)
        if ((*this)[i].x() < min_x)
            min_x = (*this)[i].x();

    return min_x;
}

/**
 * Get the minimum y coord of all atoms
 * \return minimum y coord
 */
template<class T>
Real32 svt_point_cloud_pdb<T>::getMinYCoord()
{
    Real32 min_y = 1000000.0f;
    unsigned int i;
    for(i=0;i<m_aAtoms.size();i++)
        if ((*this)[i].y() < min_y)
            min_y = (*this)[i].y();

    return min_y;
}

/**
 * Get the minimum z coord of all atoms
 * \return minimum z coord
 */
template<class T>
Real32 svt_point_cloud_pdb<T>::getMinZCoord()
{
    Real32 min_z = 1000000.0f;
    unsigned int i;
    for(i=0;i<m_aAtoms.size();i++)
        if ((*this)[i].z() < min_z)
            min_z = (*this)[i].z();

    return min_z;
}

/**
 * Get the maximum x coord of all atoms
 * \return maximum x coord
 */
template<class T>
Real32 svt_point_cloud_pdb<T>::getMaxXCoord()
{
    Real32 max_x = -1000000.0f;
    unsigned int i;
    for(i=0;i<m_aAtoms.size();i++)
        if ((*this)[i].x() > max_x)
            max_x = (*this)[i].x();

    return max_x;
}

/**
 * Get the maximum y coord of all atoms
 * \return maximum y coord
 */
template<class T>
Real32 svt_point_cloud_pdb<T>::getMaxYCoord()
{
    Real32 max_y = -1000000.0f;
    unsigned int i;
    for(i=0;i<m_aAtoms.size();i++)
        if ((*this)[i].y() > max_y)
            max_y = (*this)[i].y();

    return max_y;
}

/**
 * Get the maximum z coord of all atoms
 * \return maximum z coord
 */
template<class T>
Real32 svt_point_cloud_pdb<T>::getMaxZCoord()
{
    Real32 max_z = -1000000.0f;
    unsigned int i;
    for(i=0;i<m_aAtoms.size();i++)
        if ((*this)[i].z() > max_z)
            max_z = (*this)[i].z();

    return max_z;
}

/**
 * get the maximum bfactor of all atoms
 * \return maximum bfactor
 */
template<class T>
Real32 svt_point_cloud_pdb<T>::getMaxTempFact()
{
    Real32 fMax = -10000.0f;
    unsigned int i;
    for(i=0;i<m_aAtoms.size();i++)
        if (m_aAtoms[i].getTempFact() > fMax)
            fMax = m_aAtoms[i].getTempFact();

    return fMax;
}

/**
 * get the minimum bfactor of all atoms
 * \return minimum bfactor
 */
template<class T>
Real32 svt_point_cloud_pdb<T>::getMinTempFact()
{
    Real32 fMin = 10000.0f;
    unsigned int i;
    for(i=0;i<m_aAtoms.size();i++)
        if (m_aAtoms[i].getTempFact() > fMin)
            fMin = m_aAtoms[i].getTempFact();

    return fMin;
}

/**
 * indicates if the psf_file has been reads
 * \return a bool true(1) if the psf has been read; false(0)  in the other case
 */
template<class T>
bool svt_point_cloud_pdb<T>::getPSFRead()
{
    return m_bPSFRead;
}

/**
 * Get the distance in the graph between 2 atoms(codebook vectors)
 * \param iIndex1 an int representing the row
 * \param iIndex2 an int representing the column
 * \return a float - the distance (inside the graph) between the atom iIndex1, iIndex2
 */
template<class T>
Real64 svt_point_cloud_pdb<T>::getGraphDist(unsigned int iIndex1, unsigned int iIndex2){
    return m_oGraphDists[iIndex1][iIndex2];
}

/**
 * Get the pointer to the graph distance matrix
 * \return svt_matrix& object m_oGraphDists = distances (inside the graph) between any atoms
 */
template<class T>
svt_matrix<Real64>& svt_point_cloud_pdb<T>::getGraphDists()
{
    return m_oGraphDists;
}


/**
 * Calculate the sphericity of the pdb
 */
template<class T>
Real32 svt_point_cloud_pdb<T>::getSphericity()
{
    float cumx=0;
    float cumy=0;
    float cumz=0;
    float cum;
    int i;

    int iAtomNum = m_aAtoms.size();

    for (i=0;i<iAtomNum;++i)
    {
        cumx += (*this)[i].x()*1.0E10;
        cumy += (*this)[i].y()*1.0E10;
        cumz += (*this)[i].z()*1.0E10;
    }
    cumx /= (float)iAtomNum;
    cumy /= (float)iAtomNum;
    cumz /= (float)iAtomNum;
    cum=0;
    for (i=0;i<iAtomNum;++i) {
        cum += ((*this)[i].x()*1.0E10-cumx)*((*this)[i].x()*1.0E10-cumx)+((*this)[i].y()*1.0E10-cumy)*((*this)[i].y()*1.0E10-cumy)+((*this)[i].z()*1.0E10-cumz)*((*this)[i].z()*1.0E10-cumz);
    }
    cum /= (float)iAtomNum;
    cum = sqrt(cum);

    return iAtomNum/(0.60*cum*cum*cum);
};

/**
 * calculate all lists (atom types, residues, ...)
 */
template<class T>
void svt_point_cloud_pdb<T>::calcAllLists()
{
    calcAtomNames();
    calcAtomResnames();
    calcAtomChainIDs();
    calcAtomSegmentIDs();
    calcAtomResidueSeqs();
    calcAtomSecStructureIDs();
};

/**
 * calculate array with the different atom type
 */
template<class T>
void svt_point_cloud_pdb<T>::calcAtomNames()
{
    bool bAdd;
    unsigned int iL1, iL2, i, j, k;
    m_aAtomNames.clear();

    for(i=0;i<m_aAtoms.size();i++)
    {
        bAdd = true;

        // search list
        for(j=0;j<m_aAtomNames.size();j++)
        {
            iL1 = strlen(m_aAtoms[i].getName());
            iL2 = strlen(m_aAtomNames[j]);

            // already in list?
            if (iL1 == iL2)
            {
                for(k=0;k<iL1;k++)
                {
                    if (m_aAtoms[i].getName()[k] != m_aAtomNames[j][k])
                        break;
                    if (k == (iL1-1))
                        bAdd = false;
                }
            }
        }

        // the atom type is not in the list -> add it.
        if (bAdd)
            m_aAtomNames.push_back(m_aAtoms[i].getName());
    }
}

/**
 * calculate array with the different atom resnames
 */
template<class T>
void svt_point_cloud_pdb<T>::calcAtomResnames()
{
    bool bAdd;
    unsigned int iL1, iL2, i, j, k;
    m_aAtomResnames.clear();

    for(i=0;i<m_aAtoms.size();i++)
    {
        bAdd = true;

        // search list
        for(j=0;j<m_aAtomResnames.size();j++)
        {
            iL1 = strlen(m_aAtoms[i].getResname());
            iL2 = strlen(m_aAtomResnames[j]);

            // already in list?
            if (iL1 == iL2)
            {
                for(k=0;k<iL1;k++)
                {
                    if (m_aAtoms[i].getResname()[k] != m_aAtomResnames[j] [k])
                        break;
                    if (k == (iL1-1))
                        bAdd = false;
                }
            }
        }

        // the atom type is not in the list -> add it.
        if (bAdd)
            m_aAtomResnames.push_back(m_aAtoms[i].getResname());
    }
}

/**
 * calculate array with the different atom chain ids
 */
template<class T>
void svt_point_cloud_pdb<T>::calcAtomChainIDs()
{
    bool bAdd;
    unsigned int i, j;
    m_aAtomChainIDs.clear();

    for(i=0;i<m_aAtoms.size();i++)
    {
        bAdd = true;

        // search list
        for(j=0;j<m_aAtomChainIDs.size();j++)
            // already in list?
            if (m_aAtoms[i].getChainID() == m_aAtomChainIDs[j])
                bAdd = false;

        // the atom type is not in the list -> add it.
        if (bAdd)
            m_aAtomChainIDs.push_back(m_aAtoms[i].getChainID());
    }
}

/**
 * calculate array with the different atom secondary structure ids
 */
template<class T>
void svt_point_cloud_pdb<T>::calcAtomSecStructureIDs()
{
    bool bAdd;
    unsigned int i, j;
    m_aAtomSecStructureIDs.clear();

    for(i=0;i<m_aAtoms.size();i++)
    {
        bAdd = true;

        // search list
        for(j=0;j<m_aAtomSecStructureIDs.size();j++)
            // already in list?
            if (m_aAtoms[i].getSecStruct() == m_aAtomSecStructureIDs[j])
                bAdd = false;

        // the atom type is not in the list -> add it.
        if (bAdd)
            m_aAtomSecStructureIDs.push_back(m_aAtoms[i].getSecStruct());
    }
}

/**
 * calculate array with the different atom segment ids
 */
template<class T>
void svt_point_cloud_pdb<T>::calcAtomSegmentIDs()
{
    bool bAdd;
    unsigned int iL1, iL2, i, j, k;
    m_aAtomSegmentIDs.clear();

    for(i=0;i<m_aAtoms.size();i++)
    {
        bAdd = true;

        // search list
        for(j=0;j<m_aAtomSegmentIDs.size();j++)
        {
            iL1 = strlen(m_aAtoms[i].getSegmentID());
            iL2 = strlen(m_aAtomSegmentIDs[j]);

            // already in list?
            if (iL1 == iL2)
            {
                for(k=0;k<iL1;k++)
                {
                    if (m_aAtoms[i].getSegmentID()[k] != m_aAtomSegmentIDs[j] [k])
                        break;
                    if (k == (iL1-1))
                        bAdd = false;
                }
            }
        }

        // the atom type is not in the list -> add it.
        if (bAdd)
            m_aAtomSegmentIDs.push_back(m_aAtoms[i].getSegmentID());
    }
}

/**
 * calculate array with the different atom residue sequence numbers
 */
template<class T>
void svt_point_cloud_pdb<T>::calcAtomResidueSeqs()
{
    bool bAdd;
    unsigned int i, j;
    m_aAtomResSeqs.clear();

    for(i=0;i<m_aAtoms.size();i++)
    {
        bAdd = true;

        // search list
        for(j=0;j<m_aAtomResSeqs.size();j++)
            // already in list?
            if (m_aAtoms[i].getResidueSeq() == m_aAtomResSeqs[j])
                bAdd = false;

        // the atom type is not in the list -> add it.
        if (bAdd)
            m_aAtomResSeqs.push_back(m_aAtoms[i].getResidueSeq());
    }
}

/**
 * calculate array with the different atom model numbers (Attention: This function is NOT CALLED in calcAllLists as typically this array is automatically build during loadPDB. If a pdb is build by hand, call this function after assembly of the structure!).
 */
template<class T>
void svt_point_cloud_pdb<T>::calcAtomModels()
{
    m_aAtomModels.clear();

    for(unsigned int i=0;i<m_aAtoms.size();i++)
    {
	if (find( m_aAtomModels.begin(), m_aAtomModels.end(), m_aAtoms[i].getModel() ) == m_aAtomModels.end() )
            m_aAtomModels.push_back( m_aAtoms[i].getModel() );
    }

    sort( m_aAtomModels.begin(), m_aAtomModels.end() );
};

/**
 * set the coordinate to the first atom in the links list
 * \param oCoord the new coordinates
 */
template<class T>
void svt_point_cloud_pdb<T>::setLinksOrigin(svt_vector4<Real64> oCoord)
{
    if (getLinksNum()>0)
	(*this)[m_aLinks[0].getIndexA()] = oCoord;
    else
	(*this)[0] = oCoord;

}

/**
 * converts the cartesian coordinates into internal coordinates
 */
template<class T>
void svt_point_cloud_pdb<T>::updateCoords()
{
    svt_matrix4<Real64> oMat, oMatProd;
    oMatProd.loadIdentity();

    svt_vector4<Real64> oVec, oVecNew;
    if (m_aLinks.size()>0)
    {
        oVec =  (*this)[ m_aLinks[0].getIndexA() ];
        oMatProd.translate( oVec );
    }

    oVec = svt_vector4<Real64>(0.0f,0.0f,0.0f);
    for (unsigned int iIndex=0; iIndex < getLinksNum(); iIndex++)
    {
        oMat =  m_aLinks[iIndex].getTransformation();
	oMatProd = oMatProd*oMat;
        oVecNew = oMatProd*oVec;

        (*this)[ m_aLinks[ iIndex ].getIndexB() ] = oVecNew;
    }

};

/**
 * converts the cartesian coordinates into internal coordinates
 */
template<class T>
vector<Real64>& svt_point_cloud_pdb<T>::getInternalCoords()
{
    vector<Real64> oInternalCoords;
    svt_vector4<Real64> oVec;
    if (m_aLinks.size()>0)
        oVec =  (*this)[ m_aLinks[0].getIndexA() ];

    oInternalCoords.push_back( oVec.x() );
    oInternalCoords.push_back( oVec.y() );
    oInternalCoords.push_back( oVec.z() );

    for (unsigned int iIndex=0; iIndex < getLinksNum(); iIndex++)
    {
	oInternalCoords.push_back( m_aLinks[iIndex].length()    );
	oInternalCoords.push_back( m_aLinks[iIndex].getPhi()    );
	oInternalCoords.push_back( m_aLinks[iIndex].getTheta()  );
	oInternalCoords.push_back( m_aLinks[iIndex].getPsi()    );

    }

    return oInternalCoords;

}
///////////////////////////////////////////////////////////////////////////////
// Misc functions
///////////////////////////////////////////////////////////////////////////////

/**
 * should the atoms belonging to a water molecule be ignored in clustering this structure? Default: false.
 */
template<class T>
void svt_point_cloud_pdb<T>::setIgnoreWater( bool bIgnoreWater )
{
    m_bIgnoreWater = bIgnoreWater;
};

/**
 * shall we read HETATM records?
 * \param bHetAtm if true hetatm atoms are read (default: true)
 */
template<class T>
void svt_point_cloud_pdb<T>::setHetAtm( bool bHetAtm )
{
    m_bLoadHetAtm = bHetAtm;
};

/**
 * Get the maximum b-factor
 */
template<class T>
Real32 svt_point_cloud_pdb<T>::getMaxTempFact() const
{
    unsigned int i;
    unsigned int iNum = m_aAtoms.size();
    Real32 fMax = 0.0;

    for(i=0; i<iNum; i++)
	if (m_aAtoms[i].getTempFact() > fMax)
	    fMax = m_aAtoms[i].getTempFact();

    return fMax;
}

/**
 * Get the average b-factor
 */
template<class T>
Real32 svt_point_cloud_pdb<T>::getAvgTempFact() const
{
    unsigned int i;
    unsigned int iNum = m_aAtoms.size();
    Real32 fAvg = 0.0;

    for(i=0; i<iNum; i++)
	fAvg += m_aAtoms[i].getTempFact();

    return fAvg / (Real64)(iNum);
}

/**
 * sample the object randomly and return a vector that refrects the probability distribution of the object
 */
template<class T>
T svt_point_cloud_pdb<T>::sample( )
{
    unsigned int iCount = 0;
    unsigned int iIndex = 0;

    while(iCount++ < 1000000)
    {
	iIndex = (unsigned int)(svt_genrand() * (double)(m_aAtoms.size()));

	if (m_bIgnoreWater)
	{
	    if (!m_aAtoms[iIndex].isWater() && m_aAtoms[iIndex].getMass() > (Real64)(svt_genrand() * 55.0))
	    {
		break;
	    }
	} else {
	    if (m_aAtoms[iIndex].getMass() > (Real64)(svt_genrand() * 55.0))
		break;
	}
    }

    //iIndex = (unsigned int)(svt_genrand() * (double)(m_aAtoms.size()));
    return this->getPoint( iIndex );
};

/**
 * Project mass
 * \param fWidth voxel width of the target map
 */
template<class T>
svt_volume<Real64>* svt_point_cloud_pdb<T>::projectMass(Real64 fWidth, bool bCA)
{
    // bring lattice into register with origin
    T oMin = this->getMinCoord();
    T oMax = this->getMaxCoord();

    Real64 fMinx = fWidth * floor( oMin.x() / fWidth );
    Real64 fMaxx = fWidth * ceil( oMax.x() / fWidth );
    Real64 fMiny = fWidth * floor( oMin.y() / fWidth );
    Real64 fMaxy = fWidth * ceil( oMax.y() / fWidth );
    Real64 fMinz = fWidth * floor( oMin.z() / fWidth );
    Real64 fMaxz = fWidth * ceil( oMax.z() / fWidth );

    //svtout << "blur: minimal coords: " << oMin << endl;
    //svtout << "blur: maximal coords: " << oMax << endl;

    // allocate protein density map
    int iMargin = 0;
    int iExtx = (int)(ceil( (fMaxx - fMinx) / fWidth )) + (2*iMargin) + 1;
    int iExty = (int)(ceil( (fMaxy - fMiny) / fWidth )) + (2*iMargin) + 1;
    int iExtz = (int)(ceil( (fMaxz - fMinz) / fWidth )) + (2*iMargin) + 1;

    //svtout << "blur: target map size: " << iExtx << " x " << iExty << " x " << iExtz << endl;

    svt_volume<Real64>* pMap = new svt_volume<Real64>( iExtx, iExty, iExtz );
    pMap->setValue( 0.0 );
    pMap->setWidth( fWidth );

    //svtout << "blur: project mass" << endl;

    // interpolate structure to protein map and keep track of variability - i.e. create a volumetric map with peaks at the atomic positions...
    Real64 fVarp = 0.0;
    Real64 fGx, fGy, fGz, fA, fB, fC;
    int iX0, iY0, iZ0, iX1, iY1, iZ1;
    unsigned int i;
    unsigned int iAtomsNum = this->size();

    for (i=0; i<iAtomsNum; i++)
    {
	if (!bCA || this->getAtom(i)->isCA())
        {
	    // compute position within grid
	    fGx = iMargin + ((this->getPoint(i).x() - fMinx) / fWidth);
	    fGy = iMargin + ((this->getPoint(i).y() - fMiny) / fWidth);
	    fGz = iMargin + ((this->getPoint(i).z() - fMinz) / fWidth);

	    iX0 = (int)(floor(fGx));
	    iY0 = (int)(floor(fGy));
	    iZ0 = (int)(floor(fGz));
	    iX1 = iX0+1;
	    iY1 = iY0+1;
	    iZ1 = iZ0+1;

	    // interpolate
	    fA = (Real64)(iX1) - fGx;
	    fB = (Real64)(iY1) - fGy;
	    fC = (Real64)(iZ1) - fGz;

	    pMap->setValue(iX0, iY0, iZ0, pMap->getValue(iX0, iY0, iZ0) + fA * fB * fC                 * m_aAtoms[i].getMass()); fVarp += fA * fB * fC * ((1.0-fA)*(1.0-fA)+(1.0-fB)*(1.0-fB)+(1.0-fC)*(1.0-fC));
	    pMap->setValue(iX0, iY0, iZ1, pMap->getValue(iX0, iY0, iZ1) + fA * fB * (1.0-fC)           * m_aAtoms[i].getMass()); fVarp += fA * fB * (1.0-fC) * ((1.0-fA)*(1.0-fA)+(1.0-fB)*(1.0-fB)+fC*fC);
	    pMap->setValue(iX0, iY1, iZ0, pMap->getValue(iX0, iY1, iZ0) + fA * (1-fB) * fC             * m_aAtoms[i].getMass()); fVarp += fA * (1.0-fB) * fC * ((1.0-fA)*(1.0-fA)+fB*fB+(1.0-fC)*(1.0-fC));
	    pMap->setValue(iX1, iY0, iZ0, pMap->getValue(iX1, iY0, iZ0) + (1.0-fA) * fB * fC           * m_aAtoms[i].getMass()); fVarp += (1.0-fA) * fB * fC * (fA*fA+(1.0-fB)*(1.0-fB)+(1.0-fC)*(1.0-fC));
	    pMap->setValue(iX0, iY1, iZ1, pMap->getValue(iX0, iY1, iZ1) + fA * (1-fB) * (1.0-fC)       * m_aAtoms[i].getMass()); fVarp += fA * (1.0-fB) * (1.0-fC) * ((1.0-fA)*(1.0-fA)+fB*fB+fC*fC);
	    pMap->setValue(iX1, iY1, iZ0, pMap->getValue(iX1, iY1, iZ0) + (1.0-fA) * (1-fB) * fC       * m_aAtoms[i].getMass()); fVarp += (1.0-fA) * (1.0-fB) * fC * (fA*fA+fB*fB+(1.0-fC)*(1.0-fC));
	    pMap->setValue(iX1, iY0, iZ1, pMap->getValue(iX1, iY0, iZ1) + (1.0-fA) * fB * (1.0-fC)     * m_aAtoms[i].getMass()); fVarp += (1.0-fA) * fB * (1.0-fC) * (fA*fA+(1.0-fB)*(1.0-fB)+fC*fC);
	    pMap->setValue(iX1, iY1, iZ1, pMap->getValue(iX1, iY1, iZ1) + (1.0-fA) * (1-fB) * (1.0-fC) * m_aAtoms[i].getMass()); fVarp += (1.0-fA) * (1.0-fB) * (1.0-fC) * (fA*fA+fB*fB+fC*fC);
	}
    }
    fVarp /= (double)(iAtomsNum);

    //svtout << "blur: variability: " << fVarp << endl;
    //svtout << "blur: lattice smoothing (sigma = atom rmsd): " << fWidth * sqrt(fVarp) << " Angstroem" << endl;
    //svtout << "blur: convolve with kernel" << endl;
    //svtout << "blur: done." << endl;

    // set correct position of map relative to pdb
    pMap->setGrid( fMinx - ((Real64)(iMargin)*fWidth), fMiny - ((Real64)(iMargin)*fWidth), fMinz - ((Real64)(iMargin)*fWidth) );

    // return
    return pMap;
};

/**
 * Project mass
 * \param pMap pointer to map that will hold projected structure
 * \param pTransformation pointer to transformation to apply
 */
template<class T>
void svt_point_cloud_pdb<T>::projectMass(svt_volume<Real64>* pMap, svt_matrix4<Real64> oTransformation, bool bCA, int iAtoms)
{

    // get properties of map
    Real64 fWidth = pMap->getWidth();
    Real64 fMinx  = pMap->getGridX();
    Real64 fMiny  = pMap->getGridY();
    Real64 fMinz  = pMap->getGridZ();

    // interpolate structure to protein map and keep track of variability - i.e. create a volumetric map with peaks at the atomic positions...
    Real64 fGx, fGy, fGz, fA, fB, fC;
    int iX0, iY0, iZ0, iX1, iY1, iZ1;
    unsigned int i;
    unsigned int iAtomsNum;
    if (iAtoms == -1)// default: use all atoms 
	iAtomsNum = this->size();
    else 
	iAtomsNum = iAtoms;
    svt_vector4<Real64> oCurrPoint;
    Real64 fXa, fYa, fZa;

    for (i=0; i<iAtomsNum; i++)
    {
	if (!bCA || this->getAtom(i)->isCA())
        {
	    // Apply transform
	    oCurrPoint = this->getPoint(i);

	    //replaced oCurrPoint *= oTransformation; to improve time efficiency with the following 6 lines
	    fXa =  oTransformation[0][0]*oCurrPoint.x() + oTransformation[0][1]*oCurrPoint.y() + oTransformation[0][2]*oCurrPoint.z()+oTransformation[0][3]*oCurrPoint.w();
	    fYa =  oTransformation[1][0]*oCurrPoint.x() + oTransformation[1][1]*oCurrPoint.y() + oTransformation[1][2]*oCurrPoint.z()+oTransformation[1][3]*oCurrPoint.w();
	    fZa =  oTransformation[2][0]*oCurrPoint.x() + oTransformation[2][1]*oCurrPoint.y() + oTransformation[2][2]*oCurrPoint.z()+oTransformation[2][3]*oCurrPoint.w();

	    oCurrPoint.x( fXa );
	    oCurrPoint.y( fYa );
	    oCurrPoint.z( fZa );

	    // compute position within grid
	    fGx = ((oCurrPoint.x() - fMinx) / fWidth);
	    fGy = ((oCurrPoint.y() - fMiny) / fWidth);
	    fGz = ((oCurrPoint.z() - fMinz) / fWidth);

	    iX0 = (int)(floor(fGx));
	    iY0 = (int)(floor(fGy));
	    iZ0 = (int)(floor(fGz));
	    iX1 = iX0+1;
	    iY1 = iY0+1;
	    iZ1 = iZ0+1;

	    // interpolate
	    fA = (Real64)(iX1) - fGx;
	    fB = (Real64)(iY1) - fGy;
	    fC = (Real64)(iZ1) - fGz;

	    pMap->setValue(iX0, iY0, iZ0, pMap->getValue(iX0, iY0, iZ0) + fA * fB * fC                 * m_aAtoms[i].getMass());
	    pMap->setValue(iX0, iY0, iZ1, pMap->getValue(iX0, iY0, iZ1) + fA * fB * (1.0-fC)           * m_aAtoms[i].getMass());
	    pMap->setValue(iX0, iY1, iZ0, pMap->getValue(iX0, iY1, iZ0) + fA * (1-fB) * fC             * m_aAtoms[i].getMass());
	    pMap->setValue(iX1, iY0, iZ0, pMap->getValue(iX1, iY0, iZ0) + (1.0-fA) * fB * fC           * m_aAtoms[i].getMass());
	    pMap->setValue(iX0, iY1, iZ1, pMap->getValue(iX0, iY1, iZ1) + fA * (1-fB) * (1.0-fC)       * m_aAtoms[i].getMass());
	    pMap->setValue(iX1, iY1, iZ0, pMap->getValue(iX1, iY1, iZ0) + (1.0-fA) * (1-fB) * fC       * m_aAtoms[i].getMass());
	    pMap->setValue(iX1, iY0, iZ1, pMap->getValue(iX1, iY0, iZ1) + (1.0-fA) * fB * (1.0-fC)     * m_aAtoms[i].getMass());
	    pMap->setValue(iX1, iY1, iZ1, pMap->getValue(iX1, iY1, iZ1) + (1.0-fA) * (1-fB) * (1.0-fC) * m_aAtoms[i].getMass());
	}
    }

    // return
    return;
};

/**
 * Project distance
 * \param pMap pointer to map that will hold projected distances
 * \param pTransformation pointer to transformation to apply
 */
template<class T>
void svt_point_cloud_pdb<T>::projectDistance(svt_volume<Real64>* pMap, svt_matrix4<Real64> oTransformation,
                                             bool bBackbone)
{

    // set the maximum distance we want to consider
    const Real64 cMaxExtent = 15.0;

    // get properties of map
    Real64 fWidth = pMap->getWidth();
    Real64 fMinx  = pMap->getGridX();
    Real64 fMiny  = pMap->getGridY();
    Real64 fMinz  = pMap->getGridZ();

    // prepare distance kernel
    int iKernelDim    = 2*((int) ceil( 2.0 * cMaxExtent / fWidth ))/2 + 1;
    int iKernelCenter = iKernelDim/2;

    svt_volume<Real64>* pDistanceKernel = new svt_volume<Real64>(iKernelDim, iKernelDim, iKernelDim);

    {
        int    i = 0;
	Real64 x, y, z;
	Real64 distance;

	for (int m=0; m<iKernelDim; m++)
	  for (int l=0; l<iKernelDim; l++)
	    for (int k=0; k<iKernelDim; k++) {

		x = ((Real64) (k - iKernelCenter)) * fWidth;
		y = ((Real64) (l - iKernelCenter)) * fWidth;
		z = ((Real64) (m - iKernelCenter)) * fWidth;

		distance = sqrt(x*x + y*y + z*z);

		// set the central voxel to -100 so we can distinguish voxels
		// which were already examined
		if ( (m == iKernelCenter) && (l == iKernelCenter) && (k == iKernelCenter) )
		    distance = -100.0;

		pDistanceKernel->setAt(i, distance);
		i++;
	    }
    }

    // Generate local distance information for all atoms
    Real64 fGx, fGy, fGz;
    int iX, iY, iZ;
    unsigned int i;
    unsigned int iAtomsNum = this->size();
    svt_vector4<Real64> oCurrPoint;

    for (i=0; i<iAtomsNum; i++)
    {

        // Check for backbone
	if (bBackbone && !this->getAtom(i)->isBackbone()) continue;

        // Apply transform
	oCurrPoint = this->getPoint(i);
	oCurrPoint *= oTransformation;

	// compute position within grid
	fGx = ((oCurrPoint.x() - fMinx) / fWidth);
	fGy = ((oCurrPoint.y() - fMiny) / fWidth);
	fGz = ((oCurrPoint.z() - fMinz) / fWidth);

	// ROUND to nearest index
	iX = (int)(nearbyint(fGx));
	iY = (int)(nearbyint(fGy));
	iZ = (int)(nearbyint(fGz));

	// Check if we already did this voxel
	if (pMap->getValue(iX, iY, iZ) < 0.0) continue;

	// Loop over distance kernel
	{
	    int i = 0;
	    int iKernelX, iKernelY, iKernelZ;

	    for (int m=0; m<iKernelDim; m++)
	      for (int l=0; l<iKernelDim; l++)
		for (int k=0; k<iKernelDim; k++) {

		    iKernelX = (k - iKernelCenter);
		    iKernelY = (l - iKernelCenter);
		    iKernelZ = (m - iKernelCenter);

		    // Only replace if value is zero or larger than distance from kernel
		    if ((pMap->getValue(iX+iKernelX, iY+iKernelY, iZ+iKernelZ) == 0.0) ||
		        (pMap->getValue(iX+iKernelX, iY+iKernelY, iZ+iKernelZ) > pDistanceKernel->getValue(i)))
			pMap->setValue(iX+iKernelX, iY+iKernelY, iZ+iKernelZ, pDistanceKernel->getValue(i));

		    i++;
		}
	}

    }

    // cleanup
    delete pDistanceKernel;

    // return
    return;
};

/**
 * Project mass and correlation - no new volume will get created, the correlation is calculated on the fly
 * \param pMap pointer to map that holds the target map
 * \param pTransformation pointer to transformation to apply
 * \param bCA if true, only the CA atoms are evaluated
 */
template<class T>
Real64 svt_point_cloud_pdb<T>::projectMassCorr(svt_volume<Real64>* pMap, svt_matrix4<Real64> oTransformation, bool bCA)
{
    // get properties of map
    Real64 fWidth = pMap->getWidth();
    Real64 fMinx  = pMap->getGridX();
    Real64 fMiny  = pMap->getGridY();
    Real64 fMinz  = pMap->getGridZ();

    Real64 fCorr = 0.0;
    Real64 fGx, fGy, fGz, fA, fB, fC;
    int iX0, iY0, iZ0, iX1, iY1, iZ1;
    unsigned int i;
    unsigned int iAtomsNum = this->size();
    svt_vector4<Real64> oCurrPoint;
    Real64 fMass;
    Real64 fNormB = 0.0;

    Real64* pMapData = pMap->getData();
    unsigned int iSizeX = pMap->getSizeX();
    unsigned int iSizeY = pMap->getSizeY();
    unsigned int iSizeZ = pMap->getSizeZ();
    unsigned int iSizeXY = iSizeX*iSizeY;
    int iY0S, iY1S, iZ0S, iZ1S;
    Real64 fXa, fYa, fZa;

    for (i=0; i<iAtomsNum; i++)
    {
        if (!bCA || this->getAtom(i)->isCA())
        {
            // Apply transform
            oCurrPoint = this->getPoint(i);
	    //replaced oCurrPoint *= oTransformation; to improve time efficiency with the following 6 lines
            fXa =  oTransformation[0][0]*oCurrPoint.x() + oTransformation[0][1]*oCurrPoint.y() + oTransformation[0][2]*oCurrPoint.z()+oTransformation[0][3]*oCurrPoint.w();
	    fYa =  oTransformation[1][0]*oCurrPoint.x() + oTransformation[1][1]*oCurrPoint.y() + oTransformation[1][2]*oCurrPoint.z()+oTransformation[1][3]*oCurrPoint.w();
	    fZa =  oTransformation[2][0]*oCurrPoint.x() + oTransformation[2][1]*oCurrPoint.y() + oTransformation[2][2]*oCurrPoint.z()+oTransformation[2][3]*oCurrPoint.w();

	    oCurrPoint.x( fXa );
	    oCurrPoint.y( fYa );
	    oCurrPoint.z( fZa );
	    //oCurrPoint *= oTransformation;

            // compute position within grid
            fGx = ((oCurrPoint.x() - fMinx) / fWidth);
            fGy = ((oCurrPoint.y() - fMiny) / fWidth);
            fGz = ((oCurrPoint.z() - fMinz) / fWidth);

            iX0 = (int)(floor(fGx));
            iY0 = (int)(floor(fGy));
            iZ0 = (int)(floor(fGz));
            iX1 = iX0+1;
            iY1 = iY0+1;
            iZ1 = iZ0+1;
                    
            fMass = m_aAtoms[i].getMass();

            if (iX0 >=0 && iX0 < (int)iSizeX && iX1 >=0 && iX1 < (int)iSizeX  && 
                iY0 >=0 && iY0 < (int)iSizeY && iY1 >=0 && iY1 < (int)iSizeY  && 
                iZ0 >=0 && iZ0 < (int)iSizeZ && iZ1 >=0 && iZ1 < (int)iSizeZ) 
            {

                // interpolate
                fA = (Real64)(iX1) - fGx;
                fB = (Real64)(iY1) - fGy;
                fC = (Real64)(iZ1) - fGz;

                iY0S = iY0*iSizeX;
                iY1S = iY1*iSizeX;
                iZ0S = iZ0*iSizeXY;
                iZ1S = iZ1*iSizeXY;

                fCorr += pMapData[ iX0 + iY0S + iZ0S ] * (fA        * fB        * fC        * fMass);
                fCorr += pMapData[ iX0 + iY0S + iZ1S ] * (fA        * fB        * (1.0-fC)  * fMass);
                fCorr += pMapData[ iX0 + iY1S + iZ0S ] * (fA        * (1.0-fB)  * fC        * fMass);
                fCorr += pMapData[ iX1 + iY0S + iZ0S ] * ((1.0-fA)  * fB        * fC        * fMass);
                fCorr += pMapData[ iX0 + iY1S + iZ1S ] * (fA        * (1.0-fB)  * (1.0-fC)  * fMass);
                fCorr += pMapData[ iX1 + iY1S + iZ0S ] * ((1.0-fA)  * (1.0-fB)  * fC        * fMass);
                fCorr += pMapData[ iX1 + iY0S + iZ1S ] * ((1.0-fA)  * fB        * (1.0-fC)  * fMass);
                fCorr += pMapData[ iX1 + iY1S + iZ1S ] * ((1.0-fA)  * (1.0-fB)  * (1.0-fC)  * fMass);

                fNormB += fMass * fMass;
            }
            else
            {
                fNormB += fMass * fMass;
            }
        }
    }

    Real64 fNormA = pMap->getNorm();
    fCorr /= (fNormA * fNormB);

    // return
    return fCorr;
};

/**
 * Blur and correlation - no new volume will get created, the correlation is calculated on the fly
 * \param pMap pointer to map that holds the target map
 * \param pTransformation pointer to transformation to apply
 * \param bCA if true, only the CA atoms are evaluated
 */
template<class T>
Real64 svt_point_cloud_pdb<T>::blurCorr( svt_volume<Real64>* pKernel, svt_volume<Real64>* pMap, svt_matrix4<Real64> oTransformation, bool bCA)
{
    // get properties of map
    Real64 fWidth = pMap->getWidth();
    Real64 fMinx  = pMap->getGridX();
    Real64 fMiny  = pMap->getGridY();
    Real64 fMinz  = pMap->getGridZ();

    Real64 fCorr = 0.0;
    unsigned int i;
    unsigned int iAtomsNum = this->size();
    T oCurrPoint;

    unsigned int iSize = pKernel->getSizeX();
    unsigned int iSizeS = iSize * iSize;
    Real64 fMass;
    int iKX, iKY, iKZ;
    int iDim = (int)((Real32)(pKernel->getSizeX()) * 0.5f);
    int iStart = -iDim;
    int iEnd = iDim+1;
    Real64 fGx, fGy, fGz, fA, fB, fC;
    int iX0, iY0, iZ0, iX1, iY1, iZ1;
    Real64 fKernel;
    Real64 fMassABC, fMassAB1C, fMassA1BC, fMass1ABC, fMassA1B1C, fMass1A1BC, fMass1AB1C, fMass1A1B1C;
    int iY0KY, iY1KY, iZ0KZ, iZ1KZ, iRest;

    Real64* pKernelData = pKernel->getData();
    Real64* pMapData = pMap->getData();

    int iSizeX = pMap->getSizeX();
    int iSizeY = pMap->getSizeY();
    int iSizeZ = pMap->getSizeZ();
    int iSizeXY = iSizeX*iSizeY;

    int iLBd = iDim;
    int iUBdX = (int)(iSizeX) - iDim - 2;
    int iUBdY = (int)(iSizeY) - iDim - 2;
    int iUBdZ = (int)(iSizeZ) - iDim - 2;

    vector< T >& aPoints = this->getPoints();

    for (i=0; i<iAtomsNum; i++)
    {
        if (!bCA || this->getAtom(i)->isCA())
        {
            // Apply transform
            oCurrPoint = aPoints[i];
            oCurrPoint *= oTransformation;

            // compute position within grid
            fGx = ((oCurrPoint.x() - fMinx) / fWidth);
            fGy = ((oCurrPoint.y() - fMiny) / fWidth);
            fGz = ((oCurrPoint.z() - fMinz) / fWidth);

            iX0 = (int)(fGx);
            iY0 = (int)(fGy);
            iZ0 = (int)(fGz);
            iX1 = iX0+1;
            iY1 = iY0+1;
            iZ1 = iZ0+1;

            // interpolate
            fA = (Real64)(iX1) - fGx;
            fB = (Real64)(iY1) - fGy;
            fC = (Real64)(iZ1) - fGz;

            fMass = m_aAtoms[i].getMass();

            fMassABC    = fA * fB * fC                   * fMass;
            fMassAB1C   = fA * fB * (1.0-fC)             * fMass;
            fMassA1BC   = fA * (1.0-fB) * fC             * fMass;
            fMass1ABC   = (1.0-fA) * fB * fC             * fMass;
            fMassA1B1C  = fA * (1.0-fB) * (1.0-fC)       * fMass;
            fMass1A1BC  = (1.0-fA) * (1.0-fB) * fC       * fMass;
            fMass1AB1C  = (1.0-fA) * fB * (1.0-fC)       * fMass;
            fMass1A1B1C = (1.0-fA) * (1.0-fB) * (1.0-fC) * fMass;

            if (! (iX0 < iLBd || iX0 > iUBdX || iY0 < iLBd || iY0 > iUBdY || iZ0 < iLBd || iZ0 > iUBdZ ) )
            {
                for(iKZ = iStart; iKZ < iEnd; iKZ++)
                    for(iKY = iStart; iKY < iEnd; iKY++)
                    {
                        iY0KY = (iY0+iKY)*iSizeX;
                        iY1KY = (iY1+iKY)*iSizeX;
                        iZ0KZ = (iZ0+iKZ)*iSizeXY;
                        iZ1KZ = (iZ1+iKZ)*iSizeXY;

                        iRest = ((iKY-iStart)*iSize)+iKZ-iStart;

                        for(iKX = iStart; iKX < iEnd; iKX++)
                        {
                            fKernel = pKernelData[((iKX-iStart)*iSizeS)+iRest];

                            fCorr += pMapData[(iX0+iKX) + iY0KY + iZ0KZ] * fKernel * fMassABC;
                            fCorr += pMapData[(iX0+iKX) + iY0KY + iZ1KZ] * fKernel * fMassAB1C;
                            fCorr += pMapData[(iX0+iKX) + iY1KY + iZ0KZ] * fKernel * fMassA1BC;
                            fCorr += pMapData[(iX1+iKX) + iY0KY + iZ0KZ] * fKernel * fMass1ABC;
                            fCorr += pMapData[(iX0+iKX) + iY1KY + iZ1KZ] * fKernel * fMassA1B1C;
                            fCorr += pMapData[(iX1+iKX) + iY1KY + iZ0KZ] * fKernel * fMass1A1BC;
                            fCorr += pMapData[(iX1+iKX) + iY0KY + iZ1KZ] * fKernel * fMass1AB1C;
                            fCorr += pMapData[(iX1+iKX) + iY1KY + iZ1KZ] * fKernel * fMass1A1B1C;
                        }
                    }

            } else {

                for(iKZ = iStart; iKZ < iEnd; iKZ++)
                    for(iKY = iStart; iKY < iEnd; iKY++)
                    {
                        iY0KY = (iY0+iKY)*iSizeX;
                        iY1KY = (iY1+iKY)*iSizeX;
                        iZ0KZ = (iZ0+iKZ)*iSizeXY;
                        iZ1KZ = (iZ1+iKZ)*iSizeXY;

                        iRest = ((iKY-iStart)*iSize)+iKZ-iStart;

                        for(iKX = iStart; iKX < iEnd; iKX++)
                        {
                            fKernel = pKernelData[((iKX-iStart)*iSizeS)+iRest];

                            fCorr += pMap->getValue((iX0+iKX) + iY0KY + iZ0KZ) * fKernel * fMassABC;
                            fCorr += pMap->getValue((iX0+iKX) + iY0KY + iZ1KZ) * fKernel * fMassAB1C;
                            fCorr += pMap->getValue((iX0+iKX) + iY1KY + iZ0KZ) * fKernel * fMassA1BC;
                            fCorr += pMap->getValue((iX1+iKX) + iY0KY + iZ0KZ) * fKernel * fMass1ABC;
                            fCorr += pMap->getValue((iX0+iKX) + iY1KY + iZ1KZ) * fKernel * fMassA1B1C;
                            fCorr += pMap->getValue((iX1+iKX) + iY1KY + iZ0KZ) * fKernel * fMass1A1BC;
                            fCorr += pMap->getValue((iX1+iKX) + iY0KY + iZ1KZ) * fKernel * fMass1AB1C;
                            fCorr += pMap->getValue((iX1+iKX) + iY1KY + iZ1KZ) * fKernel * fMass1A1B1C;
                        }
                    }

            }
        }
    }

    Real64 fNormA = pMap->getNorm();
    Real64 fNormB = pKernel->getNorm() * iAtomsNum;

    fCorr /= fNormA * fNormB;

    // return
    return fCorr;
};

/**
 * blur the pdb structure and thereby create an artificial low-resolution map
 * \param fWidth voxel width of the target map
 * \param fResolution resolution of the target map
 * \param fAdjX adjust x coordinates by this value (e.g. to uncenter) default: 0
 * \param fAdjY adjust y coordinates by this value (e.g. to uncenter) default: 0
 * \param fAdjZ adjust z coordinates by this value (e.g. to uncenter) default: 0
 * \param bProgress if true a progress bar is shown
 */
template<class T>
svt_volume<Real64>* svt_point_cloud_pdb<T>::blur(Real64 fWidth, Real64 fResolution, Real64 fAdjX, Real64 fAdjY, Real64 fAdjZ, bool bProgress)
{
    // create gaussian kernel
    svt_volume<Real64> oKernel;
    oKernel.createSitusBlurringKernel(fWidth, fResolution);

    // bring lattice into register with origin
    T oMin = this->getMinCoord();
    T oMax = this->getMaxCoord();

    Real64 fMinx = ( fWidth * floor( (oMin.x()+fAdjX) / fWidth ) - fAdjX);
    Real64 fMaxx = ( fWidth * ceil ( (oMax.x()+fAdjX) / fWidth ) - fAdjX);
    Real64 fMiny = ( fWidth * floor( (oMin.y()+fAdjY) / fWidth ) - fAdjY);
    Real64 fMaxy = ( fWidth * ceil ( (oMax.y()+fAdjY) / fWidth ) - fAdjY);
    Real64 fMinz = ( fWidth * floor( (oMin.z()+fAdjZ) / fWidth ) - fAdjZ);
    Real64 fMaxz = ( fWidth * ceil ( (oMax.z()+fAdjZ) / fWidth ) - fAdjZ);

    // allocate protein density map
    int iMargin = (int) ceil( (Real64)(oKernel.getSizeX()) / 2.0 );
    int iExtx = (int)(ceil( (fMaxx - fMinx) / fWidth )) + (2*iMargin) + 1;
    int iExty = (int)(ceil( (fMaxy - fMiny) / fWidth )) + (2*iMargin) + 1;
    int iExtz = (int)(ceil( (fMaxz - fMinz) / fWidth )) + (2*iMargin) + 1;

    //svtout << "blur: target map size: " << iExtx << " x " << iExty << " x " << iExtz << endl;

    svt_volume<Real64>* pMap = new svt_volume<Real64>( iExtx, iExty, iExtz );
    //pMap->setValue( 0.0 );
    pMap->setWidth( fWidth );

    //svtout << "blur: project mass" << endl;

    // interpolate structure to protein map and keep track of variability - i.e. create a volumetric map with peaks at the atomic positions...
    Real64 fVarp = 0.0;
    Real64 fGx, fGy, fGz, fA, fB, fC;
    int iX0, iY0, iZ0, iX1, iY1, iZ1;
    unsigned int i;
    unsigned int iAtomsNum = this->size();

    try
    {
        if (bProgress)
	    svt_exception::ui()->progressPopup("Projecting atoms onto lattice...", 0, this->size());

	for (i=0; i<iAtomsNum; i++)
	{
	    // compute position within grid
	    fGx = iMargin + ((this->getPoint(i).x() - fMinx) / fWidth);
	    fGy = iMargin + ((this->getPoint(i).y() - fMiny) / fWidth);
	    fGz = iMargin + ((this->getPoint(i).z() - fMinz) / fWidth);

	    iX0 = (int)(floor(fGx));
	    iY0 = (int)(floor(fGy));
	    iZ0 = (int)(floor(fGz));
	    iX1 = iX0+1;
	    iY1 = iY0+1;
	    iZ1 = iZ0+1;

	    // interpolate
	    fA = (Real64)(iX1) - fGx;
	    fB = (Real64)(iY1) - fGy;
	    fC = (Real64)(iZ1) - fGz;

	    pMap->setValue(iX0, iY0, iZ0, pMap->getValue(iX0, iY0, iZ0) + fA * fB * fC                 * m_aAtoms[i].getMass()); fVarp += fA * fB * fC * ((1.0-fA)*(1.0-fA)+(1.0-fB)*(1.0-fB)+(1.0-fC)*(1.0-fC));
	    pMap->setValue(iX0, iY0, iZ1, pMap->getValue(iX0, iY0, iZ1) + fA * fB * (1.0-fC)           * m_aAtoms[i].getMass()); fVarp += fA * fB * (1.0-fC) * ((1.0-fA)*(1.0-fA)+(1.0-fB)*(1.0-fB)+fC*fC);
	    pMap->setValue(iX0, iY1, iZ0, pMap->getValue(iX0, iY1, iZ0) + fA * (1-fB) * fC             * m_aAtoms[i].getMass()); fVarp += fA * (1.0-fB) * fC * ((1.0-fA)*(1.0-fA)+fB*fB+(1.0-fC)*(1.0-fC));
	    pMap->setValue(iX1, iY0, iZ0, pMap->getValue(iX1, iY0, iZ0) + (1.0-fA) * fB * fC           * m_aAtoms[i].getMass()); fVarp += (1.0-fA) * fB * fC * (fA*fA+(1.0-fB)*(1.0-fB)+(1.0-fC)*(1.0-fC));
	    pMap->setValue(iX0, iY1, iZ1, pMap->getValue(iX0, iY1, iZ1) + fA * (1-fB) * (1.0-fC)       * m_aAtoms[i].getMass()); fVarp += fA * (1.0-fB) * (1.0-fC) * ((1.0-fA)*(1.0-fA)+fB*fB+fC*fC);
	    pMap->setValue(iX1, iY1, iZ0, pMap->getValue(iX1, iY1, iZ0) + (1.0-fA) * (1-fB) * fC       * m_aAtoms[i].getMass()); fVarp += (1.0-fA) * (1.0-fB) * fC * (fA*fA+fB*fB+(1.0-fC)*(1.0-fC));
	    pMap->setValue(iX1, iY0, iZ1, pMap->getValue(iX1, iY0, iZ1) + (1.0-fA) * fB * (1.0-fC)     * m_aAtoms[i].getMass()); fVarp += (1.0-fA) * fB * (1.0-fC) * (fA*fA+(1.0-fB)*(1.0-fB)+fC*fC);
	    pMap->setValue(iX1, iY1, iZ1, pMap->getValue(iX1, iY1, iZ1) + (1.0-fA) * (1-fB) * (1.0-fC) * m_aAtoms[i].getMass()); fVarp += (1.0-fA) * (1.0-fB) * (1.0-fC) * (fA*fA+fB*fB+fC*fC);

	    if (bProgress && i%1000 == 0)
		svt_exception::ui()->progress( i, this->size());
	}
	fVarp /= (double)(iAtomsNum);

	if (bProgress)
	    svt_exception::ui()->progressPopdown();

    } catch (svt_userInterrupt&)
    {
	svt_exception::ui()->progressPopdown();
        delete pMap;
	return NULL;
    }

    //svtout << "blur: variability: " << fVarp << endl;
    //svtout << "blur: lattice smoothing (sigma = atom rmsd): " << fWidth * sqrt(fVarp) << " Angstroem" << endl;
    //svtout << "blur: convolve with kernel" << endl;

    // convolve
    pMap->convolve( oKernel, bProgress );

    //svtout << "blur: done." << endl;

    // set correct position of map relative to pdb
    pMap->setGrid( fMinx - ((Real64)(iMargin)*fWidth) + fAdjX, fMiny - ((Real64)(iMargin)*fWidth) + fAdjY, fMinz - ((Real64)(iMargin)*fWidth) + fAdjZ);

    // return
    return pMap;
};

/**
 * blur the pdb structure and thereby create an artificial low-resolution map
 * \param fWidth voxel width of the target map
 * \param fResolution resolution of the target map
 * \param fAdjX adjust x coordinates by this value (e.g. to uncenter) default: 0
 * \param fAdjY adjust y coordinates by this value (e.g. to uncenter) default: 0
 * \param fAdjZ adjust z coordinates by this value (e.g. to uncenter) default: 0
 * \param bProgress if true a progress bar is shown
 */
template<class T>
svt_volume<Real64>* svt_point_cloud_pdb<T>::blur1D(Real64 fWidth, Real64 fResolution, Real64 fAdjX, Real64 fAdjY, Real64 fAdjZ, bool bProgress)
{
    // create gaussian kernel
    svt_volume<Real64> oKernel;
    oKernel.create1DBlurringKernel(fWidth, fResolution);

    // bring lattice into register with origin
    T oMin = this->getMinCoord();
    T oMax = this->getMaxCoord();

    Real64 fMinx = ( fWidth * floor( (oMin.x()+fAdjX) / fWidth ) - fAdjX);
    Real64 fMaxx = ( fWidth * ceil ( (oMax.x()+fAdjX) / fWidth ) - fAdjX);
    Real64 fMiny = ( fWidth * floor( (oMin.y()+fAdjY) / fWidth ) - fAdjY);
    Real64 fMaxy = ( fWidth * ceil ( (oMax.y()+fAdjY) / fWidth ) - fAdjY);
    Real64 fMinz = ( fWidth * floor( (oMin.z()+fAdjZ) / fWidth ) - fAdjZ);
    Real64 fMaxz = ( fWidth * ceil ( (oMax.z()+fAdjZ) / fWidth ) - fAdjZ);

    // allocate protein density map
    int iMargin = (int) ceil( (Real64)(oKernel.getSizeX()) / 2.0 );
    int iExtx = (int)(ceil( (fMaxx - fMinx) / fWidth )) + (2*iMargin) + 1;
    int iExty = (int)(ceil( (fMaxy - fMiny) / fWidth )) + (2*iMargin) + 1;
    int iExtz = (int)(ceil( (fMaxz - fMinz) / fWidth )) + (2*iMargin) + 1;

    //svtout << "blur: target map size: " << iExtx << " x " << iExty << " x " << iExtz << endl;

    svt_volume<Real64>* pMap = new svt_volume<Real64>( iExtx, iExty, iExtz );
    //pMap->setValue( 0.0 );
    pMap->setWidth( fWidth );

    //svtout << "blur: project mass" << endl;

    // interpolate structure to protein map and keep track of variability - i.e. create a volumetric map with peaks at the atomic positions...
    Real64 fVarp = 0.0;
    Real64 fGx, fGy, fGz, fA, fB, fC;
    int iX0, iY0, iZ0, iX1, iY1, iZ1;
    unsigned int i;
    unsigned int iAtomsNum = this->size();

    try
    {
        if (bProgress)
	    svt_exception::ui()->progressPopup("Projecting atoms onto lattice...", 0, this->size());

	for (i=0; i<iAtomsNum; i++)
	{
	    // compute position within grid
	    fGx = iMargin + ((this->getPoint(i).x() - fMinx) / fWidth);
	    fGy = iMargin + ((this->getPoint(i).y() - fMiny) / fWidth);
	    fGz = iMargin + ((this->getPoint(i).z() - fMinz) / fWidth);

	    iX0 = (int)(floor(fGx));
	    iY0 = (int)(floor(fGy));
	    iZ0 = (int)(floor(fGz));
	    iX1 = iX0+1;
	    iY1 = iY0+1;
	    iZ1 = iZ0+1;

	    // interpolate
	    fA = (Real64)(iX1) - fGx;
	    fB = (Real64)(iY1) - fGy;
	    fC = (Real64)(iZ1) - fGz;

	    pMap->setValue(iX0, iY0, iZ0, pMap->getValue(iX0, iY0, iZ0) + fA * fB * fC                 * m_aAtoms[i].getMass()); fVarp += fA * fB * fC * ((1.0-fA)*(1.0-fA)+(1.0-fB)*(1.0-fB)+(1.0-fC)*(1.0-fC));
	    pMap->setValue(iX0, iY0, iZ1, pMap->getValue(iX0, iY0, iZ1) + fA * fB * (1.0-fC)           * m_aAtoms[i].getMass()); fVarp += fA * fB * (1.0-fC) * ((1.0-fA)*(1.0-fA)+(1.0-fB)*(1.0-fB)+fC*fC);
	    pMap->setValue(iX0, iY1, iZ0, pMap->getValue(iX0, iY1, iZ0) + fA * (1-fB) * fC             * m_aAtoms[i].getMass()); fVarp += fA * (1.0-fB) * fC * ((1.0-fA)*(1.0-fA)+fB*fB+(1.0-fC)*(1.0-fC));
	    pMap->setValue(iX1, iY0, iZ0, pMap->getValue(iX1, iY0, iZ0) + (1.0-fA) * fB * fC           * m_aAtoms[i].getMass()); fVarp += (1.0-fA) * fB * fC * (fA*fA+(1.0-fB)*(1.0-fB)+(1.0-fC)*(1.0-fC));
	    pMap->setValue(iX0, iY1, iZ1, pMap->getValue(iX0, iY1, iZ1) + fA * (1-fB) * (1.0-fC)       * m_aAtoms[i].getMass()); fVarp += fA * (1.0-fB) * (1.0-fC) * ((1.0-fA)*(1.0-fA)+fB*fB+fC*fC);
	    pMap->setValue(iX1, iY1, iZ0, pMap->getValue(iX1, iY1, iZ0) + (1.0-fA) * (1-fB) * fC       * m_aAtoms[i].getMass()); fVarp += (1.0-fA) * (1.0-fB) * fC * (fA*fA+fB*fB+(1.0-fC)*(1.0-fC));
	    pMap->setValue(iX1, iY0, iZ1, pMap->getValue(iX1, iY0, iZ1) + (1.0-fA) * fB * (1.0-fC)     * m_aAtoms[i].getMass()); fVarp += (1.0-fA) * fB * (1.0-fC) * (fA*fA+(1.0-fB)*(1.0-fB)+fC*fC);
	    pMap->setValue(iX1, iY1, iZ1, pMap->getValue(iX1, iY1, iZ1) + (1.0-fA) * (1-fB) * (1.0-fC) * m_aAtoms[i].getMass()); fVarp += (1.0-fA) * (1.0-fB) * (1.0-fC) * (fA*fA+fB*fB+fC*fC);

	    if (bProgress && i%1000 == 0)
		svt_exception::ui()->progress( i, this->size());
	}
	fVarp /= (double)(iAtomsNum);

	if (bProgress)
	    svt_exception::ui()->progressPopdown();

    } catch (svt_userInterrupt&)
    {
	svt_exception::ui()->progressPopdown();
        delete pMap;
	return NULL;
    }

    //svtout << "blur: variability: " << fVarp << endl;
    //svtout << "blur: lattice smoothing (sigma = atom rmsd): " << fWidth * sqrt(fVarp) << " Angstroem" << endl;
    //svtout << "blur: convolve with kernel" << endl;

    // convolve
    pMap->convolve1D3D( oKernel, bProgress );

    //svtout << "blur: done." << endl;

    // set correct position of map relative to pdb
    pMap->setGrid( fMinx - ((Real64)(iMargin)*fWidth) + fAdjX, fMiny - ((Real64)(iMargin)*fWidth) + fAdjY, fMinz - ((Real64)(iMargin)*fWidth) + fAdjZ);

    // return
    return pMap;
};

/**
 * Calculate rmsd between this and another structure.
 * The other structure should have equal or larger size. If larger, an oligomer is assumed,
 * and the overlap with a chain is determined. The most overlapping chain is then used as reference and the rmsd is computed.
 * \param rOlig reference to other structure
 * \param bAlign align the two structures first (default: false) - valid only for the case if the two structures have the same size, ignored otherwise!
 */
template<class T>
Real64 svt_point_cloud_pdb<T>::rmsd( svt_point_cloud_pdb<T>& rOlig, bool bAlign, Select iSelection, bool bShowProgress )
{
    Real64 fRMSD = 0.0;
    unsigned int j;

    unsigned int iChain = 0;
    bool bKearsley = bAlign;

    unsigned int iSizeThis = this->getAtomsNumber(iSelection);
    unsigned int iSizeOlig = rOlig.getAtomsNumber(iSelection);

    // Multimolecule docking or full molecule
    if ( iSizeThis < iSizeOlig )
    {
	// and calculate the rmsd to the oligomeric subcomponent
	int aChains[256];
	for(j=0; j<256; j++)
	    aChains[j] = 0;

	// calculate the nearest neighbor of each atom in the monomer regarding the oligomer and record the chain. The chain that occurs most is the chain that the rmsd is computed relative to.
	try
	{
	    if (bShowProgress)
		svt_exception::ui()->progressPopup("Determining corresponding chain...", 0, this->size());

	    for(j=0; j<this->size(); j++)
	    {
		unsigned int iIndex = rOlig.nearestNeighbor( (*this)[j] );
		if (rOlig.atom(iIndex).getChainID()-65 >= 0 && rOlig.atom(iIndex).getChainID()-65 < 255)
		    aChains[(unsigned int)(rOlig.atom(iIndex).getChainID()-65)]++;
                else {
                    if ( rOlig.atom(iIndex).getChainID() == '-' )
                        aChains[0]++;
                    else
                        svtout << "Warning: Unknown ChainID: " << rOlig.atom(iIndex).getChainID() << endl;
                }

		if (j%10000 == 0 && bShowProgress)
		    svt_exception::ui()->progress( j, this->size());
	    }
	} catch (svt_userInterrupt&)
	{
	    if (bShowProgress)
		svt_exception::ui()->progressPopdown();
	    return 1.0E10;
	}
	if (bShowProgress)
	    svt_exception::ui()->progressPopdown();


	int iMax = 0;
	unsigned int iMaxIndex = 0;
	for(j=0; j<256; j++)
	{
	    if (aChains[j] > iMax)
	    {
		iMax = aChains[j];
		iMaxIndex = j;
	    }
	}

	iChain = iMaxIndex * this->size();
	if (bShowProgress)
	    svtout << "Compare to chain: " << iMaxIndex << endl;

        bKearsley = false;
    }
    if (iSizeThis > iSizeOlig)
    {
        return 1.0E10;
    }

    // least-square fit
    svt_matrix4<Real64> oMat;
    if (bKearsley)
	oMat = kearsley( *this, rOlig );

    // now calculate the actual rmsd
    unsigned int iSizeSelection=0;
    try
    {
        unsigned int iSize = this->size();
        iSizeOlig = rOlig.size();

        if (bShowProgress)
	   svt_exception::ui()->progressPopup("Calculating RMSD...", 0, iSize);

        switch(iSelection)
        {
            case ALL:
                for(j=0; j<iSize; j++)
                {
                    if (j+iChain < iSizeOlig)
                    {
                        fRMSD += (oMat * (*this)[j]).distanceSq( rOlig[j+iChain] );
                        iSizeSelection++;
                    }else
                        return 1.0E10;

                    if (bShowProgress && j%10000 == 0)
                        svt_exception::ui()->progress( j, iSize);
                }
                break;
            case BACKBONE:
                for(j=0; j<iSize; j++)
                {
                    if ( j+iChain <= iSizeOlig)
                    {
                        if (m_aAtoms[j].isBackbone())
                        {
                            fRMSD += (oMat * (*this)[j]).distanceSq( rOlig[j+iChain] );
                            iSizeSelection++;
                        }
                    }else
                    {
                        return 1.0E10;
                    }

                    if (bShowProgress && j%10000 == 0)
                        svt_exception::ui()->progress( j, iSize);
                }
                break;
            case TRACE:
                for(j=0; j<iSize; j++)
                {
                    if ( j+iChain <= iSizeOlig)
                    {
                        if (m_aAtoms[j].isCA())
                        {
                        fRMSD += (oMat * (*this)[j]).distanceSq( rOlig[j+iChain] );
                        iSizeSelection++;
                        }
                    }
                    else
                        return 1.0E10;

                    if (bShowProgress && j%10000 == 0)
                        svt_exception::ui()->progress( j, iSize);
                }
                break;
        }

    } catch (svt_userInterrupt&)
    {
	if (bShowProgress)
            svt_exception::ui()->progressPopdown();
	return 1.0E10;
    }
    if (bShowProgress)
        svt_exception::ui()->progressPopdown();

    fRMSD /= iSizeSelection;
    fRMSD = sqrt( fRMSD );

    return fRMSD;
};

/**
 * Calculate dRMSD (distance RMSD - intramolecular distances) between this and another structure.
 * \param rOlig reference to other structure
 */
template<class T>
Real64 svt_point_cloud_pdb<T>::drmsd( svt_point_cloud_pdb<T>& rOlig )
{
    Real64 fdRMSD = 0.0;
    unsigned int iIndexAtom1, iIndexAtom2;
    Real64 fDist1, fDist2;
    unsigned int iCount = this->size() *( this->size() - 1 );

    // now calculate the actual rmsd
    try
    {
        if (this->size() != rOlig.size())
        {
           return 1.0E10;
        }

	svt_exception::ui()->progressPopup("Calculating dRMSD...", 0, iCount);

	for(iIndexAtom1=0; iIndexAtom1 < this->size(); iIndexAtom1++)
	{
            for (iIndexAtom2=0; iIndexAtom2 < iIndexAtom1; iIndexAtom2++)
            {
                fDist1 = ( (*this)[iIndexAtom1] ).distance( ((*this)[iIndexAtom2]) );
                fDist2 = ( rOlig[iIndexAtom1] ).distance( rOlig[iIndexAtom2] );
                fdRMSD += (fDist2-fDist1)*(fDist2-fDist1);

                if (iIndexAtom1%100 == 0)
                    svt_exception::ui()->progress( iIndexAtom1*(iIndexAtom1+1)+(iIndexAtom2+1), iCount);
            }

	}

    } catch (svt_userInterrupt&)
    {
	svt_exception::ui()->progressPopdown();
	return 1.0E10;
    }
    svt_exception::ui()->progressPopdown();


    fdRMSD /=  iCount;
    fdRMSD = sqrt( fdRMSD );

    return fdRMSD;
};

/**
 * Get a chain
 * \param cChainID the chain ID
 * \return returns the atoms of the chain as pointer to another svt_point_cloud_pdb object
 */
template<class T>
svt_point_cloud_pdb<T> svt_point_cloud_pdb<T>::getChain(char cChainID)
{
    svt_point_cloud_pdb<T> pPDB;

    for(unsigned int i=0; i<(*this).size(); i++)
	if ((*this).atom(i).getChainID() == cChainID)
	    pPDB.addAtom( (*this).atom(i), (*this)[i] );

    return pPDB;
};

/**
 * Get the trace
 * \return returns the CA as svt_point_cloud_pdb object
 */
template<class T>
svt_point_cloud_pdb<T> svt_point_cloud_pdb<T>::getTrace(unsigned int iSkip)
{
    unsigned int iSize = (*this).size();

    if ( iSkip > iSize )
    {
        svt_exception::ui()->info("You choose to omit more atoms than the structure has.\nPlease decrease the number of omitted atoms!");
        return *this;
    }

    svt_point_cloud_pdb<T> oPDB;
    unsigned int iSkipped = iSkip; // initialize iSkipped so that selected CA starts form the first CA

    for(unsigned int i=0; i< iSize; i++)
    {
	// if this is a nucleic acid, the resname should consist of two spaces and a letter
	//
	if (
	     // Atom is C alpha
	     (m_aAtoms[i].isCA())

	     || // or

	     // Atom belongs to a nucleic acid (using P instead of CA)
	     ( (m_aAtoms[i].getResname()[0] == ' ' && m_aAtoms[i].getResname()[1] == ' ')
	       && (m_aAtoms[i].getName()[0] == 'P')
	     )
	   )
        {
            if (iSkipped==iSkip)
            {
                oPDB.addAtom( m_aAtoms[i], (*this)[i] );
                iSkipped = 0;
            } else
                iSkipped++;
        }
    }

    svtout << oPDB.size() << " atoms in the trace." << endl;

    return oPDB;
};


/**
 * gets the backbone of the structure
 * \return svt_point_cloud_pdb with the backbone
 */
template<class T>
svt_point_cloud_pdb<T> svt_point_cloud_pdb<T>::getBackbone()
{
    svt_point_cloud_pdb<T> oPDB;
    unsigned int iSize = (*this).size();
    for(unsigned int i=0; i< iSize; i++)
    {
	if (m_aAtoms[i].isBackbone())
            oPDB.addAtom( m_aAtoms[i], (*this)[i] );
    }

    svtout << oPDB.size() << " atoms in the backbone." << endl;

    return oPDB;
};

/**
 * create a cylinder based on a circle of radius=fRadius with iPoints
 * The circle is copied on z axis iCircleCopies times using fDist as separation
 */
template<class T>
void svt_point_cloud_pdb<T>::makeCylinder(unsigned int iPoints, Real64 fRadius, unsigned int iCircleCopies, Real64 fDist )
{
    //clean the current cloud
    deleteAllAtoms();
    deleteAllBonds();

    svt_point_cloud_pdb<T> oTurn;

    T oVec;
    oVec.x( 0.0 );
    oVec.y( 0.0 );
    oVec.z( 0.0 );
    
    svt_point_cloud_atom oAtom;
    oAtom.setName( "C");
    oAtom.setRemoteness('A');
    oAtom.setResidueSeq( 1 );
    oTurn.addAtom( oAtom, oVec); // center atom - may be added as an option

    //create circle
    Real64 fDelta = 360.0/Real64(iPoints-1); 
    for (Real64 fAngle = 0; fAngle < 360; fAngle += fDelta)
    {
        //create the circle used by the expansion template 
        oVec.x ( fRadius * cos (deg2rad( fAngle ) ) );
        oVec.y ( fRadius * sin (deg2rad( fAngle ) ) );
        
        oAtom.setMass(1.0);
        oAtom.setResidueSeq( int(fAngle/fDelta) + 2);
        oTurn.addAtom( oAtom, oVec );
    }

    //copy along z axis
    oVec.x( 0.0 );
    oVec.y( 0.0 );

    svt_matrix4<Real64> oMat;
    svt_point_cloud_pdb< svt_vector4<Real64> > oTrPDB;
    append(oTurn);
    for (unsigned int iTurn = 0; iTurn< iCircleCopies; iTurn++) 
    {
        oVec.z ( fDist*iTurn ); 
        oMat.setTranslation( oVec );
        
        oTrPDB = oMat*oTurn;
        append(oTrPDB);
    }

    //center
    T oCoa = this->coa();

    oMat.loadIdentity();
    oMat.setTranslation( -oCoa );
    *this = oMat*(*this);
};

/**
 * Get the number of atoms in
 * \param iSelection can be ALL, BACKBONE, or CA; with ALL- function equivalent with this->size()
 */
template<class T>
unsigned int svt_point_cloud_pdb<T>::getAtomsNumber(Select iSelection)
{
   unsigned int iNumber=0;
   unsigned int iSize  = this->size();

   switch (iSelection)
   {
       case ALL:
           iNumber = iSize;
           break;
       case BACKBONE:
            for(unsigned int j=0; j<iSize; j++)
            {
                if (m_aAtoms[j].isBackbone())
                    iNumber++;
            }
           break;
       case TRACE:
           for(unsigned int j=0; j<iSize; j++)
            {
                if (m_aAtoms[j].isCA())
                    iNumber++;
            }
           break;
   }

   return iNumber;
};


///////////////////////////////////////////////////////////////////////////////
// Secondary Structure Information
///////////////////////////////////////////////////////////////////////////////

/**
 * is the secondary structure information already specified?
 * \param bSecStruct true if the atoms know their secondary structure membership
 */
template<class T>
void svt_point_cloud_pdb<T>::setSecStructAvailable( bool bSecStructAvailable )
{
    m_bSecStructAvailable = bSecStructAvailable;

    if (!m_bSecStructAvailable) 
        m_eSecStructSource = SSE_NOTAVAILABLE;
    else
        m_eSecStructSource = SSE_OTHER;
    
};

/**
 * is the secondary structure information already specified?
 * return true if the atoms know their secondary structure membership
 */
template<class T>
bool svt_point_cloud_pdb<T>::getSecStructAvailable()
{
    return m_bSecStructAvailable;
};

/**
 * set the secondary structure source
 * \param 0 - not available, 1 - pdb, 2 - stride 
 */
template<class T>
void svt_point_cloud_pdb<T>::setSecStructSource( int eSecStructSource )
{
    m_eSecStructSource = (svt_secStructSource)eSecStructSource;

    if (eSecStructSource != SSE_NOTAVAILABLE )
        m_bSecStructAvailable = true;
    else
        m_bSecStructAvailable = false;
        
};

/**
 * set the secondary structure source
 * \return 0 - not available, 1 - pdb, 2 - stride 
 */
template<class T>
int svt_point_cloud_pdb<T>::getSecStructSource()
{
    return m_eSecStructSource; 
};

/**
 * get the compressed list of secondary structure- the one used to hold the information wrote in pdb in HELIX and SHEET entry
 */ 
template<class T>
vector <svt_sse_pdb>& svt_point_cloud_pdb<T>::getSecStructCompressedList()
{
    return m_aSsePdb;
};

/**
 * set the compressed list of secondary structure- the one used to hold the information wrote in pdb in HELIX and SHEET entry
 */ 
template<class T>
void svt_point_cloud_pdb<T>::setSecStructCompressedList( vector <svt_sse_pdb> aSse )
{
    m_aSsePdb = aSse;
    
    //reassign the sse to the atoms
    for (unsigned int iIndex=0; iIndex< m_aAtoms.size(); iIndex++)
    {
        char cSse = getSecStructFromCompressedList( m_aAtoms[iIndex]);
        m_aAtoms[iIndex].setSecStruct ( cSse );
    }
};

/**
 * get the sse from the list for this atom 
 * \param oAtom the atom 
 */ 
template<class T>
char svt_point_cloud_pdb<T>::getSecStructFromCompressedList( svt_point_cloud_atom oAtom )
{
    char cSse = 'C';
    for (unsigned int iSse=0; iSse<m_aSsePdb.size(); iSse++ )
    {
        if (    oAtom.getChainID()      == m_aSsePdb[iSse].m_cInitialResChainID &&
                oAtom.getChainID()      == m_aSsePdb[iSse].m_cTerminalResChainID &&
                oAtom.getResidueSeq()   >= m_aSsePdb[iSse].m_iInitialResSeq &&
                oAtom.getResidueSeq()   <= m_aSsePdb[iSse].m_iTerminalResSeq    )
        {
            if ((m_aSsePdb[iSse].m_aType)[0]=='H')
            {
                //consider the different types of helices
                switch( m_aSsePdb[iSse].m_iClass)
                {
                   case 3: //Right-handed pi
                        cSse = 'I';
                        break; 
                   case 5: //Right-handed pi
                        cSse = 'G';
                        break; 
                    default: 
                        cSse = 'H';
                }
            }
            else // is beta sheet
            {
                cSse = 'E';
            }
        }
    }
    return cSse;
}

/**
 * calculate the secondary structure information
 */
template<class T>
void svt_point_cloud_pdb<T>::calcSecStruct()
{
    if ( this->getSecStructAvailable() == true )
    	return;

    if ( this->m_aAtoms.size() == 0 )
       return;

    //holds the current model or chain
    svt_point_cloud_pdb<T> oPDB;

    unsigned int iCurrentModel;
    char cCurrentChain;
    unsigned int iOrdResidueSeq=0;
    int iPrevResSeq = -1;
    int iNoCA = 0;

    unsigned int iSize = this->m_aAtoms.size();

    if (iSize>0)
    {
    	iCurrentModel = m_aAtoms[0].getModel();
    	cCurrentChain = m_aAtoms[0].getChainID();
    }
    else
    	return;

    try
    {
    	svt_exception::ui()->progressPopup("Determination of secondary structure", 0, iSize-1);

        for (unsigned int iAtom = 0; iAtom < iSize; iAtom++)
        {
            oPDB.addAtom(*(*this).getAtom(iAtom), (*this)[iAtom]);

            if (oPDB.getAtom(oPDB.size()-1)->getResidueSeq()!= iPrevResSeq)
                iOrdResidueSeq++;
            iPrevResSeq = oPDB.getAtom(oPDB.size()-1)->getResidueSeq();
            oPDB.getAtom(oPDB.size()-1)->setOrdResidueSeq( iOrdResidueSeq );

            if ( oPDB.getAtom(oPDB.size()-1)->isCA() )
                iNoCA++;

            if ( (iAtom + 1 < iSize && (m_aAtoms[iAtom + 1].getModel() != iCurrentModel || m_aAtoms[iAtom + 1].getChainID() != cCurrentChain) ) || iAtom == iSize - 1) // is this the last atom in the chain,model or last atom in the pdb
            {
                if (iNoCA>=5) // run stride only if there are at least 5 residuals
                    oPDB.runStride();

                for (unsigned int i = 0; i < oPDB.size(); i++)
                {
                    m_aAtoms[iAtom - i].setSecStruct( oPDB.getAtom(oPDB.size() - i - 1)->getSecStruct());
                    m_aAtoms[iAtom - i].setSecStructNumResidues( oPDB.getAtom(oPDB.size() - i - 1)->getSecStructNumResidues());
                }

                oPDB.deleteAllAtoms();
                oPDB.setSecStructAvailable( false );
                iNoCA = 0;

                if (iAtom + 1 < iSize)
                {
                    iCurrentModel = m_aAtoms[iAtom + 1].getModel();
                    cCurrentChain = m_aAtoms[iAtom + 1].getChainID();
                    iOrdResidueSeq = 0;
                }
            }
            if (( iAtom  % 1000) == 0)
                svt_exception::ui()->progress(iAtom, iSize-1);
        }

        svt_exception::ui()->progressPopdown();

    } catch (svt_userInterrupt&)
    {
    }

    calcOrdinalChainIDs();
    calcSecStructLengths();
    //also set the secStructureAvailable(true);

    setSecStructSource(SSE_STRIDE);
    compressSecStruct();
};

/**
 * calculate the secondary structure information
 */
template<class T>
void svt_point_cloud_pdb<T>::runStride()
{
    if ( this->m_aAtoms.size() == 0 )
        return;

    char *pBuffer;
    char aRecordName[7];

    // creates svt_stride object
    svt_stride oStride;

    // alocate memory for buffors
    vector< char *> aBuffer;
    vector< char *> aVectorRaport;

    unsigned int iAtomNumber= m_aAtoms.size();
    unsigned int i;

    char cChainID;

    for(i=0; i<iAtomNumber; i++)
    {
        cChainID = m_aAtoms[i].getChainID();
        if (cChainID == '-')
            cChainID = ' ';

        if (m_aAtoms[i].getHetAtm())
            sprintf(aRecordName,"%s","HETATM\0");
        else
            sprintf(aRecordName,"%s","ATOM  \0");

        pBuffer = new char[1024];

        sprintf(pBuffer, "%s%5i %2s%c%c%c%-2s %c%4i%c   %8.*f%8.*f%8.*f%6.2f%6.2f %3s  %4s%2s%2s\n",
                aRecordName,
                m_aAtoms[i].getPDBIndex(),
                m_aAtoms[i].getName(),
                m_aAtoms[i].getRemoteness(),
                m_aAtoms[i].getBranch(),
                m_aAtoms[i].getAltLoc(),
                m_aAtoms[i].getResname(),
                cChainID,
                m_aAtoms[i].getResidueSeq(),
                m_aAtoms[i].getICode(),
                CoordPrecisionPDB(this->getPoint(i).x()), this->getPoint(i).x(),
                CoordPrecisionPDB(this->getPoint(i).y()), this->getPoint(i).y(),
                CoordPrecisionPDB(this->getPoint(i).z()), this->getPoint(i).z(),
                m_aAtoms[i].getOccupancy(),
                m_aAtoms[i].getTempFact(),
                m_aAtoms[i].getNote(),
                m_aAtoms[i].getSegmentID(),
                m_aAtoms[i].getElement(),
                m_aAtoms[i].getCharge());

        aBuffer.push_back(pBuffer);

    }

    // run stride as a class
    if(oStride.sseindexer(aBuffer, aVectorRaport))
    {
        // read in the stride output: aVectorRaport
        char pResId[6];
        int iResId;
        char cChain;
        char cSec;
        char *pCBuffer;
        map< long, char > aSecStruct;

        unsigned int iVRDim = aVectorRaport.size();
        for(i=0; i<iVRDim; i++)
        {
            pCBuffer=aVectorRaport[i];
            pResId[0] = pCBuffer[16];
            pResId[1] = pCBuffer[17];
            pResId[2] = pCBuffer[18];
            pResId[3] = pCBuffer[19];
            pResId[4] = 0;
            iResId = atoi(pResId);
            // get the chain information
            cChain = pCBuffer[9];
            // get the secondary structure information
            cSec = toupper(pCBuffer[24]);

            aSecStruct[ iResId + (cChain * 65537) ] = cSec;
        }

        // apply this secondary structure information to all atoms of the residue
        int iOrdResidueSeq=1;
        unsigned int j;

        for(j=0; j<iAtomNumber; j++)
        {
            if (j>0 && m_aAtoms[j].getOrdResidueSeq() != m_aAtoms[j-1].getOrdResidueSeq() && !m_aAtoms[j].getHetAtm())
                iOrdResidueSeq++;

            if (m_aAtoms[j].getHetAtm())
                m_aAtoms[j].setSecStruct('C');
            else
                m_aAtoms[j].setSecStruct( aSecStruct[ iOrdResidueSeq + (m_aAtoms[j].getChainID() * 65537) ] );
        }

        for(i=0; i<iAtomNumber; i++)
            delete[] aBuffer[i];
        for(i=0; i<aVectorRaport.size(); i++)
            delete[] aVectorRaport[i];
    }
    else
        svtout << "Secondary structure assignment failed!" << endl;
}

/**
 * Compress the secondary structure information into the ssePdb object
 * \ATTENTION: erases the existant compressed information and recompresses it from the pdb atom entry 
 */
template<class T>
void svt_point_cloud_pdb<T>::compressSecStruct()
{
    unsigned int iNum = m_aAtoms.size();

    //delete the existant information
    m_aSsePdb.clear();

    char cLastSse = m_aAtoms[0].getSecStruct();
    char cCurrentSse;
    int iLastCA = 0;
    svt_sse_pdb oSse;
    sprintf(oSse.m_aType," ");

    bool bIsSseOpen = false; //if true - a Sse was started, but not yet finished;
    int iCountHelices = 0, iCountSheets = 0;
    for (unsigned int iAtom=1; iAtom < iNum; iAtom++)
    {
        if (m_aAtoms[iAtom].isCA())
        {
            cCurrentSse = m_aAtoms[iAtom].getSecStruct();
            if (cCurrentSse != cLastSse) // the secondary structure element just changed
            {
                //close previous sse element
                if (strlen(oSse.m_aType)>1 && bIsSseOpen)
                {
                    bIsSseOpen = false;

                    sprintf(oSse.m_aTerminalResname ,m_aAtoms[iLastCA].getResname() );
                    oSse.m_cTerminalResChainID  = m_aAtoms[iLastCA].getChainID() ;
                    oSse.m_iTerminalResSeq      = m_aAtoms[iLastCA].getResidueSeq() ;
                    oSse.m_cTerminalICode       = m_aAtoms[iLastCA].getICode() ;

                    if (oSse.m_aType[0]=='H')
                    {
                        oSse.m_iLen = oSse.m_iTerminalResSeq - oSse.m_iInitialResSeq + 1; 
                        iCountHelices++;
                        oSse.m_iNum = iCountHelices;
                        sprintf(oSse.m_aID,"%3d", iCountHelices%1000); 
                    }

                    if (oSse.m_aType[0]=='S')
                    {
                        iCountSheets++;
                        oSse.m_iNum = iCountSheets;
                        sprintf(oSse.m_aID,"%3d", iCountSheets%1000);
                        
                        oSse.m_iNumStrands = 1;

                        oSse.m_iSense = 0; 
                        
                        strcpy(oSse.m_aCurAtom,  "" );
                        strcpy(oSse.m_aCurResname, "");
                        oSse.m_cCurResChainID = ' ';
                        strcpy(oSse.m_aCurResSeq, "" );
                        oSse.m_cCurICode = ' ';  

                        strcpy(oSse.m_aPrevAtom,  "" );
                        strcpy(oSse.m_aPrevResname, "");
                        oSse.m_cPrevResChainID = ' ';
                        strcpy(oSse.m_aPrevResSeq, "");
                        oSse.m_cPrevICode = ' ';  

                    }


                    m_aSsePdb.push_back( oSse );                     
                }
        
                if (cCurrentSse=='H' || cCurrentSse=='G' || cCurrentSse=='I') // got a new helix
                {
                    bIsSseOpen = true;
 
                    sprintf(oSse.m_aType,"HELIX ");
                    sprintf(oSse.m_aComment," ");  
                
                    switch(cCurrentSse)
                    {   
                        case 'H':
                            oSse.m_iClass = 1;
                            break;
                        case 'G':
                            oSse.m_iClass = 5;
                            break;
                        case 'I':
                            oSse.m_iClass = 3;
                            break;
                    }

                    sprintf(oSse.m_aInitialResname ,m_aAtoms[iAtom].getResname() );
                    oSse.m_cInitialResChainID =   m_aAtoms[iAtom].getChainID() ;
                    oSse.m_iInitialResSeq = m_aAtoms[iAtom].getResidueSeq() ;
                    oSse.m_cInitialICode =  m_aAtoms[iAtom].getICode() ; 
                }
                
                if (cCurrentSse=='b' || cCurrentSse=='B' || cCurrentSse=='E')
                {
                    bIsSseOpen = true; 

                    sprintf(oSse.m_aType,"SHEET ");

                    sprintf(oSse.m_aInitialResname ,m_aAtoms[iAtom].getResname() );
                    oSse.m_cInitialResChainID   = m_aAtoms[iAtom].getChainID() ;
                    oSse.m_iInitialResSeq       = m_aAtoms[iAtom].getResidueSeq() ;
                    oSse.m_cInitialICode        = m_aAtoms[iAtom].getICode() ;
                }
            }
            cLastSse = cCurrentSse;
            iLastCA = iAtom;
       }        
    }
}


/**
 * set the distance cutoff value that determines the maximum distance for which consecutive residues are connected
 * \param fTraceCutoff the distance cutoff value
 */
template<class T>
void svt_point_cloud_pdb<T>::setTraceCutoff(float fTraceCutoff)
{
    m_fTraceCutoff = fTraceCutoff;
}
/**
 * get the distance cutoff value that determines the maximum distance for which consecutive residues are connected
 * \return the distance cutoff value
 */
template<class T>
float svt_point_cloud_pdb<T>::getTraceCutoff()
{
    return m_fTraceCutoff;
}
/**
 * compute the ordinal chainIDs. Those identify chains in the pdb based only on a C-alpha to C-alpha
 * distance criterion and are supposed to be calculated independently from the pdb chainIDs
 */
template<class T>
void svt_point_cloud_pdb<T>::calcOrdinalChainIDs()
{
    int i=0, j, iOrdChainID = 1, iNumAtoms = m_aAtoms.size();
    int iAtom_first=0, iAtom_last=0;
    T oVecA, oVecB;


    if (iNumAtoms == 0)
	return;

    while (true)
    {
	if (m_aAtoms[i].isCA() || (i+1) == iNumAtoms)
	    break;
	++i;
    }

    oVecB = T ( (*this)[i] );
    ++i;

    while (true)
    {
	if (i == iNumAtoms)
	    break;

	if (m_aAtoms[i].isCA())
	{
	    oVecA = oVecB;
	    oVecB = T ( (*this)[i] );

	    if (oVecA.distance( oVecB ) > m_fTraceCutoff)
	    {

		// find last atom of previous residue,
		// and set ord chain id from iAtom_first to iAtom_last

		iAtom_last = i;


		// now, find the index of the last atom in the previous residue. until this one, we
                // have to set the ordinal residue sequence number
                //
		// do a lot of tests, because screwed pdb files may assign the same residue sequence
                // number to consequent residues, so it is not sufficient to test for resudue
                // sequence number alone

		while (   m_aAtoms[iAtom_last].getResidueSeq() == m_aAtoms[i].getResidueSeq()
		       && strcmp(m_aAtoms[iAtom_last].getResname(), m_aAtoms[i].getResname()) == 0
		       && m_aAtoms[iAtom_last].getModel()      == m_aAtoms[i].getModel()
		       && strcmp(m_aAtoms[iAtom_last].getSegmentID(), m_aAtoms[i].getSegmentID()) == 0)
		{
	            --iAtom_last;
		}

		for (j=iAtom_first; j<=iAtom_last; ++j)
		    m_aAtoms[j].setOrdChainID(iOrdChainID);

		// store the index of the first atom. from this one on, we will set the incremented
		// ordinal chain id for the next ordinal chain
		iAtom_first = iAtom_last+1;
		++iOrdChainID;

		// now go on from the last atom in the previous residue. note that with the ++i below,
                // we actually continue the iteration from the first atom of the current residue
		i = iAtom_last;
	    }
	}
	++i;
    }

    // set ordinal chainID of remaining atoms
    for (j=iAtom_first; j<iNumAtoms; ++j)
	m_aAtoms[j].setOrdChainID(iOrdChainID);

    // set ordinal chainID of all HETATMs to -1
    for (j=0; j<iNumAtoms; ++j)
    {
	if (m_aAtoms[j].getHetAtm())
	    m_aAtoms[j].setOrdChainID(-1);

	//printf("atom %6i ordResSeq %5i ordchainID %3i chainID %c\n", j, m_aAtoms[j].getOrdResidueSeq(), m_aAtoms[j].getOrdChainID(), m_aAtoms[j].getChainID());
    }
}
/**
 * compute and set the length of the secondary structure elements. Secondary structure must have
 * been computed (obviously...), and calcOrdinalChainIDs must have been called before for
 * calcSecStructLengths to make sense
 */
template<class T>
void svt_point_cloud_pdb<T>::calcSecStructLengths()
{
    //if PDB file is corrupt or empty meaning no atoms, do nothing, return.
    if (m_aAtoms.size() == 0)
	return;
    // compute and set the length of the secondary structure elements
    //
    char cLastStruct = m_aAtoms[0].getSecStruct(), cCurrentStruct;
    int iNumAtoms = m_aAtoms.size();
    int iFirstAtom = 0, iNumResidues = 0;
    int i, j;
    bool bSetLength = false;


    // if the first atom in the array is a C_alpha, count it
    //
    if (m_aAtoms[0].isCA())
	++iNumResidues;


    // only one atom? nothing much to do
    //
    if (iNumAtoms == 1)
    {
	if (m_aAtoms[0].getSecStruct() != ' ')
	    m_aAtoms[0].setSecStructNumResidues(0);
	else
	    m_aAtoms[0].setSecStructNumResidues(iNumResidues);

	return;
    }


    // assign the length of secondary structure (i.e. the number of residues in it) to all atoms
    // in a secondary structure section. this length is defined as the number of residues, which
    // equals the number of C_alphas.
    //
    // a sec. structure part ends, if
    //   1. the sec. structure type changes,
    //   2. no sec. structure is assigned to the next atom (like for HETATMs)
    //   3. a new ordinal chain begins (calcOrdinalChainIDs must have been called before)
    //

    i = 1;

    while (true)
    {

	// at every C_alpha, increase the residue count (since there is one C_alpha per residue)
	//if (m_aAtoms[i].isCA())
	if (m_aAtoms[i].isCA())
	    ++iNumResidues;


	cCurrentStruct = m_aAtoms[i].getSecStruct();


	// 1. does the sec. structure type change, or
	// 2. is it empty?
	switch (cCurrentStruct)
	{
	    case 'H': case 'G': case 'I':
		if (cLastStruct != 'H' && cLastStruct != 'G' && cLastStruct != 'I')
		    bSetLength = true;
		break;

	    case 'E': case 'B':
		if (cLastStruct != 'E' && cLastStruct != 'B')
		    bSetLength = true;
		break;

	    case 'T': case 'C':
		if (cLastStruct != 'T' && cLastStruct != 'C')
		    bSetLength = true;
		break;

	    default://case ' ':
		bSetLength = true;
		break;
	}


	// 3. does a new (ordinal) chain begin?
	if ( m_aAtoms[i-1].getOrdChainID() != m_aAtoms[i].getOrdChainID() )
	    bSetLength = true;


	// if it was determined that the secondary structure element ends, or we arrived at the last
	// atom, setSecStructNumResidues(iNumResidues) must be called now for all atoms in the current
	// structure element (i.e., since m_aAtoms[iFirstAtom])
	if (bSetLength || i == iNumAtoms-1)
	{
	    if (i == iNumAtoms-1)
		++i;
	    // the following else if is because we are already at the C_alpha of the next part, and
	    // it was already added to iNumResidues, which is not correct
	    else if (m_aAtoms[i].isCA())
		--iNumResidues;


	    for (j=iFirstAtom; j<i; ++j)
		m_aAtoms[j].setSecStructNumResidues(iNumResidues);

	    bSetLength = false;
	    iFirstAtom = i;

	    // if the current atom is a C_alpha, it needs to be counted,too
	    if (m_aAtoms[i].isCA())
		iNumResidues = 1;
	    else
		iNumResidues = 0;
	}


	// for debugging...
//   	printf("atom index: % 6i", i);
//   	printf("  sec struct: %c", m_aAtoms[i].getSecStruct());
//   	printf("  ord res: % 4i", m_aAtoms[i].getOrdResidueSeq());
//   	printf("  ord chainID: % 3i", m_aAtoms[i].getOrdChainID());
//   	printf("  iNumResidues: % 3i ", iNumResidues);
// 	printf("\n");


	cLastStruct = cCurrentStruct;

	if (i == iNumAtoms)
	    break;
	else
	    ++i;
    }

    // for debugging...
//     for (i=0;i<iNumAtoms; ++i)
//     {
//   	printf("atom index: % 6i", i);
//   	printf("  sec struct: %c", m_aAtoms[i].getSecStruct());
//   	printf("  ord res: % 4i", m_aAtoms[i].getOrdResidueSeq());
//   	printf("  ord chainID: % 3i", m_aAtoms[i].getOrdChainID());
//   	printf("  iNumResidues: % 3i", m_aAtoms[i].getSecStructNumResidues());
// 	printf("\n");
//     }
}
/**
 * calculate the bonds between the atoms (according to their distance the bonds are guessed)
 */
template<class T>
void svt_point_cloud_pdb<T>::calcBonds(bool bShowProgress)
{
    if (m_aAtoms.size() == 0)
        return;

    svtout << "No information about covalent bonds found, therefore the bonds are guessed based on a distance criterion..." << endl;

    // cutoff is maximum vdw radius * 1.2
    Real32 fCutoff = 1.90f * 2.5f;
    Real32 fCutoffSq = fCutoff*fCutoff;

    deleteAllBonds();

    svtout << "  Space Partitioning..." << endl;

    // calculate the minimum and maximum atom coords
    Real32 fMinX = this->getMinXCoord();
    Real32 fMaxX = this->getMaxXCoord();
    Real32 fMinY = this->getMinYCoord();
    Real32 fMaxY = this->getMaxYCoord();
    Real32 fMinZ = this->getMinZCoord();
    Real32 fMaxZ = this->getMaxZCoord();

    svtout << "  Max: ( " << fMaxX << ", " << fMaxY << ", " << fMaxZ << " ) - Min: ( " << fMinX << ", " << fMinY << ", " << fMinZ << " )" << endl;

    // pre-sorting all atoms into boxes
    // how many boxes do we need?
    unsigned int iBoxesX = (unsigned int)(((fMaxX - fMinX) / fCutoff) + 0.5f) +1;
    unsigned int iBoxesY = (unsigned int)(((fMaxY - fMinY) / fCutoff) + 0.5f) +1;
    unsigned int iBoxesZ = (unsigned int)(((fMaxZ - fMinZ) / fCutoff) + 0.5f) +1;
    unsigned int iBoxesXY = iBoxesX * iBoxesY;

    svtout << "  " << iBoxesX << " x " << iBoxesY << " x " << iBoxesZ << endl;

    if ((iBoxesX+1)*(iBoxesY+1)*(iBoxesZ+1)>1e6)
    {
        svtout << "Unable to compute bonds due to space partitioning?" << endl;
        return;
    }

    // allocate memory
    vector< vector<unsigned int> > aBox( (iBoxesX+1)*(iBoxesY+1)*(iBoxesZ+1) );
    int iBoxX, iBoxY, iBoxZ, iBoxIndex;
    // now sort all atoms into these boxes
    unsigned int i;
    for(i=0;i<m_aAtoms.size();i++)
    {
        iBoxX = (int)((((*this)[i].x() - fMinX) / fCutoff) + 0.5f);
        iBoxY = (int)((((*this)[i].y() - fMinY) / fCutoff) + 0.5f);
        iBoxZ = (int)((((*this)[i].z() - fMinZ) / fCutoff) + 0.5f);
        iBoxIndex = iBoxX + (iBoxY * iBoxesX) + (iBoxZ * iBoxesXY);
	aBox[iBoxIndex].push_back(i);
    }

    // now look for neighbors in these cubes
    vector< vector<unsigned int > > aNeighbor( m_aAtoms.size() );
    unsigned int iIndex, iIndexNext, iAtom, iNumElements;
    unsigned int j,x,y,z;

    try
    {
        if (bShowProgress)
            svt_exception::ui()->progressPopup("Determination of covalent bonds", 0, iBoxesZ-1);

        for(z=0;z<iBoxesZ;z++)
        {
            for(y=0;y<iBoxesY;y++)
                for(x=0;x<iBoxesX;x++)
                {
                    // calculate the index of the current block
                    iIndex = x+(y*iBoxesX)+(z*iBoxesXY);
                    // loop over all atoms in that block
                    for(i=0;i<aBox[iIndex].size();i++)
                    {
                        iAtom = aBox[iIndex][i];

                        // search in same block for neighbors
                        for(j=i+1;j<aBox[iIndex].size();j++)
                        {
                            if ((*this)[iAtom].distanceSq( (*this)[aBox[iIndex][j]] ) > fCutoffSq)
                                continue;
                            aNeighbor[iAtom].push_back( aBox[iIndex][j] );
                        }
                        // search for neighbors in block x+1
                        if (x < iBoxesX-1)
                        {
                            iIndexNext = (x+1)+(y*iBoxesX)+(z*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ((*this)[iAtom].distanceSq( (*this)[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iAtom].push_back( aBox[iIndexNext][j] );
                            }
                        }
                        // search for neighbors in block y+1
                        if (y < iBoxesY-1)
                        {
                            iIndexNext = x+((y+1)*iBoxesX)+(z*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( (*this)[iAtom].distanceSq( (*this)[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iAtom].push_back( aBox[iIndexNext][j] );
                            }
                        }
                        // search for neighbors in block z+1 block
                        if (z < iBoxesZ-1)
                        {
                            iIndexNext = x+(y*iBoxesX)+((z+1)*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( (*this)[iAtom].distanceSq( (*this)[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iAtom].push_back( aBox[iIndexNext][j] );
                            }
                        }
                        // search for neighbors in block x+1 and y+1
                        if (x < iBoxesX-1 && y < iBoxesY-1)
                        {
                            iIndexNext = (x+1)+((y+1)*iBoxesX)+(z*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( (*this)[iAtom].distanceSq( (*this)[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iAtom].push_back( aBox[iIndexNext][j] );
                            }
                        }
                        // search for neighbors in block x+1 and z+1
                        if (x < iBoxesX-1 && z < iBoxesZ-1)
                        {
                            iIndexNext = (x+1)+(y*iBoxesX)+((z+1)*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( (*this)[iAtom].distanceSq( (*this)[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iAtom].push_back( aBox[iIndexNext][j] );
                            }
                        }
                        // search for neighbors in block y+1 and z+1
                        if (y < iBoxesY-1 && z < iBoxesZ-1)
                        {
                            iIndexNext = x+((y+1)*iBoxesX)+((z+1)*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( (*this)[iAtom].distanceSq( (*this)[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iAtom].push_back( aBox[iIndexNext][j] );
                            }
                        }
                        // search for neighbors in block x+1, y-1
                        if (x < iBoxesX-1 && y > 0)
                        {
                            iIndexNext = (x+1)+((y-1)*iBoxesX)+(z*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( (*this)[iAtom].distanceSq( (*this)[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iAtom].push_back( aBox[iIndexNext][j] );
                            }
                        }

                        // search for neighbors in block x-1, z+1
                        if (x > 0 && z < iBoxesZ-1)
                        {
                            iIndexNext = (x-1)+(y*iBoxesX)+((z+1)*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( (*this)[iAtom].distanceSq( (*this)[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iAtom].push_back( aBox[iIndexNext][j] );
                            }
                        }
                        // search for neighbors in block z+1, y-1
                        if (y > 0 && z < iBoxesZ-1)
                        {
                            iIndexNext = x+((y-1)*iBoxesX)+((z+1)*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( (*this)[iAtom].distanceSq( (*this)[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iAtom].push_back( aBox[iIndexNext][j] );
                            }
                        }
                        // search for neighbors in block z+1, y+1, x+1 block
                        if (x < iBoxesX-1 && y < iBoxesY-1 && z < iBoxesZ-1)
                        {
                            iIndexNext = (x+1)+((y+1)*iBoxesX)+((z+1)*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( (*this)[iAtom].distanceSq( (*this)[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iAtom].push_back( aBox[iIndexNext][j] );
                            }
                        }
                        // search for neighbors in block x-1, y+1, z+1
                        if (x > 0 && y < iBoxesY-1 && z < iBoxesZ-1)
                        {
                            iIndexNext = (x-1)+((y+1)*iBoxesX)+((z+1)*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( (*this)[iAtom].distanceSq( (*this)[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iAtom].push_back( aBox[iIndexNext][j] );
                            }
                        }
                        // search for neighbors in block x+1, y-1, z+1
                        if (x < iBoxesX-1 && y > 0 && z < iBoxesZ-1)
                        {
                            iIndexNext = (x+1)+((y-1)*iBoxesX)+((z+1)*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( (*this)[iAtom].distanceSq( (*this)[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iAtom].push_back( aBox[iIndexNext][j] );
                            }
                        }
                        // search for neighbors in block x-1, y-1, z+1
                        if (x > 0 && y > 0 && z < iBoxesZ-1)
                        {
                            iIndexNext = (x-1)+((y-1)*iBoxesX)+((z+1)*iBoxesXY);
                            iNumElements = aBox[iIndexNext].size();
                            for(j=0;j<iNumElements;j++)
                            {
                                if ( (*this)[iAtom].distanceSq( (*this)[aBox[iIndexNext][j]] ) > fCutoffSq)
                                    continue;
                                aNeighbor[iAtom].push_back( aBox[iIndexNext][j] );
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
            svt_exception::ui()->progressPopup("Determination of covalent bonds", 0, m_aAtoms.size()-1);

        // now we have an neighbor array with all corresponding atom pairs within a certain distance. Now we must check which of these pairs really are bonds.
        Real32 fDist, fCut;
        int k, iNeighborNum;
        for(i=0;i<m_aAtoms.size();i++)
        {
            iNeighborNum = aNeighbor[i].size();

            for(k=0;k<iNeighborNum;k++)
            {
                j = aNeighbor[i][k];

                if (m_aAtoms[i].getModel() == m_aAtoms[j].getModel())
                {
                    fDist = (*this)[i].distance( (*this)[j] );
                    fCut = (m_aAtoms[j].getVDWRadius() + m_aAtoms[i].getVDWRadius()) * 0.6f;

                    if ((!m_aAtoms[i].isHydrogen() || !m_aAtoms[j].isHydrogen()) && (!m_aAtoms[i].isQPDB() && !m_aAtoms[j].isQPDB()))
                    {
                        if (fDist < fCut)
                            addBond( i, j );
                    }
                }
            }

            if (i%1000 == 0)
                if (bShowProgress)
                    svt_exception::ui()->progress(i, m_aAtoms.size()-1);
        }

    } catch( svt_userInterrupt&) { }

    if (bShowProgress)
        svt_exception::ui()->progressPopdown();

    svtout << "Bond calculation found " << m_aBonds.size() << " covalent bonds" << endl;
}


/**
 * Calculates and returns the Carbon Alpha (CA) of the atom's residue
 * \param iIndexAtom an integer representing the atom's index in the pdb structure
 * \return an svt_point_cloud_atom - the Carbon Alpha
 */
template<class T>
unsigned int svt_point_cloud_pdb<T>::getCA(unsigned int iIndexAtom)
{
    /*
    cout << "Atom " << iIndexAtom << " " << m_aAtoms[iIndexAtom].getName();
    cout << " :Res " << m_aAtoms[iIndexAtom].getResname() << " " << m_aAtoms[iIndexAtom].getResidueSeq();
    cout << " IsCa:" << m_aAtoms[iIndexAtom].isCA() <<" ";
    */

    if (m_aAtoms[iIndexAtom].isCA()) return iIndexAtom;

    int iIndex = iIndexAtom-1;
    while( (iIndex >= 0) && (m_aAtoms[iIndex].getResidueSeq() == m_aAtoms[iIndexAtom].getResidueSeq()) &&
	    (m_aAtoms[iIndex].isCA() == 0)) {   iIndex--;}
    if (iIndex>=0 && m_aAtoms[iIndex].getResidueSeq() == m_aAtoms[iIndexAtom].getResidueSeq() && m_aAtoms[iIndex].isCA()==1) return iIndex;

    iIndex = iIndexAtom;
    while(iIndex < (int)(m_aAtoms.size()) &&
	    m_aAtoms[iIndex].getResidueSeq() == m_aAtoms[iIndexAtom].getResidueSeq() &&
	    m_aAtoms[iIndex].isCA() == 0){iIndex++;}

    if (iIndex <(int)(m_aAtoms.size()) && m_aAtoms[iIndex].getResidueSeq() == m_aAtoms[iIndexAtom].getResidueSeq() && m_aAtoms[iIndex].isCA()==1)	return iIndex;

    svt_exception("Error: Residue without CA");
    return iIndexAtom;
}

/**
 * Determine it iIndexAtom is on the backbone
 * \param iIndexAtom an integer representing the atom's index in the pdb structure
 * \return unsigned int - iIndexAtom if the atom is on the backbone or the CA if not
 */
template<class T>
unsigned int svt_point_cloud_pdb<T>::getBackbone(unsigned int iIndexAtom)
{
    if (m_aAtoms[iIndexAtom].isBackbone())
	return iIndexAtom;
    else
	return getCA(iIndexAtom);

}

/**
 * Compute the Distances between any atom of the structure using the topology - graph connections given in the PSF file
 */
template<class T>
bool svt_point_cloud_pdb<T>::computePSFGraphDistMat()
{
    // check if the psf file has been read
    if (m_aBonds.size()==0)
    {
        svt_exception::ui()->info("Can not compute bonds graph matrix because no bonds/connections have been created/loaded!");
	return false;
    }

    //scale matrix
    m_oGraphDists.resize(m_aAtoms.size(),m_aAtoms.size());

     // init the matrix
    for (unsigned int iIndexAtom1 = 0; iIndexAtom1 < m_aAtoms.size(); iIndexAtom1++)
	for (unsigned int iIndexAtom2 = 0; iIndexAtom2 < m_aAtoms.size(); iIndexAtom2++)
	    m_oGraphDists[iIndexAtom1][iIndexAtom2] = (iIndexAtom1 == iIndexAtom2) ? 0.0 : 1000000000;//numeric_limits<Real64>::max();

    Real64 fDistTmp;
    for (unsigned int iIndexBond = 0; iIndexBond < m_aBonds.size(); iIndexBond++){
	// in psf the atoms are numbered from 1 therefor getIndex -1
	fDistTmp = (*this)[m_aBonds[iIndexBond].getIndexA()].distance((*this)[m_aBonds[iIndexBond].getIndexB()]);

	m_oGraphDists[m_aBonds[iIndexBond].getIndexA()][m_aBonds[iIndexBond].getIndexB()] = fDistTmp;
	m_oGraphDists[m_aBonds[iIndexBond].getIndexB()][m_aBonds[iIndexBond].getIndexA()] = fDistTmp;
    }

    //Floyd's Algorithm
    for (unsigned int iIndexAtom3 = 0; iIndexAtom3 < m_aAtoms.size(); iIndexAtom3++)
	for (unsigned int iIndexAtom1 = 0; iIndexAtom1 < m_aAtoms.size(); iIndexAtom1++)
	    for (unsigned int iIndexAtom2 = 0; iIndexAtom2 < m_aAtoms.size(); iIndexAtom2++)
		if (m_oGraphDists[iIndexAtom1][iIndexAtom2] > m_oGraphDists[iIndexAtom1][iIndexAtom3] + m_oGraphDists[iIndexAtom3][iIndexAtom2])  m_oGraphDists[iIndexAtom1][iIndexAtom2]  =  m_oGraphDists[iIndexAtom1][iIndexAtom3] + m_oGraphDists[iIndexAtom3][iIndexAtom2];

    return true;
}

/**
 * \param bPSFRead a bool indicating if the psf files has been read;
 */
template<class T>
void svt_point_cloud_pdb<T>::setPSFRead(bool bPSFRead){
    m_bPSFRead = bPSFRead;
}
/**
 * get the symmetry Matrix transformations
 */
template<class T>
vector<svt_matrix4<Real64> >& svt_point_cloud_pdb<T>::getSymmMats()
{
  return m_aSymmMats;
};

template <class T>
void svt_point_cloud_pdb<T>::selectAtomResidueSeq( int iResidueSeq )
{

    (*this).resetAtomEnum();
    int iAtom = (*this).enumAtomResidueSeq( iResidueSeq );

    while (iAtom != -1)
    {
        (*this).getAtom(iAtom)->setSelected( true );
        iAtom = (*this).enumAtomResidueSeq( iResidueSeq  );
    }

};

/**
 * get the index of the selected/deselected atoms
 * \param bIsSelected the selection status = true - is selected; false  - is not selected
 * \return the index of the atoms with selected status indicated by bIsSelected
 */
template <class T>
vector<unsigned int> svt_point_cloud_pdb<T>::getSelection(bool bIsSelected)
{
    vector<unsigned int> aVec;
    for (unsigned int iIndex=0;iIndex < (*this).size(); iIndex++)
    {
        if ( (*this).getAtom(iIndex)->getSelected() == bIsSelected )
            aVec.push_back(iIndex);
    }
    return aVec;
};

/**
 * Calculate the Pair Distribution Function of a part A of the molecule to another part B
 * \param fWidth is width of a bin (held constant between the range Dmin and Dmax)
 * \param bIsSelectedA represent the selection status of the part A (if true ~ the selected atoms are used)
 * \param bIsSelectedB represent the selection status of the part B (if true ~ the selected atoms are used)
 * \return the computed pair Distribution
 */
template<class T> svt_pair_distribution svt_point_cloud_pdb<T>::getSelectionPDF(Real64 fWidth, bool bIsSelectedA, bool bIsSelectedB)
{
    svt_pair_distribution oPDF;
    oPDF.setWidth( fWidth );
    oPDF.setMin( 0 );

    vector<unsigned int> aAtomIndexA, aAtomIndexB;
    unsigned int iSizeBar=0;

    if (bIsSelectedA)
        aAtomIndexA = getSelection( true );
    else
        aAtomIndexA = getSelection( false );

    if (bIsSelectedA==bIsSelectedB)
    {
        aAtomIndexB = aAtomIndexA;
        iSizeBar = (aAtomIndexB.size()-1)*((aAtomIndexB.size()-1))/2;
        if (iSizeBar < 1000000)
            iSizeBar=0;
    }
    else
    {
        if (bIsSelectedB)
            aAtomIndexB = getSelection( true );
        else
            aAtomIndexB = getSelection( false );
    }

    try
    {
        unsigned int iCount=0;
        if (iSizeBar>0)
            svt_exception::ui()->busyPopup("Compute PDF");

        for (unsigned int iIndex1=0; iIndex1 < aAtomIndexA.size(); iIndex1++)
        {
            for (unsigned int iIndex2=0; iIndex2 < aAtomIndexB.size(); iIndex2++)
            {
                if (aAtomIndexA[iIndex1]>aAtomIndexB[iIndex2])
                {
                    iCount++;
                    oPDF.addPairDistance( (*this)[aAtomIndexA[iIndex1]].distance( (*this)[ aAtomIndexB[iIndex2] ] ) );
                    if (iSizeBar>0 && iCount % 100000 == 0)
                        svt_exception::ui()->busyRotate();;
                }
            }
        }
    }
    catch (svt_userInterrupt&)
    {
        oPDF.clear();
    }

    if (iSizeBar>0)
        svt_exception::ui()->busyPopdown();

    return oPDF;
};


/**
 * create symmetric oligomers
 * \param symmetry type
 * \param symmetry axis
 * \param order is equivalent with the number of monomers in the symmetric unit
 * \param fOffsetAxis1 is the offset form the first axis
 * \param fOffsetAxis2 is the offset form the second axis
 * \return pdb containing the symmetric oligomere
 */
template<class T>
svt_point_cloud_pdb<T> svt_point_cloud_pdb<T>::applySymmetry(unsigned int iOrder, const SymmetryType eType, char cAxis, Real64 fOffsetAxis1, Real64 fOffsetAxis2)
{
    svt_point_cloud_pdb<T> oPDB, oPDBMono;
    svt_point_cloud_atom* pAtom;
    Real64 fTheta, fCos, fSin;
    svt_vector4<Real64> oVec;
    svt_matrix4<Real64> oMat, oRot;
    char cChain[2];


    switch(eType)
    {
        case SYMMETRY_C:

            fTheta = 360.0/(Real64)iOrder;
            for (unsigned int iIndex=0; iIndex<iOrder; iIndex++)
            {
                oPDBMono.deleteAllAtoms();
                oPDBMono.deleteAllBonds();

                fCos = cos((Real64)iIndex*PI*fTheta/180.0);
                fSin = sin((Real64)iIndex*PI*fTheta/180.0);

                switch (cAxis)
                {
                    case 'x':
                        for (unsigned int iIndexAtom=0; iIndexAtom<(*this).size(); iIndexAtom++)
                        {
                            oVec.x( (*this)[iIndexAtom].x() );
                            oVec.y( fOffsetAxis1 + fCos*((*this)[iIndexAtom].y()-fOffsetAxis1) - fSin*((*this)[iIndexAtom].z()-fOffsetAxis2) );
                            oVec.z( fOffsetAxis2 + fSin*((*this)[iIndexAtom].y()-fOffsetAxis1) + fCos*((*this)[iIndexAtom].z()-fOffsetAxis2) );

                            oPDBMono.addAtom(*(*this).getAtom(iIndexAtom), oVec);

                            pAtom = oPDBMono.getAtom(oPDBMono.size()-1);
                            sprintf(cChain,"%d",iIndex);
                            pAtom->setChainID(cChain[0]);
                            //pAtom->setPDBIndex(iIndex*(*this).size()+ iIndexAtom);
                        }
                        break;
                    case 'y':
                        for (unsigned int iIndexAtom=0; iIndexAtom<(*this).size(); iIndexAtom++)
                        {
                            oVec.x( fOffsetAxis1 + fCos*((*this)[iIndexAtom].x()-fOffsetAxis1) + fSin*((*this)[iIndexAtom].z()-fOffsetAxis2) );
                            oVec.y( (*this)[iIndexAtom].y() );
                            oVec.z( fOffsetAxis2 - fSin*((*this)[iIndexAtom].x()-fOffsetAxis1) + fCos*((*this)[iIndexAtom].z()-fOffsetAxis2) );

                            oPDBMono.addAtom(*(*this).getAtom(iIndexAtom), oVec);

                            pAtom = oPDBMono.getAtom(oPDBMono.size()-1);
                            sprintf(cChain,"%d",iIndex);
                            pAtom->setChainID(cChain[0]);
                            //pAtom->setPDBIndex(iIndex*(*this).size()+ iIndexAtom);
                        }
                        break;
                    case 'z':
                        for (unsigned int iIndexAtom=0; iIndexAtom<(*this).size(); iIndexAtom++)
                        {
                            oVec.x( fOffsetAxis1 + fCos*((*this)[iIndexAtom].x()-fOffsetAxis1) - fSin*((*this)[iIndexAtom].y()-fOffsetAxis2) );
                            oVec.y( fOffsetAxis2 + fSin*((*this)[iIndexAtom].x()-fOffsetAxis1) + fCos*((*this)[iIndexAtom].y()-fOffsetAxis2) );
                            oVec.z( (*this)[iIndexAtom].z() );

                            oPDBMono.addAtom(*(*this).getAtom(iIndexAtom), oVec);

                            pAtom = oPDBMono.getAtom(oPDBMono.size()-1);
                            sprintf(cChain,"%d",iIndex);
                            pAtom->setChainID(cChain[0]);
                            //pAtom->setPDBIndex(iIndex*(*this).size()+ iIndexAtom);
                        }
                        break;
                }
                oPDB.append( oPDBMono );
            }

            break;
        case SYMMETRY_D:
        case SYMMETRY_H:
        default:
            svtout << "Symmetry type not yet available!" << endl;
            return (*this);
    }

    //recompute the models
    oPDB.calcAtomModels();
    
    return oPDB;
};


#endif

