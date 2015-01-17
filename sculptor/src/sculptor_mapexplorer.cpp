/***************************************************************************
                          sculptor_mapexplorer.cpp
                          -------------------
    begin                : 10/29/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <situs_document.h>
#include <sculptor_mapexplorer.h>
#include <sculptor_dlg_mapexplorer.h>
#include <sculptor_dlg_floodfill.h>
// qt4 includes
#include <QPaintEvent>
#include <QResizeEvent>
#include <QObject>
#include <QPixmap>
#include <QSize>
#include <QPrintDialog>
#include <QPrinter>
#include <QFileDialog>
#include <QPainter>

extern string QtoStd(QString);

/*
 *
 */
sculptor_mapexplorer::sculptor_mapexplorer(situs_document* pDoc)
    : QObject( NULL ),
      m_pExplorerDlg( NULL ),
      m_pDoc( pDoc ),
      m_pVolume( pDoc->getVolume() ),
      m_iZoom( 3 ),
      m_fGamma( 1.0f ),
      m_fThreshold( 0.5f ),
      m_bGrid( false ),
      m_iSlice( 0 ),
      m_iMapping( SCULPTOR_MAPEXPLORER_CROSSSECTION_X ),
      m_iDisplayMode( SCULPTOR_MAPEXPLORER_GAMMA ),
      m_fMinDensity( (float)pDoc->getVolume()->getMinDensity() ),
      m_fDensityRange( (float)(pDoc->getVolume()->getMaxDensity() - pDoc->getVolume()->getMinDensity()) ),
      m_pAlongX( NULL ),
      m_pAlongY( NULL ),
      m_pAlongZ( NULL )
{
    // set up the default colors
    m_aColors.reserve(256);
    for (int i=0; i<256; ++i)
        m_aColors.push_back( (QColor(i, i, i)) );

    // careful, only good as long as SCULPTOR_MAPEXPLORER_CROSSSECTION_X is default mapping
    m_iSlice = m_pVolume->getSizeX()/2;
}

/*
 *
 */
sculptor_mapexplorer::~sculptor_mapexplorer()
{
    int iSize, i;

    if ( m_pExplorerDlg != NULL )
        delete m_pExplorerDlg;

    if ( m_pAlongX != NULL )
    {
        iSize = sizeof(*m_pAlongX);

        for ( i=0; i< iSize; ++i )
            delete[] m_pAlongX[i];
        delete[] m_pAlongX;
    }
    if ( m_pAlongY != NULL )
    {
        iSize = sizeof(*m_pAlongY);

        for ( i=0; i< iSize; ++i )
            delete[] m_pAlongY[i];
        delete[] m_pAlongY;
    }
    if ( m_pAlongZ != NULL )
    {
        iSize = sizeof(*m_pAlongZ);

        for ( i=0; i< iSize; ++i )
            delete[] m_pAlongZ[i];
        delete[] m_pAlongZ;
    }
}

/*
 *
 */
void sculptor_mapexplorer::showDlg(QString oTitle)
{
    if ( m_pExplorerDlg == NULL )
        m_pExplorerDlg = new dlg_mapexplorer( this, m_pVolume );

    m_pExplorerDlg->setWindowTitle(oTitle);

    m_pExplorerDlg->raise();
    m_pExplorerDlg->show();
}

/*
 *
 */
void sculptor_mapexplorer::sShowFloodfillDialog()
{
    // get a pointer to the floodfill dialog
    dlg_floodfill* pFloodfillDlg = m_pDoc->createFloodfillDlg();

    unsigned int iX, iY, iZ;
    float fDensity;

    if ( !m_pExplorerDlg->getCurrentVoxel(&iX, &iY, &iZ, &fDensity) )
        return;

    // fill in the values
    pFloodfillDlg->setVoxelX( iX );
    pFloodfillDlg->setVoxelY( iY );
    pFloodfillDlg->setVoxelZ( iZ );
    pFloodfillDlg->setThreshold( fDensity );
    pFloodfillDlg->setSigma(2.0f);

    // show it
    pFloodfillDlg->raise();
    pFloodfillDlg->show();
}


/*
 *
 */
dlg_mapexplorer* sculptor_mapexplorer::getDialog()
{
    return m_pExplorerDlg;
}

/**
 *
 */
