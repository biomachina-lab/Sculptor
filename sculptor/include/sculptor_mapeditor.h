/***************************************************************************
                          sculptor_mapeditor
			  ------------------
    begin                : 27.07.2003
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef MAPEDITOR_H
#define MAPEDITOR_H

// qt includes
#include <qscrollbar.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qstring.h>
#include <q3scrollview.h>
#include <qtooltip.h>
#include <q3buttongroup.h>
// svt includes
#include <sculptor_mapviewer.h>
#include <svt_basics.h>
#include <svt_volume.h>

///////////////////////////////////////////////////////////////////////////////
// Dialog class
///////////////////////////////////////////////////////////////////////////////

class DLLEXPORT sculptor_mapdialog : public QWidget
{
    Q_OBJECT

protected:

    QLabel *m_planeLabel;
    QString m_voxelString;
    QLabel *m_voxelLabel;
    QString m_planeString;
    QLabel *m_binaryLabel;
    QString m_binaryString;
    QLabel *m_gammaLabel;
    QString m_gammaString;
    QScrollBar *m_zoomScrollBar;
    QScrollBar *m_planeScrollBar;
    QScrollBar *m_gammaScrollBar;
    QScrollBar *m_binaryScrollBar;
    QRadioButton *m_xPlane;
    QRadioButton *m_yPlane;
    QRadioButton *m_zPlane;
    QRadioButton *m_xProjection;
    QRadioButton *m_yProjection;
    QRadioButton *m_zProjection;
    QRadioButton *m_binaryMode;
    QRadioButton *m_normalMode;
    QRadioButton *m_normalFactor;
    QRadioButton *m_factorTwo;
    QRadioButton *m_factorFour;

    sculptor_mapviewer *m_mapViewer;

public:

    /**
     * sculptor_mapeditor class extends QWidget and acts as a parent widget for QSliders, QRadioButtons, QLabels,
     * etc.  It is used as a controller class between a MapViewer object and a CrossSection object.  It uses
     * it's child widgets to trigger events such that a new slice or plane can be calculated and
     * displayed in the MapViewer widget.
     */
    sculptor_mapdialog( QWidget *parent = 0, const char *name = 0, Qt::WFlags flags = 0 );

    /**
     * Initialises the sculptor_mapeditor widget i.e. creates the sliders, radio button, etc,
     * and lays them out in layout mangers.
     */
    void initialise();

public slots:

    /**
     * Gets, the cross section of the currently active plane at a specific coordinate.  Updates
     * the label to inform the user at which particular slice they are at in the cross section.
     * \param coordinate specifies the slice in the current plane
     */
    void getNewCrossSectionSLOT(int );
    /**
     *Sets up the x-plane view when the x-plane radio button is activated
     */
    void setXPlaneViewSLOT();
    /**
     * Sets up the y-plane view when the y-plane radio button is activated
     */
    void setYPlaneViewSLOT();
    /**
     * Sets up the z-plane view when the z-plane radio button is activated
     */
    void setZPlaneViewSLOT();
    /**
     * Sets up the x-projection view when the x-projection radio button is activated
     */
    void setXProjectionViewSLOT();
    /**
     * Sets up the y-projection view when the y-projection radio button is activated
     */
    void setYProjectionViewSLOT();
    /**
     * Sets up the z-projection view when the z-projection radio button is activated
     */
    void setZProjectionViewSLOT();
    /**
     * Sets the display to binary mode.
     */
    void setBinaryModeSLOT();
    /**
     * Sets the display to normal mode.
     */
    void setNormalModeSLOT();
    /**
     * Sets the factor to normal, therefore each vowel element in the volumeric map recieves one pixel
     * element.
     */
    void setNormalFactorSLOT();
    /**
     * Sets the factor to two, therefore eight vowel elements in the volumeric map are averaged
     * together and recieve one pixel element.
     */
    void setFactorTwoSLOT();
    /**
     * Sets the factor to three, therefore 27 vowel elements in the volumeric map are averaged
     * together and recieve one pixel element.
     */
    void setFactorThreeSLOT();
    /**
     * Gets the voxel value at the corresponding position in the MapViewer
     * \param x is the x coordinate in the data set
     * \param y is the y coordinate in the data set
     */
    void getVoxelValueSLOT(int , int  );
    /**
     * Update the label to inform the user of the current percentage of the threshold value.
     * \param thresholdValue, the new value that the binary threshold is set to.
     */
    void updateBinaryLabelSLOT(int );
    /**
     * Update the label to inform the user of the current gamma value.
     * \param gammaValue, the new value that the gamma is set to.
     */
    void updateGammaLabelSLOT(int );
    /**
     * Updates the data set mask at the corresponding position.
     * \param x, the x position of the mouse
     * \param y, the y position of the mouse
     * \param value, the mask needs to be set to either a one or a zero.
     */
    void updateVolumericMapMaskSLOT(int , int , bool );

    /**
     * Right mouse button was clicked
     * \param x is the x coordinate in the data set
     * \param y is the y coordinate in the data set
     */
    void rightMBSLOT(int , int );
    /**
     * Left mouse button was clicked
     * \param x is the x coordinate in the data set
     * \param y is the y coordinate in the data set
     */
    void leftMBSLOT(int , int );

    /**
     * Increase the coordinate of the crosssection - SLOT
     */
    void increaseCoordSLOT();
    /**
     * Decrease the coordinate of the crosssection - SLOT
     */
    void decreaseCoordSLOT();

