/***************************************************************************
                          svt_shadow_plane.h
                          -----------------
    begin                : 6/7/2003
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_SHADOW_NODE_H
#define SVT_SHADOW_NODE_H

#include <svt_node.h>
class svt_lightsource;

class svt_shadow_plane : public svt_node
{
protected:
    svt_node* m_pShadowNode;
    svt_lightsource* m_pLight;
    Real32 m_fWidth;
    Real32 m_fLength;

public:
    svt_shadow_plane( svt_node* pNode, svt_lightsource* pLight );
    virtual ~svt_shadow_plane(){};

    virtual void drawGL();

    inline void setWidth( Real32 fWidth ){ m_fWidth = fWidth; };
    inline Real32 getWidth(){ return m_fWidth; };
    inline void setLength( Real32 fLength ){ m_fLength = fLength; };
    inline Real32 getLength(){ return m_fLength; };
};

#endif
