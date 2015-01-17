/********************************************************************
 *                                                                  *
 *  file: svt_sky.h                                                 *
 *                                                                  *
 *  date: aug-24-02                                                 *
 *                                                                  *
 *  f.delonge                                                       *
 *                                                                  *
 ********************************************************************/


#ifndef SVT_SKY
#define SVT_SKY

#include <svt_cmath.h>
#include <svt_contextSpecific.h>
#include <svt_node.h>
#include <svt_texture.h>
#include <svt_triFan.h>
#include <svt_triStrip.h>

/**
 *  A class to produce a 32-sided sky dome that covers the scene
 *	\author Frank Delonge
 */
class DLLEXPORT svt_sky : public svt_node {

private:

    svt_sky(const svt_sky&) : svt_node() {};

    bool m_bGround;

    Real32 m_fRadiusX;
    Real32 m_fRadiusY;
    Real32 m_fHeight;
    svt_texture m_oTexture;

    svt_properties m_oPrivateProp;

    svt_canvasSpecific<svt_triFan> m_oTriFan;
    svt_canvasSpecific<svt_triStrip> m_oTriStrip;

public:

    svt_sky(Real32 fRadiusX=1.0, Real32 fRadiusY=1.0, Real32 fHeight=1.0, bool bGround=false);
    svt_sky(svt_node* pParentNode, Real32 fRadiusX=1.0, Real32 fRadiusY=1.0, Real32 fHeight=1.0, bool bGround=false);

    virtual ~svt_sky() {};

    virtual void drawGL();

    virtual void drawGL_Normals();

    virtual void printName();

    Real32 radiusX() const {return m_fRadiusX;}
    Real32 radiusY() const {return m_fRadiusY;}
    Real32 height() const {return m_fHeight;}

    void setHeight(Real32 fHeight)
    {
        m_fHeight=fHeight; rebuildDL();
    }

    void setRadius(Real32 fRadiusX, Real32 fRadiusY)
    {
        m_fRadiusX=fRadiusX;
        m_fRadiusY=fRadiusY;
        rebuildDL();
    }

    void setGround(bool b)
    {
        m_bGround=b;
    }
    bool ground() const
    {
        return m_bGround;
    }
};


#endif
