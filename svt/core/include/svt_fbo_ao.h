/*-*-c++-*-*****************************************************************
                          svt_fbo_ao.h
                          ---------
    begin                : 24/02/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_FBO_AO_H
#define SVT_FBO_AO_H

// svt includes
#include <svt_fbo.h>
#include <svt_opengl.h>
class svt_shader_manager;
class svt_shader_cg_ao_pass1;
class svt_shader_cg_ao_pass2;
class svt_shader_cg_ao_pass3;


/**
 * It is derived from the basic framebuffer class svt_fbo. It redirects output to its local
 * framebuffer object, and takes care of binding the fragment shaders that generate the ambient
 * occlusion.
 *
 * Additionally, it makes sure the following OpenGL extensions are present:
 * - multiple render targets (GL_ARB_draw_buffers)
 * - floating point textures (GL_ARB_texture_float),
 * - non-power of two textures (GL_ARB_texture_non_power_of_two), and
 * - unclamped texture values (GL_ARB_color_buffer_float).
 *
 * The core idea of the screen space ambient occlusion is that sampling points around the current
 * pixel are projected onto tthe viewport and their depth value is compared to the value in the
 * depth buffer. If the value in the depth buffer is lower, then that sampling point is occluded.
 * by the geometry that generated the lower depth value.
 *
 * This way some information about surrounding geometry is recovered and one can compute a
 * occlusion/obsurance coefficient.
 *
 * \brief This class handles framebuffer and fragment shaders for screen space ambient occlusion
 * @author Manuel Wahle
 */
class DLLEXPORT svt_fbo_ao : public svt_fbo
{

protected:

    // max number of clipplanes
    static int m_iMaxClipPlanes;
    // array with enabled clipplanes indices
    static bool* m_aClipPlanes;

    bool    m_bAOSupported;
    GLsizei m_iFBOwidth;
    GLsizei m_iFBOheight;

    GLuint m_iColortexture;
    GLuint m_iAOtexture1;
    GLuint m_iAOtexture2;
    GLuint m_iDepthtexture;
    bool m_bAttachmentsSameFormat;
    Real32 m_fIntensity;
    Real32 m_fRadius;

    svt_shader_cg_ao_pass1* m_pAOShaderPass1;
    svt_shader_cg_ao_pass1* m_pAOShaderPass1_PointBased;
    svt_shader_cg_ao_pass1* m_pAOShaderPass1_LineBased;
    svt_shader_cg_ao_pass2* m_pAOShaderPass2;
    svt_shader_cg_ao_pass3* m_pAOShaderPass3;

    int m_iAOMethod;

    /**
     * This method checks if the size of the attached textures needs to be adjusted (according to
     * the OpenGL window size) and does that if necessary. On first run, it generate the OpenGL
     * texture objects.
     */
    void initAttachments();

    // shader programs and FBO initialized?
    bool m_bShaderInitialized;

public:

    /**
     * Constructor
     */
    svt_fbo_ao();

    /**
     * Destructor
     */
    virtual ~svt_fbo_ao();

    /**
     * This is the method that needs to be called when this framebuffer should start collecting
     * depth and color for all geometry that should undergo ambient occlusion. Its arguments are the
     * width and height of the OpenGL window.
     * \param iFBOwidth integer with the current width (in pixel) of the OpenGL window
     * \param iFBOheight integer with the current height (in pixel) of the OpenGL window
     */
    void beginAO(int iFBOwidth, int iFBOheight);

    /**
     * This is the method that needs to be called when collecting data is done and ambient
     * occlusion is to be calculated and applied. Results are written to the window system
     * framebuffer for display
     */
    void finishAO();

    /**
     * Check if ambient occlusion is supported (i.e., all OpenGL functionality is present).
     */
    bool checkAOSupported();

    /**
     * Get if ambient occlusion is supported (i.e., all OpenGL functionality is present).
     */
    bool getAOSupported();

    /**
     * Set the value for the intensity of ambient occlusion. Set to zero for no intensity (like
     * switching AO off), and to one for full intensity. Values greater than one are also possible
     * \param fIntensity float with the desired intensity value
     */
    void setAOIntensity(Real32 fIntensity);

    /**
     * Set the scaling of the radius of the view space sampling sphere (see svt_shader_cg_ao_pass1)
     * \param fRadius float with the desired radius scaling factor
     */
    void setAOSampleRadiusScaling(Real32 fRadius);

    /**
     * Set ambient occlusion method
     * \param iAOMethod enum that determines AO mode to be used
     */
    void setAOMethod(int iAOMethod);
    /**
     * Set ambient occlusion method
     * \return int enum with the current AO mode
     */
    int getAOMethod();
};

#endif
