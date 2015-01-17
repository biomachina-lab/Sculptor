/***************************************************************************
                          sculptor_lua.h
                          --------------
    begin                : 10/16/2009
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SCULPTOR_LUA_H
#define __SCULPTOR_LUA_H

#include <svt_lua.h>
#include <svt_matrix4.h>
#include <svt_pdb.h>
#include <svt_volume.h>
#include <svt_sphere.h>
#include <svt_arrow.h>
#include <svt_textnode.h>

#include <QSyntaxHighlighter>

class pdb_document;
class situs_document;
class sculptor_doc_marker;

/**
 * Lua interpreter class.
 * This class encapsulates the lua interpreter. It will create a new interpreter object and will execute code. One can query the content of Lua variables from C++.
 *
 * \author Stefan Birmanns
 */
class sculptor_lua : public svt_lua
{
public:

    /**
     * Constructor
     */
    sculptor_lua();

    /**
     * Destructor
     */
    virtual ~sculptor_lua();

    /**
     * Initialize interpreter and register all classes and functions
     */
    virtual void init();
};

///////////////////////////////////////////////////////////////////////////////
// LUA BINDINGS: SCULPTOR
///////////////////////////////////////////////////////////////////////////////

/**
 * Sculptor-bindings for lua
 */
class sculptor
{
public:

    /**
     * The sculptor class is supposed to encapsulate all general function calls and global settings for the sculptor application.
     */
    sculptor(lua_State *);
    ~sculptor();

    /**
     * Load a file.
     *
     * usage:
     * sculptor:load("test.pdb")
     */
    int load(lua_State *pState);

    /**
     * Save the sculptor state to a scl file.
     *
     * usage:
     * sculptor:save("current.scl")
     */
    int save(lua_State *pState);

    /**
     * Get the number of currently loaded documents.
     * Returns a number.
     *
     * usage:
     * num = sculptor:numDoc
     */
    int numDoc(lua_State* pState);

    /**
     * Get a document
     * Returns either a volume or molecule object
     *
     * usage:
     * mol = sculptor:getDoc( 2 )
     */
    int getDoc(lua_State* pState);

    /**
     * Delete a document.
     *
     * usage:
     * sculptor:delDoc( 2 )
     */
    int delDoc(lua_State* pState);

    /**
     * Delete all documents.
     *
     * usage:
     * sculptor:delAllDocs( )
     */
    int delAllDocs(lua_State* pState);

    /**
     * Get the sculptor version number.
     * Returns a string.
     *
     * usage:
     * print( sculptor:version() )
     */
    int version(lua_State *pState);

    /**
     * Print the current svt tree.
     *
     * usage:
     * sculptor:printTree()
     */
    int printTree(lua_State *pState);

    /**
     * Makes a certain document visible.
     * Parameter: Number of document.
     *
     * usage:
     * sculptor:showDoc(2)
     */
    int showDoc(lua_State *pState);

    /**
     * Hide a document.
     * Parameter: Number of document.
     *
     * usage:
     * sculptor:hideDoc(2)
     */
    int hideDoc(lua_State *pState);

    /**
     * Triggers a redraw of the 3D window.
     *
     * usage:
     * sculptor:redraw()
     */
    int redraw(lua_State *pState);

    /**
     * Lets sculptor sleep for a certain number of milliseconds.
     * Parameter: Number of milliseconds.
     *
     * usage:
     * sculptor:sleep(10)
     */
    int sleep(lua_State *pState);

    /**
     * Clear the log window
     *
     * usage:
     * sculptor:clearLog()
     */
    int clearLog(lua_State* pState);

    /**
     * Get the current directory where Sculptor operates
     *
     * usage:
     * sculptor:getCurrentDir()
     */
    int getCurrentDir(lua_State *pState);
    /**
     * Set the current directory where Sculptor operates
     *
     * usage:
     * sculptor:setCurrentDir("/tmp")
     */
    int setCurrentDir(lua_State *pState);
    /**
     * Save a screenshot to disk
     *
     * usage:
     * sculptor:saveScreenshot("scr.png")
     */
    int saveScreenshot(lua_State *pState);

    /**
     * Returns the global scene transformation matrix.
     * Returns matrix4 object.
     *
     * usage:
     * mat = sculptor:getTrans()
     * mat:print()
     */
    int getTrans(lua_State *pState);

    /**
     * Sets the global scene transformation matrix.
     * Parameter: matrix4 object.
     *
     * usage:
     * mat = sculptor:getTrans()
     * mat:rotate(2, 10)
     * sculptor:setTrans( mat )
     */
    int setTrans(lua_State *pState);

    /**
     * Closes sculptor. Hint: This function will still check for new documents that were created, but not saved yet and will prompt the user. In a test-script that might not
     * be desirable - just delAllDocs first to make sure that the quit() function will succeed without any user intervention.
     *
     * usage:
     * sculptor:quit( )
     */
    int quit(lua_State *pState);

    /**
     * Opens a file-open-dialog. Returns a string with a file name.
     * Parameter: Message
     *
     * usage:
     * file = sculptor:guiFileOpenDlg("Please open a file for loading")
     * sculptor:load( file )
     */
    int guiFileOpenDlg(lua_State *pState);
    /**
     * Opens a file-save-dialog. Returns a string with a file name. If user cancelled the dialog, the string will be empty.
     * Parameter: Message
     *
     * usage:
     * file = sculptor:guiFileSaveDlg("Please choose a filename for saving")
     */
    int guiFileSaveDlg(lua_State *pState);

    /**
     * Opens a warning-dialog.
     * Parameter: Message
     *
     * usage:
     * sculptor:guiWarning("Your model only has CA atoms...")
     */
    int guiWarning(lua_State *pState);

    /**
     * Opens an information-dialog.
     * Parameter: Message
     *
     * usage:
     * sculptor:guiInfo("Your model only has CA atoms...")
     */
    int guiInfo(lua_State *pState);

    /**
     * Opens a yes/no question-dialog.
     * Parameter: Message
     *
     * usage:
     * answer = sculptor:guiYesNo("Do you really want to quit?")
     */
    int guiYesNo(lua_State *pState);

    /**
     * Opens a free-text question-dialog.
     * Parameter: Message
     *
     * usage:
     * answer = sculptor:guiQuestion("Name of the author?")
     */
    int guiQuestion(lua_State *pState);

    /**
     * Opens a value question-dialog.
     * Parameter: Message
     *
     * usage:
     * answer = sculptor:guiGetValue("Resolution of the map?")
     */
    int guiGetValue(lua_State *pState);
    
    /**
    * show the GA dialog
    */
    int runGa(lua_State *L);

