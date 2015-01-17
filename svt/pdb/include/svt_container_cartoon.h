/*-*-c++-*-*****************************************************************
                          svt_container_cartoon
                          -------------
    begin                : 12/05/2006
    author               : Manuel Wahle, Stefan Birmanns
    email                : sculptor@biomachina.org, sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_CONTAINER_CARTOON_H
#define SVT_CONTAINER_CARTOON_H

#include <svt_vbo.h>
#include <svt_node.h>
#include <svt_scene.h>
#include <svt_stlVector.h>
#include <svt_vertinterl.h>
#include <svt_shader_cg_tube.h>
#include <svt_shader_cg_cartoon.h>
#include <svt_shader_cg_cartoon_fragment.h>

#define CARTOON_SCALING_FACTOR 0.006f

class svt_pdb;

enum EnumCartoonParam {
    CARTOON_USE_SHADER,
    CARTOON_PEPTIDE_PLANES,
    CARTOON_SHEET_MIN_RESIDUES,
    CARTOON_SHEET_WIDTH,
    CARTOON_SHEET_HEIGHT,
    CARTOON_HELIX_MIN_RESIDUES,
    CARTOON_HELIX_WIDTH,
    CARTOON_HELIX_HEIGHT,
    CARTOON_TUBE_DIAMETER,
    CARTOON_HELIX_CYLINDER,
    CARTOON_HELIX_ARROWHEADS,
    CARTOON_SPLINE_SEGMENTS,
    CARTOON_PER_PIXEL_LIGHTING,
    CARTOON_SHEET_HELIX_PROFILE_CORNERS
};

///////////////////////////////////////////////////////////////////////////////
// svt_container_cartoon
///////////////////////////////////////////////////////////////////////////////

/**
 * \brief This is an abstract base class for primitives for the cartoon rendering mode.
 *
 * Classes doing the acutal cartoon rendering inherit from this class.
 * \author Manuel Wahle
 */
class DLLEXPORT svt_container_cartoon : public svt_node
{

protected:

    // use all the efficient goodies only if not in Windows...
#ifndef WIN32
    static SVT_PFNGLDRAWRANGEELEMENTSEXTPROC glDrawRangeElementsEXT;
    static SVT_PFNGLMULTIDRAWELEMENTSEXTPROC glMultiDrawElementsEXT;
    static SVT_PFNGLMULTIDRAWARRAYSEXTPROC   glMultiDrawArraysEXT;
#else
    static SVT_PFNGLDRAWELEMENTSEXTPROC      glDrawElementsEXT;
#endif

    bool m_bUseMultiDraw;
    bool m_bVBO_Supported;
    bool m_bVA_Generated;
    bool m_bCaps;
    unsigned int m_iNumVertices;

    svt_pdb * m_pPdb;
    svt_scene * m_pScene;
    svt_vector3< Real32 > m_oViewPos;
    svt_vector3< Real32 > m_oLighting;
    svt_vector3< Real32 > m_oSpecularColor;

    svt_vbo m_oVertexBuffer;
    svt_vbo m_oIndexBuffer;

    vector< unsigned int > m_aIndices;

    bool m_bRegenerateStartIndices;


public:

    struct v4f_c4f_t4f_n3f
    {
        Real32 v[4];
        Real32 c[4];
        Real32 t[4];
        Real32 n[3];
        Real32 pad;
    };

    /**
     * Start a tube
     */
    virtual void beginTube( ) = 0;

    /**
     * End a tube
     */
    virtual void endTube( ) = 0;

    /**
     * Start a sheet
     */
    virtual void beginSheet( ) = 0;

    /**
     * End a sheet
     */
    virtual void endSheet( ) = 0;

    /**
     * Start a helix
     */
    virtual void beginHelix( ) = 0;

    /**
     * End a helix
     */
    virtual void endHelix( ) = 0;

    /**
     * Add a segment to the tube/sheet/helix
     */
    virtual void addSegment( v4f_c4f_t4f_n3f * oV ) = 0;

    /**
     * Constructor
     */
    svt_container_cartoon(svt_pdb * pPdb);

    /**
     * Destructor
     */
    ~svt_container_cartoon();

    /**
     * delete all elements
     */
    virtual void delAll() = 0;

    /**
     * draw the content of the container (OpenGL)
     */
    virtual void drawGL() = 0;

    /**
     * draw the content of the container (Alias/Wavefront)
     */
    virtual void drawOBJ() = 0;
};



///////////////////////////////////////////////////////////////////////////////
// svt_container_cartoon_dynamic
///////////////////////////////////////////////////////////////////////////////