public:

    /**
     * Gets, the cross section of the currently active plane at a specific coordinate.  Updates
     * the label to inform the user at which particular slice they are at in the cross section.
     * \param coordinate specifies the slice in the current plane
     */
    virtual void getNewCrossSection(int );
    /**
     *Sets up the x-plane view when the x-plane radio button is activated
     */
    virtual void setXPlaneView();
    /**
     * Sets up the y-plane view when the y-plane radio button is activated
     */
    virtual void setYPlaneView();
    /**
     * Sets up the z-plane view when the z-plane radio button is activated
     */
    virtual void setZPlaneView();
    /**
     * Sets up the x-projection view when the x-projection radio button is activated
     */
    virtual void setXProjectionView();
    /**
     * Sets up the y-projection view when the y-projection radio button is activated
     */
    virtual void setYProjectionView();
    /**
     * Sets up the z-projection view when the z-projection radio button is activated
     */
    virtual void setZProjectionView();
    /**
     * Sets the display to binary mode.
     */
    virtual void setBinaryMode();
    /**
     * Sets the display to normal mode.
     */
    virtual void setNormalMode();
    /**
     * Sets the factor to normal, therefore each vowel element in the volumeric map recieves one pixel
     * element.
     */
    virtual void setNormalFactor();
    /**
     * Sets the factor to two, therefore eight vowel elements in the volumeric map are averaged
     * together and recieve one pixel element.
     */
    virtual void setFactorTwo();
    /**
     * Sets the factor to three, therefore 27 vowel elements in the volumeric map are averaged
     * together and recieve one pixel element.
     */
    virtual void setFactorThree();
    /**
     * Gets the voxel value at the corresponding position in the MapViewer
     * \param x is the x coordinate in the data set
     * \param y is the y coordinate in the data set
     */
    virtual void getVoxelValue(int , int  );
    /**
     * Update the label to inform the user of the current percentage of the threshold value.
     * \param thresholdValue, the new value that the binary threshold is set to.
     */
    virtual void updateBinaryLabel(int );
    /**
     * Update the label to inform the user of the current gamma value.
     * \param gammaValue, the new value that the gamma is set to.
     */
    virtual void updateGammaLabel(int );
    /**
     * Updates the data set mask at the corresponding position.
     * \param x, the x position of the mouse
     * \param y, the y position of the mouse
     * \param value, the mask needs to be set to either a one or a zero.
     */
    virtual void updateVolumericMapMask(int , int , bool );

    /**
     * Right mouse button was clicked
     * \param x is the x coordinate in the data set
     * \param y is the y coordinate in the data set
     */
    virtual void rightMB(int , int );
    /**
     * Left mouse button was clicked
     * \param x is the x coordinate in the data set
     * \param y is the y coordinate in the data set
     */
    virtual void leftMB(int , int );

    /**
     * Increase the coordinate of the crosssection
     */
    virtual void increaseCoord();
    /**
     * Decrease the coordinate of the crosssection
     */
    virtual void decreaseCoord();

signals:

    /**
     * Sends a signal that indicates the mouse position in the sculptor_mapviewer
     * \param x, the x position of the mouse
     * \param y, the y position of the mouse
     */
    void leftClickedVoxel(int x, int y, int z);
    /**
     * Sends a signal that indicates the mouse position in the sculptor_mapviewer
     * \param x, the x position of the mouse
     * \param y, the y position of the mouse
     */
    void rightClickedVoxel(int x, int y, int z);

};

///////////////////////////////////////////////////////////////////////////////
// Dialog class
///////////////////////////////////////////////////////////////////////////////

template<class T>
class DLLEXPORT sculptor_mapeditor : public sculptor_mapdialog
{
	
public:

