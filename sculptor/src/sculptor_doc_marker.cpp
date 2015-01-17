/***************************************************************************
                          sculptor_doc_marker.cpp  -  description
                             -------------------
    begin                : 30.07.2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// svt includes
#include <svt_pos.h>
#include <svt_sphere.h>
#include <svt_cylinder.h>
// sculptor includes
#include <sculptor_node.h>
#include <sculptor_doc_marker.h>
#include <sculptor_prop_marker.h>
#include <sculptor_window.h>
#include <sculptor_window_documentlistitem.h>

extern sculptor_window* g_pWindow;
extern string QtoStd(QString);

/**
 * Document that shows a marker in 3D space
 */
sculptor_doc_marker::sculptor_doc_marker(svt_node* pNode)
    : sculptor_document( pNode ),
      m_pMarkerPropDlg( NULL ),
      m_oPosition (svt_vector4<Real64> (0.0f,0.0f,0.0f)),
      m_bShowName3D( false )
{
    //create marker node and add it to the graphnode
    m_pMarkerNode = new sculptor_node(this);
    m_pMarkerNode->setName("(sculptor_doc_marker)m_pMarkerNode");
    m_pMarkerNode->setScale( 2.5 );
    m_pMarkerNode->setIgnoreClipPlanes(true);
    m_pGraphNode->add(m_pMarkerNode);

    m_pPoint = new svt_sphere( new svt_pos( m_oPosition.x(), m_oPosition.y(),m_oPosition.z() ), new svt_properties(new svt_color(1.0f,0.0f,0.0f)) );
    m_pPoint->setRadius(0.0035); 
    m_pPoint->rebuildDL();
    m_pPoint->setVisible( true );
    m_pMarkerNode->add(m_pPoint);

    Real64 fValue = 0.015f;

    m_pLineX = new svt_cylinder(new svt_pos(-fValue, 0.00f, 0.00f), new svt_pos(fValue, 0.00f, 0.00f),new svt_properties(new svt_color(1.0f,0.0f,0.0f)), 0.002f );
    m_pLineX->rebuildDL();
    m_pLineX->setVisible( true );
    m_pMarkerNode->add(m_pLineX);

    m_pEndX1 = new svt_sphere( new svt_pos( -fValue, 0.00f, 0.00f ), new svt_properties(new svt_color(1.0f,0.0f,0.0f)) );
    m_pEndX1->setRadius(0.0019); 
    m_pEndX1->rebuildDL();
    m_pEndX1->setVisible( true );
    m_pMarkerNode->add(m_pEndX1);

    m_pEndX2 = new svt_sphere( new svt_pos( fValue, 0.00f, 0.00f ), new svt_properties(new svt_color(1.0f,0.0f,0.0f)) );
    m_pEndX2->setRadius(0.0019); 
    m_pEndX2->rebuildDL();
    m_pEndX2->setVisible( true );
    m_pMarkerNode->add(m_pEndX2);

 
    m_pLineY = new svt_cylinder(new svt_pos(0.00f, -fValue, 0.00f), new svt_pos(0.00f, fValue, 0.00f), new svt_properties(new svt_color(1.0f,0.0f,0.0f)), 0.002f );
    m_pLineY->rebuildDL();
    m_pLineY->setVisible( true );   
    m_pMarkerNode->add(m_pLineY);
  
    m_pEndY1 = new svt_sphere( new svt_pos( 0.00f, -fValue, 0.00f ), new svt_properties(new svt_color(1.0f,0.0f,0.0f)) );
    m_pEndY1->setRadius(0.0019); 
    m_pEndY1->rebuildDL();
    m_pEndY1->setVisible( true );
    m_pMarkerNode->add(m_pEndY1);

    m_pEndY2 = new svt_sphere( new svt_pos( 0.00f, fValue, 0.00f ), new svt_properties(new svt_color(1.0f,0.0f,0.0f)) );
    m_pEndY2->setRadius(0.0019); 
    m_pEndY2->rebuildDL();
    m_pEndY2->setVisible( true );
    m_pMarkerNode->add(m_pEndY2);   

    m_pLineZ = new svt_cylinder(new svt_pos(0.00f, 0.00f, -fValue), new svt_pos(0.00f, 0.00f, fValue), new svt_properties(new svt_color(1.0f,0.0f,0.0f)), 0.002f );
    m_pLineZ->rebuildDL();
    m_pLineZ->setVisible( true );   
    m_pMarkerNode->add(m_pLineZ);
 
    m_pEndZ1 = new svt_sphere( new svt_pos( 0.00f, 0.00f, -fValue ), new svt_properties(new svt_color(1.0f,0.0f,0.0f)) );
    m_pEndZ1->setRadius(0.0019); 
    m_pEndZ1->rebuildDL();
    m_pEndZ1->setVisible( true );
    m_pMarkerNode->add(m_pEndZ1);

    m_pEndZ2 = new svt_sphere( new svt_pos( 0.00f, 0.00f, fValue ), new svt_properties(new svt_color(1.0f,0.0f,0.0f)) );
    m_pEndZ2->setRadius(0.0019); 
    m_pEndZ2->rebuildDL();
    m_pEndZ2->setVisible( true );
    m_pMarkerNode->add(m_pEndZ2);

    m_pLabel = new svt_textnode( new svt_pos(m_oPosition.x() + fValue/4.0,
                                             m_oPosition.y() - fValue/4.0,
                                             m_oPosition.z())              );
    m_pLabel->setCentered(false);
    m_pLabel->setScale(0.5f);
    m_pMarkerNode->add(m_pLabel);
}

