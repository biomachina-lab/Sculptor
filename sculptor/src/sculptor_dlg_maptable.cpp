/***************************************************************************
                          sculptor_dlg_maptable.cpp
			  -------------------
    begin                : 11/16/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_dlg_maptable.h>
// qt4 includes
#include <QGridLayout>
#include <QTableView>
#include <QHeaderView>
#include <QCloseEvent>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 */
dlg_maptable::dlg_maptable(QWidget* pParent, svt_volume<Real64>* pVolume)
    : QDialog(pParent)
{
    setSizeGripEnabled(TRUE);

    QGridLayout* pLayout = new QGridLayout(this); 
    QTableView* pTable = new QTableView();

    // set the model (Qt model/view framework)
    situs_document_model* pModel = new situs_document_model(pVolume);
    pTable->setModel(pModel);

    pLayout->addWidget( pTable );
}

/**
 *
 */
void dlg_maptable::closeEvent(QCloseEvent * oEvent)
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
situs_document_model::situs_document_model(svt_volume<Real64>* pVolume)
    : QAbstractTableModel(),
      m_pVolume( pVolume )
{
}

/**
 *
 */
int situs_document_model::rowCount(const QModelIndex&) const
{
    return (int)m_pVolume->getSizeX() * m_pVolume->getSizeY() * m_pVolume->getSizeZ() * 1.0E-1;
}

/**
 *
 */
int situs_document_model::columnCount(const QModelIndex&) const
{
    return 10;
}

/**
 *
 */
QVariant situs_document_model::headerData(int iSection, Qt::Orientation, int iRole) const
{
    if ( iRole != Qt::DisplayRole )
        return QVariant();

    return QVariant(iSection+1);
}

/**
 *
 */
QVariant situs_document_model::data(const QModelIndex &oIndex, int iRole) const
{
    if ( iRole != Qt::DisplayRole)
        return QVariant();

    return QVariant( m_pVolume->getValue(oIndex.row()*10 + oIndex.column()) );
}