    /**
     * Compute the powell off-lattice refinement against a volume object
     * Parameter: volume object. Boolean parameter: If true only a spar
     * parameter: an array of molecules
     *
     * usage:
     * sculptor:powell(pdb_array, vol, true, res)
     */
    int powell(lua_State* pState);

    /**
     * Run command line
     * Parameter: a string representing the command line
     *
     * usage:
     * sculptor:system(Commandline)
     */
    int system(lua_State* pState);
     
    /**
     * Add a graphics primitive object (e.g. a sphere) to the 3D scene
     * Parameter: graphics object.
     *
     * usage:
     * sculptor:addGraphics( sphere:new( 1, 2, 1 ) )
     */
    int addGraphics(lua_State *pState);

    /**
     * Get a graphics primitive object that was added previously to the scene
     * Parameter: Index of graphics object.
     * Returns: Graphics object
     *
     * usage:
     * sphere = sculptor:getGraphics( 1 )
     */
    int getGraphics(lua_State *pState);

    /**
     * Delete a graphics primitive object that was added previously to the scene
     * Parameter: Index of graphics object.
     *
     * usage:
     * sculptor:delGraphics( 1 )
     */
    int delGraphics(lua_State *pState);
    /**
     * Delete all graphics primitive objects that were added previously to the scene
     *
     * usage:
     * sculptor:delAllGraphics( )
     */
    int delAllGraphics(lua_State *pState);

    /**
     * Print memory table
     *
     * usage:
     * sculptor:printMemory()
     */
    int printMemory(lua_State *);

    static int getDisplayModeEnum(lua_State *L);
    static void registerDisplayModeEnum(lua_State *L);
    static int getColmapModeEnum(lua_State *L, string oStr);
    static void registerColmapModeEnum(lua_State *L);
    static const char className[];
    static const svt_luna<sculptor>::RegType methods[];
    
    
};

///////////////////////////////////////////////////////////////////////////////
// LUA BINDINGS: ATOM
///////////////////////////////////////////////////////////////////////////////

/**
 * Sculptor-bindings for lua
 */
class atom
{
protected:

    svt_point_cloud_atom m_oAtom;
    svt_vector4<Real64>  m_oPos;
    int m_iIndex;
    int m_iDisplayMode;

public:

    /**
     * The atom class encapsulates a single atom
     */
    atom(lua_State *);
    ~atom();

    /**
     * Set the atom and position information
     */
    void set( svt_point_cloud_atom oAtom, svt_vector4<Real64> oPos, int iIndex );
    /**
     * Get the atom
     */
    svt_point_cloud_atom* get();
    /**
     * Get the position
     */
    svt_vector4<Real64> getPoint( );

    /**
     * Get displaymode.
     */
    int getDisplayMode();
    /**
     * Set displaymode.
     */
    void setDisplayMode(int iDisplayMode);

    /**
     * Get the svt index.
     * \return the svt index. If -1, the atom is new and is not coming from a svt_point_cloud_pdb object!
     */
    int getIndex();

    /**
     * Get displaymode.
     */
    int getDisplayMode(lua_State* pState);
    /**
     * Set displaymode.
     */
    int setDisplayMode(lua_State* pState);

    /**
     * Set/get the x coordinate of the position of the atom
     * If no parameter is given, the function just returns a number, otherwise it will set the x coordinate using the parameter provided by the user.
     *
     * usage:
     * atm_a:x( 15.0 )
     * print( atm_a:x() )
     */
    int x(lua_State* pState);
    /**
     * Set/get the y coordinate of the position of the atom
     * If no parameter is given, the function just returns a number, otherwise it will set the y coordinate using the parameter provided by the user.
     *
     * usage:
     * atm_a:y( 15.0 )
     * print( atm_a:y() )
     */
    int y(lua_State* pState);
    /**
     * Set/get the z coordinate of the position of the atom
     * If no parameter is given, the function just returns a number, otherwise it will set the z coordinate using the parameter provided by the user.
     *
     * usage:
     * atm_a:z( 15.0 )
     * print( atm_a:z() )
     */
    int z(lua_State* pState);

    /**
     * Get the position of the atom.
     * Returns: Numbers (x,y,z)
     *
     * usage:
     * x,y,z = atm:getPos( )
     */
    int getPos(lua_State* pState);
    /**
     * Set the position of the atom.
     * Parameters: Numbers (x,y,z)
     *
     * usage:
     * atm:setPos( x,y,z )
     */
    int setPos(lua_State* pState);

    /**
     * Print content to stdout
     */
    int print(lua_State *pState);

    /**
     * Measure distance to another atom.
     * Parameter: Atom object.
     * Returns: Number.
     *
     * usage:
     * dist = mol:getAtom(123):distance( mol:getAtom(124) )
     */
    int distance(lua_State *);

    static const char className[];
    static const svt_luna<atom>::RegType methods[];
};

///////////////////////////////////////////////////////////////////////////////
// LUA BINDINGS: MOLECULE
///////////////////////////////////////////////////////////////////////////////

/**
 * Sculptor-bindings for lua
 */
class molecule
{
protected:

    svt_pdb* m_pPDB;
    pdb_document* m_pDoc;
    bool m_bDelete;

public:

    /**
     * The molecule class encapsulates a single molecule, atomic model document
     */
    molecule(lua_State *);
    ~molecule();

        
    /** This function uses fetchPDB to get PDB files loaded to sculptor window
     *  This function can handle variable number of arguments
     *  Usage:
     *  fetchPDBs("mol1.pdb", "mol2.pdb", "mol3.pdb")
     */
     int fetchPDBs(lua_State* pState);
     
    /**
     * Set the svt molecular data that is encapsulated by this object
     * \param pointer to svt_pdb object
     * \param boolean, if true, the data gets deleted if this object here gets deleted
     */
    void set( svt_pdb* pPDB, bool bDelete = false );
    /**
     * Set the sculptor document that is encapsulated by this object
     */
    void setDoc( pdb_document* pDoc);
    /**
     * Add the molecule to the list of documents.
     * Parameter: String with the name
     *
     * Usage:
     * addDoc( "test.pdb" )
     */
    int addDoc(lua_State* pState);

    /**
     * Get the svt molecular data that is encapsulated by this object
     */
    svt_pdb* get( );
    /**
     * Get the sculptor document that is encapsulated by this object
     */
    pdb_document* getDoc( );

    /**
     * Local -> global transformation.
     */
    svt_vector4<Real64> local2Global( svt_vector4<Real64> oVec );
    /**
     * Global->local transformation.
     */
    svt_vector4<Real64> global2Local( svt_vector4<Real64> oVec );

    /**
     * Check if the document is present. If the molecule is a local molecule, created in Lua, it will not be part of the document list and will not be rendered. All rendering
     * functions should therefore call this function here to make sure that everything is setup correctly.
     */
    bool checkDoc();

