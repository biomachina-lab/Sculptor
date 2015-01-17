#include <qlayout.h>
#include <q3scrollview.h>
#include <qtooltip.h>
#include <q3buttongroup.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QLabel>
#include <Q3VBoxLayout>

#include <sculptor_mapeditor.h>

/**
 * Constructor
 */
sculptor_mapdialog::sculptor_mapdialog( QWidget *parent, const char *name, Qt::WFlags flags) : QWidget(parent, name, flags | Qt::WNoAutoErase )
{
    initialise();

    QObject::connect(m_zoomScrollBar, SIGNAL(valueChanged(int)),
                     m_mapViewer, SLOT(setZoomScale(int)));
    QObject::connect(m_planeScrollBar, SIGNAL(valueChanged(int)),
                     this, SLOT(getNewCrossSectionSLOT(int)));
    QObject::connect(m_binaryScrollBar, SIGNAL(valueChanged(int)),
                     m_mapViewer, SLOT(setBinaryThreshold(int)));
    QObject::connect(m_gammaScrollBar, SIGNAL(valueChanged(int)),
                     this, SLOT(updateGammaLabelSLOT(int)));
    QObject::connect(m_gammaScrollBar, SIGNAL(valueChanged(int)),
                     m_mapViewer, SLOT(setGammaCurve(int)));
    QObject::connect(m_xPlane, SIGNAL(clicked()),
                     this, SLOT(setXPlaneViewSLOT()));
    QObject::connect(m_yPlane, SIGNAL(clicked()),
                     this, SLOT(setYPlaneViewSLOT()));
    QObject::connect(m_zPlane, SIGNAL(clicked()),
                     this, SLOT(setZPlaneViewSLOT()));
    QObject::connect(m_xProjection, SIGNAL(clicked()),
                     this, SLOT(setXProjectionViewSLOT()));
    QObject::connect(m_yProjection, SIGNAL(clicked()),
                     this, SLOT(setYProjectionViewSLOT()));
    QObject::connect(m_zProjection, SIGNAL(clicked()),
                     this, SLOT(setZProjectionViewSLOT()));
    QObject::connect(m_binaryMode, SIGNAL(clicked()),
                     this, SLOT(setBinaryModeSLOT()));
    QObject::connect(m_normalMode, SIGNAL(clicked()),
                     this, SLOT(setNormalModeSLOT()));
    QObject::connect(m_normalFactor, SIGNAL(clicked()),
                     this, SLOT(setNormalFactorSLOT()));
    QObject::connect(m_factorTwo, SIGNAL(clicked()),
                     this, SLOT(setFactorTwoSLOT()));
    QObject::connect(m_factorFour, SIGNAL(clicked()),
                     this, SLOT(setFactorThreeSLOT()));
    QObject::connect(m_mapViewer, SIGNAL(leftClickedVoxel(int , int)),
                     this, SLOT(getVoxelValueSLOT(int , int)));
    QObject::connect(m_mapViewer, SIGNAL(leftClickedVoxel(int , int)),
                     this, SLOT(leftMBSLOT(int , int)));
    QObject::connect(m_mapViewer, SIGNAL(rightClickedVoxel(int , int)),
                     this, SLOT(rightMBSLOT(int , int)));
    QObject::connect(m_mapViewer, SIGNAL(updateThresholdLabel(int )),
                     this, SLOT(updateBinaryLabelSLOT(int )));
    QObject::connect(m_mapViewer, SIGNAL(clickedUpdateVolumericMapMask(int , int, bool)),
                     this, SLOT(updateVolumericMapMaskSLOT(int , int, bool)));
    QObject::connect(m_mapViewer, SIGNAL(increaseCoord()),
                     this, SLOT(increaseCoordSLOT()));
    QObject::connect(m_mapViewer, SIGNAL(decreaseCoord()),
                     this, SLOT(decreaseCoordSLOT()));
};

/**
 * Initialises the sculptor_mapeditor widget i.e. creates the ScrollBars, radio button, etc,
 * and lays them out in layout managers.
 */
