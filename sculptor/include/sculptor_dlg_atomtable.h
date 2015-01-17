/*-*-c++-*-*****************************************************************
                          sculptor_dlg_atomtable.h
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
class QTableView;
class QCloseEvent;
class QButtonGroup;
// sculptor includes
class pdb_document;
// svt includes
#include <svt_vector4.h>
class svt_pdb;

/**
 * This class is a window with a table that displays atomic data from an svt_pdb
 * \author Manuel Wahle
 */
class dlg_atomtable : public QDialog
{
    Q_OBJECT

    // button group for the coord source radio buttons
    QButtonGroup* m_pCoordSource;

public:

    /**
     * Constructor
     */
    dlg_atomtable(QWidget* pParent, pdb_document* pPdb);

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
class pdb_document_model : public QAbstractTableModel
{
    Q_OBJECT

    // the pdfb_document holding the pdb node
    pdb_document* m_pDoc;

    // the svt_pdb where the atmic data comes from
    svt_pdb* m_pPdbNode;

    // atom coordinate source - pdb or sculptor
    int m_iCoordSource;

    // pointer to the table where the atom data is displayed
    QTableView* m_pTable;

    // the center of atoms, needed to convert Sculptor to PDB coordinates
    svt_vector4< Real64 > m_oCenter;

public:

    enum { PDB_COORD_SOURCE_PDB,
           PDB_COORD_SOURCE_SCULPTOR };

    /**
     * Constructor
     */
    pdb_document_model(pdb_document* pDoc, QTableView* pTable);

    /**
     * Virtual method from QAbstractTableModel to return the number of rows (i.e., atoms)
     */
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * Virtual method from QAbstractTableModel to return the number of columns (i.e., atom
     * properties)
     */
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * Virtual method from QAbstractTableModel to return the atomic data
     */
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    /**
     * Virtual method from QAbstractTableModel to return the column names (i.e., names of the atomic
     * properties)
     */
    virtual QVariant headerData(int iSection, Qt::Orientation iOrientation, int iRole = Qt::DisplayRole) const;

    /**
     * get the type of coordinate source - pdb coords or internal sculptor coords
     */
    int getCoordinateSource();

public slots:

    /**
     * set the type of coordinate source - pdb coords or internal sculptor coords
     */
    void setCoordinateSource(int iSource);
};