/**
 *
 */
sculptor_doc_marker::~sculptor_doc_marker()
{
   //deletion of propertydlg should be done by window_documentlist->removeItem from the PropWidgetStack.
}

/**
 *
 */
svt_node* sculptor_doc_marker::getNode()
{
    return m_pMarkerNode;
}

///////////////////////////////////////////////////////////////////////////////
// properties dlg
///////////////////////////////////////////////////////////////////////////////

/**
 *
 */
QWidget* sculptor_doc_marker::createPropDlg(QWidget* pParent)
{
    if (m_pMarkerPropDlg == NULL)
        m_pMarkerPropDlg = new prop_marker(this, pParent);

    updatePropDlg();

    return (QWidget*)m_pMarkerPropDlg;
}

/**
 *
 */
void sculptor_doc_marker::updatePropDlg()
{
    if (m_pMarkerPropDlg)
        m_pMarkerPropDlg->update();
};

/**
 *
 */
QWidget* sculptor_doc_marker::getPropDlg()
{
    return (QWidget*)m_pMarkerPropDlg;
}

/**
 *
 */
Real32 sculptor_doc_marker::getMarkerNodePosX()
{
    return m_pMarkerNode->getPosX();
}

/**
 *
 */
Real32 sculptor_doc_marker::getMarkerNodePosY()
{
    return m_pMarkerNode->getPosY();
}

/**
 *
 */
Real32 sculptor_doc_marker::getMarkerNodePosZ()
{
    return m_pMarkerNode->getPosZ();
}

/**
 *
 */
svt_vector4<Real64> sculptor_doc_marker::getMarkerNodePos()
{
    svt_vector4<Real64> oVecMarkerPos = svt_vector4<Real64>(getMarkerNodePosX(), getMarkerNodePosY(), getMarkerNodePosZ());
    return oVecMarkerPos;
}

/**
 * set position of marker node
 * \param fX x position of marker node
 * \param fY y position of marker node
 * \param fZ z position of marker node
 */
void sculptor_doc_marker::setMarkerNodePos(Real64 fX, Real64 fY, Real64 fZ)
{
    if (m_pMarkerNode->getTransformation() == NULL)
    {
        m_pMarkerNode->setTransformation( new svt_pos( fX, fY, fZ ) );
        return;
    }

    m_pMarkerNode->getTransformation()->setTranslationX( fX );
    m_pMarkerNode->getTransformation()->setTranslationY( fY );
    m_pMarkerNode->getTransformation()->setTranslationZ( fZ );
};

/**
 * set x position of marker node
 * \param fX x position of marker node
 */
void sculptor_doc_marker::setMarkerNodePosX(Real64 fX)
{
    if (m_pMarkerNode->getTransformation() == NULL)
        m_pMarkerNode->setTransformation( new svt_pos( 0.0, 0.0, 0.0 ) );

    m_pMarkerNode->getTransformation()->setTranslationX( fX );
};
/**
 * set y position of marker node
 * \param fY y position of marker node
 */
void sculptor_doc_marker::setMarkerNodePosY(Real64 fY)
{
    if (m_pMarkerNode->getTransformation() == NULL)
        m_pMarkerNode->setTransformation( new svt_pos( 0.0, 0.0, 0.0 ) );

    m_pMarkerNode->getTransformation()->setTranslationY( fY );
};
/**
 * set z position of marker node
 * \param fZ z position of marker node
 */
void sculptor_doc_marker::setMarkerNodePosZ(Real64 fZ)
{
    if (m_pMarkerNode->getTransformation() == NULL)
        m_pMarkerNode->setTransformation( new svt_pos( 0.0, 0.0, 0.0 ) );

    m_pMarkerNode->getTransformation()->setTranslationZ( fZ );
};

/**
 *
 */
int sculptor_doc_marker::getType()
{
    return SENSITUS_DOC_MARKER;
};

/**
 * Change the color of the marker
 */
void sculptor_doc_marker::setColor(svt_color  oCol)
{
    *m_pLineX->getProperties()->getColor() = oCol;
    *m_pLineY->getProperties()->getColor() = oCol;
    *m_pLineZ->getProperties()->getColor() = oCol;

    *m_pPoint->getProperties()->getColor() = oCol;
 
    *m_pEndX1->getProperties()->getColor() = oCol;
    *m_pEndX2->getProperties()->getColor() = oCol;
    *m_pEndY1->getProperties()->getColor() = oCol;
    *m_pEndY2->getProperties()->getColor() = oCol;
    *m_pEndZ1->getProperties()->getColor() = oCol;
    *m_pEndZ2->getProperties()->getColor() = oCol;

    if (m_pDLItem != NULL)
        m_pDLItem->showColor();
};
/**
 * Get a representative color for the document
 * \return svt_color object
 */
