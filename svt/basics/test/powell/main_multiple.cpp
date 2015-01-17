/***************************************************************************
                          main
                          ----
    begin                : 06/14/06
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_types.h>
#include <svt_point_cloud_pdb.h>
#include <svt_point_cloud_atom.h>
#include <svt_clustering_trn.h>
#include <svt_vector4.h>
#include <svt_powell_cc.h>
#include <svt_config.h>

#define POWELL cout << svt_trimName("powell_multi_cc")

int main(int, char* argv[])
{
    svt_config oConf( argv[1] );
    unsigned int iNoUnits = oConf.getValue("NumberUnits", 2);
    char param[256];

    //
    // Parse the main parameters
    //
    // Map Filename
    string oMapFname           = oConf.getValue("MapFilename", "");
    if (oMapFname.size() == 0)
    {
        POWELL << "Error: Please specify a map filename (e.g. MapFilename = test.situs) in your config file!" << endl;
        exit(1);
    }
    // Target PDB structure filename
    string oPdbTarFname        = oConf.getValue("TargetFilename", "");
    // Resolution
    Real64 fRes                = oConf.getValue("Resolution", 10.0);
    // Population size
    int eScore                 = oConf.getValue("ScoreFunction", 2);
    // logfile
    string oLogFname           = oConf.getValue("Logfile", "");
    // how many solutions should be written out?
    unsigned int iWriteSolutions = oConf.getValue("WriteSolutions", 1);
    // shall only the trace be used for the score calculation?
    // Output path
    string oPath               = oConf.getValue("OutputPath", "");
    if (oPath.size() == 0)
    {
        POWELL << "Error: Please specify an output path (e.g. OutputPath = /tmp/test) in your config file!" << endl;
        exit(1);
    }
    
    char pCmd[1256];
    sprintf( pCmd, "if [ -d %s ]\n then\n rm -r %s\n fi\n mkdir %s\n", oPath.c_str(), oPath.c_str(), oPath.c_str());
    svt_system( pCmd );
 
    
    
    //
    // Load the units
    //
    vector<svt_point_cloud_pdb < svt_vector4<Real64> > > oPdbs;
    svt_point_cloud_pdb < svt_vector4<Real64> > oPdb, oPdbTar;
    vector<string> oPdbFnames;
    
    for (unsigned int iUnit=1; iUnit<=iNoUnits; iUnit++)
    {
        sprintf( param, "Unit%iFilename", iUnit );
        string oUnitFname = oConf.getValue( param, "" );
        if (oUnitFname.size() == 0)
        {
            POWELL << "Error: Please specify the pdb filename of the subunits (e.g. Unit1Filename = unit.pdb) in your config file!" << endl;
            exit(1);
        }
        oPdbFnames.push_back( oUnitFname );
        
        POWELL << "Loading PDB from :" << oUnitFname << endl;
        oPdb.loadPDB(oPdbFnames[iUnit-1].c_str());
	oPdbs.push_back( oPdb );
    } 
    
     char pFname[1024];
    
    //
    // Load the comparison structure (correct oligomer)
    //
    if (oPdbTarFname.size() != 0)
        oPdbTar.loadPDB(oPdbTarFname.c_str());
    
    sprintf(pFname,"%s/Target.pdb", oPath.c_str());
    oPdbTar.writePDB( pFname );
    
    
    svt_volume<Real64> oMap;
    
    //
    // Load the map
    //
    POWELL << "Load Map:" << oMapFname << endl;
    oMap.loadSitus(oMapFname.c_str());	
    
    sprintf(pFname,"%s/Target.situs", oPath.c_str());
    oMap.saveSitus(pFname);	
    
    
    svt_point_cloud_pdb< svt_vector4<Real64> > oModel;
    
    vector< svt_matrix4<Real64> > oPdbMats;
    svt_matrix4<Real64> oPdbMat;
    oPdbMat.loadIdentity();
    for (unsigned int iUnit=0; iUnit<iNoUnits; iUnit++)
    {
	oPdbMats.push_back( oPdbMat );
	
	//
	// Create starting structure
	//
	oPdb = oPdbs[iUnit];
	if (iUnit==0)
	    oModel =  oPdb;
	else
	    oModel.append(oPdb);    
    }
    
   
    sprintf(pFname,"%s/ModelStart.pdb", oPath.c_str());
    oModel.writePDB( pFname );
    
    //clear model
    oModel.deleteAllAtoms();
    oModel.deleteAllBonds();
    
//     svt_powell_cc oPowell( oPdbs, oPdbMats, oMap, oPdbMat);
//     oPowell.setResolution( 10.0 );
//     oPowell.setMaxIter( 100 );
//     oPowell.setTolerance( 1e-7 );
//     oPowell.setFast( false );
//     oPowell.setBlur( false );
//     oPowell.setCAOnly( true );
//     
//     oPowell.initThread();
//     
//     POWELL << "Time of a single score calculation: " << oPowell.getScoreTime() << endl;
//     vector< svt_matrix4<Real64> > oOutMats = oPowell.getTrans();


    
    svt_powell_cc*  pPowell = new svt_powell_cc( oPdbs, oPdbMats, oMap, oPdbMat);
    pPowell->setResolution( 10.0 );
    pPowell->setMaxIter( 100 );
    pPowell->setTolerance( 1e-7 );
    pPowell->setFast( false );
    pPowell->setBlur( false );
    pPowell->setCAOnly( true );
    
    pPowell->maximize();
    
    POWELL << "Time of a single score calculation: " << pPowell->getScoreTime() << endl;
    vector< svt_matrix4<Real64> > oOutMats = pPowell->getTrans();
    
    for (unsigned int iUnit=0; iUnit<iNoUnits; iUnit++)
    {
	oPdb = oOutMats[iUnit] * oPdbs[iUnit];
	if (iUnit==0)
	    oModel =  oPdb;
	else
	    oModel.append(oPdb);    
	
	//cout << oOutMats[iUnit] << endl;
    }
    
    sprintf(pFname,"%s/Model.pdb", oPath.c_str());
    oModel.writePDB( pFname );
};

