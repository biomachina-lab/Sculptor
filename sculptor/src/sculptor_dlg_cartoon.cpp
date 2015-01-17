/***************************************************************************
                          sculptor_dlg_cartoon.cpp
                          -------------------
    begin                : 10/06/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_dlg_cartoon.h>
#include <sculptor_dlg_colmapsecstruct.h>
#include <sculptor_dlg_newtube.h>
#include <svt_opengl.h>
#include <pdb_document.h>
// svt includes
#include <svt_pdb.h>


/*
 * Constructor
 */
dlg_cartoon::dlg_cartoon(QWidget* pParent, pdb_document* pPdbDocument)
    : QDialog( pParent ),
      m_pPdbNode( (svt_pdb*)(pPdbDocument->getNode()) ),
      m_pPdbDocument( pPdbDocument ),
      m_pMyColmapDlgSecStruct( NULL ),
      m_pMyNewTube_Options( NULL )
{
    setupUi(this);

#ifdef EXPERIMENTAL
    m_pShapeRoundedRectangular->setEnabled( FALSE );
#else
    m_pRibbonShape->removeButton( m_pShapeRoundedRectangular );
#endif

    // fill in values
    updateMinRes();
    updateTubeProfileCorners();
    updateUseShader();

    m_pSpinBoxSegments         ->setValue( (int)m_pPdbNode->getCartoonParam(CARTOON_SPLINE_SEGMENTS) );
    m_pCartoonSheetWidth->  setValue( (double)(m_pPdbNode->getCartoonParam(CARTOON_SHEET_WIDTH)) );
    m_pCartoonSheetHeight-> setValue( (double)(m_pPdbNode->getCartoonParam(CARTOON_SHEET_HEIGHT)) );
    m_pCartoonHelixWidth->  setValue( (double)(m_pPdbNode->getCartoonParam(CARTOON_HELIX_WIDTH)) );
    m_pCartoonHelixHeight-> setValue( (double)(m_pPdbNode->getCartoonParam(CARTOON_HELIX_HEIGHT)) );
    m_pCartoonTubeDiameter->setValue( (double)(m_pPdbNode->getCartoonParam(CARTOON_TUBE_DIAMETER)) );
    m_pSpinBoxSheetHelixCorners->setValue( max(8, (int)m_pPdbNode->getCartoonParam(CARTOON_SHEET_HELIX_PROFILE_CORNERS)) );

    if ( m_pPdbNode->getCartoonParam(CARTOON_PEPTIDE_PLANES) == 1.0f )
        m_pCheckBoxPeptidePlanes->setChecked( TRUE );

    if ( m_pPdbNode->getCartoonParam(CARTOON_HELIX_ARROWHEADS) == 1.0f )
        m_pCheckBoxHelixArrowHeads->setChecked( TRUE );

    if ( m_pPdbNode->getCartoonParam(CARTOON_HELIX_CYLINDER) == 1.0f )
        m_pCheckBoxHelixCylinder->setChecked( TRUE );

    switch ( (int)m_pPdbNode->getCartoonParam(CARTOON_SHEET_HELIX_PROFILE_CORNERS) )
    {
        case 2:
            m_pCartoonHelixHeight   ->setDisabled( TRUE );
            m_pCartoonSheetHeight   ->setDisabled( TRUE );
            m_pSpinBoxSheetHelixCorners->setDisabled( TRUE );
            m_pShapeFlat->setChecked( TRUE );
            break;

        case 4:
            m_pSpinBoxSheetHelixCorners->setDisabled( TRUE );
            m_pShapeRectangular->setChecked( TRUE );
            break;

        default:
            m_pShapeElliptical->setChecked( TRUE );
            break;
    }

    // signal and slot connections
    connect( m_pCartoonTubeDiameter,    SIGNAL(editingFinished()),    this, SLOT(setCartoonTubeDiameter()) );
    connect( m_pCartoonTubeDiameter,    SIGNAL(valueChanged(double)), this, SLOT(setCartoonTubeDiameter(double)) );
    connect( m_pCartoonHelixWidth,      SIGNAL(editingFinished()),    this, SLOT(setCartoonHelixWidth()) );
    connect( m_pCartoonHelixWidth,      SIGNAL(valueChanged(double)), this, SLOT(setCartoonHelixWidth()) );
    connect( m_pCartoonSheetHeight,     SIGNAL(editingFinished()),    this, SLOT(setCartoonSheetHeight()) );
    connect( m_pCartoonSheetHeight,     SIGNAL(valueChanged(double)), this, SLOT(setCartoonSheetHeight()) );
    connect( m_pCartoonSheetWidth,      SIGNAL(editingFinished()),    this, SLOT(setCartoonSheetWidth()) );
    connect( m_pCartoonSheetWidth,      SIGNAL(valueChanged(double)), this, SLOT(setCartoonSheetWidth()) );
    connect( m_pCartoonHelixHeight,     SIGNAL(editingFinished()),    this, SLOT(setCartoonHelixHeight()) );
    connect( m_pCartoonHelixHeight,     SIGNAL(valueChanged(double)), this, SLOT(setCartoonHelixHeight()) );

    connect( m_pSpinBoxHelixRes,           SIGNAL(editingFinished()),    this, SLOT(setMinResPerHelix()) );
    connect( m_pSpinBoxHelixRes,           SIGNAL(valueChanged(int)),    this, SLOT(setMinResPerHelix()) );
    connect( m_pSpinBoxSegments,           SIGNAL(editingFinished()),    this, SLOT(setCartoonSegments()) );
    connect( m_pSpinBoxSegments,           SIGNAL(valueChanged(int)),    this, SLOT(setCartoonSegments()) );
    connect( m_pSpinBoxSheetRes,           SIGNAL(editingFinished()),    this, SLOT(setMinResPerSheet()) );
    connect( m_pSpinBoxSheetRes,           SIGNAL(valueChanged(int)),    this, SLOT(setMinResPerSheet()) );
    connect( m_pSpinBoxSheetHelixCorners,  SIGNAL(editingFinished()),    this, SLOT(setCartoonSheetHelixProfileCorners()) );
    connect( m_pSpinBoxSheetHelixCorners,  SIGNAL(valueChanged(int)),    this, SLOT(setCartoonSheetHelixProfileCorners()) );
    connect( m_pSpinBoxTubeCorners,        SIGNAL(editingFinished()),    this, SLOT(setNewTubeProfileCorners()) );
    connect( m_pSpinBoxTubeCorners,        SIGNAL(valueChanged(int)),    this, SLOT(setNewTubeProfileCorners()) );

    connect( m_pCheckBoxPeptidePlanes,     SIGNAL(toggled(bool)),        this, SLOT(toggleCartoonPeptidePlanes()) );
    connect( m_pCheckBoxHelixCylinder,     SIGNAL(toggled(bool)),        this, SLOT(toggleCartoonHelixCylinder()) );
    connect( m_pCheckBoxShader,            SIGNAL(toggled(bool)),        this, SLOT(toggleCartoonShader()) );
    connect( m_pCheckBoxPerPixelLighting,  SIGNAL(toggled(bool)),        this, SLOT(togglePerPixelLighting()) );
    connect( m_pCheckBoxHelixArrowHeads,   SIGNAL(toggled(bool)),        this, SLOT(toggleCartoonHelixArrowheads()) );
    connect( m_pButtonClose,               SIGNAL(clicked()),            this, SLOT(hide()) );
    connect( m_pRibbonShape,               SIGNAL(buttonClicked(int)),   this, SLOT(setCartoonSheetHelixProfileCorners(int)) );
}

