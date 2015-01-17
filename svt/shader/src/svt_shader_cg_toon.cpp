/***************************************************************************
                          svt_shader_cg_toon
                          ---------------
    begin                : 05/01/2007
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_shader_cg_toon.h>
#include <svt_basics.h>

/**
 * Constructor
 * \param bFile if true the following pointers are actually filenames, if false the pointers point to the actual vertex/fragment shader source code
 * \param pVertex pointer to char with filename of vertex shader sourcecode
 * \param pFragment pointer to char with filename of fragment shader sourcecode
 */
svt_shader_cg_toon::svt_shader_cg_toon(bool bFile, const char * pToonVert, const char * pToonFrag, GLfloat fFogMode, CGprofile pVertProfile, CGprofile pFragProfile)
    : svt_shader_cg( bFile, pToonVert, pToonFrag, pVertProfile, pFragProfile ),
      //m_iCelmapHandle( 0 ),
      m_iFogMode( SVT_FOG_NONE ),
      m_aFogColor( NULL ),
      m_fFogDensity( 0.0f ),
      m_fFogStart( 0.0f ),
      m_fFogEnd( 0.0f ),
      m_fFogZoomOffset( 0.0f )
{
    if (m_bVertex)
    {
	m_cgMVP        = cgGetNamedParameter(m_pVertProg, "MVP");
	m_cgMV         = cgGetNamedParameter(m_pVertProg, "MV");
	m_cgViewpos    = cgGetNamedParameter(m_pVertProg, "viewpos");
	m_cgUp         = cgGetNamedParameter(m_pVertProg, "up");
    }

    if (m_bFragment)
    {
	//m_cgCelmap        = cgGetNamedParameter(m_pFragProg, "celmap");
	m_cgViewport      = cgGetNamedParameter(m_pFragProg, "viewport");
	m_cgLightpos      = cgGetNamedParameter(m_pFragProg, "lightpos");
	m_cgLighting      = cgGetNamedParameter(m_pFragProg, "lighting");
	m_cgSpecularColor = cgGetNamedParameter(m_pFragProg, "specular_color");
	m_cgZ1Z2          = cgGetNamedParameter(m_pFragProg, "z1z2");

	if (fFogMode == GL_LINEAR)
	{
	    m_aFogColor = new GLfloat[4];
	    m_cgFogColor      = cgGetNamedParameter(m_pFragProg, "fog_color");
	    m_cgFogStart      = cgGetNamedParameter(m_pFragProg, "fog_start");
	    m_cgFogEnd        = cgGetNamedParameter(m_pFragProg, "fog_end");
	    m_cgFogZoomOffset = cgGetNamedParameter(m_pFragProg, "fog_zoom_offset");
	    m_iFogMode = SVT_FOG_LINEAR;
	}

	if (fFogMode == GL_EXP || fFogMode == GL_EXP2)
	{
	    m_aFogColor = new GLfloat[4];
	    m_cgFogColor   = cgGetNamedParameter(m_pFragProg, "fog_color");
	    m_cgFogDensity = cgGetNamedParameter(m_pFragProg, "fog_density");

	    // the parameters are the same for exp and exp2 fog, just the shader program is
	    // different. so sufficient to set fog to exp for both exp and exp2
	    m_iFogMode = SVT_FOG_EXP;
	}

	//genCelmap();
    }
};


/**
 * Destructor
 */
svt_shader_cg_toon::~svt_shader_cg_toon()
{
    if (m_aFogColor != NULL)
	delete[] m_aFogColor;
    //glDeleteTextures(1, (const GLuint*)&m_iCelmapHandle);
}


/**
 * set the current state of parameters
 */
void svt_shader_cg_toon::setStateVertParams()
{
    cgGLSetStateMatrixParameter(m_cgMVP,
				CG_GL_MODELVIEW_PROJECTION_MATRIX,
				CG_GL_MATRIX_IDENTITY);

    cgGLSetStateMatrixParameter(m_cgMV,
				CG_GL_MODELVIEW_MATRIX,
				CG_GL_MATRIX_IDENTITY);

    cgGLSetParameter3f(m_cgViewpos, m_oViewpos.x(), m_oViewpos.y(), m_oViewpos.z() );

    cgGLSetParameter3f(m_cgUp, m_oUp.x(), m_oUp.y(), m_oUp.z() );
};