    /**
     * sculptor_mapeditor class extends QWidget and acts as a parent widget for QSliders, QRadioButtons, QLabels,
     * etc.  It is used as a controller class between a MapViewer object and a CrossSection object.  It uses
     * it's child widgets to trigger events such that a new slice or plane can be calculated and
     * displayed in the MapViewer widget.
     */
    sculptor_mapeditor( svt_volume<T> *pVolume, svt_volume<unsigned int>* pVolumeMask, QWidget *parent, const char *name, Qt::WFlags flags );
    /**
     * Gets the current plane that is activated on the MapViewer.
     * \return m_currentPlane current plane;
     */
    char getCurrentPlane();
    /**
     * Gets the current value of the _planeSlider.
     * \return the current value the _planeSlider is set to.
     */
    int getCoordinate();
    /**
     * pindex finds the cross section of a 3-D slice.
     * \param x is the x coordinate of the 3-D slice
     * \param y is the y coordinate of the 3-D slice
     * \param z is the z coordinate of the 3-D slice
     * \return the data value at that 3-D cross section
     */
    unsigned int pindex (int , int , int );
    /**
     * Computes a new cross section from the ordinal data set.
     * \param planeFlag is the cross section plane
     * \param coordinate is the coordinate of the cross section as function of plane
     */
    void calcCrossSection(char , int );
    /**
     * Computes a new projection from the ordinal data set and data set mask.
     * \param projectionFlag is the projection plane
     */
    void calcProjection(char );
    /**
     * Gets the current cross section data set.
     * \return _crossSectionVolumericMap is the current cross section data set
     */
    float* getCrossSection();
    /**
     * Gets the current cross section data set mask, which is used for editing the of the data set
     * \return m_ordinalVolumericMapMask is the data set mask
     */
    unsigned int* getCrossSectionMask();
    /**
     * Get the main mapviewer object
     */
    sculptor_mapviewer* getMapViewer();

public:

    /**
     * Gets, the cross section of the currently active plane at a specific coordinate.  Updates
     * the label to inform the user at which particular slice they are at in the cross section.
     * \param coordinate specifies the slice in the current plane
     */
    virtual void getNewCrossSection(int );
    /**
     *Sets up the x-plane view when the x-plane radio button is activated
     */
    virtual void setXPlaneView();
    /**
     * Sets up the y-plane view when the y-plane radio button is activated
     */
    virtual void setYPlaneView();
    /**
     * Sets up the z-plane view when the z-plane radio button is activated
     */
    virtual void setZPlaneView();
    /**
     * Sets up the x-projection view when the x-projection radio button is activated
     */
    virtual void setXProjectionView();
    /**
     * Sets up the y-projection view when the y-projection radio button is activated
     */
    virtual void setYProjectionView();
    /**
     * Sets up the z-projection view when the z-projection radio button is activated
     */
    virtual void setZProjectionView();
    /**
     * Sets the display to binary mode.
     */
    virtual void setBinaryMode();
    /**
     * Sets the display to normal mode.
     */
    virtual void setNormalMode();

    /**
     * Gets the voxel value at the corresponding position in the MapViewer
     * \param x is the x coordinate in the data set
     * \param y is the y coordinate in the data set
     */
    virtual void getVoxelValue(int , int  );
    /**
     * Update the label to inform the user of the current percentage of the threshold value.
     * \param thresholdValue, the new value that the binary threshold is set to.
     */
    virtual void updateBinaryLabel(int );
    /**
     * Update the label to inform the user of the current gamma value.
     * \param gammaValue, the new value that the gamma is set to.
     */
    virtual void updateGammaLabel(int );
    /**
     * Updates the data set mask at the corresponding position.
     * \param x, the x position of the mouse
     * \param y, the y position of the mouse
     * \param value, the mask needs to be set to either a one or a zero.
     */
    virtual void updateVolumericMapMask(int , int , unsigned int );

    /**
     * Right mouse button was clicked
     * \param x is the x coordinate in the data set
     * \param y is the y coordinate in the data set
     */
    virtual void rightMB(int , int );
    /**
     * Left mouse button was clicked
     * \param x is the x coordinate in the data set
     * \param y is the y coordinate in the data set
     */
    virtual void leftMB(int , int );

    /**
     * Increase the coordinate of the crosssection
     */
    virtual void increaseCoord();
    /**
     * Decrease the coordinate of the crosssection
     */
    virtual void decreaseCoord();

private:

    /**
     * Checks to indentify which plane is currently activated on the sculptor_mapeditor. Use to update
     * voxel values within the data set and data set mask.
     * \param x, the x position within the data set/mask
     * \param y, the y position within the data set/mask
     */
    void checkPlaneActivated(int , int );
    /**
     * Checks to indentify which plane is currently activated on the sculptor_mapeditor. Use when the factor
     * value has changed.
     */
    void checkPlaneActivated();
    /**
     * Sets the current plane on the MapViewer.
     */
    void applyCurrentPlane();
    /**
     * Turns the x, y and z plane radio buttons off and disables the plane slider.  Called when
     * the user wishes to see a projection view of the volume set.
     */
    void disablePlanes();
    /**
     * Turns the x, y and z projection radio buttons off.  Called when the user wishes to
     * iterate through the plane slices of the volume set.
     */
    void disableProjections();

private:

