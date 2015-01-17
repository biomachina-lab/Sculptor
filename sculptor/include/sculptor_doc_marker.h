/*-*-c++-*-*****************************************************************
                          sculptor_doc_marker.h  -  description
                             -------------------
    begin                : 11.04.2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DOC_MARKER_H
#define SCULPTOR_DOC_MARKER_H

// svt includes
class svt_pos;
class svt_sphere;
class svt_cylinder;
class svt_textnode;
// sculptor includes
class prop_marker;
class sculptor_node;
#include <sculptor_document.h>


/** This class stores a single marker tool document
 * @author Mirabela Rusu
 */
class sculptor_doc_marker : public sculptor_document
{
    Q_OBJECT

protected:

    // marker node from the scene
    sculptor_node* m_pMarkerNode;
    // marker property dialog
    prop_marker* m_pMarkerPropDlg;
    // matrix that stores transformation of target
    svt_matrix4<Real64> m_oTransf;
    // vector for x,y,z position of node
    svt_vector4<Real64> m_oPosition;
    //center
    svt_sphere *m_pPoint;
    // the line on the X axis
    svt_cylinder *m_pLineX;
    svt_cylinder *m_pLineY;
    svt_cylinder *m_pLineZ;
    //ends of the cylinders
    svt_sphere *m_pEndX1, *m_pEndX2;
    svt_sphere *m_pEndY1, *m_pEndY2;
    svt_sphere *m_pEndZ1, *m_pEndZ2;
    // text label for 3d window
    svt_textnode* m_pLabel;
    // display name in 3d window?
    bool m_bShowName3D;

public:

    /**
     * Constructor
     * \param pNode pointer to svt_node to use as marker node
     */
    sculptor_doc_marker(svt_node* pNode);
    
    /**
     * Destructor
     */
    virtual ~sculptor_doc_marker();

    /**
     * Get a pointer to the svt_node the document is providing
     * \return m_pMarkerNode
     */
    svt_node* getNode();

    /**
     * Creates the marker property dialog
     * \return m_pMarkerPropDlg
     */
    virtual QWidget* createPropDlg(QWidget*);

    /**
     * Update the marker's property dialog
     * \return m_pMarkerPropDlg
     */
    void updatePropDlg();

    /**
     * Returns the marker property dialog
     * \return m_pMarkerPropDlg
     */
    virtual QWidget* getPropDlg();

    /**
     * get the type of the document
     * \return SENSITUS_DOC_MARKER
     */
    virtual int getType();

    /**
     * get x position of marker node
     * \return Real32 x position of marker node
     */
    Real32 getMarkerNodePosX();
    /**
     * get y position of marker node
     * \return Real32 y position of marker node
     */
    Real32 getMarkerNodePosY();
    /**
     * get z position of marker node
     * \return Real32 z position of marker node
     */
    Real32 getMarkerNodePosZ();

    /**
     * set position of marker node
     * \param fX x position of marker node
     * \param fY y position of marker node
     * \param fZ z position of marker node
     */
    void setMarkerNodePos(Real64 fX, Real64 fY, Real64 fZ);

    /**
     * get vector position of marker
     * \return oVecMarkerPos
     */
    svt_vector4<Real64> getMarkerNodePos();

    /**
     * set x position of marker node
     * \param fX x position of marker node
     */
    void setMarkerNodePosX(Real64 fX);
    /**
     * set y position of marker node
     * \param fY y position of marker node
     */
    void setMarkerNodePosY(Real64 fY);
    /**
     * set z position of marker node
     * \param fZ z position of marker node
     */
    void setMarkerNodePosZ(Real64 fZ);

    /**
     * Change the color of the marker
     */
    void setColor(svt_color oCol);
    /**
     * Get a representative color for the document
     * \return svt_color object
     */
    virtual svt_color getColor();

    /**
     * get the position of the sculptor document in real space (e.g. for pdb or situs files)
     * \return svt_vector4f with the position
     */
    virtual svt_vector4f getRealPos();

    /**
     * If the marker document is selected, all the lines are shown, if not, only the sphere in the middle is visible.
     */
    virtual void onSelection();
    /**
     * If the marker document is selected, all the lines are shown, if not, only the sphere in the middle is visible.
     */
    virtual void onDeselection();

    /**
     * restores the state of document from xml datastructure
     * \param xml element with stored state of document
     */
    virtual void setState(TiXmlElement* pElem);

    /**
     * stores the state of document in xml datastructure
     * \param xml element to store state of document
     */
    virtual void getState(TiXmlElement* pElem);

    /**
     * get if the display name should be shown as label in the 3d window
     */ 
    bool getShowName3D();

public slots:

    /**
     * set the display name of the marker
     */ 
    void setName(const QString&);

    /**
     * set if the display name should be shown as label in the 3d window
     */ 
    void setShowName3D(bool bShowName3D);

    /**
     * scale the marker
     */ 
    void setScaling(float fScale);
};

#endif