/**
 * set the current state of parameters
 */
void svt_shader_cg_toon::setStateFragParams()
{
//     glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

//     cgGLSetTextureParameter(m_cgCelmap, m_iCelmapHandle);
//     cgGLEnableTextureParameter(m_cgCelmap);

    cgGLSetParameter4f(m_cgViewport, m_oViewport.x(), m_oViewport.y(), m_oViewport.z(),  m_oViewport.w());
    cgGLSetParameter3f(m_cgLightpos, m_oLightpos.x(), m_oLightpos.y(), m_oLightpos.z());
    cgGLSetParameter3f(m_cgLighting,      m_oLighting.x(),      m_oLighting.y(),      m_oLighting.z());
    cgGLSetParameter3f(m_cgSpecularColor, m_oSpecularColor.x(), m_oSpecularColor.y(), m_oSpecularColor.z());
    cgGLSetParameter2f(m_cgZ1Z2, m_fZ1, m_fZ2);

    if (m_iFogMode != SVT_FOG_NONE)
    {
	// set fog color
	cgGLSetParameter3f(m_cgFogColor, m_aFogColor[0], m_aFogColor[1], m_aFogColor[2]);
	
	// for linear fog, set fog start and end
	if (m_iFogMode == SVT_FOG_LINEAR)
	{
	    cgGLSetParameter1f(m_cgFogStart, m_fFogStart);
	    cgGLSetParameter1f(m_cgFogEnd, m_fFogEnd);
	    cgGLSetParameter1f(m_cgFogZoomOffset, m_fFogZoomOffset);
	}
	// if fog not linear, it is exponential
	else
	    cgGLSetParameter1f(m_cgFogDensity, m_fFogDensity);
    }
};


void svt_shader_cg_toon::setLighting(svt_vector3<Real32> oLighting, svt_vector3<Real32> oSpecularColor)
{
    m_oLighting = oLighting;
    m_oSpecularColor = oSpecularColor;
}


void svt_shader_cg_toon::setFogColor(GLfloat * aFogColor)
{
    memcpy(m_aFogColor, aFogColor, sizeof(GLfloat)*4);
}


void svt_shader_cg_toon::setFogDensity(GLfloat fFogDensity)
{
    m_fFogDensity = fFogDensity;
}


void svt_shader_cg_toon::setFogStart(GLfloat fFogStart)
{
    m_fFogStart = fFogStart;
}


void svt_shader_cg_toon::setFogEnd(GLfloat fFogEnd)
{
    m_fFogEnd = fFogEnd;
}


void svt_shader_cg_toon::setFogZoomOffset(GLfloat fFogZoomOffset)
{
    m_fFogZoomOffset = fFogZoomOffset;
}


/**
 * set two values that are used to compute the window z from eye z
 * \param fZ1 used to compute the window z from eye z
 * \param fZ1 used to compute the window z from eye z
 */
void svt_shader_cg_toon::setZParams(Real32 fZ1, Real32 fZ2)
{
    m_fZ1 = fZ1;
    m_fZ2 = fZ2;
}

/**
 * set light position
 * \param oLightpos, an svt_vector3<Real32> with the 3D position of the light
 */
void svt_shader_cg_toon::setLightpos(svt_vector3< Real32 > oLightpos)
{
    m_oLightpos = oLightpos;
}

/**
 * set current viewing position
 * \param oViewpos current viewing position
 */
void svt_shader_cg_toon::setViewpos(svt_vector3<Real32> oViewpos)
{
    m_oViewpos = oViewpos;
}


/**
 * set viewport parameters
 * \param oViewpos, an svt_vector3<Real32> with the OpenGL viewport parameters
 */
void svt_shader_cg_toon::setViewport(svt_vector4<Real32> oViewport)
{
    m_oViewport = oViewport;
}


/**
 * set current camera up vector
 * \param oUp current camera up vector
 */
void svt_shader_cg_toon::setUp(svt_vector3<Real32> oUp)
{
    m_oUp = oUp;
}