/*
 * Destructor
 */
dlg_cartoon::~dlg_cartoon()
{
}

/**
 *
 */
void dlg_cartoon::setMinResPerSheet()
{
    // the min residues per sheet parameter has to be set twice:
    // once in svt_pdb, where the geometry is computed, and
    // once in svt_atom_strucutre_colmap, where the color for the geometry is computed
    m_pPdbNode->setCartoonParam( CARTOON_SHEET_MIN_RESIDUES, ( (float)m_pSpinBoxSheetRes->value()) );
    m_pPdbDocument->getAtomStructureColmap()->setMinResiduesPerSheet( (float)m_pSpinBoxSheetRes->value() );

    if ( m_pMyColmapDlgSecStruct != NULL )
        m_pMyColmapDlgSecStruct->updateMinRes();

}

/**
 *
 */
void dlg_cartoon::setMinResPerHelix()
{
    // the min residues per helix parameter has to be set twice:
    // once in svt_pdb, where the geometry is computed, and
    // once in svt_atom_strucutre_colmap, where the color for the geometry is computed
    m_pPdbNode->setCartoonParam( CARTOON_HELIX_MIN_RESIDUES, ( (float)m_pSpinBoxHelixRes->value()) );
    m_pPdbDocument->getAtomStructureColmap()->setMinResiduesPerHelix( (float)m_pSpinBoxHelixRes->value() );

    if ( m_pMyColmapDlgSecStruct != NULL )
        m_pMyColmapDlgSecStruct->updateMinRes();

}