void sculptor_mapexplorer::dataChanged()
{
    int i, iSize;

    if ( m_pAlongX != NULL )
    {
        iSize = sizeof(*m_pAlongX);

        for ( i=0; i< iSize; ++i )
            delete[] m_pAlongX[i];
        delete[] m_pAlongX;
        m_pAlongX = NULL;
    }
    if ( m_pAlongY != NULL )
    {
        iSize = sizeof(*m_pAlongY);

        for ( i=0; i< iSize; ++i )
            delete[] m_pAlongY[i];
        delete[] m_pAlongY;
        m_pAlongY = NULL;
    }
    if ( m_pAlongZ != NULL )
    {
        iSize = sizeof(*m_pAlongZ);

        for ( i=0; i< iSize; ++i )
            delete[] m_pAlongZ[i];
        delete[] m_pAlongZ;
        m_pAlongZ = NULL;
    }

    // recompute the min density and density range
    m_fMinDensity = (float)m_pDoc->getVolume()->getMinDensity();
    m_fDensityRange =(float)(m_pDoc->getVolume()->getMaxDensity() - m_pDoc->getVolume()->getMinDensity());
    
    m_pExplorerDlg->dataChanged();
}

/*
 *
 */
void sculptor_mapexplorer::setMapping(int iMapping)
{
    m_iMapping = iMapping;
}

/*
 *
 */
int sculptor_mapexplorer::getMapping()
{
    return m_iMapping;
}

/*
 *
 */
void sculptor_mapexplorer::setDisplayMode(int iDisplayMode)
{
    if ( m_iDisplayMode != iDisplayMode )
    {
        m_iDisplayMode = iDisplayMode;

        if ( m_iDisplayMode == SCULPTOR_MAPEXPLORER_GAMMA )
            setGamma( m_fGamma );
        else if ( m_iDisplayMode == SCULPTOR_MAPEXPLORER_THRESHOLD )
            setThreshold( m_fThreshold );
    }
}

/*
 *
 */
int sculptor_mapexplorer::getDisplayMode()
{
    return m_iDisplayMode;
}

/*
 *
 */
void sculptor_mapexplorer::setSlice(int iSlice)
{
    m_iSlice = (unsigned int)iSlice;
}

/*
 *
 */
int sculptor_mapexplorer::getSlice()
{
    return m_iSlice;
}

/*
 *
 */
void sculptor_mapexplorer::setZoom(int iZoom)
{
    m_iZoom = iZoom;
}

/*
 *
 */
int sculptor_mapexplorer::getZoom()
{
    return m_iZoom;
}

/*
 *
 */
void sculptor_mapexplorer::setGamma(float fGamma)
{
    m_fGamma = fGamma;

    float fV = 0.0f, fB;
    float fExp = 1.0f / fGamma;
    float fInc = 1.0f / 255.0f;

    for ( int i=0; i<256; ++i )
    {
        fB = pow( fV, fExp );
        fB *= 255.0f;
        m_aColors[i] = QColor(fB, fB, fB);

        fV+= fInc;
    }
}

/*
 *
 */
float sculptor_mapexplorer::getGamma()
{
    return m_fGamma;
}

/*
 *
 */
void sculptor_mapexplorer::setThreshold(float fThreshold)
{
    m_fThreshold = fThreshold;

    float iV = (int)(fThreshold * 255.0f);

    for ( int i=0; i<256; ++i )
    {
        if ( i < iV )
            m_aColors[i] = QColor(Qt::black);
        else
            m_aColors[i] = QColor(Qt::white);
    }
}

/*
 *
 */
float sculptor_mapexplorer::getThreshold()
{
    return m_fThreshold;
}

/*
 *
 */
unsigned int sculptor_mapexplorer::getDataWidth() const
{
    switch ( m_iMapping )
    {
        case SCULPTOR_MAPEXPLORER_CROSSSECTION_X:
        case SCULPTOR_MAPEXPLORER_PROJECTIONALONG_X:
            return m_pVolume->getSizeZ();
        case SCULPTOR_MAPEXPLORER_CROSSSECTION_Y:
        case SCULPTOR_MAPEXPLORER_PROJECTIONALONG_Y:
            return m_pVolume->getSizeX();
        default:
            return m_pVolume->getSizeX();
    }
}

/*
 *
 */
unsigned int sculptor_mapexplorer::getDataHeight() const
{
    switch ( m_iMapping )
    {
        case SCULPTOR_MAPEXPLORER_CROSSSECTION_X:
        case SCULPTOR_MAPEXPLORER_PROJECTIONALONG_X:
            return m_pVolume->getSizeY();
        case SCULPTOR_MAPEXPLORER_CROSSSECTION_Y:
        case SCULPTOR_MAPEXPLORER_PROJECTIONALONG_Y:
            return m_pVolume->getSizeZ();
        default:
            return m_pVolume->getSizeY();
    }
}

/*
 *
 */
