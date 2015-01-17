/*-*-c++-*-*****************************************************************
                          svt_shader_manager
                          ------------------
    begin                : 12/20/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_shader_manager.h>
#include <svt_shader_cg_ao.h>
#include <svt_shader_cg_toon.h>

/**
 *
 */
svt_shader_manager::svt_shader_manager() :
    m_pAOShaderPass1_PointBased( NULL ),
    m_pAOShaderPass1_LineBased( NULL ),
    m_pAOShaderPass2( NULL ),
    m_pAOShaderPass3( NULL ),
    m_pVDWShaderVertex( NULL ),
    m_pVDWShaderFragment_Plain( NULL ),
    m_pVDWShaderFragment_FogLinear( NULL ),
    m_pVDWShaderFragment_FogExp( NULL ),
    m_pVDWShaderFragment_FogExp2( NULL )
{
}

/**
 *
 */
svt_shader_manager::~svt_shader_manager()
{
    if (m_pAOShaderPass1_PointBased != NULL)
        delete m_pAOShaderPass1_PointBased;
    if (m_pAOShaderPass1_LineBased != NULL)
        delete m_pAOShaderPass1_LineBased;
    if (m_pAOShaderPass2 != NULL)
        delete m_pAOShaderPass2;
    if (m_pAOShaderPass3 != NULL)
        delete m_pAOShaderPass3;
    if (m_pVDWShaderVertex != NULL)
        delete m_pVDWShaderVertex;
    if (m_pVDWShaderFragment_Plain != NULL)
        delete m_pVDWShaderFragment_Plain;
    if (m_pVDWShaderFragment_FogLinear != NULL)
        delete m_pVDWShaderFragment_FogLinear;
    if (m_pVDWShaderFragment_FogExp != NULL)
        delete m_pVDWShaderFragment_FogExp;
    if (m_pVDWShaderFragment_FogExp2 != NULL)
        delete m_pVDWShaderFragment_FogExp2;
}

/**
 *
 */
svt_shader* svt_shader_manager::getAOShader(int iShader)
{
    switch (iShader)
    {
        case SVT_SHADER_AO_PASS1_POINT_BASED:
            if (m_pAOShaderPass1_PointBased == NULL)
            {
                //m_pAOShaderPass1_PointBased = new svt_shader_cg_ao_pass1(true, "/home/mwahle/Shader/ao_pointbased_screen.cg", SVT_AO_POINT_BASED);
                m_pAOShaderPass1_PointBased = new svt_shader_cg_ao_pass1(false, getAOShaderSource(iShader), SVT_AO_POINT_BASED);
            }
            return (svt_shader*)m_pAOShaderPass1_PointBased;
            break;

        case SVT_SHADER_AO_PASS1_LINE_BASED:
            if (m_pAOShaderPass1_LineBased == NULL)
            {
                //m_pAOShaderPass1_LineBased = new svt_shader_cg_ao_pass1(true, "/home/mwahle/Shader/ao_linebased_screen.cg", SVT_AO_LINE_BASED);
                m_pAOShaderPass1_LineBased = new svt_shader_cg_ao_pass1(false, getAOShaderSource(iShader), SVT_AO_LINE_BASED);
            }
            return (svt_shader*)m_pAOShaderPass1_LineBased;
            break;

        case SVT_SHADER_AO_PASS2:
            if (m_pAOShaderPass2 == NULL)
            {
                //m_pAOShaderPass2 = new svt_shader_cg_ao_pass2(true, "/home/mwahle/Shader/ao_frag_pass2.cg");
                m_pAOShaderPass2 = new svt_shader_cg_ao_pass2(false, getAOShaderSource(iShader));
            }
            return (svt_shader*)m_pAOShaderPass2;
            break;

        case SVT_SHADER_AO_PASS3:
            if (m_pAOShaderPass3 == NULL)
            {
                //m_pAOShaderPass3 = new svt_shader_cg_ao_pass3(true, "/home/mwahle/Shader/ao_frag_pass3.cg");
                m_pAOShaderPass3 = new svt_shader_cg_ao_pass3(false, getAOShaderSource(iShader));
            }
            return (svt_shader*)m_pAOShaderPass3;
            break;
    }

    // avoid compiler warning...
    return NULL;
}

/**
 *
 */
