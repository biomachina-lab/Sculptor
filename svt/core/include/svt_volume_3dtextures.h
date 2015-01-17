/***************************************************************************
                          svt_volume_3dtextures
                          ---------------------
    begin                : Oct 8 2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_VOLUME_3DTEXTURES
#define SVT_VOLUME_3DTEXTURES

extern bool __svt_mainloopWindow( void );

#ifndef GL_CLAMP_TO_BORDER
#define GL_CLAMP_TO_BORDER 0x812D
#endif

///////////////////////////////////////////////////////////////////////////////
// includes
///////////////////////////////////////////////////////////////////////////////

#include <svt_float2mat.h>
#include <svt_float2mat_linear.h>
#include <svt_volume.h>
#include <svt_texture.h>
#include <svt_texture_3d.h>
#include <svt_opengl.h>
#include <svt_shader_cg_volume.h>
#include <svt_shader_cg_volume_nospec.h>
#include <svt_shader_cg_volume_fast.h>
#include <svt_scene.h>
#include <svt_light.h>
#include <svt_lightsource.h>
#include <svt_time.h>
#include <svt_system.h>
#include <svt_volumeRenderer.h>
#include <svt_opengl.h>
#include <svt_vector4.h>

///////////////////////////////////////////////////////////////////////////////
// advanced opengl features
///////////////////////////////////////////////////////////////////////////////

// opengl 1.2 3D textures
#ifndef GL_VERSION_1_2
#define GL_TEXTURE_3D 0x806F
#define GL_TEXTURE_WRAP_R 0x8072
#endif

#ifdef SVT_SYSTEM_QT
#include <svt_window_qt.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// template class
///////////////////////////////////////////////////////////////////////////////

/**
 * Displays volume data by the help of 3d textures (Attention: Not all opengl implementations and graphics card support these).
 *@author Stefan Birmanns
 */
