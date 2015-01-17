/***************************************************************************
                          svt_pipette.h  -  description
                             -------------------
    begin                : 26.04.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_PIPETTE_H
#define SVT_PIPETTE_H

#include <svt_core.h>
#include <svt_solid_cone.h>
#include <svt_rotation.h>
#include <svt_const.h>

/**
 * A pipette
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_pipette : public svt_node
{
protected:
    svt_solid_cone* m_pCone1;
    svt_solid_cone* m_pCone2;

public:
    /**
     * Constructor
     */
    svt_pipette(svt_pos* pPos=NULL, svt_properties* pProp=NULL);
    virtual ~svt_pipette();

    /**
     * set the properties object.
     * \param pProp pointer to svt_properties object
     */
    virtual void setProperties(svt_properties* pProp);
};

#endif