int sculptor_mapexplorer::getDataValue(unsigned int iX, unsigned int iY)
{
    float fDensity;

    switch ( m_iMapping )
    {
        case SCULPTOR_MAPEXPLORER_CROSSSECTION_X:
            // get the density value
            fDensity = (float)m_pVolume->at( m_iSlice, iY, iX );
            // normalize the density value and bring it into the range 0..255
            fDensity -= m_fMinDensity;
            fDensity /= m_fDensityRange;
            fDensity *= 255.0f;
            //printf("%i,%i %f\n", iX, iY, fDensity);
            return (int)fDensity;

        case SCULPTOR_MAPEXPLORER_CROSSSECTION_Y:
            // get the density value
            fDensity = (float) m_pVolume->at( iX, m_iSlice, iY );
            // normalize the density value and bring it into the range 0..255
            fDensity -= m_fMinDensity;
            fDensity /= m_fDensityRange;
            fDensity *= 255.0f;
            return (int)fDensity;

        case SCULPTOR_MAPEXPLORER_CROSSSECTION_Z:
            // get the density value
            fDensity = (float) m_pVolume->at( iX, iY, m_iSlice );
            // normalize the density value and bring it into the range 0..255
            fDensity -= m_fMinDensity;
            fDensity /= m_fDensityRange;
            fDensity *= 255.0f;
            return (int)fDensity;

        case SCULPTOR_MAPEXPLORER_PROJECTIONALONG_X:
            if ( m_pAlongX == NULL )
            {
                m_pAlongX = generateProjection();
                if (m_pAlongX == NULL)
                    return 0;
            }
            return m_pAlongX[iX][iY];

        case SCULPTOR_MAPEXPLORER_PROJECTIONALONG_Y:
            if ( m_pAlongY == NULL )
            {
                m_pAlongY = generateProjection();
                if (m_pAlongY == NULL)
                    return 0;
            }
            return m_pAlongY[iX][iY];

        case SCULPTOR_MAPEXPLORER_PROJECTIONALONG_Z:
            if ( m_pAlongZ == NULL )
            {
                m_pAlongZ = generateProjection();
                if (m_pAlongZ == NULL)
                    return 0;
            }
            return m_pAlongZ[iX][iY];

        default:
            return 0;
    }
}

/*
 *
 */
int** sculptor_mapexplorer::generateProjection()
{
    int i, j, iSizeX, iSizeY;
    int iSlice, iNumSlices;
    float fScalingFactor, fDensitySum, fDensityMax = 0.0f;

    // iSizeX and iSizeY: the X and Y refer to screen coordinates
    iSizeX = getDataWidth();
    iSizeY = getDataHeight();

    // assign memory to the two dimensinal array holding the accumulated densities. bail if that fails
    int** m_pAlongAxis = new int*[iSizeX];
    if ( m_pAlongAxis == NULL )
        return NULL;

    for ( i=0; i<iSizeX; ++i )
    {
        m_pAlongAxis[i] = new int[iSizeY];
        if ( m_pAlongAxis[i] == NULL )
        {
            for (j=0; j<i; ++j)
                delete m_pAlongAxis[j];

            m_pAlongAxis = NULL;
            return NULL;
        }
    }


    // depending on the axis, sum up the density values, and store them in the 2d array. also
    // remember the maximum density
    switch ( m_iMapping )
    {
        case SCULPTOR_MAPEXPLORER_PROJECTIONALONG_X:
            // add up along x axis...
            iNumSlices = m_pVolume->getSizeX();

            for ( i=0; i<iSizeX; ++i )
            { 
                for ( j=0; j<iSizeY; ++j )
                {
                    fDensitySum = 0.0f;

                    for (iSlice=0; iSlice<iNumSlices; ++iSlice)
                    {
                        fDensitySum += m_pVolume->at( iSlice, j, i );
                    }

                    fDensitySum -= (m_fMinDensity*iNumSlices);

                    if (fDensityMax < fDensitySum)
                        fDensityMax = fDensitySum;
                    
                    m_pAlongAxis[i][j] = fDensitySum;
                }
            }
            break;

        case SCULPTOR_MAPEXPLORER_PROJECTIONALONG_Y:
            // add up along y axis...
            iNumSlices = m_pVolume->getSizeY();

            for ( i=0; i<iSizeX; ++i )
            { 
                for ( j=0; j<iSizeY; ++j )
                {
                    fDensitySum = 0.0f;

                    for (iSlice=0; iSlice<iNumSlices; ++iSlice)
                    {
                        fDensitySum += m_pVolume->at( i, iSlice, j );
                    }

                    fDensitySum -= (m_fMinDensity*iNumSlices);

                    if (fDensityMax < fDensitySum)
                        fDensityMax = fDensitySum;
                    
                    m_pAlongAxis[i][j] = fDensitySum;
                }
            }
            break;

        case SCULPTOR_MAPEXPLORER_PROJECTIONALONG_Z:
            // add up along y axis...
            iNumSlices = m_pVolume->getSizeZ();

            for ( i=0; i<iSizeX; ++i )
            { 
                for ( j=0; j<iSizeY; ++j )
                {
                    fDensitySum = 0.0f;

                    for (iSlice=0; iSlice<iNumSlices; ++iSlice)
                    {
                        fDensitySum += m_pVolume->at( i, j, iSlice );
                    }

                    fDensitySum -= (m_fMinDensity*iNumSlices);

                    if (fDensityMax < fDensitySum)
                        fDensityMax = fDensitySum;
                    
                    m_pAlongAxis[i][j] = fDensitySum;
                }
            }
            break;

        default:
            return NULL;

    }

    // now go through over the whole array and scale the values to 0..255
    fScalingFactor = 255.0f / fDensityMax;

    for ( i=0; i<iSizeX; ++i )
    {
       for ( j=0; j<iSizeY; ++j )
       {
           if (m_pAlongAxis[i][j] != 0)
               m_pAlongAxis[i][j] = (int) (((float)m_pAlongAxis[i][j]) * fScalingFactor);
       }
    }

    // return the array
    return m_pAlongAxis;
}