    /**
     * Check if data is present.
     */
    bool checkData();

    /**
     * Get an atom from the molecule.
     * Parameter: Index.
     *
     * usage:
     * atm = mol:getAtom( 123 )
     */
    int getAtom(lua_State* pState);
    /**
     * Set an atom in the molecule. The atom contains the index, so it knows where it was retrieved from and will go back to exactly the same position. If a new atom is supposed to be stored in the
     * molecule, please use addAtom.
     * Parameter: Atom
     *
     * usage:
     * atm = mol:getAtom( 123 )
     * atm.x( 10.5 )
     * mol:setAtom( atm )
     */
    int setAtom(lua_State* pState);
    /**
     * Get a series of atoms from the molecule.
     * Parameter: Two indices
     *
     * usage:
     * atm = mol:getAtoms( 123, 135 )
     */
    int getAtoms(lua_State* pState);
    /**
     * Set an array of atoms in the molecule. The atoms contain the index, so they know where they were retrieved from and will go back to exactly the same position. If a new atom is supposed to be stored in the
     * molecule, please use addAtom.
     * Parameter: Array of atoms
     *
     * usage:
     * atm = mol:getAtoms( 123, 135 )
     * atm[5]:x( 10.5 )
     * mol:setAtoms( atm )
     */
    int setAtoms(lua_State* pState);

    /**
     * Add an atom to the molecule. The atom is stored as a new atom in the molecule, will therefore get a new svt index.
     * Parameter: Atom
     *
     * usage:
     * atm = mol:getAtom( 123 )
     * atm:x( 10.5 )
     * mol:addAtom( atm )
     */
    int addAtom(lua_State* pState);
    /**
     * Add an array of atoms to the molecule. The atoms are stored as new atoms in the molecule, and will therefore get a new svt index.
     * Parameter: Array of atoms.
     *
     * usage:
     * atm = mol:getAtoms( 123, 135 )
     * atm[1]:x( 10.5 )
     * mol:addAtoms( atm )
     */
    int addAtoms(lua_State* pState);

    /**
     * Get the number of atoms in the molecule
     * Returns a number.
     */
    int size(lua_State* pState);

    /**
     * Compute the rmsd with another molecule object.
     * Parameter: molecule object.
     * Parameter: boolean: align the two structures before rmsd (true/false).
     *
     * usage:
     * rmsd = mol:rmsd(mol2, false)
     */
    int rmsd(lua_State* pState);

    /**
     * Compute the powell off-lattice refinement against a volume object
     * Parameter: volume object.
     *
     * usage:
     * mol:powell(vol)
     */
    int powell(lua_State* pState);

    /**
     * Blur a molecule
     * Parameter: voxelwidth, resolution
     * Returns: a volume
     *
     * usage:
     * vol = mol:blur( 3.0, 10.0 )
     */
    int blur(lua_State* pState);

    /**
     * Returns the transformation matrix of the svt object.
     * Returns matrix4 object.
     *
     * usage:
     * mat = mol:getTrans()
     * mat:print()
     */
    int getTrans(lua_State *pState);

    /**
     * Sets the transformation matrix of the svt object.
     * Parameter: matrix4 object.
     *
     * usage:
     * mat = mol:getTrans()
     * mat:rotate(2, 10)
     * mol:setTrans( mat )
     */
    int setTrans(lua_State *pState);

    /**
     * Update the rendering of the molecule. Should be called if the data was manipulated internally.
     *
     * usage:
     * mol:updateRendering()
     */
    int updateRendering(lua_State*);

    /**
     * Save the molecule to disk.
     * Parameters: Filename
     *
     * usage:
     * mol:save("test.pdb")
     */
    int save(lua_State* pState);
    /**
     * Load an atomic model from disk. The molecule will not get added to the document list of Sculptor (can be done later using the addDoc function). Typically one would prefer to use
     * the sculptor:load() function.
     * Parameters: Filename
     *
     * usage:
     * mol = molecule:new()
     * mol:load("test.pdb")
     */
    int load(lua_State* pState);

    /**
     * Create codebook/feature vectors using the neural gas / TRN algorithm
     *
     * usage:
     * mol:vectorquant( 5 )
     */
    int vectorquant(lua_State* pState);

    /**
     * Set displaymode.
     *
     * usage:
     * mol:setDisplayMode("CARTOON")
     */
    int setDisplayMode(lua_State* pState);

    /**
     * Set colmapmode.
     *
     * usage:
     * mol:setColmapMode("SOLID", 5)
     */
    int setColmapMode(lua_State* pState);

    /**
     * ProjectMassCorr. This routine only projects the atoms onto the volume object and calculates the correlation - it will not convolute the molecule with a Gaussian to bring it
     * to the same resolution as the volumetric data.
     *
     * usage:
     * cc = mol:projectMassCorr( volume )
     */
    int projectMassCorr(lua_State* pState);

    /**
     * Get secondary structure information from atom i.
     * Parameter: Index.
     * Returns: String.
     *
     * usage:
     * ss = mol:getAtomSecStruct( 123 )
     */
    int getAtomSecStruct(lua_State* pState);

    /**
     * Get atom type information for atom i
     * Parameter: Index.
     * Returns: String.
     *
     * usage:
     * type = mol:getAtomType( 123 )
     */
    int getAtomType(lua_State* pState);
    /**
     * Set atom type information for atom i
     * Parameter: Index, string with type.
     *
     * usage:
     * mol:setAtomType( 123, "H" )
     */
    int setAtomType(lua_State* pState);

    /**
     * Get atom model information for atom i
     * Parameter: Index.
     * Returns: Integer.
     *
     * usage:
     * model = mol:getAtomModel( 123 )
     */
    int getAtomModel(lua_State* pState);
    /**
     * Set atom model information for atom i
     * Parameter: Index, integer with the new model identifier
     *
     * usage:
     * mol:setAtomModel( 123, 3 )
     */
    int setAtomModel(lua_State* pState);

    /**
     * Computes the array of different models
     *
     * usage:
     * mol:calcAtomModels()
     */
    int calcAtomModels(lua_State* pState);    

    /**
     * Get remoteness information from atom i (e.g. alpha for a carbon alpha atom).
     * Parameter: Index.
     * Returns: String.
     *
     * usage:
     * rem = mol:getAtomRemoteness( 123 )
     */
    int getAtomRemoteness(lua_State* pState);
    /**
     * Set remoteness information from atom i (e.g. alpha for a carbon alpha atom).
     * Parameter: Index.
     *
     * usage:
     * mol:setAtomRemoteness( 123 )
     */
    int setAtomRemoteness(lua_State* pState);

