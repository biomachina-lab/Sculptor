/***************************************************************************
                          sculptor_dlg_flexing.cpp
                          -----------------
    begin                : 10.15.2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_window.h>
#include <sculptor_dlg_flexing.h>
// svt includes
#include <svt_flexing_inverse_dist_weighting.h>
#include <svt_flexing_thin_plate_spline.h>
#include <svt_flexing_elastic_body_spline.h>
// qt4 includes
#include <QFileDialog>

extern sculptor_window* g_pWindow;
extern string QtoStd(QString);

/**
 * c class
 */ 
void setCoordinates(vector<svt_vector4<Real64> > &oVector, svt_point_cloud<svt_vector4<Real64> > &oPDBInitial, svt_point_cloud<svt_vector4<Real64> > &oPDBFinal, Real64 fPercent,bool bUseCos = false);

/**
 * Constructor
 */
dlg_flexing::dlg_flexing(QWidget* pParent)
    : QDialog( pParent ),
m_pDockDoc(NULL),
m_pTargetDoc(NULL)
{
    setupUi(this);

    m_pIDW->setChecked(true);
    m_pCreateIntermediateConformers->setChecked(false);

    setEnabledWeightingOptions();
    setEnableConformersCount();
    setEnabledRefmac();

    m_pAdvancedFrame->hide();
    //resize to min dimensions
    resize(0.0,0.0);
        
    connect(m_pInterpolate,             SIGNAL(clicked()),              this, SLOT( interpolate() ));
    connect(m_pAdvancedOptionsBtn,      SIGNAL(clicked()),              this, SLOT( showFlexingAdvanceOption() ));    
    connect(m_pTPS,                     SIGNAL(toggled(bool)),          this, SLOT( setEnabledWeightingOptions() ));
    connect(m_pEBS,                     SIGNAL(toggled(bool)),          this, SLOT( setEnabledWeightingOptions() ));
    connect(m_pIDW,                     SIGNAL(toggled(bool)),          this, SLOT( setEnabledWeightingOptions() ));
    connect(m_pWeightingSchema,         SIGNAL(activated(int)),         this, SLOT( setEnabledWeightingOptions() ));
    connect(m_pCreateIntermediateConformers, SIGNAL(toggled(bool)),     this, SLOT( setEnableConformersCount() ));
    connect(m_pShowDispl,               SIGNAL(clicked()),              this, SLOT( renderDisplacement() ));
    connect(m_pRunRefmac,               SIGNAL(clicked()),              this, SLOT( setEnabledRefmac() ));
    connect(m_pBrowse,                  SIGNAL(clicked()),              this, SLOT( chooseRefmacPath() ));
}

/**
 * Destructor
 */
dlg_flexing::~dlg_flexing()
{
}

/**
 * set documents
 */ 
void dlg_flexing::setDocs( sculptor_document * pDockDoc, sculptor_document *pTargetDoc )
{
    m_pDockDoc      = pDockDoc;   
    m_pTargetDoc    = pTargetDoc; 

    strcpy(m_pDockDocFName, m_pDockDoc->getNameShort().c_str() );
    strcpy(m_pTargetDocFName,  m_pTargetDoc->getNameShort().c_str() );
  
    checkDocs();
    updateUi();
};
    
/**
 * get the doc document
 */
sculptor_document* dlg_flexing::getDockDoc()
{
    return m_pDockDoc;
};

/**
 * get the target document
 */
sculptor_document* dlg_flexing::getTargetDoc()
{
    return m_pTargetDoc;
};

/**
 * show/modify option based on selection
 */ 