/**
 * generate the 1D luminance texture that is used for cel shading
 * \param iCels the number of cels
 
void svt_shader_cg_toon::genCelmap(unsigned int iCels)
{

    unsigned int x, iSize = 32;
    GLfloat * aCelmap = new GLfloat[iSize];

//     for (x=0; x<3; ++x)	 aCelmap[x] = 0.5f;
//     for (x=3; x<8; ++x)	 aCelmap[x] = 0.5f;
//     for (x=8; x<9; ++x)	 aCelmap[x] = 1.0f;
//     for (x=9; x<16; ++x) aCelmap[x] = 1.0f;


//     for (x=0; x<4; ++x)	 aCelmap[x] = 0.2f;
//     for (x=4; x<0; ++x)	 aCelmap[x] = 0.4f;
//     for (x=0; x<16; ++x) aCelmap[x] = 1.0f;

//    for (x=0; x<32; ++x) aCelmap[x] = 1.0f;

     for (x= 0; x<4; ++x)     aCelmap[x] = 0.6f;
     aCelmap[4] = 0.6f;
     aCelmap[5] = 0.7f;
     aCelmap[6] = 0.8f;
     aCelmap[7] = 0.9f;
     //for (x= 8; x<10; ++x)     aCelmap[x] = 0.8f;
     //for (x= 3; x<6; ++x)     aCelmap[x] = 0.6f;
     //for (x=15; x<25; ++x)     aCelmap[x] = 0.4f;
     for (x=8; x<32; ++x)     aCelmap[x] = 1.0f;
//     for (x=30; x<iSize; ++x)  aCelmap[x] = 1.0f;


     //for (x=0; x<32; ++x) printf("%f \n", aCelmap[x]);


    if (!glIsTexture(m_iCelmapHandle))
	glGenTextures(1, (GLuint*)&m_iCelmapHandle);

    // write the data into the texture
    glBindTexture(GL_TEXTURE_1D, m_iCelmapHandle);
    glTexImage1D(GL_TEXTURE_1D,                // target
    	     0,                                // LOD
    	     GL_LUMINANCE,                     // internalFormat
    	     iSize,                            // width
    	     0,                                // border
    	     GL_LUMINANCE,                     // format
	     GL_FLOAT,                         // type
    	     aCelmap);                         // data

    if (!glIsTexture(m_iCelmapHandle))
    {
        svtout << "Problem setting the cel shading map for the toon shader!\n";
	//setVisible(false);
    }

    delete[] aCelmap;

}



 * generate the luminance map with which the spheres will be shaded
 * \param iSize the side length in pixel of the luminance map (default 64)
 

void svt_shader_cg_toon::genDepthMap(unsigned int iSize)
{
    GLfloat x, y, d, s;
    unsigned int X, Y;
    GLfloat * aSphereDepthMap = new GLfloat[iSize*iSize];

    s = 2.0f / (GLfloat) iSize;


	for (X=0, x=-1.0f; X<iSize; ++X, x+=s)
	{
	    for (Y=0, y=-1.0f; Y<iSize; ++Y, y+=s)
	    {
		// d is distance to center from unit circle (i.e. a unit sphere from orthogonal bird's eye view)
		d = sqrt(x*x + y*y);

		if (d > 1.0f)
		    aSphereDepthMap[X*iSize + Y] = 0.0f;
		else
		    aSphereDepthMap[X*iSize + Y]  = cos(asin(d));
	    }
	}




//     for (X=0, x=-1.0f; X<iSize; ++X, x+=s)
//     {
// 	for (Y=0, y=-1.0f; Y<iSize; ++Y, y+=s)
// 	{
// 	    // d is distance to center from unit circle (i.e. a unit sphere from orthogonal bird's eye view)
// 	    d = sqrt(x*x + y*y);

// 	    if (d > 1.0f)
// 		aSphereDepthMap[X*iSize + Y] = 0.0f;
// 	    else
// 		aSphereDepthMap[X*iSize + Y] = cos(asin(d)/100);
// 	}
//     }

    // if it doesn't exist already, create a texture name
    if (!glIsTexture(m_iToonDepthTexHandle))
	glGenTextures(1, &m_iToonDepthTexHandle);

    // write the data into the texture
    glBindTexture(GL_TEXTURE_2D, m_iToonDepthTexHandle);
    glTexImage2D(GL_TEXTURE_2D,                // target
    	     0,                                // LOD
    	     GL_LUMINANCE,                     // internalFormat
    	     iSize,                            // width
    	     iSize,                            // height
    	     0,                                // border
    	     GL_LUMINANCE,                     // format
	     GL_FLOAT,                         // type
    	     aSphereDepthMap);             // data

    if (!glIsTexture(m_iToonDepthTexHandle))
    {
        svtout << "Problem setting the luminance map for the toon shader!\n";
	//setVisible(false);
    }

    delete[] aSphereDepthMap;
}



 * generate the luminance map with which the spheres will be shaded
 * \param iSize the side length in pixel of the luminance map (default 64)
 
 
void svt_shader_cg_toon::genLuminanceMap(unsigned int iNumColorBins, unsigned int iSize)
{
    m_iNumColorBins = iNumColorBins;

    GLfloat x, y, d, s, b;
    unsigned int X, Y;
    GLfloat * aSphereLuminanceMap = new GLfloat[iSize*iSize];
    Real32 fBinWidth;

    // if (m_iNumColorBins == 0), then the spheres should be shaded smoothly.
    // otherwise, m_iNumColorBins determines the number of different color steps

    if (m_iNumColorBins > 0)
	fBinWidth = 1.0f / (Real32) m_iNumColorBins;
    else
	fBinWidth = 0.0f;

    s = 2.0f / (GLfloat) iSize;

    if (fBinWidth > 0.0f)
    {
	for (X=0, x=-1.0f; X<iSize; ++X, x+=s)
	{
	    for (Y=0, y=-1.0f; Y<iSize; ++Y, y+=s)
	    {
		// d is distance to center from unit circle (i.e. a unit sphere from orthogonal bird's eye view)
		d = sqrt(x*x + y*y);

		if (d > 1.0f)
		    aSphereLuminanceMap[X*iSize + Y] = 0.0f;
		else
		{
		    b = fBinWidth;

		    while (true)
		    {
			if (d<b)
			{
			    aSphereLuminanceMap[X*iSize + Y] = 1.0f - b + fBinWidth*0.5f;
			    break;
			}
			else
			    b += fBinWidth;
		    }
		}
	    }
	}
    }
    else
    {
	for (X=0, x=-1.0f; X<iSize; ++X, x+=s)
	{
	    for (Y=0, y=-1.0f; Y<iSize; ++Y, y+=s)
	    {
		// d is distance to center from unit circle (i.e. a unit sphere from orthogonal bird's eye view)
		d = sqrt(x*x + y*y);

		if (d > 1.0f)
		    aSphereLuminanceMap[X*iSize + Y] = 0.0f;
		else
		    aSphereLuminanceMap[X*iSize + Y]  = cos(asin(d));
	    }
	}
    }

    // if it doesn't exist already, create a texture name
    if (!glIsTexture(m_iToonTexHandle))
	glGenTextures(1, &m_iToonTexHandle);

    // write the data into the texture
    glBindTexture(GL_TEXTURE_2D, m_iToonTexHandle);
    glTexImage2D(GL_TEXTURE_2D,                // target
    	     0,                                // LOD
    	     GL_LUMINANCE,                     // internalFormat
    	     iSize,                            // width
    	     iSize,                            // height
    	     0,                                // border
    	     GL_LUMINANCE,                     // format
	     GL_FLOAT,                         // type
    	     aSphereLuminanceMap);             // data

    if (!glIsTexture(m_iToonTexHandle))
    {
        svtout << "Problem setting the luminance map for the toon shader!\n";
	//setVisible(false);
    }

    delete[] aSphereLuminanceMap;
}
*/

