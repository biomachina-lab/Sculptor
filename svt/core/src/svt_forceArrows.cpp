/***************************************************************************
                          svt_forceArrows.cpp  -  description
                             -------------------
    begin                : Dec 21 2004
    author               : Maik Boltes
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_forceArrows.h>

/**
 * Constructor
 * \param pPos pointer to svt_pos object
 * \param pProp pointer to svt_properties object
 * \param fHeight height of the cone
 * \param fRadius radius of the cone
 */
svt_forceArrows::svt_forceArrows(svt_pos* pPos, svt_properties* pProp) : svt_node(pPos, pProp)
{
    Triangle tri;
    svt_mesh *pArrowOutline;
    svt_rectangle *pCorrBarOutline;

    setAutoDL(false); // andauerndes durchlaufen der routine drawGl, damit Pfeile neugezeichnet werden

    m_zOffset = 0.;
    m_xOffsetTorque = 0.;
    m_yOffsetTorque = -1.1;
    m_xOffsetCorr = -1.;
    m_yOffsetCorr = -3.5;
    m_heightCorr = 3.0;
    m_widthCorr = 0.5;
    m_heightMaxFlag = 0.2;

    m_Corr = m_CorrMax = 0.0;

    lenMin = 0.01;

    // Properties for node and their surface
    getProperties()->setLighting(false);
    getProperties()->setSolid(true);
    getProperties()->setWireframe(false);

    // Properties for Outline
    svt_properties* pPropOutline = new svt_properties(new svt_color(0., 0., 0.)); // black
    pPropOutline->setLighting(false);
    pPropOutline->setSolid(true);
    pPropOutline->setWireframe(true);
    pPropOutline->setWireframeColor(pProp->getColor());

    // Correlation Bar -------------------------------------------------------------
    svt_properties* pPropCorr = new svt_properties(new svt_color(getProperties()->getColor()->getR(), 
                                                                 getProperties()->getColor()->getG(), 
                                                                 getProperties()->getColor()->getB()));
    pPropCorr->setLighting(getProperties()->getLighting());
    pPropCorr->setSolid(getProperties()->getSolid());
    pPropCorr->setWireframe(getProperties()->getWireframe());
    m_pCorrBar = new svt_rectangle(new svt_pos(m_xOffsetCorr, m_yOffsetCorr, m_zOffset), pPropCorr,
                                   m_widthCorr, m_heightCorr);    

    add(m_pCorrBar);


    // Correlation Bar Outline -----------------------------------------------------
    pCorrBarOutline = new svt_rectangle(new svt_pos(m_xOffsetCorr, m_yOffsetCorr, m_zOffset), pPropOutline, 
                                          m_widthCorr, m_heightCorr);

    add(pCorrBarOutline);

    // Correlation Bar Max Flag ----------------------------------------------------
    svt_properties* pPropMaxFlag = new svt_properties(new svt_color(1., 0., 0.));
    pPropMaxFlag->setLighting(getProperties()->getLighting());
    pPropMaxFlag->setSolid(getProperties()->getSolid());
    pPropMaxFlag->setWireframe(getProperties()->getWireframe());
    m_pCorrMaxFlag = new svt_rectangle(new svt_pos(m_xOffsetCorr, m_yOffsetCorr+m_heightCorr/2.+m_heightMaxFlag/2., 
                                                   m_zOffset), 
                                       pPropMaxFlag, 
                                       m_widthCorr, m_heightMaxFlag);
    m_pCorrMaxFlag->setVisible(false);

    add(m_pCorrMaxFlag);

    // TorqueArrows ----------------------------------------------------------------

    m_pTorqueArrows = new svt_mesh(pPos, pProp);

    // left
    tri.m_fP[0][0]= -0.1 + m_xOffsetTorque;  tri.m_fP[0][1]= -0.05 + m_yOffsetTorque; tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]= -0.5 + m_xOffsetTorque;  tri.m_fP[1][1]= 0. + m_yOffsetTorque;    tri.m_fP[1][2]= m_zOffset;
    tri.m_fP[2][0]= -0.1 + m_xOffsetTorque;  tri.m_fP[2][1]= 0.05 + m_yOffsetTorque;  tri.m_fP[2][2]= m_zOffset;
    m_pTorqueArrows->addTriangle(tri);

    // right
    tri.m_fP[0][0]= 0.1 + m_xOffsetTorque;   tri.m_fP[0][1]= -0.05 + m_yOffsetTorque; tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]= 0.5 + m_xOffsetTorque;   tri.m_fP[1][1]= 0. + m_yOffsetTorque;    tri.m_fP[1][2]= m_zOffset;
    tri.m_fP[2][0]= 0.1 + m_xOffsetTorque;   tri.m_fP[2][1]= 0.05 + m_yOffsetTorque;  tri.m_fP[2][2]= m_zOffset;
    m_pTorqueArrows->addTriangle(tri);

    // up
    tri.m_fP[0][0]= -0.05 + m_xOffsetTorque; tri.m_fP[0][1]= 0.1 + m_yOffsetTorque;   tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]= 0. + m_xOffsetTorque;    tri.m_fP[1][1]= 0.5 + m_yOffsetTorque;   tri.m_fP[1][2]= m_zOffset;
    tri.m_fP[2][0]= 0.05 + m_xOffsetTorque;  tri.m_fP[2][1]= 0.1 + m_yOffsetTorque;   tri.m_fP[2][2]= m_zOffset;
    m_pTorqueArrows->addTriangle(tri);

    // down
    tri.m_fP[0][0]= -0.05 + m_xOffsetTorque; tri.m_fP[0][1]= -0.1 + m_yOffsetTorque;  tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]= 0. + m_xOffsetTorque;    tri.m_fP[1][1]= -0.5 + m_yOffsetTorque;  tri.m_fP[1][2]= m_zOffset;
    tri.m_fP[2][0]= 0.05 + m_xOffsetTorque;  tri.m_fP[2][1]= -0.1 + m_yOffsetTorque;  tri.m_fP[2][2]= m_zOffset;
    m_pTorqueArrows->addTriangle(tri);

    // clock
    tri.m_fP[0][0]= 0.04 + m_xOffsetTorque;  tri.m_fP[0][1]= 0.39 + m_yOffsetTorque;  tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]= 0.11 + m_xOffsetTorque;  tri.m_fP[1][1]= 0.47 + m_yOffsetTorque;  tri.m_fP[1][2]= m_zOffset;
    tri.m_fP[2][0]= 0.43 + m_xOffsetTorque;  tri.m_fP[2][1]= 0.07 + m_yOffsetTorque;  tri.m_fP[2][2]= m_zOffset;
    m_pTorqueArrows->addTriangle(tri);

    // counterClock
    tri.m_fP[0][0]= -0.04 + m_xOffsetTorque; tri.m_fP[0][1]= 0.39 + m_yOffsetTorque;  tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]= -0.11 + m_xOffsetTorque; tri.m_fP[1][1]= 0.47 + m_yOffsetTorque;  tri.m_fP[1][2]= m_zOffset;
    tri.m_fP[2][0]= -0.43 + m_xOffsetTorque; tri.m_fP[2][1]= 0.07 + m_yOffsetTorque;  tri.m_fP[2][2]= m_zOffset;
    m_pTorqueArrows->addTriangle(tri);

    add(m_pTorqueArrows);

    // Torque Outline

    pArrowOutline = new svt_mesh(pPos, pPropOutline);

    // left
    tri.m_fP[0][0]= -0.1 + m_xOffsetTorque;  tri.m_fP[0][1]= -0.05 + m_yOffsetTorque; tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]= -0.5 + m_xOffsetTorque;  tri.m_fP[1][1]=  0.   + m_yOffsetTorque; tri.m_fP[1][2]= m_zOffset;
    tri.m_fP[2][0]= -0.1 + m_xOffsetTorque;  tri.m_fP[2][1]= 0.05  + m_yOffsetTorque; tri.m_fP[2][2]= m_zOffset;
    pArrowOutline->addTriangle(tri);

    // right
    tri.m_fP[0][0]= 0.1 + m_xOffsetTorque;   tri.m_fP[0][1]= -0.05 + m_yOffsetTorque; tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]= 0.5 + m_xOffsetTorque;   tri.m_fP[1][1]=  0.   + m_yOffsetTorque; tri.m_fP[1][2]= m_zOffset;
    tri.m_fP[2][0]= 0.1 + m_xOffsetTorque;   tri.m_fP[2][1]=  0.05 + m_yOffsetTorque; tri.m_fP[2][2]= m_zOffset;
    pArrowOutline->addTriangle(tri);

    // up
    tri.m_fP[0][0]= -0.05 + m_xOffsetTorque; tri.m_fP[0][1]= 0.1 + m_yOffsetTorque;   tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]= 0. + m_xOffsetTorque;    tri.m_fP[1][1]= 0.5 + m_yOffsetTorque;   tri.m_fP[1][2]= m_zOffset;
    tri.m_fP[2][0]= 0.05 + m_xOffsetTorque;  tri.m_fP[2][1]= 0.1 + m_yOffsetTorque;   tri.m_fP[2][2]= m_zOffset;
    pArrowOutline->addTriangle(tri);

    // down
    tri.m_fP[0][0]= -0.05 + m_xOffsetTorque; tri.m_fP[0][1]= -0.1 + m_yOffsetTorque;  tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]= 0. + m_xOffsetTorque;    tri.m_fP[1][1]= -0.5 + m_yOffsetTorque;  tri.m_fP[1][2]= m_zOffset;
    tri.m_fP[2][0]= 0.05 + m_xOffsetTorque;  tri.m_fP[2][1]= -0.1 + m_yOffsetTorque;  tri.m_fP[2][2]= m_zOffset;
    pArrowOutline->addTriangle(tri);

    // clock
    tri.m_fP[0][0]= 0.04 + m_xOffsetTorque;  tri.m_fP[0][1]= 0.39 + m_yOffsetTorque;  tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]= 0.11 + m_xOffsetTorque;  tri.m_fP[1][1]= 0.47 + m_yOffsetTorque;  tri.m_fP[1][2]= m_zOffset;
    tri.m_fP[2][0]= 0.43 + m_xOffsetTorque;  tri.m_fP[2][1]= 0.07 + m_yOffsetTorque;  tri.m_fP[2][2]= m_zOffset;
    pArrowOutline->addTriangle(tri);

    // counterClock
    tri.m_fP[0][0]= -0.04 + m_xOffsetTorque; tri.m_fP[0][1]= 0.39 + m_yOffsetTorque;  tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]= -0.11 + m_xOffsetTorque; tri.m_fP[1][1]= 0.47 + m_yOffsetTorque;  tri.m_fP[1][2]= m_zOffset;
    tri.m_fP[2][0]= -0.43 + m_xOffsetTorque; tri.m_fP[2][1]= 0.07 + m_yOffsetTorque;  tri.m_fP[2][2]= m_zOffset;
    pArrowOutline->addTriangle(tri);

    add(pArrowOutline);

    // TransArrows ----------------------------------------------------------------

    m_pTransArrows = new svt_mesh(pPos, pProp);

    // left
    tri.m_fP[0][0]= -0.1;  tri.m_fP[0][1]= -0.05; tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]= -0.5;  tri.m_fP[1][1]= 0.;    tri.m_fP[1][2]= m_zOffset;
    tri.m_fP[2][0]= -0.1;  tri.m_fP[2][1]= 0.05;  tri.m_fP[2][2]= m_zOffset;
    m_pTransArrows->addTriangle(tri);

    // right
    tri.m_fP[0][0]= 0.1;   tri.m_fP[0][1]= -0.05; tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]= 0.5;   tri.m_fP[1][1]= 0.;    tri.m_fP[1][2]= m_zOffset;
    tri.m_fP[2][0]= 0.1;   tri.m_fP[2][1]= 0.05;  tri.m_fP[2][2]= m_zOffset;
    m_pTransArrows->addTriangle(tri);

    // up
    tri.m_fP[0][0]= -0.05; tri.m_fP[0][1]= 0.1;   tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]= 0.;    tri.m_fP[1][1]= 0.5;   tri.m_fP[1][2]= m_zOffset;
    tri.m_fP[2][0]= 0.05;  tri.m_fP[2][1]= 0.1;   tri.m_fP[2][2]= m_zOffset;
    m_pTransArrows->addTriangle(tri);

    // down
    tri.m_fP[0][0]= -0.05; tri.m_fP[0][1]= -0.1;  tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]= 0.;    tri.m_fP[1][1]= -0.5;  tri.m_fP[1][2]= m_zOffset;
    tri.m_fP[2][0]= 0.05;  tri.m_fP[2][1]= -0.1;  tri.m_fP[2][2]= m_zOffset;
    m_pTransArrows->addTriangle(tri);

    // forward
    tri.m_fP[0][0]= -0.075; tri.m_fP[0][1]= -0.025; tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]= -0.2;   tri.m_fP[1][1]= -0.25;  tri.m_fP[1][2]= m_zOffset; //+ 0.01
    tri.m_fP[2][0]=  0.025; tri.m_fP[2][1]= -0.025; tri.m_fP[2][2]= m_zOffset;
    m_pTransArrows->addTriangle(tri);

    // backward
    tri.m_fP[0][0]= -0.025; tri.m_fP[0][1]= 0.025; tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]=  0.075; tri.m_fP[1][1]= 0.025; tri.m_fP[1][2]= m_zOffset;
    tri.m_fP[2][0]=  0.2;   tri.m_fP[2][1]= 0.25;  tri.m_fP[2][2]= m_zOffset; // -0.01
    m_pTransArrows->addTriangle(tri);

    add(m_pTransArrows);

    // Trans Outline

    pArrowOutline = new svt_mesh(pPos, pPropOutline);

    // left
    tri.m_fP[0][0]= -0.1;  tri.m_fP[0][1]= -0.05; tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]= -0.5;  tri.m_fP[1][1]= 0.;    tri.m_fP[1][2]= m_zOffset;
    tri.m_fP[2][0]= -0.1;  tri.m_fP[2][1]= 0.05;  tri.m_fP[2][2]= m_zOffset;
    pArrowOutline->addTriangle(tri);

    // right
    tri.m_fP[0][0]= 0.1;   tri.m_fP[0][1]= -0.05; tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]= 0.5;   tri.m_fP[1][1]= 0.;    tri.m_fP[1][2]= m_zOffset;
    tri.m_fP[2][0]= 0.1;   tri.m_fP[2][1]= 0.05;  tri.m_fP[2][2]= m_zOffset;
    pArrowOutline->addTriangle(tri);

    // up
    tri.m_fP[0][0]= -0.05; tri.m_fP[0][1]= 0.1;   tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]= 0.;    tri.m_fP[1][1]= 0.5;   tri.m_fP[1][2]= m_zOffset;
    tri.m_fP[2][0]= 0.05;  tri.m_fP[2][1]= 0.1;   tri.m_fP[2][2]= m_zOffset;
    pArrowOutline->addTriangle(tri);

    // down
    tri.m_fP[0][0]= -0.05; tri.m_fP[0][1]= -0.1;  tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]= 0.;    tri.m_fP[1][1]= -0.5;  tri.m_fP[1][2]= m_zOffset;
    tri.m_fP[2][0]= 0.05;  tri.m_fP[2][1]= -0.1;  tri.m_fP[2][2]= m_zOffset;
    pArrowOutline->addTriangle(tri);

    // forward
    tri.m_fP[0][0]= -0.075; tri.m_fP[0][1]= -0.025; tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]= -0.2;   tri.m_fP[1][1]= -0.25;  tri.m_fP[1][2]= m_zOffset; //+ 0.01
    tri.m_fP[2][0]=  0.025; tri.m_fP[2][1]= -0.025; tri.m_fP[2][2]= m_zOffset;
    pArrowOutline->addTriangle(tri);

    // backward
    tri.m_fP[0][0]= -0.025; tri.m_fP[0][1]= 0.025; tri.m_fP[0][2]= m_zOffset;
    tri.m_fP[1][0]=  0.075; tri.m_fP[1][1]= 0.025; tri.m_fP[1][2]= m_zOffset;
    tri.m_fP[2][0]=  0.2;   tri.m_fP[2][1]= 0.25;  tri.m_fP[2][2]= m_zOffset; //- 0.01
    pArrowOutline->addTriangle(tri);

    add(pArrowOutline);
};