/**
 * \brief This is an implementation of the class for primitives for the cartoon rendering mode using
 * customized vertex and pixel shaders.
 *
 * It uses vertex shaders to speed up the geometry generation process. This is very useful whenever
 * rendering adynamic scene. It can do per vertex and per pixel lighting.
 * \author Manuel Wahle
 */
class DLLEXPORT svt_container_cartoon_dynamic : public svt_container_cartoon
{
    // this static variable counts the number of instances of this class. this is used to prevent
    // the shader programs being created and uploaded multiple times
    static int m_iNumInstances;

    static bool m_bShaderSupported;
    static bool m_bShaderInitialized;

    // define all those variables here, because they are used a lot of times. may be a little faster
    unsigned int i, j, k;
    Real32 fCos, fSin, fX, fY, fZ;
    svt_vector3< Real32 > oT, oTT, oB, oN;
    Real32 * m_oV;

    bool m_bPerPixelLighting;
    unsigned int m_iBeg, m_iEnd;


    vector< GLsizei  > m_aTubeStripSize;
    vector< const GLvoid * > m_aTubeStripStart;
    vector< GLint    > m_aTubeStripStartIndices;

    vector< GLsizei  > m_aSheetStripSize;
    vector< GLvoid * > m_aSheetStripStart;
    vector< GLint    > m_aSheetStripStartIndices;

    vector< GLsizei  > m_aHelixStripSize;
    vector< GLvoid * > m_aHelixStripStart;
    vector< GLint    > m_aHelixStripStartIndices;

    vector< v4f_c4f_t4f_n3f >  m_aVertices;


    // TUBE vertex shader programs
           svt_shader_cg_tube * m_pTubeVertexShader;
    static svt_shader_cg_tube * m_pTubeVertexShader_VertexLighting;
    static svt_shader_cg_tube * m_pTubeVertexShader_VertexLighting_Fog;
    static svt_shader_cg_tube * m_pTubeVertexShader_PixelLighting;

    // CARTOON vertex shader programs
           vector< svt_shader_cg_cartoon * > * m_aCartoonVertexShader;
    static vector< svt_shader_cg_cartoon * >   m_aCartoonVertexShader_VertexLighting;
    static vector< svt_shader_cg_cartoon * >   m_aCartoonVertexShader_VertexLighting_Fog;
    static vector< svt_shader_cg_cartoon * >   m_aCartoonVertexShader_PixelLighting;

           svt_shader_cg_tube_helixsheet * m_pCartoonVertexShaderRound;
    static svt_shader_cg_tube_helixsheet * m_pCartoonVertexShaderRound_VertexLighting;
    static svt_shader_cg_tube_helixsheet * m_pCartoonVertexShaderRound_VertexLighting_Fog;
    static svt_shader_cg_tube_helixsheet * m_pCartoonVertexShaderRound_PixelLighting;

           svt_shader_cg_cartoon * m_pCartoonVertexShaderFlatHelix;
    static svt_shader_cg_cartoon * m_pCartoonVertexShaderFlatHelix_VertexLighting;
    static svt_shader_cg_cartoon * m_pCartoonVertexShaderFlatHelix_VertexLighting_Fog;
    static svt_shader_cg_cartoon * m_pCartoonVertexShaderFlatHelix_PixelLighting;

           svt_shader_cg_cartoon * m_pCartoonVertexShaderFlatSheet;
    static svt_shader_cg_cartoon * m_pCartoonVertexShaderFlatSheet_VertexLighting;
    static svt_shader_cg_cartoon * m_pCartoonVertexShaderFlatSheet_VertexLighting_Fog;
    static svt_shader_cg_cartoon * m_pCartoonVertexShaderFlatSheet_PixelLighting;

    // CARTOON and TUBE fragment shader programs
           svt_shader_cg_cartoon_fragment * m_pFragmentShader;
    static svt_shader_cg_cartoon_fragment * m_pFragmentShader_PixelLighting;
    static svt_shader_cg_cartoon_fragment * m_pFragmentShader_PixelLighting_Fog_Exp;
    static svt_shader_cg_cartoon_fragment * m_pFragmentShader_PixelLighting_Fog_Exp2;
    static svt_shader_cg_cartoon_fragment * m_pFragmentShader_PixelLighting_Fog_Linear;

           svt_shader_cg_cartoon_fragment * m_pFragmentShader_FrontAndBack;
    static svt_shader_cg_cartoon_fragment * m_pFragmentShader_PixelLighting_FrontAndBack;
    static svt_shader_cg_cartoon_fragment * m_pFragmentShader_PixelLighting_Fog_Exp_FrontAndBack;
    static svt_shader_cg_cartoon_fragment * m_pFragmentShader_PixelLighting_Fog_Exp2_FrontAndBack;
    static svt_shader_cg_cartoon_fragment * m_pFragmentShader_PixelLighting_Fog_Linear_FrontAndBack;


