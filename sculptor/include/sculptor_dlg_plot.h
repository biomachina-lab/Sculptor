/*-*-c++-*-*****************************************************************
                          sculptor_dlg_plot.h
			  ---------------------
    begin                : 28/10/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_PLOT_H
#define SCULPTOR_DLG_PLOT_H

// sculptor includes
class sculptor_qwtplotcurve;
// svt includes
#include <svt_core.h>
#include <svt_stlVector.h>
#include <svt_types.h>
// qwt includes
#include <qwt_plot.h>
// qt4 includes
#include <QDialog>
#include <QString>
#include <QCheckBox>
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QLabel;
class QFrame;
class QTimer;
class QLabel;


/**
 * \biref Little helper for the dlg_plot.
 *
 * Inherits from the QCheckBox and holds a pointer to the sculptor_qwtplotcurve to switch its visibility.
 * \author Manuel Wahle
 */
class dlg_plot_checkbox : public QCheckBox
{
    Q_OBJECT

    // pointer to the sculptor_qwtplotcurve to switch visibility
    sculptor_qwtplotcurve* m_pCurve;

public:

    /**
     * Constructor
     */
    dlg_plot_checkbox(sculptor_qwtplotcurve* pCurve);

    /**
     * Get the pointer to the curve
     */
    sculptor_qwtplotcurve* getCurve();
};

/**
 * \brief Sculptor plot dialog.
 *
 * Inherits from QDialog and basically holds a plot. Curves can be switched on or off with
 * checkboxes that are automatically added.
 * \author Manuel Wahle
 */
class dlg_plot : public QDialog
{ 
    Q_OBJECT

protected:

    // window and plot title
    QString m_oTitle;

    // pointer to the QwtPlot
    QwtPlot* m_pQwtPlot;

    QFrame* m_pPlot;
    QPushButton* m_pButtonClose;
    QPushButton* m_pButtonSave;
    QPushButton* m_pButtonPrint;
    QVBoxLayout* m_pPlotDlgLayout;
    QHBoxLayout* m_pButtonsLayout;
    QHBoxLayout* m_pCheckBoxesLayout;
    QTimer* m_pTimer;
    QLabel* m_pComment;

public:

    /**
     * Constructor
     */
    dlg_plot( QWidget* pParent, QString oTitle );

    /**
     * Destructor
     */
    virtual ~dlg_plot();

    /**
     * add the curve to a qwt plot widget
     * \param pQwtPlot pointer to the widget
     */
    void attachCurve(sculptor_qwtplotcurve* pCurve);

    /**
     * get the qwt plot widget to insert data and adjust the plot
     * \return pointer to QwtPlot widget
     */
    QwtPlot* getQwtPlot();

    /**
     * set the x axis title
     * \param pTitle pointer to array of char
     */
    void setXAxisTitle(QString oTitle);

    /**
     * set the y axis title
     * \param pTitle pointer to array of char
     */
    void setYAxisTitle(QString oTitle);

    /**
     * start auto update
     * \param iMSec frequency of the update in milliseconds
     */
    void autoUpdate(int iMSec);

    /**
     * set the comment
     * \param oComment the new commentary text
     */
    void setComment(QString oComment);

    /**
     * get a vector containing pointers to all attached curves
     */
    vector<sculptor_qwtplotcurve*> getCurves();

public slots:

    /**
     * update canvas
     */
    void sReplot();

    /**
     * close window
     */
    virtual void sHide();

    /**
     * save data to file in CSV format
     */
    void sSave();

    /**
     * print plot canvas
     */
    void sPrint();    

    /**
     * set visibility of a curve
     */
    void sSetVisible(bool bVisible);
};

#endif