    /**
     * Get branch information of atom i.
     * Parameter: Index.
     * Returns: String.
     *
     * usage:
     * branch = mol:getAtomBranch( 123 )
     */
    int getAtomBranch(lua_State* pState);
    /**
     * Set branch information of atom i.
     * Parameter: Index.
     *
     * usage:
     * mol:setAtomBranch( 123 )
     */
    int setAtomBranch(lua_State* pState);

    /**
     * Get alternate location indicator information of atom i.
     * Parameter: Index.
     * Returns: String.
     *
     * usage:
     * alt = mol:getAtomAltLoc( 123 )
     */
    int getAtomAltLoc(lua_State* pState);
    /**
     * Set alternate location indicator information of atom i.
     * Parameter: Index.
     *
     * usage:
     * mol:setAtomAltLoc( 123 )
     */
    int setAtomAltLoc(lua_State* pState);

    /**
     * Get residue name information of atom i (e.g. ALA for alanin).
     * Parameter: Index.
     * Returns: String.
     *
     * usage:
     * res = mol:getAtomResName( 123 )
     */
    int getAtomResName(lua_State* pState);
    /**
     * Set residue name information of atom i (e.g. ALA for alanin).
     * Parameter: Index.
     *
     * usage:
     * mol:setAtomResName( 123 )
     */
    int setAtomResName(lua_State* pState);

    /**
     * Get residue number information of atom i.
     * Parameter: Index.
     * Returns: Integer.
     *
     * usage:
     * res = mol:getAtomResNum( 123 )
     */
    int getAtomResNum(lua_State* pState);
    /**
     * Set residue number information of atom i.
     * Parameter: Index.
     *
     * usage:
     * mol:setAtomResNum( 123, 123 )
     */
    int setAtomResNum(lua_State* pState);

    /**
     * Get chain id information of atom i (e.g. A, B, C, ...).
     * Parameter: Index.
     * Returns: String.
     *
     * usage:
     * chain = mol:getAtomChain( 123 )
     */
    int getAtomChain(lua_State* pState);
    /**
     * Set chain id information of atom i (e.g. A, B, C, ...).
     * Parameter: Index.
     *
     * usage:
     * mol:setAtomChain( 123, "A" )
     */
    int setAtomChain(lua_State* pState);

    /**
     * Get icode (insertion of residues) information of atom i.
     * Parameter: Index.
     * Returns: String.
     *
     * usage:
     * chain = mol:getAtomICode( 123 )
     */
    int getAtomICode(lua_State* pState);
    /**
     * Set icode (insertion of residues) information of atom i.
     * Parameter: Index.
     *
     * usage:
     * mol:setAtomICode( 123 )
     */
    int setAtomICode(lua_State* pState);

    /**
     * Get occupancy information of atom i.
     * Parameter: Index.
     * Returns: Number.
     *
     * usage:
     * occ = mol:getAtomOccupancy( 123 )
     */
    int getAtomOccupancy(lua_State* pState);
    /**
     * Set occupancy information of atom i.
     * Parameter: Index, Number.
     *
     * usage:
     * mol:setAtomOccupancy( 123, 1.0 )
     */
    int setAtomOccupancy(lua_State* pState);

    /**
     * Get temperature factor information of atom i.
     * Parameter: Index.
     * Returns: Number.
     *
     * usage:
     * temp = mol:getAtomTempFact( 123 )
     */
    int getAtomTempFact(lua_State* pState);
    /**
     * Set temperature factor information of atom i.
     * Parameter: Index, Number.
     *
     * usage:
     * mol:setAtomTempFact( 123, 1.0 )
     */
    int setAtomTempFact(lua_State* pState);

    /**
     * Get note of atom i.
     * Parameter: Index.
     * Returns: String.
     *
     * usage:
     * note = mol:getAtomNote( 123 )
     */
    int getAtomNote(lua_State* pState);
    /**
     * Set note of atom i (at least three characters long!)
     * Parameter: Index, String.
     *
     * usage:
     * mol:setAtomNote( 123, "ABC" )
     */
    int setAtomNote(lua_State* pState);

    /**
     * Get segment id of atom i.
     * Parameter: Index.
     * Returns: String.
     *
     * usage:
     * seg = mol:getAtomSegID( 123 )
     */
    int getAtomSegID(lua_State* pState);
    /**
     * Set segment id of atom i (at least four characters long!)
     * Parameter: Index, String.
     *
     * usage:
     * mol:setAtomSegID( 123, "ABCD" )
     */
    int setAtomSegID(lua_State* pState);

    /**
     * Get element information of atom i.
     * Parameter: Index.
     * Returns: String.
     *
     * usage:
     * element = mol:getAtomElement( 123 )
     */
    int getAtomElement(lua_State* pState);
    /**
     * Set element information of atom i (at least two characters!)
     * Parameter: Index, String.
     *
     * usage:
     * mol:setAtomElement( 123, " H" )
     */
    int setAtomElement(lua_State* pState);

    /**
     * Get charge of atom i.
     * Parameter: Index.
     * Returns: String.
     *
     * usage:
     * element = mol:getAtomCharge( 123 )
     */
    int getAtomCharge(lua_State* pState);
    /**
     * Set charge of atom i (two characters at least!).
     * Parameter: Index, String.
     *
     * usage:
     * mol:setAtomCharge( 123 )
     */
    int setAtomCharge(lua_State* pState);

    /**
     * Is atom i a hydrogen?
     * Parameter: Index.
     * Returns: Boolean.
     *
     * usage:
     * hydro = mol:isAtomHydrogen( 123 )
     */
    int isAtomHydrogen(lua_State* pState);

    /**
     * Is atom i a codebook vector?
     * Parameter: Index.
     * Returns: Boolean.
     *
     * usage:
     * hydro = mol:isAtomQPDB( 123 )
     */
    int isAtomQPDB(lua_State* pState);

    /**
     * Is atom i part of a water molecule?
     * Parameter: Index.
     * Returns: Boolean.
     *
     * usage:
     * hydro = mol:isAtomWater( 123 )
     */
    int isAtomWater(lua_State* pState);

    /**
     * Is atom i a carbon alpha?
     * Parameter: Index.
     * Returns: Boolean.
     *
     * usage:
     * hydro = mol:isAtomCA( 123 )
     */
    int isAtomCA(lua_State* pState);

    /**
     * Is atom i on the backbone?
     * Parameter: Index.
     * Returns: Boolean.
     *
     * usage:
     * hydro = mol:isAtomBackbone( 123 )
     */
    int isAtomBackbone(lua_State* pState);

    /**
     * Is atom i a nucleotide?
     * Parameter: Index.
     * Returns: Boolean.
     *
     * usage:
     * hydro = mol:isAtomNucleotide( 123 )
     */
    int isAtomNucleotide(lua_State* pState);

