/***************************************************************************
                          svt_vbo.h
                          ---------
    begin                : 11/16/2006
    author               : Stefan Birmanns, Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_iostream.h>
#include <svt_opengl.h>
#include <svt_vbo.h>

SVT_PFNGLGENBUFFERSARBPROC    svt_vbo::glGenBuffersARB    = NULL;
SVT_PFNGLBINDBUFFERARBPROC    svt_vbo::glBindBufferARB    = NULL;
SVT_PFNGLBUFFERDATAARBPROC    svt_vbo::glBufferDataARB    = NULL;
SVT_PFNGLBUFFERSUBDATAARBPROC svt_vbo::glBufferSubDataARB = NULL;
SVT_PFNGLDELETEBUFFERSARBPROC svt_vbo::glDeleteBuffersARB = NULL;
SVT_PFNGLMAPBUFFERARBPROC     svt_vbo::glMapBufferARB     = NULL;
SVT_PFNGLUNMAPBUFFERARBPROC   svt_vbo::glUnmapBufferARB   = NULL;
SVT_PFNGLISBUFFERARBPROC      svt_vbo::glIsBufferARB      = NULL;


svt_vbo::svt_vbo() :
    m_iBuffer( 0 ),
    m_bBufferGenerated( false )
{
    if (glBindBufferARB == NULL)
    {
	glBindBufferARB =    (SVT_PFNGLBINDBUFFERARBPROC)    svt_lookupFunction("glBindBufferARB");
	glGenBuffersARB =    (SVT_PFNGLGENBUFFERSARBPROC)    svt_lookupFunction("glGenBuffersARB");
	glBufferDataARB =    (SVT_PFNGLBUFFERDATAARBPROC)    svt_lookupFunction("glBufferDataARB");
	glBufferSubDataARB = (SVT_PFNGLBUFFERSUBDATAARBPROC) svt_lookupFunction("glBufferSubDataARB");
	glDeleteBuffersARB = (SVT_PFNGLDELETEBUFFERSARBPROC) svt_lookupFunction("glDeleteBuffersARB");
	glMapBufferARB =     (SVT_PFNGLMAPBUFFERARBPROC)     svt_lookupFunction("glMapBufferARB");
	glUnmapBufferARB =   (SVT_PFNGLUNMAPBUFFERARBPROC)   svt_lookupFunction("glUnmapBufferARB"); 
	glIsBufferARB =      (SVT_PFNGLISBUFFERARBPROC)      svt_lookupFunction("glIsBufferARB"); 
    }

    if (glBindBufferARB == NULL)
        svt_disableEXT_VBO( true );
};


svt_vbo::~svt_vbo()
{
    if (m_bBufferGenerated)
    {
	glDeleteBuffersARB( 1, &m_iBuffer );
	svt_checkGLErrors( "svt_vbo: glDeleteBuffersARB()" );
    }
}


void svt_vbo::genBuffer( )
{
    if (!m_bBufferGenerated)
    {
        if (svt_checkEXT_VBO())
        {
            glGenBuffersARB(1, &m_iBuffer);
            m_bBufferGenerated = true;
            svt_checkGLErrors( "svt_vbo: glGenBuffersARB()" );
        }
    }
};


void svt_vbo::deleteBuffer( )
{
    if (m_bBufferGenerated)
    {
	glDeleteBuffersARB( 1, &m_iBuffer );
	m_bBufferGenerated = false;
	m_iBuffer = 0;
	svt_checkGLErrors( "svt_vbo: glDeleteBuffersARB()" );
    }
};


// not used right now because it made problems with the an system
bool svt_vbo::isBuffer( )
{
    GLboolean b = GL_FALSE;

    if (m_bBufferGenerated)
    {
	b = glIsBufferARB(m_iBuffer);
	svt_checkGLErrors( "svt_vbo: glIsBuffersARB()" );
    }

    return (b == GL_TRUE);
};


void svt_vbo::bindBuffer( GLenum target )
{
    if (m_bBufferGenerated)
    {
	glBindBufferARB(target, m_iBuffer);
	svt_checkGLErrors( "svt_vbo: glBindBufferARB()" );
    }
};


void svt_vbo::bufferData( GLenum target, int size, GLvoid *data, GLenum usage )
{
    if (m_bBufferGenerated)
    {
	glBufferDataARB(target, size, data, usage);
	svt_checkGLErrors( "svt_vbo: glBufferDataARB()" );
    }
};


void svt_vbo::bufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data)
{
    if (m_bBufferGenerated)
    {
	glBufferSubDataARB(target, offset, size, data);
	svt_checkGLErrors( "svt_vbo: glBufferSubDataARB()" );
    }
};


void svt_vbo::releaseBuffer(GLenum target)
{
    if (m_bBufferGenerated)
    {
        glBindBufferARB(target, 0);
	svt_checkGLErrors( "svt_vbo: glBindBufferARB(target, 0)" );
    }
};


void * svt_vbo::mapBuffer( GLenum target, GLenum access )
{
    void * p = NULL;

    if (m_bBufferGenerated)
    {
	p = (Real32*) glMapBufferARB(target, access);
	svt_checkGLErrors( "svt_vbo: glMapBufferARB()" );
    }

    return p;
};


bool svt_vbo::unmapBuffer( GLenum target )
{
    bool b = false;

    if (m_bBufferGenerated)
    {
	b = (bool) glUnmapBufferARB(target);
	svt_checkGLErrors( "svt_vbo: glUnmapBufferARB()" );
    }

    return b;
};
