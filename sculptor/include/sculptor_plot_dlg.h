/***************************************************************************
                          sculptor_plot_dlg.h  -  description
                             -------------------
    begin                : 06.08.2002
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SENSITUS_PLOT_DLG_H
#define SENSITUS_PLOT_DLG_H

// qt includes
#include <qvariant.h>
#include <qdialog.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <QLabel>
#include <Q3Frame>
class Q3GridLayout; 
class Q3Frame;
class QPushButton;
class QTimer;
class QCheckBox;
class QLabel;
// svt includes
#include <svt_core.h>
#include <svt_stlVector.h>
#include <svt_types.h>
// qwt includes
#include <qwt_plot.h>
#include <qwt_plot_curve.h>

/**
 * curve for the sculptor_plot_dlg
 */
class sculptor_curve
{
protected:
    char m_pTitle[256];
    Real64 *m_pXData;
    Real64 *m_pYData;
    int m_iCount;
    long m_iCurve;
    QwtPlot* m_pQwtPlot;
    Real64* m_oXData;
    Real64* m_oYData;
    QCheckBox* m_pCheckBox;
    enum QwtPlotCurve::CurveStyle m_iStyle;
    int m_iR;
    int m_iG;
    int m_iB;

public:
    /**
     * create a new curve
     * \param pTitle pointer to array of char with the name of the curve
     * \param pXData pointer to an array of Real64 with the x data of the curve (if NULL, a default data source will get created)
     * \param pYData pointer to an array of Real64 with the y data of the curve
     * \param iCount number of values
     */
    sculptor_curve(const char* pTitle, Real64* pXData, Real64* pYData, int iCount);
    
    /**
    * create a new curve
    * \param pTitle pointer to array of char with the name of the curve
    * \param pXData vector of Real64 with the x data of the curve
    * \param pYData vector Real64 with the y data of the curve
    * \param iCount number of values
    */
    sculptor_curve(const char* pTitle, vector<Real64> pXData, vector<Real64> pYData, int iCount);
            
    /**
     * copy constructor
     * \param rCurve reference to other sculptor_curve
     */
    sculptor_curve(const sculptor_curve &rCurve);
    ~sculptor_curve();

    /**
     * add the curve to a qwt plot widget
     * \param pQwtPlot pointer to the widget
     */
    void addCurve(QwtPlot* pQwtPlot);

    /**
     * set the checkbox that controls the drawing state (visible/invisible) of the curve
     * \param pCheckBox pointer to QCheckBox object
     */
    void setCheckBox(QCheckBox* pCheckBox);
    /**
     * get the checkbox that controls the drawing state (visible/invisible) of the curve
     * \return pointer to QCheckBox object
     */
    QCheckBox* getCheckBox();

    /**
     * get name
     */
    char* getName();

    /**
     * set curve style
     */
    void setStyle(enum QwtPlotCurve::CurveStyle iStyle);
    
    /**
    * set Pen style
    */
    void setPenStyle(enum Qt::PenStyle iStyle);
    
    /**
    * set Pen style
    */
    void setPenStyle(unsigned int iStyle);

    /**
     * update the curve data
     */
    void updateData();

    /**
     * set curve color
     * \param iR red component
     * \param iG green component
     * \param iB blue component
     */
    void setColor(int iR, int iG, int iB);

    /**
     * enable
     * \param bEnable if true the curves is drawn
     */
    void setEnable(bool bEnable);

    /**
     * get data value (x-axis)
     * \param i index of data value
     * \return data itself
     */
    Real64 getDataX(int i);
    /**
     * get data value (y-axis)
     * \param i index of data value
     * \return data itself
     */
    Real64 getDataY(int i);
    /**
     * get number of data values
     * \return number of data values of the curve
     */
    int getSize();
    
    /**
     * Add data point
     * \param fX 
     * \param fY
     */
    void addDataPoint( Real64 fX, Real64 fY);

    /**
     * Remove data point
     */
    void popDataPoint();


};

/**
 * universal sculptor data plot dialog
 */
class sculptor_plot_dlg : public QDialog
{ 
    Q_OBJECT

protected:
    char m_pTitle[256];
    Q3Frame* m_pPlot;
    QwtPlot* m_pQwtPlot;
    QPushButton* m_pButtonClose;
    QPushButton* m_pButtonSave;
    QPushButton* m_pButtonPrint;
    Q3GridLayout* m_pPlotDlgLayout;
    Q3GridLayout* m_pCurvesLayout;
    vector< sculptor_curve > m_aCurves;

    QTimer* m_pTimer;
    QLabel* m_pComment;
    
    bool m_bClosedByUser;

public:
    sculptor_plot_dlg( const char* pTitle, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    virtual ~sculptor_plot_dlg();

    /**
     * get the qwt plot widget to insert data and adjust the plot
     * \return pointer to QwtPlot widget
     */
    QwtPlot* getQwtPlot();
    /**
     * set the state of the CloseByUser variable
     * \param bClosedByUser whether the user closed or opened the window
     */
    void setClosedByUser(bool bClosedByUser );
    
    /**
     * \return whether the window was closed by the user
     */
    bool wasClosedByUser();

    /**
     * set the x axis title
     * \param pTitle pointer to array of char
     */
    void setXAxisTitle(const char* pTitle);
    /**
     * set the y axis title
     * \param pTitle pointer to array of char
     */
    void setYAxisTitle(const char* pTitle);

    /**
     * add a curve
     */
    void addCurve(sculptor_curve& rCurve);
    /**
     * get curve
     */
    sculptor_curve* getCurve(int i);
    
    /**
     * Get curve number
     * \return the number of curves added to the plot
     */
    unsigned int getCurveCount();

    /**
     * start auto update
     * \param iMSec frequency of the update in milliseconds
     */
    void autoUpdate(int iMSec);

public slots:

    /**
     * enable/disable curve
     */
    void enableCurve();

    /**
     * update curves
     */
    void updateCurves();
    
    /**
     * close window
     */
    void close();

    /**
     * save the data to disk
     */
    void save();

    /**
     * print plot canvas
     */
    void print();

public:

    /**
     * set the comment
     * \param pText the new commentary text
     */
    void setComment(const char* pComment);
    
};

#endif