void dlg_flexing::updateUi()
{
   //set recommendata value for R (number of feature vectors to consider in the flexing)
    m_pR->setMaximum(m_pDockDoc->getCodebook().size());
    m_pR->setValue((int)(m_pDockDoc->getCodebook().size()*0.9));
    
    Real64 fCvRMSD = g_pWindow->computeCvRMSD(*m_pDockDoc, *m_pTargetDoc);
    char pRMSD[1256];
    sprintf(pRMSD,"RMSD: %5.3f", fCvRMSD);
    m_pRMSD->setText( pRMSD ) ;
    

    if ( fCvRMSD>10.0f )
    {
        QPalette oPalette;
        oPalette.setColor( m_pRMSD->foregroundRole(), QColor(Qt::red) );
        m_pRMSD->setPalette(oPalette);
    }

    // change the caption of the dialog box to show the names of the two files involved in the flexing
    QString oCaption;
    oCaption.sprintf("Flexing %s to %s", m_pDockDocFName, m_pTargetDocFName);
    setWindowTitle(oCaption);
};

/**
 * checks the validity of the Dock/TargetDocuments
 */
void dlg_flexing::checkDocs()
{
    if (m_pDockDoc==NULL || m_pTargetDoc==NULL)
    	return;
    
    if ((m_pDockDoc->getCodebook()).size()==0)
    {
        svt_exception::ui()->info("Please compute the feature points of the probe molecule!");
	return;
    }
    
    if ((m_pTargetDoc->getCodebook()).size()==0)
    {
        if (m_pTargetDoc->getType()==SENSITUS_DOC_SITUS)
           svt_exception::ui()->info("Please compute the feature points of the target map!");
        else if (m_pTargetDoc->getType()==SENSITUS_DOC_PDB)
	   svt_exception::ui()->info("Please compute the feature points of the second probe molecule!");
        else 
            svt_exception::ui()->info("Please compute the feature points of the second !");
	return;
    }
    
    bool bUseMatching=false;
    
    if (m_pDockDoc->getTransformCount()>0)
    {
        vector<int> pMatch = *(m_pDockDoc->getCurrentMatch());

        long int iMatchTargetID = m_pDockDoc->getCurrentMatchTargetID();
        long int iMatchProbeID  = m_pDockDoc->getCurrentMatchProbeID();
        long int iThisTargetID  = m_pTargetDoc->getCodebookID();
        long int iThisProbeID   = m_pDockDoc->getCodebookID();

        if (pMatch.size()>0 && iMatchTargetID == iThisTargetID && iMatchProbeID == iThisProbeID)
            bUseMatching = true;    //the matching will be used
        else
            bUseMatching = false;
    }else
        bUseMatching = false;

    
    if (!bUseMatching && (m_pDockDoc->getCodebook()).size()!=(m_pTargetDoc->getCodebook()).size())
        svt_exception::ui()->info("The probe and target have different numbers of feature vectors! Please check \nthe feature vectors as they might cause faulty results in the flexing.");
}

/**
 * launch the interpolation
 */
