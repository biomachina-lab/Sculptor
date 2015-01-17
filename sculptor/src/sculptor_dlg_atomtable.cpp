/***************************************************************************
                          sculptor_dlg_atomtable.cpp
			  -------------------
    begin                : 11/16/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_dlg_atomtable.h>
#include <pdb_document.h>
// svt includes
#include <svt_pdb.h>
// qt4 includes
#include <QGridLayout>
#include <QTableView>
#include <QHeaderView>
#include <QCloseEvent>
#include <QButtonGroup>
#include <QRadioButton>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 */
dlg_atomtable::dlg_atomtable(QWidget* pParent, pdb_document* pPdb)
    : QDialog(pParent)
{
    setSizeGripEnabled(TRUE);

    QGridLayout* pLayout = new QGridLayout(this);

    // button group for the coord source radio buttons
    m_pCoordSource = new QButtonGroup(this);

    // radiobutton for atom coordinate source PDB
    QRadioButton*  pButtonPDB = new QRadioButton( QString("PDB Coordinates"), this );
    m_pCoordSource->addButton(pButtonPDB, pdb_document_model::PDB_COORD_SOURCE_PDB);
    pLayout->addWidget(pButtonPDB, 0, 0, 1, 1);

    // radiobutton for atom coordinate source sculptor
    QRadioButton* pButtonSculptor = new QRadioButton( QString("Internal Sculptor Coordinates"), this );
    m_pCoordSource->addButton(pButtonSculptor, pdb_document_model::PDB_COORD_SOURCE_SCULPTOR);
    pLayout->addWidget(pButtonSculptor, 0, 1, 1, 1);

    // add a spacer
    pLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 2, 1, 1);

    // and the table itself
    QTableView* pTable = new QTableView(this);
    pTable->verticalHeader()->hide();
    pLayout->addWidget(pTable, 1, 0, 1, 3);


    // set the model (Qt model/view framework)
    pdb_document_model* pModel = new pdb_document_model(pPdb, pTable);
    pTable->setModel(pModel);

    // let the first row be high enough to display header names with a line break
    pTable->resizeRowToContents(0);
    // let the columns be as wide as their contents
    // not a good idea -- it will scan through the whole dataset to determine the size
    //pTable->resizeColumnsToContents(); 
    // so we prefer to set the size manually
    pTable->setColumnWidth(0,50);
    pTable->setColumnWidth(1,50);
    pTable->setColumnWidth(2,40);
    pTable->setColumnWidth(3,50);
    pTable->setColumnWidth(4,50);
    pTable->setColumnWidth(5,70);
    pTable->setColumnWidth(6,50);
    pTable->setColumnWidth(7,100);
    pTable->setColumnWidth(8,100);
    pTable->setColumnWidth(9,100);
    pTable->setColumnWidth(10,60);
    pTable->setColumnWidth(11,60);
    pTable->setColumnWidth(12,50);
    pTable->setColumnWidth(13,40);
    pTable->setColumnWidth(14,70);
    pTable->setColumnWidth(15,60);
    pTable->setColumnWidth(16,60);

    // let the last column be stretched over the max. available space
    pTable->horizontalHeader()->setStretchLastSection(TRUE);

    // check the default source (set in pdb_document_model::pdb_document_model()) and set the
    // corresponding button checked
    if ( pModel->getCoordinateSource() == pdb_document_model::PDB_COORD_SOURCE_SCULPTOR )
        pButtonSculptor->setChecked(TRUE);
    else if ( pModel->getCoordinateSource() == pdb_document_model::PDB_COORD_SOURCE_PDB )
        pButtonPDB->setChecked(TRUE);

    // when the coordinate source changes, refresh the table
    connect( m_pCoordSource, SIGNAL(buttonClicked(int)), pModel, SLOT(setCoordinateSource(int)) );
}

/**
 *
 */
