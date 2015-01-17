/*
			situs_document
			  --------------
    begin                : 27.07.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <situs_document.h>
#include <sculptor_prop_situs.h>
#include <sculptor_window.h>
#include <sculptor_window_documentlist.h>
#include <sculptor_transfer.h>
#include <sculptor_transfer_palette.h>
#include <sculptor_mapexplorer.h>
#include <sculptor_dlg_maptable.h>
#include <sculptor_qwtplotcurve.h>
#include <sculptor_dlg_plot.h>
#include <sculptor_dlg_floodfill.h>
#include <sculptor_dlg_mpfloodfill.h>
#include <sculptor_dlg_swapaxis.h>
#include <sculptor_dlg_crop.h>
#include <sculptor_dlg_DPSV_filter.h>

// svt includes
#include <svt_init.h>
#include <svt_threads.h>
#include <svt_random.h>
// qt4 includes
#include <QFileDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QMainWindow>
#include <QMouseEvent>
#include <QTextStream>
#include <QProgressDialog>
// general includes FIXME
#include <stdio.h>

svt_palette* situs_document::s_pHighContrastPalette = NULL;
unsigned int situs_document::s_iLoadedDocuments = 0;

extern sculptor_window* g_pWindow;
extern string QtoStd(QString);
extern QString StdtoQ(string);

enum
{
    SITUS_DOC_CONSTANT,
    SITUS_DOC_LINEAR
};

typedef struct sVoxel
{
    Real64 fIntensity;
    svt_vector4<int> oPos;

    bool operator<(const sVoxel& A) const
    {
        return (*this).fIntensity < A.fIntensity;
    }
};

/**
 * Constructor
 * \param pNode pointer to the svt_node, where the class can connect its nodes to
 * \param oFilename the filename of the situs file
 * \param pVol pointer to svt_volume object
 */
