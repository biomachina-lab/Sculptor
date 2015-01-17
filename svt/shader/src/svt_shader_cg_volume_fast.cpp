/***************************************************************************
                          svt_shader_cg_volume_fast
                          -------------------------
    begin                : 12/22/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_shader_cg_volume_fast.h>
#include <svt_iostream.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>

///////////////////////////////////////////////////////////////////////////////
// Cg Shader source code
///////////////////////////////////////////////////////////////////////////////

// strings must be written that tight because of limit string size of the windows compiler
const char* g_pVolRenFastVert = "\n\
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
\n\
";

const char* g_pVolRenFastFrag = "\n\
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
        		uniform float4x4 ModelViewProj\n\
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
svt_shader_cg_volume_fast::svt_shader_cg_volume_fast() : svt_shader_cg(false, g_pVolRenFastVert, g_pVolRenFastFrag )
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
	m_pF_ModelViewProj = cgGetNamedParameter(m_pFragProg, "ModelViewProj");

    } else {

	m_pVolume = NULL;
	m_pTransfer = NULL;
    }
};

/**
 * set the current state of parameters
 */
void svt_shader_cg_volume_fast::setStateVertParams()
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
void svt_shader_cg_volume_fast::setStateFragParams()
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

        if (m_pF_ModelViewProj != NULL)
            cgGLSetMatrixParameterfr(m_pF_ModelViewProj, m_oModelView.c_data());
    }
};

/**
 * set volume ID
 * \param iVolumeID texture ID of 3D volume map
 */
void svt_shader_cg_volume_fast::setVolumeID(int iVolumeID)
{
    m_iVolumeID = iVolumeID;
};
/**
 * set transfer ID
 * \param iTransferID texture ID of transfer map
 */
void svt_shader_cg_volume_fast::setTransferID(int iTransferID)
{
    m_iTransferID = iTransferID;
};

/**
 * set slice thickness
 * \param fSliceDistance distance between slices
 */
void svt_shader_cg_volume_fast::setSliceDistance(float fSliceDistance)
{
    m_fSliceDistance = fSliceDistance;
}

/**
 * set modelview matrix
 * \param oModelView svt_matrix4<Real32> modelview matrix object
 */
void svt_shader_cg_volume_fast::setModelViewMatrix(svt_matrix4<Real32> oModelView)
{
    m_oModelView = oModelView;
};
