/*-*-c++-*-*****************************************************************
                          sculptor_document.h
                          -------------------
    begin                : 27.07.2001
    author               : Stefan Birmanns
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DOCUMENT_H
#define SCULPTOR_DOCUMENT_H

enum {
    SENSITUS_DOC_UNKNOWN,
    SENSITUS_DOC_SCENE,
    SENSITUS_DOC_PDB,
    SENSITUS_DOC_SITUS,
    SENSITUS_DOC_SAXS,
    SENSITUS_DOC_SMF,
    SENSITUS_DOC_CLIP,
    SENSITUS_DOC_ERASER,
    SENSITUS_DOC_MARKER
};

enum {
    SCULPTOR_CVSAMPLING,
    SCULPTOR_UNIONSPHERE,
    SCULPTOR_CORRELATION,
    SCULPTOR_LAPLACIAN,
};

enum {
    SCULPTOR_CVDISP_NONE,
    SCULPTOR_CVDISP_TAIL,
    SCULPTOR_CVDISP_HEAD

};

enum {
    CLUSTER_METHOD_LAPLACE,
    CLUSTER_METHOD_CLUSTERING
};

// Euler angle list
typedef struct {
  double phi;
  double theta;
  double psi;
} EULER_ANGLE;

// svt includes
class svt_node;
#include <svt_color.h>
#include <svt_vector4f.h>
#include <svt_point_cloud_pdb.h>
#include <svt_sampled.h>
#include <svt_tinyxml.h>
// dialog boxes
#include <sculptor_dlg_clustering.h>
// qt4 includes
class QWidget;
#include <QFileInfo>
// forward declarations
class QMainWindow;
class window_documentlistitem;


/**
 * A transformation helper class that encapsulates a single transformation (e.g. coming out of the interactive docking...)
 */
class sculptor_transform
{

public:

    sculptor_transform(svt_matrix4<Real64> oMatrix, const char* pName=NULL, vector<int> *pMatch=NULL, long int iProbeID = -1, long int iTargetID = -1)
	: m_pName( NULL )
    {
	m_oMatrix = oMatrix;
        setName( pName );
        
        if (pMatch!=NULL)
        {
            setMatch(pMatch, iProbeID, iTargetID);

        } else {

            m_iTargetID = (long int)(100000.0 * svt_genrand());
            m_iProbeID = (long int)(100000.0 * svt_genrand());
        }
    };

    sculptor_transform(const sculptor_transform& rOther)
	: m_pName( NULL )
    {
	m_oMatrix = rOther.m_oMatrix;
        
        setMatch( &(rOther.m_pMatch), rOther.m_iProbeID, rOther.m_iTargetID );
        
        setName( rOther.m_pName );
    };

    ~sculptor_transform()
    {
	if (m_pName != NULL)
	    delete[] m_pName;
    };

    inline svt_matrix4<Real64> getMatrix() const
    {
	return m_oMatrix;
    };
    inline void setMatrix(svt_matrix4<Real64> oMatrix)
    {
	m_oMatrix = oMatrix;
    };

    inline const char* getName()
    {
	return m_pName;
    };

    inline void setName(const char* pName)
    {
	if (pName != NULL)
	{
	    if (m_pName != NULL)
	    {
		delete[] m_pName;
		m_pName = NULL;
	    }

	    m_pName = new char[strlen(pName)+1];
	    strcpy( m_pName, pName );

	} else

            m_pName = NULL;
    };

    inline void setMatch(const vector<int>* pMatch, long int iProbeID, long int iTargetID)
    {
        if (m_pMatch.size()>0 )
            m_pMatch.clear();
        
        for (unsigned int iIndex=0; iIndex < pMatch->size(); iIndex++)
            m_pMatch.push_back(pMatch->at(iIndex));

        m_iTargetID = iTargetID;
        m_iProbeID  = iProbeID;
    }
    inline void setMatchFromString(const char *str, int iCount)
    {
        if (m_pMatch.size()>0 )
            m_pMatch.clear();
        
        //code copied from svt_matrix4:setFromStr
        unsigned int i = 0;
        char tmpstr[1256];
        int tmpptr = 0;
        int valcnt = 0;
        
        while(i<strlen(str) && valcnt < iCount)
        {
            if (str[i] != ' ' && i != strlen(str)-1)
                tmpstr[tmpptr++] = str[i];
            else 
            {
                if (i == strlen(str)-1)
                {
                    tmpstr[tmpptr++] = str[i];
                }

                if (tmpptr != 0)
                {
                    tmpstr[tmpptr] = 0;
                    m_pMatch.push_back( atoi(tmpstr) );
                    valcnt++;
                }
                tmpptr = 0;
            }
            i++;
        }
        
    }