    /**
     * Get mass of atom i.
     * Parameter: Index.
     * Returns: Number.
     *
     * usage:
     * mass = mol:getAtomMass( 123 )
     */
    int getAtomMass(lua_State* pState);
    /**
     * Set mass of atom i.
     * Parameter: Index, Number.
     *
     * usage:
     * mol:setAtomMass( 123, 1.2 )
     */
    int setAtomMass(lua_State* pState);
    /**
     * Adjust the atomic mass based on a (simple) periodic table.
     * Parameter: Index.
     *
     * usage:
     * mol:adjustAtomMass( 123 )
     */
    int adjustAtomMass(lua_State* pState);
    /**
     * Get vdw radius of atom i.
     * Parameter: Index.
     * Returns: Number.
     *
     * usage:
     * rad = mol:getAtomVDWRadius( 123 )
     */
    int getAtomVDWRadius(lua_State* pState);

    /**
     * Get a model from the molecule.
     * Parameter: Modelnumber.
     * Returns: Molecule object.
     *
     * usage:
     * mol_b = mol:getModel( 1 )
     */
    int getModel(lua_State* pState);
    /**
     * Get a chain from the molecule.
     * Parameter: Chain ID.
     * Returns: Molecule object.
     *
     * usage:
     * mol_b = mol:getChain( "A" )
     */
    int getChain(lua_State* pState);

    /**
     * Add another molecule.
     * Parameter: Molecule object.
     *
     * usage:
     * mol_a:add( mol_b )
     */
    int add(lua_State* pState);

    /**
     * Add a bond between two atoms.
     * Parameter: Index a and index b of the two atoms.
     *
     * usage:
     * mol:addBond( 5, 10 )
     */
    int addBond(lua_State* pState);
    /**
     * Is there a bond between two atoms?
     * Parameter: Index a and index b of the two atoms.
     *
     * usage:
     * bond = mol:isBond( 5, 10 )
     */
    int isBond(lua_State* pState);
    /**
     * Remove a bond between two atoms.
     * Parameter: Index a and index b of the two atoms.
     *
     * usage:
     * mol:delBond( 5, 10 )
     */
    int delBond(lua_State* pState);
    /**
     * Remove all atoms.
     *
     * usage:
     * mol:delAllAtoms( )
     */
    int delAllAtoms(lua_State* pState);
    /**
     * Remove all bonds.
     *
     * usage:
     * mol:delAllBonds( )
     */
    int delAllBonds(lua_State* pState);

    /**
     * Match two point-clouds. This function should not be used directly on atomic models, but only on feature-point-clouds. First use vector-quantization to extract a small number of feature
     * points from both data sets / models and then match those feature-points. The resulting transformation matrix can then be applied to the original atomic models. Returns an array of matrices, with the
     * first matrix representing the most likely match.
     *
     * usage:
     * mats = mol_a:match( mol_b )
     * realmol_a.setTrans( mats[1] )
     */
    int match(lua_State* pState);

    /**
     * Flex a molecule according to the coordinates of two feature-point sets. The first set describes the molecule, the other one the target configuration (for example from an
     * em map).
     * Parameters: Two molecule objects
     * Returns: New molecule object
     *
     * usage:
     * flex = mol:flexing( trn_a, trn_b )
     */
    int flexing(lua_State* pState);

    static const char className[];
    static const svt_luna<molecule>::RegType methods[];
};

///////////////////////////////////////////////////////////////////////////////
// LUA BINDINGS: VOLUME
///////////////////////////////////////////////////////////////////////////////

/**
 * Sculptor-bindings for lua
 */
class volume
{
protected:

    svt_volume<Real64>* m_pVol;
    situs_document* m_pDoc;
    bool m_bDelete;

public:

    /**
     * The volume class encapsulates a single volumetric data set
     */
    volume(lua_State *);
    ~volume();

    /**
     * Allocate memory. Attention, the old content of the molecule will get erased! Typically one would rather use the constructor/new function
     * instead of allocate.
     * Parameter: x,y,z size
     *
     * usage:
     * vol = volume:new()
     * vol:allocate(10,8,12)
     * better:
     * vol = volume:new(10,8,12)
     */
    int allocate(lua_State* pState);

    /**
     * Set the svt volume data that is encapsulated by this object
     */
    void set( svt_volume<Real64>* pVol, bool bDelete = false);
    /**
     * Set the sculptor document that is encapsulated by this object
     */
    void setDoc( situs_document* pDoc);

    /**
     * Get the svt volume data that is encapsulated by this object
     */
    svt_volume<Real64>* get( );
    /**
     * Get the sculptor document that is encapsulated by this object
     */
    situs_document* getDoc();

    /**
     * Add the volume data set to the list of documents.
     * Parameter: String with the name
     *
     * Usage:
     * addDoc( "test.situs" )
     */
    int addDoc(lua_State* pState);

    /**
     * Check if the document is present. If the molecule is a local molecule, created in Lua, it will not be part of the document list and will not be rendered. All rendering
     * functions should therefore call this function here to make sure that everything is setup correctly.
     */
    bool checkDoc();

    /**
     * Check if data is present.
     */
    bool checkData();

    /**
     * Set the isosurface threshold level
     */
    int setIsoThreshold(lua_State* pState);

    /**
     * Set wireframe
     * Parameter: bool true/false
     *
     * usage:
     * vol:setWireframe( true )
     */
    int setWireframe(lua_State* pState);

    /**
     * Set color
     * Parameter: red, green, blue (0.0 - 1.0)
     *
     * usage:
     * vol:setColor( 1.0, 1.0, 1.0 )
     */
    int setColor(lua_State* pState);

    /**
     * Get the number of voxels in the volume
     * Returns a number.
     */
    int size(lua_State* pState);

    /**
     * Get a voxel value.
     * Parameter: x,y,z index
     *
     * usage:
     * voxel = vol:getValue(10,8,12)
     */
    int getValue(lua_State* pState);

    /**
     * Get a voxel value. This function takes a normal angstroem world coordinate and interpolates the voxel values tri-linearly.
     * Parameter: x,y,z angstroem coordinate
     *
     * usage:
     * voxel = vol:getIntValue(11.5,8.24,12.21)
     */
    int getIntValue(lua_State* pState);

    /**
     * Set a voxel value.
     * Parameter: x,y,z index
     * Parameter: value
     *
     * usage:
     * vol:setValue(10,8,12, 0.567)
     */
    int setValue(lua_State* pState);

    /**
     * Copies the current volume data set. It will not automatically get added to the list of loaded documents.
     *
     * usage:
     * volCopy = vol:copy()
     */
    int copy(lua_State* pState);