    svt_volume<T> *m_pVolume;
    svt_volume<unsigned int> *m_pVolumeMask;

    float *m_pCrossSection;
    unsigned int *m_pCrossSectionMask;

    char m_cCurrentPlane;

    int m_iCoordinate;
    int m_iPosX;
    int m_iPosY;
    int m_iPosZ;

    T m_fVoxelValue;
};

/**
 * sculptor_mapeditor class extends QWidget and acts as a parent widget for QScrollBars, QRadioButtons, QLabels,
 * etc.  It is used as a controller class between a sculptor_mapviewer object and a CrossSection object.  It uses
 * it's child widgets to trigger events such that a new slice or plane can be calculated and 
 * displayed in the sculptor_mapviewer widget.
 */
template<class T>
sculptor_mapeditor<T>::sculptor_mapeditor(svt_volume<T> *pVolume, svt_volume<unsigned int>* pVolumeMask, QWidget *parent, const char *name, Qt::WFlags flags) : sculptor_mapdialog( parent, name, flags )
{
    m_pVolume = pVolume;
    m_pVolumeMask = pVolumeMask;
    m_cCurrentPlane = 'y';

    m_pCrossSection = NULL;
    m_pCrossSectionMask = NULL;

    calcCrossSection(m_cCurrentPlane, m_pVolume->getSizeY()/2);

    m_planeScrollBar->setRange(0, m_pVolume->getSizeY()-1);
    m_planeScrollBar->setValue(m_pVolume->getSizeY()/2);

    m_mapViewer->setCrossSection(
                                 getCrossSection(),
                                 getCrossSectionMask(),
                                 m_pVolume->getSizeZ(), m_pVolume->getSizeX(),
                                 m_pVolume->getMaxDensity(), m_pVolume->getMinDensity(),
                                 m_binaryMode->isChecked()
                                );
}

/**
 * Gets, the cross section of the currently active plane at a specific coordinate.  Updates
 * the label to inform the user at which particular slice they are at in the cross section. 
 * \param coordinate specifies the slice in the current plane 
 */
template<class T>
void sculptor_mapeditor<T>::getNewCrossSection(int coordinate)
{
    if (coordinate == -1)
        coordinate = getCoordinate();

    if(m_xPlane->isChecked())
    {
        calcCrossSection('x',coordinate);
        m_mapViewer->setCrossSection(
                                     getCrossSection(),
                                     getCrossSectionMask(),
                                     m_pVolume->getSizeY(), m_pVolume->getSizeZ(),
                                     m_pVolume->getMaxDensity(), m_pVolume->getMinDensity(),
                                     m_binaryMode->isChecked()
                                    );
    }

    if(m_yPlane->isChecked())
    {
        calcCrossSection('y',coordinate);
        m_mapViewer->setCrossSection(
                                     getCrossSection(),
                                     getCrossSectionMask(),
                                     m_pVolume->getSizeZ(), m_pVolume->getSizeX(),
                                     m_pVolume->getMaxDensity(), m_pVolume->getMinDensity(),
                                     m_binaryMode->isChecked()
                                    );
    }

    if(m_zPlane->isChecked())
    {
        calcCrossSection('z',coordinate);
        m_mapViewer->setCrossSection(
                                     getCrossSection(),
                                     getCrossSectionMask(),
                                     m_pVolume->getSizeX(), m_pVolume->getSizeY(),
                                     m_pVolume->getMaxDensity(), m_pVolume->getMinDensity(),
                                     m_binaryMode->isChecked()
                                    );
    }

    m_planeString = QString::number(m_planeScrollBar->value(), 10);
    m_planeLabel->setText(m_planeString);
}

/**
 * Sets up the x-plane view when the x-plane radio button is activated
 */
template<class T>
void sculptor_mapeditor<T>::setXPlaneView()
{
    disableProjections();
    m_planeScrollBar->setRange(0, m_pVolume->getSizeX()-1);
    m_cCurrentPlane = 'x';

    calcCrossSection(m_cCurrentPlane, getCoordinate());

    m_mapViewer->setCrossSection(
                                 getCrossSection(),
                                 getCrossSectionMask(),
                                 m_pVolume->getSizeY(), m_pVolume->getSizeZ(),
                                 m_pVolume->getMaxDensity(), m_pVolume->getMinDensity(),
                                 m_binaryMode->isChecked()
                                );
}

/**
 * Sets up the y-plane view when the y-plane radio button is activated 
 */
template<class T>
void sculptor_mapeditor<T>::setYPlaneView()
{
    disableProjections();
    m_planeScrollBar->setRange(0, m_pVolume->getSizeY()-1);
    m_cCurrentPlane = 'y';

    calcCrossSection(m_cCurrentPlane, getCoordinate());

    m_mapViewer->setCrossSection(
                                 getCrossSection(),
                                 getCrossSectionMask(),
                                 m_pVolume->getSizeZ(), m_pVolume->getSizeX(),
                                 m_pVolume->getMaxDensity(), m_pVolume->getMinDensity(),
                                 m_binaryMode->isChecked()
                                );
}

