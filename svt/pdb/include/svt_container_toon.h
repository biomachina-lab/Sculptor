/*-*-c++-*-*****************************************************************
                          svt_container_toon
                          -------------
    begin                : 12/8/2008
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_CONTAINER_TOON_H
#define SVT_CONTAINER_TOON_H

// svt includes
#include <svt_basics.h>
#include <svt_stlVector.h>
#include <svt_node.h>
#include <svt_vbo.h>
class svt_shader_cg_toon;
class svt_scene;

struct toon_v4f_t4f_c4f_n3f_64byte
{
    Real32  v[4];
    Real32  t[4];
    Real32  c[4];
    Real32  n[3];
    Real32  pad;   // padding byte (a multiple of 32 is always good)
};

enum {
    SVT_PDB_VDW_NO_SHADER = 1,
    SVT_PDB_VDW_GLOSSY    = 2
};

/**
 * \brief This is a container class for primitives for the toon rendering mode.
 *
 * It also takes care of VDW high quality rendering via vertex and pixel shaders
 * \author Manuel Wahle
 */
class DLLEXPORT svt_container_toon : public svt_node
{
    static SVT_PFNGLDRAWELEMENTSEXTPROC glDrawElementsEXT;

    // are VBOs supported?
    static bool m_bVBO_Supported;

    // are toons at all supported?
    static bool m_bToonSupported;

    // VBO already generated?
    bool m_bVBO_Generated;

    // number of spheres
    unsigned int m_iNumIndices;

    // pointer to the svt_scene
    svt_scene * m_pScene;

    // a temporary
    toon_v4f_t4f_c4f_n3f_64byte m_oV;

    // vector will all the quads (the sphere billboards)
    vector< toon_v4f_t4f_c4f_n3f_64byte > m_aQuads;
    // vector with the quad indices
    vector< unsigned int > m_aIndices;

    // lighting parameters
    svt_vector3<Real32> m_oLighting;
    svt_vector3<Real32> m_oSpecularColor;

    // the VBO for vertices and indices
    svt_vbo m_oVertexBuffer;
    svt_vbo m_oIndexBuffer;

    // the shader programs
    svt_shader_cg_toon * m_pShaderFrag;
    svt_shader_cg_toon * m_pShaderVertex;
    svt_shader_cg_toon * m_pShaderFragment;
    svt_shader_cg_toon * m_pShaderFragment_FogLinear;
    svt_shader_cg_toon * m_pShaderFragment_FogExp;
    svt_shader_cg_toon * m_pShaderFragment_FogExp2;

    // are shaders initialized?
    bool m_bShaderInitialized;


public:


    /**
     * Constructor
     */
    svt_container_toon();

    /**
     * Destructor
     */
    virtual ~svt_container_toon();

    /**
     * check if shader programs are supported and set the internal boolean to indicate this.
     * (needs vertex profile ARBVP1 and fragment profile ARBFP1).
     * \return true if shaders are supported
     */
    bool checkShaderSupport();

    /**
     * check if toons are supported
     * \return boolean true if toons are supported, false otherwise
     */
    bool getToonSupported();

    /**
     * Add an atom
     */
    inline void addAtom( const toon_v4f_t4f_c4f_n3f_64byte * oV )
    {
        m_bVBO_Generated = false;

        memcpy(m_oV.v, oV->v, 16);
        memcpy(m_oV.c, oV->c, 16);

        m_oV.t[0] =  1.0f;
        m_oV.t[1] =  1.0f;
        m_aQuads.push_back(m_oV);

        m_oV.t[0] = -1.0f;
        m_aQuads.push_back(m_oV);

        m_oV.t[0] = -1.0f;
        m_oV.t[1] = -1.0f;
        m_aQuads.push_back(m_oV);

        m_oV.t[0] =  1.0f;
        m_aQuads.push_back(m_oV);

        if (m_bVBO_Supported)
        {
            m_aIndices.push_back(m_iNumIndices++);
            m_aIndices.push_back(m_iNumIndices++);
            m_aIndices.push_back(m_iNumIndices++);
            m_aIndices.push_back(m_iNumIndices++);
        }
    };

    /**
     * delete all elements
     */
    void delAll();

    /**
     * draw the content of the container (OpenGL)
     */
    virtual void drawGL();

    /**
     * draw the content of the container (Alias/Wavefront)
     */
    virtual void drawOBJ();

private:

    /**
     * this method initializes the shader programs
     */
    virtual void initShader();
};

#endif