svt_forceArrows::~svt_forceArrows()
{
};

/**
 * reset the values for the correlation bar
 */
void svt_forceArrows::corrReset()
{
    m_CorrMax = m_Corr = 0.0;
    m_pCorrMaxFlag->setVisible(false);
}

/**
 * set just the max value for the correlation
 * 
 * \param co actual correlation value
 */
void svt_forceArrows::setCorrMaxValue(double co)
{
    m_CorrMax = co;
}

/**
 * set the max value for the correlation bar, if co is greater than the max before
 * 
 * \param co actual correlation value
 */
void svt_forceArrows::setCorrMax(double co)
{
    static int maxFlagShow = 100;
    static int iMaxFlag = maxFlagShow;

    if (co >= m_CorrMax)
    {
        m_pCorrMaxFlag->setVisible(true);
        iMaxFlag=0;
        m_pCorrBar->getProperties()->getColor()->set(1., 0., 0.);
    }
    else if (co >= (m_CorrMax-m_CorrMax/10.))
        m_pCorrBar->getProperties()->getColor()->set(1., 10.*(1.-co/m_CorrMax), 0.); // green: 0...1.
    else
        m_pCorrBar->getProperties()->getColor()->set(getProperties()->getColor()->getR(), 
                                                     getProperties()->getColor()->getG(), 
                                                     getProperties()->getColor()->getB());
    if (co > m_CorrMax)
        setCorrMaxValue(co);

    if (iMaxFlag < maxFlagShow)
    {
        iMaxFlag++;
        if (iMaxFlag == (maxFlagShow-1))
            m_pCorrMaxFlag->setVisible(false);
    }
}