void sculptor_mapdialog::initialise()
{
    Q3VBoxLayout *bottomLayout = new Q3VBoxLayout;
    bottomLayout->setSpacing (10);
    bottomLayout->setResizeMode (QLayout::SetFixedSize);
    Q3ButtonGroup *planeSelection = new Q3ButtonGroup(2, Qt::Vertical, tr("Plane & Cross Section Selection"), this);

    Q3ButtonGroup *planeGroup = new Q3ButtonGroup(3, Qt::Horizontal, tr("Planes"), planeSelection);
    m_xPlane = new QRadioButton(tr("X"), planeGroup);
    m_yPlane = new QRadioButton(tr("Y"), planeGroup);
    m_zPlane = new QRadioButton(tr("Z "), planeGroup);
    m_yPlane->setChecked(true);

    Q3ButtonGroup *scrollBarGroup = new Q3ButtonGroup(2, Qt::Horizontal, tr("Cross Sections"), planeSelection);
    m_planeScrollBar = new QScrollBar(Qt::Horizontal, scrollBarGroup);
    m_planeScrollBar->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, m_planeScrollBar->sizePolicy().hasHeightForWidth() ) );
    m_planeScrollBar->setPageStep(1);
    m_planeScrollBar->setLineStep(1);
    m_planeString = QString::number(m_planeScrollBar->value(), 10);
    m_planeLabel = new QLabel(tr(m_planeString),scrollBarGroup);
    m_planeLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)5, 0, 0, m_planeLabel->sizePolicy().hasHeightForWidth() ) );
    m_voxelLabel = new QLabel(tr(m_voxelString),this);

    bottomLayout->addWidget(m_voxelLabel);
    bottomLayout->addWidget(planeSelection);

    Q3ButtonGroup *projectionGroup = new Q3ButtonGroup(3, Qt::Horizontal, tr("Plane Projection"), this);
    m_xProjection = new QRadioButton(tr("X Projection"), projectionGroup);
    m_yProjection = new QRadioButton(tr("Y Projection"), projectionGroup);
    m_zProjection = new QRadioButton(tr("Z Projection"), projectionGroup);
    bottomLayout->addWidget(projectionGroup);

    Q3ButtonGroup *display = new Q3ButtonGroup(1, Qt::Horizontal, tr("Display"), this);

    Q3ButtonGroup *displayModeGroup = new Q3ButtonGroup(2, Qt::Horizontal, tr("Modes"), display);
    m_binaryMode = new QRadioButton(tr("Binary"), displayModeGroup);
    m_normalMode = new QRadioButton(tr("Normal"), displayModeGroup);
    m_normalMode->setChecked(true);

    Q3ButtonGroup *binaryGroup = new Q3ButtonGroup(2, Qt::Horizontal, tr("Binary Threshold"), display);
    m_binaryScrollBar = new QScrollBar(Qt::Horizontal, binaryGroup);
    m_binaryScrollBar->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, m_binaryScrollBar->sizePolicy().hasHeightForWidth() ) );
    m_binaryScrollBar->setRange(1, 9);
    m_binaryScrollBar->setValue(5);
    m_binaryScrollBar->setPageStep(1);
    m_binaryScrollBar->setLineStep(1);
    m_binaryScrollBar->setEnabled(false);
    m_binaryString = QString::number(m_binaryScrollBar->value() * 10, 10);
    m_binaryString.append(" %");
    m_binaryLabel = new QLabel(tr(m_binaryString),binaryGroup);
    m_binaryLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)5, 0, 0, m_binaryLabel->sizePolicy().hasHeightForWidth() ) );

    Q3ButtonGroup *gammaGroup = new Q3ButtonGroup(2, Qt::Horizontal, tr("Gamma Adjustment"), display);
    m_gammaScrollBar = new QScrollBar(Qt::Horizontal, gammaGroup);
    m_gammaScrollBar->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, m_gammaScrollBar->sizePolicy().hasHeightForWidth() ) );
    m_gammaScrollBar->setRange(1, 30);
    m_gammaScrollBar->setValue(10);
    m_gammaScrollBar->setPageStep(1);
    m_gammaScrollBar->setLineStep(1);
    double value = 1.0;
    m_gammaString = QString("%1" )
        .arg( value, 0, 'f' , 1 );
    m_gammaLabel = new QLabel(tr(m_gammaString),gammaGroup);
    m_gammaLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)5, 0, 0, m_gammaLabel->sizePolicy().hasHeightForWidth() ) );
    bottomLayout->addWidget(display);

    Q3ButtonGroup *factorGroup = new Q3ButtonGroup(3, Qt::Horizontal, tr("Factor Size"), this);
    m_normalFactor = new QRadioButton(tr("Normal"), factorGroup);
    m_factorTwo = new QRadioButton(tr("Factor 2"), factorGroup);
    m_factorFour = new QRadioButton(tr("Factor 3"), factorGroup);
    m_normalFactor->setChecked(true);
    bottomLayout->addWidget(factorGroup);

    Q3ButtonGroup *zoomGroup = new Q3ButtonGroup(3, Qt::Vertical, tr("Zoom"), this);
    m_zoomScrollBar = new QScrollBar(Qt::Horizontal, zoomGroup);
    m_zoomScrollBar->setRange(1, 64);
    m_zoomScrollBar->setValue(4);
    m_zoomScrollBar->setPageStep(1);
    m_zoomScrollBar->setLineStep(1);
    bottomLayout->addWidget(zoomGroup);

    Q3HBoxLayout *topLayout= new Q3HBoxLayout;
    Q3ScrollView *scrollView =  new Q3ScrollView(this);
    m_mapViewer = new sculptor_mapviewer(scrollView);
    scrollView->addChild(m_mapViewer);
    topLayout->addWidget(scrollView);

    Q3HBoxLayout *mainLayout= new Q3HBoxLayout(this);
    mainLayout->setSpacing (10);
    mainLayout->insertLayout(0, topLayout, 20);
    mainLayout->insertLayout(1, bottomLayout, 0);
}

