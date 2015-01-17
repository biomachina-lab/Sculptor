/***************************************************************************
			  sculptor_dlg_extract.cpp
			  -------------------
    begin                : 08/2010
    author               : Sayan Ghosh
    email                : sculptor@biomachina.org
***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <pdb_document.h>
#include <sculptor_scene.h>
#include <sculptor_window.h>
#include <sculptor_dlg_extract.h>
// svt includes
#include <svt_string.h>
#include <svt_stlVector.h>
#include <svt_pdb.h>

extern string QtoStd(QString);

/**
 *
 */
dlg_extract::dlg_extract( QWidget* pParent,
			  svt_pdb * pPdbNode,
                          pdb_document * pPdbDocument,
                          QString oShortName,
                          sculptor_window * pSculptorWindow )
    : QDialog( pParent ),
      m_oShortName( oShortName ),
      m_pSculptorWindow( pSculptorWindow ),
      m_pPdbDocument( pPdbDocument ),
      m_pPdbNode( pPdbNode )
{
    setupUi(this);

    connect( m_pExtract, SIGNAL(clicked()),     this,               SLOT(accept()) );
    connect( m_pExtract, SIGNAL(clicked()),     this,               SLOT(extract()) );

    connect( m_pFillRange, SIGNAL(clicked()),   this ,              SLOT(updateExtractDlg()) );
    connect( m_pFillRange, SIGNAL(clicked()),   m_pExtractRangeLE , SLOT(selectAll()) );

    connect( m_pMethod, SIGNAL(activated(int)), this,               SLOT(updateExtractDlg()) );

    connect( m_pCancel, SIGNAL(clicked()),      this,               SLOT(reject()) );
}

/**
 *
 */
dlg_extract::~dlg_extract()
{
}

/**
 *
 */
void dlg_extract::updateExtractDlg()
{
    QString oStr;
    vector<char > aC;
    vector<unsigned int> aUI;
    unsigned int iStepCount;

    vector <string> elems;
    string range;
    stringstream piece;

    switch(m_pMethod->currentIndex())
    {
        case 0:
            m_pExtractRangeLE->setEnabled(true);

            aC = m_pPdbNode->getAtomChainIDs();

            for (unsigned int iIndex=0; iIndex<aC.size(); iIndex++)
            {
                piece << aC[iIndex];
                elems.push_back( piece.str() );
                piece.str(std::string());
            }

            range = svt_makeRange (elems);

            if (!range.empty())
                m_pExtractRangeLE->setText( QString(range.c_str()) );
            else
            {
                m_pExtractRangeLE->clear();
                //disable lineedit
                m_pExtractRangeLE->setEnabled(false);
            }

            break;
        case 1:
            m_pExtractRangeLE->setEnabled(true);

            aUI = m_pPdbNode->getAtomModels();

            for (unsigned int iIndex=0; iIndex<aUI.size(); iIndex++)
            {
                piece << aUI[iIndex];
                elems.push_back( piece.str() );
                piece.str(std::string());
            }

            range = svt_makeRange(elems);

            if (!range.empty())
                m_pExtractRangeLE->setText( QString(range.c_str()) );
            else
            {
                m_pExtractRangeLE->clear();
                //disable lineedit
                m_pExtractRangeLE->setEnabled(false);
            }

            break;
        case 2:
            m_pExtractRangeLE->setEnabled(true);
            //flush aUI for re-use
            aUI.erase (aUI.begin(), aUI.end());
            iStepCount = ((*m_pPdbDocument).getPointCloud()).getMaxTimestep(); //((*this).getPointCloud()).getMaxTimestep();

            for (unsigned int i = 0; i < iStepCount; i++)
            {
                aUI.push_back(i);
            }


            for (unsigned int iIndex=0; iIndex<iStepCount; iIndex++)
            {
                piece << aUI[iIndex];
                elems.push_back( piece.str() );
                piece.str(std::string());
            }

            range = svt_makeRange(elems);

            if (!range.empty())
                m_pExtractRangeLE->setText( QString(range.c_str()) );
            else
            {
                m_pExtractRangeLE->clear();
                //disable lineedit
                m_pExtractRangeLE->setEnabled(false);
            }

            break;
        case 3:
            m_pExtractRangeLE->clear();
            //disable lineedit
            m_pExtractRangeLE->setEnabled(false);
            //disable linedit
            break;
        case 4:
            m_pExtractRangeLE->clear();
            //disable lineedit
            m_pExtractRangeLE->setEnabled(false);
            //disable linedit
            break;
        case 5:
            m_pExtractRangeLE->setEnabled(true);
            //Secondary structures
            //flush aC for re-use
            aC.erase (aC.begin(), aC.end());
            //Does secondary structure exists?
            aC = m_pPdbNode->getAtomSecStructIDs();

            if (aC.size() > 0)
            {
                //Extract secondary structures but leave coils -- C
                for (unsigned int iIndex=0; iIndex<aC.size(); iIndex++)
                {
                    if (aC[iIndex] != 'C')
                    {
                        piece << aC[iIndex];
                        elems.push_back( piece.str() );
                        piece.str(std::string());
                    }
                    else
                        continue;
                }

                if (elems.size() > 0)
                {
                    range = svt_makeRange(elems);
                    m_pExtractRangeLE->setText( QString(range.c_str()) );
                }
                else
                {
                    m_pExtractRangeLE->clear();
                    //disable lineedit
                    m_pExtractRangeLE->setEnabled(false);
                }
                //disable linedit

            }
            else
            {
                m_pExtractRangeLE->clear();
                //disable lineedit
                m_pExtractRangeLE->setEnabled(false);
            }
            //disable linedit
            break;
    }
}