/**
 * set the correlation bar to co
 * set also the max correlation, if it increases the old max value
 * 
 * \param co actual correlation value
 */
void svt_forceArrows::setCorr(double co)
{
    if (co < 0.)
        co = 0.;

    setCorrMax(co);

    if (m_CorrMax != 0.0)
        m_pCorrBar->setHeight(m_heightCorr*(co/m_CorrMax));
    else
        m_pCorrBar->setHeight(m_heightCorr);

    m_pCorrBar->getPos()->setY(m_yOffsetCorr-m_heightCorr*(1.-(co/m_CorrMax))/2.);
}

/**
 * set the length of force arrows
 * 
 * \param oTorqueVec torsion vector -100..100
 */
void svt_forceArrows::setTorqueArrowLength(svt_vector4<double> oTorqueVec)
{
    if (oTorqueVec.x() > 0.) {
        setTorqueArrowLength(downArrow, oTorqueVec.x()/100.);
        setTorqueArrowLength(upArrow, 0.);
    } else {
        setTorqueArrowLength(downArrow, 0.);
        setTorqueArrowLength(upArrow, -oTorqueVec.x()/100.);
    }
    if (oTorqueVec.y() > 0.) {
        setTorqueArrowLength(rightArrow, oTorqueVec.y()/100.);
        setTorqueArrowLength(leftArrow, 0.);
    } else {
        setTorqueArrowLength(rightArrow, 0.);
        setTorqueArrowLength(leftArrow, -oTorqueVec.y()/100.);
    }
    if (oTorqueVec.z() > 0.) {
        setTorqueArrowLength(counterClockArrow, oTorqueVec.z()/100.);
        setTorqueArrowLength(clockArrow, 0.);
    } else {
        setTorqueArrowLength(counterClockArrow, 0.);
        setTorqueArrowLength(clockArrow, -oTorqueVec.z()/100.);
    }
}
/**
 * set the length of force arrows
 * 
 * \param i arrow number (0..5)
 * \param len new length of the arrow (0..1)
 */
