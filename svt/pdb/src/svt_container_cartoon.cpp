/***************************************************************************
                          svt_container_cartoon
                          -------------
    begin                : 12/05/2006
    author               : Manuel Wahle, Stefan Birmanns
    email                : sculptor@biomachina.org, sculptor@biomachina.org
 ***************************************************************************/

#include <svt_container_cartoon.h>
#include <svt_window.h>
#include <svt_init.h>
#include <svt_pdb.h>


#ifndef WIN32
SVT_PFNGLDRAWRANGEELEMENTSEXTPROC svt_container_cartoon::glDrawRangeElementsEXT = NULL;
SVT_PFNGLMULTIDRAWELEMENTSEXTPROC svt_container_cartoon::glMultiDrawElementsEXT = NULL;
SVT_PFNGLMULTIDRAWARRAYSEXTPROC   svt_container_cartoon::glMultiDrawArraysEXT   = NULL;
#else
SVT_PFNGLDRAWELEMENTSEXTPROC      svt_container_cartoon::glDrawElementsEXT      = NULL;
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////// svt_container_cartoon ////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char * pTubeVertexShader_VertexLighting = "\
struct VertexInput\
{   float4 position    : POSITION;\
    float4 color       : COLOR0;\
    float4 tangent     : TEXCOORD0;\
    float3 norm        : NORMAL;\
};\
struct VertexOutput\
{   float4 position    : POSITION;\
    float4 color       : COLOR0;\
};\
VertexOutput main( VertexInput IN,\
		   uniform float4x4 MVP,\
		   uniform float3 viewPos,\
                   uniform float3 lighting,\
                   uniform float3 specular_color,\
		   uniform float angle,\
		   uniform float span)\
{   VertexOutput OUT;\
    float3 norm    = normalize( IN.norm.xyz );\
    float3 tangent = normalize( IN.tangent.xyz );\
    float3 binorm  = normalize( cross(tangent, norm) );\
    tangent = binorm * ( cos(angle + IN.tangent.w * span) * IN.position.w ) + norm * ( sin(angle + IN.tangent.w * span) * IN.position.w );\
    OUT.position.xyz = IN.position.xyz + tangent;\
    float3 color = IN.color.rgb * lighting.x;\
    float diffuse  = saturate(dot( normalize((viewPos - OUT.position.xyz)), normalize(tangent)));\
    color = color + (lighting.y * diffuse) * IN.color.rgb;\
    color = color + pow(diffuse, lighting.z) * specular_color;\
    OUT.color.xyz = color;\
    OUT.color.w = 1.0f;\
    OUT.position.w = 1.0f;\
    OUT.position = mul( MVP, OUT.position );\
    return OUT;} ";

const char * pTubeVertexShader_VertexLighting_Fog = "\
struct VertexInput\
{   float4 position    : POSITION;\
    float4 color       : COLOR0;\
    float4 tangent     : TEXCOORD0;\
    float3 norm        : NORMAL;\
};\
struct VertexOutput\
{   float4 position    : POSITION;\
    float4 color       : COLOR0;\
    float fogc         : FOG;\
};\
VertexOutput main( VertexInput IN,\
		   uniform float4x4 MV,\
		   uniform float4x4 MVP,\
		   uniform float3 viewPos,\
                   uniform float3 lighting,\
                   uniform float3 specular_color,\
		   uniform float  angle,\
		   uniform float  span)\
{   VertexOutput OUT;\
    float3 norm    = normalize( IN.norm.xyz );\
    float3 tangent = normalize( IN.tangent.xyz );\
    float3 binorm  = normalize( cross(tangent, norm) );\
    tangent = binorm * ( cos(angle + IN.tangent.w * span) * IN.position.w ) + norm * ( sin(angle + IN.tangent.w * span) * IN.position.w );\
    float4 position;\
    position.xyz = IN.position.xyz + tangent;\
    position.w = 1;\
    float3 color = IN.color.rgb * lighting.x;\
    float diffuse  = saturate(dot( normalize((viewPos - position.xyz)), normalize(tangent)));\
    color = color + (lighting.y * diffuse) * IN.color.rgb;\
    color = color + pow(diffuse, lighting.z) * specular_color;\
    float Z = MV._m20 * position.x + MV._m21 * position.y + MV._m22 * position.z + MV._m23;\
    OUT.fogc = abs(Z);\
    OUT.color.xyz = color;\
    OUT.color.w = 1.0f;\
    OUT.position.w = 1.0f;\
    OUT.position = mul( MVP, position );\
    return OUT;} ";

const char * pTubeVertexShader_PixelLighting = "\
struct VertexInput\
{   float4 position    : POSITION;\
    float4 color       : COLOR0;\
    float4 tangent     : TEXCOORD0;\
    float3 norm        : NORMAL;\
};\
struct VertexOutput\
{   float4 position    : POSITION;\
    float4 color       : COLOR0;\
    float3 obj_norm    : TEXCOORD0;\
    float3 obj_pos     : TEXCOORD1;\
    float  eye_z       : TEXCOORD2;\
};\
VertexOutput main( VertexInput IN,\
                   uniform float4x4 MVP,\
                   uniform float3 viewPos,\
                   uniform float3 lighting,\
                   uniform float3 specular_color,\
                   uniform float angle,\
                   uniform float span\
                 )\
{   VertexOutput OUT;\
    float3 norm    = normalize( IN.norm.xyz );\
    float3 tangent = normalize( IN.tangent.xyz );\
    float3 binorm  = normalize( cross(tangent, norm) );\
    tangent = binorm * ( cos(angle + IN.tangent.w * span) * IN.position.w ) + norm * ( sin(angle + IN.tangent.w * span) * IN.position.w );\
    OUT.position.xyz = IN.position.xyz + tangent;\
    OUT.color = IN.color;\
    OUT.color.a = 1;\
    OUT.obj_pos = OUT.position.xyz;\
    OUT.obj_norm = tangent;\
    OUT.position.w   = 1.0f;\
    OUT.position = mul( MVP, OUT.position );\
    OUT.eye_z = OUT.position.z / OUT.position.w;\
    return OUT;} ";

const char * pFragmentShader_PixelLighting = "\
struct FragmentInput\
{\
    float3 obj_norm  : TEXCOORD0;\
    float3 obj_pos   : TEXCOORD1;\
    float  eye_z     : TEXCOORD2;\
    float4 color     : COLOR0;\
};\
struct FragmentOutput\
{\
    float4 color     : COLOR0;\
};\
FragmentOutput main( FragmentInput IN,\
                     uniform float3 lighting,\
                     uniform float3 specular_color,\
		     uniform float3 viewPos)\
{   FragmentOutput OUT;\
    float3 normal = normalize(IN.obj_norm);\
    float diffuse = saturate(dot( normalize((viewPos - IN.obj_pos)), normal ));\
    float specular = pow(diffuse, lighting.z);\
    float3 color = IN.color.rgb * lighting.x;\
    color = color + IN.color.rgb * (lighting.y * diffuse);\
    color = color + specular * specular_color;\
    OUT.color.rgb = color;\
    OUT.color.a = 1.0f;\
    return OUT;} ";

const char * pFragmentShader_PixelLighting_Fog_Linear = "\
struct FragmentInput\
{\
    float3 obj_norm  : TEXCOORD0;\
    float3 obj_pos   : TEXCOORD1;\
    float  eye_z     : TEXCOORD2;\
    float4 color     : COLOR0;\
};\
struct FragmentOutput\
{\
    float4 color     : COLOR0;\
};\
FragmentOutput main( FragmentInput IN,\
		     uniform float4x4 MV,\
		     uniform float3 fog_color,\
		     uniform float  fog_start,\
		     uniform float  fog_end,\
		     uniform float  fog_zoom_offset,\
                     uniform float3 lighting,\
                     uniform float3 specular_color,\
		     uniform float3 viewPos)\
{   FragmentOutput OUT;\
    float3 normal = normalize(IN.obj_norm);\
    float diffuse = saturate(dot( normalize((viewPos - IN.obj_pos)), normal ));	\
    float specular = pow(diffuse, lighting.z);\
    float3 color = IN.color.rgb * (lighting.x + lighting.y * diffuse);\
    color = color + specular * specular_color;\
    float Z = MV._m20 * IN.obj_pos.x + MV._m21 * IN.obj_pos.y + MV._m22 * IN.obj_pos.z + MV._m23;\
    float F = (fog_end + Z - fog_zoom_offset) / (fog_end - fog_start);\
    F = saturate(F);\
    color = color * F + (1.0f - F) * fog_color;\
    OUT.color.rgb = color;\
    OUT.color.a = 1.0f;\
    return OUT;} ";

const char * pFragmentShader_PixelLighting_Fog_Exp = "\
struct FragmentInput\
{\
    float3 obj_norm  : TEXCOORD0;\
    float3 obj_pos   : TEXCOORD1;\
    float  eye_z     : TEXCOORD2;\
    float4 color     : COLOR0;\
};\
struct FragmentOutput\
{\
    float4 color     : COLOR0;\
};\
FragmentOutput main( FragmentInput IN,\
		     uniform float4x4 MV,\
		     uniform float3 fog_color,\
		     uniform float  fog_density,\
                     uniform float3 lighting,\
                     uniform float3 specular_color,\
		     uniform float3 viewPos)\
{   FragmentOutput OUT;\
    float3 normal = normalize(IN.obj_norm);\
    float diffuse = saturate(dot( normalize((viewPos - IN.obj_pos)), normal ));\
    float specular = pow(diffuse, lighting.z);\
    float3 color = IN.color.rgb * (lighting.x + lighting.y * diffuse);\
    color = color + specular * specular_color;\
    float Z = MV._m20 * IN.obj_pos.x + MV._m21 * IN.obj_pos.y + MV._m22 * IN.obj_pos.z + MV._m23;\
    float F = exp(fog_density * Z);\
    F = saturate(F);\
    color = color * F + (1.0f - F) * fog_color;\
    OUT.color.rgb = color;\
    OUT.color.a = 1.0f;\
    return OUT;} ";

const char * pFragmentShader_PixelLighting_Fog_Exp2 = "\
struct FragmentInput\
{\
    float3 obj_norm  : TEXCOORD0;\
    float3 obj_pos   : TEXCOORD1;\
    float  eye_z     : TEXCOORD2;\
    float4 color     : COLOR0;\
};\
struct FragmentOutput\
{\
    float4 color     : COLOR0;\
};\
FragmentOutput main( FragmentInput IN,\
		     uniform float4x4 MV,\
		     uniform float3 fog_color,\
		     uniform float  fog_density,\
                     uniform float3 lighting,\
                     uniform float3 specular_color,\
		     uniform float3 viewPos)\
{   FragmentOutput OUT;\
    float3 normal = normalize(IN.obj_norm);\
    float diffuse = saturate(dot( normalize((viewPos - IN.obj_pos)), normal ));	\
    float specular = pow(diffuse, lighting.z);\
    float3 color = IN.color.rgb * (lighting.x + lighting.y * diffuse);\
    color = color + specular * specular_color;\
    float Z = MV._m20 * IN.obj_pos.x + MV._m21 * IN.obj_pos.y + MV._m22 * IN.obj_pos.z + MV._m23;\
    float F = exp(-1.0f * pow(fog_density * Z,2));\
    F = saturate(F);\
    color = color * F + (1.0f - F) * fog_color;\
    OUT.color.rgb = color;\
    OUT.color.a = 1.0f;\
    return OUT;} ";

const char * pFragmentShader_PixelLighting_FrontAndBack = "\
struct FragmentInput\
{\
    float3 obj_norm  : TEXCOORD0;\
    float3 obj_pos   : TEXCOORD1;\
    float4 color     : COLOR0;\
};\
struct FragmentOutput\
{\
    float4 color     : COLOR0;\
};\
FragmentOutput main( FragmentInput IN,\
                     uniform float3 lighting,\
                     uniform float3 specular_color,\
		     uniform float3 viewPos)\
{   FragmentOutput OUT;\
    float3 normal = normalize(IN.obj_norm);\
    float diffuse = saturate(abs(dot( normalize((viewPos - IN.obj_pos)), normal )));\
    float specular = pow(diffuse, lighting.z);\
    float3 color = IN.color.rgb * lighting.x;\
    color = color + IN.color.rgb * (lighting.y * diffuse);\
    color = color + specular * specular_color;\
    OUT.color.rgb = color;\
    OUT.color.a = 1.0f;\
    return OUT;} ";

const char * pFragmentShader_PixelLighting_Fog_Linear_FrontAndBack = "\
struct FragmentInput\
{\
    float3 obj_norm  : TEXCOORD0;\
    float3 obj_pos   : TEXCOORD1;\
    float4 color     : COLOR0;\
    float4 wpos      : WPOS;\
};\
struct FragmentOutput\
{\
    float4 color     : COLOR0;\
};\
FragmentOutput main( FragmentInput IN,\
		     uniform float4x4 MV,\
		     uniform float3 fog_color,\
		     uniform float  fog_zoom_offset,\
		     uniform float  fog_start,\
		     uniform float  fog_end,\
                     uniform float3 lighting,\
                     uniform float3 specular_color,\
		     uniform float3 viewPos)\
{   FragmentOutput OUT;\
    float3 normal = normalize(IN.obj_norm);\
    float diffuse = saturate(abs(dot( normalize((viewPos - IN.obj_pos)), normal )));\
    float specular = pow(diffuse, lighting.z);\
    float3 color = IN.color.rgb * (lighting.x + lighting.y * diffuse);\
    color = color + specular * specular_color;\
    float Z = MV._m20 * IN.obj_pos.x + MV._m21 * IN.obj_pos.y + MV._m22 * IN.obj_pos.z + MV._m23;\
    float F = (fog_end + Z - fog_zoom_offset) / (fog_end - fog_start);\
    F = saturate(F);\
    color = color * F + (1.0f - F) * fog_color;\
    OUT.color.rgb = color;\
    OUT.color.a = 1.0f;\
    return OUT;} ";

const char * pFragmentShader_PixelLighting_Fog_Exp_FrontAndBack = "\
struct FragmentInput\
{\
    float3 obj_norm  : TEXCOORD0;\
    float3 obj_pos   : TEXCOORD1;\
    float4 color     : COLOR0;\
    float4 wpos      : WPOS;\
};\
struct FragmentOutput\
{\
    float4 color     : COLOR0;\
};\
FragmentOutput main( FragmentInput IN,\
		     uniform float4x4 MV,\
		     uniform float3 fog_color,\
		     uniform float  fog_density,\
                     uniform float3 lighting,\
                     uniform float3 specular_color,\
		     uniform float3 viewPos)\
{   FragmentOutput OUT;\
    float3 normal = normalize(IN.obj_norm);\
    float diffuse = saturate(abs(dot( normalize((viewPos - IN.obj_pos)), normal )));\
    float specular = pow(diffuse, lighting.z);\
    float3 color = IN.color.rgb * (lighting.x + lighting.y * diffuse);\
    color = color + specular * specular_color;\
    float Z = MV._m20 * IN.obj_pos.x + MV._m21 * IN.obj_pos.y + MV._m22 * IN.obj_pos.z + MV._m23;\
    float F = exp(fog_density * Z);\
    F = saturate(F);\
    color = color * F + (1.0f - F) * fog_color;\
    OUT.color.rgb = color;\
    OUT.color.a = 1.0f;\
    return OUT;} ";

const char * pFragmentShader_PixelLighting_Fog_Exp2_FrontAndBack = "\
struct FragmentInput\
{\
    float3 obj_norm  : TEXCOORD0;\
    float3 obj_pos   : TEXCOORD1;\
    float4 color     : COLOR0;\
    float4 wpos      : WPOS;\
};\
struct FragmentOutput\
{\
    float4 color     : COLOR0;\
};\
FragmentOutput main( FragmentInput IN,\
		     uniform float4x4 MV,\
		     uniform float3 fog_color,\
		     uniform float  fog_density,\
                     uniform float3 lighting,\
                     uniform float3 specular_color,\
		     uniform float3 viewPos)\
{   FragmentOutput OUT;\
    float3 normal = normalize(IN.obj_norm);\
    float diffuse = saturate(abs(dot( normalize((viewPos - IN.obj_pos)), normal )));\
    float specular = pow(diffuse, lighting.z);\
    float3 color = IN.color.rgb * (lighting.x + lighting.y * diffuse);\
    color = color + specular * specular_color;\
    float Z = MV._m20 * IN.obj_pos.x + MV._m21 * IN.obj_pos.y + MV._m22 * IN.obj_pos.z + MV._m23;\
    float F = exp(-1.0f * pow(fog_density * Z,2));\
    F = saturate(F);\
    color = color * F + (1.0f - F) * fog_color;\
    OUT.color.rgb = color;\
    OUT.color.a = 1.0f;\
    return OUT;} ";

const char * pCartoonVertexShaderRound_VertexLighting = "\
struct VertexInput\
{   float4 position    : POSITION;\
    float4 color       : COLOR0;\
    float4 tangent     : TEXCOORD0;\
    float3 norm        : NORMAL;\
};\
struct VertexOutput\
{   float4 position    : POSITION;\
    float4 color       : COLOR0;\
};\
VertexOutput main( VertexInput IN,\
		   uniform float4x4 MVP,\
		   uniform float3 viewPos,\
                   uniform float3 lighting,\
                   uniform float3 specular_color,\
		   uniform float r1,\
		   uniform float r2,\
		   uniform float angle,\
		   uniform float span)\
{   VertexOutput OUT;\
    float3 norm    = normalize( IN.norm.xyz );\
    float3 tangent = normalize( IN.tangent.xyz - IN.position.xyz );\
    float3 binorm  = normalize( cross(tangent, norm) );\
    float3 normal = binorm * ( cos(angle + IN.tangent.w * span) * (IN.position.w * r2) ) + norm * ( sin(angle + IN.tangent.w * span) * (IN.position.w * r1) );\
    OUT.position.xyz = IN.position.xyz + normal;\
    normal = binorm * ( cos(angle + IN.tangent.w * span) ) + norm * ( sin(angle + IN.tangent.w * span) );\
    float diffuse  = dot( normalize((viewPos - OUT.position.xyz)), normalize(normal) );\
    OUT.color.rgb = saturate( IN.color.rgb * (lighting.y * diffuse + lighting.x));\
    OUT.color.rgb = OUT.color.rgb + max(0, pow(diffuse, lighting.z)) * specular_color;\
    OUT.color.w = 1.0f;\
    OUT.position.w = 1.0f;\
    OUT.position = mul( MVP, OUT.position );\
    return OUT;} ";

const char * pCartoonVertexShaderRound_VertexLighting_Fog = "\
struct VertexInput\
{   float4 position    : POSITION;\
    float4 color       : COLOR0;\
    float4 tangent     : TEXCOORD0;\
    float3 norm        : NORMAL;\
};\
struct VertexOutput\
{   float4 position    : POSITION;\
    float4 color       : COLOR0;\
    float fogc         : FOG;\
};\
VertexOutput main( VertexInput IN,\
		   uniform float4x4 MV,\
		   uniform float4x4 MVP,\
		   uniform float3 viewPos,\
                   uniform float3 lighting,\
                   uniform float3 specular_color,\
		   uniform float r1,\
		   uniform float r2,\
		   uniform float angle,\
		   uniform float span)\
{   VertexOutput OUT;\
    float3 norm    = normalize( IN.norm.xyz );\
    float3 tangent = normalize( IN.tangent.xyz - IN.position.xyz );\
    float3 binorm  = normalize( cross(tangent, norm) );\
    float3 normal = binorm * ( cos(angle + IN.tangent.w * span) * (IN.position.w * r2) ) + norm * ( sin(angle + IN.tangent.w * span) * (IN.position.w * r1) );\
    float4 position;\
    position.xyz = IN.position.xyz + normal;\
    normal = binorm * ( cos(angle + IN.tangent.w * span) ) + norm * ( sin(angle + IN.tangent.w * span) );\
    float diffuse  = dot( normalize((viewPos - position.xyz)), normalize(normal) );\
    float3 color;\
    color = saturate( IN.color.rgb * (lighting.y * diffuse + lighting.x));\
    color = color + max(0, pow(diffuse, lighting.z)) * specular_color;\
    float Z = MV._m20 * position.x + MV._m21 * position.y + MV._m22 * position.z + MV._m23;\
    OUT.fogc = abs(Z);							\
    OUT.color.rgb = color;\
    OUT.color.w = 1.0f;\
    position.w = 1.0f;\
    OUT.position = mul( MVP, position );\
    return OUT;} ";

const char * pCartoonVertexShaderRound_PixelLighting = "\
struct VertexInput\
{   float4 position    : POSITION;\
    float4 color       : COLOR0;\
    float4 tangent     : TEXCOORD0;\
    float3 norm        : NORMAL;\
};\
struct VertexOutput\
{   float4 position    : POSITION;\
    float4 color       : COLOR0;\
    float3 obj_norm  : TEXCOORD0;\
    float3 obj_pos   : TEXCOORD1;\
};\
VertexOutput main( VertexInput IN,\
		   uniform float4x4 MVP,\
		   uniform float3 viewPos,\
                   uniform float3 lighting,\
                   uniform float3 specular_color,\
		   uniform float r1,\
		   uniform float r2,\
		   uniform float angle,\
		   uniform float span)\
{   VertexOutput OUT;\
    float3 norm    = normalize( IN.norm.xyz );\
    float3 tangent = normalize( IN.tangent.xyz - IN.position.xyz );\
    float3 binorm  = normalize( cross(tangent, norm) );\
    float3 normal = binorm * ( cos(angle + IN.tangent.w * span) * (IN.position.w * r2) ) + norm * ( sin(angle + IN.tangent.w * span) * (IN.position.w * r1) );\
    OUT.position.xyz = IN.position.xyz + normal;\
    OUT.position.w = 1.0f;\
    OUT.obj_pos = OUT.position.xyz;\
    OUT.position = mul( MVP, OUT.position );\
    normal = binorm * ( cos(angle + IN.tangent.w * span) ) + norm * ( sin(angle + IN.tangent.w * span) );\
    OUT.obj_norm = normal;\
    OUT.color = IN.color;\
    return OUT;} ";

const char * pCartoonVertexShaderFlatHelix_VertexLighting = "\
struct VertexInput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float4 next      : TEXCOORD0;\
    float3 binorm    : NORMAL;\
};\
struct VertexOutput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
};\
VertexOutput main( VertexInput IN,\
		   uniform float4x4 MVP,\
		   uniform float    height,\
                   uniform float3   lighting,\
                   uniform float3   specular_color,\
		   uniform float3   viewPos)\
{   VertexOutput OUT;\
    float3 binorm = normalize(IN.binorm);\
    float3 norm   = normalize(cross((IN.next.xyz - IN.position.xyz), binorm));\
    float3 position = IN.position.xyz;\
    position = position + binorm * (IN.position.w * IN.next.w);\
    OUT.position   = mul( MVP, float4(position,1) );\
    float diffuse  = saturate(dot( normalize((viewPos - OUT.position.xyz)), norm ));\
    OUT.color.rgb = IN.color.rgb * (lighting.x + lighting.y * diffuse);\
    OUT.color.rgb = OUT.color.rgb + pow(diffuse, lighting.z) * specular_color; \
    OUT.color.w = 1.0f;\
    return OUT;} ";