svt_color sculptor_doc_marker::getColor()
{
    return *m_pLineX->getProperties()->getColor();
};

/**
 * get the position of the sculptor document in real space (e.g. for pdb or situs files)
 * \return svt_vector4f with the position
 */
svt_vector4f sculptor_doc_marker::getRealPos()
{
    return svt_vector4f( getMarkerNodePosX() * 1.0E-8f,getMarkerNodePosY() * 1.0E-8f, getMarkerNodePosZ()  * 1.0E-8f);
};

/**
 * If the marker document is selected, all the lines are shown, if not, only the sphere in the middle is visible.
 */
void sculptor_doc_marker::onSelection()
{
    m_pLineX->setVisible(true);
    m_pLineY->setVisible(true);
    m_pLineZ->setVisible(true);

    m_pPoint->setVisible(true);
 
    m_pEndX1->setVisible(true);
    m_pEndX2->setVisible(true);
    m_pEndY1->setVisible(true);
    m_pEndY2->setVisible(true);
    m_pEndZ1->setVisible(true);
    m_pEndZ2->setVisible(true);
};
/**
 * If the marker document is selected, all the lines are shown, if not, only the sphere in the middle is visible.
 */
void sculptor_doc_marker::onDeselection()
{
    m_pLineX->setVisible(false);
    m_pLineY->setVisible(false);
    m_pLineZ->setVisible(false);

    m_pPoint->setVisible(true);
 
    m_pEndX1->setVisible(false);
    m_pEndX2->setVisible(false);
    m_pEndY1->setVisible(false);
    m_pEndY2->setVisible(false);
    m_pEndZ1->setVisible(false);
    m_pEndZ2->setVisible(false);
};

/**
 * restores the state of document from xml datastructure
 * \param xml element with stored state of document
 */
void sculptor_doc_marker::setState(TiXmlElement* pElem)
{
    sculptor_document::setState(pElem);

    double d;
    int i;
    svt_vector3< Real64 > oVec;
    Real64 fR = -1.0;
    Real64 fG = -1.0;
    Real64 fB = -1.0;

    if (pElem->Attribute("SHOWNAME3D", &i))    setShowName3D( (bool)i );
    if (pElem->Attribute("SCALE", &d))         m_pMarkerNode->setScale( d );
    if (pElem->Attribute("POSX", &d))          setMarkerNodePosX(d);
    if (pElem->Attribute("POSY", &d))          setMarkerNodePosY(d);
    if (pElem->Attribute("POSZ", &d))          setMarkerNodePosZ(d);
    if (pElem->Attribute("COLRED",   &d))      fR = d;
    if (pElem->Attribute("COLGREEN", &d))      fG = d;
    if (pElem->Attribute("COLBLUE",  &d))      fB = d;

    svt_color oCol;
    if (fR != -1.0)
    {
        oCol.setR( fR );
        oCol.setG( fG );
        oCol.setB( fB );
        setColor( oCol );
    }

    updatePropDlg();
};

/**
 * stores the state of document in xml datastructure
 * \param xml element to store state of document
 */
void sculptor_doc_marker::getState(TiXmlElement* pElem)
{
    sculptor_document::getState(pElem);

    pElem->SetAttribute      ("SHOWNAME3D", (int)getShowName3D() );
    pElem->SetDoubleAttribute("SCALE",      m_pMarkerNode->getScaleX() );
    pElem->SetDoubleAttribute("POSX",       getMarkerNodePosX() );
    pElem->SetDoubleAttribute("POSY",       getMarkerNodePosY() );
    pElem->SetDoubleAttribute("POSZ",       getMarkerNodePosZ() );
    pElem->SetDoubleAttribute("COLRED",     getColor().getR() );
    pElem->SetDoubleAttribute("COLGREEN",   getColor().getG() );
    pElem->SetDoubleAttribute("COLBLUE",    getColor().getB() );
};

/**
 *
 */
void sculptor_doc_marker::setName(const QString& oStr)
{
    if ( oStr.isEmpty() )
        setDisplayName( string("Marker") );
    else
        setDisplayName( QtoStd(oStr) );

    m_pDLItem->showDocName();

    if (m_bShowName3D)
    {
        string oStr = getDisplayName();
        oStr.insert(0, " ");
        m_pLabel->setText( (char*)oStr.c_str() );
    }
}

/**
 *
 */
void sculptor_doc_marker::setShowName3D(bool bShowName3D)
{
    m_bShowName3D = bShowName3D;

    string oStr = getDisplayName();
    oStr.insert(0, " ");

    m_pLabel->setText( (char*)(oStr.c_str()) );
    m_pLabel->setVisible(bShowName3D);
}

/**
 *
 */
bool sculptor_doc_marker::getShowName3D()
{
    return m_bShowName3D;
}

/**
 * scale the marker
 */ 
void sculptor_doc_marker::setScaling(float fScale)
{
    getNode()->setScale(fScale);
    getNode()->rebuildDL();
}