/**
 * Gets, the cross section of the currently active plane at a specific coordinate.  Updates
 * the label to inform the user at which particular slice they are at in the cross section.
 * \param coordinate specifies the slice in the current plane
 */
void sculptor_mapdialog::getNewCrossSectionSLOT(int i)
{
    getNewCrossSection(i);
};

/**
 *Sets up the x-plane view when the x-plane radio button is activated
 */
void sculptor_mapdialog::setXPlaneViewSLOT()
{
    setXPlaneView();
};
/**
 * Sets up the y-plane view when the y-plane radio button is activated
 */
void sculptor_mapdialog::setYPlaneViewSLOT()
{
    setYPlaneView();
};
/**
 * Sets up the z-plane view when the z-plane radio button is activated
 */
void sculptor_mapdialog::setZPlaneViewSLOT()
{
    setZPlaneView();
};
/**
 * Sets up the x-projection view when the x-projection radio button is activated
 */
void sculptor_mapdialog::setXProjectionViewSLOT()
{
    setXProjectionView();
};
/**
 * Sets up the y-projection view when the y-projection radio button is activated
 */
void sculptor_mapdialog::setYProjectionViewSLOT()
{
    setYProjectionView();
};
/**
 * Sets up the z-projection view when the z-projection radio button is activated
 */
void sculptor_mapdialog::setZProjectionViewSLOT()
{
    setZProjectionView();
};
/**
 * Sets the display to binary mode.
 */
void sculptor_mapdialog::setBinaryModeSLOT()
{
    setBinaryMode();
};
/**
 * Sets the display to normal mode.
 */
void sculptor_mapdialog::setNormalModeSLOT()
{
    setNormalMode();
};
/**
 * Sets the factor to normal, therefore each vowel element in the volumeric map recieves one pixel
 * element.
 */
void sculptor_mapdialog::setNormalFactorSLOT()
{
    setNormalFactor();
};
/**
 * Sets the factor to two, therefore eight vowel elements in the volumeric map are averaged
 * together and recieve one pixel element.
 */
void sculptor_mapdialog::setFactorTwoSLOT()
{
    setFactorTwo();
};
/**
 * Sets the factor to three, therefore 27 vowel elements in the volumeric map are averaged
 * together and recieve one pixel element.
 */
void sculptor_mapdialog::setFactorThreeSLOT()
{
    setFactorThree();
};
/**
 * Gets the voxel value at the corresponding position in the MapViewer
 * \param x is the x coordinate in the data set
 * \param y is the y coordinate in the data set
 */
void sculptor_mapdialog::getVoxelValueSLOT( int iX, int iY )
{
    getVoxelValue( iX, iY );
};
/**
 * Update the label to inform the user of the current percentage of the threshold value.
 * \param thresholdValue, the new value that the binary threshold is set to.
 */
void sculptor_mapdialog::updateBinaryLabelSLOT( int i )
{
    updateBinaryLabel( i );
};
/**
 * Update the label to inform the user of the current gamma value.
 * \param gammaValue, the new value that the gamma is set to.
 */
void sculptor_mapdialog::updateGammaLabelSLOT( int i )
{
    updateGammaLabel( i );
};
/**
 * Updates the data set mask at the corresponding position.
 * \param x, the x position of the mouse
 * \param y, the y position of the mouse
 * \param value, the mask needs to be set to either a one or a zero.
 */
