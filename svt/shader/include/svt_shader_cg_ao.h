/*-*-c++-*-*****************************************************************
                          svt_shader_cg_ao.h
                          ---------------
    begin                : 24/02/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_SHADER_CG_AO_H
#define __SVT_SHADER_CG_AO_H


#include <svt_opengl.h>
#include <svt_shader_cg.h>
#include <svt_types.h>


enum { SVT_AO_POINT_BASED,
       SVT_AO_LINE_BASED
};


/**
 * Ambient Occlusion first pass shader: after SVT has done all OpenGL drawing (and svt_fbo_ao has
 * catched all of that into its attachments), this shader uses the depth texture and computes the
 * per-pixel ao coefficients. The result is written into the second color attachment.
 * \brief This is a class derived from svt_shader_cg for the screen space ambient occlusion fragment
 * shader.
 * @author Manuel Wahle
 */
class DLLEXPORT svt_shader_cg_ao_pass1 : public svt_shader_cg
{

protected:

    GLfloat * m_aPointSamples;
    GLfloat * m_aLineSamples;
    unsigned int m_iNumPointSamples;
    unsigned int m_iNumLineSamples;

    GLuint m_iDepthtexture;
    GLuint m_iDithertexture;
    GLfloat m_fFBOwidth;
    GLfloat m_fFBOheight;
    GLfloat m_fP1;
    GLfloat m_fP2;
    GLfloat m_fA;
    GLfloat m_fB;
    GLfloat m_fC;
    GLfloat m_fD;
    GLfloat m_fR;
    GLfloat m_fR1;
    GLfloat m_fR2;
    GLfloat m_fIntensity;
    int m_iAOMethod;

    CGparameter m_cgDepthtexture;
    CGparameter m_cgDithertexture;
    CGparameter m_cgPointSamples;
    CGparameter m_cgLineSamples;
    CGparameter m_cgFBOsize;
    CGparameter m_cgP1P2;
    CGparameter m_cgABCD;
    CGparameter m_cgAOIntensity;
    CGparameter m_cgRadius;

    /**
     * Set state of parameters for the fragment shader
     */
    virtual void setStateFragParams();

    /**
     * This function (re-)generates an array with positions of sample points in a sphere
     */
    void generatePointSamples();

    /**
     * This function (re-)generates an array with positions of lines in a sphere
     */
    void generateLineSamples();

    /**
     * This method generates the texture for the dithering. The fragment shader will alter the
     * direction of the vectors in the sampling sphere using the values in the dithering map. Since
     * the values are random for the whole map, this helps to prevent the low frequency changes of
     * the ao factor across the resulting image.
     */
    void generateDitheringMap();

public:

    /**
     * Constructor
     */
    svt_shader_cg_ao_pass1(bool bFile, const char* pFragment, int iAOMethod);

    /**
     * Destructor
     */
    virtual ~svt_shader_cg_ao_pass1();

    /**
     * unbind the shader
     */
    virtual void unbind();

    /**
     * Sets the OpenGL texture object for the depth texture
     * \param iDepthtexture the name of the OpenGL texture object for the depth texture
     */
    void setDepthTexture(unsigned int iDepthtexture);

    /**
     * Sets the size of the FBO in pixels
     * \param fWidth the width of the FBO in pixels
     * \param fHeight the height of the FBO in pixels
     */
    void setFBOsize(unsigned int iWidth, unsigned int iHeight);

    /**
     * The fragment shader needs some values from the projection matrix. This method sets the values
     * of P[0][0] and P[1][1]
     * \param fP1 the value of P[0][0]
     * \param fP2 the value of P[1][1]
     */
    void setP1P2(Real32 fP1, Real32 fP2);

    /**
     * The fragment shader needs some values from the projection matrix. This method sets the values
     * of A, B, C, and D (see the OpenGL documentation of glFrustum())
     * \param fA the value of A
     * \param fB the value of B
     * \param fC the value of C
     * \param fD the value of D
     */
    void setABCD(Real32 fA, Real32 fB, Real32 fC, Real32 fD);

    /**
     * Set the scaling of the radius of the view space sampling sphere
     * \param fRadius float with the desired radius scaling factor
     */
    void setAOSampleRadiusScaling(Real32 fRadius);

    /**
     * Return the scaling of the radius of the view space sampling sphere
     * \return float with the current radius scaling factor
     */
    Real32 getAOSampleRadiusScaling();

    /**
     * Set the value for the intensity of ambient occlusion. Set to zero for no intensity (like
     * switching AO off), and to one for full intensity. Values greater than one are also possible
     * \param fIntensity float with the desired intensity value
     */
    void setAOIntensity(Real32 fIntensity);

