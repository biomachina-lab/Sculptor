/*-*-c++-*-*****************************************************************
  svt_coord_system
  --------
  begin                : 04/25/2008
  author               : Manuel Wahle
  email                : sculptor@biomachina.org
***************************************************************************/

#ifndef SVT_COORD_SYSTEM
#define SVT_COORD_SYSTEM

#include <svt_node.h>
#include <svt_matrix4.h>
#include <svt_opengl.h>

/**
 * A little coordinate system that shows rotation
 */
class DLLEXPORT svt_coord_system : public svt_node
{

protected:

    svt_matrix4< Real32 > m_oMatrix;
    GLUquadricObj* m_pQuadric;

    Real32 m_fLength;
    GLdouble m_dX;
    GLdouble m_dY;
    GLdouble m_dZ;
    GLdouble * m_aModelviewMatrix;   // 16 GLdouble
    GLdouble * m_aProjectionMatrix;  // 16 GLdouble
    GLint * m_aViewport;             //  4 GLint

public:

    /**
     * Constructor
     */
    svt_coord_system();

    /**
     * Destructor
     */
    virtual ~svt_coord_system();

    /**
     * draw the coordinate system
     */
    void drawGL();
};

#endif // SVT_COORD_SYSTEM