svt_shader* svt_shader_manager::getVDWShader(int iShader)
{
    switch (iShader)
    {
        case SVT_SHADER_TOON_VDW_VERTEX:
            if (m_pVDWShaderVertex == NULL)
            {
                //m_pVDWShaderVertex = new svt_shader_cg_toon(true, "/home/mwahle/Shader/pShaderVert.cg", NULL);
                m_pVDWShaderVertex = new svt_shader_cg_toon(false, getVDWShaderSource(iShader), NULL);
            }
            return (svt_shader*)m_pVDWShaderVertex;
            break;

        case SVT_SHADER_TOON_VDW_FRAG_PLAIN:
            if (m_pVDWShaderFragment_Plain == NULL)
            {
                //m_pVDWShaderFragment = new svt_shader_cg_toon(true, NULL, "/home/mwahle/Shader/pShaderFrag.cg");
                m_pVDWShaderFragment_Plain = new svt_shader_cg_toon(false, NULL, getVDWShaderSource(iShader));
            }
            return (svt_shader*)m_pVDWShaderFragment_Plain;
            break;

        case SVT_SHADER_TOON_VDW_FRAG_FOGLINEAR:
            if (m_pVDWShaderFragment_FogLinear == NULL)
            {
                //m_pVDWShaderFragment_FogLinear = new svt_shader_cg_toon(true, NULL, "/home/mwahle/Shader/pShaderFragment_FogLinear.cg", GL_LINEAR);
                m_pVDWShaderFragment_FogLinear = new svt_shader_cg_toon(false, NULL, getVDWShaderSource(iShader), GL_LINEAR);
            }
            return (svt_shader*)m_pVDWShaderFragment_FogLinear;
            break;

        case SVT_SHADER_TOON_VDW_FRAG_FOGEXP:
            if (m_pVDWShaderFragment_FogExp == NULL)
            {
                //m_pVDWShaderFragment_FogExp = new svt_shader_cg_toon(true, NULL, "/home/mwahle/Shader/pShaderFragment_FogExp.cg", GL_EXP);
                m_pVDWShaderFragment_FogExp = new svt_shader_cg_toon(false, NULL, getVDWShaderSource(iShader), GL_EXP);
            }
            return (svt_shader*)m_pVDWShaderFragment_FogExp;
            break;

        case SVT_SHADER_TOON_VDW_FRAG_FOGEXP2:
            if (m_pVDWShaderFragment_FogExp2 == NULL)
            {
                //m_pVDWShaderFragment_FogExp2 = new svt_shader_cg_toon(true, NULL, "/home/mwahle/Shader/pShaderFragment_FogExp2.cg", GL_EXP2);
                m_pVDWShaderFragment_FogExp2 = new svt_shader_cg_toon(false, NULL, getVDWShaderSource(iShader), GL_EXP2);
            }
            return (svt_shader*)m_pVDWShaderFragment_FogExp2;
            break;
    }

    // avoid compiler warning...
    return NULL;
}

/**
 *
 */
