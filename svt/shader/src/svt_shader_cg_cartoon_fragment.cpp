/*-*-c++-*-*****************************************************************
                          svt_shader_cg_cartoon_fragment
                          ---------------
    begin                : 09/04/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_shader_cg_cartoon_fragment.h>


svt_shader_cg_cartoon_fragment::svt_shader_cg_cartoon_fragment(GLint iFogMode, bool bFile, const char * pTubeFrag)
    : svt_shader_cg(bFile, NULL, pTubeFrag),
      m_aFogColor( NULL ),
      m_fFogDensity ( 0.0f ),
      m_fFogStart ( 0.0f ),
      m_fFogEnd ( 0.0f ),
      m_fFogZoomOffset ( 0.0f ),
      m_iFogMode( iFogMode )
{
    m_cgViewPos       = cgGetNamedParameter(m_pFragProg, "viewPos");
    m_cgLighting      = cgGetNamedParameter(m_pFragProg, "lighting");
    m_cgSpecularColor = cgGetNamedParameter(m_pFragProg, "specular_color");

    if (iFogMode != GL_FALSE)
    {
	m_aFogColor = new GLfloat[4];
	m_cgFogColor = cgGetNamedParameter(m_pFragProg, "fog_color");
	m_cgMV = cgGetNamedParameter(m_pFragProg, "MV");

	if (m_iFogMode == GL_LINEAR)
	{
	    m_cgFogZoomOffset = cgGetNamedParameter(m_pFragProg, "fog_zoom_offset");
	    m_cgFogStart      = cgGetNamedParameter(m_pFragProg, "fog_start");
	    m_cgFogEnd        = cgGetNamedParameter(m_pFragProg, "fog_end");
	}
	else
	{
	    m_cgFogDensity = cgGetNamedParameter(m_pFragProg, "fog_density");
	}
    }
}

svt_shader_cg_cartoon_fragment::~svt_shader_cg_cartoon_fragment()
{
    if (m_aFogColor != NULL)
        delete[] m_aFogColor;
}


void svt_shader_cg_cartoon_fragment::setStateFragParams()
{
    cgGLSetParameter3f(m_cgViewPos, m_oViewPos.x(), m_oViewPos.y(), m_oViewPos.z());

    cgGLSetParameter3f(m_cgLighting, m_oLighting.x(), m_oLighting.y(), m_oLighting.z());

    cgGLSetParameter3f(m_cgSpecularColor, m_oSpecularColor.x(), m_oSpecularColor.y(), m_oSpecularColor.z());

    if (m_iFogMode != GL_FALSE)
    {
	cgGLSetParameter3f(m_cgFogColor, m_aFogColor[0], m_aFogColor[1], m_aFogColor[2]);
	cgGLSetStateMatrixParameter(m_cgMV, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);

	if (m_iFogMode == GL_LINEAR)
	{
	    cgGLSetParameter1f(m_cgFogStart, m_fFogStart);
	    cgGLSetParameter1f(m_cgFogEnd, m_fFogEnd);
	    cgGLSetParameter1f(m_cgFogZoomOffset, m_fFogZoomOffset);
	}
	else
	{
	    cgGLSetParameter1f(m_cgFogDensity, m_fFogDensity);
	}
    }
}


void svt_shader_cg_cartoon_fragment::setViewPos(svt_vector3<Real32> oViewPos)
{
    m_oViewPos = oViewPos;
}


void svt_shader_cg_cartoon_fragment::setLighting(svt_vector3<Real32> oLighting, svt_vector3<Real32> oSpecularColor)
{
    m_oLighting = oLighting;
    m_oSpecularColor = oSpecularColor;
}


void svt_shader_cg_cartoon_fragment::setFogColor(GLfloat * aFogColor)
{
    memcpy(m_aFogColor, aFogColor, sizeof(GLfloat)*4);
}


void svt_shader_cg_cartoon_fragment::setFogDensity(GLfloat fFogDensity)
{
    m_fFogDensity = fFogDensity;
}


void svt_shader_cg_cartoon_fragment::setFogStart(GLfloat fFogStart)
{
    m_fFogStart = fFogStart;
}


void svt_shader_cg_cartoon_fragment::setFogEnd(GLfloat fFogEnd)
{
    m_fFogEnd = fFogEnd;
}


void svt_shader_cg_cartoon_fragment::setFogZoomOffset(GLfloat fFogZoomOffset)
{
    m_fFogZoomOffset = fFogZoomOffset;
}