/*

------------------------------------------------
this is the fragment shader source with comments
------------------------------------------------

struct FragmentInput
{   float  rad_sq         : TEXCOORD0;
    float2 pos            : TEXCOORD1;
    float3 viewvec        : TEXCOORD2;
    float3 centervec      : TEXCOORD3;
    float4 color          : COLOR0;
    float4 wpos           : WPOS;
};

struct FragmentOutput
{  
    float  depth          : DEPTH;
    float4 color          : COLOR0;
};

FragmentOutput main( FragmentInput IN,
		     uniform float4 viewport,
		     uniform float3 lightpos,
		     uniform float2 z1z2,
		     uniform sampler1D celmap)

{
    FragmentOutput OUT;

    // there is probably no faster way to determine if this fragment can be killed. unless no true 
    // conditional code is executed, but it doesn't make the shader much longer anyway

//    if (length(IN.pos) > 1)
//	discard;


    // all the following computations are done in eye space


    // fast ray-sphere intersection from http://devmaster.net/wiki/Ray-sphere_intersection

    float3 viewvec = normalize(IN.viewvec);
    float3 centervec = IN.centervec;

    float b = dot(viewvec, centervec);
    float c = dot(centervec, centervec) - IN.rad_sq;
    float discriminant = b*b - c;

    // if there is no intersection, kill fragment

    if (discriminant < 0)
	discard;


    // x becomes the smaller solution of ray sphere intersection equation

    float x = b - sqrt(discriminant);

    float3 intersection = x * viewvec;
    float3 normal = normalize(intersection - centervec);

    
    // from eye to window space depth value
    // z1 and z2 are precomputed in the toon container for faster computation here
    // see ISBN 1584500379 p.99 and conversion of ndc-z to window-z

    float z = (z1z2.x / intersection.z) + z1z2.y;
    OUT.depth = z;

 
    // lighting

    float3 color = IN.color.xyz * saturate(dot(normal, lightpos));
    color = color + IN.color.xyz * 0.1f;
    OUT.color.xyz = color;
    OUT.color.w = 1.0f;

    return OUT;
}
*/


