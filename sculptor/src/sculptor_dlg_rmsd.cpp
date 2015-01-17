/***************************************************************************
                          sculptor_dlg_rmsd.cpp
                          -----------------
    begin                : 09.10.2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_window.h>
#include <sculptor_dlg_rmsd.h>
#include <sculptor_window_documentlist.h>
// qt4 includes
#include <QString>

extern sculptor_window* g_pWindow;
extern string QtoStd(QString);


/**
 * Constructor
 */
dlg_rmsd::dlg_rmsd(QWidget* pParent)
    : QDialog( pParent ),
      m_pFirst(NULL),
      m_pSecond(NULL)
{
    setupUi(this);

    // if user hits OK button, check if the string has correct PDB ID format
    connect( m_pCompute, SIGNAL(clicked()), this, SLOT( computeRMSD() ) );
}

/**
 * Destructor
 */
dlg_rmsd::~dlg_rmsd()
{
}
/**
 * get Selected documents
 */
void dlg_rmsd::getDocs()
{
    // First step = look for the first two selected pdb documents
    m_pFirst = NULL;
    m_pSecond = NULL;

    window_documentlist *pDocList =  g_pWindow->getDocumentList();
    for(int i=1; i<pDocList->count(); ++i)
    {
        if ( pDocList->getSelected(i) && pDocList->getDocType(i) == SENSITUS_DOC_PDB && m_pFirst==NULL  )
            m_pFirst = (pdb_document*)pDocList->getDoc(i);
        else if ( pDocList->getSelected(i) && pDocList->getDocType(i) == SENSITUS_DOC_PDB && m_pSecond==NULL  )
            m_pSecond = (pdb_document*)pDocList->getDoc(i);
    }

    if (m_pFirst==NULL || m_pSecond==NULL)
    {
        svt_exception::ui()->info("Please select two pdb files!");
        return;
    }
};

/**
 * show/modify option based on selection
 */
void dlg_rmsd::updateUi()
{
    getDocs();

    if (m_pFirst != NULL && m_pSecond != NULL && m_pFirst != m_pSecond)
    {
        if (m_pFirst->size() != m_pSecond->size())
            m_pAlign->setEnabled(false);
        else
            m_pAlign->setEnabled(true);
    }

};

/**
 * computes RMSD
 */
void dlg_rmsd::computeRMSD()
{
    if (m_pFirst==NULL || m_pSecond==NULL)
        updateUi();

    if (m_pFirst != NULL && m_pSecond != NULL && m_pFirst != m_pSecond)
    {
        // dialog option
        bool bAlign = m_pAlign->isChecked();
        string pSelectedAtoms = QtoStd( m_pSelectedAtoms->currentText() );

        svt_point_cloud_pdb<svt_vector4<Real64> > oFirst = *m_pFirst->getPDB();
        svt_point_cloud_pdb<svt_vector4<Real64> > oSecond = *m_pSecond->getPDB();

        double fRMSD = 0.0f;
        string oString;
        char oResults[256];

        Select iSelection = ALL;
        vector<Select> aSelection;
        aSelection.push_back(ALL);
        aSelection.push_back(BACKBONE);
        aSelection.push_back(TRACE);

        vector<string> aSelectionNames;
        aSelectionNames.push_back("All atoms RMSD:");
        aSelectionNames.push_back("Backbone RMSD:");
        aSelectionNames.push_back("Trace RMSD:");

        switch(toupper(pSelectedAtoms[0]))
        {
            case 'A':
                iSelection = ALL;
                break;

            case 'B':
                iSelection = BACKBONE;
                break;

            case 'T':
                iSelection = TRACE;
                break;
        }

        switch(toupper(pSelectedAtoms[0]))
        {
            case 'A': case 'B': case 'T':
                fRMSD = oFirst.rmsd( oSecond, bAlign, iSelection);
                if (fRMSD == 1.0E10)
                {
                    g_pWindow->guiWarning("RMSD Error." ,"The RMSD could not be computed. Please check the number of atoms. ","RMSD Error" );
                    return;
                }
                sprintf(oResults, "%s %5.3f Angstroem", pSelectedAtoms.c_str(), fRMSD);
                g_pWindow->guiInfo(pSelectedAtoms , string(oResults), string(oResults) );
                break;

            case 'S':
                oString = "";
                for (unsigned int iIndex = 0; iIndex< aSelection.size(); iIndex++)
                {
                    fRMSD = oFirst.rmsd( oSecond, bAlign, aSelection[iIndex]);
                    if (fRMSD== 1.0E10)
                    {
                        g_pWindow->guiWarning("RMSD Error." ,"The RMSD could not be computed. Please check the number of atoms. ","RMSD Error" );
                        return;
                    }
                    sprintf(oResults, "%s %5.3f Angstroem\n", aSelectionNames[iIndex].c_str(), fRMSD);
                    oString += string(oResults);
                }
                g_pWindow->guiInfo("Compute RMSD", oString, "Compute RMSD" );
                break;
        }
    }
    else
        g_pWindow->guiWarning("RMSD Error." ,"Please select two pdb files in order to calculate the rmsd between them.","RMSD Error" );
}
