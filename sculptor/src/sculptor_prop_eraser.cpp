/***************************************************************************
                          sculptor_prop_eraser.cpp  -  description
                             -------------------
    begin                : 17.09.2010
    author               : Francisco Serna
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////


// sculptor includes
#include <sculptor_prop_eraser.h>
#include <sculptor_doc_eraser.h>


/**
 *
 */
prop_eraser::prop_eraser(sculptor_doc_eraser* pEraserDoc, QWidget* pParent ) 
    : QWidget( pParent ),
      m_pEraserDoc( pEraserDoc )

{
    setupUi(this);   
  
    //Assign button group for easy usage of button group slot and enums from sculptor_doc_eraser
    m_pEraserShapeButtonGroup->setId(m_pCubeTool,    sculptor_doc_eraser::SCULPTOR_ERASER_TOOL_CUBE);
    m_pEraserShapeButtonGroup->setId(m_pSphereTool,  sculptor_doc_eraser::SCULPTOR_ERASER_TOOL_SPHERE);
    /* Removing the copy volume functionality for now..
    m_pEraserTargetButtonGroup->setId(m_pModify,     sculptor_doc_eraser::SCULPTOR_ERASER_TOOL_METHOD_MODIFY);
    m_pEraserTargetButtonGroup->setId(m_pCopyModify, sculptor_doc_eraser::SCULPTOR_ERASER_TOOL_METHOD_COPYMODIFY); */
    
    // update the ui elements from the values set in m_pEraserDoc - defaults should be stored in 
    // the sculptor_doc_eraser, and when this property dialog is
    // instantiated, it should query all values from that doc and set the ui

    updateEraserShape();
    updateInvertCheckBox();
    updateEraserSize();
    updateEraserOpacity();
 

    connect(m_pEraserShapeButtonGroup, 	SIGNAL(buttonClicked(int)), 	m_pEraserDoc, SLOT(sSetEraserShape(int)));
  //  connect(m_pEraserTargetButtonGroup, SIGNAL(buttonClicked(int)),     m_pEraserDoc, SLOT(sSetEraserMethod(int)));
    connect(m_pEraserOpacitySlider, 	SIGNAL(valueChanged(int)), 	m_pEraserDoc, SLOT(sSetOpacity(int)));
    connect(m_pEraserSizeSlider, 	SIGNAL(valueChanged(int)), 	m_pEraserDoc, SLOT(sSetEraserSize(int)));
    connect(m_pEraserInvertCheckBox, 	SIGNAL(toggled(bool)), 		m_pEraserDoc, SLOT(sSetInverted(bool)));
    connect(m_pErase, 			SIGNAL(pressed()), 		m_pEraserDoc, SLOT(sEraseRegion()));

}

/**
 * 
 */
prop_eraser::~prop_eraser()
{
}

/**
 *
 */
void prop_eraser::updateEraserShape()
{
    switch ( m_pEraserDoc->getEraserShape() )
    {
	case sculptor_doc_eraser::SCULPTOR_ERASER_TOOL_CUBE:
	    // block signals or that thing will emit signals when you set it to some value
	    m_pCubeTool->blockSignals( TRUE );
	    m_pCubeTool->setChecked( TRUE );
	    m_pCubeTool->blockSignals( FALSE );
	    break;

	case sculptor_doc_eraser::SCULPTOR_ERASER_TOOL_SPHERE:
	    m_pSphereTool->blockSignals( TRUE );
	    m_pSphereTool->setChecked( TRUE );
	    m_pSphereTool->blockSignals( FALSE );
	    break;
    }
}

/**
 *
 */
void prop_eraser::updateEraserSize()
{
    m_pEraserSizeSlider->blockSignals( TRUE );
    EraserSizeSpinBox->setValue(m_pEraserDoc->getEraserSize());
    m_pEraserSizeSlider->blockSignals( FALSE );    
}

/**
 *
 */
void prop_eraser::updateInvertCheckBox()
{
    m_pEraserInvertCheckBox->blockSignals( TRUE );
    m_pEraserInvertCheckBox->setChecked(m_pEraserDoc->getInvertSetting());
    m_pEraserInvertCheckBox->blockSignals( FALSE );
}

/**
 *
 */
void prop_eraser::updateEraserOpacity()
{
    m_pEraserOpacitySlider->blockSignals( TRUE );
    EraserOpacitySpinBox->setValue(m_pEraserDoc->getEraserOpacityGUI());
    m_pEraserOpacitySlider->blockSignals( FALSE );
}
