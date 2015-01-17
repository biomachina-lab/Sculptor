/***************************************************************************
                          sculptor_dlg_force.h
                          -----------------
    begin                : 11.08.2010
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_FORCE_H
#define SCULPTOR_DLG_FORCE_H

// uic-qt4 includes
#include <ui_dlg_force.h>

class pdb_document;

/**
 * Simple class that inherits the dialog created with the designer and implements the dialog's
 * functionality.
 * \author Mirabela Rusu
 */
class dlg_force : public QDialog, private Ui::dlg_force
{
    Q_OBJECT

public:

    /**
     * Constructor
     */
    dlg_force(QWidget* pParent);

    /**
     * Destructor
     */
    virtual ~dlg_force();

public slots:
 
    /**
     * set translational force scaling
     */
    void setForceTransScale();
   
    /**
     * en/disable the translational forces along the x axis
     * \param bSet if true force is enabled
     */
    void setForceTransX(bool bSet);
    /**
     * en/disable the translational forces along the y axis
     * \param bSet if true force is enabled
     */
    void setForceTransY(bool bSet);
    /**
     * en/disable the translational forces along the z axis
     * \param bSet if true force is enabled
     */
    void setForceTransZ(bool bSet);

    /**
     * en/disable the rotational forces along the x axis
     * \param bSet if true force is enabled
     */
    void setForceRotX(bool bSet);
    /**
     * en/disable the rotational forces along the y axis
     * \param bSet if true force is enabled
     */
    void setForceRotY(bool bSet);
    /**
     * en/disable the rotational forces along the z axis
     * \param bSet if true force is enabled
     */
    void setForceRotZ(bool bSet);
    /**
     * set the correlation force scale
     * \param iScale scaling of the force in percent (0..100)
     */
    void setCorrForceScale();
    /**
     * set the steric force scale
     */
    void setStericForceScale();
    /**
     * switch the logic of the button of the force feedback device
     * \param bLogic if true the button logic is switched (if you press the button, no forces are generated).
     */
    void setButtonLogic(bool bLogic);

    /**
     * make force arrow (in)visible
     * \param bVisible show force arrow?
     */
    void setForceArrow(bool bVisible);

    /**
     * set the force arrow scale
     */
    void setForceArrowScale();

    /**
     * set the central sphere scale
     */
    void setCentralSphereScale();

    /**
     * set the threshold for steric clashes
     */
    void setStericClashThreshold();

    /**
     * set the threshold for "good" steric contacts
     */
    void setStericGoodThreshold();

    /**
     * set the sphere color function Red
     * \param iScale (0..1000)
     */
    void setSphereColorFunctionRed();
    /**
     * set the sphere color function Blue
     * \param iScale (0..1000)
     */
    void setSphereColorFunctionBlue();
    /**
     * set the sphere color function Green
     * \param iScale (0..1000)
     */
    void setSphereColorFunctionGreen();

};

#endif