void dlg_flexing::interpolate()
{	
    try
    {
	char pMethodName[4];
	char pFname[1256];
	unsigned int iConformersCount;
	unsigned int iIndexAtom;
	svt_point_cloud_pdb< svt_vector4<Real64> > oCV_DockDoc;
	svt_point_cloud_pdb< svt_vector4<Real64> > oCV_TargetDoc;
        svt_point_cloud_pdb< svt_vector4<Real64> > oCV_Tmp;
	svt_point_cloud_pdb<svt_vector4<Real64> > oInterpolatedConformer;
	svt_point_cloud_atom *oAtom;
	svt_vector4<Real64> oVec;
	svt_vector4f oPos; 
	
	unsigned int iAtomCount = (((pdb_document*)m_pDockDoc)->getPointCloud()).size();
	svt_vector4f oOrigin = g_pWindow->getOrigin(); 
	//copy original structure 
	for(iIndexAtom = 0; iIndexAtom < iAtomCount; iIndexAtom++)
	{
	    oPos = ((pdb_document*)m_pDockDoc)->getRealPos(iIndexAtom, oOrigin); 
	    oVec.x(oPos.x());
	    oVec.y(oPos.y());
	    oVec.z(oPos.z());
	    
	    oAtom = ((((pdb_document*)m_pDockDoc)->getPointCloud()).getAtom(iIndexAtom));
	    
	    oInterpolatedConformer.addAtom( *oAtom, oVec );

	}

	m_pDockDoc->getCodebookRealPos(oCV_DockDoc, oOrigin);
	m_pTargetDoc->getCodebookRealPos(oCV_Tmp, oOrigin);
        
        vector<int> pCV2del;
        unsigned int iCVCount = oCV_DockDoc.size();
        unsigned int iCVCountTarget = oCV_Tmp.size();
        
        if (((pdb_document*)m_pDockDoc)->getTransformCount() > 0)
        {
            vector<int> pMatch = *(((pdb_document*)m_pDockDoc)->getCurrentMatch());

            long int iMatchTargetID = ((pdb_document*)m_pDockDoc)->getCurrentMatchTargetID();
            long int iMatchProbeID  = ((pdb_document*)m_pDockDoc)->getCurrentMatchProbeID();
            long int iThisTargetID  = m_pTargetDoc->getCodebookID();
            long int iThisProbeID  = m_pDockDoc->getCodebookID();

            //printf("DEBUG: IDs: (%li,%li) = (%li,%li)\n", iMatchProbeID, iMatchTargetID, iThisProbeID, iThisTargetID );

            if (pMatch.size()>0 && iMatchTargetID == iThisTargetID && iMatchProbeID == iThisProbeID)
            {
                for (unsigned int iIndexCV = 0; iIndexCV < iCVCount; iIndexCV++)
                {
                    if (pMatch[iIndexCV] > (int)oCV_Tmp.size())
                    {
                        // delete the point that does not match
                        //Can't delete here the non matching features cause the indices will be mixed 
                        pCV2del.push_back(iIndexCV);
                    }
                    else
                        oCV_TargetDoc.addPoint( oCV_Tmp[ pMatch[iIndexCV] ] );
                }
            }
            else
            {
                for (unsigned int iIndexCV = 0; iIndexCV < iCVCountTarget; iIndexCV++)
                    oCV_TargetDoc.addPoint( oCV_Tmp[ iIndexCV ] );
            }
        }
        else
        {
            for (unsigned int iIndexCV = 0; iIndexCV < iCVCountTarget; iIndexCV++)
                    oCV_TargetDoc.addPoint( oCV_Tmp[ iIndexCV ] );
        }
        
        //delete here the feature that have no match
        for (int iIndex = pCV2del.size()-1; iIndex >= 0; iIndex--)
        {
            svtout << "Feature " << pCV2del[iIndex] << " could not be match with a feature in the target map and will not be used for the flexing." << endl;
            oCV_DockDoc.delPoint( pCV2del[iIndex] );
        }
        
	
        if (oCV_DockDoc.size()!=oCV_TargetDoc.size())//if the number of feature vectors still differs  between the two documents, then remove the extra:
	{
             unsigned int iCVCountProbe = oCV_DockDoc.size();
             unsigned int iCVCountTarget = oCV_TargetDoc.size();
            
            if (iCVCountProbe>iCVCountTarget)
            {
                for (unsigned int iIndexCV = iCVCountProbe-1; iIndexCV >= iCVCountTarget; iIndexCV--)
                    oCV_DockDoc.delPoint(iIndexCV);  
            }else // iCVCountProbe < iCVCountTarget
            {
                for (unsigned int iIndexCV = iCVCountTarget-1; iIndexCV >= iCVCountProbe; iIndexCV--)
                    oCV_TargetDoc.delPoint(iIndexCV);
            }
	}
	
	if (m_pCreateIntermediateConformers->isChecked())
	    iConformersCount = m_pConformersCount->value();
	else
	    iConformersCount = 1;
	
	svtout << endl;
	svt_flexing *oInterpol = NULL;
	svt_point_cloud_pdb<svt_vector4<Real64> > oConformers;
        bool bFlexAllAtoms = false;
	
	if (m_pTPS->isChecked())
	{
	    oInterpol = new svt_flexing_thin_plate_spline( oInterpolatedConformer, oCV_DockDoc, oCV_TargetDoc, bFlexAllAtoms, "Cartesian","none", m_pKernel->currentIndex()+1);
	    
	    svtout << "    Kernel:"            << QtoStd(m_pKernel->currentText()).c_str() << endl;
	    sprintf(pMethodName, "TPS");
	}
	
	if (m_pEBS->isChecked())
	{
            oInterpol = new svt_flexing_elastic_body_spline( oInterpolatedConformer, oCV_DockDoc, oCV_TargetDoc, bFlexAllAtoms, "none", "none", m_pKernel->currentIndex()+1, m_pElasticity->value()/100.0);
	    
	    svtout << "    Elasticity: "        << m_pElasticity->value()/100.0 << endl;
	    svtout << "    Kernel:     "        << QtoStd(m_pKernel->currentText()).c_str() << endl;
	    sprintf(pMethodName, "EBS");
	}
	
	if (m_pIDW->isChecked())
	{
           QString pWeightingPower = m_pWeightingPow->text();
           Real64 fWeightingPower;
           
           if (!pWeightingPower.isEmpty())
               fWeightingPower = pWeightingPower.toDouble();
           else
           {
               svt_exception::ui()->info("The interpolation was not successful. Please select a value for the weighting power!");
               return;
           }
           
	   oInterpol = new svt_flexing_inverse_dist_weighting( oInterpolatedConformer ,oCV_DockDoc, oCV_TargetDoc, bFlexAllAtoms, 
	                                                       "Cartesian", "none", QtoStd(m_pWeightingSchema->currentText()).c_str(),
                                                               fWeightingPower);

           svtout << "    Weighting Schema:"  << QtoStd(m_pWeightingSchema->currentText()).c_str() << endl;
	    
           if (m_pIDW->isChecked() && m_pWeightingSchema->currentIndex()==1 &&  m_pR->isEnabled())
           {
               ((svt_flexing_inverse_dist_weighting*)oInterpol)->setNbCdbksOfInfluence(m_pR->value());
                svtout << "    Features of Influence:"  << m_pR->value() << endl;
           }
                
           svtout << "    Weighting Power: "  << fWeightingPower << endl;
           sprintf(pMethodName, "IDW");
	}
	
	svtout << "    Interpolation method: " << pMethodName << endl;
	svtout << endl;
        
	//interpolate
	if (iConformersCount>1)
	{
            svt_exception::ui()->progressPopup("Building conformers ...", 0);
	    
	    vector<svt_vector4<Real64> > oVecInitialConfCodebook;
	    vector<svt_vector4<Real64> > oVecFinalConfCodebook;
	    
	    // one desires to have iConformersCount in total; thus need to substact the one corresponding to the original conformation
	    Real64 fStepPer100 = 1.0/(iConformersCount-1);
	    unsigned int iIndexAtom;
	    
	    //set the first conformer to the original structure
	    for ( iIndexAtom = 0; iIndexAtom < iAtomCount ; iIndexAtom++)
	    {
		oAtom = ((((pdb_document*)m_pDockDoc)->getPointCloud()).getAtom(iIndexAtom));
	 	oConformers.addAtom(*oAtom, oInterpolatedConformer[iIndexAtom]);
	    }
	    
	    setCoordinates(oVecFinalConfCodebook, oCV_DockDoc, oCV_TargetDoc, 0, true);
            
            
	    for (unsigned int iIndex=1; iIndex < iConformersCount; iIndex++)
	    {
                try
                {
                    int iPerc = (int)((((float)( iIndex ) / (float)( iConformersCount )) * 100.0));
                        svt_exception::ui()->progress( iPerc);
                    //Step 1: get the initial and final coordinates of the codebook
                    oVecInitialConfCodebook = oVecFinalConfCodebook;
                    setCoordinates(oVecFinalConfCodebook, oCV_DockDoc, oCV_TargetDoc, fStepPer100*iIndex, true);
                    
                    //Step2: set the new computed codebook.
                    oInterpol->setCodebooks(oVecInitialConfCodebook, oVecFinalConfCodebook);
                    
                    //Step3: Interpolate
                    oInterpolatedConformer  = oInterpol->interpolate();
                    
                    oConformers.addTimestep();
                    for ( iIndexAtom = 0; iIndexAtom < iAtomCount; iIndexAtom++)
                        oConformers.setPoint(iIndexAtom, oInterpolatedConformer[iIndexAtom]);
                    
                    oInterpol->setOrigConf(oInterpolatedConformer);
                }
                catch (svt_userInterrupt&)
                {
                    //delete 
                    oConformers.deleteAllAtoms();
		    if (oInterpol != NULL)
		        delete oInterpol;
                    
                    svt_exception::ui()->progressPopdown();
                    return;
                }   

	    }
	    
	    oConformers.setTimestep(0);
	    sprintf(pFname, "flex_%s_to_%s_by_%s_%d.pdb", m_pDockDocFName, m_pTargetDocFName, pMethodName,iConformersCount);
            svt_exception::ui()->progressPopdown();
	}
	else
	{
            // just one conformer = the intepolated one
	    oConformers  = oInterpol->interpolate(); 
	    sprintf(pFname, "flex_%s_to_%s_by_%s.pdb", m_pDockDocFName, m_pTargetDocFName, pMethodName);
	}
        
        if ( m_pRunRefmac->isChecked() )
        {
            //check if the selected refmac file exists
            QString oFile;
            oFile = m_pRefmacPath->text();
	    svtout << QtoStd(oFile).c_str() << endl;
            
            if ( !QFileInfo(oFile).exists() )
            {
               svt_exception::ui()->info("The selected refmac file does not exist! \nThe flexed structure did not undergo a molecular refinement by refmac. "); 
            }else
            {
		svt_point_cloud_pdb< svt_vector4<Real64> > oConformer;
		unsigned int iSize = oConformers.size();
		for (unsigned int iIndex=0; iIndex < iConformersCount; iIndex++)
		{
		    oConformer.deleteAllAtoms();
		    oConformers.setTimestep( iIndex );
		
		    for (unsigned int iIndexAtom=0; iIndexAtom<iSize; iIndexAtom++)
		    {
			oAtom = oConformers.getAtom(iIndexAtom);
			oConformer.addAtom(*oAtom, oConformers[iIndexAtom]);
		    }
		    
		    try
		    {

			//write pdb file on the disk			
		
			//crystal parameters 
			Real64 fXExpand = oConformer.getMaxXCoord()-oConformer.getMinXCoord();
			Real64 fYExpand = oConformer.getMaxYCoord()-oConformer.getMinYCoord();
			Real64 fZExpand = oConformer.getMaxZCoord()-oConformer.getMinZCoord();
		    
			char oCrystalParamString[1256];
			sprintf(oCrystalParamString,"CRYST1%9.3f%9.3f%9.3f  90.00  90.00  90.00 P 1					\nSCALE1      %8.6f  0.000000  0.000000        0.00000                         \nSCALE2      0.000000  %8.6f  0.000000        0.00000                         \nSCALE3      0.000000  0.000000  %8.6f        0.00000                         \n", fXExpand, fYExpand, fZExpand, 1/fXExpand,  1/fYExpand, 1/fZExpand);

			
			//write file on disk for refmac to optimize
			char pFname_refmac_in[1256];
			strcpy(pFname_refmac_in, "tmpstruct.pdb");
			
			FILE* pFile = fopen(pFname_refmac_in, "w");
			fprintf(pFile, oCrystalParamString);
			fclose(pFile);
			oConformer.writePDB(pFname_refmac_in, true, false, true); // only write the first frame 

			//create refmac script
			char pFname_refmac_out[1256];
			sprintf(pFname_refmac_out,"tmpstruct_ref.pdb");
			
			char pRefmacRunFN[1256];
			sprintf(pRefmacRunFN,"./refmac.run");
			
			char pCmdLn[1256];
			pFile = fopen(pRefmacRunFN,"w");
			fprintf(pFile,"#!/bin/csh -f\n");
			sprintf(pCmdLn,"set crdin=%s\nset crdout=%s\n", pFname_refmac_in, pFname_refmac_out);
			fprintf(pFile,pCmdLn);
			sprintf(pCmdLn,"refmac:\n%s \\\nXYZIN   $crdin \\\nXYZOUT $crdout \\\n<< eor\n", QtoStd(oFile).c_str() );
			fprintf(pFile,pCmdLn);
			fprintf(pFile," MAKE HYDR Y\nMAKE CHECK NONE\nREFI TYPE IDEAlise\nNCYC 2\nMONI MEDI\nend\neor\nif ($status) exit\n");
			fclose(pFile);
			
			char pFname_refmac_log[1256];
			sprintf(pFname_refmac_log,"refmac.log");
			
			//run refmac 
			sprintf(pCmdLn,"chmod 755 %s; %s > %s", pRefmacRunFN, pRefmacRunFN, pFname_refmac_log );
			
			svtout << "Running Refmac" << endl;
			//please wait
			svt_exception::ui()->busyPopup("Please wait for Refmac molecular refinement!");
			svt_system(pCmdLn);
			svt_exception::ui()->busyPopdown();
			
			// read in the output
			pFile = fopen(pFname_refmac_out,"r");
			if (pFile == NULL)
			{
                            char pInfo[1024];
                            sprintf(pInfo," Unsuccessful Refmac run!  The flexed structure did not undergo a molecular refinement by refmac.\n Please see the files \"%s\" or  \"%s\"  for more details. ", pRefmacRunFN, pFname_refmac_log );

			    svt_exception::ui()->info(pInfo);
			}
			else
			{
			    fclose(pFile);
			    
			    oConformer.deleteAllAtoms();
			    oConformer.loadPDB( pFname_refmac_out );
			    
			    //erase temporary files
			    remove(pFname_refmac_out);

                            //erase temporary files
                            remove(pFname_refmac_in);
                            remove(pRefmacRunFN);
                            remove(pFname_refmac_log);
                            }
		    }
		    catch(svt_exception oError)
                    {
			oError.uiMessage();
                    }
		    
		    for (unsigned int iIndexAtom=0; iIndexAtom<iSize; iIndexAtom++)
		    	oConformers[iIndexAtom] =  oConformer[iIndexAtom];
		}
            }
        }
        
	oConformers.setSecStructSource( (((pdb_document*)m_pDockDoc)->getPointCloud()).getSecStructSource() );
	g_pWindow->createDocForFlexedStruct(pFname, &oConformers, (iConformersCount>1));

	if (oInterpol != NULL)
	    delete oInterpol;
    }
    catch(svt_exception oError)
    {
	oError.uiMessage();
    }
};