/**
 * Sets up the z-plane view when the z-plane radio button is activated
 */
template<class T>
void sculptor_mapeditor<T>::setZPlaneView()
{
    disableProjections();
    m_planeScrollBar->setRange(0, m_pVolume->getSizeZ()-1);
    m_cCurrentPlane = 'z';

    calcCrossSection(m_cCurrentPlane, getCoordinate());

    m_mapViewer->setCrossSection(
                                 getCrossSection(),
                                 getCrossSectionMask(),
                                 m_pVolume->getSizeX(), m_pVolume->getSizeY(),
                                 m_pVolume->getMaxDensity(), m_pVolume->getMinDensity(),
                                 m_binaryMode->isChecked()
                                );
}

/**
 * Sets up the x-projection view when the x-projection radio button is activated
 */
template<class T>
void sculptor_mapeditor<T>::setXProjectionView()
{
    disablePlanes();
    m_cCurrentPlane = 'x';

    calcProjection(m_cCurrentPlane);

    m_mapViewer->setCrossSection(
                                 getCrossSection(),
                                 getCrossSectionMask(),
                                 m_pVolume->getSizeY(), m_pVolume->getSizeZ(),
                                 m_pVolume->getMaxDensity(), m_pVolume->getMinDensity(),
                                 m_binaryMode->isChecked()
                                );
}

/**
 * Sets up the y-projection view when the y-projection radio button is activated
 */
template<class T>
void sculptor_mapeditor<T>::setYProjectionView()
{
    disablePlanes();
    m_cCurrentPlane = 'y';
    calcProjection(m_cCurrentPlane);

    m_mapViewer->setCrossSection(
                                 getCrossSection(),
                                 getCrossSectionMask(),
                                 m_pVolume->getSizeZ(), m_pVolume->getSizeX(),
                                 m_pVolume->getMaxDensity(), m_pVolume->getMinDensity(),
                                 m_binaryMode->isChecked()
                                );
}

/**
 * Sets up the z-projection view when the z-projection radio button is activated
 */
template<class T>
void sculptor_mapeditor<T>::setZProjectionView()
{
    disablePlanes();
    m_cCurrentPlane = 'z';
    calcProjection(m_cCurrentPlane);

    m_mapViewer->setCrossSection(
                                 getCrossSection(),
                                 getCrossSectionMask(),
                                 m_pVolume->getSizeX(), m_pVolume->getSizeY(),
                                 m_pVolume->getMaxDensity(), m_pVolume->getMinDensity(),
                                 m_binaryMode->isChecked()
                                );
}
/**
 * Sets up the display to binary mode.
 */
template<class T>
void sculptor_mapeditor<T>::setBinaryMode()
{
    m_binaryScrollBar->setEnabled(true);
    m_gammaScrollBar->setEnabled(false);
    applyCurrentPlane();
}
/**
 * Sets up the display to normal mode.
 */
template<class T>
void sculptor_mapeditor<T>::setNormalMode()
{
    m_binaryScrollBar->setEnabled(false);
    m_gammaScrollBar->setEnabled(true);
    applyCurrentPlane();
}

/**
 * Gets the current plane that is activated on the sculptor_mapviewer.
 * \return m_cCurrentPlane current plane;
 */
template<class T>
char sculptor_mapeditor<T>::getCurrentPlane()
{
    return m_cCurrentPlane;
}
/**
 * Turns the x, y and z plane radio buttons off and disables the plane ScrollBar.  Called when the user
 * wishes to see a projection view of the volume set.
 */
template<class T>
void sculptor_mapeditor<T>::disablePlanes()
{
    m_xPlane->setChecked(false);
    m_yPlane->setChecked(false);
    m_zPlane->setChecked(false);
    m_planeScrollBar->setEnabled(false);
}
/**
 * Turns the x, y and z projection radio buttons off.  Called when the user wishes to
 * iterate through the plane slices of the volume set.
 */
template<class T>
void sculptor_mapeditor<T>::disableProjections()
{
    m_xProjection->setChecked(false);
    m_yProjection->setChecked(false);
    m_zProjection->setChecked(false);
    m_planeScrollBar->setEnabled(true);
}
/**
 * Sets the current plane on the sculptor_mapviewer.
 */