    /**
     * Crop the volume.
     * Parameters: min/max x, min/max y, min/max z
     *
     * usage:
     * vol:crop(2,12,5,24,3,34)
     */
    int crop(lua_State* pState);

    /**
     * Update the rendering of the volume data. Should be called if the data was manipulated internally.
     *
     * usage:
     * vol:updateRendering()
     */
    int updateRendering(lua_State* pState);

    /**
     * Save the volume to disk.
     * Parameters: Filename
     *
     * usage:
     * vol:save("test.situs")
     */
    int save(lua_State* pState);
    /**
     * Load a volume from disk. The volume will not get added to the document list of Sculptor (can be done later using the addDoc function). Typically one would prefer to use
     * the sculptor:load() function.
     * Parameters: Filename
     *
     * usage:
     * vol = volumw:new()
     * vol:load("test.situs")
     */
    int load(lua_State* pState);

    /**
     * Create codebook/feature vectors using the neural gas / TRN algorithm. Returns a new "molecule" with the feature vectors. Can start from an already existing configuration and
     * will in that case also return a matched feature point set (can be used to flex the molecule).
     *
     * usage:
     * vectors = vol:vectorquant(10)
     */
    int vectorquant(lua_State* pState);

    /**
     * Calculate the correlation with another volume object.
     * Parameters: Other volume object. The second parameter specifies if the correlation coefficient should only be computed under the current molecule (aka local correlation), or over the entire volume.
     *
     * usage:
     * cc = vol_a:correlation( vol_b )
     */
    int correlation(lua_State* pState);

    /**
     * Delete a spherical subregion
     * Parameters: center voxel coordinate for the spherical region x,y,z and radius of sphere
     *
     * usage:
     * vol:cutSphere( 10,12,8, 3.5 )
     */
    int cutSphere( lua_State* pState );

    /**
     * Threshold the volumetric map. All voxel below a certain value are set to 0.
     * Parameters: Threshold value
     *
     * usage:
     * vol:threshold( 3.5 )
     */
    int threshold( lua_State* pState );

    /**
     * Get the maximal voxel value in the map.
     *
     * usage:
     * max = vol:getMaxDensity( )
     */
    int getMaxDensity( lua_State* pState );
    /**
     * Get the minimal voxel value in the map.
     *
     * usage:
     * max = vol:getMinDensity( )
     */
    int getMinDensity( lua_State* pState );

    /**
     * Get the voxelwidth of the volume.
     *
     * usage:
     * vw = vol:getVoxelwidth( )
     */
    int getVoxelwidth( lua_State* pState );
    /**
     * Set the voxelwidth of the volume. This will not re-interpolate the map, but only set the internal voxelwidth variable! See interpolate function.
     *
     * usage:
     * vol:setVoxelwidth( 2.0 )
     */
    int setVoxelwidth( lua_State* pState );

    /**
     * Get the size of the volume in x dimension.
     *
     * usage:
     * sizex = vol:getSizeX( )
     */
    int getSizeX( lua_State* pState );
    /**
     * Get the size of the volume in y dimension.
     *
     * usage:
     * sizey = vol:getSizeY( )
     */
    int getSizeY( lua_State* pState );
    /**
     * Get the size of the volume in z dimension.
     *
     * usage:
     * sizez = vol:getSizeZ( )
     */
    int getSizeZ( lua_State* pState );

    /**
     * Normalize the volumetric map to [0..1].
     *
     * usage:
     * vol:normalize( )
     */
    int normalize( lua_State* pState );

    /**
     * Mask with another volume object - all the voxels in this vol are multiplied with the mask volume voxels (multiplied by 0 for not overlapping voxels).
     *
     * usage:
     * vol:mask( maskvol )
     */
    int mask( lua_State* pState );

    /**
     * Create a binary mask using a threshold value. Every voxel below the threshold will get set to 0, the rest to 1.
     *
     * usage:
     * vol:makeMask( 1.0 )
     */
    int makeMask( lua_State* pState );

    /**
     * Re-interpolate the map to a different voxelsize.
     *
     * usage:
     * vol:interpolate( 2.0 )
     */
    int interpolate( lua_State* pState );

    /**
     * Convolve map with another volume (kernel).
     *
     * usage:
     * vol:convolve( kernelvol )
     */
    int convolve( lua_State* pState );

    /**
     * Create a Gaussian kernel volume within SigmaFactor*fSigma.
     * Attention: This will overwrite the current content of the volume object with the filter kernel. It will allocate the memory internally.
     * Parameters: sigma of map and sigma factor
     *
     * usage:
     * kernel:createGaussian( sigma, sigmafactor )
     */
    int createGaussian( lua_State* pState );

    /**
     * Create a laplacian kernel volume (3x3x3).
     *
     * usage:
     * kernel:createLaplacian()
     */
    int createLaplacian( lua_State* pState );

    /**
     * Set/get the x coordinate of the position of the map
     * If no parameter is given, the function just returns a number, otherwise it will set the x coordinate using the parameter provided by the user.
     *
     * usage:
     * vol:x( 15.0 )
     * print( vol:x() )
     */
    int x(lua_State* pState);
    /**
     * Set/get the y coordinate of the position of the map
     * If no parameter is given, the function just returns a number, otherwise it will set the y coordinate using the parameter provided by the user.
     *
     * usage:
     * vol:y( 15.0 )
     * print( vol:y() )
     */
    int y(lua_State* pState);
    /**
     * Set/get the z coordinate of the position of the map
     * If no parameter is given, the function just returns a number, otherwise it will set the z coordinate using the parameter provided by the user.
     *
     * usage:
     * vol:z( 15.0 )
     * print( vol:z() )
     */
    int z(lua_State* pState);

    static const char className[];
    static const svt_luna<volume>::RegType methods[];
};

///////////////////////////////////////////////////////////////////////////////
// LUA BINDINGS: MATRIX4
///////////////////////////////////////////////////////////////////////////////

/**
 * Matrix class for Lua
 */
class matrix4
{
protected:

    svt_matrix4<Real64> m_oMatrix;

public:

    matrix4(lua_State *);
    ~matrix4();

    /**
     * Set Matrix
     * \param oMat matrix
     */
    void set( svt_matrix4<Real64> oMatrix );
    /**
     * Get Matrix
     * \return svt_matrix4 object
     */
    svt_matrix4<Real64> get();

    /**
     * Rotate around a single axis.
     * rotate( iAxis, fAngle ).
     * \param iAxis values: (0,1,2) = (x,y,z)
     * \param fAngle angle in degrees
     */
    int rotate(lua_State *pState);
    /**
     * Rotate around a single axis around the center.
     * rotate( iAxis, fAngle ).
     * \param iAxis values: (0,1,2) = (x,y,z)
     * \param fAngle angle in degrees
     */
    int rotateCenter(lua_State *pState);
    
