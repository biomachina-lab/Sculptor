/***************************************************************************
                          svt_forceArrows.h  -  description
                             -------------------
    begin                : Dec 21 2004
    author               : Maik Boltes
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_FORCEARROWS_H
#define SVT_FORCEARROWS_H

#include <svt_node.h>
#include <svt_pos.h>
#include <svt_properties.h>
#include <svt_mesh.h>
#include <svt_rectangle.h>
#include <svt_const.h>

enum arrowType {leftArrow, rightArrow, upArrow, downArrow, clockArrow, counterClockArrow, forwardArrow, backwardArrow};

/**
 * A class for arrows to visualize the torque forces.
 *@author Maik Boltes
 */
class DLLEXPORT svt_forceArrows : public svt_node  {

protected:
    svt_mesh *m_pTorqueArrows;
    svt_mesh *m_pTransArrows;
    svt_rectangle *m_pCorrBar;
    svt_rectangle *m_pCorrMaxFlag;

    double m_zOffset;
    double m_xOffsetTorque;
    double m_yOffsetTorque;
    double m_xOffsetCorr;
    double m_yOffsetCorr;
    double m_heightCorr;
    double m_widthCorr;
    double m_heightMaxFlag;
    double m_CorrMax;
    double m_Corr;

    double lenMin;

public:
    /**
     * Constructor
     * \param pPos pointer to svt_pos object
     * \param pProp pointer to svt_properties object
     */
    svt_forceArrows(svt_pos* pPos =NULL, svt_properties* pProp =NULL);
    virtual ~svt_forceArrows();

    /**
     * reset the values for the correlation bar
     */
    void corrReset();

    /**
     * set just the max value for the correlation
     * 
     * \param co actual correlation value
     */
    void setCorrMaxValue(double co);

    /**
     * set the max value for the correlation bar, if co is greater than the max before
     * 
     * \param co actual correlation value
     */
    void setCorrMax(double co);

    /**
     * set the correlation bar to co
     * set also the max correlation, if it increases the old max value
     * 
     * \param co actual correlation value
     */
    void setCorr(double co);

    /**
     * draw the arrows
     */
    void drawGL();

    /**
     * set the length of torque force arrows
     * 
     * \param oTorqueVec torsion vector -100..100
     */
    void setTorqueArrowLength(svt_vector4<double> oTorqueVec);
    /**
     * set the length of torque force arrow
     * 
     * \param i arrow number (0..5)
     * \param len new length of the arrow
     */
    void setTorqueArrowLength(enum arrowType i, double len);

    /**
     * set the length of transition force arrows
     * 
     * \param oTorqueVec torsion vector -100..100
     */
    void setTransArrowLength(svt_vector4<double> oTransVec);
    /**
     * set the length of transition force arrow
     * 
     * \param i arrow number (0..3,6,7)
     * \param len new length of the arrow
     */
    void setTransArrowLength(enum arrowType aT, double len);
};

#endif