const char* svt_shader_manager::getAOShaderSource(int iShader)
{
    // the const char* pointing to the sources must be static so they won't be on the stack (so
    // their contents are not deleted)

    switch (iShader)
    {
        case SVT_SHADER_AO_PASS1_POINT_BASED:
            static const char* pSourceAO1 = "\
struct FragmentInput\
{   float4 wpos           : WPOS;\
};\
struct FragmentOutput\
{   float color          : COLOR0;\
};\
FragmentOutput main( FragmentInput IN,\
                     uniform sampler2D depthtexture,\
                     uniform sampler2D dithertexture,\
                     uniform float3[8] pointsamples,\
                     uniform float2 FBOsize,\
                     uniform float2 P1P2,\
                     uniform float4 ABCD,\
                     uniform float intensity,\
                     uniform float radius)\
{   FragmentOutput OUT;\
    float3 pos_s;\
    float3 pos_e;\
    float3 pos_n;\
    float3 samplepos_s;\
    float2 radius_s;\
    float  ao = 0.0f;\
    float  samplepos_e_z;\
    float  C_minus_one = ABCD.z - 1.0f;\
    float  minus_W     = ABCD.w * -1.0f;\
    float  samplepos_s_z;\
    float3 pointsample;\
    float3 axis;\
    pos_s.x = IN.wpos.x / FBOsize.x;\
    pos_s.y = IN.wpos.y / FBOsize.y;\
    pos_s.z = tex2D(depthtexture, pos_s.xy).x;\
    if (pos_s.z == 1.0f)\
        discard;\
    axis = normalize(tex2D(dithertexture, pos_s.xy).xyz);\
    pos_n = pos_s * 2.0f - float3(1.0f);\
    pos_e.z = minus_W / (pos_n.z + ABCD.z);\
    pos_e.x = - pos_e.z * (ABCD.x + pos_n.x) / P1P2.x;\
    pos_e.y = - pos_e.z * (ABCD.y + pos_n.y) / P1P2.y;\
    float posX_s = P1P2.x * (pos_e.x + radius)  +  ABCD.x * pos_e.z;\
    posX_s = posX_s / -pos_e.z;\
    posX_s = (posX_s + 1.0f) /  2.0;\
    radius_s.x = (posX_s - pos_s.x);\
    float posY_s = P1P2.y * (pos_e.y + radius)  +  ABCD.y * pos_e.z;\
    posY_s = posY_s / -pos_e.z;\
    posY_s = (posY_s + 1.0f) / 2.0f;\
    radius_s.y = (posY_s - pos_s.y);\
    for (int i=0; i<8; ++i)\
    {   pointsample = reflect(pointsamples[i], axis);\
        samplepos_e_z = pos_e.z + pointsample.z;\
        samplepos_s_z = (ABCD.z * samplepos_e_z + ABCD.w) / -samplepos_e_z;\
        samplepos_s_z = (samplepos_s_z + 1.0f) / 2.0f;\
        samplepos_s.x = pos_s.x + pointsample.x * radius_s.x;\
        samplepos_s.y = pos_s.y + pointsample.y * radius_s.y;\
        samplepos_s.z  = tex2D(depthtexture, samplepos_s.xy).x;\
        ao = ao + step(samplepos_s.z, samplepos_s_z);\
    }\
    ao = ao / 8.0f;\
    ao = ao * intensity;\
    OUT.color = (ao);\
    return OUT;}";
            return pSourceAO1;
            break;


        case SVT_SHADER_AO_PASS1_LINE_BASED:
            static const char* pSourceAO2 = "\
struct FragmentInput\
{   float4 wpos           : WPOS;\
};\
struct FragmentOutput\
{   float color          : COLOR0;\
};\
FragmentOutput main( FragmentInput IN,\
                     uniform sampler2D depthtexture,\
                     uniform float4[12] linesamples,\
                     uniform float2 FBOsize,\
                     uniform float2 P1P2,\
                     uniform float4 ABCD,\
                     uniform float intensity,\
                     uniform float radius)\
{   FragmentOutput OUT;\
    float3 pos_s;\
    float3 pos_e;\
    float3 pos_n;\
    float3 samplepos_s;\
    float2 radius_s;\
    float  ao = 0.0f;\
    float  samplepos_e_z;\
    float  C_minus_one = ABCD.z - 1.0f;\
    float  minus_W     = ABCD.w * -1.0f;\
    float  d;\
    pos_s.x = IN.wpos.x / FBOsize.x;\
    pos_s.y = IN.wpos.y / FBOsize.y;\
    pos_s.z = tex2D(depthtexture, pos_s.xy).x;\
    if (pos_s.z == 1.0f)\
        discard;\
    pos_n = (pos_s * 2.0f - float3(1.0f));\
    pos_e.z = minus_W / (pos_n.z + ABCD.z);\
    pos_e.x = - pos_e.z * (ABCD.x + pos_n.x) / P1P2.x;\
    pos_e.y = - pos_e.z * (ABCD.y + pos_n.y) / P1P2.y;\
    float posX_s = P1P2.x * (pos_e.x + radius)  +  ABCD.x * pos_e.z;\
    posX_s = posX_s / -pos_e.z;\
    posX_s = (posX_s + 1.0f) /  2.0;\
    radius_s.x = (posX_s - pos_s.x);\
    float posY_s = P1P2.y * (pos_e.y + radius)  +  ABCD.y * pos_e.z;\
    posY_s = posY_s / -pos_e.z;\
    posY_s = (posY_s + 1.0f) / 2.0f;\
    radius_s.y = (posY_s - pos_s.y);\
    for (int i=0; i<12; ++i)\
    {   samplepos_s.x = pos_s.x + linesamples[i].x * radius_s.x;\
        samplepos_s.y = pos_s.y + linesamples[i].y * radius_s.y;\
        samplepos_s.z = tex2D(depthtexture, samplepos_s.xy).x;\
        samplepos_e_z = minus_W / (samplepos_s.z * 2.0f + C_minus_one);\
        d = pos_e.z - samplepos_e_z;\
        ao = ao + max(min(linesamples[i].z, d) + linesamples[i].z, 0.0f) * linesamples[i].w;\
    }\
    ao = 1.0f - ao;\
    ao = ao * intensity;\
    OUT.color = (ao);\
    return OUT;}";
            return pSourceAO2;
            break;

        case SVT_SHADER_AO_PASS2:
            static const char* pSourceAO3 = "\
struct FragmentInput\
{   float4 wpos           : WPOS;\
};\
struct FragmentOutput\
{   float color          : COLOR0;\
};\
FragmentOutput main( FragmentInput IN,\
                     uniform sampler2D aotexture,\
                     uniform sampler2D depthtexture,\
                     uniform float2 pixelsize,\
                     uniform float2 FBOsize)\
{   FragmentOutput OUT;\
    float2 screenpos;\
    float depth;\
    float ao=0.0f;\
    screenpos.x = IN.wpos.x / FBOsize.x;\
    screenpos.y = IN.wpos.y / FBOsize.y;\
    depth = tex2D(depthtexture, screenpos).x;\
    if (depth == 1.0f)\
        discard;\
    ao = ao +        tex2D(aotexture, float2(screenpos.x - 3.0f * pixelsize.x,  screenpos.y)).x;\
    ao = ao +        tex2D(aotexture, float2(screenpos.x - 2.0f * pixelsize.x,  screenpos.y)).x;\
    ao = ao +        tex2D(aotexture, float2(screenpos.x -        pixelsize.x,  screenpos.y)).x;\
    ao = ao +        tex2D(aotexture, float2(screenpos.x,                       screenpos.y)).x;\
    ao = ao +        tex2D(aotexture, float2(screenpos.x +        pixelsize.x,  screenpos.y)).x;\
    ao = ao +        tex2D(aotexture, float2(screenpos.x + 2.0f * pixelsize.x,  screenpos.y)).x;\
    ao = ao +        tex2D(aotexture, float2(screenpos.x + 3.0f * pixelsize.x,  screenpos.y)).x;\
    ao = ao/7.0f;\
    OUT.color = (ao);\
    return OUT;}";
            return pSourceAO3;
            break;


        case SVT_SHADER_AO_PASS3:
            static const char* pSourceAO4 = "\
struct FragmentInput\
{   float4 wpos           : WPOS;\
};\
struct FragmentOutput\
{   float  depth          : DEPTH;\
    float4 color          : COLOR0;\
};\
FragmentOutput main( FragmentInput IN,\
                     uniform sampler2D aotexture,\
                     uniform sampler2D colortexture,\
                     uniform sampler2D depthtexture,\
                     uniform float2 pixelsize,\
                     uniform float2 FBOsize)\
{   FragmentOutput OUT;\
    float4 color;\
    float2 screenpos;\
    float depth;\
    float ao = 0.0f;\
    screenpos.x = IN.wpos.x / FBOsize.x;\
    screenpos.y = IN.wpos.y / FBOsize.y;\
    depth = tex2D(depthtexture, screenpos).x;\
    if (depth == 1.0f)\
        discard;\
    color = tex2D(colortexture, screenpos);\
    ao = ao +        tex2D(aotexture, float2(screenpos.x,  screenpos.y - 3.0f * pixelsize.y)).x;\
    ao = ao +        tex2D(aotexture, float2(screenpos.x,  screenpos.y - 2.0f * pixelsize.y)).x;\
    ao = ao +        tex2D(aotexture, float2(screenpos.x,  screenpos.y -        pixelsize.y)).x;\
    ao = ao +        tex2D(aotexture, float2(screenpos.x,  screenpos.y)                     ).x;\
    ao = ao +        tex2D(aotexture, float2(screenpos.x,  screenpos.y +        pixelsize.y)).x;\
    ao = ao +        tex2D(aotexture, float2(screenpos.x,  screenpos.y + 2.0f * pixelsize.y)).x;\
    ao = ao +        tex2D(aotexture, float2(screenpos.x,  screenpos.y + 3.0f * pixelsize.y)).x;\
    ao = ao/7.0f;\
    ao = ao * sqrt(color.w);\
    color.xyz = color.xyz * (1.0f - ao);\
    color.w = 1;\
    OUT.color = color;\
    OUT.depth = depth;\
    return OUT;}";
            return pSourceAO4;
    }

    // avoid compiler warning...
    return NULL;
}