/**
 *Shows the advanced option for the Flexing
 */
void dlg_flexing::showFlexingAdvanceOption()
{

    if(m_pAdvancedFrame->isVisible())
    {
	m_pAdvancedOptionsBtn->setText("Options >>");
        m_pAdvancedFrame->hide();
    }
    else
    {
	m_pAdvancedOptionsBtn->setText("Options <<");
	m_pAdvancedFrame->show();
    }
    
    //resize to min dimensions
    resize(0.0,0.0);

    // update the layout - it controls the resizing 
    layout()->update();
}

/**
 * enable/disable create conformers count  
 */
void dlg_flexing::setEnableConformersCount()
{
    m_pConformersCount->setEnabled(m_pCreateIntermediateConformers->isChecked());
}

/**
 * Disable/Enables the IDW inteepolation related options
 */
void dlg_flexing::setEnabledWeightingOptions()
{
    bool bOption;
    if (m_pIDW->isChecked())
    {
	bOption = true;

	m_pElasticityLabel->setEnabled(!bOption);
	m_pElasticity->setEnabled(!bOption);
    }
    else
    {
	bOption = false;

	m_pKernel->clear();
	if (m_pTPS->isChecked())
	{
	    m_pKernel->insertItem(0,"u(|x|) = |x|");
	    //m_pKernel->insertItem("u(|x|) = |x|^2*log(|x|^2)",1);
	    //m_pKernel->insertItem("u(|x|) = |x|^3",2);
	    m_pKernel->setCurrentIndex(0);
	    
	    m_pElasticityLabel->setEnabled(bOption);
	    m_pElasticity->setEnabled(bOption);
	}
	else
	{//EBS is selected
	    m_pKernel->addItem( QString("G(x) = (c*|x|^2-3*x*x^T)*|x|") );
	    m_pKernel->addItem( QString("G(x) = c*|x|-x*x^T/|x|") );
	    m_pKernel->setCurrentIndex(1);
	    
	    m_pElasticityLabel->setEnabled(!bOption);
	    m_pElasticity->setEnabled(!bOption);
	}
    }
	
    //m_pWeightingPow
    m_pWeightingPowLabel->setEnabled(bOption);
    m_pWeightingPow->setEnabled(bOption);
    
    
    // m_pWeightingSchema
    m_pWeightingSchema->setEnabled(bOption);
    m_pWeightingSchemaLabel->setEnabled(bOption);
    
    //m_pFearture of influence
    if (m_pIDW->isChecked() && m_pWeightingSchema->isEnabled() && m_pWeightingSchema->currentIndex()==1)
    {
        m_pRadiusInfluenceLabel->setEnabled(true);
        m_pR->setEnabled(true);
    }
    else
    {
        m_pRadiusInfluenceLabel->setEnabled(false);
        m_pR->setEnabled(false);
    }
    
    //kernel
    m_pKernelLabel->setEnabled(!bOption);
    m_pKernel->setEnabled(!bOption);
}