template<class T>
void sculptor_mapeditor<T>::applyCurrentPlane()
{
    switch(getCurrentPlane())
    {
    case 'x':
        m_mapViewer->setCrossSection(
                                     getCrossSection(),
                                     getCrossSectionMask(),
                                     m_pVolume->getSizeY(), m_pVolume->getSizeZ(),
                                     m_pVolume->getMaxDensity(), m_pVolume->getMinDensity(),
                                     m_binaryMode->isChecked()
                                    );
        break;
    case 'y':
        m_mapViewer->setCrossSection(
                                     getCrossSection(),
                                     getCrossSectionMask(),
                                     m_pVolume->getSizeZ(), m_pVolume->getSizeX(),
                                     m_pVolume->getMaxDensity(), m_pVolume->getMinDensity(),
                                     m_binaryMode->isChecked()
                                    );
        break;
    case 'z':
        m_mapViewer->setCrossSection(
                                     getCrossSection(),
                                     getCrossSectionMask(),
                                     m_pVolume->getSizeX(), m_pVolume->getSizeY(),
                                     m_pVolume->getMaxDensity(), m_pVolume->getMinDensity(),
                                     m_binaryMode->isChecked()
                                    );
        break;
    }
}
/**
 * Gets the current value of the m_planeScrollBar.
 * \return the current value the m_planeScrollBar is set to.
 */
template<class T>
int sculptor_mapeditor<T>::getCoordinate()
{
    return m_planeScrollBar->value();
}
/**
 * pindex finds the cross section of a 3-D slice.
 * \param x is the x coordinate of the 3-D slice
 * \param y is the y coordinate of the 3-D slice
 * \param z is the z coordinate of the 3-D slice
 * \return the data value at that 3-D cross section
 */
template<class T>
unsigned int sculptor_mapeditor<T>::pindex (int x, int y, int z)
{
    return x + y * m_pVolume->getSizeX() + z* m_pVolume->getSizeX() * m_pVolume->getSizeY();

}
/**
 * Computes a new cross section from the ordinal data set and data set mask.
 * \param planeFlag is the cross section plane
 * \param coordinate is the coordinate of the cross section as function of plane
 */
template<class T>
void sculptor_mapeditor<T>::calcCrossSection(char planeFlag, int coordinate)
{
    int x, y, z;

    if (m_pCrossSection != NULL )
    {
        delete[] m_pCrossSection;
    }

    if (m_pCrossSectionMask != NULL )
    {
        delete[] m_pCrossSectionMask;
    }

    switch(planeFlag)
    {

    case 'x':
        m_pCrossSection = new float[m_pVolume->getSizeY() * m_pVolume->getSizeZ()];
        m_pCrossSectionMask = new unsigned int[m_pVolume->getSizeY() * m_pVolume->getSizeZ()];

        if (coordinate < 0) coordinate = 0;
        if (coordinate >= (int)m_pVolume->getSizeX()) coordinate = m_pVolume->getSizeX() - 1;

        m_iCoordinate = coordinate;

        for(z = 0; z < (int)m_pVolume->getSizeZ(); z++)
        {
            for(y = 0; y < (int)m_pVolume->getSizeY(); y++)
            {
                m_pCrossSection[z*m_pVolume->getSizeY()+y] = m_pVolume->getValue(m_iCoordinate, y, z);
                if (m_pVolumeMask != NULL)
                    m_pCrossSectionMask[z*m_pVolume->getSizeY()+y] = m_pVolumeMask->getValue(m_iCoordinate, y, z);
                else
                    m_pCrossSectionMask[z*m_pVolume->getSizeY()+y] = 0;
            }
        }
        break;

    case 'y':
        m_pCrossSection = new float[m_pVolume->getSizeX() * m_pVolume->getSizeZ()];
        m_pCrossSectionMask = new unsigned int[m_pVolume->getSizeX() * m_pVolume->getSizeZ()];

        if (coordinate < 0) coordinate = 0;
        if (coordinate >= (int)m_pVolume->getSizeY()) coordinate = m_pVolume->getSizeY() - 1;

        m_iCoordinate = coordinate;

        for(x = 0; x < (int)m_pVolume->getSizeX(); x++)
        {
            for(z = 0; z < (int)m_pVolume->getSizeZ(); z++)
            {
                m_pCrossSection[x*m_pVolume->getSizeZ()+z] = m_pVolume->getValue(x, m_iCoordinate, z);
                if (m_pVolumeMask != NULL)
                    m_pCrossSectionMask[x*m_pVolume->getSizeZ()+z] = m_pVolumeMask->getValue(x, m_iCoordinate, z);
                else
                    m_pCrossSectionMask[x*m_pVolume->getSizeZ()+z] = 0;
            }
        }
        break;

    case 'z':
        m_pCrossSection = new float[m_pVolume->getSizeX() * m_pVolume->getSizeY()];
        m_pCrossSectionMask = new unsigned int[m_pVolume->getSizeX() * m_pVolume->getSizeY()];

        if (coordinate < 0) coordinate = 0;
        if (coordinate >= (int)m_pVolume->getSizeZ()) coordinate = m_pVolume->getSizeZ() - 1;

        m_iCoordinate = coordinate;

        for(y = 0; y < (int)m_pVolume->getSizeY(); y++)
        {
            for(x = 0; x < (int)m_pVolume->getSizeX(); x++)
            {
                m_pCrossSection[y*m_pVolume->getSizeX()+x] = m_pVolume->getValue(x, y, m_iCoordinate);
                if (m_pVolumeMask != NULL)
                    m_pCrossSectionMask[y*m_pVolume->getSizeX()+x] = m_pVolumeMask->getValue(x, y, m_iCoordinate);
                else
                    m_pCrossSectionMask[y*m_pVolume->getSizeX()+x] = 0;
            }
        }
        break;

    default:
        m_pCrossSection = NULL;
        m_pCrossSectionMask = NULL;
        break;

    }

    m_planeScrollBar->setValue( m_iCoordinate );
}