const char * pCartoonVertexShaderFlatHelix_VertexLighting_Fog = "\
struct VertexInput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float4 next      : TEXCOORD0;\
    float3 binorm    : NORMAL;\
};\
struct VertexOutput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float  fogc      : FOG;\
};\
VertexOutput main( VertexInput IN,\
		   uniform float4x4 MV,\
		   uniform float4x4 MVP,\
		   uniform float    height,\
                   uniform float3   lighting,\
                   uniform float3   specular_color,\
		   uniform float3   viewPos)\
{   VertexOutput OUT;\
    float3 binorm = normalize(IN.binorm);\
    float3 norm   = normalize(cross((IN.next.xyz - IN.position.xyz), binorm));\
    float3 position = IN.position.xyz;\
    position = position + binorm * (IN.position.w * IN.next.w);\
    OUT.position   = mul( MVP, float4(position,1) );\
    float Z = MV._m20 * position.x + MV._m21 * position.y + MV._m22 * position.z + MV._m23; \
    OUT.fogc = abs(Z);							\
    float diffuse  = saturate(dot( normalize((viewPos - position)), norm ));\
    OUT.color.rgb = IN.color.rgb * (lighting.x + lighting.y * diffuse);\
    OUT.color.rgb = OUT.color.rgb + pow(diffuse, lighting.z) * specular_color; \
    OUT.color.w = 1.0f;\
    return OUT;} ";

const char * pCartoonVertexShaderFlatHelix_PixelLighting = "\
struct VertexInput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float4 next      : TEXCOORD0;\
    float3 binorm    : NORMAL;\
};\
struct VertexOutput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float3 obj_norm  : TEXCOORD0;\
    float3 obj_pos   : TEXCOORD1;\
};\
VertexOutput main( VertexInput IN,\
                   uniform float3   lighting,\
                   uniform float3   height,\
                   uniform float3   specular_color,\
		   uniform float3   viewPos,\
		   uniform float4x4 MVP)\
{   VertexOutput OUT;\
    float3 binorm = normalize(IN.binorm);\
    float3 norm   = normalize(cross((IN.next.xyz - IN.position.xyz), binorm));\
    float3 position = IN.position.xyz;\
    position = position + binorm * (IN.position.w * IN.next.w);\
    OUT.obj_pos = position;\
    OUT.obj_norm = norm;\
    OUT.color = IN.color;\
    OUT.position   = mul( MVP, float4(position,1) );\
    return OUT;} ";

const char * pCartoonVertexShaderFlatSheet_VertexLighting = "\
struct VertexInput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float4 next      : TEXCOORD0;\
    float3 binorm    : NORMAL;\
};\
struct VertexOutput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
};\
VertexOutput main( VertexInput IN,\
		   uniform float4x4 MVP,\
                   uniform float3   height,\
                   uniform float3   lighting,\
                   uniform float3   specular_color,\
		   uniform float3   viewPos)\
{   VertexOutput OUT;\
    float3 binorm = normalize(IN.binorm);\
    float3 norm   = normalize(cross((IN.next.xyz - IN.position.xyz), binorm));\
    float3 position = IN.position.xyz;\
    position = position + binorm * (IN.position.w * IN.next.w);\
    OUT.position   = mul( MVP, float4(position,1) );\
    float3 color = IN.color;\
    float diffuse = abs(dot(normalize(viewPos - position), norm));\
    color = color * (lighting.x + lighting.y * diffuse);\
    color = color + pow(diffuse, lighting.z) * specular_color; \
    OUT.color.xyz = color;\
    OUT.color.w = 1;\
    return OUT;} ";

const char * pCartoonVertexShaderFlatSheet_VertexLighting_Fog = "\
struct VertexInput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float4 next      : TEXCOORD0;\
    float3 binorm    : NORMAL;\
};\
struct VertexOutput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float  fogc      : FOG;\
};\
VertexOutput main( VertexInput IN,\
		   uniform float4x4 MV,\
		   uniform float4x4 MVP,\
		   uniform float    height,\
                   uniform float3   lighting,\
                   uniform float3   specular_color,\
		   uniform float3   viewPos)\
{   VertexOutput OUT;\
    float3 binorm = normalize(IN.binorm);\
    float3 norm   = normalize(cross((IN.next.xyz - IN.position.xyz), binorm));\
    float3 position = IN.position.xyz;\
    position = position + binorm * (IN.position.w * IN.next.w);\
    OUT.position   = mul( MVP, float4(position,1) );\
    float Z = MV._m20 * position.x + MV._m21 * position.y + MV._m22 * position.z + MV._m23; \
    OUT.fogc = abs(Z);\
    float3 color = IN.color;\
    float diffuse = abs(dot(normalize(viewPos - position), norm));\
    color = color * (lighting.x + lighting.y * diffuse);\
    color = color + pow(diffuse, lighting.z) * specular_color; \
    OUT.color.xyz = color;\
    OUT.color.w = 1;\
    return OUT;} ";

const char * pCartoonVertexShaderFlatSheet_PixelLighting = "\
struct VertexInput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float4 next      : TEXCOORD0;\
    float3 binorm    : NORMAL;\
};\
struct VertexOutput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float3 obj_norm  : TEXCOORD0;\
    float3 obj_pos   : TEXCOORD1;\
};\
VertexOutput main( VertexInput IN,\
		   uniform float    height,\
                   uniform float3   lighting,\
                   uniform float3   specular_color,\
		   uniform float3   viewPos,\
		   uniform float4x4 MVP)\
{   VertexOutput OUT;\
    float3 binorm = normalize(IN.binorm);\
    float3 norm   = normalize(cross((IN.next.xyz - IN.position.xyz), binorm));\
    float3 position = IN.position.xyz;\
    position = position + binorm * (IN.position.w * IN.next.w);\
    OUT.obj_pos = position;\
    OUT.obj_norm = norm;\
    OUT.color = IN.color;\
    OUT.position = mul( MVP, float4(position,1) );\
    return OUT;} ";

const char * pVert0_VertexLighting = "\
struct VertexInput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float4 next      : TEXCOORD0;\
    float3 binorm    : NORMAL;\
};\
struct VertexOutput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
};\
VertexOutput main( VertexInput IN,\
		   uniform float4x4 MVP,\
		   uniform float    height,\
                   uniform float3   lighting,\
                   uniform float3   specular_color,\
		   uniform float3   viewPos)\
{   VertexOutput OUT;\
    float3 binorm = normalize(IN.binorm);\
    float3 norm   = normalize(cross((IN.next.xyz - IN.position.xyz), binorm));\
    float3 position = IN.position.xyz;\
    position = position + binorm * (IN.position.w * IN.next.w);\
    position = position + norm * height;\
    OUT.position   = mul( MVP, float4(position,1) );\
    float3 color = IN.color;\
    color = color * (lighting.x + saturate(lighting.y * dot(normalize(viewPos - position), norm)));\
    OUT.color.xyz = color;\
    OUT.color.w = 1;\
    return OUT;} ";

const char * pVert1_VertexLighting = "\
struct VertexInput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float4 next      : TEXCOORD0;\
    float3 binorm    : NORMAL;\
};\
struct VertexOutput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
};\
VertexOutput main( VertexInput IN,\
		   uniform float4x4 MVP,\
		   uniform float    height,\
                   uniform float3   lighting,\
                   uniform float3   specular_color,\
		   uniform float3   viewPos)\
{   VertexOutput OUT;\
    float3 binorm = normalize(IN.binorm);\
    float3 norm   = normalize(cross(binorm, (IN.next.xyz - IN.position.xyz)));\
    float3 position = IN.position.xyz;\
    position = position + binorm * (IN.position.w * IN.next.w);\
    position = position + norm * height;\
    OUT.position   = mul( MVP, float4(position,1) );\
    float3 color = IN.color;\
    color = color * (lighting.x + saturate(lighting.y * dot(normalize(viewPos - position), norm)));\
    OUT.color.xyz = color;\
    OUT.color.w = 1;\
    return OUT;} ";

const char * pVert2_VertexLighting = "\
struct VertexInput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float4 next      : TEXCOORD0;\
    float3 binorm    : NORMAL;\
};\
struct VertexOutput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
};\
VertexOutput main( VertexInput IN,\
		   uniform float4x4 MVP,\
		   uniform float    height,\
                   uniform float3   lighting,\
                   uniform float3   specular_color,\
		   uniform float3   viewPos)\
{   VertexOutput OUT;\
    float3 binorm = normalize(IN.binorm);\
    float3 norm   = normalize(cross((IN.next.xyz - IN.position.xyz), binorm));\
    float3 position = IN.position.xyz;\
    position = position + binorm * IN.position.w;\
    position = position + norm * (IN.next.w * height);\
    OUT.position   = mul( MVP, float4(position,1) );\
    float3 color = IN.color;\
    color = color * (lighting.x + saturate(lighting.y * dot(normalize(viewPos - position), binorm)));\
    OUT.color.xyz = color;\
    OUT.color.w = 1;\
    return OUT;} ";

const char * pVert3_VertexLighting = "\
struct VertexInput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float4 next      : TEXCOORD0;\
    float3 binorm    : NORMAL;\
};\
struct VertexOutput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
};\
VertexOutput main( VertexInput IN,\
		   uniform float4x4 MVP,\
		   uniform float    height,\
                   uniform float3   lighting,\
                   uniform float3   specular_color,\
		   uniform float3   viewPos)\
{   VertexOutput OUT;\
    float3 binorm = normalize(-1.0f * IN.binorm);\
    float3 norm   = normalize(cross((IN.next.xyz - IN.position.xyz), binorm));\
    float3 position = IN.position.xyz;\
    position = position + binorm * IN.position.w;\
    position = position + norm * (IN.next.w * height);\
    OUT.position   = mul( MVP, float4(position,1) );\
    float3 color = IN.color;\
    color = color * (lighting.x + saturate(lighting.y * dot(normalize(viewPos - position), binorm)));\
    OUT.color.xyz = color;\
    OUT.color.w = 1;\
    return OUT;} ";

const char * pVert0_VertexLighting_Fog = "\
struct VertexInput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float4 next      : TEXCOORD0;\
    float3 binorm    : NORMAL;\
};\
struct VertexOutput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float  fogc      : FOG;\
};\
VertexOutput main( VertexInput IN,\
		   uniform float4x4 MV,\
		   uniform float4x4 MVP,\
		   uniform float    height,\
                   uniform float3   lighting,\
                   uniform float3   specular_color,\
		   uniform float3   viewPos)\
{   VertexOutput OUT;\
    float3 binorm = normalize(IN.binorm);\
    float3 norm   = normalize(cross((IN.next.xyz - IN.position.xyz), binorm));\
    float3 position = IN.position.xyz;\
    position = position + binorm * (IN.position.w * IN.next.w);\
    position = position + norm * height;\
    float Z = MV._m20 * position.x + MV._m21 * position.y + MV._m22 * position.z + MV._m23;\
    OUT.fogc = abs(Z);\
    OUT.position   = mul( MVP, float4(position,1) );\
    float3 color = IN.color;\
    color = color * (lighting.x + saturate(lighting.y * dot(normalize(viewPos - position), norm)));\
    OUT.color.xyz = color;\
    OUT.color.w = 1;\
    return OUT;} ";

const char * pVert1_VertexLighting_Fog = "\
struct VertexInput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float4 next      : TEXCOORD0;\
    float3 binorm    : NORMAL;\
};\
struct VertexOutput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float  fogc      : FOG;\
};\
VertexOutput main( VertexInput IN,\
		   uniform float4x4 MV,\
		   uniform float4x4 MVP,\
		   uniform float    height,\
                   uniform float3   lighting,\
                   uniform float3   specular_color,\
		   uniform float3   viewPos)\
{   VertexOutput OUT;\
    float3 binorm = normalize(IN.binorm);\
    float3 norm   = normalize(cross(binorm, (IN.next.xyz - IN.position.xyz)));\
    float3 position = IN.position.xyz;\
    position = position + binorm * (IN.position.w * IN.next.w);\
    position = position + norm * height;\
    float Z = MV._m20 * position.x + MV._m21 * position.y + MV._m22 * position.z + MV._m23;\
    OUT.fogc = abs(Z);\
    OUT.position   = mul( MVP, float4(position,1) );\
    float3 color = IN.color;\
    color = color * (lighting.x + saturate(lighting.y * dot(normalize(viewPos - position), norm)));\
    OUT.color.xyz = color;\
    OUT.color.w = 1;\
    return OUT;} ";

const char * pVert2_VertexLighting_Fog = "\
struct VertexInput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float4 next      : TEXCOORD0;\
    float3 binorm    : NORMAL;\
};\
struct VertexOutput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float  fogc      : FOG;\
};\
VertexOutput main( VertexInput IN,\
		   uniform float4x4 MV,\
		   uniform float4x4 MVP,\
		   uniform float    height,\
                   uniform float3   lighting,\
                   uniform float3   specular_color,\
		   uniform float3   viewPos)\
{   VertexOutput OUT;\
    float3 binorm = normalize(IN.binorm);\
    float3 norm   = normalize(cross((IN.next.xyz - IN.position.xyz), binorm));\
    float3 position = IN.position.xyz;\
    position = position + binorm * IN.position.w;\
    position = position + norm * (IN.next.w * height);\
    float Z = MV._m20 * position.x + MV._m21 * position.y + MV._m22 * position.z + MV._m23;\
    OUT.fogc = abs(Z);\
    OUT.position   = mul( MVP, float4(position,1) );\
    float3 color = IN.color;\
    color = color * (lighting.x + saturate(lighting.y * dot(normalize(viewPos - position), binorm)));\
    OUT.color.xyz = color;\
    OUT.color.w = 1;\
    return OUT;} ";

const char * pVert3_VertexLighting_Fog = "\
struct VertexInput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float4 next      : TEXCOORD0;\
    float3 binorm    : NORMAL;\
};\
struct VertexOutput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float  fogc      : FOG;\
};\
VertexOutput main( VertexInput IN,\
		   uniform float4x4 MV,\
		   uniform float4x4 MVP,\
		   uniform float    height,\
                   uniform float3   lighting,\
                   uniform float3   specular_color,\
		   uniform float3   viewPos)\
{   VertexOutput OUT;\
    float3 binorm = normalize(-1.0f * IN.binorm);\
    float3 norm   = normalize(cross((IN.next.xyz - IN.position.xyz), binorm));\
    float3 position = IN.position.xyz;\
    position = position + binorm * IN.position.w;\
    position = position + norm * (IN.next.w * height);\
    float Z = MV._m20 * position.x + MV._m21 * position.y + MV._m22 * position.z + MV._m23;\
    OUT.fogc = abs(Z);\
    OUT.position   = mul( MVP, float4(position,1) );\
    float3 color = IN.color;\
    color = color * (lighting.x + saturate(lighting.y * dot(normalize(viewPos - position), binorm)));\
    OUT.color.xyz = color;\
    OUT.color.w = 1;\
    return OUT;} ";

const char * pVert0_PixelLighting = "\
struct VertexInput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float4 next      : TEXCOORD0;\
    float3 binorm    : NORMAL;\
};\
struct VertexOutput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float3 obj_norm  : TEXCOORD0;\
    float3 obj_pos   : TEXCOORD1;\
};\
VertexOutput main( VertexInput IN,\
		   uniform float4x4 MVP,\
                   uniform float3   lighting,\
                   uniform float3   specular_color,\
		   uniform float3   viewPos,\
		   uniform float    height)\
{   VertexOutput OUT;\
    float3 binorm = normalize(IN.binorm);\
    float3 norm   = normalize(cross((IN.next.xyz - IN.position.xyz), binorm));\
    float3 position = IN.position.xyz;\
    position = position + binorm * (IN.position.w * IN.next.w);\
    position = position + norm * height;\
    OUT.obj_norm = norm;\
    OUT.obj_pos = position;\
    OUT.position = mul( MVP, float4(position,1) );\
    OUT.color = IN.color;\
    return OUT;} ";

const char * pVert1_PixelLighting = "\
struct VertexInput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float4 next      : TEXCOORD0;\
    float3 binorm    : NORMAL;\
};\
struct VertexOutput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float3 obj_norm  : TEXCOORD0;\
    float3 obj_pos   : TEXCOORD1;\
};\
VertexOutput main( VertexInput IN,\
                   uniform float3   lighting,\
                   uniform float3   specular_color,\
		   uniform float3   viewPos,\
		   uniform float4x4 MVP,\
		   uniform float    height)\
{   VertexOutput OUT;\
    float3 binorm = normalize(IN.binorm);\
    float3 norm   = normalize(cross(binorm, (IN.next.xyz - IN.position.xyz)));\
    float3 position = IN.position.xyz;\
    position = position + binorm * (IN.position.w * IN.next.w);\
    position = position + norm * height;\
    OUT.obj_norm = norm;\
    OUT.obj_pos = position;\
    OUT.position   = mul( MVP, float4(position,1) );\
    OUT.color = IN.color;\
    return OUT;} ";

const char * pVert2_PixelLighting = "\
struct VertexInput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float4 next      : TEXCOORD0;\
    float3 binorm    : NORMAL;\
};\
struct VertexOutput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float3 obj_norm  : TEXCOORD0;\
    float3 obj_pos   : TEXCOORD1;\
};\
VertexOutput main( VertexInput IN,\
                   uniform float3   lighting,\
                   uniform float3   specular_color,\
		   uniform float3   viewPos,\
		   uniform float4x4 MVP,\
		   uniform float    height)	\
{   VertexOutput OUT;\
    float3 binorm = normalize(IN.binorm);\
    float3 norm   = normalize(cross((IN.next.xyz - IN.position.xyz), binorm));\
    float3 position = IN.position.xyz;\
    position = position + binorm * IN.position.w;\
    position = position + norm * (IN.next.w * height);\
    OUT.obj_pos = position;\
    OUT.obj_norm = binorm;\
    OUT.position   = mul( MVP, float4(position,1) );\
    OUT.color = IN.color;\
    return OUT;} ";

const char * pVert3_PixelLighting = "\
struct VertexInput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float4 next      : TEXCOORD0;\
    float3 binorm    : NORMAL;\
};\
struct VertexOutput\
{   float4 position  : POSITION;\
    float4 color     : COLOR0;\
    float3 obj_norm  : TEXCOORD0;\
    float3 obj_pos   : TEXCOORD1;\
};\
VertexOutput main( VertexInput IN,\
                   uniform float3   lighting,\
                   uniform float3   specular_color,\
		   uniform float3   viewPos,\
		   uniform float4x4 MVP,\
		   uniform float    height)\
{   VertexOutput OUT;	\
    float3 binorm = normalize(-1.0f * IN.binorm);\
    float3 norm   = normalize(cross((IN.next.xyz - IN.position.xyz), binorm));\
    float3 position = IN.position.xyz;\
    position = position + binorm * IN.position.w;\
    position = position + norm * (IN.next.w * height);\
    OUT.obj_pos = position;\
    OUT.obj_norm = binorm;\
    OUT.position   = mul( MVP, float4(position,1) );\
    OUT.color = IN.color;\
    return OUT;} ";

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////// svt_container_cartoon ////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 */
svt_container_cartoon::svt_container_cartoon(svt_pdb * pPdb)
    : svt_node(),
      m_bUseMultiDraw( false ),
      m_bVBO_Supported( false ),
      m_bVA_Generated( false ),
      m_bCaps( true ),
      m_iNumVertices( 0 ),
      m_pPdb( pPdb ),
      m_pScene( svt_getScene() ),
      m_bRegenerateStartIndices( true )
{
    if (svt_checkEXT_VBO())
    {
	m_oVertexBuffer.genBuffer();
	m_oIndexBuffer.genBuffer();

	m_bVBO_Supported = true;
    }


    // the glMultiDraw calls are a very desirable function, because it can replace a huge number of
    // function calls with a single one. for now, there are some limitations to this. some company
    // does not release good drivers, another one tries to promote their own graphics APIs, so does
    // not implement OpenGL very well
    //
    // this might change some time, and then consider the next couple of lines for a revision


    // Microsoft suppresses competitors to Direct3D. So can use the efficient glMultiDraw calls
    // only if not on Windows (independent from the graphics card). glDrawRangeElements is also not
    // liked under Windows
    //
    // this can be checked at compiletime:
#ifndef WIN32
    // can we use glMultiDraw*EXT?
    if (glMultiDrawElementsEXT == NULL)	glMultiDrawElementsEXT = (SVT_PFNGLMULTIDRAWELEMENTSEXTPROC) svt_lookupFunction("glMultiDrawElementsEXT");
    if (glMultiDrawArraysEXT == NULL)	glMultiDrawArraysEXT   = (SVT_PFNGLMULTIDRAWARRAYSEXTPROC)   svt_lookupFunction("glMultiDrawArraysEXT");
    //
    // do not use multidraw if renderer is MESA or Intel.
    if ((glMultiDrawElementsEXT != NULL || glMultiDrawArraysEXT != NULL) && 
        (svt_getRenderer() != SVT_RENDERER_MESA && svt_getRenderer() != SVT_RENDERER_INTEL))
	m_bUseMultiDraw = true;

    if (glDrawRangeElementsEXT == NULL)
	glDrawRangeElementsEXT = (SVT_PFNGLDRAWRANGEELEMENTSEXTPROC) svt_lookupFunction("glDrawRangeElementsEXT");
#else
    if (glDrawElementsEXT == NULL)
	glDrawElementsEXT = (SVT_PFNGLDRAWELEMENTSEXTPROC) svt_lookupFunction("glDrawElementsEXT");
#endif

    // unfortunately, we need to distinguish between graphics cards and graphics cards from ATI. it
    // is unclear what the reason for the problems with the ati cards is.  glMultiDrawElements
    // causes severe graphical distortions, glMultiDraw* causes temporary freezing on program
    // exit. glDrawRangeElements too, but less severe than the multidraw/draw*element
    // calls. glDrawArrays isn't perfect either, but causes the least amount of freeze time
    //
    // this has to be checked at runtime:
    if (svt_getRenderer() == SVT_RENDERER_ATI)
	m_bUseMultiDraw = false;
}


/**
 * Destructor
 */
