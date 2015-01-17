/*-*-c++-*-*****************************************************************
                          svt_shader_manager
                          ------------------
    begin                : 12/20/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_SHADER_MANAGER_H
#define __SVT_SHADER_MANAGER_H

// svt includes
class svt_shader;
class svt_shader_cg_toon;
class svt_shader_cg_ao_pass1;
class svt_shader_cg_ao_pass2;
class svt_shader_cg_ao_pass3;

/**
 * This class is responsible for holding the shaders. For now, all of them are held in source code,
 * but later on we may decide that it is better to load the shader source form the file system.
 *
 * Next to that, a centralized shader management has several advantages. Most prominently, this will
 * reduce redundancy or memory leaks.
 *
 * This class is not meant to implement some advanced logic, it shall just construct, destruct, and
 * return pointers to the shaders. Classes requesting shaders are still responsible to do their own
 * sanity checks.
 */
class svt_shader_manager {

    /**
     * Return the source code of an ambient occlusion shader. Allows for somewhat cleaner code. Good
     * for now, but is probably not an optimal solution.
     */
    static const char* getAOShaderSource(int iShader);

    /**
     * Return the source code of a vdw shader. Allows for somewhat cleaner code. Good
     * for now, but is probably not an optimal solution.
     */
    static const char* getVDWShaderSource(int iShader);

public:

    /**
     * Default constructor.
     */
    svt_shader_manager();

    /**
     * Destructor.
     */
    virtual ~svt_shader_manager();

    // the ao shaders
    svt_shader_cg_ao_pass1* m_pAOShaderPass1_PointBased;
    svt_shader_cg_ao_pass1* m_pAOShaderPass1_LineBased;
    svt_shader_cg_ao_pass2* m_pAOShaderPass2;
    svt_shader_cg_ao_pass3* m_pAOShaderPass3;

    // the toon-vdw shaders
    svt_shader_cg_toon* m_pVDWShaderVertex;
    svt_shader_cg_toon* m_pVDWShaderFragment_Plain;
    svt_shader_cg_toon* m_pVDWShaderFragment_FogLinear;
    svt_shader_cg_toon* m_pVDWShaderFragment_FogExp;
    svt_shader_cg_toon* m_pVDWShaderFragment_FogExp2;

    /**  list of the different ambient occlusion shaders */
    enum eAO_shader {
        SVT_SHADER_AO_PASS1_POINT_BASED,
        SVT_SHADER_AO_PASS1_LINE_BASED,
        SVT_SHADER_AO_PASS2,
        SVT_SHADER_AO_PASS3 };

    /** list of toon vdw shaders */
    enum eVDW_shader {
        SVT_SHADER_TOON_VDW_VERTEX,
        SVT_SHADER_TOON_VDW_FRAG_PLAIN,
        SVT_SHADER_TOON_VDW_FRAG_FOGLINEAR,
        SVT_SHADER_TOON_VDW_FRAG_FOGEXP,
        SVT_SHADER_TOON_VDW_FRAG_FOGEXP2
        };

    /**
     * Return pointer to an ambient occlusion shader. Initialize the shader if that didn't happen yet.
     */
    svt_shader* getAOShader(int iShader);

    /**
     * Return pointer to a vdw shader. Initialize the shader if that didn't happen yet.
     */
    svt_shader* getVDWShader(int iShader);
};

#endif