    inline vector<int>* getMatch()
    {
        if (m_pMatch.size()==0)
        {
            for (unsigned int iIndex=0; iIndex < m_pMatch.size(); iIndex++)
               m_pMatch.push_back(iIndex);
        }
        
        return &m_pMatch;
    }
    inline void printMatch()
    {
        
        if (m_pMatch.size()>0)
        {
            svtout << "Size=" << m_pMatch.size() << ":";
            for (unsigned int iIndex=0; iIndex < m_pMatch.size(); iIndex++)
               cout << m_pMatch[iIndex] << " ";
            cout << endl;
        }
        else
            svtout << "No match" << endl; 
    }

    sculptor_transform& operator=(const sculptor_transform& that)
    {
        m_oMatrix = that.m_oMatrix;
        setMatch( &(that.m_pMatch), that.m_iProbeID, that.m_iTargetID );
        
        m_pName = NULL;

        setName( that.m_pName );

        m_iTargetID = that.m_iTargetID;
        m_iProbeID  = that.m_iProbeID;

	return *this;
    };

    /**
     * Get the codebook ID of the target map
     */
    long int getTargetID()
    {
        return m_iTargetID;
    };
    /**
     * Get the codebook ID of the probe molecule
     */
    long int getProbeID()
    {
        return m_iProbeID;
    };
    /**
     * Set the codebook ID of the target map
     */
    void setTargetID(long int iTargetID)
    {
        m_iTargetID = iTargetID;
    };
    /**
     * Set the codebook ID of the probe molecule
     */
    void setProbeID(long int iProbeID)
    {
        m_iProbeID = iProbeID;
    };

protected:

    svt_matrix4<Real64> m_oMatrix;
    
    char* m_pName;
    
    //pairing of matching codebook vectors 
    vector<int> m_pMatch;
    //the codebook ids
    long int m_iTargetID;
    long int m_iProbeID;
};


/**
 * This class is the base class for all sculptor documents. If you want sculptor to support a new
 * type of document you must subclass this class and write your own routines to load a file and to
 * display it.
 * @author Stefan Birmanns
 */
class sculptor_document : public QObject
{
    Q_OBJECT

protected:

    // pointer to the documentlistitem
    window_documentlistitem* m_pDLItem;

    // qfileinfo holding the complete file name
    QFileInfo m_oFileInfo;
    // qfileinfo holding the complete file name
    QFileInfo m_oFileInfo_SRCB;
    // display name. set to file.extension by default, or to the given display name
    QString m_oDisplayName;


    // pointer to the svt_node with the actual svt object
    svt_node* m_pGraphNode;
    // initial matrix
    svt_matrix4f m_oInitMat;


    // is the document a solution document?
    bool m_bSolution;
    // is the document a copy of some other document?
    bool m_bCopy;
    // is the object selected or not?
    bool m_bSelected;
    //has the content of the data changed
    bool m_bChanged;
    //is the document locked
    bool m_bLocked;


    // transformation list
    vector< sculptor_transform > m_aTransforms;
    // current transformation
    unsigned int m_iCurrentTrans;

    // internal double transformation matrix
    svt_matrix4<double> m_oDoubleTrans;

    // codebook
    svt_point_cloud_pdb< svt_vector4<Real64> > m_aCodebook;

    // codebook for steric clashing
    svt_point_cloud_pdb< svt_vector4<Real64> > m_aCodebookSC;

    // clustering dialog
    dlg_clustering* m_pClusteringDlg;
    
    // has transform list been modified?
    bool m_bTransformListModified;

    // basic node to render the cluster centers
    svt_node* m_pCodebookSpheres;

    // color for the cluster centers
    Real32 m_fCV_Red, m_fCV_Green, m_fCV_Blue;

    // node for the cv-spheres for the steric clashing...
    svt_node* m_pStericSpheres;

    //Document is head or tail for an CV displacement object
    int m_iCVDispSrc;