/**
 *
 */
const char* svt_shader_manager::getVDWShaderSource(int iShader)
{
    switch (iShader)
    {
        case SVT_SHADER_TOON_VDW_VERTEX:
            static const char * pShaderVert = "\
struct VertexInput\
{   float4 position       : POSITION;\
    float2 tex            : TEXCOORD0;\
    float4 color          : COLOR0;};\
struct VertexOutput\
{   float4 position       : POSITION;\
    float  radius         : TEXCOORD0;\
    float2 pos            : TEXCOORD1;\
    float3 centervec      : TEXCOORD2;\
    float4 color          : COLOR0;};\
VertexOutput main( VertexInput IN,\
                   uniform float4x4 MVP,\
                   uniform float4x4 MV,\
                   uniform float3   viewpos,\
                   uniform float3   up )\
{   VertexOutput OUT;\
    float3 viewvec = normalize(IN.position.xyz - viewpos);\
    float3 right = cross(up, viewvec);\
    float3 realup = cross( viewvec, right );\
    float4 position;\
    position.xyz = IN.position.xyz + (IN.tex.x * IN.position.w) * right + (IN.tex.y * IN.position.w) * realup;\
    position.w   = 1.0f;\
    OUT.position = mul(MVP, position);\
    position = float4(IN.position.xyz, 1);\
    float4 spherecenter = mul(MV, position);\
    OUT.centervec = spherecenter.xyz;\
    position = float4(IN.position.x + IN.position.w, IN.position.y, IN.position.z, 1);\
    float4 spherepoint  = mul(MV, position);\
    OUT.radius = length(spherepoint.xyz - spherecenter.xyz);\
    OUT.pos = IN.tex;\
    OUT.color = IN.color;\
    return OUT;}";
            return pShaderVert;
            break;


        case SVT_SHADER_TOON_VDW_FRAG_PLAIN:
            static const char * pShaderFrag = "\
struct FragmentInput\
{   float  radius         : TEXCOORD0;\
    float2 pos            : TEXCOORD1;\
    float3 centervec      : TEXCOORD2;\
    float4 color          : COLOR0;};\
struct FragmentOutput\
{   float  depth          : DEPTH;\
    float4 color          : COLOR0;};\
FragmentOutput main( FragmentInput IN,\
                     uniform float4 viewport,\
                     uniform float3 lightpos,\
                     uniform float2 z1z2,\
                     uniform float3 lighting,\
                     uniform float3 specular_color)\
{   FragmentOutput OUT;\
    float d = length(IN.pos);\
    if (d > 1.0f) discard;\
    float z = cos(asin(d));\
    float3 normal = normalize(float3(IN.pos.x, IN.pos.y, z));\
    float3 intersection = IN.centervec + (normal * IN.radius);\
    float3 color = IN.color.xyz * (lighting.x + lighting.y * z);\
    color = color + specular_color * pow(z, lighting.z);\
    color = color + IN.color.xyz * lighting.x;\
    OUT.color.xyz = color;\
    OUT.color.w = 1.0f;\
    OUT.depth = (z1z2.x / intersection.z) + z1z2.y;\
    return OUT;}";
            return pShaderFrag;
            break;


        case SVT_SHADER_TOON_VDW_FRAG_FOGLINEAR:
            static const char * pShaderFragment_FogLinear = "\
struct FragmentInput\
{   float  radius         : TEXCOORD0;\
    float2 pos            : TEXCOORD1;\
    float3 centervec      : TEXCOORD2;\
    float4 color          : COLOR0;};\
struct FragmentOutput\
{   float  depth          : DEPTH;\
    float4 color          : COLOR0;};\
FragmentOutput main( FragmentInput IN,\
                     uniform float4 viewport,\
                     uniform float3 lightpos,\
                     uniform float2 z1z2,\
                     uniform float3 lighting,\
                     uniform float3 specular_color,\
		     uniform float3 fog_color,\
		     uniform float  fog_start,\
		     uniform float  fog_end,\
                     uniform float  fog_zoom_offset)\
{   FragmentOutput OUT;\
    float d = length(IN.pos);\
    if (d > 1.0f) discard;\
    float z = cos(asin(d));\
    float3 normal = normalize(float3(IN.pos.x, IN.pos.y, z));\
    float3 intersection = IN.centervec + (normal * IN.radius);\
    float3 color = IN.color.xyz * (lighting.x + lighting.y * z);\
    color = color + specular_color * pow(z, lighting.z);\
    color = color + IN.color.xyz * lighting.x;\
    float F = (fog_end + intersection.z - fog_zoom_offset) / (fog_end - fog_start);\
    F = saturate(F);\
    color = color * F + (1.0f - F) * fog_color;\
    OUT.color.xyz = color;\
    OUT.color.w = 1.0f;\
    OUT.depth = (z1z2.x / intersection.z) + z1z2.y;\
    return OUT;}";
            return pShaderFragment_FogLinear;
            break;


        case SVT_SHADER_TOON_VDW_FRAG_FOGEXP:
            static const char * pShaderFragment_FogExp = "\
struct FragmentInput\
{   float  radius         : TEXCOORD0;\
    float2 pos            : TEXCOORD1;\
    float3 centervec      : TEXCOORD2;\
    float4 color          : COLOR0;};\
struct FragmentOutput\
{   float  depth          : DEPTH;\
    float4 color          : COLOR0;};\
FragmentOutput main( FragmentInput IN,\
                     uniform float4 viewport,\
                     uniform float3 lightpos,\
                     uniform float2 z1z2,\
                     uniform float3 lighting,\
                     uniform float3 specular_color,\
		     uniform float3 fog_color,\
		     uniform float  fog_density)\
{   FragmentOutput OUT;\
    float d = length(IN.pos);\
    if (d > 1.0f) discard;\
    float z = cos(asin(d));\
    float3 normal = normalize(float3(IN.pos.x, IN.pos.y, z));\
    float3 intersection = IN.centervec + (normal * IN.radius);\
    float3 color = IN.color.xyz * (lighting.x + lighting.y * z);\
    color = color + specular_color * pow(z, lighting.z);\
    color = color + IN.color.xyz * lighting.x;\
    float F = exp(fog_density * intersection.z);\
    F = saturate(F);\
    color = color * F + (1.0f - F) * fog_color;\
    OUT.color.xyz = color;\
    OUT.color.w = 1.0f;\
    OUT.depth = (z1z2.x / intersection.z) + z1z2.y;\
    return OUT;}";
            return pShaderFragment_FogExp;
            break;


        case SVT_SHADER_TOON_VDW_FRAG_FOGEXP2:
            static const char * pShaderFragment_FogExp2 = "\
struct FragmentInput\
{   float  radius         : TEXCOORD0;\
    float2 pos            : TEXCOORD1;\
    float3 centervec      : TEXCOORD2;\
    float4 color          : COLOR0;};\
struct FragmentOutput\
{   float  depth          : DEPTH;\
    float4 color          : COLOR0;};\
FragmentOutput main( FragmentInput IN,\
                     uniform float4 viewport,\
                     uniform float3 lightpos,\
                     uniform float2 z1z2,\
                     uniform float3 lighting,\
                     uniform float3 specular_color,\
		     uniform float3 fog_color,\
		     uniform float  fog_density)\
{   FragmentOutput OUT;\
    float d = length(IN.pos);\
    if (d > 1.0f) discard;\
    float z = cos(asin(d));\
    float3 normal = normalize(float3(IN.pos.x, IN.pos.y, z));\
    float3 intersection = IN.centervec + (normal * IN.radius);\
    float3 color = IN.color.xyz * (lighting.x + lighting.y * z);\
    color = color + specular_color * pow(z, lighting.z);\
    color = color + IN.color.xyz * lighting.x;\
    float F = exp(-1.0f * pow(fog_density * intersection.z,2));\
    F = saturate(F);\
    color = color * F + (1.0f - F) * fog_color;\
    OUT.color.xyz = color;\
    OUT.color.w = 1.0f;\
    OUT.depth = (z1z2.x / intersection.z) + z1z2.y;\
    return OUT;}";
            return pShaderFragment_FogExp2;
            break;
    }

    // avoid compiler warnings
    return NULL;
}