svt_container_cartoon::~svt_container_cartoon()
{
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////// svt_container_cartoon_dynamic //////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int  svt_container_cartoon_dynamic::m_iNumInstances = 0;
bool svt_container_cartoon_dynamic::m_bShaderSupported = true;
bool svt_container_cartoon_dynamic::m_bShaderInitialized = false;

svt_shader_cg_tube * svt_container_cartoon_dynamic::m_pTubeVertexShader_VertexLighting     = NULL;
svt_shader_cg_tube * svt_container_cartoon_dynamic::m_pTubeVertexShader_VertexLighting_Fog = NULL;

svt_shader_cg_tube * svt_container_cartoon_dynamic::m_pTubeVertexShader_PixelLighting = NULL;

svt_shader_cg_cartoon_fragment * svt_container_cartoon_dynamic::m_pFragmentShader_PixelLighting            = NULL;
svt_shader_cg_cartoon_fragment * svt_container_cartoon_dynamic::m_pFragmentShader_PixelLighting_Fog_Exp    = NULL;
svt_shader_cg_cartoon_fragment * svt_container_cartoon_dynamic::m_pFragmentShader_PixelLighting_Fog_Exp2   = NULL;
svt_shader_cg_cartoon_fragment * svt_container_cartoon_dynamic::m_pFragmentShader_PixelLighting_Fog_Linear = NULL;

svt_shader_cg_cartoon_fragment * svt_container_cartoon_dynamic::m_pFragmentShader_PixelLighting_FrontAndBack            = NULL;
svt_shader_cg_cartoon_fragment * svt_container_cartoon_dynamic::m_pFragmentShader_PixelLighting_Fog_Exp_FrontAndBack    = NULL;
svt_shader_cg_cartoon_fragment * svt_container_cartoon_dynamic::m_pFragmentShader_PixelLighting_Fog_Exp2_FrontAndBack   = NULL;
svt_shader_cg_cartoon_fragment * svt_container_cartoon_dynamic::m_pFragmentShader_PixelLighting_Fog_Linear_FrontAndBack = NULL;

svt_shader_cg_tube_helixsheet * svt_container_cartoon_dynamic::m_pCartoonVertexShaderRound_VertexLighting = NULL;
svt_shader_cg_tube_helixsheet * svt_container_cartoon_dynamic::m_pCartoonVertexShaderRound_VertexLighting_Fog = NULL;
svt_shader_cg_tube_helixsheet * svt_container_cartoon_dynamic::m_pCartoonVertexShaderRound_PixelLighting  = NULL;

vector< svt_shader_cg_cartoon * >   svt_container_cartoon_dynamic::m_aCartoonVertexShader_VertexLighting     = vector< svt_shader_cg_cartoon * >();
vector< svt_shader_cg_cartoon * >   svt_container_cartoon_dynamic::m_aCartoonVertexShader_VertexLighting_Fog = vector< svt_shader_cg_cartoon * >();
vector< svt_shader_cg_cartoon * >   svt_container_cartoon_dynamic::m_aCartoonVertexShader_PixelLighting      = vector< svt_shader_cg_cartoon * >();

svt_shader_cg_cartoon * svt_container_cartoon_dynamic::m_pCartoonVertexShaderFlatHelix_VertexLighting = NULL;
svt_shader_cg_cartoon * svt_container_cartoon_dynamic::m_pCartoonVertexShaderFlatHelix_VertexLighting_Fog = NULL;
svt_shader_cg_cartoon * svt_container_cartoon_dynamic::m_pCartoonVertexShaderFlatHelix_PixelLighting  = NULL;

svt_shader_cg_cartoon * svt_container_cartoon_dynamic::m_pCartoonVertexShaderFlatSheet_VertexLighting = NULL;
svt_shader_cg_cartoon * svt_container_cartoon_dynamic::m_pCartoonVertexShaderFlatSheet_VertexLighting_Fog = NULL;
svt_shader_cg_cartoon * svt_container_cartoon_dynamic::m_pCartoonVertexShaderFlatSheet_PixelLighting  = NULL;


/**
 * Constructor
 */
svt_container_cartoon_dynamic::svt_container_cartoon_dynamic(svt_pdb * pPdb)
    : svt_container_cartoon(pPdb),
      m_oV( NULL ),
      m_bPerPixelLighting( false ),
      m_pTubeVertexShader( NULL ),
      m_aCartoonVertexShader( NULL ),
      m_pCartoonVertexShaderRound( NULL ),
      m_pCartoonVertexShaderFlatHelix( NULL ),
      m_pCartoonVertexShaderFlatSheet( NULL ),
      m_pFragmentShader( NULL ),
      m_pFragmentShader_FrontAndBack( NULL )
{
    setAutoDL(false);

    m_oV = new Real32[3];

    if (m_iNumInstances == 0)
        checkShaderSupport();

    ++m_iNumInstances;
}


/**
 * Destructor
 */
svt_container_cartoon_dynamic::~svt_container_cartoon_dynamic()
{
    --m_iNumInstances;

    if (m_oV)
	delete[] m_oV;

    if (m_iNumInstances == 0)
    {
	if (m_pTubeVertexShader_VertexLighting)
	{
	    delete m_pTubeVertexShader_VertexLighting;
	    m_pTubeVertexShader_VertexLighting = NULL;
	}
	if (m_pTubeVertexShader_VertexLighting_Fog)
	{
	    delete m_pTubeVertexShader_VertexLighting_Fog;
	    m_pTubeVertexShader_VertexLighting_Fog = NULL;
	}
	if (m_pTubeVertexShader_PixelLighting)
	{
	    delete m_pTubeVertexShader_PixelLighting;
	    m_pTubeVertexShader_PixelLighting = NULL;
	}



	if (m_pFragmentShader_PixelLighting)
	{
	    delete m_pFragmentShader_PixelLighting;
	    m_pFragmentShader_PixelLighting = NULL;
	}
	if (m_pFragmentShader_PixelLighting_Fog_Exp)
	{
	    delete m_pFragmentShader_PixelLighting_Fog_Exp;
	    m_pFragmentShader_PixelLighting_Fog_Exp = NULL;
	}
	if (m_pFragmentShader_PixelLighting_Fog_Exp2)
	{
	    delete m_pFragmentShader_PixelLighting_Fog_Exp2;
	    m_pFragmentShader_PixelLighting_Fog_Exp2 = NULL;
	}
	if (m_pFragmentShader_PixelLighting_Fog_Linear)
	{
	    delete m_pFragmentShader_PixelLighting_Fog_Linear;
	    m_pFragmentShader_PixelLighting_Fog_Linear = NULL;
	}



	if (m_pFragmentShader_PixelLighting_FrontAndBack)
	{
	    delete m_pFragmentShader_PixelLighting_FrontAndBack;
	    m_pFragmentShader_PixelLighting_FrontAndBack = NULL;
	}
	if (m_pFragmentShader_PixelLighting_Fog_Exp_FrontAndBack)
	{
	    delete m_pFragmentShader_PixelLighting_Fog_Exp_FrontAndBack;
	    m_pFragmentShader_PixelLighting_Fog_Exp_FrontAndBack = NULL;
	}
	if (m_pFragmentShader_PixelLighting_Fog_Exp2_FrontAndBack)
	{
	    delete m_pFragmentShader_PixelLighting_Fog_Exp2_FrontAndBack;
	    m_pFragmentShader_PixelLighting_Fog_Exp2_FrontAndBack = NULL;
	}
	if (m_pFragmentShader_PixelLighting_Fog_Linear_FrontAndBack)
	{
	    delete m_pFragmentShader_PixelLighting_Fog_Linear_FrontAndBack;
	    m_pFragmentShader_PixelLighting_Fog_Linear_FrontAndBack = NULL;
	}



	for(unsigned int i=0;i<m_aCartoonVertexShader_VertexLighting.size();i++)
	    if (m_aCartoonVertexShader_VertexLighting[i] != NULL)
	    {
		delete m_aCartoonVertexShader_VertexLighting[i];
		m_aCartoonVertexShader_VertexLighting[i] = NULL;
	    }
	for(unsigned int i=0;i<m_aCartoonVertexShader_VertexLighting_Fog.size();i++)
	    if (m_aCartoonVertexShader_VertexLighting_Fog[i] != NULL)
	    {
		delete m_aCartoonVertexShader_VertexLighting_Fog[i];
		m_aCartoonVertexShader_VertexLighting_Fog[i] = NULL;
	    }
	for(unsigned int i=0;i<m_aCartoonVertexShader_PixelLighting.size();i++)
	    if (m_aCartoonVertexShader_PixelLighting[i] != NULL)
	    {
		delete m_aCartoonVertexShader_PixelLighting[i];
		m_aCartoonVertexShader_PixelLighting[i] = NULL;
	    }



	if (m_pCartoonVertexShaderRound_VertexLighting)
	{
	    delete m_pCartoonVertexShaderRound_VertexLighting;
	    m_pCartoonVertexShaderRound_VertexLighting = NULL;
	}
	if (m_pCartoonVertexShaderRound_VertexLighting_Fog)
	{
	    delete m_pCartoonVertexShaderRound_VertexLighting_Fog;
	    m_pCartoonVertexShaderRound_VertexLighting_Fog = NULL;
	}
	if (m_pCartoonVertexShaderRound_PixelLighting)
	{
	    delete m_pCartoonVertexShaderRound_PixelLighting;
	    m_pCartoonVertexShaderRound_PixelLighting = NULL;
	}



	if (m_pCartoonVertexShaderFlatHelix_VertexLighting)
	{
	    delete m_pCartoonVertexShaderFlatHelix_VertexLighting;
	    m_pCartoonVertexShaderFlatHelix_VertexLighting = NULL;
	}
	if (m_pCartoonVertexShaderFlatHelix_VertexLighting_Fog)
	{
	    delete m_pCartoonVertexShaderFlatHelix_VertexLighting_Fog;
	    m_pCartoonVertexShaderFlatHelix_VertexLighting_Fog = NULL;
	}
	if (m_pCartoonVertexShaderFlatHelix_PixelLighting)
	{
	    delete m_pCartoonVertexShaderFlatHelix_PixelLighting;
	    m_pCartoonVertexShaderFlatHelix_PixelLighting = NULL;
	}



	if (m_pCartoonVertexShaderFlatSheet_VertexLighting)
	{
	    delete m_pCartoonVertexShaderFlatSheet_VertexLighting;
	    m_pCartoonVertexShaderFlatSheet_VertexLighting = NULL;
	}
	if (m_pCartoonVertexShaderFlatSheet_VertexLighting_Fog)
	{
	    delete m_pCartoonVertexShaderFlatSheet_VertexLighting_Fog;
	    m_pCartoonVertexShaderFlatSheet_VertexLighting_Fog = NULL;
	}
	if (m_pCartoonVertexShaderFlatSheet_PixelLighting)
	{
	    delete m_pCartoonVertexShaderFlatSheet_PixelLighting;
	    m_pCartoonVertexShaderFlatSheet_PixelLighting = NULL;
	}
    }
}


/**
 * Start a tube
 */
void svt_container_cartoon_dynamic::beginTube( )
{
    m_iBeg = m_iNumVertices;
    m_bRegenerateStartIndices = true;
}


/**
 * End a tube. Takes the normal at the first vertex and parallel-transports it to the last vertex of
 * the current tube.
 */
void svt_container_cartoon_dynamic::endTube( )
{
    m_iEnd = m_iNumVertices-1;

    m_aTubeStripSize.push_back((GLsizei)(m_iNumVertices - m_iBeg));

    if (m_bVBO_Supported)
	m_aTubeStripStart.push_back((char*)(NULL)+(sizeof(unsigned int) * m_iBeg));
    else
	m_aTubeStripStartIndices.push_back((GLint)m_iBeg);


    // compute the local coord frames along the tube part
    // first frame is given, generate consecutive ones by parallel transport

    memcpy(oN.c_data(), m_aVertices[m_iBeg].n, 12);
    oN.normalize();

    oT.set(m_aVertices[m_iBeg+2].v[0] - m_aVertices[m_iBeg].v[0],
	   m_aVertices[m_iBeg+2].v[1] - m_aVertices[m_iBeg].v[1],
	   m_aVertices[m_iBeg+2].v[2] - m_aVertices[m_iBeg].v[2]);
    oT.normalize();

    memcpy(m_aVertices[m_iBeg  ].t, oT.c_data(), 12);
    memcpy(m_aVertices[m_iBeg+1].t, oT.c_data(), 12);

//#define DRAW_TUBE_FRAMES
#ifdef DRAW_TUBE_FRAMES
    glDisable(GL_LIGHTING); glBegin(GL_LINES);
    glColor3f(1.0f, 1.0f, 1.0f); glVertex3f(m_aVertices[m_iBeg].v[0], m_aVertices[m_iBeg].v[1], m_aVertices[m_iBeg].v[2]);
    glColor3f(1.0f, 1.0f, 1.0f); glVertex3f(m_aVertices[m_iBeg].v[0] + oT.x() * 0.01f, m_aVertices[m_iBeg].v[1] + oT.y() * 0.01f, m_aVertices[m_iBeg].v[2] + oT.z() * 0.01f);
    glColor3f(1.0f, 1.0f, 1.0f); glVertex3f(m_aVertices[m_iBeg].v[0], m_aVertices[m_iBeg].v[1], m_aVertices[m_iBeg].v[2]);
    glColor3f(1.0f, 1.0f, 1.0f); glVertex3f(m_aVertices[m_iBeg].v[0] + oN.x() * 0.01f, m_aVertices[m_iBeg].v[1] + oN.y() * 0.01f, m_aVertices[m_iBeg].v[2] + oN.z() * 0.01f);
    glEnd(); glEnable(GL_LIGHTING);
#endif
    for (j=m_iBeg; j<m_iEnd-4; j+=2)
    {
	oTT.set(m_aVertices[j+4].v[0] - m_aVertices[j+2].v[0], 
		m_aVertices[j+4].v[1] - m_aVertices[j+2].v[1], 
		m_aVertices[j+4].v[2] - m_aVertices[j+2].v[2]);
	oTT.normalize();


	// oB used as parallel transport spinning axis
	oB = crossProduct(oT, oTT);
	oB.normalize();

	fCos = oT * oTT;
	// only update oN if oT and oTT are not parallel
	if (fCos < 0.99999f)
	{
	    fSin = sin(acos(fCos));

	    fX = (fCos + (oB.x() * oB.x() * (1.0f-fCos))) * oN.x()
		+ ((oB.x() * oB.y() * (1.0f-fCos)) - fSin * oB.z()) * oN.y()
		+ ((oB.z() * oB.x() * (1.0f-fCos)) + fSin * oB.y()) * oN.z();

	    fY = ( oB.x() * oB.y() * (1.0f-fCos)  + fSin * oB.z()) * oN.x()
		+ (fCos + oB.y() * oB.y() * (1.0f-fCos)) * oN.y()
		+ (oB.z() * oB.y() * (1.0f-fCos)  - fSin * oB.x()) * oN.z();

	    fZ = ( oB.x() * oB.z() * (1.0f-fCos) - fSin * oB.y()) * oN.x()
		+ (oB.y() * oB.z() * (1.0f-fCos) + fSin * oB.x()) * oN.y()
		+ (fCos + oB.z() * oB.z() * (1.0f-fCos)) * oN.z();

	    oN.set(fX, fY, fZ);
	    //oN.normalize(); -- not necessary
	}

	memcpy(m_aVertices[j+2].n, oN.c_data(), 12);
	memcpy(m_aVertices[j+3].n, oN.c_data(), 12);

	memcpy(oT.c_data(), oTT.c_data(), 12);

	memcpy(m_aVertices[j+2].t, oTT.c_data(), 12);
	memcpy(m_aVertices[j+3].t, oTT.c_data(), 12);

#ifdef DRAW_TUBE_FRAMES
	glDisable(GL_LIGHTING); glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(m_aVertices[j+2].v[0], m_aVertices[j+2].v[1], m_aVertices[j+2].v[2]);
	glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(m_aVertices[j+2].v[0] + oT.x() * 0.01f, m_aVertices[j+2].v[1] + oT.y() * 0.01f, m_aVertices[j+2].v[2] + oT.z() * 0.01f);
	glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(m_aVertices[j+2].v[0], m_aVertices[j+2].v[1], m_aVertices[j+2].v[2]);
	glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(m_aVertices[j+2].v[0] + oN.x() * 0.01f, m_aVertices[j+2].v[1] + oN.y() * 0.01f, m_aVertices[j+2].v[2] + oN.z() * 0.01f);
	glEnd(); glEnable(GL_LIGHTING);
#endif
    }
    memcpy(m_aVertices[m_iEnd-1].n, oN.c_data(), 12);
    memcpy(m_aVertices[m_iEnd  ].n, oN.c_data(), 12);

    memcpy(m_aVertices[m_iEnd-1].t, oTT.c_data(), 12);
    memcpy(m_aVertices[m_iEnd  ].t, oTT.c_data(), 12);
}


/**
 * Start a helix
 */
void svt_container_cartoon_dynamic::beginHelix( )
{
    m_iBeg = m_iNumVertices;
    m_bRegenerateStartIndices = true;
}


/**
 * End a helix. Created the tangent information at each vertex
 */
void svt_container_cartoon_dynamic::endHelix( )
{
    m_iEnd = m_iNumVertices-1;

    m_aHelixStripSize.push_back((GLsizei)(m_iNumVertices - m_iBeg));

    if (m_bVBO_Supported)
	m_aHelixStripStart.push_back((char*)(NULL)+(sizeof(unsigned int) * m_iBeg));
    else
	m_aHelixStripStartIndices.push_back(m_iBeg);


    for (j=m_iBeg; j<m_iEnd; j+=2)
    {
	memcpy(m_aVertices[ j ].t, m_aVertices[j+2].v, 12);
	memcpy(m_aVertices[j+1].t, m_aVertices[j+2].v, 12);
    }

    m_oV[0] = 2.0f * m_aVertices[m_iEnd].v[0] - m_aVertices[m_iEnd-2].v[0];
    m_oV[1] = 2.0f * m_aVertices[m_iEnd].v[1] - m_aVertices[m_iEnd-2].v[1];
    m_oV[2] = 2.0f * m_aVertices[m_iEnd].v[2] - m_aVertices[m_iEnd-2].v[2];

    memcpy(m_aVertices[m_iEnd-1].t, m_oV, 12);
    memcpy(m_aVertices[m_iEnd  ].t, m_oV, 12);
}


/**
 * Start a sheet
 */
void svt_container_cartoon_dynamic::beginSheet( )
{
    m_iBeg = m_iNumVertices;
    m_bRegenerateStartIndices = true;
}


/**
 * End a sheet. Created the tangent information at each vertex
 */
void svt_container_cartoon_dynamic::endSheet( )
{
    m_iEnd = m_iNumVertices-1;

    m_aSheetStripSize.push_back((GLsizei)(m_iNumVertices - m_iBeg));

    if (m_bVBO_Supported)
	m_aSheetStripStart.push_back((char*)(NULL)+(sizeof(unsigned int) * m_iBeg));
    else
	m_aSheetStripStartIndices.push_back(m_iBeg);

    
    for (j=m_iBeg; j<m_iEnd; j+=2)
    {
	memcpy(m_aVertices[ j ].t, m_aVertices[j+2].v, 12);
	memcpy(m_aVertices[j+1].t, m_aVertices[j+2].v, 12);
    }

    m_oV[0] = 2.0f * m_aVertices[m_iEnd].v[0] - m_aVertices[m_iEnd-2].v[0];
    m_oV[1] = 2.0f * m_aVertices[m_iEnd].v[1] - m_aVertices[m_iEnd-2].v[1];
    m_oV[2] = 2.0f * m_aVertices[m_iEnd].v[2] - m_aVertices[m_iEnd-2].v[2];

    memcpy(m_aVertices[m_iEnd-1].t, m_oV, 12);
    memcpy(m_aVertices[m_iEnd  ].t, m_oV, 12);
}


/**
 * draw the content of the container (OpenGL)
 */
void svt_container_cartoon_dynamic::drawGL()
{
    if (m_iNumVertices == 0) return;

    if (m_pScene == NULL)
	m_pScene = svt_getScene();

    //unsigned long iTime = svt_getToD();
    unsigned int iSize;
    GLfloat f[4];

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glLineWidth(1.0f);

    if (m_bVBO_Supported)
    {
	if (m_bVA_Generated)
	{
	    m_oVertexBuffer.bindBuffer( GL_ARRAY_BUFFER_ARB );
	    m_oIndexBuffer.bindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB );
	}
	else
	{
	    m_oVertexBuffer.bindBuffer( GL_ARRAY_BUFFER_ARB );
	    m_oVertexBuffer.bufferData( GL_ARRAY_BUFFER_ARB, m_aVertices.size() * 64, &m_aVertices[0], GL_DYNAMIC_DRAW_ARB);

	    m_oIndexBuffer.bindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB );
	    m_oIndexBuffer.bufferData( GL_ELEMENT_ARRAY_BUFFER_ARB, m_aIndices.size() * sizeof( unsigned int ), &m_aIndices[0], GL_DYNAMIC_DRAW_ARB);

	    m_bVA_Generated = true;
	}

	glVertexPointer  ( 4, GL_FLOAT, 64, (char*)(NULL) );
	glColorPointer   ( 4, GL_FLOAT, 64, (char*)(NULL) + 16 );
	glTexCoordPointer( 4, GL_FLOAT, 64, (char*)(NULL) + 32 );
	glNormalPointer  (    GL_FLOAT, 64, (char*)(NULL) + 48 );
    }
    else
    {
	glVertexPointer  ( 4, GL_FLOAT, 64, (void*)(&m_aVertices[0]) );
	glColorPointer   ( 4, GL_FLOAT, 64, (char*)(&m_aVertices[0]) + 16 );
	glTexCoordPointer( 4, GL_FLOAT, 64, (char*)(&m_aVertices[0]) + 32 );
	glNormalPointer  (    GL_FLOAT, 64, (char*)(&m_aVertices[0]) + 48 );
    }


    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY  );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );


//     // read out of the VBO (for debugging purposes)
//     //
//     printf("sizeof(v4f_c4f_t4f_n3f): %i\n", sizeof(v4f_c4f_t4f_n3f));
//     unsigned char * pVertVBO = (unsigned char *) m_oVertexBuffer.mapBuffer(GL_ARRAY_BUFFER_ARB, GL_READ_ONLY);
//     if (pVertVBO == NULL) { svtout << "no pointer to vertex mem to read it\n"; return; }
//     //
//     unsigned int *pIndxVBO = (unsigned int *) m_oIndexBuffer.mapBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_READ_ONLY);
//     if (pIndxVBO == NULL) { svtout << "no pionter to index mem to read it\n"; return; }
//     //
//     v4f_c4f_t4f_n3f oTmp;
//     //
//     for (i=0; i<m_aVertices.size(); i++)
//     {
// 	memcpy(&oTmp, pVertVBO, sizeof(v4f_c4f_t4f_n3f));
// 	pVertVBO += sizeof(v4f_c4f_t4f_n3f);
// 	printf("%5i v: ", *pIndxVBO++);
// 	printf("% 2.3f % 2.3f % 2.3f % 2.3f   c: %2.3f %2.3f %2.3f %2.3f   t: % 2.3f % 2.3f % 2.3f % 2.3f   n: % 2.3f % 2.3f % 2.3f\n",
// 	       oTmp.v[0], oTmp.v[1], oTmp.v[2], oTmp.v[3],
// 	       oTmp.c[0], oTmp.c[1], oTmp.c[2], oTmp.c[3],
// 	       oTmp.t[0], oTmp.t[1], oTmp.t[2], oTmp.t[3],
// 	       oTmp.n[0], oTmp.n[1], oTmp.n[2]);
//     }
//     m_oVertexBuffer.unmapBuffer(GL_ARRAY_BUFFER_ARB);
//     m_oIndexBuffer.unmapBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB);



    m_oCam.update();
    m_oViewPos = svt_vector3<Real32>(m_oCam.viewPos());

    // the diffuse color that is stored with the vertices is used as color for ambient, diffuse, and
    // specular
    //
    // the weight for ambient and diffuse is passed to the shader in the first two components of
    // m_oLighting. its third component is the shininess factor. the weight for the specular color
    // is implicitely given when in m_oSpecularColor
    //
    m_oLighting.set(0.3f, 0.9f, 128);
    m_oSpecularColor.set(0.5f, 0.5f, 0.5f);


    /////////////////////////////////////////////////// determine shader programs ///////////////////////////////////////////////////////
    //
    // according to the different drawing mode (per vertex/per pixel, linear/exp/exp2 fog), the
    // following lines set pointers to the appropriate shader
    //
    // for example, if the shading is per pixel with exponential fog, then
    //
    // * m_pCartoonVertexShader is set to m_pCartoonVertexShader_PixelLighting, and
    // * m_pFragmentShader is set to m_pFragmentShader_Fog_Exp.
    //
    if (m_pScene != NULL && m_pPdb->getCartoonParam(CARTOON_PER_PIXEL_LIGHTING) == 1.0f)
    {
	// choose fragment shader program for fog and set appropriate parameters. The FrontAndBack
	// fragment shader is only used for the sheets, as we want them to be shaded on both sides
	if (m_pScene->getFogEnabled())
	{
	    m_pScene->getFogParam(GL_FOG_MODE, f);

	    if (f[0] == GL_LINEAR)
	    {
		m_pFragmentShader              = m_pFragmentShader_PixelLighting_Fog_Linear;
		m_pFragmentShader_FrontAndBack = m_pFragmentShader_PixelLighting_Fog_Linear_FrontAndBack;


		m_pScene->getFogParam(GL_FOG_START, &f[0]);
		m_pScene->getFogParam(GL_FOG_END,   &f[1]);
		m_pScene->getFogParam(GL_LINEAR,    &f[2]);

		m_pFragmentShader->setFogStart(f[0]);
		m_pFragmentShader->setFogEnd(f[1]);
		m_pFragmentShader->setFogZoomOffset(f[2]);

		m_pFragmentShader_FrontAndBack->setFogStart(f[0]);
		m_pFragmentShader_FrontAndBack->setFogEnd(f[1]);
		m_pFragmentShader_FrontAndBack->setFogZoomOffset(f[2]);
	    }
	    else if (f[0] == GL_EXP)
	    {
		m_pFragmentShader              = m_pFragmentShader_PixelLighting_Fog_Exp;
		m_pFragmentShader_FrontAndBack = m_pFragmentShader_PixelLighting_Fog_Exp_FrontAndBack;


		m_pScene->getFogParam(GL_FOG_DENSITY, f);
		m_pFragmentShader->setFogDensity(f[0]);
		m_pFragmentShader_FrontAndBack->setFogDensity(f[0]);
	    }
	    else //if (f[0] == GL_EXP2)
	    {
		m_pFragmentShader              = m_pFragmentShader_PixelLighting_Fog_Exp2;
		m_pFragmentShader_FrontAndBack = m_pFragmentShader_PixelLighting_Fog_Exp2_FrontAndBack;


		m_pScene->getFogParam(GL_FOG_DENSITY, f);
		m_pFragmentShader->setFogDensity(f[0]);
		m_pFragmentShader_FrontAndBack->setFogDensity(f[0]);
	    }

	    // fog color needs to be set in any fog mode
	    m_pScene->getFogParam(GL_FOG_COLOR, f);
	    m_pFragmentShader->setFogColor(f);
	    m_pFragmentShader_FrontAndBack->setFogColor(f);
	}
	else // no fog
	{
	    m_pFragmentShader              = m_pFragmentShader_PixelLighting;
	    m_pFragmentShader_FrontAndBack = m_pFragmentShader_PixelLighting_FrontAndBack;
	}

	m_pTubeVertexShader = m_pTubeVertexShader_PixelLighting;

	m_pCartoonVertexShaderFlatHelix  =  m_pCartoonVertexShaderFlatHelix_PixelLighting;
	m_pCartoonVertexShaderFlatSheet  =  m_pCartoonVertexShaderFlatSheet_PixelLighting;
	m_pCartoonVertexShaderRound      =  m_pCartoonVertexShaderRound_PixelLighting;
	m_aCartoonVertexShader           = &m_aCartoonVertexShader_PixelLighting;

	m_pFragmentShader->setViewPos(m_oViewPos);
	m_pFragmentShader->setLighting(m_oLighting, m_oSpecularColor);
	m_pFragmentShader->bind();

	// don't bind the FrontAndBack shader now, that is done when the flat sheets are drawn
	m_pFragmentShader_FrontAndBack->setViewPos(m_oViewPos);
	m_pFragmentShader_FrontAndBack->setLighting(m_oLighting, m_oSpecularColor);
    }
    //
    // with no per pixel lighting, we just need the vertex shaders:
    //
    else if (m_pPdb->getCartoonParam(CARTOON_PER_PIXEL_LIGHTING) == 0.0f)
    {
	if (m_pScene != NULL && m_pScene->getFogEnabled())
	{
	    m_pTubeVertexShader              =  m_pTubeVertexShader_VertexLighting_Fog;
	    m_pCartoonVertexShaderFlatHelix  =  m_pCartoonVertexShaderFlatHelix_VertexLighting_Fog;
	    m_pCartoonVertexShaderFlatSheet  =  m_pCartoonVertexShaderFlatSheet_VertexLighting_Fog;
	    m_aCartoonVertexShader           = &m_aCartoonVertexShader_VertexLighting_Fog;
	    m_pCartoonVertexShaderRound      =  m_pCartoonVertexShaderRound_VertexLighting_Fog;
	}
	else // if (m_pScene == NULL || m_pScene->getFogEnabled() == false)
	{
	    m_pTubeVertexShader              =  m_pTubeVertexShader_VertexLighting;
	    m_pCartoonVertexShaderFlatHelix  =  m_pCartoonVertexShaderFlatHelix_VertexLighting;
	    m_pCartoonVertexShaderFlatSheet  =  m_pCartoonVertexShaderFlatSheet_VertexLighting;
	    m_aCartoonVertexShader           = &m_aCartoonVertexShader_VertexLighting;
	    m_pCartoonVertexShaderRound      =  m_pCartoonVertexShaderRound_VertexLighting;
	}
    }

    /////////////////////////////////////////////////////////// tubes ///////////////////////////////////////////////////////////////////
    //
    iSize = m_aTubeStripSize.size();
    if (iSize > 0)
    {
	k = m_pPdb->getNewTubeProfileCorners();
	fY = 2.0f*PI / (Real32) k;
	
	m_pTubeVertexShader->setViewPos(m_oViewPos);
	m_pTubeVertexShader->setLighting(m_oLighting, m_oSpecularColor);
	m_pTubeVertexShader->setSpan(fY/2.0f);
	m_pTubeVertexShader->bind();


	// m_aTubeStripStart needs to be filled with pointers into the index array m_aIndices.
	// 
	// if VBOs are supported, this is done in the endTube() and similar methods, because these
	// pointers are only relative and not absolute.
	//
	// if VBOs are not supported, the pointers are absolute pointers into client memory, and
	// since we use vectors for the vertex and index data, their address can change. so we can
	// generate the array m_aTubeStripStart only after the vectors won't get anymore elements
	// added. this is the case here ( in drawGL() ). m_aTubeStripStartIndices contains the
	// indices for the start of the triangle strips, which are used to generate the pointers.
	//
	if (!m_bVBO_Supported && m_bRegenerateStartIndices)
	{
	    m_aTubeStripStart.clear();

	    for (i=0; i<iSize; ++i)
	    {
		m_aTubeStripStart.push_back((const GLvoid*)(&(m_aIndices[m_aTubeStripStartIndices[i]])));
	    }
	}
	
	for (j=0, fX=0; j<k; ++j, fX += fY)
	{
	    m_pTubeVertexShader->setAngle(fX);

#ifndef WIN32
	    if (m_bUseMultiDraw)
	    {
		if (m_bVBO_Supported)
		{
		    glMultiDrawElementsEXT(GL_TRIANGLE_STRIP,
					   (const GLsizei*)&m_aTubeStripSize[0],
					   GL_UNSIGNED_INT,
					   &m_aTubeStripStart[0],
					   (GLsizei)iSize);
		}
		else
		{
		    glMultiDrawArraysEXT(GL_TRIANGLE_STRIP,
					 &m_aTubeStripStartIndices[0],
					 &m_aTubeStripSize[0],
					 (GLsizei)iSize);
		}		    
	    }
	    else
#endif
	    {
		if (m_bVBO_Supported)
		{
		    for (i=0; i<iSize; ++i)
#ifndef WIN32
			glDrawRangeElementsEXT(GL_TRIANGLE_STRIP,
					       m_aTubeStripStartIndices[i],
					       m_aTubeStripStartIndices[i] + m_aTubeStripSize[i] - 1,
					       m_aTubeStripSize[i],
					       GL_UNSIGNED_INT,
					       &m_aIndices[0] + m_aTubeStripStartIndices[i]);
#else
		        glDrawElementsEXT(GL_TRIANGLE_STRIP,
					  m_aTubeStripSize[i],
					  GL_UNSIGNED_INT,
					  &m_aTubeStripStartIndices[i]);
#endif
		}
		else
		{
		    for (i=0; i<iSize; ++i)
			glDrawArrays(GL_TRIANGLE_STRIP,
				     m_aTubeStripStartIndices[i],
				     m_aTubeStripSize[i]);
		}
	    }
	}
	m_pTubeVertexShader->unbind();
    }


// // for performance measurements: triangle strips from the beginning to the end
//     iSize = m_aIndices.size();
//     for (j=0; j<4; ++j)
//     {
// 	(*m_aCartoonVertexShader)[j]->setHeight(m_pPdb->getCartoonParam(CARTOON_HELIX_HEIGHT) * CARTOON_SCALING_FACTOR);
// 	(*m_aCartoonVertexShader)[j]->setLighting(m_oLighting, m_oSpecularColor);
// 	(*m_aCartoonVertexShader)[j]->setViewPos(m_oViewPos);
// 	(*m_aCartoonVertexShader)[j]->bind();
// 	glDrawRangeElementsEXT(GL_TRIANGLE_STRIP,
// 			    0,
// 			    iSize-1,
// 			    iSize,
// 			    GL_UNSIGNED_INT,
// 			    (char*)(NULL) );
// 	(*m_aCartoonVertexShader)[j]->unbind();
//     }



    ////////////////////////////////////////////////////// flat sheets/helices //////////////////////////////////////////////////////////
    //
    if (m_pPdb->getCartoonParam(CARTOON_SHEET_HELIX_PROFILE_CORNERS) == 2.0f)
    {
	iSize = m_aHelixStripSize.size();
	if (iSize > 0)
	{

	    if (!m_bVBO_Supported && m_bRegenerateStartIndices)
	    {
		m_aHelixStripStart.clear();

		for (i=0; i<iSize; ++i)
		{
		    m_aHelixStripStart.push_back(&(m_aIndices[m_aHelixStripStartIndices[i]]));
		}
	    }

	    m_pCartoonVertexShaderFlatHelix->setViewPos(m_oViewPos);
	    m_pCartoonVertexShaderFlatHelix->setLighting(m_oLighting, m_oSpecularColor);
	    m_pCartoonVertexShaderFlatHelix->setHeight(0.0f);
	    m_pCartoonVertexShaderFlatHelix->bind();

	    // now draw the strips...
#ifndef WIN32
	    if (m_bUseMultiDraw)
	    {
		if (m_bVBO_Supported)
		{
		    glMultiDrawElementsEXT(GL_TRIANGLE_STRIP,
					   (const GLsizei*)&m_aHelixStripSize[0],
					   GL_UNSIGNED_INT,
					   (const GLvoid* *)(&m_aHelixStripStart[0]),
					   (GLsizei)iSize);
		}
		else
		{
		    glMultiDrawArraysEXT(GL_TRIANGLE_STRIP,
					 &m_aHelixStripStartIndices[0],
					 &m_aHelixStripSize[0],
					 (GLsizei)iSize);
		}		    
	    }
	    else
#endif
	    {
		if (m_bVBO_Supported)
		{
		    for (i=0; i<iSize; ++i)
#ifndef WIN32
			glDrawRangeElementsEXT(GL_TRIANGLE_STRIP,
					       m_aHelixStripStartIndices[i],
					       m_aHelixStripStartIndices[i] + m_aHelixStripSize[i] - 1,
					       m_aHelixStripSize[i],
					       GL_UNSIGNED_INT,
					       &m_aIndices[0] + m_aHelixStripStartIndices[i]);
#else
		        glDrawElementsEXT(GL_TRIANGLE_STRIP,
					  m_aHelixStripSize[i],
					  GL_UNSIGNED_INT,
					  &m_aHelixStripStartIndices[i]);
#endif
		}
		else
		{
		    for (i=0; i<iSize; ++i)
			glDrawArrays(GL_TRIANGLE_STRIP,
				     m_aHelixStripStartIndices[i],
				     m_aHelixStripSize[i]);
		}
	    }

	    m_pCartoonVertexShaderFlatHelix->unbind();
	}
	//
	iSize = m_aSheetStripSize.size();
	if (iSize > 0)
	{

	    if (m_pPdb->getCartoonParam(CARTOON_PER_PIXEL_LIGHTING) == 1.0f && m_pScene != NULL)
	    {
		m_pFragmentShader->unbind();
		m_pFragmentShader_FrontAndBack->bind();
	    }


	    if (!m_bVBO_Supported && m_bRegenerateStartIndices)
	    {
		m_aSheetStripStart.clear();

		for (i=0; i<iSize; ++i)
		{
		    m_aSheetStripStart.push_back(&(m_aIndices[m_aSheetStripStartIndices[i]]));
		}
	    }

	    m_pCartoonVertexShaderFlatSheet->setViewPos(m_oViewPos);
	    m_pCartoonVertexShaderFlatSheet->setLighting(m_oLighting, m_oSpecularColor);
	    m_pCartoonVertexShaderFlatSheet->setHeight(0.0f);
	    m_pCartoonVertexShaderFlatSheet->bind();

	    // now draw the strips...
#ifndef WIN32
	    if (m_bUseMultiDraw)
	    {
		if (m_bVBO_Supported)
		{
		    glMultiDrawElementsEXT(GL_TRIANGLE_STRIP,
					   (const GLsizei*)&m_aSheetStripSize[0],
					   GL_UNSIGNED_INT,
					   (const GLvoid* *)(&m_aSheetStripStart[0]),
					   (GLsizei)iSize);
		}
		else
		{
		    glMultiDrawArraysEXT(GL_TRIANGLE_STRIP,
					 &m_aSheetStripStartIndices[0],
					 &m_aSheetStripSize[0],
					 (GLsizei)iSize);
		}		    
	    }
	    else
#endif
	    {
		if (m_bVBO_Supported)
		{
		    for (i=0; i<iSize; ++i)
#ifndef WIN32
			glDrawRangeElementsEXT(GL_TRIANGLE_STRIP,
					       m_aSheetStripStartIndices[i],
					       m_aSheetStripStartIndices[i] + m_aSheetStripSize[i] - 1,
					       m_aSheetStripSize[i],
					       GL_UNSIGNED_INT,
					       &m_aIndices[0] + m_aSheetStripStartIndices[i]);
#else
		        glDrawElementsEXT(GL_TRIANGLE_STRIP,
					  m_aSheetStripSize[i],
					  GL_UNSIGNED_INT,
					  &m_aSheetStripStartIndices[i]);
#endif
		}
		else
		{
		    for (i=0; i<iSize; ++i)
			glDrawArrays(GL_TRIANGLE_STRIP,
				     m_aSheetStripStartIndices[i],
				     m_aSheetStripSize[i]);
		}
	    }

	    m_pCartoonVertexShaderFlatSheet->unbind();

	    if (m_pPdb->getCartoonParam(CARTOON_PER_PIXEL_LIGHTING) == 1.0f && m_pScene != NULL)
	    {
		m_pFragmentShader_FrontAndBack->unbind();
		m_pFragmentShader->bind();
	    }
	}
    }
    ////////////////////////////////////////////////////// rectangular sheets/helices //////////////////////////////////////////////////////////
    //
    else if (m_pPdb->getCartoonParam(CARTOON_SHEET_HELIX_PROFILE_CORNERS) == 4.0f)
    {
	iSize = m_aHelixStripSize.size();
	if (iSize > 0)
	{

	    if (!m_bVBO_Supported && m_bRegenerateStartIndices)
	    {
		m_aHelixStripStart.clear();

		for (i=0; i<iSize; ++i)
		{
		    m_aHelixStripStart.push_back(&(m_aIndices[m_aHelixStripStartIndices[i]]));
		}
	    }

	    for (j=0; j<4; ++j)
	    {
		(*m_aCartoonVertexShader)[j]->setHeight(m_pPdb->getCartoonParam(CARTOON_HELIX_HEIGHT) * CARTOON_SCALING_FACTOR);
		(*m_aCartoonVertexShader)[j]->setLighting(m_oLighting, m_oSpecularColor);
		(*m_aCartoonVertexShader)[j]->setViewPos(m_oViewPos);
		(*m_aCartoonVertexShader)[j]->bind();

		// now draw the strips...
#ifndef WIN32
		if (m_bUseMultiDraw)
		{
		    if (m_bVBO_Supported)
		    {
			glMultiDrawElementsEXT(GL_TRIANGLE_STRIP,
					       (const GLsizei*)&m_aHelixStripSize[0],
					       GL_UNSIGNED_INT,
					       (const GLvoid* *)(&m_aHelixStripStart[0]),
					       (GLsizei)iSize);
		    }
		    else
		    {
			glMultiDrawArraysEXT(GL_TRIANGLE_STRIP,
					     &m_aHelixStripStartIndices[0],
					     &m_aHelixStripSize[0],
					     (GLsizei)iSize);
		    }		    
		}
		else
#endif
		{
		    if (m_bVBO_Supported)
		    {
			for (i=0; i<iSize; ++i)
#ifndef WIN32
			    glDrawRangeElementsEXT(GL_TRIANGLE_STRIP,
						   m_aHelixStripStartIndices[i],
						   m_aHelixStripStartIndices[i] + m_aHelixStripSize[i] - 1,
						   m_aHelixStripSize[i],
						   GL_UNSIGNED_INT,
						   &m_aIndices[0] + m_aHelixStripStartIndices[i]);
#else
			    glDrawElementsEXT(GL_TRIANGLE_STRIP,
					      m_aHelixStripSize[i],
					      GL_UNSIGNED_INT,
					      &m_aHelixStripStartIndices[i]);
#endif
		    }
		    else
		    {
			for (i=0; i<iSize; ++i)
			    glDrawArrays(GL_TRIANGLE_STRIP,
					 m_aHelixStripStartIndices[i],
					 m_aHelixStripSize[i]);
		    }
		}

		(*m_aCartoonVertexShader)[j]->unbind();
	    }
	}

	iSize = m_aSheetStripSize.size();
	if (iSize > 0)
	{

	    if (!m_bVBO_Supported && m_bRegenerateStartIndices)
	    {
		m_aSheetStripStart.clear();

		for (i=0; i<iSize; ++i)
		{
		    m_aSheetStripStart.push_back(&(m_aIndices[m_aSheetStripStartIndices[i]]));
		}
	    }

	    for (j=0; j<4; ++j)
	    {
		(*m_aCartoonVertexShader)[j]->setHeight(m_pPdb->getCartoonParam(CARTOON_SHEET_HEIGHT) * CARTOON_SCALING_FACTOR);
		(*m_aCartoonVertexShader)[j]->setLighting(m_oLighting, m_oSpecularColor);
		(*m_aCartoonVertexShader)[j]->setViewPos(m_oViewPos);
		(*m_aCartoonVertexShader)[j]->bind();

		// now draw the strips...
#ifndef WIN32
		if (m_bUseMultiDraw)
		{
		    if (m_bVBO_Supported)
		    {
			glMultiDrawElementsEXT(GL_TRIANGLE_STRIP,
					       (const GLsizei*)&m_aSheetStripSize[0],
					       GL_UNSIGNED_INT,
					       (const GLvoid* *)(&m_aSheetStripStart[0]),
					       (GLsizei)iSize);
		    }
		    else
		    {
			glMultiDrawArraysEXT(GL_TRIANGLE_STRIP,
					     &m_aSheetStripStartIndices[0],
					     &m_aSheetStripSize[0],
					     (GLsizei)iSize);
		    }		    
		}
		else
#endif
		{
		    if (m_bVBO_Supported)
		    {
			for (i=0; i<iSize; ++i)
#ifndef WIN32
			    glDrawRangeElementsEXT(GL_TRIANGLE_STRIP,
						   m_aSheetStripStartIndices[i],
						   m_aSheetStripStartIndices[i] + m_aSheetStripSize[i] - 1,
						   m_aSheetStripSize[i],
						   GL_UNSIGNED_INT,
						   &m_aIndices[0] + m_aSheetStripStartIndices[i]);
#else
			    glDrawElementsEXT(GL_TRIANGLE_STRIP,
					      m_aSheetStripSize[i],
					      GL_UNSIGNED_INT,
					      &m_aSheetStripStartIndices[i]);
#endif
		    }
		    else
		    {
			for (i=0; i<iSize; ++i)
			    glDrawArrays(GL_TRIANGLE_STRIP,
					 m_aSheetStripStartIndices[i],
					 m_aSheetStripSize[i]);
		    }
		}

		(*m_aCartoonVertexShader)[j]->unbind();
	    }
	}
    }

    ////////////////////////////////////////////////////// round sheets/helices //////////////////////////////////////////////////////////
    //
    else if (m_pPdb->getCartoonParam(CARTOON_SHEET_HELIX_PROFILE_CORNERS) > 5.0f)
    {
	iSize = m_aHelixStripSize.size();
	if (iSize > 0)
	{

	    if (!m_bVBO_Supported && m_bRegenerateStartIndices)
	    {
		m_aHelixStripStart.clear();

		for (i=0; i<iSize; ++i)
		{
		    m_aHelixStripStart.push_back(&(m_aIndices[m_aHelixStripStartIndices[i]]));
		}
	    }

	    k = (unsigned int) m_pPdb->getCartoonParam(CARTOON_SHEET_HELIX_PROFILE_CORNERS);
	    fY = 2.0f*PI / (Real32) k;
	
	    m_pCartoonVertexShaderRound->setViewPos(m_oViewPos);
	    m_pCartoonVertexShaderRound->setSpan(fY/2.0f);
	    m_pCartoonVertexShaderRound->setR1(m_pPdb->getCartoonParam(CARTOON_HELIX_WIDTH)*fZ);
	    m_pCartoonVertexShaderRound->setR2(m_pPdb->getCartoonParam(CARTOON_HELIX_HEIGHT)*fZ);
	    m_pCartoonVertexShaderRound->setLighting(m_oLighting, m_oSpecularColor);
	    m_pCartoonVertexShaderRound->bind();

	    // now draw the strips...
	    for (j=0, fX=0; j<k; ++j, fX += fY)
	    {
		m_pCartoonVertexShaderRound->setAngle(fX);

#ifndef WIN32
		if (m_bUseMultiDraw)
		{
		    if (m_bVBO_Supported)
		    {
			glMultiDrawElementsEXT(GL_TRIANGLE_STRIP,
					       (const GLsizei*)&m_aHelixStripSize[0],
					       GL_UNSIGNED_INT,
					       (const GLvoid* *)(&m_aHelixStripStart[0]),
					       (GLsizei)iSize);
		    }
		    else
		    {
			glMultiDrawArraysEXT(GL_TRIANGLE_STRIP,
					     &m_aHelixStripStartIndices[0],
					     &m_aHelixStripSize[0],
					     (GLsizei)iSize);
		    }		    
		}
		else
#endif
		{
		    if (m_bVBO_Supported)
		    {
			for (i=0; i<iSize; ++i)
#ifndef WIN32
			    glDrawRangeElementsEXT(GL_TRIANGLE_STRIP,
						   m_aHelixStripStartIndices[i],
						   m_aHelixStripStartIndices[i] + m_aHelixStripSize[i] - 1,
						   m_aHelixStripSize[i],
						   GL_UNSIGNED_INT,
						   &m_aIndices[0] + m_aHelixStripStartIndices[i]);
#else
			    glDrawElementsEXT(GL_TRIANGLE_STRIP,
					      m_aHelixStripSize[i],
					      GL_UNSIGNED_INT,
					      &m_aHelixStripStartIndices[i]);
#endif
		    }
		    else
		    {
			for (i=0; i<iSize; ++i)
			    glDrawArrays(GL_TRIANGLE_STRIP,
					 m_aHelixStripStartIndices[i],
					 m_aHelixStripSize[i]);
		    }
		}
	    }
	    m_pCartoonVertexShaderRound->unbind();
	}
	//
	iSize = m_aSheetStripSize.size();
	if (iSize > 0)
	{

	    if (!m_bVBO_Supported && m_bRegenerateStartIndices)
	    {
		m_aSheetStripStart.clear();

		for (i=0; i<iSize; ++i)
		{
		    m_aSheetStripStart.push_back(&(m_aIndices[m_aSheetStripStartIndices[i]]));
		}
	    }

	    k = (unsigned int) m_pPdb->getCartoonParam(CARTOON_SHEET_HELIX_PROFILE_CORNERS);
	    fY = 2.0f*PI / (Real32) k;

	    fZ = 1.0f / m_pPdb->getCartoonParam(CARTOON_SHEET_WIDTH);


	    m_pCartoonVertexShaderRound->setViewPos(m_oViewPos);
	    m_pCartoonVertexShaderRound->setSpan(fY/2.0f);
	    m_pCartoonVertexShaderRound->setR1(m_pPdb->getCartoonParam(CARTOON_SHEET_WIDTH)*fZ);
	    m_pCartoonVertexShaderRound->setR2(m_pPdb->getCartoonParam(CARTOON_SHEET_HEIGHT)*fZ);
	    m_pCartoonVertexShaderRound->setLighting(m_oLighting, m_oSpecularColor);
	    m_pCartoonVertexShaderRound->bind();

	    // now draw the strips...
	    for (j=0, fX=0; j<k; ++j, fX += fY)
	    {
		m_pCartoonVertexShaderRound->setAngle(fX);

#ifndef WIN32
		if (m_bUseMultiDraw)
		{
		    if (m_bVBO_Supported)
		    {
			glMultiDrawElementsEXT(GL_TRIANGLE_STRIP,
					       (const GLsizei*)&m_aSheetStripSize[0],
					       GL_UNSIGNED_INT,
					       (const GLvoid* *)(&m_aSheetStripStart[0]),
					       (GLsizei)iSize);
		    }
		    else
		    {
			glMultiDrawArraysEXT(GL_TRIANGLE_STRIP,
					     &m_aSheetStripStartIndices[0],
					     &m_aSheetStripSize[0],
					     (GLsizei)iSize);
		    }		    
		}
		else
#endif
		{
		    if (m_bVBO_Supported)
		    {
			for (i=0; i<iSize; ++i)
#ifndef WIN32
			    glDrawRangeElementsEXT(GL_TRIANGLE_STRIP,
						   m_aSheetStripStartIndices[i],
						   m_aSheetStripStartIndices[i] + m_aSheetStripSize[i] - 1,
						   m_aSheetStripSize[i],
						   GL_UNSIGNED_INT,
						   &m_aIndices[0] + m_aSheetStripStartIndices[i]);
#else
			    glDrawElementsEXT(GL_TRIANGLE_STRIP,
					      m_aSheetStripSize[i],
					      GL_UNSIGNED_INT,
					      &m_aSheetStripStartIndices[i]);
#endif
		    }
		    else
		    {
			for (i=0; i<iSize; ++i)
			    glDrawArrays(GL_TRIANGLE_STRIP,
					 m_aSheetStripStartIndices[i],
					 m_aSheetStripSize[i]);
		    }
		}
	    }
	    m_pCartoonVertexShaderRound->unbind();
	}
    }


    // for per pixel lighting, the same fragment shader for all geometry is used.
    // it can be unbound now
    if (m_pPdb->getCartoonParam(CARTOON_PER_PIXEL_LIGHTING) == 1.0f)
	m_pFragmentShader->unbind();


    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    glDisableClientState( GL_NORMAL_ARRAY );


    if (m_bVBO_Supported)
    {
	m_oVertexBuffer.releaseBuffer( GL_ARRAY_BUFFER_ARB );
	m_oIndexBuffer.releaseBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB );
    }
    else
	m_bRegenerateStartIndices = false;

    //printf("svt_container_cartoon_dynamic::drawGL() %lu\n", svt_getToD()-iTime);
}