/*

----------------------------------------------
this is the vertex shader source with comments
----------------------------------------------

struct VertexInput
{   float4 position       : POSITION;
    float2 tex            : TEXCOORD0;
    float4 color          : COLOR0;
};

struct VertexOutput
{   float4 position       : POSITION;
    float  rad_sq         : TEXCOORD0;
    float2 pos            : TEXCOORD1;
    float3 viewvec        : TEXCOORD2;
    float3 centervec      : TEXCOORD3;
    float4 color          : COLOR0;
};

VertexOutput main( VertexInput IN,
		   uniform float4x4 MVP,
		   uniform float4x4 MV,
		   uniform float3   viewpos,
		   uniform float3   up )
{
    VertexOutput OUT;

    // setup an orthogonal frame

    float3 viewvec = normalize(IN.position.xyz - viewpos);
    float3 right = cross(up, viewvec);
    float3 realup = cross( viewvec, right );


    // this is a billboard, so this vertex is one of four corners of the billboard, and
    // IN.tex is either (-1,-1), (-1, 1), ( 1, 1), or ( 1,-1)
    // use the frame and IN.tex to move this vertex into one of these corners
    // and project it into clip space

    float4 position;
    position.xyz = IN.position.xyz + (IN.tex.x * IN.position.w) * right + (IN.tex.y * IN.position.w) * realup;
    position.w   = 1.0f;
    OUT.position = mul(MVP, position);


    // calculate the viewing ray (to the current corner) in eye space. it will be interpolated by the fragment shader

    float4 vec = normalize(mul(MV, position));
    //vec = vec / vec.w;
    OUT.viewvec = vec.xyz;


    // the fragment shader needs the center of the sphere in eye space...

    position = float4(IN.position.xyz, 1);
    float4 spherecenter = mul(MV, position);
    //spherecenter = spherecenter / spherecenter.w;
    OUT.centervec = spherecenter.xyz;


    // ... and the squared sphere radius in eye space

    position = float4(IN.position.x + IN.position.w, IN.position.y, IN.position.z, 1);
    float4 spherepoint  = mul(MV, position);
    //spherepoint = spherepoint / spherepoint.w;
    float r = length(spherepoint.xyz - spherecenter.xyz);
    OUT.rad_sq = r*r;


    // keep the information in which corner this vertex lies to enable a very early test in the fragment shader
    OUT.pos = IN.tex;


    // pass color through
    OUT.color = IN.color;


    return OUT;
}
*/