    /**
     * Rotate around Phi, theta and psi 
     * rotate( fPhiAngle,  fThetaAngle, fPsiAngle ).
     * \param fPhiAngle angle in degrees
     * \param fThetaAngle angle in degrees
     * \param fPsiAngle angle in degrees
     */
    int rotatePTP(lua_State *pState);

    /**
     * Translate.
     * translate( fX, fY, fZ ).
     */
    int translate(lua_State *pState);

    /**
     * Print content to stdout
     */
    int print(lua_State *pState);

    /**
     * Multiply with another matrix4 matrix.
     * Parameter: Matrix4 object
     * Returns: New matrix4 object.
     *
     * usage:
     * mat_c = mat_a:mult( mat_b )
     */
    int mult(lua_State* pState);

    static const char className[];
    static const svt_luna<matrix4>::RegType methods[];
};

///////////////////////////////////////////////////////////////////////////////
// LUA BINDINGS: SPHERE
///////////////////////////////////////////////////////////////////////////////

/**
 * The sphere class is a very simple svt visualization node to mark positions
 */
class sphere
{
protected:

    svt_sphere* m_pSphere;

public:

    sphere(lua_State *);
    ~sphere();

    /**
     * Set sphere
     * \param pSphere pointer to svt_sphere object
     */
    void set( svt_sphere* pSphere );
    /**
     * Get sphere
     * \return pointer to svt_sphere object
     */
    svt_sphere* get();

    /**
     * Is the given svt_node object a sphere?
     */
    static bool isSphere( svt_node* pSphere );

    /**
     * Set the radius of the sphere.
     * Parameter: Number
     *
     * usage:
     * sph:setRadius( 15.0 )
     */
    int setRadius(lua_State* pState);
    /**
     * Get the radius of the sphere.
     * Returns: Number
     *
     * usage:
     * rad = sph:getRadius( )
     */
    int getRadius(lua_State* pState);

    /**
     * Set the color of the sphere.
     * Parameter: Number between 0.0 and 1.0 for red, green and blue.
     *
     * usage:
     * sph:setColor( 1.0, 0.0, 0.5 )
     */
    int setColor(lua_State* pState);
    /**
     * Get the color of the sphere.
     * Returns: Numbers between 0.0 and 1.0 for red, green and blue.
     *
     * usage:
     * r,g,b = sph:getColor( )
     */
    int getColor(lua_State* pState);

    /**
     * Get the position of the sphere.
     * Returns: Numbers (x,y,z)
     *
     * usage:
     * x,y,z = sph:getPos( )
     */
    int getPos(lua_State* pState);
    /**
     * Set the position of the sphere.
     * Parameters: Numbers (x,y,z)
     *
     * usage:
     * sph:setPos( x,y,z )
     */
    int setPos(lua_State* pState);

    /**
     * Set/get the x coordinate of the position of the atom
     * If no parameter is given, the function just returns a number, otherwise it will set the x coordinate using the parameter provided by the user.
     *
     * usage:
     * atm_a:x( 15.0 )
     * print( atm_a:x() )
     */
    int x(lua_State* pState);
    /**
     * Set/get the y coordinate of the position of the atom
     * If no parameter is given, the function just returns a number, otherwise it will set the y coordinate using the parameter provided by the user.
     *
     * usage:
     * atm_a:y( 15.0 )
     * print( atm_a:y() )
     */
    int y(lua_State* pState);
    /**
     * Set/get the z coordinate of the position of the atom
     * If no parameter is given, the function just returns a number, otherwise it will set the z coordinate using the parameter provided by the user.
     *
     * usage:
     * atm_a:z( 15.0 )
     * print( atm_a:z() )
     */
    int z(lua_State* pState);

    static const char className[];
    static const svt_luna<sphere>::RegType methods[];
};

///////////////////////////////////////////////////////////////////////////////
// LUA BINDINGS: ARROW
///////////////////////////////////////////////////////////////////////////////

/**
 * The arrow class is a very simple svt visualization node to mark positions
 */
class arrow
{
protected:

    svt_arrow* m_pArrow;

public:

    arrow(lua_State *);
    ~arrow();

    /**
     * Set arrow
     * \param pArrow pointer to svt_arrow object
     */
    void set( svt_arrow* pArrow );
    /**
     * Get arrow
     * \return pointer to svt_arrow object
     */
    svt_arrow* get();

    /**
     * Is the given svt_node object a arrow?
     */
    static bool isArrow( svt_node* pArrow );

    /**
     * Set the radius of the arrow.
     * Parameter: Number
     *
     * usage:
     * sph:setRadius( 15.0 )
     */
    int setRadius(lua_State* pState);
    /**
     * Get the radius of the arrow.
     * Returns: Number
     *
     * usage:
     * rad = sph:getRadius( )
     */
    int getRadius(lua_State* pState);

    /**
     * Set the color of the arrow.
     * Parameter: Number between 0.0 and 1.0 for red, green and blue.
     *
     * usage:
     * sph:setColor( 1.0, 0.0, 0.5 )
     */
    int setColor(lua_State* pState);
    /**
     * Get the color of the arrow.
     * Returns: Numbers between 0.0 and 1.0 for red, green and blue.
     *
     * usage:
     * r,g,b = sph:getColor( )
     */
    int getColor(lua_State* pState);

    /**
     * Get the position of the arrow.
     * Returns: Numbers (x,y,z)
     *
     * usage:
     * x,y,z = sph:getPos( )
     */
    int getPos(lua_State* pState);
    /**
     * Set the position of the arrow.
     * Parameters: Numbers (x,y,z)
     *
     * usage:
     * sph:setPos( x,y,z )
     */
    int setPos(lua_State* pState);

    /**
     * Set/get the x coordinate of the position of the atom
     * If no parameter is given, the function just returns a number, otherwise it will set the x coordinate using the parameter provided by the user.
     *
     * usage:
     * atm_a:x( 15.0 )
     * print( atm_a:x() )
     */
    int x(lua_State* pState);
    /**
     * Set/get the y coordinate of the position of the atom
     * If no parameter is given, the function just returns a number, otherwise it will set the y coordinate using the parameter provided by the user.
     *
     * usage:
     * atm_a:y( 15.0 )
     * print( atm_a:y() )
     */
    int y(lua_State* pState);
    /**
     * Set/get the z coordinate of the position of the atom
     * If no parameter is given, the function just returns a number, otherwise it will set the z coordinate using the parameter provided by the user.
     *
     * usage:
     * atm_a:z( 15.0 )
     * print( atm_a:z() )
     */
    int z(lua_State* pState);

    static const char className[];
    static const svt_luna<arrow>::RegType methods[];
};