/**
 * delete all elements
 */
void svt_container_cartoon_dynamic::delAll()
{
    m_iNumVertices = 0;
    m_bVA_Generated = false;
    m_bRegenerateStartIndices = true;

    m_aVertices.clear();
    m_aIndices.clear();

    m_aTubeStripStart.clear();
    m_aTubeStripSize.clear();
    m_aTubeStripStartIndices.clear();

    m_aHelixStripSize.clear();
    m_aHelixStripStart.clear();
    m_aHelixStripStartIndices.clear();

    m_aSheetStripSize.clear();
    m_aSheetStripStart.clear();
    m_aSheetStripStartIndices.clear();
};


/**
 * draw the content of the container (Alias/Wavefront)
 */
void svt_container_cartoon_dynamic::drawOBJ()
{
    svtout << "Please deactivate vertex/pixel shader in the Cartoon properties dialog to enable OBJ export of the cartoons!" << endl;
}


/**
 * check if shader programs are supported and set the internal boolean to indicate this.
 * (needs vertex profile ARBVP1 and fragment profile ARBFP1)
 */
bool svt_container_cartoon_dynamic::checkShaderSupport()
{
    // return true if we already know about support and initialized the shaders
    if ( m_bShaderInitialized )
        return m_bShaderInitialized;

    if ( cgGLIsProfileSupported(CG_PROFILE_ARBVP1) == CG_TRUE && cgGLIsProfileSupported(CG_PROFILE_ARBFP1) == CG_TRUE )
    {
        m_bShaderSupported = true;

        if (!m_bShaderInitialized)
            initShader();
    }
    else
    {
        m_bShaderSupported = false;
    }

    return m_bShaderSupported;
}


/**
 * activate/deactivate per pixel shading
 */
void svt_container_cartoon_dynamic::setPerPixelLighting(bool bPerPixelLighting)
{
    m_bPerPixelLighting = bPerPixelLighting;
}


/**
 * get if per pixel shading is enabled
 */
bool svt_container_cartoon_dynamic::getPerPixelLighting()
{
    return m_bPerPixelLighting;
}


/**
 * This method initializes all the Cg shader
 */
void svt_container_cartoon_dynamic::initShader()
{
    if ( !svt_getAllowShaderPrograms() )
        return;

    //m_pTubeVertexShader_VertexLighting = new svt_shader_cg_tube(true, "/data/shader/pTubeVertexShader_VertexLighting.cg");
    m_pTubeVertexShader_VertexLighting = new svt_shader_cg_tube(false, pTubeVertexShader_VertexLighting, NULL);

    //m_pTubeVertexShader_VertexLighting_Fog = new svt_shader_cg_tube(true, "/home/mwahle/Shader/pTubeVertexShader_VertexLighting_Fog.cg", NULL, GL_LINEAR);
    m_pTubeVertexShader_VertexLighting_Fog = new svt_shader_cg_tube(false, pTubeVertexShader_VertexLighting_Fog, NULL, true);

    //m_pTubeVertexShader_PixelLighting = new svt_shader_cg_tube(true, "/data/shader/tub_ppl.cg");
    m_pTubeVertexShader_PixelLighting = new svt_shader_cg_tube(false, pTubeVertexShader_PixelLighting, NULL);

    //m_pFragmentShader_PixelLighting = new svt_shader_cg_cartoon_fragment(GL_FALSE, true, "/data/shader/pFragmentShader_PixelLighting.cg");
    m_pFragmentShader_PixelLighting = new svt_shader_cg_cartoon_fragment(GL_FALSE, false, pFragmentShader_PixelLighting);

    //m_pFragmentShader_PixelLighting_Fog_Exp = new svt_shader_cg_cartoon_fragment(GL_EXP, true, "/data/shader/pFragmentShader_PixelLighting_Fog_Exp.cg");
    m_pFragmentShader_PixelLighting_Fog_Exp = new svt_shader_cg_cartoon_fragment(GL_EXP, false, pFragmentShader_PixelLighting_Fog_Exp);

    //m_pFragmentShader_PixelLighting_Fog_Exp2 = new svt_shader_cg_cartoon_fragment(GL_EXP2, true, "/data/shader/pFragmentShader_PixelLighting_Fog_Exp2.cg");
    m_pFragmentShader_PixelLighting_Fog_Exp2 = new svt_shader_cg_cartoon_fragment(GL_EXP2, false, pFragmentShader_PixelLighting_Fog_Exp2);

   //m_pFragmentShader_PixelLighting_Fog_Linear = new svt_shader_cg_cartoon_fragment(GL_LINEAR, true, "/data/shader/pFragmentShader_PixelLighting_Fog_Linear.cg");
    m_pFragmentShader_PixelLighting_Fog_Linear = new svt_shader_cg_cartoon_fragment(GL_LINEAR, false, pFragmentShader_PixelLighting_Fog_Linear);

    //m_pFragmentShader_PixelLighting_FrontAndBack = new svt_shader_cg_cartoon_fragment(GL_FALSE, true, "/data/shader/pFragmentShader_PixelLighting_FrontAndBack.cg");
    m_pFragmentShader_PixelLighting_FrontAndBack = new svt_shader_cg_cartoon_fragment(GL_FALSE, false, pFragmentShader_PixelLighting_FrontAndBack);

    //m_pFragmentShader_PixelLighting_Fog_Exp_FrontAndBack = new svt_shader_cg_cartoon_fragment(GL_EXP, true, "/data/shader/pFragmentShader_PixelLighting_Fog_Exp_FrontAndBack.cg");
    m_pFragmentShader_PixelLighting_Fog_Exp_FrontAndBack = new svt_shader_cg_cartoon_fragment(GL_EXP, false, pFragmentShader_PixelLighting_Fog_Exp_FrontAndBack);

    //m_pFragmentShader_PixelLighting_Fog_Exp2_FrontAndBack = new svt_shader_cg_cartoon_fragment(GL_EXP2, true, "/data/shader/pFragmentShader_PixelLighting_Fog_Exp2_FrontAndBack.cg");
    m_pFragmentShader_PixelLighting_Fog_Exp2_FrontAndBack = new svt_shader_cg_cartoon_fragment(GL_EXP2, false, pFragmentShader_PixelLighting_Fog_Exp2_FrontAndBack);

    //m_pFragmentShader_PixelLighting_Fog_Linear_FrontAndBack = new svt_shader_cg_cartoon_fragment(GL_LINEAR, true, "/data/shader/pFragmentShader_PixelLighting_Fog_Linear_FrontAndBack.cg");
    m_pFragmentShader_PixelLighting_Fog_Linear_FrontAndBack = new svt_shader_cg_cartoon_fragment(GL_LINEAR, false, pFragmentShader_PixelLighting_Fog_Linear_FrontAndBack);

    //m_pCartoonVertexShaderRound_VertexLighting = new svt_shader_cg_tube_helixsheet(true, "/home/mwahle/Shader/pCartoonVertexShaderRound_VertexLighting.cg");
    m_pCartoonVertexShaderRound_VertexLighting = new svt_shader_cg_tube_helixsheet(false, pCartoonVertexShaderRound_VertexLighting, NULL, false);

    //m_pCartoonVertexShaderRound_VertexLighting_Fog = new svt_shader_cg_tube_helixsheet(true, "/home/mwahle/Shader/pCartoonVertexShaderRound_VertexLighting_Fog.cg", NULL, true);
    m_pCartoonVertexShaderRound_VertexLighting_Fog = new svt_shader_cg_tube_helixsheet(false, pCartoonVertexShaderRound_VertexLighting_Fog, NULL, true);

    //m_pCartoonVertexShaderRound_PixelLighting = new svt_shader_cg_tube_helixsheet(true, "/home/mwahle/Shader/pCartoonVertexShaderRound_PixelLighting.cg");
    m_pCartoonVertexShaderRound_PixelLighting = new svt_shader_cg_tube_helixsheet(false, pCartoonVertexShaderRound_PixelLighting, NULL);

    //m_pCartoonVertexShaderFlatHelix_VertexLighting = new svt_shader_cg_cartoon(true, "/data/shader/pCartoonVertexShaderFlatHelix_VertexLighting.cg");
    m_pCartoonVertexShaderFlatHelix_VertexLighting = new svt_shader_cg_cartoon(false, pCartoonVertexShaderFlatHelix_VertexLighting);

    //m_pCartoonVertexShaderFlatHelix_VertexLighting_Fog = new svt_shader_cg_cartoon(true, "/data/shader/pCartoonVertexShaderFlatHelix_VertexLighting_Fog.cg", NULL, true);
    m_pCartoonVertexShaderFlatHelix_VertexLighting_Fog = new svt_shader_cg_cartoon(false, pCartoonVertexShaderFlatHelix_VertexLighting_Fog, NULL, true);

    //m_pCartoonVertexShaderFlatHelix_PixelLighting = new svt_shader_cg_cartoon(true, "/data/shader/pCartoonVertexShaderFlatHelix_PixelLighting.cg");
    m_pCartoonVertexShaderFlatHelix_PixelLighting = new svt_shader_cg_cartoon(false, pCartoonVertexShaderFlatHelix_PixelLighting);

    //m_pCartoonVertexShaderFlatSheet_VertexLighting = new svt_shader_cg_cartoon(true, "/data/shader/CartoonVertexShaderFlatSheet_VertexLighting.cg");
    m_pCartoonVertexShaderFlatSheet_VertexLighting = new svt_shader_cg_cartoon(false, pCartoonVertexShaderFlatSheet_VertexLighting);

    //m_pCartoonVertexShaderFlatSheet_VertexLighting_Fog = new svt_shader_cg_cartoon(true, "/data/shader/CartoonVertexShaderFlatSheet_VertexLighting_Fog.cg", NULL, true);
    m_pCartoonVertexShaderFlatSheet_VertexLighting_Fog = new svt_shader_cg_cartoon(false, pCartoonVertexShaderFlatSheet_VertexLighting_Fog, NULL, true);

    //m_pCartoonVertexShaderFlatSheet_PixelLighting = new svt_shader_cg_cartoon(true, "/data/shader/CartoonVertexShaderFlatSheet_PixelLighting.cg");
    m_pCartoonVertexShaderFlatSheet_PixelLighting = new svt_shader_cg_cartoon(false, pCartoonVertexShaderFlatSheet_PixelLighting);

    //svt_shader_cg_cartoon * pCartoonVertexShader = new svt_shader_cg_cartoon(true, "/data/shader/cartoon0_VertexLighting.cg");
    svt_shader_cg_cartoon * pCartoonVertexShader = new svt_shader_cg_cartoon(false, pVert0_VertexLighting);
    m_aCartoonVertexShader_VertexLighting.push_back(pCartoonVertexShader);

    //pCartoonVertexShader = new svt_shader_cg_cartoon(true, "/data/shader/cartoon1_VertexLighting.cg");
    pCartoonVertexShader = new svt_shader_cg_cartoon(false, pVert1_VertexLighting);
    m_aCartoonVertexShader_VertexLighting.push_back(pCartoonVertexShader);

    //pCartoonVertexShader = new svt_shader_cg_cartoon(true, "/data/shader/cartoon2_VertexLighting.cg");
    pCartoonVertexShader = new svt_shader_cg_cartoon(false, pVert2_VertexLighting);
    m_aCartoonVertexShader_VertexLighting.push_back(pCartoonVertexShader);

    //pCartoonVertexShader = new svt_shader_cg_cartoon(true, "/data/shader/cartoon3_VertexLighting.cg");
    pCartoonVertexShader = new svt_shader_cg_cartoon(false, pVert3_VertexLighting);
    m_aCartoonVertexShader_VertexLighting.push_back(pCartoonVertexShader);


    //pCartoonVertexShader = new svt_shader_cg_cartoon(true, "/data/shader/cartoon0_VertexLighting_Fog.cg", NULL, true);
    pCartoonVertexShader = new svt_shader_cg_cartoon(false, pVert0_VertexLighting_Fog, NULL, true);
    m_aCartoonVertexShader_VertexLighting_Fog.push_back(pCartoonVertexShader);

    //pCartoonVertexShader = new svt_shader_cg_cartoon(true, "/data/shader/cartoon1_VertexLighting_Fog.cg", NULL, true);
    pCartoonVertexShader = new svt_shader_cg_cartoon(false, pVert1_VertexLighting_Fog, NULL, true);
    m_aCartoonVertexShader_VertexLighting_Fog.push_back(pCartoonVertexShader);

    //pCartoonVertexShader = new svt_shader_cg_cartoon(true, "/data/shader/cartoon2_VertexLighting_Fog.cg", NULL, true);
    pCartoonVertexShader = new svt_shader_cg_cartoon(false, pVert2_VertexLighting_Fog, NULL, true);
    m_aCartoonVertexShader_VertexLighting_Fog.push_back(pCartoonVertexShader);

    //pCartoonVertexShader = new svt_shader_cg_cartoon(true, "/data/shader/cartoon3_VertexLighting_Fog.cg", NULL, true);
    pCartoonVertexShader = new svt_shader_cg_cartoon(false, pVert3_VertexLighting_Fog, NULL, true);
    m_aCartoonVertexShader_VertexLighting_Fog.push_back(pCartoonVertexShader);


    //svt_shader_cg_cartoon * pCartoonVertexShader = new svt_shader_cg_cartoon(true, "/data/shader/cartoon0_PixelLighting.cg");
    pCartoonVertexShader = new svt_shader_cg_cartoon(false, pVert0_PixelLighting);
    m_aCartoonVertexShader_PixelLighting.push_back(pCartoonVertexShader);

    //pCartoonVertexShader = new svt_shader_cg_cartoon(true, "/data/shader/cartoon1_PixelLighting.cg");
    pCartoonVertexShader = new svt_shader_cg_cartoon(false, pVert1_PixelLighting);
    m_aCartoonVertexShader_PixelLighting.push_back(pCartoonVertexShader);

    //pCartoonVertexShader = new svt_shader_cg_cartoon(true, "/data/shader/cartoon2_PixelLighting.cg");
    pCartoonVertexShader = new svt_shader_cg_cartoon(false, pVert2_PixelLighting);
    m_aCartoonVertexShader_PixelLighting.push_back(pCartoonVertexShader);

    //pCartoonVertexShader = new svt_shader_cg_cartoon(true, "/data/shader/cartoon3_PixelLighting.cg");
    pCartoonVertexShader = new svt_shader_cg_cartoon(false, pVert3_PixelLighting);
    m_aCartoonVertexShader_PixelLighting.push_back(pCartoonVertexShader);

    m_bShaderInitialized = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// svt_container_cartoon_static ////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/**
 * Constructor
 */
svt_container_cartoon_static::svt_container_cartoon_static(svt_pdb * pPdb) : svt_container_cartoon(pPdb)
{
    m_bVBO_Supported = false;
    delAll();
}


/**
 * Destructor
 */
svt_container_cartoon_static::~svt_container_cartoon_static()
{
}

/*
*
 * draw the content of the container (Alias/Wavefront)
 */
void svt_container_cartoon_static::drawOBJ()
{
    vector< svt_face > aFaces;
    vector< svt_vector4<Real32> > aVecs;
    vector< svt_vector4<Real32> > aNorms;

    svt_face oFace;
    svt_vector4<Real32> oVec;
    svt_vector4<Real32> oNorm;
    svt_color oCurrCol;

    unsigned int iStart, iEnd;
    //
    // First: Pump all the vertices into the array
    //
    for( i=0; i<m_aVertices.size(); i++)
    {
        // vertices of the face
        oVec.set(
                 m_aVertices[ i ].m_oV[0],
                 m_aVertices[ i ].m_oV[1],
                 m_aVertices[ i ].m_oV[2]
                );
        aVecs.push_back( oVec );

        // normals of the face
        oNorm.set(
                  m_aVertices[ i ].m_oN[0],
                  m_aVertices[ i ].m_oN[1],
                  m_aVertices[ i ].m_oN[2]
                 );
        aNorms.push_back( oNorm );
    }

    if (m_aVertices.size() > 0)
        oCurrCol.set(
                     (Real32)(m_aVertices[ 0 ].m_oC[0]) / 255.0f,
                     (Real32)(m_aVertices[ 0 ].m_oC[1]) / 255.0f,
                     (Real32)(m_aVertices[ 0 ].m_oC[2]) / 255.0f
                    );
    svt_getScene()->getAliasOBJ().setColor( oCurrCol );


    //
    // draw all triangle strips
    //
    for(i=0; i<m_aTriStripStart.size(); ++i)
    {
	//
	// write all faces
	//
	if (m_bVBO_Supported)
	    iStart = m_aIndices[*(   (unsigned int*) (m_aTriStripStart[i])   )];
	else
	    iStart = *(   (unsigned int*) (m_aTriStripStart[i])   );

	iEnd = iStart + m_aTriStripSize[i];

	unsigned int iCount = 0;
	for (j=iStart; j<iEnd-2; j++)
	{
	    // did the colors change?
	    if (
		fabs( oCurrCol.getR() - (Real32)(m_aVertices[ m_aIndices[j] ].m_oC[0]) / 255.0f ) > 0.01f ||
		fabs( oCurrCol.getG() - (Real32)(m_aVertices[ m_aIndices[j] ].m_oC[1]) / 255.0f ) > 0.01f ||
		fabs( oCurrCol.getB() - (Real32)(m_aVertices[ m_aIndices[j] ].m_oC[2]) / 255.0f ) > 0.01f )
	    {
		oCurrCol.set(
		    (Real32)(m_aVertices[ m_aIndices[j] ].m_oC[0]) / 255.0f,
		    (Real32)(m_aVertices[ m_aIndices[j] ].m_oC[1]) / 255.0f,
		    (Real32)(m_aVertices[ m_aIndices[j] ].m_oC[2]) / 255.0f
		    );
		svt_getScene()->getAliasOBJ().setColor( oCurrCol, aFaces.size() );
	    }

	    // add face
	    if (iCount % 2 == 0)
	    {
		// face
		oFace.setVI_A( m_aIndices[j] +1 );
		oFace.setVI_C( m_aIndices[j+1] +1 );
		oFace.setVI_B( m_aIndices[j+2] +1 );
		oFace.setNI_A( m_aIndices[j] +1 );
		oFace.setNI_C( m_aIndices[j+1] +1 );
		oFace.setNI_B( m_aIndices[j+2] +1 );
		aFaces.push_back( oFace );

	    } else {
		// face
		oFace.setVI_A( m_aIndices[j] +1 );
		oFace.setVI_B( m_aIndices[j+1] +1 );
		oFace.setVI_C( m_aIndices[j+2] +1 );
		oFace.setNI_A( m_aIndices[j] +1 );
		oFace.setNI_B( m_aIndices[j+1] +1 );
		oFace.setNI_C( m_aIndices[j+2] +1 );
		aFaces.push_back( oFace );
	    }

	    ++iCount;

	}
    }

    //
    // draw all triangle fans
    //
    for(i=0; i<m_aTriFanStart.size(); ++i)
    {
	//
	// write all faces
	//
	if (m_bVBO_Supported)
	    iStart = m_aIndices[*(   (unsigned int*) (m_aTriFanStart[i])   )];
	else
	    iStart = *(   (unsigned int*) (m_aTriFanStart[i])   );

	iEnd = iStart + m_aTriFanSize[i];

	for (j=iStart; j<iEnd-2; j++)
	{
	    // did the colors change?
	    if (
		fabs( oCurrCol.getR() - (Real32)(m_aVertices[ m_aIndices[j] ].m_oC[0]) / 255.0f ) > 0.01f ||
		fabs( oCurrCol.getG() - (Real32)(m_aVertices[ m_aIndices[j] ].m_oC[1]) / 255.0f ) > 0.01f ||
		fabs( oCurrCol.getB() - (Real32)(m_aVertices[ m_aIndices[j] ].m_oC[2]) / 255.0f ) > 0.01f )
	    {
		oCurrCol.set(
		    (Real32)(m_aVertices[ m_aIndices[j] ].m_oC[0]) / 255.0f,
		    (Real32)(m_aVertices[ m_aIndices[j] ].m_oC[1]) / 255.0f,
		    (Real32)(m_aVertices[ m_aIndices[j] ].m_oC[2]) / 255.0f
		    );
		svt_getScene()->getAliasOBJ().setColor( oCurrCol, aFaces.size() );
	    }


	    oFace.setVI_A( m_aIndices[iStart] +1 );
	    oFace.setVI_B( m_aIndices[j+1] +1 );
	    oFace.setVI_C( m_aIndices[j+2] +1 );
	    oFace.setNI_A( m_aIndices[iStart] +1 );
	    oFace.setNI_B( m_aIndices[j+1] +1 );
	    oFace.setNI_C( m_aIndices[j+2] +1 );

	    aFaces.push_back( oFace );
	}
    }

    if (aFaces.size() > 0)
	svt_getScene()->getAliasOBJ().addMesh( aVecs, aNorms, aFaces );
}


/**
 * Start a tube
 */
void svt_container_cartoon_static::beginTube( )
{
    m_aTmpVertices.clear();
    m_aBinormals.clear();
    m_aNormals.clear();
    m_bRegenerateStartIndices = true;
}


/**
 * End a tube, and create the corresponding geometry
 */
void svt_container_cartoon_static::endTube( )
{
    unsigned int iTubeCorners = (unsigned int)m_pPdb->getNewTubeProfileCorners();
    Real32 aTubeCos[iTubeCorners+1];
    Real32 aTubeSin[iTubeCorners+1];
    Real32 fCos, fSin, fX, fY, fZ;


    fY = m_aTmpVertices[0].v[3];
    for ( j=0, fX=0.0f; j < iTubeCorners; ++j, fX+=(2.0f*PI/((float)iTubeCorners)) )
    {
	aTubeCos[j] = cos(fX) * fY;
	aTubeSin[j] = sin(fX) * fY;
    }
    aTubeCos[iTubeCorners] = aTubeCos[0];
    aTubeSin[iTubeCorners] = aTubeSin[0];


    m_iLen = m_aTmpVertices.size();


    memcpy(oN.c_data(), m_aTmpVertices[0].n, 12);
    oN.normalize();

    oT.set(m_aTmpVertices[1].v[0] - m_aTmpVertices[0].v[0],
	   m_aTmpVertices[1].v[1] - m_aTmpVertices[0].v[1],
	   m_aTmpVertices[1].v[2] - m_aTmpVertices[0].v[2]);
    oT.normalize();

    oB = crossProduct(oT, oN);

    m_aNormals  .push_back(oN);
    m_aBinormals.push_back(oB);



//#define DRAW_TUBE_FRAMES
#ifdef DRAW_TUBE_FRAMES
    glDisable(GL_LIGHTING); glBegin(GL_LINES);
    glColor3f(1.0f, 1.0f, 1.0f); glVertex3f(m_aVertices[iBeg].v[0], m_aVertices[iBeg].v[1], m_aVertices[iBeg].v[2]);
    glColor3f(1.0f, 1.0f, 1.0f); glVertex3f(m_aVertices[iBeg].v[0] + oT.x() * 0.01f, m_aVertices[iBeg].v[1] + oT.y() * 0.01f, m_aVertices[iBeg].v[2] + oT.z() * 0.01f);
    glColor3f(1.0f, 1.0f, 1.0f); glVertex3f(m_aVertices[iBeg].v[0], m_aVertices[iBeg].v[1], m_aVertices[iBeg].v[2]);
    glColor3f(1.0f, 1.0f, 1.0f); glVertex3f(m_aVertices[iBeg].v[0] + oN.x() * 0.01f, m_aVertices[iBeg].v[1] + oN.y() * 0.01f, m_aVertices[iBeg].v[2] + oN.z() * 0.01f);
    glEnd(); glEnable(GL_LIGHTING);
#endif


    for (j=0; j<m_iLen-2; ++j)
    {
	oTT.set(m_aTmpVertices[j+2].v[0] - m_aTmpVertices[j+1].v[0], 
		m_aTmpVertices[j+2].v[1] - m_aTmpVertices[j+1].v[1], 
		m_aTmpVertices[j+2].v[2] - m_aTmpVertices[j+1].v[2]);
	oTT.normalize();

	// oB used as parallel transport spinning axis
	oB = crossProduct(oT, oTT);
	oB.normalize();

	fCos = oT * oTT;
	// if oT and oTT are almost parallel, just reuse previous normal/binormal
	if (fCos > 0.99999f)
	{
	    m_aNormals.push_back(m_aNormals.back());
	    m_aBinormals.push_back(m_aBinormals.back());
	    memcpy(oT.c_data(), oTT.c_data(), 12);
	    continue;
	}
	fSin = sin(acos(fCos));

	fX = (fCos + (oB.x() * oB.x() * (1.0f-fCos))) * oN.x()
	    + ((oB.x() * oB.y() * (1.0f-fCos)) - fSin * oB.z()) * oN.y()
	    + ((oB.z() * oB.x() * (1.0f-fCos)) + fSin * oB.y()) * oN.z();

	fY = ( oB.x() * oB.y() * (1.0f-fCos)  + fSin * oB.z()) * oN.x()
	    + (fCos + oB.y() * oB.y() * (1.0f-fCos)) * oN.y()
	    + (oB.z() * oB.y() * (1.0f-fCos)  - fSin * oB.x()) * oN.z();

	fZ = ( oB.x() * oB.z() * (1.0f-fCos) - fSin * oB.y()) * oN.x()
	    + (oB.y() * oB.z() * (1.0f-fCos) + fSin * oB.x()) * oN.y()
	    + (fCos + oB.z() * oB.z() * (1.0f-fCos)) * oN.z();

	oN.set(fX, fY, fZ);
	//oN.normalize();

        oB = crossProduct(oT, oN);
	//oB.normalize();

        m_aNormals  .push_back(oN);
        m_aBinormals.push_back(oB);

	memcpy(oT.c_data(), oTT.c_data(), 12);

#ifdef DRAW_TUBE_FRAMES
	glDisable(GL_LIGHTING); glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(m_aTmpVertices[j+2].v[0], m_aTmpVertices[j+2].v[1], m_aTmpVertices[j+2].v[2]);
	glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(m_aTmpVertices[j+2].v[0] + oT.x() * 0.01f, m_aTmpVertices[j+2].v[1] + oT.y() * 0.01f, m_aTmpVertices[j+2].v[2] + oT.z() * 0.01f);
	glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(m_aTmpVertices[j+2].v[0], m_aTmpVertices[j+2].v[1], m_aTmpVertices[j+2].v[2]);
	glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(m_aTmpVertices[j+2].v[0] + oN.x() * 0.01f, m_aTmpVertices[j+2].v[1] + oN.y() * 0.01f, m_aTmpVertices[j+2].v[2] + oN.z() * 0.01f);
	glEnd(); glEnable(GL_LIGHTING);
#endif
    }
    m_aNormals  .push_back(m_aNormals.back());
    m_aBinormals.push_back(m_aBinormals.back());



    // tri strips around the tube body. possible optimization: re-use vertices, since right vertices
    // of tri strip n are identical with left vertices of strip n+1
    for (i=0; i<iTubeCorners; ++i)
    {

	k = m_iNumVertices;
	if (m_bVBO_Supported)
	    m_aTriStripStart.push_back((char*)(NULL) + (sizeof(unsigned int) * k));
	else
	    m_aTriStripStartIndices.push_back((GLint)k);

	for (j=0; j<m_iLen; ++j)
	{

	    // vertex on right side of quad strip
	    oN = m_aNormals[j] * aTubeCos[i+1] + m_aBinormals[j] * aTubeSin[i+1];
	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						   oN.x(), oN.y(), oN.z(),
						   m_aTmpVertices[j].v[0] + oN.x(), m_aTmpVertices[j].v[1] + oN.y(), m_aTmpVertices[j].v[2] + oN.z()));
	    m_aIndices.push_back(m_iNumVertices++);

	    // vertex on left side of quad strip
	    oN = m_aNormals[j] * aTubeCos[i] + m_aBinormals[j] * aTubeSin[i];
	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						   oN.x(), oN.y(), oN.z(),
						   m_aTmpVertices[j].v[0] + oN.x(), m_aTmpVertices[j].v[1] + oN.y(), m_aTmpVertices[j].v[2] + oN.z()));
	    m_aIndices.push_back(m_iNumVertices++);


	}
	m_aTriStripSize.push_back((GLsizei)(m_iNumVertices - k));
    }



    // cap the tubes
    if (m_bCaps)
    {
	///////////////// cap at the beginning /////////////////
	k = m_iNumVertices;
	if (m_bVBO_Supported)
	    m_aTriFanStart.push_back((char*)(NULL)+(sizeof(unsigned int) * k));
	else
	    m_aTriFanStartIndices.push_back((GLint)k);


	// normal is the inverse first tangent
	oN.set(m_aTmpVertices[0].v[0] - m_aTmpVertices[1].v[0],
	       m_aTmpVertices[0].v[1] - m_aTmpVertices[1].v[1],
	       m_aTmpVertices[0].v[2] - m_aTmpVertices[1].v[2]);
	oN.normalize();

	// center vertex
	m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[0].c[0]), (unsigned char) (255.0f * m_aTmpVertices[0].c[1]), (unsigned char) (255.0f * m_aTmpVertices[0].c[2]),
					       oN.x(), oN.y(), oN.z(),
					       m_aTmpVertices[0].v[0], m_aTmpVertices[0].v[1], m_aTmpVertices[0].v[2]));
	m_aIndices.push_back(m_iNumVertices++);


	// vertices on the outer ring
	for (i=iTubeCorners; i>0; --i)
	{
	    oB = m_aNormals[0] * aTubeCos[i] + m_aBinormals[0] * aTubeSin[i];
	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[0].c[0]), (unsigned char) (255.0f * m_aTmpVertices[0].c[1]), (unsigned char) (255.0f * m_aTmpVertices[0].c[2]),
						   oN.x(), oN.y(), oN.z(),
						   m_aTmpVertices[0].v[0] + oB.x(), m_aTmpVertices[0].v[1] + oB.y(), m_aTmpVertices[0].v[2] + oB.z()));
	    m_aIndices.push_back(m_iNumVertices++);
	}

	// close ring with first vertex
	oB = m_aNormals[0] * aTubeCos[0] + m_aBinormals[0] * aTubeSin[0];
	m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[0].c[0]), (unsigned char) (255.0f * m_aTmpVertices[0].c[1]), (unsigned char) (255.0f * m_aTmpVertices[0].c[2]),
					       oN.x(), oN.y(), oN.z(),
					       m_aTmpVertices[0].v[0] + oB.x(), m_aTmpVertices[0].v[1] + oB.y(), m_aTmpVertices[0].v[2] + oB.z()));
	m_aIndices.push_back(m_iNumVertices++);

	m_aTriFanSize.push_back((GLsizei)(m_iNumVertices - k));


	///////////////// cap at the end /////////////////
	k = m_iNumVertices;
	if (m_bVBO_Supported)
	    m_aTriFanStart.push_back((char*)(NULL)+(sizeof(unsigned int) * k));
	else
	    m_aTriFanStartIndices.push_back((GLint)k);

	// normal is the last tangent
	oN.set(m_aTmpVertices[m_aTmpVertices.size()-1].v[0] - m_aTmpVertices[m_aTmpVertices.size()-2].v[0],
	       m_aTmpVertices[m_aTmpVertices.size()-1].v[1] - m_aTmpVertices[m_aTmpVertices.size()-2].v[1],
	       m_aTmpVertices[m_aTmpVertices.size()-1].v[2] - m_aTmpVertices[m_aTmpVertices.size()-2].v[2]);
	oN.normalize();

	// center vertex
	m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices.back().c[0]), (unsigned char) (255.0f * m_aTmpVertices.back().c[1]), (unsigned char) (255.0f * m_aTmpVertices.back().c[2]),
					       oN.x(), oN.y(), oN.z(),
					       m_aTmpVertices.back().v[0] , m_aTmpVertices.back().v[1] , m_aTmpVertices.back().v[2]));
	m_aIndices.push_back(m_iNumVertices++);


	// vertices on the outer ring
	for (i=0; i<iTubeCorners; ++i)
	{
	    oB = m_aNormals.back() * aTubeCos[i] + m_aBinormals.back() * aTubeSin[i];
	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices.back().c[0]), (unsigned char) (255.0f * m_aTmpVertices.back().c[1]), (unsigned char) (255.0f * m_aTmpVertices.back().c[2]),
						   oN.x(), oN.y(), oN.z(),
						   m_aTmpVertices.back().v[0] + oB.x(), m_aTmpVertices.back().v[1] + oB.y(), m_aTmpVertices.back().v[2] + oB.z()));
	    m_aIndices.push_back(m_iNumVertices++);
	}

	// close ring with first vertex
	oB = m_aNormals.back() * aTubeCos[0] + m_aBinormals.back() * aTubeSin[0];
	m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices.back().c[0]), (unsigned char) (255.0f * m_aTmpVertices.back().c[1]), (unsigned char) (255.0f * m_aTmpVertices.back().c[2]),
					       oN.x(), oN.y(), oN.z(),
					       m_aTmpVertices.back().v[0] + oB.x(), m_aTmpVertices.back().v[1] + oB.y(), m_aTmpVertices.back().v[2] + oB.z()));
	m_aIndices.push_back(m_iNumVertices++);

	m_aTriFanSize.push_back((GLsizei)(m_iNumVertices - k));
    }
}