    // index of the two selected codebook vectors
    int m_iSelectA;
    int m_iSelectB;

    // interactive global docking
    //
    bool m_bIGDOn;
    bool m_bIGDWarningShown;
    bool m_bEliDataLoaded;
    unsigned long m_iDocID;
    sculptor_document* m_pOrigProbeDoc;
    // translation and orientation objects
    svt_volume<Real64> m_cEliCrossCorr;
    svt_volume<Real64> m_cEliAngleIndex;
    vector<EULER_ANGLE> m_cEliAngleList;
    // filenames so we can reload files
    QString m_sEliCrossCorrFile;
    QString m_sEliAngleIndexFile;
    QString m_sEliAngleListFile;

    long int m_iCodebookID;

public:

    /**
     * Constructor
     * \param pNode corresponding svt_node*
     * \param pFileName the complete file name
     * \param pDisplayName a name for the document list
     * \param pFileName_SRCB complete file name of a supplementary file (e.g., a psf)
     */
    sculptor_document(svt_node* pNode,
		      string pFileName = "",
		      string pFileName_SRCB = "");

    /**
     * Destructor
     */
    virtual ~sculptor_document();

    /**
     * get the full filename with absolute path and extension
     * \param pFilename
     */
    void setFileName(string pFilename);

    /**
     * get the full filename with absolute path and extension
     * \return pFilename
     */
    string getFileName(bool bWithPath = true);

    /**
     * get the filename without path and without extension
     * \return pFilename
     */
    string getNameShort();

    /**
     * get the full filename of the supplementary file (e.g. a psf) with absolute path and extension
     * \return pFilename
     */
    string getFileName_SRCB(bool bWithPath = true);

    /**
     * Sets the document list name
     * \param pFilename a string representing the display name
     */
    void setDisplayName(string pName);

    /**
     * Gets the document list name
     * \return pName the name the document should have in the file list
     */
    string getDisplayName();

    /// get the svt_node the document is providing
    virtual svt_node* getNode();

    /**
     * set visibility status
     * \param bVisible desired visibility status
     */
    void setVisible(bool bVisible);

    /**
     * get visibility status
     * \return bool visibility status
     */
    bool getVisible();

    /**
     * Set visibility status of the codebook
     * \param bVisible if true the feature-vectors are rendered
     */
    void setCV_Visible( bool bVisible );

    /**
     * Get visibility status of the codebook
     * \return if true the feature-vectors are rendered
     */
    bool getCV_Visible( );

    /**
     * Set the visibility status of the steric spheres container
     * \param bVisible if true the spheres are visible
     */
    void setStericSpheresVisible( bool bVisible );

    /**
     * Set the visibility status of the steric spheres
     * \param iIndex index of sphere to turn on or off
     * \param bVisible if true the spheres are visible
     */
    void setStericSpheresVisible( unsigned int iIndex, bool bVisible );

    /**
     * Set the color of individual steric spheres
     * \param iIndex index of sphere to turn on or off
     * \param color 
     */
    void setStericSpheresColor( unsigned int iIndex, svt_color* color);

    /**
     * get the type of the document
     * \return SENSITUS_DOC_UNKNOWN, SENSITUS_DOC_PDB or SENSITUS_DOC_SITUS
     */
    virtual int getType();
    
    /**
     * get rather document is SRC (and which) in a CVDisp Object 
     * \return SCULPTOR_CVDISP_NONE, SCULPTOR_CVDISP_HEAD, SCULPTOR_CVDISP_TAIL
     */
    virtual int getCVDispSrc();
    
    /**
     * set rather document is SRC (and which) in a CVDisp Object 
     * \param SCULPTOR_CVDISP_NONE, SCULPTOR_CVDISP_HEAD, SCULPTOR_CVDISP_TAIL
     */
    virtual void setCVDispSrc(int iCVDispSrc = SCULPTOR_CVDISP_NONE);

    /**
     * get the position of the sculptor document in real space (e.g. for pdb or situs files)
     * \return svt_vector4f with the position
     */
    virtual svt_vector4f getRealPos();
    
    /**
     * Get the coordinates of the codebook in the real space based on the transfomations that the doc has endured 
     * \param oOrigin the scene's origin
     * \return svt_point_cloud codebook in the real space coordinates 
     */
    void getCodebookRealPos(svt_point_cloud_pdb<svt_vector4<Real64> >& oCodebook, svt_vector4<Real64> oOrigin);