void svt_forceArrows::setTorqueArrowLength(enum arrowType aT, double len)
{
    static double lenMin = 0.01;

    if (len > 1)
        len = 1;
    // len = sqrt(len); // will increase arrow length with lower len

    switch (aT) {
        case leftArrow:
            if (len < lenMin)
                (*(m_pTorqueArrows->getTriangleArray()))[0].in = false;
            else
            {
                (*(m_pTorqueArrows->getTriangleArray()))[0].in = true;
                (*(m_pTorqueArrows->getVertexArray()))[(*(m_pTorqueArrows->getTriangleArray()))[0].ver[1]].pkt[0] = -0.1 - 0.4*len + m_xOffsetTorque;
            }
            break;
        case rightArrow:
            if (len < lenMin)
                (*(m_pTorqueArrows->getTriangleArray()))[1].in = false;
            else
            {
                (*(m_pTorqueArrows->getTriangleArray()))[1].in = true;
                (*(m_pTorqueArrows->getVertexArray()))[(*(m_pTorqueArrows->getTriangleArray()))[1].ver[1]].pkt[0] =  0.1 + 0.4*len + m_xOffsetTorque;
            }
            break;
        case upArrow:
            if (len < lenMin)
                (*(m_pTorqueArrows->getTriangleArray()))[2].in = false;
            else
            {
                (*(m_pTorqueArrows->getTriangleArray()))[2].in = true;
                (*(m_pTorqueArrows->getVertexArray()))[(*(m_pTorqueArrows->getTriangleArray()))[2].ver[1]].pkt[1] =  0.1 + 0.4*len + m_yOffsetTorque;
            }
            break;
        case downArrow:
            if (len < lenMin)
                (*(m_pTorqueArrows->getTriangleArray()))[3].in = false;
            else
            {
                (*(m_pTorqueArrows->getTriangleArray()))[3].in = true;
                (*(m_pTorqueArrows->getVertexArray()))[(*(m_pTorqueArrows->getTriangleArray()))[3].ver[1]].pkt[1] = -0.1 - 0.4*len + m_yOffsetTorque;
            }
            break;
        case clockArrow:
            if (len < lenMin)
                (*(m_pTorqueArrows->getTriangleArray()))[4].in = false;
            else
            {
                (*(m_pTorqueArrows->getTriangleArray()))[4].in = true;
                (*(m_pTorqueArrows->getVertexArray()))[(*(m_pTorqueArrows->getTriangleArray()))[4].ver[2]].pkt[0] =  0.075 + m_xOffsetTorque + 0.355*len;
                (*(m_pTorqueArrows->getVertexArray()))[(*(m_pTorqueArrows->getTriangleArray()))[4].ver[2]].pkt[1] =  0.43  + m_yOffsetTorque - 0.36*len;
            }
            break;
        case counterClockArrow:
            if (len < lenMin)
                (*(m_pTorqueArrows->getTriangleArray()))[5].in = false;
            else
            {
                (*(m_pTorqueArrows->getTriangleArray()))[5].in = true;
                (*(m_pTorqueArrows->getVertexArray()))[(*(m_pTorqueArrows->getTriangleArray()))[5].ver[2]].pkt[0] = -0.075 + m_xOffsetTorque - 0.355*len;
                (*(m_pTorqueArrows->getVertexArray()))[(*(m_pTorqueArrows->getTriangleArray()))[5].ver[2]].pkt[1] =  0.43  + m_yOffsetTorque - 0.36*len;
            }
            break;
        case forwardArrow:
        case backwardArrow:
        default:
            break;
    }
}