    /**
     * This method intializes all Cg shaders
     */
    void initShader();

public:

    /**
     * Constructor
     */
    svt_container_cartoon_dynamic(svt_pdb * pPdb);

    /**
     * Destructor
     */
    ~svt_container_cartoon_dynamic();

    /**
     * check if shader programs are supported and set the internal boolean to indicate this.
     * (needs vertex profile ARBVP1 and fragment profile ARBFP1).
     * \return true if shaders are supported
     */
    bool checkShaderSupport();

    /**
     * Get if use of shaders for the NewTube and Cartoon mode is possible
     */
    bool getShaderSupported();

    /**
     * Start a tube
     */
    virtual void beginTube( );

    /**
     * End a tube
     */
    virtual void endTube( );

    /**
     * Start a sheet
     */
    virtual void beginSheet( );

    /**
     * End a sheet. Created the tangent information at each vertex
     */
    virtual void endSheet( );

    /**
     * Start a helix
     */
    virtual void beginHelix( );

    /**
     * End a sheet. Created the tangent information at each vertex
     */
    virtual void endHelix( );

    /**
     * Add a segment to the tube/sheet/helix
     */
    inline virtual void addSegment( v4f_c4f_t4f_n3f * oV )
    {
        oV->t[3] = 1.0f;
        m_aVertices.push_back(*oV);
        m_aIndices.push_back(m_iNumVertices++);

        oV->t[3] = -1.0f;
        m_aVertices.push_back(*oV);
        m_aIndices.push_back(m_iNumVertices++);
    };

    /**
     * activate/deactivate per pixel shading
     */
    void setPerPixelLighting(bool bPerPixelLighting);

    /**
     * get if per pixel shading is enabled
     */
    bool getPerPixelLighting();

    /**
     * draw the content of the container (OpenGL)
     */
    virtual void drawGL();

    /**
     * draw the content of the container (Alias/Wavefront)
     */
    virtual void drawOBJ();

    /**
     * delete all elements
     */
    virtual void delAll();

    /**
     * return an array with pointers to the fragment shaders. They are used, too, in the static
     * container
     */
    vector<svt_shader_cg_cartoon_fragment *> getFragmentShaders();
};




/**
 * \brief This is an implementation of the class for primitives for the cartoon rendering mode that
 * generates all the geometry on the CPU.
 *
 * This is useful when rendering a static scene, as vertices are not changed and just get uploaded
 * onto the graphics card.
 * \author Manuel Wahle
 */
class DLLEXPORT svt_container_cartoon_static : public svt_container_cartoon
{

private:

    vector< svt_c4ub_n3f_v3f >  m_aVertices;

    vector< GLsizei  > m_aTriStripSize;
    vector< GLvoid * > m_aTriStripStart;
    vector< GLint    > m_aTriStripStartIndices;

    vector< GLsizei  > m_aTriStripSize_FlatSheets;
    vector< GLvoid * > m_aTriStripStart_FlatSheets;
    vector< GLint    > m_aTriStripStartIndices_FlatSheets;

    vector< GLsizei  > m_aTriFanSize;
    vector< GLvoid * > m_aTriFanStart;
    vector< GLint    > m_aTriFanStartIndices;

    vector< svt_vector3< Real32 > > m_aNormals;
    vector< svt_vector3< Real32 > > m_aBinormals;
    vector< v4f_c4f_t4f_n3f >       m_aTmpVertices;
    svt_vector3< Real32 > oN, oNN, oB, oT, oTT;

    unsigned int m_iLen, i, j, k;

public:

    /**
     * Constructor
     */
    svt_container_cartoon_static(svt_pdb * pPdb);

    /**
     * Destructor
     */
    ~svt_container_cartoon_static();

    /**
     * Start a tube
     */
    virtual void beginTube( );

    /**
     * End a tube, and create the corresponding geometry
     */
    virtual void endTube( );

    /**
     * Start a sheet
     */
    virtual void beginSheet( );

    /**
     * End a sheet, and create the corresponding geometry
     */
    virtual void endSheet( );

    /**
     * Start a helix
     */
    virtual void beginHelix( );

    /**
     * End a helix, and create the corresponding geometry
     */
    virtual void endHelix( );

    /**
     * Add a segment to the tube/sheet/helix
     */
    inline virtual void addSegment( v4f_c4f_t4f_n3f * oV )
    {
        m_aTmpVertices.push_back(*oV);
    };

    /**
     * draw the content of the container (OpenGL)
     */
    virtual void drawGL();

    /**
     * draw the content of the container (Alias/Wavefront)
     */
    virtual void drawOBJ();

    /**
     * delete all elements
     */
    virtual void delAll();

};

#endif