    /**
     * Returns the current scaling of the radius of the sampling sphere
     * \return float with the current intensity scaling value
     */
    Real32 getAOIntensity();

    /**
     * get ambient occlusion method
     * \return int enum with the current AO mode
     */
    int getAOMethod();
};


/**
 * Ambient Occlusion second pass shader. It takes the output of the pass1 shader (the per pixel ao
 * coefficients) and does the first blurring step (in x-direction). The result is stored in the
 * third color attachment
 * \brief This is a class derived from svt_shader_cg for the screen space ambient occlusion fragment
 * shader.
 * @author Manuel Wahle
 */
class DLLEXPORT svt_shader_cg_ao_pass2 : public svt_shader_cg
{

protected:

    GLuint  m_iAOtexture;
    GLuint  m_iDepthtexture;
    GLfloat m_fPixelSizeX;
    GLfloat m_fPixelSizeY;
    GLfloat m_fFBOwidth;
    GLfloat m_fFBOheight;

    CGparameter m_cgAOtexture;
    CGparameter m_cgDepthtexture;
    CGparameter m_cgPixelSize;
    CGparameter m_cgFBOsize;

    /**
     * Set state of parameters for the fragment shader
     */
    virtual void setStateFragParams();

public:

    /**
     * Constructor
     */
    svt_shader_cg_ao_pass2(bool bFile, const char* pFragment);

    /**
     * Destructor
     */
    virtual ~svt_shader_cg_ao_pass2();

    /**
     * unbind the shader
     */
    virtual void unbind();

    /**
     * Sets the size of the FBO in pixels
     * \param fWidth the width of the FBO in pixels
     * \param fHeight the height of the FBO in pixels
     */
    void setFBOsize(unsigned int fWidth, unsigned int fHeight);

    /**
     * Sets the OpenGL texture object for the depth texture
     * \param iDepthtexture the name of the OpenGL texture object for the depth texture
     */
    void setDepthTexture(unsigned int iDepthtexture);

    /**
     * Sets the OpenGL texture object for the AO coefficients texture
     * \param iAOtexture the name of the OpenGL texture object for the AO coefficients texture
     */
    void setAOTexture(unsigned int iAOtexture);
};




/**
 * Ambient Occlusion third pass shader. It takes the output of the pass2 shader (the in x-direction
 * blurred ao coefficients) and does the second blurring step (in y-direction). The result is written
 * into the first color attachment. The window system framebuffer should be bound.
 * \brief This is a class derived from svt_shader_cg for the screen space ambient occlusion fragment
 * shader.
 * @author Manuel Wahle
 */
class DLLEXPORT svt_shader_cg_ao_pass3 : public svt_shader_cg
{

protected:

    GLuint  m_iAOtexture;
    GLuint  m_iDepthtexture;
    GLuint  m_iColortexture;
    GLfloat m_fPixelSizeX;
    GLfloat m_fPixelSizeY;
    GLfloat m_fFBOwidth;
    GLfloat m_fFBOheight;

    CGparameter m_cgAOtexture;
    CGparameter m_cgDepthtexture;
    CGparameter m_cgColortexture;
    CGparameter m_cgPixelSize;
    CGparameter m_cgFBOsize;
 
    /**
     * Set state of parameters for the fragment shader
     */
    virtual void setStateFragParams();

public:

    /**
     * Constructor
     */
    svt_shader_cg_ao_pass3(bool bFile, const char* pFragment);

    /**
     * Destructor
     */
    virtual ~svt_shader_cg_ao_pass3();

    /**
     * unbind the shader
     */
    virtual void unbind();

    /**
     * Sets the size of the FBO in pixels
     * \param fWidth the width of the FBO in pixels
     * \param fHeight the height of the FBO in pixels
     */
    void setFBOsize(unsigned int fWidth, unsigned int fHeight);

    /**
     * Sets the OpenGL texture object for the AO coefficients texture
     * \param iAOtexture the name of the OpenGL texture object for the AO texture
     */
    void setAOTexture(unsigned int iAOtexture);

    /**
     * Sets the OpenGL texture object for the depth texture
     * \param iDepthtexture the name of the OpenGL texture object for the depth texture
     */
    void setDepthTexture(unsigned int iDepthtexture);

    /**
     * Sets the OpenGL texture object for the color texture
     * \param iColortexture the name of the OpenGL texture object for the color texture
     */
    void setColorTexture(unsigned int iColortexture);
};

#endif