/**
 * enables the selection of the Refmac path
 */
void dlg_flexing::setEnabledRefmac()
{
    m_pLocation->setEnabled(m_pRunRefmac->isChecked());
    m_pRefmacPath->setEnabled(m_pRunRefmac->isChecked());
    m_pBrowse->setEnabled(m_pRunRefmac->isChecked());
};

/**
 * choose refmac path
 */
void dlg_flexing::chooseRefmacPath()
{
    // open file chooser dialog
    QFileDialog oFD(this);

    oFD.setFileMode( QFileDialog::ExistingFiles );
    oFD.setViewMode( QFileDialog::List );
    oFD.setDirectory( QString( g_pWindow->getCurrentDir().c_str()) );

    QString oFileA;
    if ( oFD.exec() == QDialog::Accepted )
    {
        QStringList oFiles = oFD.selectedFiles();
	if (!oFiles.isEmpty())
	    oFileA = oFiles[0];

    } //else
        //g_pWindow->statusBar()->showMessage( "Loading aborted", 2000 );
       
    
    if (this!=NULL)
    {
        m_pRefmacPath->setText( oFileA );
        raise();
    }
};

/**
 * Extract the xyz coordinates from the codebook containing the pdb file  
 */
void setCoordinates(vector<svt_vector4<Real64> > &oVector, svt_point_cloud<svt_vector4<Real64> > &oPDBInitial, svt_point_cloud<svt_vector4<Real64> > &oPDBFinal, Real64 fPercent,bool bUseCos)
{
    unsigned int iIndex;
    Real64 fCoeff;
    
    oVector.clear();
    if (bUseCos) 
	fCoeff = (1-cos(fPercent*PI))/2.0;
     else 
	fCoeff = fPercent; 
    
    for (iIndex = 0; iIndex<oPDBInitial.size(); iIndex++)
	oVector.push_back(oPDBInitial[iIndex]*(1 - fCoeff)  + oPDBFinal[iIndex]*fCoeff);
}

/**
 * Render Displacement
 */
void dlg_flexing::renderDisplacement()
{
    g_pWindow->renderDisplacement( true );
};
     
/**
 * Set state for ShowDisplacement
 */  
void dlg_flexing::setShowDisp( bool bShowDisp )
{
    m_pShowDispl->setChecked( bShowDisp );
};

/**
 * get state for ShowDisplacement
 */  
bool dlg_flexing::getShowDisp()
{
    return m_pShowDispl->isChecked();
};