/**
 * Start a helix
 */
void svt_container_cartoon_static::beginHelix( )
{
    m_aTmpVertices.clear();
    m_aBinormals.clear();
    m_aNormals.clear();
    m_bRegenerateStartIndices = true;
}


/**
 * End a helix, and create the corresponding geometry
 */
void svt_container_cartoon_static::endHelix( )
{
    unsigned int iHelixCorners = (unsigned int)m_pPdb->getCartoonParam(CARTOON_SHEET_HELIX_PROFILE_CORNERS);
    Real32 aTubeCos[iHelixCorners+1];
    Real32 aTubeSin[iHelixCorners+1];
    Real32 fX; 
    Real32 fHeight = m_pPdb->getCartoonParam(CARTOON_HELIX_HEIGHT) * CARTOON_SCALING_FACTOR;

    m_iLen = m_aTmpVertices.size();

    for (j=0; j<m_iLen-1; ++j)
    {
	oB.set(m_aTmpVertices[j].n[0], m_aTmpVertices[j].n[1], m_aTmpVertices[j].n[2]);
	oB.normalize();

	oT.set(m_aTmpVertices[j+1].v[0] - m_aTmpVertices[j].v[0],
	       m_aTmpVertices[j+1].v[1] - m_aTmpVertices[j].v[1],
	       m_aTmpVertices[j+1].v[2] - m_aTmpVertices[j].v[2]);
	oT.normalize();

	oB = oB * (m_aTmpVertices[j].v[3]);
	m_aBinormals.push_back(oB);
	        
	oN = crossProduct(oT, oB);


	oN.normalize();
	oN = oN * fHeight;
	m_aNormals.push_back(oN);
    }

    oB.normalize();
    oB = oB * (m_aTmpVertices[j].v[3] + 0.0001f); // + 0.0001f, otherwise the binormal used as normal will be zero, screwing up the lighting
    m_aBinormals.push_back(oB);
    m_aNormals.push_back(oN);


    ////////////////////////////////////////////////////// flat sheets/helices //////////////////////////////////////////////////////////
    //
    if (m_pPdb->getCartoonParam(CARTOON_SHEET_HELIX_PROFILE_CORNERS) == 2.0f)
    {
	////////// only one tri strip //////////
	//
	k = m_iNumVertices;
	if (m_bVBO_Supported)
	    m_aTriStripStart.push_back((char*)(NULL)+(sizeof(unsigned int) * k));
	else
	    m_aTriStripStartIndices.push_back((GLint)k);


	for (j=0; j<m_iLen; ++j)
	{
	    oT.set(m_aTmpVertices[j].v[0],
		   m_aTmpVertices[j].v[1],
		   m_aTmpVertices[j].v[2]);

	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						   m_aNormals[j][0], m_aNormals[j][1], m_aNormals[j][2], 
						   oT[0] + m_aBinormals[j][0],
						   oT[1] + m_aBinormals[j][1],
						   oT[2] + m_aBinormals[j][2]));
	    m_aIndices.push_back(m_iNumVertices++);

	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						   m_aNormals[j][0], m_aNormals[j][1], m_aNormals[j][2], 
						   oT[0] - m_aBinormals[j][0],
						   oT[1] - m_aBinormals[j][1],
						   oT[2] - m_aBinormals[j][2]));
	    m_aIndices.push_back(m_iNumVertices++);
	}
	m_aTriStripSize.push_back((GLsizei)(m_iNumVertices - k));
    }

    ////////////////////////////////////////////////////// rectangular sheets/helices //////////////////////////////////////////////////////////
    //
    else if (m_pPdb->getCartoonParam(CARTOON_SHEET_HELIX_PROFILE_CORNERS) == 4.0f)
    {

	////////// upper tri strip //////////
	//
	k = m_iNumVertices;
	if (m_bVBO_Supported)
	    m_aTriStripStart.push_back((char*)(NULL)+(sizeof(unsigned int) * k));
	else
	    m_aTriStripStartIndices.push_back((GLint)k);


	for (j=0; j<m_iLen; ++j)
	{
	    oT.set(m_aTmpVertices[j].v[0] + m_aBinormals[j][0],
		   m_aTmpVertices[j].v[1] + m_aBinormals[j][1],
		   m_aTmpVertices[j].v[2] + m_aBinormals[j][2]);

	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						   m_aBinormals[j][0], m_aBinormals[j][1], m_aBinormals[j][2], 
						   oT[0] + m_aNormals[j][0],
						   oT[1] + m_aNormals[j][1],
						   oT[2] + m_aNormals[j][2]));
	    m_aIndices.push_back(m_iNumVertices++);

	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						   m_aBinormals[j][0], m_aBinormals[j][1], m_aBinormals[j][2], 
						   oT[0] - m_aNormals[j][0],
						   oT[1] - m_aNormals[j][1],
						   oT[2] - m_aNormals[j][2]));
	    m_aIndices.push_back(m_iNumVertices++);
	}
	m_aTriStripSize.push_back((GLsizei)(m_iNumVertices - k));


	////////// lower tri strip //////////
	//
	k = m_iNumVertices;
	if (m_bVBO_Supported)
	    m_aTriStripStart.push_back((char*)(NULL)+(sizeof(unsigned int) * k));
	else
	    m_aTriStripStartIndices.push_back((GLint)k);


	for (j=0; j<m_iLen; ++j)
	{
	    oT.set(m_aTmpVertices[j].v[0] - m_aBinormals[j][0],
		   m_aTmpVertices[j].v[1] - m_aBinormals[j][1],
		   m_aTmpVertices[j].v[2] - m_aBinormals[j][2]);

	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						   -1.0f * m_aBinormals[j][0], -1.0f * m_aBinormals[j][1], -1.0f * m_aBinormals[j][2], 
						   oT[0] - m_aNormals[j][0],
						   oT[1] - m_aNormals[j][1],
						   oT[2] - m_aNormals[j][2]));
	    m_aIndices.push_back(m_iNumVertices++);

	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						   -1.0f * m_aBinormals[j][0], -1.0f * m_aBinormals[j][1], -1.0f * m_aBinormals[j][2], 
						   oT[0] + m_aNormals[j][0],
						   oT[1] + m_aNormals[j][1],
						   oT[2] + m_aNormals[j][2]));
	    m_aIndices.push_back(m_iNumVertices++);
	}
	m_aTriStripSize.push_back((GLsizei)(m_iNumVertices - k));


	//////////  left tri strip //////////
	//
	k = m_iNumVertices;
	if (m_bVBO_Supported)
	    m_aTriStripStart.push_back((char*)(NULL)+(sizeof(unsigned int) * k));
	else
	    m_aTriStripStartIndices.push_back((GLint)k);


	for (j=0; j<m_iLen; ++j)
	{
	    oT.set(m_aTmpVertices[j].v[0] - m_aNormals[j][0],
		   m_aTmpVertices[j].v[1] - m_aNormals[j][1],
		   m_aTmpVertices[j].v[2] - m_aNormals[j][2]);

	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						   -1.0f * m_aNormals[j][0], -1.0f * m_aNormals[j][1], -1.0f * m_aNormals[j][2], 
						   oT[0] + m_aBinormals[j][0],
						   oT[1] + m_aBinormals[j][1],
						   oT[2] + m_aBinormals[j][2]));
	    m_aIndices.push_back(m_iNumVertices++);

	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						   -1.0f * m_aNormals[j][0], -1.0f * m_aNormals[j][1], -1.0f * m_aNormals[j][2], 
						   oT[0] - m_aBinormals[j][0],
						   oT[1] - m_aBinormals[j][1],
						   oT[2] - m_aBinormals[j][2]));
	    m_aIndices.push_back(m_iNumVertices++);
	}
	m_aTriStripSize.push_back((GLsizei)(m_iNumVertices - k));


	////////// right tri strip //////////
	//
	k = m_iNumVertices;
	if (m_bVBO_Supported)
	    m_aTriStripStart.push_back((char*)(NULL)+(sizeof(unsigned int) * k));
	else
	    m_aTriStripStartIndices.push_back((GLint)k);


	for (j=0; j<m_iLen; ++j)
	{
	    oT.set(m_aTmpVertices[j].v[0] + m_aNormals[j][0],
		   m_aTmpVertices[j].v[1] + m_aNormals[j][1],
		   m_aTmpVertices[j].v[2] + m_aNormals[j][2]);

	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						   m_aNormals[j][0], m_aNormals[j][1], m_aNormals[j][2], 
						   oT[0] - m_aBinormals[j][0],
						   oT[1] - m_aBinormals[j][1],
						   oT[2] - m_aBinormals[j][2]));
	    m_aIndices.push_back(m_iNumVertices++);

	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						   m_aNormals[j][0], m_aNormals[j][1], m_aNormals[j][2], 
						   oT[0] + m_aBinormals[j][0],
						   oT[1] + m_aBinormals[j][1],
						   oT[2] + m_aBinormals[j][2]));
	    m_aIndices.push_back(m_iNumVertices++);
	}
	m_aTriStripSize.push_back((GLsizei)(m_iNumVertices - k));


	if (m_bCaps)
	{
	    ///////////////// cap at the beginning /////////////////
	    k = m_iNumVertices;
	    if (m_bVBO_Supported)
		m_aTriFanStart.push_back((char*)(NULL)+(sizeof(unsigned int) * k));
	    else
		m_aTriFanStartIndices.push_back((GLint)k);

	    // normal is the inverse first tangent
	    oN.set(m_aTmpVertices[0].v[0] - m_aTmpVertices[1].v[0],
		   m_aTmpVertices[0].v[1] - m_aTmpVertices[1].v[1],
		   m_aTmpVertices[0].v[2] - m_aTmpVertices[1].v[2]);
	    oN.normalize();

	    // center vertex
	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[0].c[0]), (unsigned char) (255.0f * m_aTmpVertices[0].c[1]), (unsigned char) (255.0f * m_aTmpVertices[0].c[2]),
						   oN.x(), oN.y(), oN.z(),
						   m_aTmpVertices[0].v[0], m_aTmpVertices[0].v[1], m_aTmpVertices[0].v[2]));
	    m_aIndices.push_back(m_iNumVertices++);


	    // vertices on the outer "ring"
	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[0].c[0]), (unsigned char) (255.0f * m_aTmpVertices[0].c[1]), (unsigned char) (255.0f * m_aTmpVertices[0].c[2]),
						   oN.x(), oN.y(), oN.z(),
						   m_aTmpVertices[0].v[0] + m_aBinormals[0][0] + m_aNormals[0][0], m_aTmpVertices[0].v[1] + m_aBinormals[0][1] + m_aNormals[0][1], m_aTmpVertices[0].v[2] + m_aBinormals[0][2] + m_aNormals[0][2]));
	    m_aIndices.push_back(m_iNumVertices++);


	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[0].c[0]), (unsigned char) (255.0f * m_aTmpVertices[0].c[1]), (unsigned char) (255.0f * m_aTmpVertices[0].c[2]),
						   oN.x(), oN.y(), oN.z(),
						   m_aTmpVertices[0].v[0] + m_aBinormals[0][0] - m_aNormals[0][0], m_aTmpVertices[0].v[1] + m_aBinormals[0][1] - m_aNormals[0][1], m_aTmpVertices[0].v[2] + m_aBinormals[0][2] - m_aNormals[0][2]));
	    m_aIndices.push_back(m_iNumVertices++);



	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[0].c[0]), (unsigned char) (255.0f * m_aTmpVertices[0].c[1]), (unsigned char) (255.0f * m_aTmpVertices[0].c[2]),
						   oN.x(), oN.y(), oN.z(),
						   m_aTmpVertices[0].v[0] - m_aBinormals[0][0] - m_aNormals[0][0], m_aTmpVertices[0].v[1] - m_aBinormals[0][1] - m_aNormals[0][1], m_aTmpVertices[0].v[2] - m_aBinormals[0][2] - m_aNormals[0][2]));
	    m_aIndices.push_back(m_iNumVertices++);


	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[0].c[0]), (unsigned char) (255.0f * m_aTmpVertices[0].c[1]), (unsigned char) (255.0f * m_aTmpVertices[0].c[2]),
						   oN.x(), oN.y(), oN.z(),
						   m_aTmpVertices[0].v[0] - m_aBinormals[0][0] + m_aNormals[0][0], m_aTmpVertices[0].v[1] - m_aBinormals[0][1] + m_aNormals[0][1], m_aTmpVertices[0].v[2] - m_aBinormals[0][2] + m_aNormals[0][2]));
	    m_aIndices.push_back(m_iNumVertices++);


	    // close ring with first vertex
	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[0].c[0]), (unsigned char) (255.0f * m_aTmpVertices[0].c[1]), (unsigned char) (255.0f * m_aTmpVertices[0].c[2]),
						   oN.x(), oN.y(), oN.z(),
						   m_aTmpVertices[0].v[0] + m_aBinormals[0][0] + m_aNormals[0][0], m_aTmpVertices[0].v[1] + m_aBinormals[0][1] + m_aNormals[0][1], m_aTmpVertices[0].v[2] + m_aBinormals[0][2] + m_aNormals[0][2]));
	    m_aIndices.push_back(m_iNumVertices++);


	    m_aTriFanSize.push_back((GLsizei)(m_iNumVertices - k));


	    if (!m_pPdb->getCartoonParam(CARTOON_HELIX_ARROWHEADS))
	    {
		///////////////// cap at the end /////////////////
		k = m_iNumVertices;
		if (m_bVBO_Supported)
		    m_aTriFanStart.push_back((char*)(NULL)+(sizeof(unsigned int) * k));
		else
		    m_aTriFanStartIndices.push_back((GLint)k);


		// normal is the last tangent
		oN.set(m_aTmpVertices[m_aTmpVertices.size()-1].v[0] - m_aTmpVertices[m_aTmpVertices.size()-2].v[0],
		       m_aTmpVertices[m_aTmpVertices.size()-1].v[1] - m_aTmpVertices[m_aTmpVertices.size()-2].v[1],
		       m_aTmpVertices[m_aTmpVertices.size()-1].v[2] - m_aTmpVertices[m_aTmpVertices.size()-2].v[2]);
		oN.normalize();

		// center vertex
		m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices.back().c[0]), (unsigned char) (255.0f * m_aTmpVertices.back().c[1]), (unsigned char) (255.0f * m_aTmpVertices.back().c[2]),
						       oN.x(), oN.y(), oN.z(),
						       m_aTmpVertices.back().v[0], m_aTmpVertices.back().v[1], m_aTmpVertices.back().v[2]));
		m_aIndices.push_back(m_iNumVertices++);


		// vertices on the outer "ring"
		m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices.back().c[0]), (unsigned char) (255.0f * m_aTmpVertices.back().c[1]), (unsigned char) (255.0f * m_aTmpVertices.back().c[2]),
						       oN.x(), oN.y(), oN.z(),
						       m_aTmpVertices.back().v[0] + m_aBinormals.back()[0] + m_aNormals.back()[0], m_aTmpVertices.back().v[1] + m_aBinormals.back()[1] + m_aNormals.back()[1], m_aTmpVertices.back().v[2] + m_aBinormals.back()[2] + m_aNormals.back()[2]));
		m_aIndices.push_back(m_iNumVertices++);


		m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices.back().c[0]), (unsigned char) (255.0f * m_aTmpVertices.back().c[1]), (unsigned char) (255.0f * m_aTmpVertices.back().c[2]),
						       oN.x(), oN.y(), oN.z(),
						       m_aTmpVertices.back().v[0] - m_aBinormals.back()[0] + m_aNormals.back()[0], m_aTmpVertices.back().v[1] - m_aBinormals.back()[1] + m_aNormals.back()[1], m_aTmpVertices.back().v[2] - m_aBinormals.back()[2] + m_aNormals.back()[2]));
		m_aIndices.push_back(m_iNumVertices++);


		m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices.back().c[0]), (unsigned char) (255.0f * m_aTmpVertices.back().c[1]), (unsigned char) (255.0f * m_aTmpVertices.back().c[2]),
						       oN.x(), oN.y(), oN.z(),
						       m_aTmpVertices.back().v[0] - m_aBinormals.back()[0] - m_aNormals.back()[0], m_aTmpVertices.back().v[1] - m_aBinormals.back()[1] - m_aNormals.back()[1], m_aTmpVertices.back().v[2] - m_aBinormals.back()[2] - m_aNormals.back()[2]));
		m_aIndices.push_back(m_iNumVertices++);


		m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices.back().c[0]), (unsigned char) (255.0f * m_aTmpVertices.back().c[1]), (unsigned char) (255.0f * m_aTmpVertices.back().c[2]),
						       oN.x(), oN.y(), oN.z(),
						       m_aTmpVertices.back().v[0] + m_aBinormals.back()[0] - m_aNormals.back()[0], m_aTmpVertices.back().v[1] + m_aBinormals.back()[1] - m_aNormals.back()[1], m_aTmpVertices.back().v[2] + m_aBinormals.back()[2] - m_aNormals.back()[2]));
		m_aIndices.push_back(m_iNumVertices++);


		// close ring with first vertex
		m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices.back().c[0]), (unsigned char) (255.0f * m_aTmpVertices.back().c[1]), (unsigned char) (255.0f * m_aTmpVertices.back().c[2]),
						       oN.x(), oN.y(), oN.z(),
						       m_aTmpVertices.back().v[0] + m_aBinormals.back()[0] + m_aNormals.back()[0], m_aTmpVertices.back().v[1] + m_aBinormals.back()[1] + m_aNormals.back()[1], m_aTmpVertices.back().v[2] + m_aBinormals.back()[2] + m_aNormals.back()[2]));
		m_aIndices.push_back(m_iNumVertices++);


		m_aTriFanSize.push_back((GLsizei)(m_iNumVertices - k));
	    }
	}
    }

    ////////////////////////////////////////////////////// round sheets/helices //////////////////////////////////////////////////////////
    //
    else if (m_pPdb->getCartoonParam(CARTOON_SHEET_HELIX_PROFILE_CORNERS) > 5.0f)
    {

	for ( j=0, fX=0.0f; j < iHelixCorners; ++j, fX+=(2.0f*PI/((float)iHelixCorners)) )
	{
	    aTubeCos[j] = cos(fX);
	    aTubeSin[j] = sin(fX);
	}
	aTubeCos[iHelixCorners] = aTubeCos[0];
	aTubeSin[iHelixCorners] = aTubeSin[0];

	// cap the round helices
	if (m_bCaps)
	{
	    ///////////////// cap at the beginning /////////////////
	    k = m_iNumVertices;
	    if (m_bVBO_Supported)
		m_aTriFanStart.push_back((char*)(NULL)+(sizeof(unsigned int) * k));
	    else
		m_aTriFanStartIndices.push_back((GLint)k);


	    // normal is the inverse first tangent
	    oN.set(m_aTmpVertices[0].v[0] - m_aTmpVertices[1].v[0],
		   m_aTmpVertices[0].v[1] - m_aTmpVertices[1].v[1],
		   m_aTmpVertices[0].v[2] - m_aTmpVertices[1].v[2]);
	    oN.normalize();

	    // center vertex
	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[0].c[0]), (unsigned char) (255.0f * m_aTmpVertices[0].c[1]), (unsigned char) (255.0f * m_aTmpVertices[0].c[2]),
						   oN.x(), oN.y(), oN.z(),
						   m_aTmpVertices[0].v[0], m_aTmpVertices[0].v[1], m_aTmpVertices[0].v[2]));
	    m_aIndices.push_back(m_iNumVertices++);


	    // vertices on the outer ring
	    for (i=0; i<iHelixCorners; ++i)
	    {
		oB = m_aNormals[0] * aTubeCos[i] + m_aBinormals[0] * aTubeSin[i];
		m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[0].c[0]), (unsigned char) (255.0f * m_aTmpVertices[0].c[1]), (unsigned char) (255.0f * m_aTmpVertices[0].c[2]),
						       oN.x(), oN.y(), oN.z(),
						       m_aTmpVertices[0].v[0] + oB.x(), m_aTmpVertices[0].v[1] + oB.y(), m_aTmpVertices[0].v[2] + oB.z()));
		m_aIndices.push_back(m_iNumVertices++);
	    }

	    // close ring with first vertex
	    oB = m_aNormals[0] * aTubeCos[0] + m_aBinormals[0] * aTubeSin[0];
	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[0].c[0]), (unsigned char) (255.0f * m_aTmpVertices[0].c[1]), (unsigned char) (255.0f * m_aTmpVertices[0].c[2]),
						   oN.x(), oN.y(), oN.z(),
						   m_aTmpVertices[0].v[0] + oB.x(), m_aTmpVertices[0].v[1] + oB.y(), m_aTmpVertices[0].v[2] + oB.z()));
	    m_aIndices.push_back(m_iNumVertices++);

	    m_aTriFanSize.push_back((GLsizei)(m_iNumVertices - k));


	    if (!m_pPdb->getCartoonParam(CARTOON_HELIX_ARROWHEADS))
	    {
		///////////////// cap at the end /////////////////
		k = m_iNumVertices;
		if (m_bVBO_Supported)
		    m_aTriFanStart.push_back((char*)(NULL)+(sizeof(unsigned int) * k));
		else
		    m_aTriFanStartIndices.push_back((GLint)k);

		// normal is the last tangent
		oN.set(m_aTmpVertices[m_aTmpVertices.size()-1].v[0] - m_aTmpVertices[m_aTmpVertices.size()-2].v[0],
		       m_aTmpVertices[m_aTmpVertices.size()-1].v[1] - m_aTmpVertices[m_aTmpVertices.size()-2].v[1],
		       m_aTmpVertices[m_aTmpVertices.size()-1].v[2] - m_aTmpVertices[m_aTmpVertices.size()-2].v[2]);
		oN.normalize();

		// center vertex
		m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices.back().c[0]), (unsigned char) (255.0f * m_aTmpVertices.back().c[1]), (unsigned char) (255.0f * m_aTmpVertices.back().c[2]),
						       oN.x(), oN.y(), oN.z(),
						       m_aTmpVertices.back().v[0] , m_aTmpVertices.back().v[1] , m_aTmpVertices.back().v[2]));
		m_aIndices.push_back(m_iNumVertices++);


		// vertices on the outer ring
		for (i=0; i<iHelixCorners; ++i)
		{
		    oB = m_aNormals.back() * aTubeCos[i] + m_aBinormals.back() * aTubeSin[i];
		    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices.back().c[0]), (unsigned char) (255.0f * m_aTmpVertices.back().c[1]), (unsigned char) (255.0f * m_aTmpVertices.back().c[2]),
							   oN.x(), oN.y(), oN.z(),
							   m_aTmpVertices.back().v[0] + oB.x(), m_aTmpVertices.back().v[1] + oB.y(), m_aTmpVertices.back().v[2] + oB.z()));
		    m_aIndices.push_back(m_iNumVertices++);
		}

		// close ring with first vertex
		oB = m_aNormals.back() * aTubeCos[0] + m_aBinormals.back() * aTubeSin[0];
		m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices.back().c[0]), (unsigned char) (255.0f * m_aTmpVertices.back().c[1]), (unsigned char) (255.0f * m_aTmpVertices.back().c[2]),
						       oN.x(), oN.y(), oN.z(),
						       m_aTmpVertices.back().v[0] + oB.x(), m_aTmpVertices.back().v[1] + oB.y(), m_aTmpVertices.back().v[2] + oB.z()));
		m_aIndices.push_back(m_iNumVertices++);


		m_aTriFanSize.push_back((GLsizei)(m_iNumVertices - k));
	    }
	}


	// now compute the triangle strips along the helix

	for (j=0; j<m_iLen; ++j)
	{
	    m_aNormals[j].normalize();
	    m_aBinormals[j].normalize();
	}

	for (i=0; i<iHelixCorners; ++i)
	{

	    k = m_iNumVertices;
	    if (m_bVBO_Supported)
		m_aTriStripStart.push_back((char*)(NULL)+(sizeof(unsigned int) * k));
	    else
		m_aTriStripStartIndices.push_back((GLint)k);


	    for (j=0; j<m_iLen; ++j)
	    {
		// vertex on left side of quad strip
		oNN = m_aNormals[j] * (aTubeCos[i] * fHeight) + m_aBinormals[j] * (aTubeSin[i] * m_aTmpVertices[j].v[3]);
		oN  = m_aNormals[j] *  aTubeCos[i]            + m_aBinormals[j] *  aTubeSin[i];

		m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						       oN.x(), oN.y(), oN.z(),
						       m_aTmpVertices[j].v[0] + oNN.x(), m_aTmpVertices[j].v[1] + oNN.y(), m_aTmpVertices[j].v[2] + oNN.z()));
		m_aIndices.push_back(m_iNumVertices++);


		// vertex on right side of quad strip
		oNN = m_aNormals[j] * (aTubeCos[i+1] * fHeight) + m_aBinormals[j] * (aTubeSin[i+1] * m_aTmpVertices[j].v[3]);
		oN  = m_aNormals[j] *  aTubeCos[i+1]            + m_aBinormals[j] *  aTubeSin[i+1];

		m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						       oN.x(), oN.y(), oN.z(),
						       m_aTmpVertices[j].v[0] + oNN.x(), m_aTmpVertices[j].v[1] + oNN.y(), m_aTmpVertices[j].v[2] + oNN.z()));
		m_aIndices.push_back(m_iNumVertices++);
	    }
	    m_aTriStripSize.push_back((GLsizei)(m_iNumVertices - k));
	}

    }
}