template<class T> class DLLEXPORT svt_volume_3dtextures : public svt_volumeRenderer<T>
{
protected:
    
    // count the number of instances of this class
    int m_iNumInstances;
    // transfer functions
    svt_float2mat* m_pCTF;
    svt_float2mat_linear* m_pCTFDefault;
    // size of textures (have to be power of 2!)
    unsigned int m_iTexSizeX;
    unsigned int m_iTexSizeY;
    unsigned int m_iTexSizeZ;
    // wie gross ist der schwarze Rand um die eigentlichen Daten?
    int m_iBordX, m_iBordY, m_iBordZ;
    // wieviel abstand zwischen den slices?
    float m_fGapXY, m_fGapXZ, m_fGapZY;
    // cg shader
    svt_shader_cg_volume* m_pShader;  // FIXME shaders should be static
    svt_shader_cg_volume_nospec* m_pShaderNospec;
    svt_shader_cg_volume_fast* m_pShaderFast;
    bool m_bShaderInitialized;

    // this is the dependent texture
    unsigned char m_aTrans[256*256*4]; 
    // precomputed integrals for dependent texture calculation
    float m_aT[256]; 
    float m_aKtauR[256];
    float m_aKtauG[256];
    float m_aKtauB[256];

    // texture handle for the dependent texture
    GLuint m_iTexHandle;
    bool m_bTexHandle;

    // sampling rate
    Real64 m_fSampling;

    bool m_bUpdateTF;
    bool m_bCalc3D;

    // phong shading parameter
    Real32 m_fDiffuse;
    Real32 m_fAmbient;
    Real32 m_fShininess;
    bool m_bShading;

    // last full renderer
    int m_iLastFull;

    // background intensity
    Real32 m_fZero;

    // start slice
    Real32 m_fStartSlice;
    Real32 m_fEndSlice;

public:

    /**
     * Constructor
     */
    svt_volume_3dtextures(svt_volume<T>& rVolume) : svt_volumeRenderer<T>(rVolume),
        m_pCTF( NULL ),
        m_pCTFDefault( NULL ),
        m_bShaderInitialized( false ),
	m_iTexHandle(0),
	m_bTexHandle(false),
	m_fSampling( 1.0 ),
	m_bUpdateTF(false),
	m_bCalc3D(false),
	m_fDiffuse( 0.8f ),
	m_fAmbient( 0.4f ),
	m_fShininess( 0.0 ),
	m_bShading( false ),
        m_iLastFull( 1000 ),
        m_fStartSlice( 0.0 ),
        m_fEndSlice( 1.0 )
    {
        initShader();
	this->setAutoDL(false);
    };
    virtual ~svt_volume_3dtextures()
    {
	if (m_pCTFDefault != NULL)
	    delete m_pCTFDefault;
        if (m_bShaderInitialized)
        {
            delete m_pShader;
            delete m_pShaderFast;
            delete m_pShaderNospec;
        }
    };

public:

    /**
     * Initialize the shaders
     */
    void initShader()
    {
        if ( svt_getAllowShaderPrograms() )
        {
            m_pShader       = new svt_shader_cg_volume;
            m_pShaderNospec = new svt_shader_cg_volume_nospec;
            m_pShaderFast   = new svt_shader_cg_volume_fast;
            m_bShaderInitialized = true;
        }
    };
    /**
     * Check if shaders are initialized
     */
    bool getShaderInitialized()
    {
        return m_bShaderInitialized;
    };
    /**
     * set lighting params
     * \param fDiffuse diffuse lighting factor (0..10)
     * \param fAmbient ambient light contribution (0..1)
     * \param fShininess shininess of the material (0..128)
     */
    void setLightingParams(Real32 fDiffuse, Real32 fAmbient, Real32 fShininess)
    {
	m_fDiffuse = fDiffuse;
	m_fAmbient = fAmbient;
	m_fShininess = fShininess;
    };
    /**
     * get ambient light contribution
     */
    Real32 getAmbient()
    {
	return m_fAmbient;
    };
    /**
     * set ambient light contribution
     */
    void setAmbient(Real32 f)
    {
        m_fAmbient = f;
    };
    /**
     * get diffuse lighting factor
     */
    Real32 getDiffuse()
    {
	return m_fDiffuse;
    };
    /**
     * set diffuse lighting factor
     */
    void setDiffuse(Real32 f)
    {
        m_fDiffuse = f;
    };
    /**
     * get shininess
     */
    Real32 getShininess()
    {
	return m_fShininess;
    };
    /**
     * set shininess
     */
    void setShininess(Real32 f)
    {
        m_fShininess = f;
    };

    /**
     * turn shading on/off
     */
    void setShading( bool bShading )
    {
	m_bShading = bShading;
    };
    /**
     * get state of shading (true if rendered)
     */
    bool getShading( )
    {
	return m_bShading;
    };

    /**
     * init the volumetric rendering - the textures are created according to the svt_volume data set
     */
    void recalcTextures()
    {
	if (!svt_checkEXT_3DT())
            return;

	m_iTexSizeX = 2; while(m_iTexSizeX < this->getVolume().getSizeX()) { m_iTexSizeX*=2; };
	m_iTexSizeY = 2; while(m_iTexSizeY < this->getVolume().getSizeY()) { m_iTexSizeY*=2; };
	m_iTexSizeZ = 2; while(m_iTexSizeZ < this->getVolume().getSizeZ()) { m_iTexSizeZ*=2; };

	// verschnitt / 2 zum mittigen ausrichten auf rechteck
	m_iBordX = (m_iTexSizeX - this->getVolume().getSizeX())/2;
	m_iBordY = (m_iTexSizeY - this->getVolume().getSizeY())/2;
	m_iBordZ = (m_iTexSizeZ - this->getVolume().getSizeZ())/2;
	// wieviel abstand zwischen den slices?
	m_fGapXY = 1.0f / m_iTexSizeZ;
	m_fGapXZ = 1.0f / m_iTexSizeY;
	m_fGapZY = 1.0f / m_iTexSizeX;

	svtout << "3D-texture size ("<< m_iTexSizeX << "," << m_iTexSizeY << "," << m_iTexSizeZ << ")" << endl;

	// create a new color_model, if the user has not supplied one
	if (m_pCTF == NULL)
	{
	    m_pCTFDefault = new svt_float2mat_linear(this->getVolume().getMinDensity(), this->getVolume().getMaxDensity());
	    m_pCTF = m_pCTFDefault;
	}

	// jetzt koennen die texturen generiert werden
	calc3DTexture();
        updateTF();
    };

    ///////////////////////////////////////////////////////////////////////////////
    // Texture Generation
    ///////////////////////////////////////////////////////////////////////////////

protected:

    /**
     * calc the 3d texture
     */
    void calc3DTexture()
    {
	m_bCalc3D = true;
    };
    /**
     * calc the 3d texture
     */
    void calc3DTexture_DOIT()
    {
	m_bCalc3D = false;

	unsigned int i, iX, iY, iZ;

	T fMax = this->getVolume().getMaxDensity();
	T fMin = this->getVolume().getMinDensity();
	T fLength = fMax - fMin;

	// memory for a single slice
	unsigned char* pSlice = new unsigned char[this->getVolume().getSizeX()*this->getVolume().getSizeY()*4];
	for(i=0; i<this->getVolume().getSizeX()*this->getVolume().getSizeY()*4; i++)
	    pSlice[i]=0;
	// memory for the 1st pass scaled volume (to size of 2^x, necessary for textures)
	unsigned char* pScaled = new unsigned char[m_iTexSizeX*m_iTexSizeY*m_iTexSizeZ*4];
	for(i=0; i<m_iTexSizeX*m_iTexSizeY*m_iTexSizeZ*4; i++)
            pScaled[i]=0;
        // memory for the 2nd pass - z direction
	unsigned char* pScaledSlice = new unsigned char[m_iTexSizeY*m_iTexSizeZ*4];
	unsigned char* pPreSlice = new unsigned char[m_iTexSizeY*m_iTexSizeZ*4];

        try
        {

        // test first if we have different voxels in the outermost layers
        unsigned int iSizeX = this->getVolume().getSizeX();
        unsigned int iSizeY = this->getVolume().getSizeY();
        unsigned int iSizeZ = this->getVolume().getSizeZ();

        // 1st pass: stretch in x/y direction
	svtout << "   Rescaling..." << endl;
        svt_exception::ui()->progressPopup("Rescaling...", 0, iSizeZ);
        for(iZ=0; iZ<iSizeZ; iZ++)
        {
            if (iZ % 5 == 0)
                svt_exception::ui()->progress( iZ, iSizeZ);

            // create single slice
            for(iX=0; iX<iSizeX; iX++)
            {
                for(iY=0; iY<iSizeY; iY++)
                {
                    // voxel value
                    pSlice[(iX*4)+(iY*this->getVolume().getSizeX()*4)+0] = (unsigned char)(255.0*((this->getVolume().getValue(iX, iY, iZ) - fMin) / fLength));
                    pSlice[(iX*4)+(iY*this->getVolume().getSizeX()*4)+1] = (unsigned char)(255.0*((this->getVolume().getValue(iX, iY, iZ) - fMin) / fLength));
                    pSlice[(iX*4)+(iY*this->getVolume().getSizeX()*4)+2] = (unsigned char)(255.0*((this->getVolume().getValue(iX, iY, iZ) - fMin) / fLength));
                    pSlice[(iX*4)+(iY*this->getVolume().getSizeX()*4)+3] = (unsigned char)(255.0*((this->getVolume().getValue(iX, iY, iZ) - fMin) / fLength));
		}
	    }

	    // now scale it
	    gluScaleImage(GL_RGBA, this->getVolume().getSizeX(), this->getVolume().getSizeY(), GL_UNSIGNED_BYTE, pSlice, m_iTexSizeX, m_iTexSizeY, GL_UNSIGNED_BYTE, pScaled+((iZ+m_iBordZ)*m_iTexSizeX*m_iTexSizeY*4));
        }
        svt_exception::ui()->progressPopdown();

	// 2nd pass: stretch in the missing z direction
        svt_exception::ui()->progressPopup("Rescaling...", 0, m_iTexSizeX);
	for(iX=0; iX<m_iTexSizeX; iX++)
	{
            if (iX % 5 == 0)
                svt_exception::ui()->progress( iX, m_iTexSizeX);

	    // create single slice
	    for(iY=0; iY<m_iTexSizeY; iY++)
		for(iZ=0; iZ<this->getVolume().getSizeZ(); iZ++)
		{
		    pPreSlice[(iZ*4)+(iY*this->getVolume().getSizeZ()*4)+0] = pScaled[(iX*4)+(iY*m_iTexSizeX*4)+((iZ+m_iBordZ)*m_iTexSizeX*m_iTexSizeY*4)+0];
		    pPreSlice[(iZ*4)+(iY*this->getVolume().getSizeZ()*4)+1] = pScaled[(iX*4)+(iY*m_iTexSizeX*4)+((iZ+m_iBordZ)*m_iTexSizeX*m_iTexSizeY*4)+1];
		    pPreSlice[(iZ*4)+(iY*this->getVolume().getSizeZ()*4)+2] = pScaled[(iX*4)+(iY*m_iTexSizeX*4)+((iZ+m_iBordZ)*m_iTexSizeX*m_iTexSizeY*4)+2];
		    pPreSlice[(iZ*4)+(iY*this->getVolume().getSizeZ()*4)+3] = pScaled[(iX*4)+(iY*m_iTexSizeX*4)+((iZ+m_iBordZ)*m_iTexSizeX*m_iTexSizeY*4)+3];
		}

	    // now scale it
	    gluScaleImage(GL_RGBA, this->getVolume().getSizeZ(), m_iTexSizeY, GL_UNSIGNED_BYTE, pPreSlice, m_iTexSizeZ, m_iTexSizeY, GL_UNSIGNED_BYTE, pScaledSlice);

	    // and copy it back
	    for(iY=0; iY<m_iTexSizeY; iY++)
		for(iZ=0; iZ<m_iTexSizeZ; iZ++)
		{
		    pScaled[(iX*4)+(iY*m_iTexSizeX*4)+(iZ*m_iTexSizeX*m_iTexSizeY*4)+0] = pScaledSlice[(iY*m_iTexSizeZ*4)+(iZ*4)+0];
		    pScaled[(iX*4)+(iY*m_iTexSizeX*4)+(iZ*m_iTexSizeX*m_iTexSizeY*4)+1] = pScaledSlice[(iY*m_iTexSizeZ*4)+(iZ*4)+1];
		    pScaled[(iX*4)+(iY*m_iTexSizeX*4)+(iZ*m_iTexSizeX*m_iTexSizeY*4)+2] = pScaledSlice[(iY*m_iTexSizeZ*4)+(iZ*4)+2];
		    pScaled[(iX*4)+(iY*m_iTexSizeX*4)+(iZ*m_iTexSizeX*m_iTexSizeY*4)+3] = pScaledSlice[(iY*m_iTexSizeZ*4)+(iZ*4)+3];
		}
	}
        svt_exception::ui()->progressPopdown();

        // check if there are varying voxels in the first layer
        bool bWarn = false;
        unsigned char fSampleValue = pScaled[3];
        for(iZ=0; iZ<m_iTexSizeZ && bWarn==false; iZ++)
            for(iX=0; iX<m_iTexSizeX && bWarn==false; iX++)
                for(iY=0; iY<m_iTexSizeY && bWarn==false; iY++)
                    if (iX==0 || iX==m_iTexSizeX-1 || iY==0 || iY==m_iTexSizeY-1 || iZ==0 || iZ==m_iTexSizeZ-1)
                        if (pScaled[(iX*4)+(iY*m_iTexSizeX*4)+(iZ*m_iTexSizeX*m_iTexSizeY*4)+3] != fSampleValue)
                            bWarn = true;

        if (bWarn)
            for(iZ=0; iZ<m_iTexSizeZ; iZ++)
                for(iX=0; iX<m_iTexSizeX; iX++)
                    for(iY=0; iY<m_iTexSizeY; iY++)
                        if (iX==0 || iX==m_iTexSizeX-1 || iY==0 || iY==m_iTexSizeY-1 || iZ==0 || iZ==m_iTexSizeZ-1)
                        {
                            pScaled[(iX*4)+(iY*m_iTexSizeX*4)+(iZ*m_iTexSizeX*m_iTexSizeY*4)+0] = fSampleValue;
                            pScaled[(iX*4)+(iY*m_iTexSizeX*4)+(iZ*m_iTexSizeX*m_iTexSizeY*4)+1] = fSampleValue;
                            pScaled[(iX*4)+(iY*m_iTexSizeX*4)+(iZ*m_iTexSizeX*m_iTexSizeY*4)+2] = fSampleValue;
                            pScaled[(iX*4)+(iY*m_iTexSizeX*4)+(iZ*m_iTexSizeX*m_iTexSizeY*4)+3] = fSampleValue;
                        }

	// 2.5th pass: normalization
	svtout << "   Volume Normalization..." << endl;
	unsigned char cMax = 0;
	unsigned char cMin = 255;
	for(iZ=0; iZ<m_iTexSizeZ; iZ++)
	{
	    for(iX=0; iX<m_iTexSizeX; iX++)
	    {
		for(iY=0; iY<m_iTexSizeY; iY++)
		{
		    if (pScaled[(iX*4)+(iY*m_iTexSizeX*4)+(iZ*m_iTexSizeX*m_iTexSizeY*4)+3] > cMax)
			cMax = pScaled[(iX*4)+(iY*m_iTexSizeX*4)+(iZ*m_iTexSizeX*m_iTexSizeY*4)+3];
		    if (pScaled[(iX*4)+(iY*m_iTexSizeX*4)+(iZ*m_iTexSizeX*m_iTexSizeY*4)+3] < cMin)
			cMin = pScaled[(iX*4)+(iY*m_iTexSizeX*4)+(iZ*m_iTexSizeX*m_iTexSizeY*4)+3];
		}
	    }
	}

	float fVal;
	float fMaxV = (float)cMax;
	float fMinV = (float)cMin;
        float fLenV = fMaxV - fMinV;

	if (cMin != 0 || cMax != 255)
	{
	    svtout << "      Performing normalization" << endl;
            svt_exception::ui()->progressPopup("Performing normalization...", 0, m_iTexSizeZ);

	    for(iZ=0; iZ<m_iTexSizeZ; iZ++)
            {
                if (iZ % 5 == 0)
                    svt_exception::ui()->progress( iZ, m_iTexSizeZ);

		for(iX=0; iX<m_iTexSizeX; iX++)
		{
		    for(iY=0; iY<m_iTexSizeY; iY++)
		    {
			fVal = (float)pScaled[(iX*4)+(iY*m_iTexSizeX*4)+(iZ*m_iTexSizeX*m_iTexSizeY*4)+3];
			fVal -= fMinV;
			fVal /= fLenV;
			pScaled[(iX*4)+(iY*m_iTexSizeX*4)+(iZ*m_iTexSizeX*m_iTexSizeY*4)+3] = (char)((fVal*255.0f) + 0.5f);
		    }
		}
            }

            svt_exception::ui()->progressPopdown();
	}

	// 3rd pass: compute gradient
	svt_vector4<Real32> oVec;
	svtout << "   Gradient calculation..." << endl;
	int iKX, iKY, iKZ;
	// sobel kernels (fastest running index = z, then y and then x!!)
	Real64 xKernel[27] =
	{
	    -1, -3, -1,
	    -3, -6, -3,
	    -1, -3, -1,
	    0, 0, 0,
	    0, 0, 0,
	    0, 0, 0,
	    1, 3, 1,
	    3, 6, 3,
	    1, 3, 1
	};
	Real64 yKernel[27] =
	{
	    1, 3, 1,
	    0, 0, 0,
	    -1, -3, -1,
	    3, 6, 3,
	    0, 0, 0,
	    -3, -6, -3,
	    1, 3, 1,
	    0, 0, 0,
	    -1, -3, -1
	};
	Real64 zKernel[27] =
	{
	    -1, 0, 1,
	    -3, 0, 3,
	    -1, 0, 1,
	    -3, 0, 3,
	    -6, 0, 6,
	    -3, 0, 3,
	    -1, 0, 1,
	    -3, 0, 3,
	    -1, 0, 1
	};

        float fVoxel;

        svt_exception::ui()->progressPopup("Gradient calculation...", 0, m_iTexSizeZ);
	for(iZ=0; iZ<m_iTexSizeZ; iZ++)
        {
            if (iZ % 5 == 0)
                svt_exception::ui()->progress( iZ, m_iTexSizeZ);

	    for(iX=0; iX<m_iTexSizeX; iX++)
	    {
		for(iY=0; iY<m_iTexSizeY; iY++)
		{
		    // apply x kernel
		    Real64 xVal = 0.0;
		    for(iKZ = -1; iKZ < 2; iKZ++)
			for(iKY = -1; iKY < 2; iKY++)
			    for(iKX = -1; iKX < 2; iKX++)
                            {
                                if ((((int)(iX)+iKX) < (int)(m_iTexSizeX) && ((int)(iX)+iKX) >= 0) && (((int)(iY)+iKY) < (int)(m_iTexSizeY) && ((int)(iY)+iKY) >= 0) && (((int)(iZ)+iKZ) < (int)(m_iTexSizeZ) && ((int)(iZ)+iKZ) >= 0))
                                    xVal += xKernel[((iKX+1)*9)+((iKY+1)*3)+iKZ+1] * (Real32(pScaled[(((int)(iX)+iKX)*4)+(((int)(iY)+iKY)*m_iTexSizeX*4)+(((int)(iZ)+iKZ)*m_iTexSizeX*m_iTexSizeY*4)+3]));
			    }

		    // apply y kernel
		    Real64 yVal = 0.0;
		    for(iKZ = -1; iKZ < 2; iKZ++)
			for(iKY = -1; iKY < 2; iKY++)
			    for(iKX = -1; iKX < 2; iKX++)
			    {
                                if ((((int)(iX)+iKX) < (int)(m_iTexSizeX) && ((int)(iX)+iKX) >= 0) && (((int)(iY)+iKY) < (int)(m_iTexSizeY) && ((int)(iY)+iKY) >= 0) && (((int)(iZ)+iKZ) < (int)(m_iTexSizeZ) && ((int)(iZ)+iKZ) >= 0))
                                    yVal += yKernel[((iKX+1)*9)+((iKY+1)*3)+iKZ+1] * (Real32(pScaled[(((int)(iX)+iKX)*4)+(((int)(iY)+iKY)*m_iTexSizeX*4)+(((int)(iZ)+iKZ)*m_iTexSizeX*m_iTexSizeY*4)+3]));
			    }

		    // apply z kernel
		    Real64 zVal = 0.0;
		    for(iKX = -1; iKX < 2; iKX++)
			for(iKY = -1; iKY < 2; iKY++)
			    for(iKZ = -1; iKZ < 2; iKZ++)
                            {
                                if ((((int)(iX)+iKX) < (int)(m_iTexSizeX) && ((int)(iX)+iKX) >= 0) && (((int)(iY)+iKY) < (int)(m_iTexSizeY) && ((int)(iY)+iKY) >= 0) && (((int)(iZ)+iKZ) < (int)(m_iTexSizeZ) && ((int)(iZ)+iKZ) >= 0))
                                    zVal += zKernel[((iKX+1)*9)+((iKY+1)*3)+iKZ+1] * (Real32(pScaled[(((int)(iX)+iKX)*4)+(((int)(iY)+iKY)*m_iTexSizeX*4)+(((int)(iZ)+iKZ)*m_iTexSizeX*m_iTexSizeY*4)+3]));
			    }

		    oVec.x( xVal );
		    oVec.y( -yVal );
		    oVec.z( zVal );
		    oVec.w( 0.0 );

                    fVoxel = (((float)(pScaled[(iX*4)+(iY*m_iTexSizeX*4)+(iZ*m_iTexSizeX*m_iTexSizeY*4)+3])/255.0f) * fLength) + fMin;
                    if (fVoxel < 0.0)
                    {
                        oVec.x( -xVal );
                        oVec.y( yVal );
                        oVec.z( -zVal );
                        oVec.w( 0.0 );
                    }

		    oVec.normalize();

		    if (oVec.x() > 1.0)
                        oVec.x( 1.0 );
		    if (oVec.y() > 1.0)
                        oVec.y( 1.0 );
		    if (oVec.z() > 1.0)
                        oVec.z( 1.0 );

		    if (oVec.x() < -1.0)
                        oVec.x( -1.0 );
		    if (oVec.y() < -1.0)
                        oVec.y( -1.0 );
		    if (oVec.z() < -1.0)
                        oVec.z( -1.0 );

		    oVec.x( oVec.x() + 1.0 );
		    oVec.y( oVec.y() + 1.0 );
		    oVec.z( oVec.z() + 1.0 );

		    pScaled[(iX*4)+(iY*m_iTexSizeX*4)+(iZ*m_iTexSizeX*m_iTexSizeY*4)+0] = (unsigned char)((127.5 * oVec.x()) + 0.5);
		    pScaled[(iX*4)+(iY*m_iTexSizeX*4)+(iZ*m_iTexSizeX*m_iTexSizeY*4)+1] = (unsigned char)((127.5 * oVec.y()) + 0.5);
		    pScaled[(iX*4)+(iY*m_iTexSizeX*4)+(iZ*m_iTexSizeX*m_iTexSizeY*4)+2] = (unsigned char)((127.5 * oVec.z()) + 0.5);
		}
	    }
        }
        svt_exception::ui()->progressPopdown();

        } catch (svt_userInterrupt& ) {

        }

        m_fZero = (Real32)(pScaled[3]) / 256.0f;

	// create new 3d texture object and register the data
	svt_texture_3d* pTexture = this->getTexture3D();
	if (pTexture == NULL)
	    pTexture = new svt_texture_3d();

	pTexture->setDataRGBA(pScaled, m_iTexSizeX,m_iTexSizeY,m_iTexSizeZ);
	this->setTexture3D(pTexture);

	// create new 2d texture object with transfer function data
	updateTF();

        delete[] pSlice;
	delete[] pScaledSlice;
        delete[] pPreSlice;
    };

    ///////////////////////////////////////////////////////////////////////////////
    // Misc
    ///////////////////////////////////////////////////////////////////////////////

public:

    /**
     * Set the sampling rate (number of slices through the volume)
     */
    inline void setSampling(Real64 fSampling)
    {
	m_fSampling = fSampling;
    };
    /**
     * Get the sampling rate (number of slices through the volume)
     */
    inline Real64 getSampling() const
    {
	return m_fSampling;
    };

    /**
     * Set the start slice
     * \param fStartSlice value between 0.0 and 1.0 - if set to 0.5 skips the first 50%
     */
    inline void setStartSlice( Real32 fStartSlice )
    {
        m_fStartSlice = fStartSlice;
    };
    /**
     * Set the end slice
     * \param fEndSlice value between 0.0 and 1.0 - if set to 0.5 skips the first 50%
     */
    inline void setEndSlice( Real32 fEndSlice )
    {
        m_fEndSlice = fEndSlice;
    };

    /**
     * Get the start slice
     * \return value between 0.0 and 1.0 - if set to 0.5 skips the first 50%
     */
    inline Real32 getStartSlice()
    {
        return m_fStartSlice;
    };
    /**
     * Get the end slice
     * \return value between 0.0 and 1.0 - if set to 0.5 skips the first 50%
     */
    inline Real32 getEndSlice()
    {
        return m_fEndSlice;
    };

    ///////////////////////////////////////////////////////////////////////////////
    // Transfer functions
    ///////////////////////////////////////////////////////////////////////////////

public:

    /**
     * Get the current transfer function
     */
    inline svt_float2mat* getFloat2Mat() const
    {
	return m_pCTF;
    };
    /**
     * Set the current transfer function
     */
    inline void setFloat2Mat(svt_float2mat* pCTF)
    {
	m_pCTF = pCTF;
    };

    void updateTF()
    {
	m_bUpdateTF = true;
    };
    void updateTF_DOIT()
    {
	if (this->getTexture3D() == NULL)
	    return;

	int iX, iY;

	T fMax = this->getVolume().getMaxDensity();
	T fMin = this->getVolume().getMinDensity();
	T fLength = fMax - fMin;
	float fVoxel;

	float fOldR = 0.0;
	float fOldG = 0.0;
	float fOldB = 0.0;
	float fOldA = 0.0;
        float fA = 0.0;

        // Precompute the alpha and C~ integrals.
        // All color and opacity values are scaled to 0.0-1.0. In this case the maximal value of an integral can be 255*1.0 = 255. Ktau can - as it is multiplied by alpha - reach 256*
	for(iX=0; iX<256; iX++)
	{
            fVoxel = (((float)(iX)/255.0f) * fLength) + fMin;

            fA = (float)(m_pCTF->getOpacity( fVoxel )) / 255.0f;

	    m_aT[iX]     = (( (float)(m_pCTF->getOpacity( fVoxel ) ) / 255.0f)      ) + fOldA;
	    m_aKtauR[iX] = (( (float)(m_pCTF->getR(       fVoxel ) ) / 255.0f) * fA ) + fOldR;
	    m_aKtauG[iX] = (( (float)(m_pCTF->getG(       fVoxel ) ) / 255.0f) * fA ) + fOldG;
	    m_aKtauB[iX] = (( (float)(m_pCTF->getB(       fVoxel ) ) / 255.0f) * fA ) + fOldB;

	    fOldR = m_aKtauR[iX];
	    fOldG = m_aKtauG[iX];
	    fOldB = m_aKtauB[iX];
	    fOldA = m_aT[iX];
	}

	// iX is back, iY is front!
	float fFact = 1.0f;
        float fR, fG, fB;

	for(iX=0; iX<256; iX++)
	    for(iY=0; iY<256; iY++)
	    {
		if (iX != iY)
                {
                    fFact = (1.0f / ((float)(iX) - (float)(iY)));
                    fR = fFact * ( m_aKtauR[iX] - m_aKtauR[iY] );
                    fG = fFact * ( m_aKtauG[iX] - m_aKtauG[iY] );
                    fB = fFact * ( m_aKtauB[iX] - m_aKtauB[iY] );

                    fA = fFact * ( m_aT[iX] - m_aT[iY] );

		} else {

		    if (iX > 128)
		    {
			fR = (m_aKtauR[iX] - m_aKtauR[iX-1]);
			fG = (m_aKtauG[iX] - m_aKtauG[iX-1]);
			fB = (m_aKtauB[iX] - m_aKtauB[iX-1]);
			fA = (m_aT[iX]     - m_aT[iX-1]);
		    } else {
			fR = (m_aKtauR[iX+1] - m_aKtauR[iX]);
			fG = (m_aKtauG[iX+1] - m_aKtauG[iX]);
			fB = (m_aKtauB[iX+1] - m_aKtauB[iX]);
			fA = (m_aT[iX+1]     - m_aT[iX]);
		    }

		}

		//fA = 1.0f - exp( -fA );

		if (fR > 1.0f)
		    fR = 1.0f;
		if (fR < 0.0f)
		    fR = 0.0f;

		if (fG > 1.0f)
		    fG = 1.0f;
		if (fG < 0.0f)
		    fG = 0.0f;

		if (fB > 1.0f)
		    fB = 1.0f;
		if (fB < 0.0f)
		    fB = 0.0f;

		if (fA > 1.0f)
		    fA = 1.0f;
		if (fA < 0.0f)
		    fA = 0.0f;

		m_aTrans[(iX*4)+(iY*256*4)+0] = (unsigned char)(fR * 255.0f);
		m_aTrans[(iX*4)+(iY*256*4)+1] = (unsigned char)(fG * 255.0f);
		m_aTrans[(iX*4)+(iY*256*4)+2] = (unsigned char)(fB * 255.0f);
		m_aTrans[(iX*4)+(iY*256*4)+3] = (unsigned char)(fA * 255.0f);
	    }

	// create the texture
	if (m_bTexHandle == false)
	{
	    glGenTextures(1, (GLuint*)&m_iTexHandle);
	    m_bTexHandle = true;
	}

	glBindTexture(GL_TEXTURE_2D, m_iTexHandle);
	glTexImage2D((ENUM_GLENUM)GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, (ENUM_GLENUM)GL_RGBA, (ENUM_GLENUM)GL_UNSIGNED_BYTE, m_aTrans);

	m_bUpdateTF = false;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Clipping
    ///////////////////////////////////////////////////////////////////////////////

    enum
    {
        polygonInterior = 1,
        polygonBoundary = 0,
        polygonExterior = -1
    };

    /**
     * Simple polygon clipping routine
     */
    vector<svt_vector4<Real64> > ClipPolygonAgainstPlane( const vector<svt_vector4<Real64> > rVertices, const svt_vector4<Real64> rPlane )
    {
        long iPositive = 0;
        long iNegative = 0;
        const Real64 fBoundaryEpsilon = 1.0e-6F;

        unsigned int iVertexCount = rVertices.size();
        vector<signed char> aLocation;
        vector<svt_vector4<Real64> > aResult;

        //
        // First step: Check for every vertex on which side of the clipplane it lies
        //
        for (unsigned int iA = 0; iA < rVertices.size(); iA++)
        {
            // see Lengyel, d = N * Q + D
            Real64 fD = (rPlane.x()*rVertices[iA].x()) + (rPlane.y()*rVertices[iA].y()) + (rPlane.z()*rVertices[iA].z()) + rPlane.w();
            if (fD > fBoundaryEpsilon)
            {
                aLocation.push_back( polygonInterior );
                iPositive++;
            }
            else
            {
                if (fD < -fBoundaryEpsilon)
                {
                    aLocation.push_back( polygonExterior );
                    iNegative++;
                }
                else
                {
                    aLocation.push_back( polygonBoundary );
                }
            }
        }

        //
        // Second step: Decide if simple case or not. If all points are on either side of the clipplane, we are essentially done...
        //
        if (iNegative == 0)
        {
            for (long iA = 0; iA < iVertexCount; iA++)
                aResult.push_back( rVertices[iA] );
            return aResult;
        }
        else if (iPositive == 0)
        {
            return aResult;
        }

        //
        // Third step: Real clipping.
        //
        long iPrevious = iVertexCount - 1;
        for (long iIndex = 0; iIndex < iVertexCount; iIndex++)
        {
            long iLoc = aLocation[iIndex];
            if (iLoc == polygonExterior)
            {
                if (aLocation[iPrevious] == polygonInterior)
                {
                    const svt_vector4<Real64> rV1 = rVertices[iPrevious];
                    const svt_vector4<Real64> rV2 = rVertices[iIndex];
                    svt_vector4<Real64> dV = rV2 - rV1;

                    Real64 fT = ( (rPlane.x()*rV2.x()) + (rPlane.y()*rV2.y()) + (rPlane.z()*rV2.z()) + rPlane.w()) / ( (rPlane.x()*dV.x()) + (rPlane.y()*dV.y()) + (rPlane.z()*dV.z()) );
                    aResult.push_back( rV2 - (dV * fT) );
                }
            }
            else
            {
                const svt_vector4<Real64> rV1 = rVertices[iIndex];
                if ((iLoc == polygonInterior) && (aLocation[iPrevious] == polygonExterior))
                {
                    const svt_vector4<Real64> rV2 = rVertices[iPrevious];
                    svt_vector4<Real64> dV = rV2 - rV1;

                    Real64 fT = ( (rPlane.x()*rV2.x()) + (rPlane.y()*rV2.y()) + (rPlane.z()*rV2.z()) + rPlane.w()) / ( (rPlane.x()*dV.x()) + (rPlane.y()*dV.y()) + (rPlane.z()*dV.z()) );
                    aResult.push_back( rV2 - (dV * fT) );
                }

                aResult.push_back(rV1);
            }

            iPrevious = iIndex;
        }

        return aResult;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Drawing
    ///////////////////////////////////////////////////////////////////////////////

public:

    /**
     * drawGL
     */
    virtual void drawGL()
    {
	if (!svt_checkEXT_3DT() || svt_getAllowShaderPrograms() == false)
            return;

	if (m_bCalc3D == true)
	    calc3DTexture_DOIT();
	if (m_bUpdateTF == true)
	    updateTF_DOIT();

	if (this->getTexture3D())
	{
	    this->getTexture3D()->bindTexture();

	    svt_checkGLErrors("svt_volume_3dtextures::drawGL>");

            // what is the largest dimension?
            float fMaxDim = svt_max( this->getVolume().getSizeX(), this->getVolume().getSizeY() );
	    fMaxDim = svt_max( fMaxDim, this->getVolume().getSizeZ() );

            // init shaders
	    m_pShader->setVolumeID( this->getTexture3D()->getTextureID() );
	    m_pShader->setTransferID( m_iTexHandle );
	    m_pShaderNospec->setVolumeID( this->getTexture3D()->getTextureID() );
	    m_pShaderNospec->setTransferID( m_iTexHandle );
	    m_pShaderFast->setVolumeID( this->getTexture3D()->getTextureID() );
	    m_pShaderFast->setTransferID( m_iTexHandle );

            // init opengl
	    glBindTexture(GL_TEXTURE_2D, m_iTexHandle);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

	    glDisable(GL_LIGHTING);
	    glEnable(GL_TEXTURE_3D);
	    glEnable(GL_TEXTURE_2D);

	    // filtering
	    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	    // to avoid boundary problems
	    //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	    //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);

            GLfloat aBorder[4];
            aBorder[0] = 0.0f;
            aBorder[1] = 0.0f;
            aBorder[2] = 0.0f;
            aBorder[3] = m_fZero;
            glTexParameterfv( GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, aBorder );

	    // set current mvp matrix in shader
	    svt_matrix4<Real32> oShaderMVP;
	    oShaderMVP.loadModelviewMatrix();

	    // calculate the current rotation, but remove translation
	    svt_matrix4<Real32> oTexTrans;
	    oTexTrans.loadModelviewMatrix();
	    Real32 fScaleX = sqrt(oTexTrans[0][0]*oTexTrans[0][0] + oTexTrans[1][0]*oTexTrans[1][0] + oTexTrans[2][0]*oTexTrans[2][0]);
	    //Real32 fScaleY = sqrt(oTexTrans[0][1]*oTexTrans[0][1] + oTexTrans[1][1]*oTexTrans[1][1] + oTexTrans[2][1]*oTexTrans[2][1]);
            //Real32 fScaleZ = sqrt(oTexTrans[0][2]*oTexTrans[0][2] + oTexTrans[1][2]*oTexTrans[1][2] + oTexTrans[2][2]*oTexTrans[2][2]);
            Real32 fPosX, fPosY, fPosZ;
	    fPosX = oTexTrans.translationX();
	    fPosY = oTexTrans.translationY();
	    fPosZ = oTexTrans.translationZ();
	    oTexTrans.setTranslationX(0.0f);
	    oTexTrans.setTranslationY(0.0f);
            oTexTrans.setTranslationZ(0.0f);
            oTexTrans.invert();

	    // adjust texture matrix
	    glMatrixMode(GL_TEXTURE);
	    glPushMatrix();
	    glLoadIdentity();
            glTranslatef( 0.50f, 0.50f, 0.50f );
            glScalef( fMaxDim / (Real64)(this->getVolume().getSizeX()), fMaxDim / (Real64)(this->getVolume().getSizeY()), fMaxDim / (Real64)(this->getVolume().getSizeZ()) );
	    oTexTrans.applyToGL();

	    // save modelview matrix
	    glMatrixMode(GL_MODELVIEW);
	    glPushMatrix();
	    // slices have to be parallel to projection plane
	    glLoadIdentity();
	    // only add translation
            glTranslatef( fPosX, fPosY, fPosZ );

            // bind 2D dependent texture
	    glBindTexture(GL_TEXTURE_2D, m_iTexHandle);

	    // lightsource
	    svt_vector4<Real32> oLightPosition(0.0,0.0,-1.0,0.0);
            oShaderMVP.invert();
	    oLightPosition = oShaderMVP * oLightPosition;
            oLightPosition.normalize();
	    m_pShader->setLightPosition( svt_vector3<Real32>(oLightPosition.x(), oLightPosition.y(), oLightPosition.z()) );
	    m_pShaderNospec->setLightPosition( svt_vector3<Real32>(oLightPosition.x(), oLightPosition.y(), oLightPosition.z()) );

	    // now draw all the slices
            float fLimit = svt_max( m_iTexSizeX, m_iTexSizeY);
	    fLimit = svt_max( fLimit, m_iTexSizeZ );
            fLimit *= m_fSampling;

            float fStep = 0.71f * fScaleX;

	    float fR = -fStep;
	    float fDR = (2.0f * fStep) / fLimit;

            float fC = fStep * (this->getVolume().getWidth() * fMaxDim * 1.0e-2);
	    float fZ = -fC;
	    float fDZ = (2.0*fC) / fLimit;

	    // bind shader
            if ( m_bShading )
	    {
		if (m_fShininess > 0.0)
		{
		    m_pShader->setSliceDistance( fDR );
		    m_pShader->setModelViewMatrix( oShaderMVP );
		    m_pShader->setLightingParams( m_fDiffuse, m_fAmbient, m_fShininess );
		    m_pShader->bind();

		} else {

		    m_pShaderNospec->setSliceDistance( fDR );
		    m_pShaderNospec->setModelViewMatrix( oShaderMVP );
		    m_pShaderNospec->setLightingParams( m_fDiffuse, m_fAmbient );
		    m_pShaderNospec->bind();
		}

	    } else {

		// bind fast shader
		m_pShaderFast->setSliceDistance( fDR );
		m_pShaderFast->setModelViewMatrix( oShaderMVP );
		m_pShaderFast->bind();
	    }

	    // switch on blending
	    glEnable(GL_BLEND);
	    glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);

            // which slices need to be rendered?
            int iStart = (int)(fLimit * m_fStartSlice);
            int iEnd = (int)(fLimit * m_fEndSlice);

            // get the number of supported clipplanes
            GLint iMaxClipPlanes;
            glGetIntegerv(GL_MAX_CLIP_PLANES, &iMaxClipPlanes);

            // OK, now do it...
	    for (int i=0; i<(int)(fLimit); i++)
            {
                if (i >= iStart && i<=iEnd)
                {
                    svt_vector4<Real32> oVecA( -0.25, -0.25, fR );
                    oVecA = oTexTrans * oVecA;
                    svt_vector4<Real32> oVecB( +0.25, -0.25, fR );
                    oVecB = oTexTrans * oVecB;
                    svt_vector4<Real32> oVecC( -0.25, +0.25, fR );
                    oVecC = oTexTrans * oVecC;
                    svt_vector4<Real32> oVecD( +0.25, +0.25, fR );
                    oVecD = oTexTrans * oVecD;
                    svt_vector4<Real32> oVecE( +0.00, +0.00, fR );
                    oVecE = oTexTrans * oVecE;

                    if (
                        ( oVecA.x() < 1.1f && oVecA.x() > -1.1f && oVecA.y() < 1.1f && oVecA.y() > -1.1f && oVecA.z() < 1.1f && oVecA.z() > -1.1f ) ||
                        ( oVecB.x() < 1.1f && oVecB.x() > -1.1f && oVecB.y() < 1.1f && oVecB.y() > -1.1f && oVecB.z() < 1.1f && oVecB.z() > -1.1f ) ||
                        ( oVecC.x() < 1.1f && oVecC.x() > -1.1f && oVecC.y() < 1.1f && oVecC.y() > -1.1f && oVecC.z() < 1.1f && oVecC.z() > -1.1f ) ||
                        ( oVecD.x() < 1.1f && oVecD.x() > -1.1f && oVecD.y() < 1.1f && oVecD.y() > -1.1f && oVecD.z() < 1.1f && oVecD.z() > -1.1f ) ||
                        ( oVecE.x() < 1.1f && oVecE.x() > -1.1f && oVecE.y() < 1.1f && oVecE.y() > -1.1f && oVecE.z() < 1.1f && oVecE.z() > -1.1f )
                       )
                    {
                        //
                        // Create the geometry for the proxy-planes that sample the 3D texture
                        //
                        vector< svt_vector4<Real64> > aVertices;
                        aVertices.push_back( svt_vector4<Real64>(-fStep, -fStep, fR) );
                        aVertices.push_back( svt_vector4<Real64>(-fStep, +fStep, fR) );
                        aVertices.push_back( svt_vector4<Real64>(+fStep, +fStep, fR) );
                        aVertices.push_back( svt_vector4<Real64>(+fStep, -fStep, fR) );

                        //
                        // Apply modelview matrix, as the clipping-planes come from ogl and have the matrix already applied...
                        //
                        vector< bool > aClipPlanes;
                        svt_matrix4<Real64> oClipMV;
                        oClipMV.loadModelviewMatrix();
                        aVertices[0] = oClipMV * aVertices[0];
                        aVertices[1] = oClipMV * aVertices[1];
                        aVertices[2] = oClipMV * aVertices[2];
                        aVertices[3] = oClipMV * aVertices[3];

                        //
                        // And now clip those against the currently active ogl clipping planes
                        //
                        for(int k=0; k<iMaxClipPlanes; k++)
                        {
                            svt_vector4<Real64> oPlane;
                            GLdouble oPlaneEq[4];
                            if (glIsEnabled( GL_CLIP_PLANE0+k ))
                            {
                                aClipPlanes.push_back( true );
                                glGetClipPlane( GL_CLIP_PLANE0+k, oPlaneEq );

                                oPlane.x( oPlaneEq[0] );
                                oPlane.y( oPlaneEq[1] );
                                oPlane.z( oPlaneEq[2] );
                                oPlane.w( oPlaneEq[3] );
                                oPlane.normalize();

                                aVertices = ClipPolygonAgainstPlane( aVertices, oPlane );

                            } else
                                aClipPlanes.push_back( false );
                        }

                        //
                        // OK, now get back to object space
                        //
                        oClipMV.invert();
                        for(unsigned int k=0; k<aVertices.size(); k++)
                            aVertices[k] = oClipMV * aVertices[k];

                        //
                        // And disable the clipplanes
                        //
                        for(int k=0; k<iMaxClipPlanes; k++)
                            if (aClipPlanes[k])
                                glDisable( GL_CLIP_PLANE0+k );

                        //
                        // Now draw the final, clipped geometry
                        //
                        float fScaling = (this->getVolume().getWidth() * fMaxDim * 1.0e-2);
                        glBegin(GL_POLYGON);
                        for(unsigned int k=0;k<aVertices.size(); k++)
                        {
                            glTexCoord3f( aVertices[k].x(),            aVertices[k].y(),            aVertices[k].z() );
                            glVertex3f(   aVertices[k].x() * fScaling, aVertices[k].y() * fScaling, aVertices[k].z() * fScaling );
                        }
                        glEnd();

                        //
                        // Re-enable the clipplanes again
                        //
                        for(int k=0; k<iMaxClipPlanes; k++)
                            if (aClipPlanes[k])
                                glEnable( GL_CLIP_PLANE0+k );
                    }
                }

                fR += fDR;
                fZ += fDZ;
            }

            if (!svt_renderCoarse())
            {
                m_pShader->printInfoLog();
                m_pShader->unbind();
            } else {
                m_pShaderFast->printInfoLog();
                m_pShaderFast->unbind();
	    }

	    // restore modelview matrix
	    glPopMatrix();

	    // restore the texture matrix
	    glMatrixMode(GL_TEXTURE);
	    glPopMatrix();
	    glMatrixMode(GL_MODELVIEW);

	    // restore all mode changes
	    glDisable(GL_BLEND);
	    glDisable(GL_TEXTURE_3D);
	    glDisable(GL_TEXTURE_2D);
	    glDisable(GL_DEPTH_TEST);
	    glEnable(GL_LIGHTING);
	}

        if (!svt_renderCoarse())
	    //m_iLastFull = iTime;
            m_iLastFull = 0;
    };
};

#endif