void sculptor_mapdialog::updateVolumericMapMaskSLOT( int iX, int iY, bool bB )
{
    updateVolumericMapMask( iX, iY, bB );
};

/**
 * Right mouse button was clicked
 * \param x is the x coordinate in the data set
 * \param y is the y coordinate in the data set
 */
void sculptor_mapdialog::rightMBSLOT( int iX, int iY )
{
    rightMB( iX, iY );
};
/**
 * Left mouse button was clicked
 * \param x is the x coordinate in the data set
 * \param y is the y coordinate in the data set
 */
void sculptor_mapdialog::leftMBSLOT( int iX, int iY)
{
    leftMB( iX, iY );
};

/**
 * Increase the coordinate of the crosssection - SLOT
 */
void sculptor_mapdialog::increaseCoordSLOT()
{
    increaseCoord();
};
/**
 * Decrease the coordinate of the crosssection - SLOT
 */
void sculptor_mapdialog::decreaseCoordSLOT()
{
    decreaseCoord();
};



/**
 * Gets, the cross section of the currently active plane at a specific coordinate.  Updates
 * the label to inform the user at which particular slice they are at in the cross section.
 * \param coordinate specifies the slice in the current plane
 */
void sculptor_mapdialog::getNewCrossSection(int )
{
};
/**
 *Sets up the x-plane view when the x-plane radio button is activated
 */
void sculptor_mapdialog::setXPlaneView()
{
};
/**
 * Sets up the y-plane view when the y-plane radio button is activated
 */
void sculptor_mapdialog::setYPlaneView()
{
};
/**
 * Sets up the z-plane view when the z-plane radio button is activated
 */
void sculptor_mapdialog::setZPlaneView()
{
};
/**
 * Sets up the x-projection view when the x-projection radio button is activated
 */
void sculptor_mapdialog::setXProjectionView()
{
};
/**
 * Sets up the y-projection view when the y-projection radio button is activated
 */
void sculptor_mapdialog::setYProjectionView()
{
};
/**
 * Sets up the z-projection view when the z-projection radio button is activated
 */
void sculptor_mapdialog::setZProjectionView()
{
};
/**
 * Sets the display to binary mode.
 */
void sculptor_mapdialog::setBinaryMode()
{
};
/**
 * Sets the display to normal mode.
 */
void sculptor_mapdialog::setNormalMode()
{
};
/**
 * Sets the factor to normal, therefore each vowel element in the volumeric map recieves one pixel
 * element.
 */
void sculptor_mapdialog::setNormalFactor()
{
};
/**
 * Sets the factor to two, therefore eight vowel elements in the volumeric map are averaged
 * together and recieve one pixel element.
 */
void sculptor_mapdialog::setFactorTwo()
{
};
/**
 * Sets the factor to three, therefore 27 vowel elements in the volumeric map are averaged
 * together and recieve one pixel element.
 */
void sculptor_mapdialog::setFactorThree()
{
};
/**
 * Gets the voxel value at the corresponding position in the MapViewer
 * \param x is the x coordinate in the data set
 * \param y is the y coordinate in the data set
 */
void sculptor_mapdialog::getVoxelValue(int , int  )
{
};
/**
 * Update the label to inform the user of the current percentage of the threshold value.
 * \param thresholdValue, the new value that the binary threshold is set to.
 */
void sculptor_mapdialog::updateBinaryLabel(int )
{
};
/**
 * Update the label to inform the user of the current gamma value.
 * \param gammaValue, the new value that the gamma is set to.
 */
void sculptor_mapdialog::updateGammaLabel(int )
{
};
/**
 * Updates the data set mask at the corresponding position.
 * \param x, the x position of the mouse
 * \param y, the y position of the mouse
 * \param value, the mask needs to be set to either a one or a zero.
 */
void sculptor_mapdialog::updateVolumericMapMask(int , int , bool )
{
};

/**
 * Right mouse button was clicked
 * \param x is the x coordinate in the data set
 * \param y is the y coordinate in the data set
 */
void sculptor_mapdialog::rightMB(int , int )
{
};
/**
 * Left mouse button was clicked
 * \param x is the x coordinate in the data set
 * \param y is the y coordinate in the data set
 */
void sculptor_mapdialog::leftMB(int , int )
{
};

/**
 * Increase the coordinate of the crosssection
 */
void sculptor_mapdialog::increaseCoord()
{
};
/**
 * Decrease the coordinate of the crosssection
 */
void sculptor_mapdialog::decreaseCoord()
{
};