    /**
     * document was changed. This function sets the internal state to "changed". It will automatically get un-set if the document is saved.
     */
    virtual void setChanged();
    /**
     * was the document changed?
     * \return true if the document was changed
     */
    bool getChanged();

    /**
     * set locked
     */
    void setLocked( bool bLocked );
    /**
     * set locked
     * \return whether the document is locked
     */
    bool getLocked();

    /**
     * set solution flag
     */
    void setSolution(bool bSolution);
    /**
     * get solution flag
     */
    bool getSolution();
    
    /**
     * set copy flag
     */
    void setCopy(bool bCopy);
    /**
     * get copy flag
     */
    bool getCopy();
    
    /**
     * reset the initial transformation matrix of the document (that means: the matrix after loading the doc)
     */
    void resetInitMat();
    /**
     * store the current node transformation matrix as initial matrix
     */
    void storeInitMat();

    /**
     * restores the state of document from xml datastructure
     * \param xml element with stored state of document
     */
    virtual void setState(TiXmlElement* doc);
    /**
     * stores the state of document in xml datastructure
     * \param xml element to store state of document
     */
    virtual void getState(TiXmlElement* doc);

    /**
     * save the document to disk
     * \param pFilename string with the filename
     */
    virtual void save(string oFilename);
    /**
     * is this document saveable?
     * \return true if the document can be saved to disk
     */
    virtual bool getSaveable();

    // Transformations

    /**
     * Get number of transforms stored.
     */
    unsigned int getTransformCount();
    /**
     * Add the current transformation.
     * \return true if successful (if there is no svt node attached yet, the document might fail to add the transform).
     */
    bool addTransform(const char* pName =NULL);
    /**
     * Add a transformation.
     * \return true if successful (if there is no svt node attached yet, the document might fail to add the transform).
     */
    bool addTransform(const char* pName, svt_matrix4<Real64> oMatrix, vector<int>* pMatch=NULL, long int iProbeID = -1, long int iTargetID = -1);
    /**
     * Remove a transformation from the list
     * \param iTrans the transformation that has to be removed
     */
    void removeTransform( unsigned int iTrans );
    /**
     * Remove all transforms
     */
    void removeAllTransforms();
    /**
     * Make current
     */
    void applyTransform( );
    /**
     * Make previous transform in the list the current one
     */
    void prevTransform( );
    /**
     * Make next transform in the list the current one
     */
    void nextTransform( );
    /**
     * Make current
     * \param iTrans the transformation that has to be removed
     */
    void applyTransform( unsigned int iTrans );
    /**
     * Get name of a transformation
     * \return pointer to array of char
     */
    const char* getTransformName( unsigned int iTrans );
    /**
     * Set name of a transformation
     * \param iTrans index of transformation
     * \param pName pointer to array of char
     */
    void setTransformName( unsigned int iTrans, const char* pName );
    /**
     * Load all the transforms from a file
     * \param pointer to array of char with the filename
     */
    bool loadTransform(const char* pFname);
    /**
     * Save all the transforms from to a file
     * \param pointer to array of char with the filename
     */
    void saveTransform(const char* pFname);
    /**
     * Move an item in the transform list up
     * \param iItem index of the item (must be greater than zero)
     */
    void transformUp( unsigned int iItem );
    /**
     * Move an item in the transform list down
     * \param iItem index of the item (cannot be the last element of the list)
     */
    void transformDown( unsigned int iItem );
    /**
     * Transform list modified?
     */
    bool transformListModified( );
    /**
     * Set Transform list modified
     */
    void setTransformListModified( bool bTransformListModified );
    /**
     * Get the match of the current transform
     */
    vector<int>* getCurrentMatch();
    /**
     * Get the target-map ID of the current transform
     */
    long int getCurrentMatchTargetID();
    /**
     * Get the probe-molecule ID of the current transform
     */
    long int getCurrentMatchProbeID();
    /**
     * Delete all the match information, but leave the transformations intact
     */
    void delAllMatch();
    

    // Selection

    /**
     * If the document is selected it may have to perform some internal tasks. Obviously, setting
     * the internal boolean variable for being selected makes sense
     */
    virtual void onSelection();
    /**
     * If the document is deselected it has perhaps to do some cleanup
     * \param pParent pointer to parent widget
     */
    virtual void onDeselection();