situs_document::situs_document(svt_node* pNode, string oFilename, svt_volume<Real64>* pVol)
    : sculptor_document( pNode, oFilename ),
      m_pVolumeSlices( NULL ),
      m_pVolume3DText( NULL ),
      m_pVolumeMC( NULL ),
      m_bDockToLaplaceData( false ),
      m_iSizeHistogram( 500 ),
      m_pHisX( NULL ),
      m_pHisY( NULL ),
      m_pSelVox( NULL ),
      m_pSitusPropDlg( NULL ),
      m_pMapTable( NULL ),
      m_pMapExplorer( NULL ),
      m_pFloodfillDlg( NULL ),
      m_pMpFloodfillDlg( NULL ),
      m_pSwapaxisDlg( NULL ),
      m_pDPSV_filterDlg ( NULL ),
      m_pCropDlg ( NULL )
{
    if (pVol == NULL)
    {
        QString oFile = QString( oFilename.c_str() );

        if (   oFile.right(5).toLower().compare( QString("situs") ) == 0
	    || oFile.right(3).toLower().compare( QString("sit") )   == 0)
            loadSitus( oFilename.c_str() );

        if (   oFile.right(3).toLower().compare( QString("mrc") )  == 0
	    || oFile.right(3).toLower().compare( QString("map") )  == 0
	    || oFile.right(4).toLower().compare( QString("ccp4") ) == 0)
            loadMRC( oFilename.c_str() );

	if (   oFile.right(3).toLower().compare( QString("spi") )    == 0
	    || oFile.right(6).toLower().compare( QString("spider") ) == 0 )
            loadSpider( oFilename.c_str() );

        if (m_cVolume.getSizeX() == 0)
	    return;

    } else {

        m_cVolume = *pVol;
    }

    // resize the segmentation volume
    //m_cSegVol.allocate( m_cVolume.getSizeX(),
    //                    m_cVolume.getSizeY(),
    //    		m_cVolume.getSizeZ(),
    //    		(unsigned char)(0), true );

    // adjust transfer functions
    m_cF2C_bw.setMax(m_cVolume.getMaxDensity());
    m_cF2C_bw.setMin(m_cVolume.getMinDensity());
    m_cF2C_bw.update();
    m_cF2C_rwb.setMax(m_cVolume.getMaxDensity());
    m_cF2C_rwb.setMin(m_cVolume.getMinDensity());
    m_cF2C_rwb.update();
    m_cF2C_ryg.setMax(m_cVolume.getMaxDensity());
    m_cF2C_ryg.setMin(m_cVolume.getMinDensity());
    m_cF2C_ryg.update();
    m_cF2C_rgb.setMax(m_cVolume.getMaxDensity());
    m_cF2C_rgb.setMin(m_cVolume.getMinDensity());
    m_cF2C_rgb.update();
    m_cF2O_constant.setMax(m_cVolume.getMaxDensity());
    m_cF2O_constant.setMin(m_cVolume.getMinDensity());
    m_cF2O_constant.update();
    m_cF2O_linear.setMax(m_cVolume.getMaxDensity());
    m_cF2O_linear.setMin(m_cVolume.getMinDensity());
    m_cF2O_linear.update();
    m_cF2M_points.setMax(m_cVolume.getMaxDensity());
    m_cF2M_points.setMin(m_cVolume.getMinDensity());
    m_cF2M_points.update();

    // add main node
    m_pVolNode = new svt_node();
    m_pVolNode->setName( "(situs_document)m_pVolNode" );
    m_pGraphNode->add(m_pVolNode);

    // surrounding cube
    m_pVolumeCube = new svt_box( new svt_pos(0.0f, 0.0f, 0.0f), new svt_pos(1.0f, 1.0f, 1.0f) );
    m_pVolumeCube->setName( "(situs_document)m_pVolumeCube" );
    m_pVolumeCube->setProperties(new svt_properties(new svt_color(255.0f,255.0f,255.0f)));
    m_pVolumeCube->getProperties()->setWireframe(true);
    m_pVolumeCube->getProperties()->setWireframeColor(new svt_color(255.0f,255.0f,255.0f));
    m_pVolumeCube->getProperties()->setSolid(false);
    m_pVolumeCube->setScale(m_cVolume.getWidth()*(double)(m_cVolume.getSizeX())*1.0E-2,
	                    m_cVolume.getWidth()*(double)(m_cVolume.getSizeY())*1.0E-2,
			    m_cVolume.getWidth()*(double)(m_cVolume.getSizeZ())*1.0E-2);
    m_pVolumeCube->getProperties()->setLighting(false);
    m_pVolumeCube->getProperties()->setTwoSided(true);
    m_pVolumeCube->setVisible( false );
    m_pVolNode->add(m_pVolumeCube);

    // surrounding selection cube
    m_pVolumeSelectionCube = new svt_box(new svt_pos(0.0f, 0.0f, 0.0f), new svt_pos(1.0f, 1.0f, 1.0f));
    m_pVolumeSelectionCube->setName( "(situs_document)m_pVolumeSelectionCube" );
    m_pVolumeSelectionCube->setProperties(new svt_properties(new svt_color(0.0f,1.0f,0.0f)));
    m_pVolumeSelectionCube->getProperties()->getColor()->setTransparency( 0.5f );
    m_pVolumeSelectionCube->getProperties()->setWireframe(true);
    m_pVolumeSelectionCube->getProperties()->setWireframeColor(new svt_color(1.0f,1.0f,1.0f));
    m_pVolumeSelectionCube->getProperties()->setSolid(true);
    m_pVolumeSelectionCube->getProperties()->setLighting(false);
    m_pVolumeSelectionCube->setScale(m_cVolume.getWidth()*(double)(m_cVolume.getSizeX())*1.0E-2,
	                             m_cVolume.getWidth()*(double)(m_cVolume.getSizeY())*1.0E-2,
			             m_cVolume.getWidth()*(double)(m_cVolume.getSizeZ())*1.0E-2);
    m_pVolumeSelectionCube->setVisible( false );
    m_pVolNode->add(m_pVolumeSelectionCube);

    calcHistogram();

    // put the marching cube volume into the graph
    m_pVolumeMC = new svt_volume_compact<Real64>( m_cVolume );
    m_pVolumeMC->setName( "(situs_document)m_pVolumeMC" );
    m_pVolumeMC->setVisible(false);

    // initialize the two palettes
   if (s_pHighContrastPalette == NULL)
    {
        s_pHighContrastPalette = new svt_palette;
        s_pHighContrastPalette->addColor( svt_color( 178.5f/255.0f,   0.0f/255.0f,   0.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color(  97.0f/255.0f, 205.0f/255.0f,  88.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color( 252.0f/255.0f, 175.0f/255.0f,  23.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color(   0.0f/255.0f, 127.0f/255.0f, 141.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color( 183.0f/255.0f,  39.0f/255.0f, 191.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color( 255.0f/255.0f, 245.0f/255.0f, 109.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color( 172.0f/255.0f, 224.0f/255.0f, 241.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color( 160.0f/255.0f, 255.0f/255.0f, 134.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color(   0.0f/255.0f, 169.0f/255.0f, 157.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color( 242.0f/255.0f, 244.0f/255.0f, 245.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color(   0.0f/255.0f, 114.0f/255.0f, 188.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color(   2.0f/255.0f, 137.0f/255.0f,  64.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color( 195.0f/255.0f, 192.0f/255.0f, 218.0f/255.0f) );
        s_pHighContrastPalette->addColor( svt_color( 197.0f/255.0f, 250.0f/255.0f, 246.0f/255.0f) );
    }

    //create random values for the color
    m_pVolumeMC->getCont()->setProperties(new svt_properties(new svt_color( *s_pHighContrastPalette->getColor( s_iLoadedDocuments ))) );
    m_pVolumeMC->getCont()->getProperties()->setWireframeColor(new svt_color( *s_pHighContrastPalette->getColor( s_iLoadedDocuments )) );
    m_pVolumeMC->getCont()->getProperties()->getColor()->setShininess( 70.0 );
    m_pVolumeMC->setScale(m_cVolume.getWidth()*(double)(m_cVolume.getSizeX())*1.0E-2,
	                  m_cVolume.getWidth()*(double)(m_cVolume.getSizeY())*1.0E-2,
			  m_cVolume.getWidth()*(double)(m_cVolume.getSizeZ())*1.0E-2);
    m_pVolumeMC->setSegVol( &m_cSegVol );
    m_pVolNode->add(m_pVolumeMC);

    //add document
    s_iLoadedDocuments++;
    if (s_iLoadedDocuments>14)
        s_iLoadedDocuments-=14;
    svtout << "Loaded doc: " << s_iLoadedDocuments << endl;


    // put the slices into the graph
    m_pVolumeSlices = new svt_volume_slices<Real64>( m_cVolume );
    m_pVolumeSlices->setName( "(situs_document)m_pVolumeSlices" );
    m_pVolumeSlices->setVisible(false);
    m_pVolumeSlices->setScale(m_cVolume.getWidth()*(double)(m_cVolume.getSizeX())*1.0E-2,
	                      m_cVolume.getWidth()*(double)(m_cVolume.getSizeY())*1.0E-2,
			      m_cVolume.getWidth()*(double)(m_cVolume.getSizeZ())*1.0E-2);
    m_pVolNode->add(m_pVolumeSlices);

    // put the 3d textures into the graph
    m_pVolume3DText = new svt_volume_3dtextures<Real64>( m_cVolume );
    m_pVolume3DText->setName( "(situs_document)m_pVolume3DText" );
    m_pVolume3DText->setVisible(false);
    m_pVolNode->add(m_pVolume3DText);

    m_pVolumeMC->setIsolevel( m_cVolume.getMaxDensity() * 0.5 );
    // if volume is tooo big, sample less
    if (m_cVolume.getSizeX() < 64 && m_cVolume.getSizeY() < 64 && m_cVolume.getSizeZ() < 64)
	m_pVolumeMC->setStep(1);
    else
        if (m_cVolume.getSizeX() < 256 && m_cVolume.getSizeY() < 256 && m_cVolume.getSizeZ() < 256)
	    m_pVolumeMC->setStep(2);
        else
            m_pVolumeMC->setStep(3);

    m_pVolumeMC->recalcMesh();
    m_pVolumeMC->setVisible( true );
    m_pVolumeMC->setStep(1);
}

situs_document::~situs_document()
{
    if (s_iLoadedDocuments>0)
        s_iLoadedDocuments--;

    if (m_pHisX)
        delete[] m_pHisX;
    if (m_pHisY)
        delete[] m_pHisY;

    if (m_pMapExplorer)
        delete m_pMapExplorer;
    if (m_pMapTable)
        delete m_pMapTable;
    if (m_pFloodfillDlg)
        delete (m_pFloodfillDlg);
    if (m_pMpFloodfillDlg)
        delete (m_pMpFloodfillDlg);
     if (m_pSwapaxisDlg)
    	delete (m_pSwapaxisDlg);
    if (m_pCropDlg != NULL)
        delete (m_pCropDlg);

    // the qscrollarea (that requested it) will destroy the property dialog
}

///////////////////////////////////////////////////////////////////////////////
// sculptor document specific functions
///////////////////////////////////////////////////////////////////////////////

/**
 * get the svt_node the document provides
 */
svt_node* situs_document::getNode()
{
    return m_pVolNode;
}

/**
 * get the type of the document
 * \return SENSITUS_DOC_SITUS
 */
int situs_document::getType()
{
    return SENSITUS_DOC_SITUS;
}

/**
 * restores the state of document from xml datastructure
 * \param xml element with stored state of document
 */
void situs_document::setState(TiXmlElement* pElem)
{
    sculptor_document::setState(pElem);

    // pElem->Value() == SITUS_DOCUMENT
    int i; // used also as bool
    double d;
    const char *str;
    TiXmlElement* pChild;
    int j;
    svt_vector3< Real64 > oVec;

    if (pElem->Attribute("VOXELWIDTH", &d)) m_cVolume.setWidth(d);
    if (pElem->Attribute("ORIGINX", &d))    m_cVolume.setGridX(d);
    if (pElem->Attribute("ORIGINY", &d))    m_cVolume.setGridY(d);
    if (pElem->Attribute("ORIGINZ", &d))    m_cVolume.setGridZ(d);

    updateRedraw(/* scale and rebuild DLs */ true);

    if (pElem->Attribute("BOX", &i))
        m_pVolumeCube->setVisible((bool) i);

    if ((pChild = (TiXmlElement*) pElem->FirstChild("ISOSURFACE")) != NULL)
    {
        if (pChild->Attribute("DISABLE", &i))
            m_pVolumeMC->setVisible(!(bool) i);
        if (pChild->Attribute("LEVEL", &d))
            m_pVolumeMC->setIsolevel(d);
        if (pChild->Attribute("STEPS", &i))
            m_pVolumeMC->setStep(i);
        if (pChild->Attribute("WIREFRAME", &i))
        {
            m_pVolumeMC->getCont()->getProperties()->setSolid(!(bool) i);
            m_pVolumeMC->getCont()->getProperties()->setWireframe((bool) i);
        }
        if (pChild->Attribute("FLAT", &i))
            m_pVolumeMC->getCont()->getProperties()->setSmooth(!(bool) i);
        if (pChild->Attribute("TWO-SIDED", &i))
            m_pVolumeMC->getCont()->getProperties()->setTwoSided((bool) i);
        if (pChild->Attribute("CULLING", &i))
            m_pVolumeMC->getCont()->getProperties()->setBackfaceCulling((bool) i);
        if (pChild->Attribute("TRANSPARENCY", &i))
        {
            m_pVolumeMC->getCont()->getProperties()->getColor()->setTransparency(i/100.);
            m_pVolumeMC->getCont()->getProperties()->getColor()->setStipple(false);
        }
        if ((str = pChild->Attribute("COLOR")) != NULL)
        {
            int rCol, gCol, bCol, iResults;
	    iResults = sscanf(str, "#%2X%2X%2X", &rCol, &gCol, &bCol);
	    if (iResults != 0)
	    {
		m_pVolumeMC->getCont()->getProperties()->getColor()->set(rCol/255., gCol/255., bCol/255.);
                if (m_pVolumeMC->getCont()->getProperties()->getWireframeColor() != NULL)
		    m_pVolumeMC->getCont()->getProperties()->getWireframeColor()->set(rCol/255., gCol/255., bCol/255.);
	    }
        }
	m_pVolumeMC->recalcMesh();
    }

    if ((pChild = (TiXmlElement*) pElem->FirstChild("SLICES")) != NULL)
    {
        if (pChild->Attribute("DISABLE", &i))
            m_pVolumeSlices->setVisible(!(bool) i);
        if (pChild->Attribute("CUTOFF", &d))
            m_pVolumeSlices->setCutoff(d);
        if (pChild->Attribute("F2C", &i))
            switch((enum Float2ColorName) i) // same as sChangeF2C
            {
            case BW:
                m_pVolumeSlices->setFloat2Color(&m_cF2C_bw);
                break;
            case RWB:
                m_pVolumeSlices->setFloat2Color(&m_cF2C_rwb);
                break;
            case RYG:
                m_pVolumeSlices->setFloat2Color(&m_cF2C_ryg);
                break;
            case RGB:
                m_pVolumeSlices->setFloat2Color(&m_cF2C_rgb);
                break;
            default:
                break;
            }
        if (pChild->Attribute("SLICEZY", &i))
            m_pVolumeSlices->setCurrentSliceZY(i);
        if (pChild->Attribute("SLICEXZ", &i))
            m_pVolumeSlices->setCurrentSliceXZ(i);
        if (pChild->Attribute("SLICEXY", &i))
            m_pVolumeSlices->setCurrentSliceXY(i);
        if (pChild->Attribute("DRAWZY", &i))
            m_pVolumeSlices->setDrawZY((bool) i);
        if (pChild->Attribute("DRAWXZ", &i))
            m_pVolumeSlices->setDrawXZ((bool) i);
        if (pChild->Attribute("DRAWXY", &i))
            m_pVolumeSlices->setDrawXY((bool) i);
        if (pChild->Attribute("BOX", &i))
            m_pVolumeSlices->setBox((bool) i);
        if (pChild->Attribute("GENERATED", &i))
            if ((bool) i) // same as sCreateSlices()
            {
//                 svt_setStop(true);
                m_pVolumeSlices->recalcTextures();
                m_pVolumeSlices->rebuildDL();
//                 svt_setStop(false);
            }
    }

    if ((pChild = (TiXmlElement*) pElem->FirstChild("DVR")) != NULL)
    {
//         if (pChild->Attribute("ZEROTRANS", &i))
//             m_pSitusPropDlg->m_pTransfer->getFloat2Mat()->setZero((bool) i);
        if (pChild->Attribute("SHADING", &i))
            m_pVolume3DText->setShading((bool) i);
        if (pChild->Attribute("SAMPLING", &d))
            m_pVolume3DText->setSampling(d);
        if (pChild->Attribute("DIFFUSE", &d))
            m_pVolume3DText->setDiffuse(d);
        if (pChild->Attribute("SHININESS", &d))
            m_pVolume3DText->setShininess(d);
        if (pChild->Attribute("AMBIENT", &d))
            m_pVolume3DText->setAmbient(d);
        if (pChild->Attribute("STARTSLICE", &d))
            m_pVolume3DText->setStartSlice(d);
        if (pChild->Attribute("ENDSLICE", &d))
            m_pVolume3DText->setEndSlice(d);

        m_cF2M_points.deletePoints();

        char pStr[256];

        if (pChild->Attribute("TRANSPOINTSR", &i))
        {
            for(j=0; j<i; j++)
            {
                sprintf( pStr, "TRANSPOINTRX%i", j );
                pChild->Attribute( pStr, &d );
                oVec.x( d );
                sprintf( pStr, "TRANSPOINTRY%i", j );
                pChild->Attribute( pStr, &d );
                oVec.y( d );
                oVec.z( 0.0 );

                m_cF2M_points.addPointR( oVec );
            }
        }

        if (pChild->Attribute("TRANSPOINTSG", &i))
        {
            for(j=0; j<i; j++)
            {
                sprintf( pStr, "TRANSPOINTGX%i", j );
                pChild->Attribute( pStr, &d );
                oVec.x( d );
                sprintf( pStr, "TRANSPOINTGY%i", j );
                pChild->Attribute( pStr, &d );
                oVec.y( d );
                oVec.z( 0.0 );

                m_cF2M_points.addPointG( oVec );
            }
        }

        if (pChild->Attribute("TRANSPOINTSB", &i))
        {
            for(j=0; j<i; j++)
            {
                sprintf( pStr, "TRANSPOINTBX%i", j );
                pChild->Attribute( pStr, &d );
                oVec.x( d );
                sprintf( pStr, "TRANSPOINTBY%i", j );
                pChild->Attribute( pStr, &d );
                oVec.y( d );
                oVec.z( 0.0 );

                m_cF2M_points.addPointB( oVec );
		m_pVolume3DText->setFloat2Mat(&m_cF2M_points);
            }
        }

        if (pChild->Attribute("TRANSPOINTSO", &i))
        {
            for(j=0; j<i; j++)
            {
                sprintf( pStr, "TRANSPOINTOX%i", j );
                pChild->Attribute( pStr, &d );
                oVec.x( d );
                sprintf( pStr, "TRANSPOINTOY%i", j );
                pChild->Attribute( pStr, &d );
                oVec.y( d );
                oVec.z( 0.0 );

                m_cF2M_points.addPointO( oVec );
            }
        }

	m_pVolume3DText->updateTF();

        // OK, shall we create the dvr rendering object?
	if (pChild->Attribute("DISABLE", &i))
	{
            m_pVolume3DText->setVisible(!(bool) i);

	    if (!(bool)i)
	    {
		m_pVolume3DText->recalcTextures();
		m_pVolume3DText->rebuildDL();
	    }
	}
    }
}
/**
 * stores the state of document in xml datastructure
 * \param xml element to store state of document
 */
void situs_document::getState(TiXmlElement* pElem)
{
    sculptor_document::getState(pElem);

    pElem->SetDoubleAttribute("VOXELWIDTH", m_cVolume.getWidth());
    pElem->SetDoubleAttribute("ORIGINX",    m_cVolume.getGridX());
    pElem->SetDoubleAttribute("ORIGINY",    m_cVolume.getGridY());
    pElem->SetDoubleAttribute("ORIGINZ",    m_cVolume.getGridZ());

    pElem->SetAttribute("BOX", m_pVolumeCube->getVisible()); // interpreted as int

    TiXmlElement* pMode = new TiXmlElement("ISOSURFACE");
    pElem->LinkEndChild(pMode);
    pMode->SetAttribute("DISABLE", !m_pVolumeMC->getVisible());
    pMode->SetDoubleAttribute("LEVEL", m_pVolumeMC->getIsolevel());
    pMode->SetAttribute("STEPS", m_pVolumeMC->getStep());
    pMode->SetAttribute("WIREFRAME", m_pVolumeMC->getCont()->getProperties()->getWireframe());
    pMode->SetAttribute("FLAT", !m_pVolumeMC->getCont()->getProperties()->getSmooth());
    pMode->SetAttribute("TWO-SIDED", m_pVolumeMC->getCont()->getProperties()->getTwoSided());
    pMode->SetAttribute("CULLING", m_pVolumeMC->getCont()->getProperties()->getBackfaceCulling());
    pMode->SetAttribute("TRANSPARENCY", (int)(m_pVolumeMC->getCont()->getProperties()->getColor()->getTransparency() * 100.0f));
    char str[8];
    sprintf(str, "#%02X%02X%02X", (int) (255.*m_pVolumeMC->getCont()->getProperties()->getColor()->getR()),
            (int) (255.*m_pVolumeMC->getCont()->getProperties()->getColor()->getG()),
            (int) (255.*m_pVolumeMC->getCont()->getProperties()->getColor()->getB()));
    pMode->SetAttribute("COLOR", str);

    pMode = new TiXmlElement("SLICES");
    pElem->LinkEndChild(pMode);
    pMode->SetAttribute("DISABLE", !m_pVolumeSlices->getVisible());
    pMode->SetAttribute("GENERATED", (m_pVolumeSlices->getFloat2Color() != NULL)?true:false);
    pMode->SetDoubleAttribute("CUTOFF", m_pVolumeSlices->getCutoff());
    if (m_pVolumeSlices->getFloat2Color() != NULL)
	pMode->SetAttribute("F2C", m_pVolumeSlices->getFloat2Color()->getFloat2ColorName());
    pMode->SetAttribute("SLICEZY", m_pVolumeSlices->getCurrentSliceZY());
    pMode->SetAttribute("SLICEXZ", m_pVolumeSlices->getCurrentSliceXZ());
    pMode->SetAttribute("SLICEXY", m_pVolumeSlices->getCurrentSliceXY());
    pMode->SetAttribute("DRAWZY", m_pVolumeSlices->getDrawZY());
    pMode->SetAttribute("DRAWXZ", m_pVolumeSlices->getDrawXZ());
    pMode->SetAttribute("DRAWXY", m_pVolumeSlices->getDrawXY());
    pMode->SetAttribute("BOX", m_pVolumeSlices->getBox());

    pMode = new TiXmlElement("DVR");
    pElem->LinkEndChild(pMode);
    pMode->SetAttribute("DISABLE", !m_pVolume3DText->getVisible());
//     pMode->SetAttribute("GENERATED", (m_pVolume3DText->getFloat2Color() != NULL)?true:false);
//     pMode->SetAttribute("ZEROTRANS", m_pSitusPropDlg->m_pTransfer->getFloat2Mat()->getZero());
    pMode->SetAttribute("SHADING", m_pVolume3DText->getShading());
    pMode->SetDoubleAttribute("SAMPLING", m_pVolume3DText->getSampling());
    pMode->SetDoubleAttribute("DIFFUSE", m_pVolume3DText->getDiffuse());
    pMode->SetDoubleAttribute("SHININESS", m_pVolume3DText->getShininess());
    pMode->SetDoubleAttribute("AMBIENT", m_pVolume3DText->getAmbient());
    pMode->SetDoubleAttribute("STARTSLICE", m_pVolume3DText->getStartSlice());
    pMode->SetDoubleAttribute("ENDSLICE", m_pVolume3DText->getEndSlice());

    vector< svt_vector3<double> >& rPointsR = m_cF2M_points.getPointsR();
    vector< svt_vector3<double> >& rPointsG = m_cF2M_points.getPointsG();
    vector< svt_vector3<double> >& rPointsB = m_cF2M_points.getPointsB();
    vector< svt_vector3<double> >& rPointsO = m_cF2M_points.getPointsO();

    pMode->SetAttribute("TRANSPOINTSR", rPointsR.size() );
    pMode->SetAttribute("TRANSPOINTSG", rPointsG.size() );
    pMode->SetAttribute("TRANSPOINTSB", rPointsB.size() );
    pMode->SetAttribute("TRANSPOINTSO", rPointsO.size() );

    unsigned int i;
    char pStr[256];
    for(i=0; i<rPointsR.size(); i++)
    {
        sprintf( pStr, "TRANSPOINTRX%i", i );
        pMode->SetDoubleAttribute( pStr, rPointsR[i].x() );
        sprintf( pStr, "TRANSPOINTRY%i", i );
        pMode->SetDoubleAttribute( pStr, rPointsR[i].y() );
    }
    for(i=0; i<rPointsG.size(); i++)
    {
        sprintf( pStr, "TRANSPOINTGX%i", i );
        pMode->SetDoubleAttribute( pStr, rPointsG[i].x() );
        sprintf( pStr, "TRANSPOINTGY%i", i );
        pMode->SetDoubleAttribute( pStr, rPointsG[i].y() );
    }
    for(i=0; i<rPointsB.size(); i++)
    {
        sprintf( pStr, "TRANSPOINTBX%i", i );
        pMode->SetDoubleAttribute( pStr, rPointsB[i].x() );
        sprintf( pStr, "TRANSPOINTBY%i", i );
        pMode->SetDoubleAttribute( pStr, rPointsB[i].y() );
    }
    for(i=0; i<rPointsO.size(); i++)
    {
        sprintf( pStr, "TRANSPOINTOX%i", i );
        pMode->SetDoubleAttribute( pStr, rPointsO[i].x() );
        sprintf( pStr, "TRANSPOINTOY%i", i );
        pMode->SetDoubleAttribute( pStr, rPointsO[i].y() );
    }
}

/**
 * save the document
 * \param pFilename pointer to an array of char with the filename
 */
void situs_document::save(string oFilename)
{
    if (oFilename.size()==0)
        return;

    sculptor_document::save(oFilename);
    m_cVolume.save(oFilename.c_str());
}
/**
 * is this document saveable?
 * \return true if the document can be saved to disk
 */
bool situs_document::getSaveable()
{
    return true;
}

/**
 *
 */
void situs_document::setColor(svt_color oCol)
{
    svt_color* pColor;

    pColor = m_pVolumeMC->getCont()->getProperties()->getColor();
    pColor->setR(oCol.getR());
    pColor->setG(oCol.getG());
    pColor->setB(oCol.getB());

    pColor = m_pVolumeMC->getCont()->getProperties()->getWireframeColor();
    pColor->setR(oCol.getR());
    pColor->setG(oCol.getG());
    pColor->setB(oCol.getB());

    if (m_pDLItem != NULL)
        m_pDLItem->showColor();

    m_pVolumeMC->getCont()->rebuildDL();
}

/**
 * Get a representative color for the document
 * \return svt_color object
 */
svt_color situs_document::getColor()
{
    svt_color oColor;

    if (m_pVolumeMC != NULL)
    {
	oColor.setR( m_pVolumeMC->getCont()->getProperties()->getColor()->getR() );
	oColor.setG( m_pVolumeMC->getCont()->getProperties()->getColor()->getG() );
	oColor.setB( m_pVolumeMC->getCont()->getProperties()->getColor()->getB() );
    }

    return oColor;
}

///////////////////////////////////////////////////////////////////////////////
// properties dlg
///////////////////////////////////////////////////////////////////////////////

/**
 * return the properties dialog for the document
 * \param pParent pointer to parent widget
 */
QWidget* situs_document::createPropDlg(QWidget* pParent)
{
    if ( m_pSitusPropDlg == NULL)
	m_pSitusPropDlg = new prop_situs( this, pParent );

    return (QWidget*)m_pSitusPropDlg;
}

/**
 * 
 */
QWidget* situs_document::getPropDlg()
{
    return (QWidget*)m_pSitusPropDlg;;
}

/**
 * SLOT enable the surrounding box
 * \param bEnable if true the surrounding box is rendered
 */
void situs_document::sEnableBox(bool bEnable)
{
    m_pVolumeCube->setVisible(bEnable);
}

/**
 * Rendering of the triangular mesh was set to smooth/flat shading
 */
void situs_document::sRendering_Flat( bool bToggle )
{
    m_pVolumeMC->getCont()->getProperties()->setSmooth( !bToggle );
    m_pVolumeMC->getCont()->rebuildDL();
}

/**
 * Rendering of the triangular mesh was set to single/two-sided
 */
void situs_document::sRendering_TwoSide( bool bToggle )
{
    m_pVolumeMC->getCont()->getProperties()->setTwoSided( bToggle );
    m_pVolumeMC->getCont()->rebuildDL();
}

/**
 * Rendering of the triangular mesh was set to culling on/off
 */
void situs_document::sRendering_Culling( bool bToggled )
{
    m_pVolumeMC->getCont()->getProperties()->setBackfaceCulling( bToggled );
    m_pVolumeMC->getCont()->rebuildDL();
}

/**
 * create isosurface
 */
void situs_document::createIsoSurface(float fLevel, int iSteps)
{
    m_pVolumeMC->setIsolevel(fLevel);
    m_pVolumeMC->setStep(iSteps);
    m_pVolumeMC->recalcMesh();
    m_pVolumeMC->setVisible(true);
}

///////////////////////////////////////////////////////////////////////////////
// isosurface
///////////////////////////////////////////////////////////////////////////////

/**
 * SLOT set wireframe mode for the isosurface
 * \param bWireframe true if the iso surface should be drawn in wireframe mode
 */
void situs_document::sSetIsoWireframe(bool bWireframe)
{
    m_pVolumeMC->getCont()->getProperties()->setSolid(!bWireframe);
    m_pVolumeMC->getCont()->getProperties()->setWireframe(bWireframe);
    m_pVolumeMC->getCont()->rebuildDL();
}

/**
 * SLOT disable the isosurface
 * \param bDisable if true the isosurface drawing is disabled
 */
void situs_document::setIsoEnabled(bool bEnable)
{
    m_pVolumeMC->setVisible( bEnable );

    if ( bEnable && m_pVolume3DText->getVisible() == true )
        m_pVolume3DText->setVisible(false);
}

/**
 * SLOT setting transparency
 * \param iTrans 100 means full opacity
 */
void situs_document::sSetIsoTransparency(int iTrans)
{
    if ( m_pVolumeMC == NULL
	 || m_pVolumeMC->getCont() == NULL
	 || m_pVolumeMC->getCont()->getProperties() == NULL
	 || m_pVolumeMC->getCont()->getProperties()->getColor() == NULL )
        return;

    svt_color* pColor = m_pVolumeMC->getCont()->getProperties()->getColor();
    Real32 fTrans = (Real32)(iTrans)/ 100.0f;

    if (pColor->getTransparency() != fTrans)
    {
	pColor->setTransparency(fTrans);
	pColor->setStipple(false);
	m_pVolumeMC->rebuildDL();
	m_pVolumeMC->getCont()->rebuildDL();
    }
}

/**
 * Get isosurface object
 * \return svt_volume_mc_cline pointer
 */
svt_volume_compact<Real64>* situs_document::getIso()
{
    return m_pVolumeMC;
};

///////////////////////////////////////////////////////////////////////////////
// slices
///////////////////////////////////////////////////////////////////////////////

/**
 * create the slices
 */
void situs_document::createSlices()
{
    if ( m_pSitusPropDlg != NULL )
	m_pVolumeSlices->setCutoff( m_pSitusPropDlg->getSliceTECutoff() );

    m_pVolumeSlices->recalcTextures();
    m_pVolumeSlices->rebuildDL();
    m_pVolumeSlices->setVisible(true);

    if ( m_pSitusPropDlg != NULL )
	m_pSitusPropDlg->updateTabSlices();
}

/**
 * SLOT for setting the current XY slice
 * \param iXY number of XY slice
 */
void situs_document::sSetSliceXY(int iXY)
{
    //m_pSitusPropDlg->m_pSliceSpinZ->setValue(iXY);
    //m_pSitusPropDlg->m_pSliderXY->setValue(iXY);
    m_pVolumeSlices->setCurrentSliceXY(iXY);
}
/**
 * SLOT for setting the current XZ slice
 * \param iXZ number of XZ slice
 */
void situs_document::sSetSliceXZ(int iXZ)
{
    //m_pSitusPropDlg->m_pSliceSpinY->setValue(iXZ);
    //m_pSitusPropDlg->m_pSliderXZ->setValue(iXZ);
    m_pVolumeSlices->setCurrentSliceXZ(iXZ);
}
/**
 * SLOT for setting the current ZY slice
 * \param iZY number of ZY slice
 */
void situs_document::sSetSliceZY(int iZY)
{
    //m_pSitusPropDlg->m_pSliceSpinX->setValue(iZY);
    //m_pSitusPropDlg->m_pSliderZY->setValue(iZY);
    m_pVolumeSlices->setCurrentSliceZY(iZY);
}

/**
 * SLOT enable drawing slice XY
 * \param bEnable if true the XY slice will be displayed
 */
void situs_document::sEnableSliceXY(bool bEnable)
{
    m_pVolumeSlices->setDrawXY(bEnable);
}
/**
 * SLOT enable drawing slice XZ
 * \param bEnable if true the XZ slice will be displayed
 */
void situs_document::sEnableSliceXZ(bool bEnable)
{
    m_pVolumeSlices->setDrawXZ(bEnable);
}
/**
 * SLOT enable drawing slice ZY
 * \param bEnable if true the ZY slice will be displayed
 */
void situs_document::sEnableSliceZY(bool bEnable)
{
    m_pVolumeSlices->setDrawZY(bEnable);
}

/**
 * disable slices
 * \param bEnable if true the slices are enabled
 */
void situs_document::setSlicesEnabled(bool bEnabled)
{
    m_pVolumeSlices->setVisible(bEnabled);
}

/**
 * SLOT enable box around crosssections
 * \param bEnable if true a box around the crosssections is shown
 */
void situs_document::sBoxSlices(bool bEnable)
{
    m_pVolumeSlices->setBox(bEnable);
}

/**
 * SLOT enable volume mode
 * \param bEnable if true the volume mode is enabled
 */
void situs_document::sEnableVolumeMode(bool bEnable)
{
    m_pVolumeSlices->setVolumeMode(bEnable);
}

/**
 * SLOT for changing the float2color mapping
 * \param iF color transfer function (0 means bw, 1 rwb, 2 ryg, 3 rgb)
 */
void situs_document::sChangeF2C(int iF)
{
    switch((enum Float2ColorName) iF)
    {
    case BW:
        m_pVolumeSlices->setFloat2Color(&m_cF2C_bw);
        break;
    case RWB:
        m_pVolumeSlices->setFloat2Color(&m_cF2C_rwb);
        break;
    case RYG:
        m_pVolumeSlices->setFloat2Color(&m_cF2C_ryg);
        break;
    case RGB:
        m_pVolumeSlices->setFloat2Color(&m_cF2C_rgb);
        break;
    default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
// 3d textures
///////////////////////////////////////////////////////////////////////////////

/**
 * create the 3d textures
 */
void situs_document::create3DT(bool bForce)
{
    // if any data changed, the textures will be recalculated in dataChanged(), so
    // if the textures have already been created, just apply the transfer function
    // unless somebody wants to force an update
    if ( bForce || m_pVolume3DText->getTexture3D() == NULL )
    {	
	svt_setStop(true);

	m_pVolume3DText->setFloat2Mat(&m_cF2M_points);
	m_pVolume3DText->recalcTextures();
	m_pVolume3DText->setVisible(true);
	m_pVolume3DText->rebuildDL();

	svt_setStop(false);
    }
    else
	applyTF();
}

/**
 * disable/enable the 3d textures
 * also switches off the isosurface
 */
void situs_document::set3DTextEnabled(bool bEnabled)
{
    m_pVolume3DText->setVisible(bEnabled);
}

/**
 * disable/enable 3d texture shading
 */
void situs_document::sEnable3DTShading(bool bEnabled)
{
    m_pVolume3DText->setShading(bEnabled);
    m_pVolume3DText->rebuildDL();
}

/**
 * SLOT disable/enable the zero treatment in the transfer function
 */
void situs_document::sZero(bool tf)
{
    m_cF2M_points.setZero(tf);
    m_pVolume3DText->updateTF();
}

/**
 * SLOT for changing the sampling rate
 */
void situs_document::sChangeSampling()
{
    m_pVolume3DText->setSampling( m_pSitusPropDlg->get3DSamplingRate() );

    svt_vector3<float> oV = m_pSitusPropDlg->getLightingParams();
    m_pVolume3DText->setLightingParams( oV.x(), oV.y(), oV.z() );

    m_pVolume3DText->rebuildDL();
}

/**
 * SLOT load a transfer function from disk
 */
void situs_document::sLoadTF()
{
    QString oFilter( "Transfer Function (*.tff)" );
    // open file chooser dialog
    QFileDialog oFD(NULL);
    oFD.setNameFilter( oFilter );
    oFD.setViewMode( QFileDialog::List );
    oFD.setFileMode( QFileDialog::ExistingFile );
    oFD.selectNameFilter( oFilter );

    if ( oFD.exec() == QDialog::Accepted )
    {
        QStringList oFileList = oFD.selectedFiles();

	m_cF2M_points.loadFromFile( QtoStd(oFileList[0]) );

	m_pVolume3DText->updateTF();

	if ( m_pSitusPropDlg != NULL )
	    m_pSitusPropDlg->updatePlots( false, true );
    }
}
/**
 * SLOT save a transfer function to disk
 */
void situs_document::sSaveTF()
{
    QString oFilter( "Transfer Function (*.tff)" );
    // open file chooser dialog
    QFileDialog oFD(NULL);
    oFD.setNameFilter( oFilter );
    oFD.setFileMode( QFileDialog::AnyFile );
    oFD.setViewMode( QFileDialog::List );
    oFD.selectNameFilter( oFilter );

    if ( oFD.exec() == QDialog::Accepted )
    {
        QStringList oFileList = oFD.selectedFiles();

	m_cF2M_points.saveToFile( QtoStd(oFileList[0]) );
    }
}

/**
 * 
 */
void situs_document::applyTF()
{
    m_pVolume3DText->updateTF();
    m_pVolume3DText->rebuildDL();
}

/**
 * reset transfer function
 */
void situs_document::resetTF()
{
    Real64 fMax = m_cVolume.getMaxDensity();
    Real64 fMin = m_cVolume.getMinDensity();

    m_cF2M_points.deletePoints();
    svt_vector3< Real64 > oVec;

    oVec.x( fMin ); oVec.y( 0.0 );
    m_cF2M_points.addPointR( oVec );
    oVec.x( fMax); oVec.y( 255.0 );
    m_cF2M_points.addPointR( oVec );

    oVec.x( fMin ); oVec.y( 0.0 );
    m_cF2M_points.addPointG( oVec );
    oVec.x( fMax); oVec.y( 255.0 );
    m_cF2M_points.addPointG( oVec );

    oVec.x( fMin ); oVec.y( 0.0 );
    m_cF2M_points.addPointB( oVec );
    oVec.x( fMax); oVec.y( 255.0 );
    m_cF2M_points.addPointB( oVec );

    oVec.x( fMin ); oVec.y( 0.0 );
    m_cF2M_points.addPointO( oVec );
    oVec.x( fMax); oVec.y( 255.0 );
    m_cF2M_points.addPointO( oVec );

    m_pVolume3DText->updateTF();
    m_pVolume3DText->rebuildDL();
}

///////////////////////////////////////////////////////////////////////////////
// histogram dialog
///////////////////////////////////////////////////////////////////////////////

/**
 * calculate the histogram
 */
void situs_document::calcHistogram()
{
    if (m_pHisX == NULL)
        m_pHisX = new double[m_iSizeHistogram];
    if (m_pHisY == NULL)
        m_pHisY = new double[m_iSizeHistogram];

    for (unsigned int j=0;j<m_iSizeHistogram;j++)
        m_pHisY[j]=0;

    float fMin = m_cVolume.getMinDensity();
    float fMax = m_cVolume.getMaxDensity();

    // if there is only one value in the volume, add something to min and max to show at least a line on the histogram
    if (fMax==fMin) //(fabs(fMax - fMin) < 0.01f) //original code
    {
         fMin -= 0.5f;
         fMax = fMin + 1.0f;
    }

    float fScale = ((float)(m_iSizeHistogram-1)/(fMax - fMin));
    float fScaleInv = 1.0f / fScale;

    for (unsigned int i=0;i<m_iSizeHistogram;i++)
	m_pHisX[i] = fMin + (i*fScaleInv);

    int iCurrent;
    int iCount;
    int iMax = m_cVolume.getSizeX()*m_cVolume.getSizeY()*m_cVolume.getSizeZ();
    for (iCount=0;iCount<iMax;iCount++)
    {
        iCurrent = (int)(0.5f + (m_cVolume.getValue(iCount) - fMin) * fScale);
        if (iCurrent < (int)m_iSizeHistogram && iCurrent > 0)
            ++m_pHisY[iCurrent];
    }

    // delete the 0, because otherwise it dominates the histogram too much
    iCurrent = (int)(0.5f + (0.0f - fMin) * fScale);
    if (iCurrent < (int)m_iSizeHistogram && iCurrent >= 0)
        m_pHisY[iCurrent] = 0;
}
/**
 * get histogram x
 * \return pointer to an array with 50 double values with the histogram values
 */
double* situs_document::getHistogramX()
{
    return m_pHisX;
}
/**
 * get histogram y
 * \return pointer to an array with 50 double values with the histogram values
 */
double* situs_document::getHistogramY()
{
    return m_pHisY;
}
/**
 * get histogram size
 * \return int size of the histogram
 */
unsigned int situs_document::getHistogramSize()
{
    return m_iSizeHistogram;
}

/**
 *
 */
Real64 situs_document::getIsoLevel()
{
    return m_pVolumeMC->getIsolevel();
}

/**
 * show the volume data histogram dialog
 */
void situs_document::showHistogramDlg()
{
    dlg_plot* pDlg = new dlg_plot(g_pWindow, QString("Histogram"));
    //((QWidget*)pDlg)->setWindowFlags( Qt::WA_DeleteOnClose ); FIXME

    pDlg->setComment(QString("Voxels with values equal to zero were omitted."));

    sculptor_qwtplotcurve oCurve(QString(getDisplayName().c_str()), getHistogramX(), getHistogramY(), m_iSizeHistogram);

    oCurve.setStyle(QwtPlotCurve::Steps);
    oCurve.setColor( 20, 20, 200 );
    pDlg->attachCurve(&oCurve);;

    pDlg->setXAxisTitle(QString("Density"));
    pDlg->setYAxisTitle(QString("Counts"));

    pDlg->show();
}

/**
 * subtract two maps
 */
void situs_document::subtractMap()
{
    // first step = look for the first two selected situs documents

    vector<sculptor_document*> aDocuments = g_pWindow->getDocumentList()->getDocuments(SENSITUS_DOC_SITUS, true);

    situs_document* pFirst = NULL;
    situs_document* pSecond = NULL;

    for(unsigned int i=1;i < aDocuments.size() && pSecond == NULL; i++)
    {
	if ( pFirst == NULL )
            pFirst = (situs_document*)aDocuments[i];
	else if ( pSecond == NULL )
            pSecond = (situs_document*)aDocuments[i];
    }

    // ok, now calculate the subtraction

    if (pFirst != NULL && pSecond != NULL && pFirst != pSecond)
    {
        int iFirst = pFirst->getExtX();

        QProgressDialog oProgress;
        oProgress.setMaximum(iFirst);
        oProgress.setLabelText("Subtracting Maps...");
        oProgress.show();

        float fVal = 0.0f;

        // precalculate the matrix to do the transformation towards the current situs data
        svt_matrix4<double> oTransMatrix;
        oTransMatrix.set(*pFirst->getNode()->getTransformation());
        oTransMatrix.invert();
	oTransMatrix *= svt_matrix4<double>(*pSecond->getNode()->getTransformation());

        float fWidthSecond = pSecond->getVoxelWidth();
        float fWidthFirst = pFirst->getVoxelWidth();

        int iVoxelX, iVoxelY, iVoxelZ;

        for(int iX = 0; iX < pSecond->getExtX(); iX++)
        {
            // progress bar
            if (oProgress.wasCanceled())
                return;

            oProgress.setValue(iX);

            for(int iY = 0; iY < pSecond->getExtY(); iY++)
            {
                for(int iZ = 0; iZ < pSecond->getExtZ(); iZ++)
                {
                    // get original position
		    svt_vector4<double> oVec(iX - (pSecond->getExtX()/2), iY - (pSecond->getExtY()/2), iZ - (pSecond->getExtZ()/2));
                    oVec *= fWidthSecond;
                    // move it into the situs coordinate system
		    oVec = oTransMatrix * oVec;

                    // get the index of the voxel
                    iVoxelX = (int)((oVec.x() / fWidthFirst) + 0.5f);
                    iVoxelY = (int)((oVec.y() / fWidthFirst) + 0.5f);
		    iVoxelZ = (int)((oVec.z() / fWidthFirst) + 0.5f);

                    iVoxelX += pFirst->getExtX()/2;
                    iVoxelY += pFirst->getExtY()/2;
                    iVoxelZ += pFirst->getExtZ()/2;

                    // subtract
		    fVal = pFirst->getAt( iVoxelX, iVoxelY, iVoxelZ ) - pSecond->getAt( iX, iY, iZ );
		    pFirst->setAt( iVoxelX, iVoxelY, iVoxelZ, fVal );
                    //if ( pSecond->getAt( iX, iY, iZ ) > 0.1f)
		    //    pFirst->setAt( iVoxelX, iVoxelY, iVoxelZ, 0.0f );
                }
            }
        }

	// update min/max
        // update gradient map
	//pFirst->calcGradientMap();
	pFirst->calcHistogram();

	// updating the isosurface (if any)
	pFirst->m_pVolumeMC->recalcMesh();
        pFirst->m_pVolumeMC->rebuildDL();

    } else
        QMessageBox::information(0, "CC", "Please select two situs files in order to calculate the correlation between them.");
}

/**
 * show the table with all data values
 */
void situs_document::showTable()
{
    if (m_pMapTable == NULL)
    {
        m_pMapTable = new dlg_maptable(g_pWindow, &m_cVolume);
        m_pMapTable->setWindowTitle( QString("Situs Data for ") + StdtoQ(getDisplayName()) );
    }

    m_pMapTable->raise();
    m_pMapTable->show();
}

///////////////////////////////////////////////////////////////////////////////
// public functions
///////////////////////////////////////////////////////////////////////////////

/**
 * get the voxel-width of the situs data
 * \return voxel width in angstroem
 */
float situs_document::getVoxelWidth()
{
    return m_cVolume.getWidth();
}

/**
 * get the number of voxels in x of the situs data
 * \return m_cVolume.getSizeX()
 */
int situs_document::getExtX()
{
    return m_cVolume.getSizeX();
}
/**
 * get the number of voxels in y of the situs data
 * \return m_cVolume.getSizeY()
 */
int situs_document::getExtY()
{
    return m_cVolume.getSizeY();
}
/**
 * get the number of voxels in z of the situs data
 * \return m_cVolume.getSizeZ()
 */
int situs_document::getExtZ()
{
    return m_cVolume.getSizeZ();
}
/**
 * get a voxel value
 * \param iX x index
 * \param iY y index
 * \param iZ z index
 * \return voxel value
 */
float situs_document::getAt(int iX, int iY, int iZ)
{
    return m_cVolume.getValue( iX, iY, iZ );
}
/**
 * set a voxel value
 * \param iX x index
 * \param iY y index
 * \param iZ z index
 * \param fVal new value
 */
void situs_document::setAt(int iX, int iY, int iZ, float fVal)
{
    m_cVolume.setValue( iX, iY, iZ, fVal );
}

/**
 * Calculate gradient map
 */
void situs_document::calcGradientMap()
{
    if (!m_bDockToLaplaceData)
    {
	svtout << "Generating gradient map." << endl;
	m_cGradientX.allocate( m_cVolume.getSizeX(), m_cVolume.getSizeY(), m_cVolume.getSizeZ() );
	m_cGradientY.allocate( m_cVolume.getSizeX(), m_cVolume.getSizeY(), m_cVolume.getSizeZ() );
	m_cGradientZ.allocate( m_cVolume.getSizeX(), m_cVolume.getSizeY(), m_cVolume.getSizeZ() );
	m_cVolume.calcGradient( svt_volume<Real64>::CentralDistance, m_cGradientX, m_cGradientY, m_cGradientZ );

    } else {

	// calculate Laplace-filtered volume
	svtout << "Generating Laplace filtered volume data." << endl;
	m_cVolumeLaplace.deepCopy( m_cVolume );
	m_cVolumeLaplace.applyLaplacian();
	m_cVolumeLaplace * (-1.0);
	m_cVolumeLaplace.threshold(0.0);
        m_cVolumeLaplace.normalize();

	// calculate gradient from filtered data for docking
	svtout << "Generating gradient map for Laplace filtered volume data." << endl;
	m_cGradientX.allocate( m_cVolume.getSizeX(), m_cVolume.getSizeY(), m_cVolume.getSizeZ() );
	m_cGradientY.allocate( m_cVolume.getSizeX(), m_cVolume.getSizeY(), m_cVolume.getSizeZ() );
	m_cGradientZ.allocate( m_cVolume.getSizeX(), m_cVolume.getSizeY(), m_cVolume.getSizeZ() );
	m_cVolumeLaplace.calcGradient( svt_volume<Real64>::CentralDistance, m_cGradientX, m_cGradientY, m_cGradientZ );
        m_cVolumeLaplace.setWidth( m_cVolume.getWidth() );
    }
}

/**
 * Was the Gradient already calculated?
 * \return true if the calcGradient() routine was already called once.
 */
bool situs_document::gradientCalculated()
{
    if (m_cGradientX.getSizeX() == 0)
        return false;
    else
        return true;
};

/**
 * normalize
 */
void situs_document::normalize()
{
    Real64 fIsoLevel = m_pVolumeMC->getIsolevel();
    Real64 fMax = m_cVolume.getMaxDensity();
    Real64 fMin = m_cVolume.getMinDensity();

    if (fIsoLevel >= fMin && fIsoLevel <= fMax)
    {
        Real64 fLength = fMax - fMin;
        if (fLength != 0.0)
            fIsoLevel = (fIsoLevel - fMin) / fLength;
        m_pVolumeMC->setIsolevel( fIsoLevel );
    }

    m_cVolume.normalize();

    dataChanged();
};

/**
 * flip pos/neg voxel values
 */
void situs_document::scale()
{
    bool bOK = false;
    QString oInText;
    QString oText;
    Real64 fFactor = 1.0;

    // minimum density threshold
    oInText.sprintf( "1.0" );
    oText = QInputDialog::getText( g_pWindow, "Sculptor", "Enter scaling factor:", QLineEdit::Normal,
                                   oInText, &bOK );

    if ( bOK && !oText.isEmpty() )
        fFactor = oText.toDouble();
    else
        return;

    m_cVolume.scale( fFactor );

    dataChanged();
};

/**
 * threshold
 */
void situs_document::threshold()
{
    Real64 fMinThreshold;
    Real64 fMaxThreshold;

    bool bOK = false;
    QString oInText;
    QString oText;

    // minimum density threshold
    oInText.sprintf( "%f", m_cVolume.getMinDensity() );
    oText = QInputDialog::getText( g_pWindow, "Sculptor", "Enter minimum density threshold:", QLineEdit::Normal,
                                   oInText, &bOK );
    if ( bOK && !oText.isEmpty() )
        fMinThreshold = oText.toDouble();
    else
        return;

    // maximum density threshold
    oInText.sprintf( "%f", m_cVolume.getMaxDensity() );
    oText = QInputDialog::getText( g_pWindow, "Sculptor", "Enter maximum density threshold:", QLineEdit::Normal,
                                   oInText, &bOK );
    if ( bOK && !oText.isEmpty() )
        fMaxThreshold = oText.toDouble();
    else
        return;

    // we need to distinguish two cases:
    // - if the minimal threshold is negative, we set everything below it to the value of the min. threshold
    // - if the minimal threshold is positive, we set everything below it to 0
    if (fMinThreshold >= 0.0)
    {
       m_cVolume.threshold( fMinThreshold );
        m_cVolume.threshold( 0.0, fMaxThreshold );
    } else
        m_cVolume.threshold( fMinThreshold, fMaxThreshold );

    dataChanged();
};

/**
 * SLOT crop dialog..called from documentlistitem
 * set the init values
 */
void situs_document::crop()
{
    
    if (m_pCropDlg == NULL)
    {
	m_pCropDlg = new dlg_crop(g_pWindow, this, m_cVolume);
    }

    //set the default values
    m_pCropDlg->setValues();

    m_pCropDlg->show();
    m_pVolumeSelectionCube->setVisible( true );
} 

/**
 *
 */
void situs_document::turnOffVolSelectCube()
{
  if (m_pVolumeSelectionCube)
    m_pVolumeSelectionCube->setVisible( false );
}


/** crop
 * crop a subregion...called by the crop dlg
 * \param iMinX minimum x coordinate/index
 * \param iMaxX maximum x coordinate/index
 * \param iMinY minimum y coordinate/index
 * \param iMaxY maximum y coordinate/index
 * \param iMinZ minimum z coordinate/index
 * \param iMaxZ maximum z coordinate/index
 */
void situs_document::crop( unsigned int iMinX, unsigned int iMaxX, unsigned int iMinY, unsigned int iMaxY, unsigned int iMinZ, unsigned int iMaxZ )
{
   
    m_cVolume.crop( iMinX, iMaxX, iMinY, iMaxY, iMinZ, iMaxZ );
    
    m_pVolumeMC->recalcMesh();
    m_pVolumeCube->setPos( new svt_pos(0.0, 0.0, 0.0) );
    m_pVolumeCube->setSize( new svt_pos(1.0, 1.0, 1.0) );
    m_pVolumeSelectionCube->setPos( new svt_pos(0.0, 0.0, 0.0) );
    m_pVolumeSelectionCube->setSize( new svt_pos(1.0, 1.0, 1.0) );

    updateRedraw(/* scale and rebuild DLs */ true);

    dataChanged();

    g_pWindow->adjustOrigin();

    svt_redraw();
    m_pVolumeSelectionCube->setVisible( false );
    //delete and create a new cropdlg according to the new dimensions
    if (m_pCropDlg != NULL)
    {
      m_pCropDlg = NULL;
      m_pCropDlg = new dlg_crop(g_pWindow, this, m_cVolume);
    }
}

/**
 * change the origin of the volume data
 */
void situs_document::changeOrigin()
{
    Real64 fGridX = m_cVolume.getGridX();
    Real64 fGridY = m_cVolume.getGridY();
    Real64 fGridZ = m_cVolume.getGridZ();
    svt_vector3<Real64> oOrigin;

    if (m_pSitusPropDlg != NULL)
    {
	oOrigin = m_pSitusPropDlg->getOrigin();
	fGridX = oOrigin.x();
        fGridY = oOrigin.y();
        fGridZ = oOrigin.z();
    };

    bool bOK = false;
    QString oInText;

    // grid x
    oInText.sprintf( "%f", fGridX );
    QString oText = QInputDialog::getText( g_pWindow, "Sculptor", "Enter new origin x coord:", QLineEdit::Normal,
                                           oInText, &bOK );
    if ( bOK && !oText.isEmpty() )
        fGridX = oText.toDouble();
    else
        return;

    // grid y
    oInText.sprintf( "%f", fGridY );
    oText = QInputDialog::getText( g_pWindow, "Sculptor", "Enter new origin y coord:", QLineEdit::Normal,
                                   oInText, &bOK );
    if ( bOK && !oText.isEmpty() )
        fGridY = oText.toDouble();
    else
        return;

    // grid z
    oInText.sprintf( "%f", fGridZ );
    oText = QInputDialog::getText( g_pWindow, "Sculptor", "Enter new origin z coord:", QLineEdit::Normal,
                                   oInText, &bOK );
    if ( bOK && !oText.isEmpty() )
        fGridZ = oText.toDouble();
    else
        return;

    m_cVolume.setGrid( fGridX, fGridY, fGridZ );

    // update info panel
    m_pSitusPropDlg->updateTabInfo();

    // scale and rebuild DLs
    updateRedraw(false);

    setChanged();
};

/**
 * method called from the swapaxis dialog to start the swapaxis dialog
 */
void situs_document::swapaxis_DOIT()
{
	unsigned int iOldExtX = m_cVolume.getSizeX();
	unsigned int iOldExtY = m_cVolume.getSizeY();
	unsigned int iOldExtZ = m_cVolume.getSizeZ();

	unsigned int iNewExtX = iOldExtX;
	unsigned int iNewExtY = iOldExtY;
	unsigned int iNewExtZ = iOldExtZ;

	bool bOldX = false;
	bool bOldY = false;
	bool bOldZ = false;

	int iNewX = m_pSwapaxisDlg->getOldX();
	int iNewY = m_pSwapaxisDlg->getOldY();
	int iNewZ = m_pSwapaxisDlg->getOldZ();

	// new x
	switch (iNewX)
		{
		case SITUS_DOC_SWAPAXIS_OLD_X_X:
			iNewExtX = iOldExtX;
			bOldX = !bOldX;
			break;
		case SITUS_DOC_SWAPAXIS_OLD_X_Y:
			iNewExtX = iOldExtY;
			bOldY = !bOldY;
			break;
		case SITUS_DOC_SWAPAXIS_OLD_X_Z:
			iNewExtX = iOldExtZ;
			bOldZ = !bOldZ;
			break;
	    }

	// new y
	switch (iNewY)
	    {
		case SITUS_DOC_SWAPAXIS_OLD_Y_X:
			iNewExtY = iOldExtX;
			bOldX = !bOldX;
			break;
		case SITUS_DOC_SWAPAXIS_OLD_Y_Y:
			iNewExtY = iOldExtY;
			bOldY = !bOldY;
			break;
		case SITUS_DOC_SWAPAXIS_OLD_Y_Z:
			iNewExtY = iOldExtZ;
			bOldZ = !bOldZ;
			break;
	    }

	// new z
	switch (iNewZ)
	    {
		case SITUS_DOC_SWAPAXIS_OLD_Z_X:
			iNewExtZ = iOldExtX;
			bOldX = !bOldX;
			break;
		case SITUS_DOC_SWAPAXIS_OLD_Z_Y:
			iNewExtZ = iOldExtY;
			bOldY = !bOldY;
			break;
		case SITUS_DOC_SWAPAXIS_OLD_Z_Z:
			iNewExtZ = iOldExtZ;
			bOldZ = !bOldZ;
			break;
	    }

	if ( !bOldX || !bOldY || !bOldZ )
	    {
	    QMessageBox::warning(0, QString("Swap Axes"), QString("You have to assign each of the old axes to a new axis."));
	    return;
	    }

	// Figure out new indices
    unsigned int iFactX = 1;
    unsigned int iFactY = (iNewExtX);
    unsigned int iFactZ = (iNewExtX*iNewExtY);

     // factors x
    if (iNewY == SITUS_DOC_SWAPAXIS_OLD_Y_X)
       iFactX = iNewExtX;
    if (iNewZ == SITUS_DOC_SWAPAXIS_OLD_Z_X)
	   iFactX = iNewExtX * iNewExtY;
     // factors y
    if (iNewX == SITUS_DOC_SWAPAXIS_OLD_X_Y)
    	iFactY = 1;
    if (iNewZ == SITUS_DOC_SWAPAXIS_OLD_Z_Y)
    	iFactY = iNewExtX * iNewExtY;
        // factors z
    if (iNewX == SITUS_DOC_SWAPAXIS_OLD_X_Z)
    	iFactZ = 1;
    if (iNewY == SITUS_DOC_SWAPAXIS_OLD_Y_Z)
    	iFactZ = iNewExtX;

     // flip x,y,z
    int iFlipX = 0;
    int iFlipY = 0;
    int iFlipZ = 0;

    if (m_pSwapaxisDlg->getAxisXcheck())
    	iFlipX = iOldExtX - 1;
    if (m_pSwapaxisDlg->getAxisYcheck())
    	iFlipY = iOldExtY - 1;
    if (m_pSwapaxisDlg->getAxisZcheck())
    	iFlipZ = iOldExtZ - 1;

    svt_volume<Real64> oNewVolume( iNewExtX, iNewExtY, iNewExtZ );

	// set origin and voxel width
    oNewVolume.setGrid(m_cVolume.getGridX(), m_cVolume.getGridY(), m_cVolume.getGridZ());
	oNewVolume.setWidth(m_cVolume.getWidth());

	// copy data
	for(int iX = 0; iX < (int) iOldExtX; iX++)
	   for(int iY = 0; iY < (int) iOldExtY; iY++)
	      for(int iZ = 0; iZ < (int) iOldExtZ; iZ++)
	    	  oNewVolume.setAt( (abs(iFlipX - iX) * iFactX) + (abs(iFlipY - iY) * iFactY) + (abs(iFlipZ - iZ) * iFactZ), m_cVolume.at( iX, iY, iZ ) );


	m_cVolume = oNewVolume;

	dataChanged();

	updateRedraw(/* scale and rebuild DLs */ true);

};


/**
 * swap axis
 */
void situs_document::swapaxis()
{

    if ( m_pSwapaxisDlg == NULL )
    	m_pSwapaxisDlg = new dlg_swapaxis(g_pWindow, this);


    m_pSwapaxisDlg->raise();
    m_pSwapaxisDlg->show();


 };


/**
 * DPSV filter
 */
void situs_document::DPSV_filter()
{
 if ( m_pDPSV_filterDlg == NULL )
    	m_pDPSV_filterDlg = new dlg_DPSV_filter(g_pWindow, this,  m_cVolume);

    m_pDPSV_filterDlg->raise();
    m_pDPSV_filterDlg->show();


 };

/**
 * method called from the swapaxis dialog to start the swapaxis dialog
 */
void situs_document::DPSV_filter_DOIT()
{
    Real64 fBeta=m_pDPSV_filterDlg->getBeta();
    int iMaskSize = m_pDPSV_filterDlg->getMaskSize();
    int iPathLength= m_pDPSV_filterDlg->getPathLength();
    int iNeigborModel=m_pDPSV_filterDlg->getModelOfNeigborhood();
    m_cVolume.filtrGeodesicPath(fBeta, double(iNeigborModel), iMaskSize, iPathLength,  true);
   //hide dialog after work is done
    m_pDPSV_filterDlg->accept();
    
	dataChanged();

	updateRedraw(/* scale and rebuild DLs */ true);
};


/**
 * change the origin of the volume data
 */
void situs_document::sChangeOrigin()
{
    if (m_pSitusPropDlg != NULL)
    {
	svt_vector3<Real64> oOrigin = m_pSitusPropDlg->getOrigin();
	Real64 fGridX = oOrigin.x();
        Real64 fGridY = oOrigin.y();
        Real64 fGridZ = oOrigin.z();

        if (fGridX != m_cVolume.getGridX() || fGridY != m_cVolume.getGridY() || fGridZ != m_cVolume.getGridZ())
        {
            m_cVolume.setGrid( fGridX, fGridY, fGridZ );

	    updateRedraw(/* scale and rebuild DLs */ false);

            setChanged();
        }
    }
};

/**
 * set the voxel-width of the situs data
 */
void situs_document::changeVoxelWidth()
{
    Real64 fWidth;

    bool bOK = false;
    QString oInText;
    oInText.sprintf( "%f", m_cVolume.getWidth() );
    QString oText = QInputDialog::getText( g_pWindow, "Sculptor", "Enter new voxel size:", QLineEdit::Normal,
                                           oInText, &bOK );
    if ( bOK && !oText.isEmpty() )
        fWidth = oText.toDouble();
    else
        return;

    svtout << "Changing voxelwidth to " << fWidth << endl;

    m_cVolume.setWidth( fWidth );

    // update info tab
    if ( m_pSitusPropDlg != NULL )
	m_pSitusPropDlg->updateTabInfo();

    // scale and rebuild DLs
    updateRedraw(true);

    //
    // Spider files do not store the voxelwidth in the file, in that case the scene should get saved
    // as the voxelwidth is also stored in the scene files
    //
    QString oFilename = m_oFileInfo.fileName();
    if (   oFilename.right(3).toLower().compare( QString("spi") )    == 0
        || oFilename.right(3).toLower().compare( QString("SPI") )    == 0
        || oFilename.right(6).toLower().compare( QString("spider") ) == 0
        || oFilename.right(6).toLower().compare( QString("SPIDER") ) == 0 )
        g_pWindow->getDocumentList()->getDoc(0)->setChanged();
    else
        setChanged();
};

/**
 * set the voxel-width of the situs data
 */
void situs_document::sChangeVoxelWidth()
{
    
    Real64 fWidth = m_pSitusPropDlg->getVoxelWidth();

    if (fWidth != m_cVolume.getWidth())
    {
        svtout << "Changing voxelwidth to " << fWidth << endl;

        m_cVolume.setWidth( fWidth );

	updateRedraw(/* scale and rebuild DLs */ true);

        //
        // Spider files do not store the voxelwidth in the file, in that case the scene should get saved as the voxelwidth is also stored in the scene files
        //
        QString oFilename = m_oFileInfo.fileName();
        if (   oFilename.right(3).toLower().compare( QString("spi") )    == 0
            || oFilename.right(3).toLower().compare( QString("SPI") )    == 0
            || oFilename.right(6).toLower().compare( QString("spider") ) == 0
            || oFilename.right(6).toLower().compare( QString("SPIDER") ) == 0 )
            g_pWindow->getDocumentList()->getDoc(0)->setChanged();
        else
            setChanged();
    }
};

/**
 * get gradient value for x axis at a certain position
 * \param iX x coord
 * \param iY y coord
 * \param iZ z coord
 */
Real64 situs_document::getGradientX(int iX, int iY, int iZ)
{
    return m_cGradientX.getValue(iX, iY, iZ);
}
/**
 * get gradient value for y axis at a certain position
 * \param iX x coord
 * \param iY y coord
 * \param iZ z coord
 */
Real64 situs_document::getGradientY(int iX, int iY, int iZ)
{
    return m_cGradientY.getValue(iX, iY, iZ);
}
/**
 * get gradient value for z axis at a certain position
 * \param iX x coord
 * \param iY y coord
 * \param iZ z coord
 */
Real64 situs_document::getGradientZ(int iX, int iY, int iZ)
{
    return m_cGradientZ.getValue(iX, iY, iZ);
}

/**
 * get maximum density
 * \return maximum
 */
float situs_document::getMaxDensity()
{
    return (float)(m_cVolume.getMaxDensity());
}
/**
 * get minimum density
 * \return minimum
 */
float situs_document::getMinDensity()
{
    return (float)(m_cVolume.getMinDensity());
}

/**
 * get the position of the situs document in real space (e.g. for pdb or situs files)
 * \return svt_vector4f with the position
 */
svt_vector4f situs_document::getRealPos()
{
    svt_vector4f oVec((m_cVolume.getGridX()*1.0E-10f) + (m_cVolume.getWidth()*(m_cVolume.getSizeX()-1)*0.5E-10f),
                      (m_cVolume.getGridY()*1.0E-10f) + (m_cVolume.getWidth()*(m_cVolume.getSizeY()-1)*0.5E-10f),
		      (m_cVolume.getGridZ()*1.0E-10f) + (m_cVolume.getWidth()*(m_cVolume.getSizeZ()-1)*0.5E-10f));

    return oVec;
}

/**
 * set the subregion that is selected
 * \param iMinX minimum x coordinate/index
 * \param iMaxX maximum x coordinate/index
 * \param iMinY minimum y coordinate/index
 * \param iMaxY maximum y coordinate/index
 * \param iMinZ minimum z coordinate/index
 * \param iMaxZ maximum z coordinate/index
 */
void situs_document::setSelection( unsigned int iMinX, unsigned int iMaxX, unsigned int iMinY, unsigned int iMaxY, unsigned int iMinZ, unsigned int iMaxZ )
{
    iMaxX++;
    iMaxY++;
    iMaxZ++;

    Real64 fSizeX = (Real64)(iMaxX - iMinX);
    Real64 fSizeY = (Real64)(iMaxY - iMinY);
    Real64 fSizeZ = (Real64)(iMaxZ - iMinZ);

    // top left corner
    Real64 fTX = (-(Real64)(m_cVolume.getSizeX()) / 2.0) + (Real64)(iMinX) + (fSizeX*0.5);
    Real64 fTY = (-(Real64)(m_cVolume.getSizeY()) / 2.0) + (Real64)(iMinY) + (fSizeY*0.5);
    Real64 fTZ = (-(Real64)(m_cVolume.getSizeZ()) / 2.0) + (Real64)(iMinZ) + (fSizeZ*0.5);

    fTX *= m_cVolume.getWidth() * 1.0E-2;
    fTY *= m_cVolume.getWidth() * 1.0E-2;
    fTZ *= m_cVolume.getWidth() * 1.0E-2;

    fSizeX *= m_cVolume.getWidth() * 1.0E-2;
    fSizeY *= m_cVolume.getWidth() * 1.0E-2;
    fSizeZ *= m_cVolume.getWidth() * 1.0E-2;

    m_pVolumeSelectionCube->setPos( new svt_pos(fTX, fTY, fTZ) );
    m_pVolumeSelectionCube->setSize( new svt_pos(fSizeX, fSizeY, fSizeZ) );
    m_pVolumeSelectionCube->setScale( 1.0, 1.0, 1.0 );
    m_pVolumeSelectionCube->rebuildDL();
};

///////////////////////////////////////////////////////////////////////////////
// private functions
///////////////////////////////////////////////////////////////////////////////

/**
 * read situs file
 * \param pFilename pointer to array of char with the filename
 */
void situs_document::loadSitus(const char* pFilename)
{
    m_cVolume.loadSitus( pFilename );

    m_cF2C_bw.setMin(m_cVolume.getMinDensity());
    m_cF2C_bw.setMax(m_cVolume.getMaxDensity());
    m_cF2C_bw.update();
    m_cF2C_rwb.setMin(m_cVolume.getMinDensity());
    m_cF2C_rwb.setMax(m_cVolume.getMaxDensity());
    m_cF2C_rwb.update();
    m_cF2C_ryg.setMin(m_cVolume.getMinDensity());
    m_cF2C_ryg.setMax(m_cVolume.getMaxDensity());
    m_cF2C_ryg.update();
    m_cF2C_rgb.setMin(m_cVolume.getMinDensity());
    m_cF2C_rgb.setMax(m_cVolume.getMaxDensity());
    m_cF2C_rgb.update();
};

/**
 * read mrc file
 * \param pFilename pointer to array of char with the filename
 */
void situs_document::loadMRC(const char* pFilename)
{
    m_cVolume.loadMRC( pFilename );

    m_cF2C_bw.setMin(m_cVolume.getMinDensity());
    m_cF2C_bw.setMax(m_cVolume.getMaxDensity());
    m_cF2C_bw.update();
    m_cF2C_rwb.setMin(m_cVolume.getMinDensity());
    m_cF2C_rwb.setMax(m_cVolume.getMaxDensity());
    m_cF2C_rwb.update();
    m_cF2C_ryg.setMin(m_cVolume.getMinDensity());
    m_cF2C_ryg.setMax(m_cVolume.getMaxDensity());
    m_cF2C_ryg.update();
    m_cF2C_rgb.setMin(m_cVolume.getMinDensity());
    m_cF2C_rgb.setMax(m_cVolume.getMaxDensity());
    m_cF2C_rgb.update();
};

/**
 * read Spider file
 * \param pFilename pointer to array of char with the filename
 */
void situs_document::loadSpider(const char* pFilename)
{
    m_cVolume.loadSpider( pFilename );

    m_cF2C_bw.setMin(m_cVolume.getMinDensity());
    m_cF2C_bw.setMax(m_cVolume.getMaxDensity());
    m_cF2C_bw.update();
    m_cF2C_rwb.setMin(m_cVolume.getMinDensity());
    m_cF2C_rwb.setMax(m_cVolume.getMaxDensity());
    m_cF2C_rwb.update();
    m_cF2C_ryg.setMin(m_cVolume.getMinDensity());
    m_cF2C_ryg.setMax(m_cVolume.getMaxDensity());
    m_cF2C_ryg.update();
    m_cF2C_rgb.setMin(m_cVolume.getMinDensity());
    m_cF2C_rgb.setMax(m_cVolume.getMaxDensity());
    m_cF2C_rgb.update();
};

/**
 * if the data has changed the isosurface is updated
 */
void situs_document::dataChanged()
{
    setChanged();

    float fMax = m_cVolume.getMaxDensity();
    float fMin = m_cVolume.getMinDensity();

    Real64 fIsoLevel = m_pVolumeMC->getIsolevel();

    if (fIsoLevel < fMin)
	fIsoLevel = fMin;
    if (fIsoLevel > fMax)
	fIsoLevel = fMax;

    m_pVolumeMC->setIsolevel( fIsoLevel );
    m_pVolumeMC->recalcMesh();

    if ( m_pVolume3DText->getVisible() == true )
    {
        m_pVolume3DText->recalcTextures();
        m_pVolume3DText->rebuildDL();
    }

    calcHistogram();
    resetTF();

    m_cF2C_bw.setMax(m_cVolume.getMaxDensity());
    m_cF2C_bw.setMin(m_cVolume.getMinDensity());
    m_cF2C_bw.update();
    m_cF2C_rwb.setMax(m_cVolume.getMaxDensity());
    m_cF2C_rwb.setMin(m_cVolume.getMinDensity());
    m_cF2C_rwb.update();
    m_cF2C_ryg.setMax(m_cVolume.getMaxDensity());
    m_cF2C_ryg.setMin(m_cVolume.getMinDensity());
    m_cF2C_ryg.update();
    m_cF2C_rgb.setMax(m_cVolume.getMaxDensity());
    m_cF2C_rgb.setMin(m_cVolume.getMinDensity());
    m_cF2C_rgb.update();
    m_cF2O_constant.setMax(m_cVolume.getMaxDensity());
    m_cF2O_constant.setMin(m_cVolume.getMinDensity());
    m_cF2O_constant.update();
    m_cF2O_linear.setMax(m_cVolume.getMaxDensity());
    m_cF2O_linear.setMin(m_cVolume.getMinDensity());
    m_cF2O_linear.update();
    m_cF2M_points.setMax(m_cVolume.getMaxDensity());
    m_cF2M_points.setMin(m_cVolume.getMinDensity());
    m_cF2M_points.update();

    if (m_pSitusPropDlg != NULL)
    {
	m_pSitusPropDlg->updatePlots();
	m_pSitusPropDlg->updateTabInfo();
    }

    if (m_pMapExplorer != NULL)
        m_pMapExplorer->dataChanged();
};

/**
 * switch docking data to laplace data
 */
void situs_document::setDockToLaplace(bool b)
{
    m_bDockToLaplaceData = b;
}

/**
 * get state of docking: laplace or spatial
 */
bool situs_document::getDockToLaplace()
{
    return m_bDockToLaplaceData;
}

/**
 * Set the cutoff for the clustering
 * \param fCutoff only voxels above this value are considered.
 */
void situs_document::setCutoff( Real64 fCutoff )
{
    m_cVolume.setCutoff( fCutoff );
}

/**
 * Return the special svt_sampled object for clustering
 */
svt_sampled< svt_vector4<Real64> >& situs_document::getSampled()
{
    if (!m_bDockToLaplaceData)
	return m_cVolume;
    else
        return m_cVolumeLaplace;
};

/**
 * Return a pointer to the volume data
 */
svt_volume<Real64>* situs_document::getVolume()
{
    return &m_cVolume;
};

/**
 * Return a pointer to the 3d texture data
 */
svt_volume_3dtextures<Real64>* situs_document::get3DText()
{
    return  m_pVolume3DText;
}

/**
 * Return a pointer to the volume slices data
 */
svt_volume_slices<Real64>* situs_document::getSlices()
{
    return m_pVolumeSlices;
}

/**
 * Return a pointer to the laplace-filtered volume data
 */
svt_volume<Real64>* situs_document::getLaplaceVol()
{
    if ( getDockToLaplace() == false )
    {
	// calculate Laplace-filtered volume
	svtout << "Generating Laplace filtered volume data." << endl;
	m_cVolumeLaplace.deepCopy( m_cVolume );
	m_cVolumeLaplace.applyLaplacian();
	m_cVolumeLaplace * (-1.0);
	m_cVolumeLaplace.threshold(0.0);
	m_cVolumeLaplace.normalize();
	m_cVolumeLaplace.setWidth( m_cVolume.getWidth() );
    }

    return &m_cVolumeLaplace;
};

/**
 * Calculate number of occupied voxels
 */
void situs_document::calcOccupied()
{
    unsigned long iOcc = 0;
    Real64 fThreshold = 0.0;

    // threshold
    bool bOK = false;
    QString oInText;
    oInText.sprintf( "3.0" );
    QString oText = QInputDialog::getText( g_pWindow, "Sculptor", "Enter density threshold:", QLineEdit::Normal,
                                           oInText, &bOK );
    if ( bOK && !oText.isEmpty() )
        fThreshold = oText.toDouble();
    else
        return;

    if ( getDockToLaplace() == false )
	iOcc = m_cVolume.getOccupied( fThreshold );
    else
	iOcc = m_cVolumeLaplace.getOccupied( fThreshold );

    // display result
    oInText.sprintf( "Number of occupied voxels: %lu", iOcc );
    QMessageBox::information(0, "Sculptor", oInText);
}

/**
 * Apply a gaussian kernel filter to volume
 */
void situs_document::gaussian()
{
    Real64 fSigma = 1.0;//m_cVolume.getWidth();
    Real64 fSigmaFactor = 3.0;
    Real64 fDouble;
    bool bOk = false;

    fDouble = QInputDialog::getDouble( g_pWindow, "Question", "Sigma1D (voxel units):", fSigma, 0, 1000, 4, &bOk );
    if ( bOk )
	fSigma = fDouble;
    else
        return;

    fDouble = QInputDialog::getDouble( g_pWindow, "Question", "Max radius Gaussian, sigma1D units:", fSigmaFactor, 0, 1000, 4, &bOk );
    if ( bOk )
	fSigmaFactor = fDouble;
    else
        return;

    if (fSigmaFactor<3)
        svt_exception::ui()->info("The Gaussian radius is too small causing a \"truncated\" Gaussian!\n This may result in artifacts during blurring.");
        

    svt_volume<Real64> oGaussian;
    oGaussian.createGaussian( fSigma, fSigmaFactor );

    svtout << "Size of Gaussian: " << oGaussian.getSizeX() << " x " << oGaussian.getSizeY() << " x " << oGaussian.getSizeZ() << endl;

    m_cVolume.convolve( oGaussian, true );

    dataChanged();
}

/**
 * Apply a laplacian kernel filter to volume
 */
void situs_document::laplacian()
{
    m_cVolume.applyLaplacian( );
    dataChanged();
}

/**
 * Apply a local normalization
 */
void situs_document::localNorm()
{
    Real64 fSigma1D = 2.5;
    Real64 fDouble;
    bool bOk = false;

    fDouble = QInputDialog::getDouble( g_pWindow, "Question", " Sigma:", fSigma1D, 0, 20, 4, &bOk );
    if ( bOk )
	    fSigma1D = fDouble;
    else
        return;

    m_cVolume.locallyNormalizeCorrectBorders(fSigma1D, true);
    dataChanged();
};

/**
 * Apply a bilateral Filter to volume
 */
void situs_document::bilateralFilter()
{
    Real64 fSigma1D1 = 2.0;
    Real64 fSigma1D2 = 2.0;
    Real64 fSize = 3.0;

    unsigned int iSize = (int)fSize ;

    Real64 fDouble;
    bool bOk = false;

    fDouble = QInputDialog::getDouble( g_pWindow, "Question", " Sigma S:", fSigma1D1, 0, 10, 4, &bOk );
    if ( bOk )
	fSigma1D1 = fDouble;
    else
        return;

    fDouble = QInputDialog::getDouble( g_pWindow, "Question", " Sigma R :", fSigma1D2, 0, 10, 4, &bOk );
    if ( bOk )
	fSigma1D2 = fDouble;
    else
        return;

    fDouble = QInputDialog::getDouble( g_pWindow, "Question", "Kernel size:", fSize, 0, 10, 4, &bOk );
    if ( bOk )
    iSize = (int)fDouble;
    else
        return;

    m_cVolume.filtrBilateral(fSigma1D1,fSigma1D2,iSize,true);

    dataChanged();
}

/**
 *
 */
sculptor_mapexplorer* situs_document::createMapExplorer()
{
    if ( m_pMapExplorer == NULL )
        m_pMapExplorer = new sculptor_mapexplorer( this );

    QString oStr;
    oStr.sprintf("Map Explorer (%s)", getDisplayName().c_str());

    // show the dialog of the map explorer
    m_pMapExplorer->showDlg(oStr);

    return m_pMapExplorer;
}

/**
 *
 */
sculptor_mapexplorer* situs_document::getMapExplorer()
{
    return m_pMapExplorer;
}

/**
 * Select a voxel
 */
void situs_document::selectVoxel( int iX, int iY, int iZ )
{
    m_aSelectedVoxels.push_back( svt_vector4<Real64>(iX,iY,iZ) );

    // add a container for the selected voxels to the scenegraph
    if (m_pSelVox == NULL)
    {
	m_pSelVox = new svt_container();
	m_pSelVox->setScale( m_cVolume.getWidth()*1.0E-2 );
	m_pVolNode->add( m_pSelVox );
    }

    // render all selected voxels
    Real64 fOriginX = (Real64)(m_cVolume.getSizeX()) * 0.5;
    Real64 fOriginY = (Real64)(m_cVolume.getSizeY()) * 0.5;
    Real64 fOriginZ = (Real64)(m_cVolume.getSizeZ()) * 0.5;

    m_pSelVox->delAll();
    for(unsigned int i=0; i<m_aSelectedVoxels.size(); i++)
    {
	m_pSelVox->addSphere( m_aSelectedVoxels[i].x() - fOriginX, m_aSelectedVoxels[i].y() - fOriginY, m_aSelectedVoxels[i].z() - fOriginZ, 1.0f, svt_color( 1.0, 1.0, 1.0) );
    }
    m_pSelVox->rebuildDL();
}

/**
 * UnSelect all voxels
 */
void situs_document::unselectAllVoxel( int, int, int )
{
    if (m_aSelectedVoxels.size() > 0)
    {
	m_aSelectedVoxels.pop_back();

	// add a container for the selected voxels to the scenegraph
	if (m_pSelVox == NULL)
	{
	    m_pSelVox = new svt_container();
	    m_pSelVox->setScale( 1.0E-2 );
	    m_pVolNode->add( m_pSelVox );
	}

	// render all selected voxels
	Real64 fOriginX = (Real64)(m_cVolume.getSizeX()) * 0.5;
	Real64 fOriginY = (Real64)(m_cVolume.getSizeY()) * 0.5;
	Real64 fOriginZ = (Real64)(m_cVolume.getSizeZ()) * 0.5;

	if (m_aSelectedVoxels.size() == 0)
	{
	    m_pSelVox->delAll();
	    m_pSelVox->rebuildDL();
	}
	else
	{
	    m_pSelVox->delAll();

	    for(unsigned int i=0; i<m_aSelectedVoxels.size(); i++)
		m_pSelVox->addSphere( m_aSelectedVoxels[i].x() - fOriginX, m_aSelectedVoxels[i].y() - fOriginY, m_aSelectedVoxels[i].z() - fOriginZ, 1.0f, svt_color( 1.0, 1.0, 1.0) );

	    m_pSelVox->rebuildDL();

	}
    }
}

///////////////////////////////////////////////////////////////////////////////
// Segmentation functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Load a new segmentation file
 */
void situs_document::loadSegmentation()
{
    // open file chooser dialog
    QString oFilter( "Segmentation Volume (*.situs)" );
    QFileDialog oFD(NULL);
    oFD.setNameFilter( oFilter );
    oFD.setViewMode( QFileDialog::List );
    oFD.selectNameFilter( oFilter );
    oFD.setFileMode( QFileDialog::ExistingFile );

    if ( oFD.exec() == QDialog::Accepted )
    {
        QStringList oFiles = oFD.selectedFiles();

        m_cSegVol.loadSitus( QtoStd(oFiles[0]).c_str() );
    };
};

/**
 * Save a new segmentation file
 */
void situs_document::saveSegmentation()
{
    // open file chooser dialog
    QString oFilter( "Segmentation Volume (*.situs)" );
    QFileDialog oFD(NULL);
    oFD.setFileMode( QFileDialog::AnyFile );
    oFD.setNameFilter( oFilter );
    oFD.setViewMode( QFileDialog::List );
    oFD.selectNameFilter( oFilter );

    if ( oFD.exec() == QDialog::Accepted )
    {
        QStringList oFiles = oFD.selectedFiles();

        m_cSegVol.saveSitus( QtoStd(oFiles[0]).c_str() );
    };
};

/**
 * Set segment
 */
void situs_document::setSegment()
{
    bool bOK = false;
    QString oInText;
    QString oText;
    unsigned int iSegment = 1;

    // minimum density threshold
    oInText.sprintf( "1" );
    oText = QInputDialog::getText( g_pWindow, "Sculptor", "Number/ID of segment to be shown:", QLineEdit::Normal,
                                   oInText, &bOK );

    if ( bOK && !oText.isEmpty() )
        iSegment = oText.toInt();
    else
        return;

    m_pVolumeMC->setSegSel( iSegment );
    m_pVolumeMC->recalcMesh();

    // if (m_pMapExplorer != NULL)
    //     FIXME
};

/**
 * Show the segmentation map in the mapexplorer
 */
void situs_document::showSegmentation()
{
    // if (m_pMapExplorer != NULL)
    //     FIXME
};

/**
 * Is the box enabled?
 */
bool situs_document::getBoxEnabled()
{
    return m_pVolumeCube->getVisible();
}

/**
 * Get the size of the document
 * \return svt_vector4 with the three dimensions of the size of the document
 */
svt_vector4<Real32> situs_document::getBoundingBox()
{
    return svt_vector4<Real32>(m_cVolume.getWidth()*(double)(m_cVolume.getSizeX())*1.0E-2,
	                       m_cVolume.getWidth()*(double)(m_cVolume.getSizeY())*1.0E-2,
			       m_cVolume.getWidth()*(double)(m_cVolume.getSizeZ())*1.0E-2);
};

/**
 * SLOT start slice slider was changed
 */
void situs_document::sStartSlice(int iStartSlice)
{
    m_pVolume3DText->setStartSlice( (Real32)(iStartSlice) / 99.0f );
    svt_redraw();
};

/**
 * SLOT start slice slider was changed
 */
void situs_document::sEndSlice(int iEndSlice)
{
    m_pVolume3DText->setEndSlice( (Real32)(iEndSlice) / 99.0f );
    svt_redraw();
};

/**
 * method called from the floodfill dialog to start the floodfill dialog
 */
void situs_document::floodfill_DOIT()
{
    unsigned int iX = m_pFloodfillDlg->getVoxelX();
    unsigned int iY = m_pFloodfillDlg->getVoxelY();
    unsigned int iZ = m_pFloodfillDlg->getVoxelZ();

    Real64 fThreshold = m_pFloodfillDlg->getThreshold();
    Real64 fSigma = m_pFloodfillDlg->getSigma();

    m_cVolume.floodfill_segmentation( iX, iY, iZ, fThreshold, fSigma );

    dataChanged();
};

/**
 * method called from the floodfill dialog to start the floodfill dialog
 */
void situs_document::multiPointFloodfill_DOIT()
{
    unsigned int iPointCount = m_pMpFloodfillDlg->getPointCount();
    Real64 fThreshold = m_pMpFloodfillDlg->getThreshold();
    Real64 fSigma = m_pMpFloodfillDlg->getSigma();

    svt_volume<Real64> oSeg, oTmp, oGaussian;

    oGaussian.createGaussian(fSigma, 3.0);
    m_cVolume.convolve(oGaussian, true);
    oSeg = m_cVolume;
    oSeg.setValue(0.0);

    svt_vector4<Real64> oSize;
    oSize.x(m_cVolume.getSizeX());
    oSize.y(m_cVolume.getSizeY());
    oSize.z(m_cVolume.getSizeZ());

    vector<sVoxel> oVoxels;

    for (unsigned iIndexX=0; iIndexX< oSize.x(); iIndexX++)
    {
        for (unsigned iIndexY=0; iIndexY< oSize.y(); iIndexY++)
            for (unsigned iIndexZ=0; iIndexZ< oSize.z(); iIndexZ++)
            {
               sVoxel oVox;
               oVox.fIntensity = m_cVolume.at(iIndexX, iIndexY, iIndexZ);
               oVox.oPos.x(iIndexX);
               oVox.oPos.y(iIndexY);
               oVox.oPos.z(iIndexZ);
               oVoxels.push_back(oVox);
            }
    }
    sort(oVoxels.rbegin(), oVoxels.rend());
     
    svt_point_cloud_pdb<svt_vector4<Real64> > oPDB;
    svt_point_cloud_atom oAtom;
    svt_vector4<Real64> oVec;
  
    int iIndex = 0;
    int iRunFloodfill = 0; 
    while (iIndex<(int)oVoxels.size() && iRunFloodfill < iPointCount)
    {
        if (oSeg.at(oVoxels[iIndex].oPos.x(), oVoxels[iIndex].oPos.y(),oVoxels[iIndex].oPos.z())<1e-6 &&  m_cVolume.at(oVoxels[iIndex].oPos.x(), oVoxels[iIndex].oPos.y(),oVoxels[iIndex].oPos.z()) > fThreshold)
        {
    
            svtout << "Floodfill # " << iRunFloodfill << ":" << oVoxels[iIndex].oPos.x() << " " << oVoxels[iIndex].oPos.y()<< " " << oVoxels[iIndex].oPos.z() << " " << m_cVolume.at(oVoxels[iIndex].oPos.x(), oVoxels[iIndex].oPos.y(),oVoxels[iIndex].oPos.z()) << " " << m_cVolume.at(oVoxels[iIndex].oPos.x(), oVoxels[iIndex].oPos.y(),oVoxels[iIndex].oPos.z()) << endl;
            oVec.x(m_cVolume.getGridX()+m_cVolume.getWidth()*oVoxels[iIndex].oPos.x()); 
            oVec.y(m_cVolume.getGridY()+m_cVolume.getWidth()*oVoxels[iIndex].oPos.y()); 
            oVec.z(m_cVolume.getGridZ()+m_cVolume.getWidth()*oVoxels[iIndex].oPos.z());
            oPDB.addAtom(oAtom, oVec);
  
            oTmp = m_cVolume;
            oTmp.floodfill_segmentation(oVoxels[iIndex].oPos.x(), oVoxels[iIndex].oPos.y(),oVoxels[iIndex].oPos.z(), fThreshold, 2.0);
            oSeg+=oTmp;
            iRunFloodfill++;
        }
        iIndex++;
    };

    m_cVolume = oSeg;

    dataChanged();
};

/**
 * SLOT floodfill algorithm
 */
void situs_document::floodfill()
{
    if ( m_pFloodfillDlg == NULL )
        m_pFloodfillDlg = new dlg_floodfill(g_pWindow, this);

    unsigned int iX = (unsigned int)(m_cVolume.getSizeX() * 0.5);
    unsigned int iY = (unsigned int)(m_cVolume.getSizeY() * 0.5);
    unsigned int iZ = (unsigned int)(m_cVolume.getSizeZ() * 0.5);

    Real64 fMax = m_cVolume.getMaxDensity();
    Real64 fMin = m_cVolume.getMinDensity();

    float fThreshold = (float)((fMax - fMin) * 0.5);
    float fSigma = 2.0f;

    m_pFloodfillDlg->setVoxelX( iX );
    m_pFloodfillDlg->setVoxelY( iY );
    m_pFloodfillDlg->setVoxelZ( iZ );
    m_pFloodfillDlg->setThreshold( fThreshold );
    m_pFloodfillDlg->setSigma( fSigma );

    m_pFloodfillDlg->raise();
    m_pFloodfillDlg->show();
};

/**
 * SLOT floodfill algorithm
 */
void situs_document::multiPointFloodfill()
{
    if ( m_pMpFloodfillDlg == NULL )
        m_pMpFloodfillDlg = new dlg_mpfloodfill(g_pWindow, this);

    Real64 fMax = m_cVolume.getMaxDensity();
    Real64 fMin = m_cVolume.getMinDensity();

    float fThreshold = (float)((fMax - fMin) * 0.5);
    float fSigma = 2.0f;
    unsigned int iPointCount = 20;

    m_pMpFloodfillDlg->setPointCount( iPointCount );
    m_pMpFloodfillDlg->setThreshold( fThreshold );
    m_pMpFloodfillDlg->setSigma( fSigma );

    m_pMpFloodfillDlg->raise();
    m_pMpFloodfillDlg->show();
};

/**
 *
 */
dlg_floodfill* situs_document::createFloodfillDlg()
{
    if ( m_pFloodfillDlg == NULL )
        m_pFloodfillDlg = new dlg_floodfill(g_pWindow, this);

    return m_pFloodfillDlg;
}

/**
 * Some map parameters have changed, update and redraw everything
 * /param bScaleRebuildDLs If true, scale objects and rebuild all DLs
 */
void situs_document::updateRedraw(bool bScaleRebuildDLs)
{
        // update origin
        svt_vector4f oDocOrigin = getRealPos();
        oDocOrigin.multScalar( 1.0E8f );
        getNode()->setPos(new svt_pos(oDocOrigin.x(),oDocOrigin.y(),oDocOrigin.z()));
        storeInitMat();
        g_pWindow->adjustOrigin();

	if (bScaleRebuildDLs)
	{
	    // redraw everything

	    unsigned int iSizeX = m_cVolume.getSizeX();
	    unsigned int iSizeY = m_cVolume.getSizeY();
	    unsigned int iSizeZ = m_cVolume.getSizeZ();

	    double fWidth = m_cVolume.getWidth();

	    m_pVolumeMC->setScale(fWidth*(double)(iSizeX)*1.0E-2,
				  fWidth*(double)(iSizeY)*1.0E-2,
				  fWidth*(double)(iSizeZ)*1.0E-2);
	    m_pVolumeCube->setScale(fWidth*(double)(iSizeX)*1.0E-2,
				    fWidth*(double)(iSizeY)*1.0E-2,
				    fWidth*(double)(iSizeZ)*1.0E-2);
	    m_pVolumeSelectionCube->setScale(fWidth*(double)(iSizeX)*1.0E-2,
					     fWidth*(double)(iSizeY)*1.0E-2,
					     fWidth*(double)(iSizeZ)*1.0E-2);
	    m_pVolumeSlices->setScale(fWidth*(double)(iSizeX)*1.0E-2,
				      fWidth*(double)(iSizeY)*1.0E-2,
                                      fWidth*(double)(iSizeZ)*1.0E-2);

            //
            // the scaling for the direct volume rendering should not get set here, it gets set automatically in the
            // dvr class, which takes the voxelwidth from the svt_volume object
            //
	    //m_pVolume3DText->setScale(fWidth*(double)(iSizeX)*1.0E-2,
	    //                          fWidth*(double)(iSizeY)*1.0E-2,
	    //    		      fWidth*(double)(iSizeZ)*1.0E-2 );

	    m_pVolumeMC->rebuildDL();
	    m_pVolumeCube->rebuildDL();
	    m_pVolumeSelectionCube->rebuildDL();
	    m_pVolumeSlices->rebuildDL();
	    m_pVolume3DText->rebuildDL();
	}

	svt_redraw();
}

/**
 * Return a pointer to the transfer function
 */
svt_float2mat_points* situs_document::getFloat2Mat()
{
    return &m_cF2M_points;
}

/**
 *
 */
void situs_document::setAllowShaderPrograms()
{
    if ( svt_getAllowShaderPrograms() )
    {
        if ( !m_pVolume3DText->getShaderInitialized() )
            m_pVolume3DText->initShader();

        if (m_pSitusPropDlg != NULL)
        {
            m_pSitusPropDlg->initTab3DT();
            m_pSitusPropDlg->updateTab3DT(0);
            m_pSitusPropDlg->set3DTTabEnabled(true);
        }
    }
    else
    {
        if (m_pSitusPropDlg != NULL)
        {
            m_pSitusPropDlg->set3DTTabEnabled(false);
            m_pSitusPropDlg->sEnable3DT(false);
            m_pSitusPropDlg->sEnableIso(true);
        }
    }
}
