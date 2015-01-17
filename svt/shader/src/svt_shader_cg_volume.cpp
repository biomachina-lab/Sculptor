/***************************************************************************
                          svt_shader_cg_volume
                          --------------------
    begin                : 08/05/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_shader_cg_volume.h>
#include <svt_iostream.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>

///////////////////////////////////////////////////////////////////////////////
// Cg Shader source code
///////////////////////////////////////////////////////////////////////////////

// strings must be written that tight because of limit string size of the windows compiler
const char* g_pVolRenVert = "\n\
struct appdata\n\
{\n\
	float4 position		: POSITION;\n\
     	float4 texCoord		: TEXCOORD0;\n\
};\n\
\n\
struct volOutput\n\
{\n\
	float4 position 	: POSITION;\n\
        float4 texCoord0	: TEXCOORD0;\n\
        float4 texCoord1	: TEXCOORD1;\n\
};\n\
\n\
volOutput main(\n\
    		   appdata IN,\n\
		   uniform float4x4 ModelViewProj,\n\
                   uniform float4x4 ModelViewInv,\n\
                   uniform float4x4 TextureMatrix,\n\
                   uniform float SliceDistance\n\
              )\n\
{\n\
	volOutput OUT;\n\
\n\
	// Calculate vertex's texture coordinate\n\
	OUT.texCoord0 = mul( TextureMatrix, IN.texCoord );\n\
\n\
	// Calculate vertex's final, transformed position.\n\
        OUT.position = mul( ModelViewProj, IN.position );\n\
\n\
	// Calculate other side of slab\n\
        //float4 vPosition = mul( ModelViewInv, float4( 0, 0, 0, 1 ) );\n\
  	//float4 vDir      = normalize( mul( ModelViewInv, float4( 0.0f, 0.0f, 1.0f, 1.0f ) ) );\n\
  	//float3 vEye2vert  = normalize( IN.position.xyz - vPosition.xyz );\n\
	//float4 vBackVert = { 1, 1, 1, 1 };\n\
  	//vBackVert.xyz = IN.position.xyz + vEye2vert * (SliceDistance / dot( vDir.xyz, vEye2vert ) );\n\
	//OUT.texCoord1 = mul( TextureMatrix, vBackVert );\n\
\n\
  	float3 vDir      = float3( 0.0f, 0.0f, SliceDistance );\n\
	float4 vBackVert = { 1, 1, 1, 1 };\n\
  	vBackVert.xyz = IN.texCoord.xyz + vDir;\n\
        OUT.texCoord1 = mul( TextureMatrix, vBackVert );\n\
\n\
	return OUT;\n\
}\n\
";

const char* g_pVolRenFrag = "\n\
struct vol_fragOUTPUT\n\
{\n\
        half4 color : COLOR;\n\
};\n\
\n\
vol_fragOUTPUT main(\n\
			float4 position  : POSITION,\n\
        		float3 texCoord0 : TEXCOORD0,\n\
        		float3 texCoord1 : TEXCOORD1,\n\
			uniform sampler3D Volume,\n\
        		uniform sampler2D Transfer,\n\
        		uniform float3 LightPosition,\n\
        		uniform float Diffuse,\n\
        		uniform float Ambient,\n\
        		uniform float Shininess\n\
	           )\n\
{\n\
        vol_fragOUTPUT OUT;\n\
\n\
	// sample the 3d volume\n\
        half4 data0 = tex3D(Volume, texCoord0);\n\
        half4 data1 = tex3D(Volume, texCoord1);\n\
\n\
        // sample the transfer function according to the voxel values\n\
        OUT.color = tex2D(Transfer, half2( data1.a, data0.a) );\n\
\n\
	// shading\n\
        half3 n1 = (data0.rgb - 0.5) * 2.0;\n\
	half3 n2 = (data1.rgb - 0.5) * 2.0;\n\
        half3 n = normalize((n1 + n2) * 0.5);\n\
\n\
        //if (length( n ) < 0.1 || data0.a == 0)\n\
        //	discard;\n\
\n\
     	// diffuse lighting\n\
        half fDiffuse = ( dot(LightPosition, n.xyz ) );\n\
        // clamp diffuse reflection contribution\n\
        fDiffuse = max( fDiffuse, 0.0);\n\
        fDiffuse = min( fDiffuse, 1.0);\n\
\n\
        // specular light\n\
        half fSpec = 0;\n\
       	fSpec = pow( fDiffuse, Shininess );\n\
        // clamp specular reflection contribution\n\
        fSpec = min( fSpec, 1.0);\n\
        fSpec = max( fSpec, 0.0);\n\
\n\
        // diffuse light factor\n\
        //fDiffuse *= Diffuse;\n\
\n\
        // set and return output\n\
        OUT.color.rgb = (OUT.color.rgb*fDiffuse) + (OUT.color.rgb*Ambient) + (half3(1,1,1)*fSpec*OUT.color.a);\n\
        return OUT;\n\
}\n\
\n\
";

///////////////////////////////////////////////////////////////////////////////
// C++ source code
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 */
svt_shader_cg_volume::svt_shader_cg_volume() : svt_shader_cg(false, g_pVolRenVert, g_pVolRenFrag )
//svt_shader_cg_volume::svt_shader_cg_volume() : svt_shader_cg(true, "svt_shader_cg_volume_vert.cg", "svt_shader_cg_volume_frag.cg" )
{
    if (m_bVertex && m_pVertProfile != CG_PROFILE_UNKNOWN)
    {
	m_pTexMat = cgGetNamedParameter(m_pVertProg, "TextureMatrix");
	m_pModelViewInv = cgGetNamedParameter(m_pVertProg, "ModelViewInv");
	m_pSliceDistance = cgGetNamedParameter(m_pVertProg, "SliceDistance");

    } else {

        m_pTexMat = NULL;
    }

    if (m_bFragment && m_pFragProfile != CG_PROFILE_UNKNOWN)
    {
	m_pVolume = cgGetNamedParameter(m_pFragProg, "Volume");
	m_pTransfer = cgGetNamedParameter(m_pFragProg, "Transfer");
        m_pLightPosition = cgGetNamedParameter(m_pFragProg, "LightPosition");
        m_pDiffuse = cgGetNamedParameter(m_pFragProg, "Diffuse");
        m_pAmbient = cgGetNamedParameter(m_pFragProg, "Ambient");
        m_pShininess = cgGetNamedParameter(m_pFragProg, "Shininess");

    } else {

	m_pVolume = NULL;
	m_pTransfer = NULL;
    }
};

