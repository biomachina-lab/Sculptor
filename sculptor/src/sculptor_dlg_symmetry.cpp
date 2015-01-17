/***************************************************************************
                          sculptor_dlg_symmetry.cpp
                          -----------------
    begin                : 11.11.2010
    author               : Sayan Ghosh
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
//#include <sculptor_document.h>
#include <sculptor_window.h>
#include <sculptor_window_documentlist.h>
#include <sculptor_dlg_symmetry.h>
#include <pdb_document.h>
//#include <sculptor_scene.h>
//svt includes
#include <svt_vector3.h>
//qt4 includes
#include <QString>

extern sculptor_window* g_pWindow;
extern QString StdtoQ(string);


/**
 * Constructor
 */
dlg_symmetry::dlg_symmetry(QWidget* pParent, pdb_document* pDoc)
    : QDialog( pParent ),
      m_pDoc( (pdb_document*) pDoc )
{
    setupUi(this);

    // button connections
    connect (m_pApply,       SIGNAL(clicked()),                this, SLOT( applySymmetry()) );
    connect (m_pCancel,      SIGNAL(clicked()),                this, SLOT( hide()) );
    // combobox connection
    connect (m_pPdbFiles,    SIGNAL(currentIndexChanged(int)), this, SLOT( updatePoint2(int)) );

    // fill the dialog
    init();
}

/**
 * Destructor
 */
dlg_symmetry::~dlg_symmetry()
{
}

/**
 *
 */
void dlg_symmetry::init()
{
    vector<sculptor_document*> aDocs = g_pWindow->getDocumentList()->getDocuments(false, SENSITUS_DOC_PDB);


    // remember all the pdb documents in the scene
    m_pPDBDocs.clear();
    m_pPdbFiles->clear();
    m_pPdbFiles->addItem( QString("none") );

    for (unsigned int i=0; i<aDocs.size(); ++i)
    {
        m_pPdbFiles->addItem( StdtoQ(aDocs[i]->getNameShort()) );
        m_pPDBDocs.push_back((pdb_document*)aDocs[i]);
    }
}

/**
 *
 */
void dlg_symmetry::updatePoint2(int iIndex)
{
    // add pdb files
    svt_vector4< Real64 > oPoint(0.0, 0.0, 1.0);

    if ( iIndex >= 1 && iIndex <= (int)m_pPDBDocs.size() )
        oPoint = ((pdb_document*)m_pPDBDocs[iIndex-1])->getCenter();


    char pValue[256];
    sprintf( pValue, "%8.3f", oPoint.x() );
    m_pPoint2X->setText( QString(pValue) );

    sprintf(pValue, "%8.3f", oPoint.y());
    m_pPoint2Y->setText( QString(pValue) );

    sprintf(pValue, "%8.3f", oPoint.z());
    m_pPoint2Z->setText( QString(pValue) );
}

/**
 *
 */
void dlg_symmetry::applySymmetry()
{
    // add pdb files
    svt_vector3<Real64> oPoint1, oPoint2;

    oPoint1.x( m_pPoint1X->text().toDouble() );
    oPoint1.y( m_pPoint1Y->text().toDouble() );
    oPoint1.z( m_pPoint1Z->text().toDouble() );

    if (m_pPdbFiles->currentIndex()==0) // none is selected then use the values given in the box
    {
        oPoint2.x( m_pPoint2X->text().toDouble() );
        oPoint2.y( m_pPoint2Y->text().toDouble() );
        oPoint2.z( m_pPoint2Z->text().toDouble() );
    }
    else
    {
        cout << m_pPdbFiles->currentIndex() - 1  << " "  << m_pPDBDocs.size() << endl;
        if ( m_pPdbFiles->currentIndex() - 1 < (int)m_pPDBDocs.size() )
        {
            oPoint2.x( (((pdb_document*)m_pPDBDocs[m_pPdbFiles->currentIndex()-1])->getCenter() )[0][0]);
            oPoint2.y( (((pdb_document*)m_pPDBDocs[m_pPdbFiles->currentIndex()-1])->getCenter() )[0][1]);
            oPoint2.z( (((pdb_document*)m_pPDBDocs[m_pPdbFiles->currentIndex()-1])->getCenter() )[0][2]);
        }
    }

    hide();

    m_pDoc->applySymmetry(oPoint1, oPoint2, (int) m_pOrder->value());
}