/*
 *
 */
QColor sculptor_mapexplorer::getColor(int iIndex)
{
    return m_aColors[iIndex];
}

/*
 *
 */
void sculptor_mapexplorer::setGridVisible(bool bGrid)
{
    m_bGrid = bGrid;
}

/*
 *
 */
bool sculptor_mapexplorer::getGridVisible()
{
    return m_bGrid;
}

/**
 *
 */
void sculptor_mapexplorer::sPrint()
{
    QPrinter oPrinter;

    QPrintDialog *oDialog = new QPrintDialog(&oPrinter, m_pExplorerDlg);
    oDialog->setWindowTitle(tr("Volume Printing"));

    if (oDialog->exec() != QDialog::Accepted)
        return;

    // determine size of image
    int iWidth, iHeight;
    if ( getGridVisible() )
    {
        iWidth =  getDataWidth()  * (getZoom() + 1) + 1;
        iHeight = getDataHeight() * (getZoom() + 1) + 1;
    }
    else
    {
        iWidth =  getDataWidth()  * getZoom();
        iHeight = getDataHeight() * getZoom();
    }


    // grap pixmap from the label in the explorer dialog
    QPixmap oPixmap = QPixmap::grabWidget( m_pExplorerDlg->getLabel(), QRect(0, 0, iWidth, iHeight) );

    // print it
    QPainter oPainter;
    oPainter.begin(&oPrinter);
    oPainter.drawPixmap(0, 0, oPixmap);
    oPainter.end();
}

/**
 *
 */
void sculptor_mapexplorer::sSave()
{
    // get a filename
    QFileDialog oFD(m_pExplorerDlg);
    QString oFilterPNG_JPG("Image File (*.png *.jpg)");
    oFD.setNameFilter(oFilterPNG_JPG);
    oFD.selectNameFilter(oFilterPNG_JPG);
    oFD.setFileMode(QFileDialog::AnyFile);
    oFD.setAcceptMode( QFileDialog::AcceptSave );

    QString oSel = QString( "screenshot.png" );
    oFD.selectFile( oSel );

    if (oFD.exec() == QDialog::Accepted )
    {
        QString oFilename;
        QStringList oFiles = oFD.selectedFiles();

        if (!oFiles.isEmpty())
            oFilename = oFiles[0];
        else
            return;

        // determine size of image
        int iWidth, iHeight;
        if ( getGridVisible() )
        {
            iWidth =  getDataWidth()  * (getZoom() + 1) + 1;
            iHeight = getDataHeight() * (getZoom() + 1) + 1;
        }
        else
        {
            iWidth =  getDataWidth()  * getZoom();
            iHeight = getDataHeight() * getZoom();
        }


        // grab image from the label in the dialog
        QPixmap oPixmap = QPixmap::grabWidget( m_pExplorerDlg->getLabel(), QRect(0, 0, iWidth, iHeight) );
        

        // save it
        QString oFormat = QString("JPEG");

        if (oFilename.endsWith( ".png"))
            oFormat = QString("PNG");
        else if (oFilename.endsWith( ".jpg"))
            oFormat = QString("JPEG");

        oPixmap.save( oFilename, QtoStd(oFormat).c_str(), -1 );
            
    }    
}