/**
 * set the current state of parameters
 */
void svt_shader_cg_volume::setStateVertParams()
{
    if (m_bVertex && m_pVertProfile != CG_PROFILE_UNKNOWN)
    {
        svt_shader_cg::setStateVertParams();

        if(m_pTexMat != NULL)
            cgGLSetStateMatrixParameter(m_pTexMat,
                                        CG_GL_TEXTURE_MATRIX,
                                        CG_GL_MATRIX_IDENTITY);

        if (m_pModelViewInv != NULL)
            cgGLSetStateMatrixParameter(m_pModelViewInv,
                                        CG_GL_MODELVIEW_MATRIX,
                                        CG_GL_MATRIX_INVERSE);

        if (m_pSliceDistance != NULL)
            cgGLSetParameter1f(m_pSliceDistance, m_fSliceDistance);
    }
};

/**
 * set the current state of parameters
 */
void svt_shader_cg_volume::setStateFragParams()
{
    if (m_bFragment && m_pFragProfile != CG_PROFILE_UNKNOWN)
    {
        svt_shader_cg::setStateFragParams();

        if (m_pVolume != NULL)
        {
            // activate 3D volume texture
            cgGLSetTextureParameter(m_pVolume, m_iVolumeID);
            cgGLEnableTextureParameter(m_pVolume);
        }

        if (m_pTransfer != NULL)
        {
            // activate 2D transfer texture
            cgGLSetTextureParameter(m_pTransfer, m_iTransferID);
            cgGLEnableTextureParameter(m_pTransfer);
        }

        if (m_pLightPosition != NULL)
            cgGLSetParameter3f(m_pLightPosition, m_vLightPosition.x(), m_vLightPosition.y(), m_vLightPosition.z() );

        if (m_pDiffuse != NULL)
            cgGLSetParameter1f(m_pDiffuse, m_fDiffuse);
        if (m_pAmbient != NULL)
            cgGLSetParameter1f(m_pAmbient, m_fAmbient);
        if (m_pShininess != NULL)
            cgGLSetParameter1f(m_pShininess, m_fShininess);
    }
};

/**
 * set volume ID
 * \param iVolumeID texture ID of 3D volume map
 */
void svt_shader_cg_volume::setVolumeID(int iVolumeID)
{
    m_iVolumeID = iVolumeID;
};
/**
 * set transfer ID
 * \param iTransferID texture ID of transfer map
 */
void svt_shader_cg_volume::setTransferID(int iTransferID)
{
    m_iTransferID = iTransferID;
};

/**
 * set slice thickness
 * \param fSliceDistance distance between slices
 */
void svt_shader_cg_volume::setSliceDistance(float fSliceDistance)
{
    m_fSliceDistance = fSliceDistance;
}

/**
 * set light position
 * \param vLightPosition svt_vector3<Real32> with the 3D position of the light source
 */
void svt_shader_cg_volume::setLightPosition(svt_vector3<Real32> vLightPosition)
{
    m_vLightPosition = vLightPosition;
}
/**
 * set modelview matrix
 * \param oModelView svt_matrix4<Real32> modelview matrix object
 */
void svt_shader_cg_volume::setModelViewMatrix(svt_matrix4<Real32> oModelView)
{
    m_oModelView = oModelView;
};

/**
 * set lighting params
 * \param fDiffuse diffuse lighting factor (0..10)
 * \param fAmbient ambient light contribution (0..1)
 * \param fShininess shininess of the material (0..128)
 */
void svt_shader_cg_volume::setLightingParams(Real32 fDiffuse, Real32 fAmbient, Real32 fShininess)
{
    m_fDiffuse = fDiffuse;
    m_fAmbient = fAmbient;
    m_fShininess = fShininess;
};