/**
 *
 */
void dlg_cartoon::togglePerPixelLighting()
{
    m_pPdbNode->setCartoonParam( CARTOON_PER_PIXEL_LIGHTING, (1.0f - m_pPdbNode->getCartoonParam(CARTOON_PER_PIXEL_LIGHTING)) );

    if ( m_pMyNewTube_Options != NULL )
        m_pMyNewTube_Options->updateUseShader();
}

/**
 *
 */
void dlg_cartoon::toggleCartoonShader()
{
    m_pPdbNode->setCartoonParam( CARTOON_USE_SHADER, (1.0f - m_pPdbNode->getCartoonParam(CARTOON_USE_SHADER)) );
    updateUseShader();

    if (m_pMyNewTube_Options != NULL)
        m_pMyNewTube_Options->updateUseShader();
}

/**
 *
 */
void dlg_cartoon::toggleCartoonHelixCylinder()
{
    m_pPdbNode->setCartoonParam( CARTOON_HELIX_CYLINDER, (1.0f - m_pPdbNode->getCartoonParam(CARTOON_HELIX_CYLINDER)) );
}

/**
 *
 */
void dlg_cartoon::toggleCartoonPeptidePlanes()
{
    m_pPdbNode->setCartoonParam( CARTOON_PEPTIDE_PLANES, (1.0f - m_pPdbNode->getCartoonParam(CARTOON_PEPTIDE_PLANES)) );
}

/**
 *
 */
void dlg_cartoon::toggleCartoonHelixArrowheads()
{
    m_pPdbNode->setCartoonParam( CARTOON_HELIX_ARROWHEADS, (1.0f - m_pPdbNode->getCartoonParam(CARTOON_HELIX_ARROWHEADS)) );
}

/**
 *
 */
void dlg_cartoon::setCartoonSheetWidth()
{
    m_pPdbNode->setCartoonParam( CARTOON_SHEET_WIDTH, (float) m_pCartoonSheetWidth->value() );
}

/**
 *
 */
void dlg_cartoon::setCartoonSheetHeight()
{
    m_pPdbNode->setCartoonParam( CARTOON_SHEET_HEIGHT, (float) m_pCartoonSheetHeight->value() );
}

/**
 *
 */
void dlg_cartoon::setCartoonHelixWidth()
{
    m_pPdbNode->setCartoonParam( CARTOON_HELIX_WIDTH, (float) m_pCartoonHelixWidth->value() );
}

/**
 *
 */
void dlg_cartoon::setCartoonHelixHeight()
{
    m_pPdbNode->setCartoonParam( CARTOON_HELIX_HEIGHT, (float) m_pCartoonHelixHeight->value() );
}

/**
 *
 */
void dlg_cartoon::setCartoonTubeDiameter(double)
{
    m_pPdbNode->setCartoonParam( CARTOON_TUBE_DIAMETER, (float) m_pCartoonTubeDiameter->value() );
}

/**
 *
 */
void dlg_cartoon::setCartoonSegments()
{
    m_pPdbNode->setCartoonParam( CARTOON_SPLINE_SEGMENTS, (float) m_pSpinBoxSegments->value() );
}

/**
 *
 */
void dlg_cartoon::setNewTubeProfileCorners()
{
    m_pPdbNode->setNewTubeProfileCorners( m_pSpinBoxTubeCorners->value() );

    if (m_pMyNewTube_Options != NULL)
        m_pMyNewTube_Options->updateTubeProfileCorners();
}

/**
 *
 */