///////////////////////////////////////////////////////////////////////////////
// LUA BINDINGS: LABEL
///////////////////////////////////////////////////////////////////////////////

/**
 * The label class is a very simple svt visualization node to put text labels into the 3D scene
 */
class label
{
protected:

    svt_textnode* m_pLabel;

public:

    label(lua_State *);
    ~label();

    /**
     * Set label
     * \param pLabel pointer to svt_textnode object
     */
    void set( svt_textnode* pLabel );
    /**
     * Get label
     * \return pointer to svt_textnode object
     */
    svt_textnode* get();

    /**
     * Is the given svt_node object a label?
     */
    static bool isLabel( svt_node* pLabel );

    /**
     * Set the color of the arrow.
     * Parameter: Number between 0.0 and 1.0 for red, green and blue.
     *
     * usage:
     * sph:setColor( 1.0, 0.0, 0.5 )
     */
    int setColor(lua_State* pState);
    /**
     * Get the color of the arrow.
     * Returns: Numbers between 0.0 and 1.0 for red, green and blue.
     *
     * usage:
     * r,g,b = sph:getColor( )
     */
    int getColor(lua_State* pState);

    /**
     * Get the position of the arrow.
     * Returns: Numbers (x,y,z)
     *
     * usage:
     * x,y,z = sph:getPos( )
     */
    int getPos(lua_State* pState);
    /**
     * Set the position of the arrow.
     * Parameters: Numbers (x,y,z)
     *
     * usage:
     * sph:setPos( x,y,z )
     */
    int setPos(lua_State* pState);

    /**
     * Get the text of the label
     * Returns: String
     *
     * usage:
     * text = lbl:getText( )
     */
    int getText(lua_State* pState);
    /**
     * Set the text of the label
     * Parameters: String
     *
     * usage:
     * lbl:setText( "Hello World" )
     */
    int setText(lua_State* pState);

    /**
     * Set/get the x coordinate of the position of the atom
     * If no parameter is given, the function just returns a number, otherwise it will set the x coordinate using the parameter provided by the user.
     *
     * usage:
     * atm_a:x( 15.0 )
     * print( atm_a:x() )
     */
    int x(lua_State* pState);
    /**
     * Set/get the y coordinate of the position of the atom
     * If no parameter is given, the function just returns a number, otherwise it will set the y coordinate using the parameter provided by the user.
     *
     * usage:
     * atm_a:y( 15.0 )
     * print( atm_a:y() )
     */
    int y(lua_State* pState);
    /**
     * Set/get the z coordinate of the position of the atom
     * If no parameter is given, the function just returns a number, otherwise it will set the z coordinate using the parameter provided by the user.
     *
     * usage:
     * atm_a:z( 15.0 )
     * print( atm_a:z() )
     */
    int z(lua_State* pState);

    static const char className[];
    static const svt_luna<label>::RegType methods[];
};

///////////////////////////////////////////////////////////////////////////////
// LUA BINDINGS: MARKER
///////////////////////////////////////////////////////////////////////////////

/**
 * The marker class is a very simple document to mark a position in 3D space.
 */
class marker
{
protected:

    sculptor_doc_marker* m_pDoc;
    bool m_bDelete;

public:

    /**
     * The marker class encapsulates a single 3D marker
     */
    marker(lua_State *);
    ~marker();

    /**
     * Set the sculptor document that is encapsulated by this object
     */
    void setDoc( sculptor_doc_marker* pDoc);

    /**
     * Get the sculptor document that is encapsulated by this object
     */
    sculptor_doc_marker* getDoc();

    /**
     * Set/get the x coordinate of the position of the marker
     * If no parameter is given, the function just returns a number, otherwise it will set the x coordinate using the parameter provided by the user.
     *
     * usage:
     * mar:x( 15.0 )
     * print( mar:x() )
     */
    int x(lua_State* pState);
    /**
     * Set/get the y coordinate of the position of the marker
     * If no parameter is given, the function just returns a number, otherwise it will set the y coordinate using the parameter provided by the user.
     *
     * usage:
     * mar:y( 15.0 )
     * print( mar:y() )
     */
    int y(lua_State* pState);
    /**
     * Set/get the z coordinate of the position of the marker
     * If no parameter is given, the function just returns a number, otherwise it will set the z coordinate using the parameter provided by the user.
     *
     * usage:
     * mar:z( 15.0 )
     * print( mar:z() )
     */
    int z(lua_State* pState);

    /**
     * Get the position of the marker.
     * Returns: Numbers (x,y,z)
     *
     * usage:
     * x,y,z = mar:getPos( )
     */
    int getPos(lua_State* pState);
    /**
     * Set the position of the marker.
     * Parameters: Numbers (x,y,z)
     *
     * usage:
     * mar:setPos( x,y,z )
     */
    int setPos(lua_State* pState);

    /**
     * Set the scale of the marker node
     *
     * usage:
     * mar:setScale( 2.0 )
     */
    int setScale( lua_State* pState );

    /**
     * Set the color of the marker node
     *
     * usage:
     * mar:setColor( R,G,B )
     */
    int setColor( lua_State* pState );

    /**
     * Measure distance to another marker.
     * Parameter: Marker object.
     * Returns: Number.
     *
     * usage:
     * dist = mar1:distance( mar2 )
     */
    int distance(lua_State * pState);


    static const char className[];
    static const svt_luna<marker>::RegType methods[];
};

///////////////////////////////////////////////////////////////////////////////
// LUA SYNTAX HIGHLIGHTER FOR QTEXTEDIT
///////////////////////////////////////////////////////////////////////////////

/**
 * This class derives from QSyntaxHighlighter to do the lua syntax highlighting. It mainly follows
 * the highlighting used in LuaEdit.
 * \author Manuel Wahle
 */
class sculptor_lua_syntax : public QSyntaxHighlighter
{

    // this struct holds pattern-format rules
    struct SVT_LUA_HIGHLIGHTINGRULE
    {
        QRegExp pattern;
        QTextCharFormat format;
    };

    // this vector holds all the rules
    QVector<SVT_LUA_HIGHLIGHTINGRULE> m_aHighlightingRules;

    // multi line comment start expression
    QRegExp m_oCommentStartExpression;
    // multi line comment end expression
    QRegExp m_oCommentEndExpression;

    // format for comments
    QTextCharFormat m_oCommentFormat;

public:

    /**
     * Constructor
     */
    sculptor_lua_syntax(QTextEdit* pTextEdit);

    /**
     * Destructor
     */
    virtual ~sculptor_lua_syntax();

    /**
     * method from QSyntaxHighlighter, called automatically whenever text need to be
     * highlighted. Code similar to the syntax highlighting example in the Qt4 doc
     */
    virtual void highlightBlock(const QString& oText);
};

#endif