/**
 * set the length of transition force arrows
 * 
 * \param oTorqueVec torsion vector -100..100 maximal component size
 */
void svt_forceArrows::setTransArrowLength(svt_vector4<double> oTransVec)
{
    if (oTransVec.x() > 0.) {
        setTransArrowLength(rightArrow, oTransVec.x()/100.);
        setTransArrowLength(leftArrow, 0.);
    } else {
        setTransArrowLength(rightArrow, 0.);
        setTransArrowLength(leftArrow, -oTransVec.x()/100.);
    }
    if (oTransVec.y() > 0.) {
        setTransArrowLength(upArrow, oTransVec.y()/100.);
        setTransArrowLength(downArrow, 0.);
    } else {
        setTransArrowLength(upArrow, 0.);
        setTransArrowLength(downArrow, -oTransVec.y()/100.);
    }
    if (oTransVec.z() > 0.) {
        setTransArrowLength(forwardArrow, oTransVec.z()/100.);
        setTransArrowLength(backwardArrow, 0.);
    } else {
        setTransArrowLength(forwardArrow, 0.);
        setTransArrowLength(backwardArrow, -oTransVec.z()/100.);
    }
}
/**
 * set the length of transition force arrow
 * 
 * \param i arrow number (0..3,6,7)
 * \param len new length of the arrow (lenMin..1)
 */