void dlg_cartoon::setCartoonSheetHelixProfileCorners(int)
{
    if ( m_pShapeFlat->isChecked() )
    {
        m_pCartoonHelixHeight   ->setDisabled( TRUE );
        m_pCartoonSheetHeight   ->setDisabled( TRUE );
        m_pSpinBoxSheetHelixCorners->setDisabled( TRUE );

        m_pPdbNode->setCartoonParam( CARTOON_SHEET_HELIX_PROFILE_CORNERS, 2.0f );
    }
    else if ( m_pShapeRectangular->isChecked() )
    {
        m_pCartoonHelixHeight   ->setEnabled( TRUE );
        m_pCartoonSheetHeight   ->setEnabled( TRUE );
        m_pSpinBoxSheetHelixCorners->setDisabled( TRUE );

        m_pPdbNode->setCartoonParam( CARTOON_SHEET_HELIX_PROFILE_CORNERS, 4.0f );
    }
    else if ( m_pShapeRoundedRectangular->isChecked() )
    {
        m_pCartoonHelixHeight   ->setEnabled( TRUE );
        m_pCartoonSheetHeight   ->setEnabled( TRUE );
        m_pSpinBoxSheetHelixCorners->setEnabled( FALSE );

        // to be implemented
    }
    else if ( m_pShapeElliptical->isChecked() )
    {
        m_pCartoonHelixHeight   ->setEnabled( TRUE );
        m_pCartoonSheetHeight   ->setEnabled( TRUE );
        m_pSpinBoxSheetHelixCorners->setEnabled( TRUE );

        m_pPdbNode->setCartoonParam( CARTOON_SHEET_HELIX_PROFILE_CORNERS, (float)m_pSpinBoxSheetHelixCorners->value() );
    }
}

/**
 *
 */
void dlg_cartoon::updateMinRes()
{
    m_pSpinBoxSheetRes->blockSignals( TRUE );
    m_pSpinBoxSheetRes->setValue( (int)m_pPdbNode->getCartoonParam(CARTOON_SHEET_MIN_RESIDUES) );
    m_pSpinBoxSheetRes->blockSignals( FALSE );

    m_pSpinBoxHelixRes->blockSignals( TRUE );
    m_pSpinBoxHelixRes->setValue( (int)m_pPdbNode->getCartoonParam(CARTOON_HELIX_MIN_RESIDUES) );
    m_pSpinBoxHelixRes->blockSignals( FALSE );
}

/**
 *
 */
void dlg_cartoon::updateTubeProfileCorners()
{
    m_pSpinBoxTubeCorners->blockSignals( TRUE );
    m_pSpinBoxTubeCorners->setValue( (int)m_pPdbNode->getNewTubeProfileCorners() );
    m_pSpinBoxTubeCorners->blockSignals( FALSE );
}

/**
 *
 */
void dlg_cartoon::updateUseShader()
{
    m_pCheckBoxShader->blockSignals(TRUE);
    m_pCheckBoxPerPixelLighting->blockSignals(TRUE);

    if ( !m_pPdbNode->getShaderSupported() || !svt_getAllowShaderPrograms() )
    {
        // shader not support/allowed - uncheck and disable all relevant checkboxes
        m_pCheckBoxShader->setEnabled(FALSE);
        m_pCheckBoxShader->setChecked(FALSE);
        m_pCheckBoxPerPixelLighting->setEnabled(FALSE);
        m_pCheckBoxPerPixelLighting->setChecked(FALSE);
    }
    else
    {
        // shader supported/allowed
        m_pCheckBoxShader->setEnabled(TRUE);

        if (m_pPdbNode->getCartoonParam(CARTOON_USE_SHADER) == 1.0f)
        {
            // shader supported and activated
            m_pCheckBoxShader->setChecked(TRUE);
            m_pCheckBoxPerPixelLighting->setEnabled(TRUE);

            if (m_pPdbNode->getCartoonParam( CARTOON_PER_PIXEL_LIGHTING ) == 1.0f)
                m_pCheckBoxPerPixelLighting->setChecked(TRUE);
            else
                m_pCheckBoxPerPixelLighting->setChecked(FALSE);

        }
        else
        {
            // shader supported but not activated
            m_pCheckBoxShader->setChecked(FALSE);
            m_pCheckBoxPerPixelLighting->setEnabled(FALSE);
            m_pCheckBoxPerPixelLighting->setChecked(FALSE);
        }
    }

    m_pCheckBoxShader->blockSignals(FALSE);
    m_pCheckBoxPerPixelLighting->blockSignals(FALSE);
}

/**
 *
 */
void dlg_cartoon::setColorDlgSecStruct(dlg_colmapsecstruct* pMyColmapDlgSecStruct)
{
    m_pMyColmapDlgSecStruct = pMyColmapDlgSecStruct;
}

/**
 *
 */
void dlg_cartoon::setNewTubePropDlg(dlg_newtube * pMyNewTube_Options)
{
    m_pMyNewTube_Options = pMyNewTube_Options;
}