void dlg_atomtable::closeEvent(QCloseEvent * oEvent)
{
    oEvent->ignore();
    hide();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
 *
 */
pdb_document_model::pdb_document_model(pdb_document* pDoc, QTableView* pTable)
    : QAbstractTableModel(),
      m_pDoc( pDoc ),
      m_pPdbNode( (svt_pdb*)pDoc->getNode() ),
      m_iCoordSource( PDB_COORD_SOURCE_PDB ),
      m_pTable( pTable )
{
    m_oCenter = m_pDoc->getCenter();
}

/**
 *
 */
void pdb_document_model::setCoordinateSource(int iSource)
{
    if ( m_iCoordSource != iSource )
    {
        m_iCoordSource = iSource;

        // get the center of atoms
        if ( m_iCoordSource == PDB_COORD_SOURCE_PDB )
            m_oCenter = m_pDoc->getCenter();

        m_pTable->viewport()->update();
    }
}

/**
 *
 */
int pdb_document_model::getCoordinateSource()
{
    return m_iCoordSource;
}

/**
 *
 */
int pdb_document_model::rowCount(const QModelIndex&) const
{
    return m_pPdbNode->size();
}

/**
 *
 */
int pdb_document_model::columnCount(const QModelIndex&) const
{
    return 17;
}

/**
 *
 */
QVariant pdb_document_model::headerData(int iSection, Qt::Orientation iOrientation, int iRole) const
{
    if ( iRole != Qt::DisplayRole || iOrientation != Qt::Horizontal )
        return QVariant();

    switch (iSection)
    {
        case  0: return QVariant("Index");
        case  1: return QVariant("Name");
        case  2: return QVariant("Alt.\nLoc.");
        case  3: return QVariant("Res.\nName");
        case  4: return QVariant("Chain\nID");
        case  5: return QVariant("Residue\nSeq.No.");
        case  6: return QVariant("iCode");
        case  7: return QVariant("X\nCoord.");
        case  8: return QVariant("Y\nCoord.");
        case  9: return QVariant("Z\nCoord.");
        case 10: return QVariant("Occup.");
        case 11: return QVariant("Temp.\nFactor");
        case 12: return QVariant("Note");
        case 13: return QVariant("Seg.\nID");
        case 14: return QVariant("Element");
        case 15: return QVariant("Charge");
        case 16: return QVariant("Sec.\nStruct.");
    }
    return QVariant();
}

/**
 *
 */
QVariant pdb_document_model::data(const QModelIndex &oIndex, int iRole) const
{
    if ( iRole != Qt::DisplayRole)
        return QVariant();

    switch ( oIndex.column() )
    {
        case  0: return QVariant( m_pPdbNode->atom(oIndex.row()).getPDBIndex() );
        case  1: return QVariant( m_pPdbNode->atom(oIndex.row()).getName() );
        case  2: return QVariant( m_pPdbNode->atom(oIndex.row()).getAltLoc() );
        case  3: return QVariant( m_pPdbNode->atom(oIndex.row()).getResname() );
        case  4: return QVariant( m_pPdbNode->atom(oIndex.row()).getChainID() );
        case  5: return QVariant( m_pPdbNode->atom(oIndex.row()).getResidueSeq() );
        case  6: return QVariant( m_pPdbNode->atom(oIndex.row()).getICode() );
        case  7:
            if (m_iCoordSource == PDB_COORD_SOURCE_SCULPTOR)
                return QVariant( (*m_pPdbNode)[oIndex.row()].x() );
            else
                return QVariant( (*m_pPdbNode)[oIndex.row()].x() + m_oCenter.x() );
        case  8:
            if (m_iCoordSource == PDB_COORD_SOURCE_SCULPTOR)
                return QVariant( (*m_pPdbNode)[oIndex.row()].y() );
            else
                return QVariant( (*m_pPdbNode)[oIndex.row()].y() + m_oCenter.y() );
        case  9:
            if (m_iCoordSource == PDB_COORD_SOURCE_SCULPTOR)
                return QVariant( (*m_pPdbNode)[oIndex.row()].z() );
            else
                return QVariant( (*m_pPdbNode)[oIndex.row()].z() + m_oCenter.z() );
        case 10: return QVariant( m_pPdbNode->atom(oIndex.row()).getOccupancy() );
        case 11: return QVariant( m_pPdbNode->atom(oIndex.row()).getTempFact() );
        case 12: return QVariant( m_pPdbNode->atom(oIndex.row()).getNote() );
        case 13: return QVariant( m_pPdbNode->atom(oIndex.row()).getSegmentID() );
        case 14: return QVariant( m_pPdbNode->atom(oIndex.row()).getElement() );
        case 15: return QVariant( m_pPdbNode->atom(oIndex.row()).getCharge() );
        case 16: return QVariant( QChar(m_pPdbNode->atom(oIndex.row()).getSecStruct()) );
    }
    return QVariant();
}