void svt_forceArrows::setTransArrowLength(enum arrowType aT, double len)
{
    if (len > 1)
        len = 1; 
    switch (aT) {
        case leftArrow:
            if (len < lenMin)
                (*(m_pTransArrows->getTriangleArray()))[0].in = false;
            else
            {
                (*(m_pTransArrows->getTriangleArray()))[0].in = true;
                (*(m_pTransArrows->getVertexArray()))[(*(m_pTransArrows->getTriangleArray()))[0].ver[1]].pkt[0] = -0.1 - 0.4*len;
            }
            break;
        case rightArrow:
            if (len < lenMin)
                (*(m_pTransArrows->getTriangleArray()))[1].in = false;
            else
            {
                (*(m_pTransArrows->getTriangleArray()))[1].in = true;
                (*(m_pTransArrows->getVertexArray()))[(*(m_pTransArrows->getTriangleArray()))[1].ver[1]].pkt[0] =  0.1 + 0.4*len;
            }
            break;
        case upArrow:
            if (len < lenMin)
                (*(m_pTransArrows->getTriangleArray()))[2].in = false;
            else
            {
                (*(m_pTransArrows->getTriangleArray()))[2].in = true;
                (*(m_pTransArrows->getVertexArray()))[(*(m_pTransArrows->getTriangleArray()))[2].ver[1]].pkt[1] =  0.1 + 0.4*len;
            }
            break;
        case downArrow:
            if (len < lenMin)
                (*(m_pTransArrows->getTriangleArray()))[3].in = false;
            else
            {
                (*(m_pTransArrows->getTriangleArray()))[3].in = true;
                (*(m_pTransArrows->getVertexArray()))[(*(m_pTransArrows->getTriangleArray()))[3].ver[1]].pkt[1] = -0.1 - 0.4*len;
            }
            break;
        case forwardArrow:
            if (len < lenMin)
                (*(m_pTransArrows->getTriangleArray()))[4].in = false;
            else
            {
                (*(m_pTransArrows->getTriangleArray()))[4].in = true;
                (*(m_pTransArrows->getVertexArray()))[(*(m_pTransArrows->getTriangleArray()))[4].ver[1]].pkt[0] = -0.025 - 0.175*len;
                (*(m_pTransArrows->getVertexArray()))[(*(m_pTransArrows->getTriangleArray()))[4].ver[1]].pkt[1] = -0.025 - 0.225*len;
            }
            break;
        case backwardArrow:
            if (len < lenMin)
                (*(m_pTransArrows->getTriangleArray()))[5].in = false;
            else
            {
                (*(m_pTransArrows->getTriangleArray()))[5].in = true;
                (*(m_pTransArrows->getVertexArray()))[(*(m_pTransArrows->getTriangleArray()))[5].ver[2]].pkt[0] = 0.025 + 0.175*len;
                (*(m_pTransArrows->getVertexArray()))[(*(m_pTransArrows->getTriangleArray()))[5].ver[2]].pkt[1] = 0.025 + 0.225*len;
            }
            break;
        case clockArrow:
        case counterClockArrow:
        default:
            break;
    }
}

/**
 * draw the arrows
 */
void svt_forceArrows::drawGL()
{
    m_pTorqueArrows->rebuildDL();
    m_pTransArrows->rebuildDL();
    m_pCorrBar->rebuildDL();
}