/**
 *
 */
void dlg_extract::extract()
{
    QString oFileName;
    vector<char> aC, aCAll, aCN;
    vector<unsigned int> aUI, aUIAll, aUIN;
    vector<string> expandedList;

    int iAtom = -1;
    svt_atom* pAtom = NULL;
    pdb_document* pNewDoc;
    char pWarning[1256];
    svt_point_cloud_pdb< svt_vector4<Real64> >* rPDB = new svt_point_cloud_pdb< svt_vector4<Real64> >();
    unsigned int iStepCount;
    unsigned int iWarningSize = 10;
    unsigned int iIndexMin, iIndexMax;
    unsigned int iOldTimeStep;
    char sc;

    vector<char>::iterator itC;
    vector<unsigned int>::iterator itU;
    string range;

    unsigned int iSelMode = m_pMethod->currentIndex();

    if ( m_pExtractRangeLE->isEnabled() ) //lineedit is enabled
    {
        //Get the range from the lineedit

        range = QtoStd(m_pExtractRangeLE->text());
        //cout << "range = " << range << endl;

        if (!range.empty())
            expandedList = svt_expandRange(range);
    }

    switch (iSelMode)
    {

        case 0:  //all chain
        {
            //populate aC from the range specified by user
            for (unsigned int i = 0; i < expandedList.size(); i++)
            {
                if (expandedList[i].length() > 1)
                    continue;
                else
                    aC.push_back(toupper(expandedList[i].c_str()[0]));
            }

            aCAll = m_pPdbNode->getAtomChainIDs();

            for (unsigned int k = 0; k < aCAll.size(); k++)
            {
                itC = std::find( aC.begin(), aC.end(), aCAll[k] );
                if ( itC != aC.end() )
                    aCN.push_back( aCAll[k] );
                else
                    continue;

            }

            if ( !aCN.size() )
            {
                //cout << "The PDB does not have the input chain of " << notinrange << endl;
                svt_exception::ui()->info("Something is wrong with the input; please check again");
            }

            else if ( aCN.size() > iWarningSize )
            {
                sprintf( pWarning,"The pdb file has %d chains. Are you sure you want to proceed with the split?", (int)aCN.size());

                if ( !svt_exception::ui()->question( pWarning ))
                    return;
            }

            //cout << "aCN size = " << aCN.size() << endl;

            for (unsigned int iIndex=0; iIndex < aCN.size(); iIndex++)
            {
                m_pPdbNode->resetAtomEnum();

                if (rPDB->size()>0)
                    rPDB->deleteAllAtoms();

                try
                {
                    int iPerc = (int)((((float)( iIndex ) / (float)( aCN.size() )) * 100.0));
                    svt_exception::ui()->progress( iPerc);

                    iAtom = m_pPdbNode->enumAtomChainID( aCN[iIndex] );

                    while (iAtom != -1)
                    {
                        pAtom = m_pPdbNode->getRenAtom(iAtom);
                        rPDB->addAtom(*pAtom, ((*m_pPdbDocument).getPointCloud())[iAtom] );
                        iAtom = m_pPdbNode->enumAtomChainID( aCN[iIndex] );
                    }
                }
                catch (svt_userInterrupt&)
                {
                    //delete
                    rPDB->deleteAllAtoms();
                    svt_exception::ui()->progressPopdown();
                    return;
                }

                rPDB->setSecStructSource(SSE_OTHER);
                rPDB->calcAtomModels();

                //set name
                oFileName.sprintf( "%s_Chain_%c.pdb", QtoStd(m_oShortName).c_str(), aCN[iIndex] );

                // create new document
                pNewDoc = new pdb_document(m_pSculptorWindow->getScene()->getDataTopNode(),
                                           QtoStd(oFileName), SVT_PDB,
                                           "",                SVT_NONE,
                                           rPDB);


                // set transformation as the original PDB
                svt_matrix4f* pTransformation = new svt_matrix4f;
                *pTransformation = *(m_pPdbNode->getTransformation());

                //set the own translation
                svt_vector4f oOrigin = svt_vector4f(pNewDoc->getRealPos().x(),
                                                    pNewDoc->getRealPos().y(),
                                                    pNewDoc->getRealPos().z());
                oOrigin.multScalar( 1.0E8f );
                pTransformation->preAddTranslation( oOrigin.x(),oOrigin.y(),oOrigin.z() );

                pNewDoc->getNode()->setTransformation(pTransformation);
                pNewDoc->storeInitMat();
                pNewDoc->setChanged();

                m_pSculptorWindow->addDocSimple( (sculptor_document*)pNewDoc );
            }

            svt_exception::ui()->progressPopdown();
        }
        break;

        case 1:// split by model
        {
            //re-use aUIAll, aUI and aUIN
            aUI.erase (aUI.begin(), aUI.end());
            aUIAll.erase (aUIAll.begin(), aUIAll.end());
            aUIN.erase (aUIN.begin(), aUIN.end());

            //populate aUI from the range specified by user
            for (unsigned int i = 0; i < expandedList.size(); i++)
            {
                aUI.push_back(atoi(expandedList[i].c_str()));
            }

            //cout << "aUI.size() = " << aUI.size() << endl;


            aUIAll = m_pPdbNode->getAtomModels();

            for (unsigned int k = 0; k < aUIAll.size(); k++)
            {
                itU = std::find( aUI.begin(), aUI.end(), aUIAll[k] );
                if ( itU != aUI.end() )
                    aUIN.push_back( aUIAll[k] );
                else
                    continue;
            }

            if ( !aUIN.size() )
            {
                svt_exception::ui()->info("Something is wrong with the input; please check again");
            }
            else if ( aUIN.size() > iWarningSize )
            {
                sprintf( pWarning,"The pdb file has %d models. Are you sure you want to proceed with the split?", (int)aUIN.size());
                if ( !svt_exception::ui()->question( pWarning ))
                    return;
            }

            for (unsigned int iIndex=0; iIndex < aUIN.size(); iIndex++)
            {
                m_pPdbNode->resetAtomEnum();

                if (rPDB->size()>0)
                    rPDB->deleteAllAtoms();

                try
                {
                    int iPerc = (int)((((float)( iIndex ) / (float)( aUIN.size() )) * 100.0));
                    svt_exception::ui()->progress( iPerc);

                    iAtom = m_pPdbNode->enumAtomModel( aUIN[iIndex] );
                    while (iAtom != -1)
                    {
                        pAtom = m_pPdbNode->getRenAtom(iAtom);
                        rPDB->addAtom(*pAtom, ((*m_pPdbDocument).getPointCloud())[iAtom] );
                        iAtom = m_pPdbNode->enumAtomModel( aUIN[iIndex] );
                    }
                }
                catch (svt_userInterrupt&)
                {
                    //delete
                    rPDB->deleteAllAtoms();
                    svt_exception::ui()->progressPopdown();
                    return;
                }

                rPDB->setSecStructSource(SSE_OTHER);
                rPDB->calcAtomModels();

                //set name
                oFileName.sprintf( "%s_Model_%d.pdb", QtoStd(m_oShortName).c_str(), aUIN[iIndex] );

                // create new document
                pNewDoc = new pdb_document(m_pSculptorWindow->getScene()->getDataTopNode(),
                                           QtoStd(oFileName), SVT_PDB,
                                           "",                SVT_NONE,
                                           rPDB);

                // set transformation as the original PDB
                svt_matrix4f* pTransformation = new svt_matrix4f;
                *pTransformation = *(m_pPdbNode->getTransformation());

                //set the own translation
                svt_vector4f oOrigin = svt_vector4f(pNewDoc->getRealPos().x(),
                                                    pNewDoc->getRealPos().y(),
                                                    pNewDoc->getRealPos().z());
                oOrigin.multScalar( 1.0E8f );
                pTransformation->preAddTranslation( oOrigin.x(),oOrigin.y(),oOrigin.z() );

                pNewDoc->getNode()->setTransformation(pTransformation);
                pNewDoc->storeInitMat();
                pNewDoc->setChanged();

                m_pSculptorWindow->addDocSimple( (sculptor_document*)pNewDoc );
            }

            svt_exception::ui()->progressPopdown();
        }
        break;

        case 2: //split by frame;
        {
            //re-use aUIAll, aUI and aUIN
            aUI.erase (aUI.begin(), aUI.end());
            aUIAll.erase (aUIAll.begin(), aUIAll.end());
            aUIN.erase (aUIN.begin(), aUIN.end());

            //convert expandedList contents to unsigned int
            for (unsigned int i = 0; i < expandedList.size(); i++)
            {
                aUI.push_back(atoi(expandedList[i].c_str()));
            }

            iStepCount = ((*m_pPdbDocument).getPointCloud()).getMaxTimestep();//((*this).getPointCloud()).getMaxTimestep();

            //fill aUIAll with the frames
            for (unsigned int i = 0; i < iStepCount; i++)
            {
                aUIAll.push_back(i);
            }
            //////////////////////////////////////////////////
            for (unsigned int k = 0; k < aUIAll.size(); k++)
            {
                itU = std::find( aUI.begin(), aUI.end(), aUIAll[k] );
                if ( itU != aUI.end())
                    aUIN.push_back( aUIAll[k] );
                else
                    continue;
            }

            if ( !aUIN.size() )
            {
                svt_exception::ui()->info("Something is wrong with the input; please check again");
            }
            else if ( aUIN.size() > iWarningSize )
            {
                sprintf( pWarning,"The pdb file has %d frames. Are you sure you want to proceed with the split?", (int)aUIN.size());
                if ( !svt_exception::ui()->question( pWarning ))
                    return;
            }

            iIndexMin = aUIN[0];
            iIndexMax = (aUIN[aUIN.size() - 1]);

            svt_exception::ui()->progressPopup("Extract Frame", 0);


            iOldTimeStep = ((*m_pPdbDocument).getPointCloud()).getTimestep();
            for (unsigned int iIndex=iIndexMin; iIndex <= iIndexMax; iIndex++)
            {
                if (rPDB->size()>0)
                    rPDB->deleteAllAtoms();

                try
                {
                    int iPerc = (int)((((float)( iIndex ) / (float)( iStepCount )) * 100.0));
                    svt_exception::ui()->progress( iPerc);

                    ((*m_pPdbDocument).getPointCloud()).setTimestep( iIndex );
                    for (unsigned int iIndexAtom=0; iIndexAtom<((*m_pPdbDocument).getPointCloud()).size(); iIndexAtom++)
                    {
                        pAtom = (svt_atom*)((*m_pPdbDocument).getPointCloud()).getAtom(iIndexAtom);
                        rPDB->addAtom(*pAtom, ((*m_pPdbDocument).getPointCloud())[iIndexAtom] );
                    }
                }
                catch (svt_userInterrupt&)
                {
                    //delete
                    rPDB->deleteAllAtoms();
                    svt_exception::ui()->progressPopdown();
                    return;
                }

                rPDB->setSecStructSource(SSE_OTHER);
                rPDB->calcAtomModels();

                //set name
                oFileName.sprintf( "%s_Frame_%d.pdb", QtoStd(m_oShortName).c_str(), iIndex );

                // create new document
                pNewDoc = new pdb_document(m_pSculptorWindow->getScene()->getDataTopNode(),
                                           QtoStd(oFileName), SVT_PDB,
                                           "",                SVT_NONE,
                                           rPDB);

                // set transformation as the original PDB
                svt_matrix4f* pTransformation = new svt_matrix4f;
                *pTransformation = *(m_pPdbNode->getTransformation());

                //set the own translation
                svt_vector4f oOrigin = svt_vector4f(pNewDoc->getRealPos().x(),
                                                    pNewDoc->getRealPos().y(),
                                                    pNewDoc->getRealPos().z());
                oOrigin.multScalar( 1.0E8f );
                pTransformation->preAddTranslation( oOrigin.x(),oOrigin.y(),oOrigin.z() );

                pNewDoc->getNode()->setTransformation(pTransformation);
                pNewDoc->storeInitMat();
                pNewDoc->setChanged();

                m_pSculptorWindow->addDocSimple( (sculptor_document*)pNewDoc );

                ((*m_pPdbDocument).getPointCloud()).setTimestep( iOldTimeStep );
            }
            svt_exception::ui()->progressPopdown();
        }
        break;
        case 3:// extract CA
            //disable lineedit
        {
            m_pExtractRangeLE->setEnabled(false);
        }
        //disable linedit
        case 4://
        {

            //disable lineedit
            m_pExtractRangeLE->setEnabled(false);
            //disable linedit

            if (rPDB->size()>0)
                rPDB->deleteAllAtoms();
            try
            {
                if (iSelMode==3)
                    *rPDB =  m_pPdbNode->getTrace();
                if (iSelMode==4)
                    *rPDB =  m_pPdbNode->getBackbone();

            }catch (svt_userInterrupt&)
            {
                //delete
                rPDB->deleteAllAtoms();
                return;
            }

            rPDB->calcAtomModels();


            //set name
            if ( iSelMode == 3 )
                oFileName.sprintf( "%s_Trace.pdb", QtoStd(m_oShortName).c_str() );
            else
                oFileName.sprintf( "%s_Backbone.pdb", QtoStd(m_oShortName).c_str() );

            // create new document
            pNewDoc = new pdb_document(m_pSculptorWindow->getScene()->getDataTopNode(),
                                       QtoStd(oFileName), SVT_PDB,
                                       "",                SVT_NONE,
                                       rPDB);

            // set transformation as the original PDB
            svt_matrix4f* pTransformation = new svt_matrix4f;
            *pTransformation = *(m_pPdbNode->getTransformation());

            //set the own translation
            svt_vector4f oOrigin = svt_vector4f(pNewDoc->getRealPos().x(),
                                                pNewDoc->getRealPos().y(),
                                                pNewDoc->getRealPos().z());
            oOrigin.multScalar( 1.0E8f );
            pTransformation->preAddTranslation( oOrigin.x(),oOrigin.y(),oOrigin.z() );

            pNewDoc->getNode()->setTransformation(pTransformation);
            pNewDoc->storeInitMat();
            pNewDoc->setChanged();

            m_pSculptorWindow->addDocSimple( (sculptor_document*)pNewDoc );

            svt_exception::ui()->progressPopdown();
        }
        break;
        //Secondary structures

        case 5:// Extracts secondary structures
        {
            //re-use aCAll, aC and aCN
            aC.erase (aC.begin(), aC.end());
            aCAll.erase (aCAll.begin(), aCAll.end());
            aCN.erase (aCN.begin(), aCN.end());

            //populate aC from the range specified by user
            for (unsigned int i = 0; i < expandedList.size(); i++)
            {
                if (expandedList[i].length() > 1)
                    continue;
                else
                    aC.push_back(toupper(expandedList[i].c_str()[0]));
            }

            aCAll = m_pPdbNode->getAtomSecStructIDs();

            for (unsigned int k = 0; k < aCAll.size(); k++)
            {
                if (aCAll[k] == 'C')
                    continue;
                else
                {
                    itC = std::find( aC.begin(), aC.end(), aCAll[k] );
                    if ( itC != aC.end() )
                        aCN.push_back( aCAll[k] );
                    else
                        continue;
                }

            }

            if ( !aCN.size() )
            {
                svt_exception::ui()->info("Something is wrong with the input; please check again");
                return;
            }

            else if ( aCN.size() > iWarningSize )
            {
                sprintf( pWarning,"The pdb file has %d chains. Are you sure you want to proceed with the split?", (int)aCN.size());

                if ( !svt_exception::ui()->question( pWarning ))
                    return;
            }

            for (unsigned int iIndex=0; iIndex < aCN.size(); iIndex++)
            {

                m_pPdbNode->resetAtomEnum();

                if (rPDB->size()>0)
                    rPDB->deleteAllAtoms();

                //Fetch secondary structure into rPDB
                try
                {
                    //modified
                    sc = aCN[iIndex];

                    for (unsigned int iIndexAtom=0; iIndexAtom < (*m_pPdbNode).size(); iIndexAtom++)
                    {
                        if ( sc == (m_pPdbNode->getAtom(iIndexAtom))->getSecStruct() )
                        {
                            rPDB->addAtom(*(*m_pPdbNode).getAtom(iIndexAtom), (*m_pPdbNode)[iIndexAtom]);
                        }
                        else
                            continue;
                    }

                }
                catch (svt_userInterrupt&)
                {
                    rPDB->deleteAllAtoms();
                    return;
                }

                //set name
                oFileName.sprintf( "%s_SecStr%c.pdb", QtoStd(m_oShortName).c_str(), aCN[iIndex] );

                // create new document
                pNewDoc = new pdb_document(m_pSculptorWindow->getScene()->getDataTopNode(),
                                           QtoStd(oFileName), SVT_PDB,
                                           "",                SVT_NONE,
                                           rPDB);

                //set transformation as the original PDB
                svt_matrix4f* pTransformation1 = new svt_matrix4f;
                *pTransformation1 = *(m_pPdbNode->getTransformation());

                //set the own translation
                svt_vector4f oOrigin1 = svt_vector4f(pNewDoc->getRealPos().x(),
                                                     pNewDoc->getRealPos().y(),
                                                     pNewDoc->getRealPos().z());

                oOrigin1.multScalar( 1.0E8f );
                pTransformation1->preAddTranslation( oOrigin1.x(),oOrigin1.y(),oOrigin1.z() );

                pNewDoc->getNode()->setTransformation(pTransformation1);
                pNewDoc->storeInitMat();
                pNewDoc->setChanged();

                m_pSculptorWindow->addDocSimple( (sculptor_document*)pNewDoc );

                svt_exception::ui()->progressPopdown();
            }
        }
        break;
    }

    //free memory;
    delete rPDB;
}
