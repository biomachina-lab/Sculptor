/*-*-c++-*-*****************************************************************
                          sculptor_prop_situs.h
                          -------------------
    begin                : 09/27/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_PROP_SITUS_H
#define SCULPTOR_PROP_SITUS_H

// uic-qt4 includes
#include <ui_prop_situs.h>
// qwt5 includes
#include <qwt_plot_picker.h>
// forward declarations
class situs_document;
class QwtPlotCurve;
class QwtPlotMarker;
class prop_situs;
class svt_float2mat_points;

/**
 * \brief Helper class to let prop_situs know that the histogram marker is dragged.
 *
 * While the marker is dragged, the isosurface should be computed coarsely. This class just calls
 * the QwtPlotPicker mouse button methods, but additionally prop_situs::sIsoSliderPressed() and
 * prop_situs::sIsoSliderReleased(), respectively. Results in same functionality like when the 
 * isosurface slider is moved.
 * \author Manuel Wahle
 */
class prop_situs_HistPlotPicker : public QwtPlotPicker
{
    Q_OBJECT

    prop_situs* m_pPropSitus;

public:

    prop_situs_HistPlotPicker(QwtPlotCanvas* pCanvas, prop_situs* pPropSitus);
    virtual ~prop_situs_HistPlotPicker() {};

protected:

    virtual void widgetMousePressEvent(QMouseEvent* e);
    virtual void widgetMouseReleaseEvent(QMouseEvent* e);
};


/*
 * This class inherits from the QT4 Designer prop_situs class and implements its functionality.
 * \author Manuel Wahle
 */
class prop_situs : public QWidget, private Ui::prop_situs
{
    Q_OBJECT

    // the situs_document this dialogs belongs to
    situs_document* m_pDoc;

    // histogram curve for the volume
    QwtPlotCurve* m_pHistCurve;

    // marker to choose value in histogram
    QwtPlotMarker* m_pHistMarker;

    // picker to select values in the histogram
    prop_situs_HistPlotPicker* m_pHistPicker;

    // iso slider pressed?
    bool m_bIsoSliderPressed;

    // the default color component in the transfer plot. change it in the constructor
    int m_iTransferDefaultComponent;

    // were the slices already created?
    bool m_bSlicesCreated;

    // is the 3DT tab already initialized?
    bool m_b3DTTabInitialized;

    /**
     * update the position of the iso slider
     */
    void updateIsoSlider(int iValue);

    /**
     * enable/disable the iso plot and the iso controls
     */
    void sEnableIsoControls(bool bEnable);

    /**
     * enable/disable the slice controls
     */
    void sEnableSlicesControls(bool bEnable);

    /**
     * enable/disable the transfer plot and the 3d texture controls
     */
    void sEnable3DTControls(bool bEnable);

public:

    /**
     * completely enable/disable the 3dt tab
     */
    void set3DTTabEnabled(bool bEnabled);

    /**
     * Constructor
     */
    prop_situs( situs_document* pDoc, QWidget* parent );

    /**
     * Destructor
     */
    virtual ~prop_situs();

    /**
     * initialize the 3DT tab. this is in its own method so that we can call it when shader programs
     * are enable later
     */
    void initTab3DT();

    /**
     * update the background color of the iso color chooser button
     */
    void updateIsoColorButton(QColor oCol);

    /**
     * update the plots after the histogram of the volume changed
     * \param bHistPlot should the histogram be updated
     * \param bTransferPlot should the transfer plot be updated
     */
    void updatePlots(bool bHistPlot = true, bool bTransferPlot = true);

    /**
     * update the ui elements values in the iso tab
     */
    void updateTabIso(int iEnabled = -1);

    /**
     * update the ui elements values in the slices tab
     */
    void updateTabSlices(int iEnabled = -1);

    /**
     * update the ui elements values in the direct volume rendering tab
     */
    void updateTab3DT(int iEnabled = -1);

    /**
     * update the ui elements values in the information tab
     */
    void updateTabInfo();

    /**
     * get the value from the iso TE level lineedit
     * \return float
     */
    float getIsoTELevel();

    /**
     * get the value from the iso TE steps lineedit
     * \return int
     */
    int getIsoTESteps();

    /**
     * get the value from the splice TE cutoff lineedit
     * \return float
     */
    float getSliceTECutoff();

    /**
     * get the value from the 3d sampling rate lineedit
     * \return double
     */
    double get3DSamplingRate();

    /**
     * get the values from the shading lineedits
     * \return svt_vector3<float>
     */
    svt_vector3<float> getLightingParams();

    /**
     * get the value from the origin lineedits
     * \return svt_vector3<Real64>
     */
    svt_vector3<Real64> getOrigin();

    /**
     * get the value from the voxel widht lineedit
     * \return Real64
     */
    Real64 getVoxelWidth();

public slots:

    void sHistClicked(const QwtDoublePoint &pos);
    void sCreateIsoSurface();
    void sSetIsoColor();
    void sIsoTextInput();
    void sIsoSliderChanged(int iValue);
    void sIsoSliderReleased();
    void sIsoSliderPressed();
    void sEnableIso(bool bEnable);
    void sEnableSlices(bool bDisable);
    void sEnable3DT(bool bDisable);
    void sChangeTransferComp();
    void sResetTF();
    void sApplyTF(QwtPlotCurve*);
    void sButtonApplyTF(bool bEnablingAuto = true);
};

#endif // SCULPTOR_PROP_SITUS_H
