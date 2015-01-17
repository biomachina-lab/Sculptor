/***************************************************************************
                          svt_move_tool.h  -  description
                             -------------------
    begin                : 07.05.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_MOVE_TOOL_H
#define SVT_MOVE_TOOL_H

#include <svt_core.h>
#include <svt_node.h>
#include <svt_tool.h>
#include <svt_matrix4f.h>
#include <svt_matrix4.h>
#include <svt_spline.h>
#include <svt_vector4.h>

/** A move tool. This class is a more specialized form of the universal tool class. You can connect a node (and with the node the complete subtree of course) to the move tool. If the users presses the button of the device, he can move (rotate and translate) the objects. If he releases the button, the objects are again fixed.
 * The class will search for PointerDevice, PointerMatrixChannel, PointerButtonChannel in the config file.
 *
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_move_tool : public svt_tool
{
protected:
    svt_node* m_pMoveNode;

    svt_matrix4f* m_pViewMat;
    svt_matrix4f* m_pOldViewMat;
    svt_matrix4f* m_pOldDevMat;
    svt_matrix4f* m_pTempMat;
    svt_matrix4f* m_pMoveMat;
    svt_matrix4f* m_pGlobalMatrix;

    Real32 m_fOldDevX;
    Real32 m_fOldDevY;
    Real32 m_fOldDevZ;
    Real32 m_fOldViewX;
    Real32 m_fOldViewY;
    Real32 m_fOldViewZ;
    Real32 m_fTempX;
    Real32 m_fTempY;
    Real32 m_fTempZ;
    bool m_bCenter;
    bool m_bAlways;
    bool m_bNeverMoving;
    bool m_bTranslationOnly;

    svt_vector4<Real32> m_oPivotPoint;
    svt_vector4<Real32> m_oOldPivot;

    // scaling of translations
    Real32 m_fTranslationScale;

    //
    // new stuff for fly mode and auto-pilot
    // (fd)
    // NOTE: temporary solution!
    //       new entries are redundant
    //       class needs cleanup and redesign

    bool                m_bFlyMode;

    int                 m_t0;
    int                 m_t1;
    Real32              m_fSpeed;
    Real32              m_zMove;

    svt_spline          m_oAP_Route;
    bool                m_bAP;
    Real32              m_fAP_Dist;
    svt_vector4<Real32> m_vUp;

    bool                m_bLocalTrans;

    void setAP_Position(float fDist);

public:
    /**
     * Constructor
     * \param pName of the pointer device (like in svt_tool)
     * \param pNode the node to move (default: NULL)
     * \param pTool the tool representation node (default: NULL)
     */
    svt_move_tool(const char* pName, svt_node* pNode = NULL);
    virtual ~svt_move_tool();

    /**
     * set the node which should be moved around
     * \param pNode the node which should be moved around
     */
    void setMoveNode(svt_node* pNode);
    /**
     * get the node which is moved around
     * \return pointer to the node
     */
    svt_node* getMoveNode();

    /**
     * if center mode is set to true, the transformed object will get rotated around its current position (default = false)
     */
    void setCenterMode(bool tf);
    /**
     * \return the center mode setting
     */
    bool getCenterMode();

    /**
     * always moving, or only if the button is pressed (Default: true)
     * \param bAlways true if the object is always moving around
     */
    void setAlwaysMoving(bool bAlways);
    /**
     * always moving, or only if the button is pressed (Default: true)
     * \return always moving the objects around, or only if the button is pressed?
     */
    bool getAlwaysMoving();

    /**
     * Only return translations? (Default: false)
     * \param bTranslationOnly true if only translations of tool are used
     */
    void setTranslationOnly(bool bTranslationOnly);
    /**
     * Only return translations?
     * \return Only translations of tool are used?
     */
    bool getTranslationOnly();

    /**
     * set a global transformation matrix
     * \param pMatrix pointer to the global matrix
     */
    void setGlobalMatrix(svt_matrix4f* pMatrix);
    /**
     * get a global transformation matrix
     * \return pointer to the global matrix
     */
    svt_matrix4f* getGlobalMatrix();

    /**
     * switch on/off the moving
     * \param bNeverMoving if true the target is never moved around
     */
    void setNeverMoving(bool bNeverMoving);
    /**
     * get the state of the on/off switch
     * \return if true the target is never moved around
     */
    bool getNeverMoving();

    /**
     * set pivot point (default: origin)
     * \param oPivotPoint svt_vector4<Real32> with the pivot point information
     */
    void setPivotPoint(svt_vector4<Real32> oPivotPoint);
    /**
     * get pivot point (default: origin)
     * \return svt_vector4<Real32> with the pivot point information
     */
    svt_vector4<Real32> getPivotPoint();

public:

    /**
     * Button pressed
     */
    void onButtonPressed();
    /**
     * Button released
     */
    void onButtonReleased();
    /**
     * Recalc function
     */
    bool recalc();

    /**
     * Set the translation scaling
     */
    void setTranslationScale(Real32 fTranslationScale);

    /**
     * Get the translation scaling
     */
    Real32 getTranslationScale();


    // Fly Mode

    bool flyMode() const {return m_bFlyMode;}
    void setFlyMode(bool b);
    bool toggleFlyMode() {setFlyMode(!m_bFlyMode); return m_bFlyMode;}

    Real32 speed() const {return m_fSpeed;}
    void   setSpeed(Real32 f) { if (m_bFlyMode) m_fSpeed=f;}
    void   flyFaster() { if (m_bFlyMode) m_fSpeed+=0.0005;}
    void   flySlower() {
	if (m_bFlyMode) m_fSpeed-=0.0005;
	if (m_bAP) m_fSpeed = (m_fSpeed>0.0f?m_fSpeed:0.0f); //  max(m_fSpeed, 0.0f);
    }

    void turnLeft();
    void turnRight();
    void up();
    void down();


    void startAutoPilot(const char* pszFilename,
			const svt_vector4<Real32>& upDir,
			Real32 initialSpeed=0.0035);

    void startAutoPilot(const char* pszFilename,
			Real32 initialSpeed=0.0035) {
	startAutoPilot(pszFilename, svt_vector4<Real32>(0,1,0,0), initialSpeed);
    }

    bool autopilot() const { return m_bAP; }
    void stopAutoPilot() {m_bAP=false; m_bFlyMode=true; setSpeed(0);}

    svt_vector4<Real32> APpos();



private:

    /**
     * this function applies a global matrix
     */
    void applyGlobal(svt_matrix4f& oLocal, svt_matrix4f& oGlobal);
};

#endif