    /**
     * is the document selected?
     * \return true if document is selected
     */
    bool getSelected();

    /**
     * set the select-status of the document
     * \param bSelected true if the document is selected
     */
    void setSelected(bool bSelected);

    /**
     * Return an array with all svt_node-IDs of the codebook vector spheres
     */
    vector< int > getCodebookIDs();

    // Codebook

    /**
     * Get the codebook
     * \return reference to svt_point_cloud object with the codebook vectors
     */
    inline svt_point_cloud< svt_vector4<Real64> >& getCodebook()
    {
        return m_aCodebook;
    };
    /**
     * Set the codebook
     * \param reference to svt_point_cloud object with the codebook vectors
     */
    void setCodebook(svt_point_cloud< svt_vector4<Real64> >& rOther);

    /**
     * Get the codebook for steric clashing
     * \return reference to svt_point_cloud object with the codebook vectors
     */
    inline svt_point_cloud< svt_vector4<Real64> >& getCodebookSC()
    {
        return m_aCodebookSC;
    };

    /**
     * Get the ID of the current codebook. Codebook-vectors are often utilized to relate objects with each other, e.g. for a multi-scale docking.
     * If the codebook gets regenerated, all the relationships that are stored elsewhere are invalid and need to be recalculated aswell. Therefore the codebook has a unique ID
     * which one can use to keep track if it remains unchanged. If the codebook gets regenerated, even if the number of vectors stay the same, it receives a new ID.
     */
    long int getCodebookID();
    /**
     * Regenerate the codebook ID. It should not be necessary to call this function.
     */
    void resetCodebookID();

    /**
     * Get a single sample for the clustering
     */
    virtual svt_sampled<svt_vector4<Real64> >& getSampled();

    /**
     * Show laplace clustering dlg options
     */ 
    void showLaplaceQuantizationDlgOptions();

    /**
     * Laplace quantization method
     * \param fLi Real64 Li
     * \param fLf Real64 Lf
     * \param fEi Real64 Ei
     * \param fEf Real64 Ef
     * \param iMaxStep int number of steps
     * \param iCV unsigned int number of codebook vectors
     * \param bTopologyDetermination bool use topology
     * \param fResolution Real64 Resolution of volume
     * \param fWidth Real64 Width of each voxel
     */
    void laplaceQuantization(Real64 fLi, Real64 fLf, Real64 fEi, Real64 fEf, int iMaxStep, unsigned int iCV, bool bTopologyDetermination, Real64 fResolution, Real64 fWidth);
    
    /**
     * Show clustering dlg options
     * \param rCV pointer to starting position for CV
     */
    virtual void showClusteringDlgOptions( svt_point_cloud_pdb< svt_vector4<Real64> >* rCV = NULL);

    /**
     * Cluster the document
     * \param fLi Real64 Li
     * \param fLf Real64 Lf
     * \param fEi Real64 Ei
     * \param fEf Real64 Ef
     * \param iMaxStep int number of steps
     * \param iCV unsigned int number of codebook vectors
     * \param fTi Real64 Ti
     * \param fTf Real64 Tf
     * \param bTopologyDetermination bool use topology
     * \param rCV pointer to starting position for CV; if NULL use FAM, otherwise use TRN
     */
    void clustering(Real64 fLi, Real64 fLf, Real64 fEi, Real64 fEf, int iMaxStep, unsigned int iCV, Real64 fTi, Real64 fTf, bool bTopologyDetermination, svt_point_cloud_pdb< svt_vector4<Real64> >* pCV);

    /**
     * update codebook rendering
     */
    void updateCodebookRendering();

    /**
     * update steric clashing codebook rendering
     */
    void updateCodebookSCRendering();
      
    /**
     * Set the steric clashing codebook
     * \param reference to svt_point_cloud object with the codebook vectors
     */
    void setCodebookSC(svt_point_cloud< svt_vector4<Real64> >& rOther);

    /**
     * set the color of the codebook vectors
     */
    void setCodebookColor( Real32 fRed, Real32 fGreen, Real32 fBlue );

    /**
     * Export the codebook into a pdb file with world coordinates
     */
    void exportCodebook( QString oFilename, svt_vector4f oOrigin );
    /**
     * Import the codebook from a pdb file with world coordinates
     */
    void importCodebook(QString oFilename, svt_vector4f oOrigin);