/**
 * Start a sheet
 */
void svt_container_cartoon_static::beginSheet( )
{
    m_aTmpVertices.clear();
    m_aBinormals.clear();
    m_aNormals.clear();
    m_bRegenerateStartIndices = true;
}


/**
 * End a sheet, and create the corresponding geometry
 */
void svt_container_cartoon_static::endSheet( )
{
    unsigned int iSheetCorners = (unsigned int)m_pPdb->getCartoonParam(CARTOON_SHEET_HELIX_PROFILE_CORNERS);
    Real32 aTubeCos[iSheetCorners+1];
    Real32 aTubeSin[iSheetCorners+1];
    Real32 fX;
    Real32 fHeight = m_pPdb->getCartoonParam(CARTOON_SHEET_HEIGHT) * CARTOON_SCALING_FACTOR;

    m_iLen = m_aTmpVertices.size();

    for (j=0; j<m_iLen-1; ++j)
    {
	oB.set(m_aTmpVertices[j].n[0], m_aTmpVertices[j].n[1], m_aTmpVertices[j].n[2]);
	oB.normalize();

	oT.set(m_aTmpVertices[j+1].v[0] - m_aTmpVertices[j].v[0],
	       m_aTmpVertices[j+1].v[1] - m_aTmpVertices[j].v[1],
	       m_aTmpVertices[j+1].v[2] - m_aTmpVertices[j].v[2]);
	oT.normalize();

	oB.normalize();
	oB = oB * (m_aTmpVertices[j].v[3]);
	m_aBinormals.push_back(oB);
	        
	oN = crossProduct(oT, oB);

	oN.normalize();
	oN = oN * fHeight;
	m_aNormals.push_back(oN);
    }
    oB.normalize();
    oB = oB * (m_aTmpVertices[j].v[3] + 0.0001f); // + 0.0001f, otherwise the binormal used as normal will be zero, screwing up the lighting
    m_aBinormals.push_back(oB);
    m_aNormals.push_back(oN);


//#define DRAW_SHEET_REFERENCE_FRAMES
#ifdef DRAW_SHEET_REFERENCE_FRAMES
    glLineWidth(5.0f);
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    for (j=0; j<m_aNormals.size(); ++j)
    {
	if ( j != m_aNormals.size()-1)
	oT.set(m_aTmpVertices[j+2].v[0] - m_aTmpVertices[j].v[0],
	       m_aTmpVertices[j+2].v[1] - m_aTmpVertices[j].v[1],
	       m_aTmpVertices[j+2].v[2] - m_aTmpVertices[j].v[2]);
	oT.normalize();
	oT = oT * 0.009f;

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(m_aTmpVertices[j].v[0],          m_aTmpVertices[j].v[1],          m_aTmpVertices[j].v[2]         );
	glVertex3f(m_aTmpVertices[j].v[0] + oT.x(), m_aTmpVertices[j].v[1] + oT.y(), m_aTmpVertices[j].v[2] + oT.z());

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(m_aTmpVertices[j].v[0],                     m_aTmpVertices[j].v[1],                     m_aTmpVertices[j].v[2]                    );
	glVertex3f(m_aTmpVertices[j].v[0] + m_aNormals[j].x(), m_aTmpVertices[j].v[1] + m_aNormals[j].y(), m_aTmpVertices[j].v[2] + m_aNormals[j].z());

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(m_aTmpVertices[j].v[0],                       m_aTmpVertices[j].v[1],                       m_aTmpVertices[j].v[2]                      );
	glVertex3f(m_aTmpVertices[j].v[0] + m_aBinormals[j].x(), m_aTmpVertices[j].v[1] + m_aBinormals[j].y(), m_aTmpVertices[j].v[2] + m_aBinormals[j].z());
    }
    glEnd();
    glEnable(GL_LIGHTING);
#endif



    ////////////////////////////////////////////////////// flat sheets/helices //////////////////////////////////////////////////////////
    //
    if (m_pPdb->getCartoonParam(CARTOON_SHEET_HELIX_PROFILE_CORNERS) == 2.0f)
    {

	////////// only one tri strip //////////
	//
	k = m_iNumVertices;
	if (m_bVBO_Supported)
	    m_aTriStripStart_FlatSheets.push_back((char*)(NULL)+(sizeof(unsigned int) * k));
	else
	    m_aTriStripStartIndices_FlatSheets.push_back((GLint)k);


	for (j=0; j<m_iLen; ++j)
	{
	    oT.set(m_aTmpVertices[j].v[0],
		   m_aTmpVertices[j].v[1],
		   m_aTmpVertices[j].v[2]);

	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						   m_aNormals[j][0], m_aNormals[j][1], m_aNormals[j][2], 
						   oT[0] + m_aBinormals[j][0],
						   oT[1] + m_aBinormals[j][1],
						   oT[2] + m_aBinormals[j][2]));
	    m_aIndices.push_back(m_iNumVertices++);

	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						   m_aNormals[j][0], m_aNormals[j][1], m_aNormals[j][2], 
						   oT[0] - m_aBinormals[j][0],
						   oT[1] - m_aBinormals[j][1],
						   oT[2] - m_aBinormals[j][2]));
	    m_aIndices.push_back(m_iNumVertices++);
	}
	m_aTriStripSize_FlatSheets.push_back((GLsizei)(m_iNumVertices - k));
    }

    ////////////////////////////////////////////////////// rectangular sheets/helices //////////////////////////////////////////////////////////
    //
    else if (m_pPdb->getCartoonParam(CARTOON_SHEET_HELIX_PROFILE_CORNERS) == 4.0f)
    {

	////////// upper tri strip //////////
	//
	k = m_iNumVertices;
	if (m_bVBO_Supported)
	    m_aTriStripStart.push_back((char*)(NULL)+(sizeof(unsigned int) * k));
	else
	    m_aTriStripStartIndices.push_back((GLint)k);


	for (j=0; j<m_iLen; ++j)
	{
	    oT.set(m_aTmpVertices[j].v[0] + m_aBinormals[j][0],
		   m_aTmpVertices[j].v[1] + m_aBinormals[j][1],
		   m_aTmpVertices[j].v[2] + m_aBinormals[j][2]);

	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						   m_aBinormals[j][0], m_aBinormals[j][1], m_aBinormals[j][2], 
						   oT[0] + m_aNormals[j][0],
						   oT[1] + m_aNormals[j][1],
						   oT[2] + m_aNormals[j][2]));
	    m_aIndices.push_back(m_iNumVertices++);

	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						   m_aBinormals[j][0], m_aBinormals[j][1], m_aBinormals[j][2], 
						   oT[0] - m_aNormals[j][0],
						   oT[1] - m_aNormals[j][1],
						   oT[2] - m_aNormals[j][2]));
	    m_aIndices.push_back(m_iNumVertices++);
	}
	m_aTriStripSize.push_back((GLsizei)(m_iNumVertices - k));


	////////// lower tri strip //////////
	//
	k = m_iNumVertices;
	if (m_bVBO_Supported)
	    m_aTriStripStart.push_back((char*)(NULL)+(sizeof(unsigned int) * k));
	else
	    m_aTriStripStartIndices.push_back((GLint)k);


	for (j=0; j<m_iLen; ++j)
	{
	    oT.set(m_aTmpVertices[j].v[0] - m_aBinormals[j][0],
		   m_aTmpVertices[j].v[1] - m_aBinormals[j][1],
		   m_aTmpVertices[j].v[2] - m_aBinormals[j][2]);

	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						   -1.0f * m_aBinormals[j][0], -1.0f * m_aBinormals[j][1], -1.0f * m_aBinormals[j][2], 
						   oT[0] - m_aNormals[j][0],
						   oT[1] - m_aNormals[j][1],
						   oT[2] - m_aNormals[j][2]));
	    m_aIndices.push_back(m_iNumVertices++);

	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						   -1.0f * m_aBinormals[j][0], -1.0f * m_aBinormals[j][1], -1.0f * m_aBinormals[j][2], 
						   oT[0] + m_aNormals[j][0],
						   oT[1] + m_aNormals[j][1],
						   oT[2] + m_aNormals[j][2]));
	    m_aIndices.push_back(m_iNumVertices++);
	}
	m_aTriStripSize.push_back((GLsizei)(m_iNumVertices - k));


	//////////  left tri strip //////////
	//
	k = m_iNumVertices;
	if (m_bVBO_Supported)
	    m_aTriStripStart.push_back((char*)(NULL)+(sizeof(unsigned int) * k));
	else
	    m_aTriStripStartIndices.push_back((GLint)k);


	for (j=0; j<m_iLen; ++j)
	{
	    oT.set(m_aTmpVertices[j].v[0] - m_aNormals[j][0],
		   m_aTmpVertices[j].v[1] - m_aNormals[j][1],
		   m_aTmpVertices[j].v[2] - m_aNormals[j][2]);

	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						   -1.0f * m_aNormals[j][0], -1.0f * m_aNormals[j][1], -1.0f * m_aNormals[j][2], 
						   oT[0] + m_aBinormals[j][0],
						   oT[1] + m_aBinormals[j][1],
						   oT[2] + m_aBinormals[j][2]));
	    m_aIndices.push_back(m_iNumVertices++);

	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						   -1.0f * m_aNormals[j][0], -1.0f * m_aNormals[j][1], -1.0f * m_aNormals[j][2], 
						   oT[0] - m_aBinormals[j][0],
						   oT[1] - m_aBinormals[j][1],
						   oT[2] - m_aBinormals[j][2]));
	    m_aIndices.push_back(m_iNumVertices++);
	}
	m_aTriStripSize.push_back((GLsizei)(m_iNumVertices - k));


	////////// right tri strip //////////
	//
	k = m_iNumVertices;
	if (m_bVBO_Supported)
	    m_aTriStripStart.push_back((char*)(NULL)+(sizeof(unsigned int) * k));
	else
	    m_aTriStripStartIndices.push_back((GLint)k);


	for (j=0; j<m_iLen; ++j)
	{
	    oT.set(m_aTmpVertices[j].v[0] + m_aNormals[j][0],
		   m_aTmpVertices[j].v[1] + m_aNormals[j][1],
		   m_aTmpVertices[j].v[2] + m_aNormals[j][2]);

	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						   m_aNormals[j][0], m_aNormals[j][1], m_aNormals[j][2], 
						   oT[0] - m_aBinormals[j][0],
						   oT[1] - m_aBinormals[j][1],
						   oT[2] - m_aBinormals[j][2]));
	    m_aIndices.push_back(m_iNumVertices++);

	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						   m_aNormals[j][0], m_aNormals[j][1], m_aNormals[j][2], 
						   oT[0] + m_aBinormals[j][0],
						   oT[1] + m_aBinormals[j][1],
						   oT[2] + m_aBinormals[j][2]));
	    m_aIndices.push_back(m_iNumVertices++);
	}
	m_aTriStripSize.push_back((GLsizei)(m_iNumVertices - k));


	if (m_bCaps)
	{
	    ///////////////// cap at the beginning /////////////////
	    k = m_iNumVertices;
	    if (m_bVBO_Supported)
		m_aTriFanStart.push_back((char*)(NULL)+(sizeof(unsigned int) * k));
	    else
		m_aTriFanStartIndices.push_back((GLint)k);


	    // normal is the inverse first tangent
	    oN.set(m_aTmpVertices[0].v[0] - m_aTmpVertices[1].v[0],
		   m_aTmpVertices[0].v[1] - m_aTmpVertices[1].v[1],
		   m_aTmpVertices[0].v[2] - m_aTmpVertices[1].v[2]);
	    oN.normalize();

	    // center vertex
	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[0].c[0]), (unsigned char) (255.0f * m_aTmpVertices[0].c[1]), (unsigned char) (255.0f * m_aTmpVertices[0].c[2]),
						   oN.x(), oN.y(), oN.z(),
						   m_aTmpVertices[0].v[0], m_aTmpVertices[0].v[1], m_aTmpVertices[0].v[2]));
	    m_aIndices.push_back(m_iNumVertices++);


	    // vertices on the outer "ring"
	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[0].c[0]), (unsigned char) (255.0f * m_aTmpVertices[0].c[1]), (unsigned char) (255.0f * m_aTmpVertices[0].c[2]),
						   oN.x(), oN.y(), oN.z(),
						   m_aTmpVertices[0].v[0] + m_aBinormals[0][0] + m_aNormals[0][0], m_aTmpVertices[0].v[1] + m_aBinormals[0][1] + m_aNormals[0][1], m_aTmpVertices[0].v[2] + m_aBinormals[0][2] + m_aNormals[0][2]));
	    m_aIndices.push_back(m_iNumVertices++);


	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[0].c[0]), (unsigned char) (255.0f * m_aTmpVertices[0].c[1]), (unsigned char) (255.0f * m_aTmpVertices[0].c[2]),
						   oN.x(), oN.y(), oN.z(),
						   m_aTmpVertices[0].v[0] + m_aBinormals[0][0] - m_aNormals[0][0], m_aTmpVertices[0].v[1] + m_aBinormals[0][1] - m_aNormals[0][1], m_aTmpVertices[0].v[2] + m_aBinormals[0][2] - m_aNormals[0][2]));
	    m_aIndices.push_back(m_iNumVertices++);



	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[0].c[0]), (unsigned char) (255.0f * m_aTmpVertices[0].c[1]), (unsigned char) (255.0f * m_aTmpVertices[0].c[2]),
						   oN.x(), oN.y(), oN.z(),
						   m_aTmpVertices[0].v[0] - m_aBinormals[0][0] - m_aNormals[0][0], m_aTmpVertices[0].v[1] - m_aBinormals[0][1] - m_aNormals[0][1], m_aTmpVertices[0].v[2] - m_aBinormals[0][2] - m_aNormals[0][2]));
	    m_aIndices.push_back(m_iNumVertices++);


	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[0].c[0]), (unsigned char) (255.0f * m_aTmpVertices[0].c[1]), (unsigned char) (255.0f * m_aTmpVertices[0].c[2]),
						   oN.x(), oN.y(), oN.z(),
						   m_aTmpVertices[0].v[0] - m_aBinormals[0][0] + m_aNormals[0][0], m_aTmpVertices[0].v[1] - m_aBinormals[0][1] + m_aNormals[0][1], m_aTmpVertices[0].v[2] - m_aBinormals[0][2] + m_aNormals[0][2]));
	    m_aIndices.push_back(m_iNumVertices++);


	    // close ring with first vertex
	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[0].c[0]), (unsigned char) (255.0f * m_aTmpVertices[0].c[1]), (unsigned char) (255.0f * m_aTmpVertices[0].c[2]),
						   oN.x(), oN.y(), oN.z(),
						   m_aTmpVertices[0].v[0] + m_aBinormals[0][0] + m_aNormals[0][0], m_aTmpVertices[0].v[1] + m_aBinormals[0][1] + m_aNormals[0][1], m_aTmpVertices[0].v[2] + m_aBinormals[0][2] + m_aNormals[0][2]));
	    m_aIndices.push_back(m_iNumVertices++);


	    m_aTriFanSize.push_back((GLsizei)(m_iNumVertices - k));
	}
    }

    ////////////////////////////////////////////////////// round sheets/helices //////////////////////////////////////////////////////////
    //
    else if (m_pPdb->getCartoonParam(CARTOON_SHEET_HELIX_PROFILE_CORNERS) > 5.0f)
    {

	for ( j=0, fX=0.0f; j < iSheetCorners; ++j, fX+=(2.0f*PI/((float)iSheetCorners)) )
	{
	    aTubeCos[j] = cos(fX);
	    aTubeSin[j] = sin(fX);
	}
	aTubeCos[iSheetCorners] = aTubeCos[0];
	aTubeSin[iSheetCorners] = aTubeSin[0];

	// cap the round sheets
	if (m_bCaps)
	{
	    ///////////////// cap at the beginning /////////////////
	    k = m_iNumVertices;
	    if (m_bVBO_Supported)
		m_aTriFanStart.push_back((char*)(NULL)+(sizeof(unsigned int) * k));
	    else
		m_aTriFanStartIndices.push_back((GLint)k);


	    // normal is the inverse first tangent
	    oN.set(m_aTmpVertices[0].v[0] - m_aTmpVertices[1].v[0],
		   m_aTmpVertices[0].v[1] - m_aTmpVertices[1].v[1],
		   m_aTmpVertices[0].v[2] - m_aTmpVertices[1].v[2]);
	    oN.normalize();

	    // center vertex
	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[0].c[0]), (unsigned char) (255.0f * m_aTmpVertices[0].c[1]), (unsigned char) (255.0f * m_aTmpVertices[0].c[2]),
						   oN.x(), oN.y(), oN.z(),
						   m_aTmpVertices[0].v[0], m_aTmpVertices[0].v[1], m_aTmpVertices[0].v[2]));
	    m_aIndices.push_back(m_iNumVertices++);


	    // vertices on the outer ring
	    for (i=0; i<iSheetCorners; ++i)
	    {
		oB = m_aNormals[0] * aTubeCos[i] + m_aBinormals[0] * aTubeSin[i];
		m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[0].c[0]), (unsigned char) (255.0f * m_aTmpVertices[0].c[1]), (unsigned char) (255.0f * m_aTmpVertices[0].c[2]),
						       oN.x(), oN.y(), oN.z(),
						       m_aTmpVertices[0].v[0] + oB.x(), m_aTmpVertices[0].v[1] + oB.y(), m_aTmpVertices[0].v[2] + oB.z()));
		m_aIndices.push_back(m_iNumVertices++);
	    }

	    // close ring with first vertex
	    oB = m_aNormals[0] * aTubeCos[0] + m_aBinormals[0] * aTubeSin[0];
	    m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[0].c[0]), (unsigned char) (255.0f * m_aTmpVertices[0].c[1]), (unsigned char) (255.0f * m_aTmpVertices[0].c[2]),
						   oN.x(), oN.y(), oN.z(),
						   m_aTmpVertices[0].v[0] + oB.x(), m_aTmpVertices[0].v[1] + oB.y(), m_aTmpVertices[0].v[2] + oB.z()));
	    m_aIndices.push_back(m_iNumVertices++);

	    m_aTriFanSize.push_back((GLsizei)(m_iNumVertices - k));
	}


	// now compute the triangle strips along the sheet

	for (j=0; j<m_iLen; ++j)
	{
	    m_aNormals[j].normalize();
	    m_aBinormals[j].normalize();
	}

	for (i=0; i<iSheetCorners; ++i)
	{

	    k = m_iNumVertices;
	    if (m_bVBO_Supported)
		m_aTriStripStart.push_back((char*)(NULL)+(sizeof(unsigned int) * k));
	    else
		m_aTriStripStartIndices.push_back((GLint)k);


	    for (j=0; j<m_iLen; ++j)
	    {
		// vertex on left side of quad strip
		oNN = m_aNormals[j] * (aTubeCos[i] * fHeight) + m_aBinormals[j] * (aTubeSin[i] * m_aTmpVertices[j].v[3]);
		oN  = m_aNormals[j] *  aTubeCos[i]            + m_aBinormals[j] *  aTubeSin[i];

		m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						       oN.x(), oN.y(), oN.z(),
						       m_aTmpVertices[j].v[0] + oNN.x(), m_aTmpVertices[j].v[1] + oNN.y(), m_aTmpVertices[j].v[2] + oNN.z()));
		m_aIndices.push_back(m_iNumVertices++);


		// vertex on right side of quad strip
		oNN = m_aNormals[j] * (aTubeCos[i+1] * fHeight) + m_aBinormals[j] * (aTubeSin[i+1] * m_aTmpVertices[j].v[3]);
		oN  = m_aNormals[j] *  aTubeCos[i+1]            + m_aBinormals[j] *  aTubeSin[i+1];

		m_aVertices.push_back(svt_c4ub_n3f_v3f((unsigned char) (255.0f * m_aTmpVertices[j].c[0]), (unsigned char) (255.0f * m_aTmpVertices[j].c[1]), (unsigned char) (255.0f * m_aTmpVertices[j].c[2]),
						       oN.x(), oN.y(), oN.z(),
						       m_aTmpVertices[j].v[0] + oNN.x(), m_aTmpVertices[j].v[1] + oNN.y(), m_aTmpVertices[j].v[2] + oNN.z()));
		m_aIndices.push_back(m_iNumVertices++);
	    }
	    m_aTriStripSize.push_back((GLsizei)(m_iNumVertices - k));
	}
    }
}