/**
 * Computes a new projection from the ordinal data set and data set mask.
 * \param projectionFlag is the projection plane  
 */
template<class T>
void sculptor_mapeditor<T>::calcProjection(char projectionFlag)
{
    int x, y, z;
    double value = 0.0;

    if (m_pCrossSection != NULL )
    {
        delete[] m_pCrossSection;
    }
    if (m_pCrossSectionMask != NULL )
    {
        delete[] m_pCrossSectionMask;
    }
    m_pCrossSectionMask = NULL;
	
    switch(projectionFlag)
    {
    case 'x':
        m_pCrossSection = new float[m_pVolume->getSizeY() * m_pVolume->getSizeZ()];
        for(z = 0; z < (int)m_pVolume->getSizeZ(); z++)
            for(y = 0; y < (int)m_pVolume->getSizeY(); y++)
            {
                for(x = 0; x < (int)m_pVolume->getSizeX(); x++)
                    value = value + m_pVolume->getValue(x, y, z);
                m_pCrossSection[z*m_pVolume->getSizeY()+y] = value / m_pVolume->getSizeX();
                value = 0.0;
            }
        break;

    case 'y':
        m_pCrossSection = new float[m_pVolume->getSizeX() * m_pVolume->getSizeZ()];
        for(x = 0; x < (int)m_pVolume->getSizeX(); x++)
            for(z = 0; z < (int)m_pVolume->getSizeZ(); z++)
            {
                for(y = 0; y < (int)m_pVolume->getSizeY(); y++)
                    value = value + m_pVolume->getValue(x, y, z);
                m_pCrossSection[x*m_pVolume->getSizeZ()+z] = value / m_pVolume->getSizeY();
                value = 0.0;
            }
        break;

    case 'z':
        m_pCrossSection = new float[m_pVolume->getSizeX() * m_pVolume->getSizeY()];
        for(y = 0; y < (int)m_pVolume->getSizeY(); y++)
            for(x = 0; x < (int)m_pVolume->getSizeX(); x++)
            {
                for(z = 0; z < (int)m_pVolume->getSizeZ(); z++)
                    value = value + m_pVolume->getValue(x, y, z);
                m_pCrossSection[y*m_pVolume->getSizeX()+x] = value / m_pVolume->getSizeZ();
                value = 0.0;
            }
        break;
    }
}

/**
 * Gets the current cross section data set.
 * \return _crossSectionVolumericMap is the current cross section data set
 */
template<class T>
float* sculptor_mapeditor<T>::getCrossSection()
{
    return m_pCrossSection;
}

/**
 * Gets the current cross section data set mask, which is used for editing the of the data set
 * \return m_pCrossSectionMask is the data set mask
 */
template<class T>
unsigned int* sculptor_mapeditor<T>::getCrossSectionMask()
{
    return m_pCrossSectionMask;
}

/**
 * Gets the voxel value at the corresponding position in the sculptor_mapviewer
 * \param positionsX is the x coordinate in the data set
 * \param positionsY is the y coordinate in the data set
 */
template<class T>
void sculptor_mapeditor<T>::getVoxelValue(int positionsX, int positionsY)
{
    checkPlaneActivated(positionsX, positionsY);
    if(m_xPlane->isChecked() || m_yPlane->isChecked() || m_zPlane->isChecked())
    {
        m_fVoxelValue = m_pVolume->getValue(m_iPosX, m_iPosY, m_iPosZ);
        m_voxelString = QString("The voxel value at x:%1 y:%2 z:%3 is:%4")
            .arg(m_iPosX)
            .arg(m_iPosY)
            .arg(m_iPosZ)
            .arg((Real32)m_fVoxelValue);
        m_voxelLabel->setText(m_voxelString);

    } else {

        m_voxelString = QString("The voxel value at x:%1 y:%2 z:%3 is:%4")
            .arg(m_iPosX)
            .arg(m_iPosY)
            .arg(m_iPosZ)
            .arg((Real32)m_fVoxelValue);
        m_voxelLabel->setText(m_voxelString);
    }
}
/** 
 * Update the label to inform the user of the current percentage of the threshold value.
 * \param thresholdValue, the new value that the binary threshold is set to.
 */
