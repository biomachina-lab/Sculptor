/*-*-c++-*-*****************************************************************
                          sculptor_dlg_maptable.h
			  -------------------
    begin                : 11/16/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// qt4 includes
#include <QDialog>
#include <QVariant>
#include <QAbstractTableModel>
class QCloseEvent;
// svt includes
#include <svt_volume.h>


/**
 * This class is a window with a table that displays volumetri data values from an svt_volume
 * \author Manuel Wahle
 */
class dlg_maptable : public QDialog
{

public:

    /**
     * Constructor
     */
    dlg_maptable(QWidget* pParent, svt_volume<Real64>* pVolume);

protected:

    /**
     * Override the closeEvent to just hide the window instead of closing it
     */
    virtual void closeEvent(QCloseEvent * oEvent);
};



/**
 * This class is the model for the Qt model/view framework. It delivers dthe atomic ata to the tableview.
 * \author Manuel Wahle
 */
class situs_document_model : public QAbstractTableModel
{
    // the svt_pdb where the atmic data comes from
    svt_volume<Real64>* m_pVolume;

public:

    /**
     * Constructor
     */
    situs_document_model(svt_volume<Real64>* pVolumeNode);

    /**
     * Virtual method from QAbstractTableModel to return the number of rows (like in the situs file
     * format)
     */
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * Virtual method from QAbstractTableModel to return the number of columns (10 like in the situs
     * file format)
     */
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * Virtual method from QAbstractTableModel to return the volumetric data values
     */
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    /**
     * Virtual method from QAbstractTableModel to return the column/row names (i.e., 1 to 10 like in
     * the situs file format)
     */
    virtual QVariant headerData(int iSection, Qt::Orientation iOrientation, int iRole = Qt::DisplayRole) const;
};
