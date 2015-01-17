/*-*-c++-*-*****************************************************************
			  sculptor_transfer
			  -----------------
    begin                : 03/03/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef __SCULPTOR_TRANSFER_H
#define __SCULPTOR_TRANSFER_H


// svt includes
#include <svt_float2mat_points.h>
#include <svt_basics.h>
#include <svt_volume.h>
#include <svt_vector3.h>
// sculptor includes
class sculptor_transfer_picker;
class sculptor_qwtdata;
class prop_situs;
// qwt5 includes
#include <qwt_plot.h>
#include <qwt_plot_curve.h>


/**
 * \brief This class inherits from QwtPlot and implements displaying the histogram and the color component curves.
 *
 * The sculptor_transfer_picker is a major component needed for this class.
 *
 * The only thing it has in common with the previous Qt3 version is the file name ;-)
 * \author Manuel Wahle
 */
class DLLEXPORT sculptor_transfer: public QwtPlot
{
    Q_OBJECT

protected:

    // pointer to volume object
    svt_volume<Real64>* m_pVolume;

    // qwt curves for the color components and their data
    QwtPlotCurve* m_pCompR;
    sculptor_qwtdata* m_pDataR;
    QwtPlotCurve* m_pCompG;
    sculptor_qwtdata* m_pDataG;
    QwtPlotCurve* m_pCompB;
    sculptor_qwtdata* m_pDataB;
    QwtPlotCurve* m_pCompO;
    sculptor_qwtdata* m_pDataO;

    // qwt curve for the histogram in the background
    QwtPlotCurve* m_pHistCurve;

    // active color component currently edited
    int m_iActiveComp;

    // the curve that the picker should manipulate by default
    int m_iDefaultCurve;

    // the svt transfer function object
    svt_float2mat_points* m_pF2M;

    // the picker attached to this plot
    sculptor_transfer_picker* m_pPicker;

public:

    enum
    {
	TRANS_ACTIVE_RED,
	TRANS_ACTIVE_GREEN,
	TRANS_ACTIVE_BLUE,
	TRANS_ACTIVE_OPACITY,
	TRANS_ACTIVE_RGB
    };

    /**
     * Constructor
     */
    sculptor_transfer(QWidget *pParent);

    /**
     * Destructor
     */
    virtual ~sculptor_transfer();

    /**
     * set volume
     * \param pVolume pointer to svt_volume<Real64>
     * \param pXHist pointer to array of double with x values for the histogram
     * \param pYHist pointer to array of double with y values for the histogram
     * \param iSizeHistogram int number of histogram bins
     */
    void setVolume(svt_volume<Real64>* pVolume, double* pXHist, double* pYHist, unsigned int iSizeHistogram);

    /**
     * set the transfer function
     * \param pTrans pointer to svt_float2mat_points object
     */
    void setFloat2Mat( svt_float2mat_points* pF2M );

    /**
     * get the transfer function
     * \return pointer to svt_float2mat_points object
     */
    svt_float2mat_points* getFloat2Mat();

    /**
     * set active component
     * \param iActiveComp active color component:
     * - TRANS_ACTIVE_RED,
     * - TRANS_ACTIVE_GREEN,
     * - TRANS_ACTIVE_BLUE,
     * - TRANS_ACTIVE_RGB,
     * - TRANS_ACTIVE_OPACITY
     */
    void setActiveComp(int iActiveComp);

    /**
     * connect the picker to the dialog's slot that manages updating of the transfer function
     * \param pDlg pointer to the dialog
     */
    void connectPicker(prop_situs* pDlg);

    /**
     * enable or disable the picker
     * \param bDisabled bool to enable/disable the picker
     */
    void setPickerEnabled(bool bEnabled);

    /**
     * copy values from the transfer function into the plot
     */
    void transToPlot();

    /**
     * copy values from the plot into the transfer function
     */
    void plotToTrans();
};

#endif