/**
 * delete all elements
 */
void svt_container_cartoon_static::delAll()
{
    m_iNumVertices = 0;
    m_bVA_Generated = false;
    m_bRegenerateStartIndices = true;

    m_aVertices.clear();
    m_aIndices.clear();


    m_aTriStripStart.clear();
    m_aTriStripSize.clear();
    m_aTriStripStartIndices.clear();

    m_aTriStripSize_FlatSheets.clear();
    m_aTriStripStart_FlatSheets.clear();
    m_aTriStripStartIndices_FlatSheets.clear();

    m_aTriFanStart.clear();
    m_aTriFanSize.clear();
    m_aTriFanStartIndices.clear();
}


/**
 * draw the content of the container (OpenGL)
 */
void svt_container_cartoon_static::drawGL()
{
    if (m_iNumVertices == 0) return;

    //unsigned long iTime = svt_getToD();
    unsigned int iSize = sizeof(svt_c4ub_n3f_v3f);;

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glLineWidth(1.0f);


    if (m_bVBO_Supported)
    {
	if (m_bVA_Generated)
	{
	    m_oVertexBuffer.bindBuffer( GL_ARRAY_BUFFER_ARB );
	    m_oIndexBuffer.bindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB );
	}
	else
	{
	    m_oVertexBuffer.bindBuffer( GL_ARRAY_BUFFER_ARB );
	    m_oVertexBuffer.bufferData( GL_ARRAY_BUFFER_ARB, m_aVertices.size() * iSize, &m_aVertices[0], GL_DYNAMIC_DRAW_ARB);

	    m_oIndexBuffer.bindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB );
	    m_oIndexBuffer.bufferData( GL_ELEMENT_ARRAY_BUFFER_ARB, m_aIndices.size() * sizeof( unsigned int ), &m_aIndices[0], GL_DYNAMIC_DRAW_ARB);

	    m_bVA_Generated = true;
	}

	glColorPointer   ( 4, GL_UNSIGNED_BYTE, iSize, (char*)(NULL) );
	glNormalPointer  (    GL_FLOAT,         iSize, (char*)(NULL) +  (sizeof(unsigned char)*8) );
	glVertexPointer  ( 3, GL_FLOAT,         iSize, (char*)(NULL) +  (sizeof(unsigned char)*8) + 12 );
    }
    else
    {
	glColorPointer   ( 4, GL_UNSIGNED_BYTE, iSize, (void*)(&m_aVertices[0]) );
	glNormalPointer  (    GL_FLOAT,         iSize, (char*)(&m_aVertices[0]) + (sizeof(unsigned char)*8) );
	glVertexPointer  ( 3, GL_FLOAT,         iSize, (char*)(&m_aVertices[0]) + (sizeof(unsigned char)*8) + 12 );
    }


    glEnableClientState( GL_COLOR_ARRAY  );
    glEnableClientState( GL_NORMAL_ARRAY );
    glEnableClientState( GL_VERTEX_ARRAY );


// for performance measurements: triangle strips from the beginning through the end

//     iSize = m_aIndices.size();
//     glEnable(GL_COLOR_MATERIAL);
//     glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
//     glDrawRangeElementsEXT(GL_TRIANGLE_STRIP,
// 			0,
// 			iSize-1,
// 			iSize,
// 			GL_UNSIGNED_INT,
// 			&m_aIndices[0] );

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // the following loop fills m_aTriStripStart with pointers into the index array m_aIndices.
    // this must be done HERE, because only here it is guaranteed that m_aIndices doesn't move
    // anymore (since it is a vector, it is moved if its size needs to be increased, which can
    // happen only as long as indices are added).
    // not necessary for VBOs because they use the pointers as offsets, not as absolute pointers
    //
    iSize = m_aTriStripSize.size();
    if (iSize > 0)
    {
	if (!m_bVBO_Supported && m_bRegenerateStartIndices)
	{
	    m_aTriStripStart.clear();

	    for (i=0; i<iSize; ++i)
	    {
		m_aTriStripStart.push_back(&(m_aIndices[m_aTriStripStartIndices[i]]));
	    }
	}

      	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

#ifndef WIN32
	if (m_bUseMultiDraw)
	{
	    if (m_bVBO_Supported)
	    {
		glMultiDrawElementsEXT(GL_TRIANGLE_STRIP,
				       (const GLsizei*)&m_aTriStripSize[0],
				       GL_UNSIGNED_INT,
				       (const GLvoid* *)(&m_aTriStripStart[0]),
				       (GLsizei)iSize);
	    }
	    else
	    {
		glMultiDrawArraysEXT(GL_TRIANGLE_STRIP,
				     &m_aTriStripStartIndices[0],
				     &m_aTriStripSize[0],
				     (GLsizei)iSize);
	    }		    
	}
	else
#endif
	{
	    if (m_bVBO_Supported)
	    {
		for (i=0; i<iSize; ++i)
#ifndef WIN32
		    glDrawRangeElementsEXT(GL_TRIANGLE_STRIP,
					   m_aTriStripStartIndices[i],
					   m_aTriStripStartIndices[i] + m_aTriStripSize[i] - 1,
					   m_aTriStripSize[i],
					   GL_UNSIGNED_INT,
					   &m_aIndices[0] + m_aTriStripStartIndices[i]);
#else
		    glDrawElementsEXT(GL_TRIANGLE_STRIP,
				      m_aTriStripSize[i],
				      GL_UNSIGNED_INT,
				      &m_aTriStripStartIndices[i]);
#endif
	    }
	    else
	    {
		for (i=0; i<iSize; ++i)
		    glDrawArrays(GL_TRIANGLE_STRIP,
				 m_aTriStripStartIndices[i],
				 m_aTriStripSize[i]);
	    }
	}
    }


    // now do the same for the flat sheets (helices are simple triangle strips, so they are already
    // drawn
    //
    iSize = m_aTriStripSize_FlatSheets.size();
    if (iSize > 0)
    {
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	if (!m_bVBO_Supported && m_bRegenerateStartIndices)
	{
	    m_aTriStripStart_FlatSheets.clear();

	    for (i=0; i<iSize; ++i)
	    {
		m_aTriStripStart_FlatSheets.push_back(&(m_aIndices[m_aTriStripStartIndices_FlatSheets[i]]));
	    }
	}

#ifndef WIN32
	if (m_bUseMultiDraw)
	{
	    if (m_bVBO_Supported)
	    {
		glMultiDrawElementsEXT(GL_TRIANGLE_STRIP,
				       (const GLsizei*)&m_aTriStripSize_FlatSheets[0],
				       GL_UNSIGNED_INT,
				       (const GLvoid* *)(&m_aTriStripStart_FlatSheets[0]),
				       (GLsizei)iSize);
	    }
	    else
	    {
		glMultiDrawArraysEXT(GL_TRIANGLE_STRIP,
				     &m_aTriStripStartIndices_FlatSheets[0],
				     &m_aTriStripSize_FlatSheets[0],
				     (GLsizei)iSize);
	    }		    
	}
	else
#endif
	{
	    if (m_bVBO_Supported)
	    {
		for (i=0; i<iSize; ++i)
#ifndef WIN32
		    glDrawRangeElementsEXT(GL_TRIANGLE_STRIP,
					   m_aTriStripStartIndices_FlatSheets[i],
					   m_aTriStripStartIndices_FlatSheets[i] + m_aTriStripSize_FlatSheets[i] - 1,
					   m_aTriStripSize_FlatSheets[i],
					   GL_UNSIGNED_INT,
					   &m_aIndices[0] + m_aTriStripStartIndices_FlatSheets[i]);
#else
		    glDrawElementsEXT(GL_TRIANGLE_STRIP,
				      m_aTriStripSize_FlatSheets[i],
				      GL_UNSIGNED_INT,
				      &m_aTriStripStartIndices_FlatSheets[i]);
#endif
	    }
	    else
	    {
		for (i=0; i<iSize; ++i)
		    glDrawArrays(GL_TRIANGLE_STRIP,
				 m_aTriStripStartIndices_FlatSheets[i],
				 m_aTriStripSize_FlatSheets[i]);
	    }
	}
    }


    iSize = m_aTriFanSize.size();
    if (iSize > 0)
    {
	if (!m_bVBO_Supported && m_bRegenerateStartIndices)
	{
	    m_aTriFanStart.clear();

	    for (i=0; i<iSize; ++i)
	    {
		m_aTriFanStart.push_back(&(m_aIndices[m_aTriFanStartIndices[i]]));
	    }
	}

      	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

#ifndef WIN32
	if (m_bUseMultiDraw)
	{
	    if (m_bVBO_Supported)
	    {
		glMultiDrawElementsEXT(GL_TRIANGLE_FAN,
				       (const GLsizei*)&m_aTriFanSize[0],
				       GL_UNSIGNED_INT,
				       (const GLvoid* *)(&m_aTriFanStart[0]),
				       (GLsizei)iSize);
	    }
	    else
	    {
		glMultiDrawArraysEXT(GL_TRIANGLE_FAN,
				     &m_aTriFanStartIndices[0],
				     &m_aTriFanSize[0],
				     (GLsizei)iSize);
	    }		    
	}
	else
#endif
	{
	    if (m_bVBO_Supported)
	    {
		for (i=0; i<iSize; ++i)
#ifndef WIN32
		    glDrawRangeElementsEXT(GL_TRIANGLE_FAN,
					   m_aTriFanStartIndices[i],
					   m_aTriFanStartIndices[i] + m_aTriFanSize[i] - 1,
					   m_aTriFanSize[i],
					   GL_UNSIGNED_INT,
					   &m_aIndices[0] + m_aTriFanStartIndices[i]);
#else
		    glDrawElementsEXT(GL_TRIANGLE_STRIP,
				      m_aTriFanSize[i],
				      GL_UNSIGNED_INT,
				      &m_aTriFanStartIndices[i]);
#endif
	    }
	    else
	    {
		for (i=0; i<iSize; ++i)
		    glDrawArrays(GL_TRIANGLE_FAN,
				 m_aTriFanStartIndices[i],
				 m_aTriFanSize[i]);
	    }
	}
    }


    glDisableClientState( GL_COLOR_ARRAY );
    glDisableClientState( GL_NORMAL_ARRAY );
    glDisableClientState( GL_VERTEX_ARRAY );

    glDisable(GL_COLOR_MATERIAL);

    if (m_bVBO_Supported)
    {
	m_oVertexBuffer.releaseBuffer( GL_ARRAY_BUFFER_ARB );
	m_oIndexBuffer.releaseBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB );
    }

    if (!m_bVBO_Supported)
	m_bRegenerateStartIndices = false;

    //printf("svt_container_cartoon_static::drawGL() %lu\n", svt_getToD()-iTime);
}