template<class T>
void sculptor_mapeditor<T>::updateBinaryLabel(int thresholdValue)
{
    m_binaryString = QString::number(thresholdValue * 10, 10);
    m_binaryString.append(" %");
    m_binaryLabel->setText(m_binaryString);
}
/**
 * Update the label to inform the user of the current gamma value.
 * \param gammaValue, the new value that the gamma is set to.
 */
template<class T>
void sculptor_mapeditor<T>::updateGammaLabel(int gammaValue)
{
    double value = gammaValue / 10.0f;
    m_gammaString = QString("%1" )
        .arg( value, 0, 'f' , 1 );
    m_gammaLabel->setText(m_gammaString);
}
/**
 * Updates the data set mask at the corresponding position.
 * \param x, the x position of the mouse
 * \param y, the y position of the mouse
 * \param value, the mask needs to be set to either a one or a zero.
 */
template<class T>
void sculptor_mapeditor<T>::updateVolumericMapMask(int iX, int iY, unsigned int iValue)
{
    checkPlaneActivated(iX, iY);

    if (m_pVolumeMask != NULL)
        m_pVolumeMask[pindex(m_iPosX, m_iPosY, m_iPosZ)] = iValue;
}
/**
 * Checks to indentify which plane is currently activated on the sculptor_mapeditor. Use to update
 * voxel values within the data set and data set mask.
 * \param x, the x position within the data set/mask
 * \param y, the y position within the data set/mask
 */
template<class T>
void sculptor_mapeditor<T>::checkPlaneActivated(int x, int y)
{
    if(m_xPlane->isChecked())
    {
        m_iPosY = x;
        m_iPosZ = y;
        m_iPosX = getCoordinate();
    }
    if(m_yPlane->isChecked())
    {
        m_iPosX = y;
        m_iPosZ = x;
        m_iPosY = getCoordinate();
    }
    if(m_zPlane->isChecked())
    {
        m_iPosX = x;
        m_iPosY = y;
        m_iPosZ = getCoordinate();
    }
    if(m_xProjection->isChecked())
    {
        m_iPosY = x;
        m_iPosZ = y;
        m_iPosX = 0;
        m_fVoxelValue = (T)m_mapViewer->getValue(m_iPosY, m_iPosZ);
    }
    if(m_yProjection->isChecked())
    {
        m_iPosX = y;
        m_iPosZ = x;
        m_iPosY = 0;
        m_fVoxelValue = (T)m_mapViewer->getValue(m_iPosZ, m_iPosX);
    }
    if(m_zProjection->isChecked())
    {
        m_iPosX = x;
        m_iPosY = y;
        m_iPosZ = 0;
        m_fVoxelValue = (T)m_mapViewer->getValue(m_iPosX, m_iPosY);
    }
}

/**
 * Checks to indentify which plane is currently activated on the sculptor_mapeditor. Use when the factor 
 * value has changed.
 */
template<class T>
void sculptor_mapeditor<T>::checkPlaneActivated()
{
    if(m_xPlane->isChecked())
        setXPlaneView();
    if(m_yPlane->isChecked())
        setYPlaneView();
    if(m_zPlane->isChecked())
        setZPlaneView();
    if(m_xProjection->isChecked())
        setXProjectionView();
    if(m_yProjection->isChecked())
        setYProjectionView();
    if(m_zProjection->isChecked())
        setZProjectionView();
}

/**
 * Get the main mapviewer object
 */
template<class T>
sculptor_mapviewer* sculptor_mapeditor<T>::getMapViewer()
{
    return m_mapViewer;
};

/**
 * Right mouse button was clicked
 * \param x is the x coordinate in the data set
 * \param y is the y coordinate in the data set
 */
template<class T>
void sculptor_mapeditor<T>::rightMB(int iPosX, int iPosY)
{
    checkPlaneActivated( iPosX, iPosY );

    emit rightClickedVoxel( iPosX, iPosY, m_iPosZ);

};
/**
 * Left mouse button was clicked
 * \param x is the x coordinate in the data set
 * \param y is the y coordinate in the data set
 */
template<class T>
void sculptor_mapeditor<T>::leftMB(int iPosX, int iPosY)
{
    checkPlaneActivated( iPosX, iPosY);

    emit leftClickedVoxel( iPosX, iPosY, m_iPosZ);
};

/**
 * Increase the coordinate of the crosssection
 */
template<class T>
void sculptor_mapeditor<T>::increaseCoord()
{
    getNewCrossSection( getCoordinate() + 1 );
};
/**
 * Decrease the coordinate of the crosssection
 */
template<class T>
void sculptor_mapeditor<T>::decreaseCoord()
{
    getNewCrossSection( getCoordinate() - 1 );
};


#endif