    /**
     * A codebook vector was selected
     */
    void selectCodebookVector( svt_node* pNode );
    /**
     * De-Select all codebook vectors
     */
    void clearCVSelection();

    // interactive global docking -- BEGIN

    /**
     * set document ID
     */
    void setDocID(unsigned long iDocID);
     
    /**
     * get document ID
     */
    unsigned long getDocID();
     
    /**
     * set pointer to original probe document
     */
    void setOrigProbeDoc(sculptor_document* pOrigProbe);
     
    /**
     * get pointer to original probe document
     */
    sculptor_document* getOrigProbeDoc();
     
    /**
     * set Eliquos cross-correlation map file name
     */
    void setEliCrossCorrFile(QString sFilename);
    /**
     * get Eliquos cross-correlation map file name
     */
    QString getEliCrossCorrFile();
     
    /**
     * set Eliquos angle index map file name
     */
    void setEliAngleIndexFile(QString sFilename);
    /**
     * get Eliquos angle index map file name
     */
    QString getEliAngleIndexFile();
     
    /**
     * set Eliquos angle list file name
     */
    void setEliAngleListFile(QString sFilename);
    /**
     * get Eliquos angle list file name
     */
    QString getEliAngleListFile();

    /**
     * load Eliquos cross-correlation map
     */
    bool loadEliCrossCorr();
    
    /**
     * load Eliquos angle index map
     */
    bool loadEliAngleIndex();
    
    /**
     * load Eliquos Euler angle list
     */
    bool loadEliAngleList();
    
    /**
     * set Eliquos data loaded flag
     */
    void setEliDataLoaded(bool flag);
    
    /**
     * is Eliquos data loaded?
     */
    bool getEliDataLoaded();
    
    /**
     * is IGD turned on?
     */
    bool getIGDActive();
    /**
     * set IGD on/off
     */
    void setIGDActive(bool bIGDOn);

    /**
     *  set the status of the m_bIDGWarningShown
     *  \param bool to indicate whether the IDGWarning was already shown or not
     */
    void setIGDWarningShown( bool bIGDWarningShown );
    
    /**
     *  get the status of the m_bIDGWarningShown
     *  \return bool to indicate whether the IDGWarning was already shown or not
     */
    bool getIGDWarningShown();

    /**
     * Get pointer to cross-correlation volume
     */
    svt_volume<Real64>* getEliCrossCorrVolume();
    
    /**
     * Get pointer to angle index volume
     */
    svt_volume<Real64>* getEliAngleIndexVolume();
    
    /**
     * Get pointer to angle list
     */
    vector<EULER_ANGLE>* getEliAngleList();
    
    // interactive global docking -- END

    // Misc

    /**
     * if the document was changed, this function should be called to give the document the opportunity to update the visualization
     */
    virtual void dataChanged();

    /**
     * get the matrix of the pdb node as double svt_matrix4
     */
    svt_matrix4<double>& getTransformation();
    /**
     * update the svt_node transformation matrix with the content of the sculptor internal double matrix
     */
    void updateSVTTransformation();
    /**
     * update the internal double transformation matrix with the content of the svt matrix
     */
    void updateSculptorTransformation();

    /**
     * Get a representative color for the document
     * \return svt_color object
     */
    virtual svt_color getColor();

    /**
     * Get the size of the document
     * \return svt_vector4 with the three dimensions of the size of the document
     */
    virtual svt_vector4<Real32> getBoundingBox();

    /**
     * virtual method to create the property dialog. reimplemented in the documents that inherit
     * from sculptor_document
     * \param pParent pointer to parent widget
     */
    virtual QWidget* createPropDlg(QWidget* pParent);

    /**
     * get the property dialog without creating it if it does not exzist
     * \return QWidget* pointer to the property dialog
     */
    virtual QWidget* getPropDlg();

    /**
     * get pointer to the document's window_documentlistitem in the window_documentlist
     * \return window_documentlistitem* pointer to the document's window_documentlistitem
     */
    window_documentlistitem* getDLItem();

    /*
     * set pointer to the document's window_documentlistitem in the window_documentlist
     * \param window_documentlistitem* pointer to the document's window_documentlistitem
     */
    void setDLItem(window_documentlistitem* pDLItem);
};

#endif
