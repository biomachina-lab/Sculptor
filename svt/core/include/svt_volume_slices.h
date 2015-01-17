/***************************************************************************
                          svt_volume_slices
			  -----------------
    begin                : Oct 23 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_node_noautodl.h>
#include <svt_volumeRenderer.h>
#include <svt_texture.h>
#include <svt_box.h>
#include <svt_const.h>
#include <svt_float2color.h>
#include <svt_float2color_bw.h>
#include <svt_float2opacity.h>
#include <svt_float2opacity_linear.h>
#include <svt_float2opacity_constant.h>
#include <svt_window.h>
#include <svt_init.h>
#include <svt_opengl.h>

#ifndef SVT_VOLUME_SLICES
#define SVT_VOLUME_SLICES

/**
 * Renders the cross sections of volumetric data sets
 *@author Stefan Birmanns
 */
template<class T> class DLLEXPORT svt_volume_slices : public svt_volumeRenderer<T>
{
protected:

    // position of the slices inside the volume data
    unsigned int m_iCurrSliceXY;
    unsigned int m_iCurrSliceXZ;
    unsigned int m_iCurrSliceZY;
    // volume mode
    bool m_bVolumeMode;
    // textures
    svt_texture** m_pTexXY;
    svt_texture** m_pTexXZ;
    svt_texture** m_pTexZY;
    // size of texture (power of two, i.e. not equal to size of volume!)
    unsigned int m_iTexSizeX;
    unsigned int m_iTexSizeY;
    unsigned int m_iTexSizeZ;
    // cutoff
    T m_fCutoff;
    // distance between the slices
    float m_fFactXY;
    float m_fFactXZ;
    float m_fFactZY;
    // transfer functions
    svt_float2color* m_pCTF;
    svt_float2color* m_pCTFDefault;
    svt_float2opacity* m_pOTF;
    svt_float2opacity* m_pOTFDefault;
    // next time rendering texture generation?
    bool m_bGenerateTextures;
    // box
    bool m_bBox;
    // draw XY, XZ, ZY slice, enable (1) / disable (0)
    bool m_bDrawXY;
    bool m_bDrawXZ;
    bool m_bDrawZY;

public:

    /**
     * Constructor
     */
    svt_volume_slices(svt_volume<T>& rVolume) : svt_volumeRenderer<T>(rVolume),
	m_iCurrSliceXY(1),
	m_iCurrSliceXZ(1),
	m_iCurrSliceZY(1),
	m_bVolumeMode(false),
	m_pTexXY(NULL),
	m_pTexXZ(NULL),
	m_pTexZY(NULL),
	m_iTexSizeX(1),
	m_iTexSizeY(1),
	m_iTexSizeZ(1),
	m_fCutoff(0.0),
	m_fFactXY(0.0),
	m_fFactXZ(0.0),
	m_fFactZY(0.0),
	m_pCTF(NULL),
	m_pCTFDefault(NULL),
	m_pOTF(NULL),
	m_pOTFDefault(NULL),
	m_bGenerateTextures(false),
	m_bBox(true),
	m_bDrawXY(true),
	m_bDrawXZ(true),
	m_bDrawZY(true)
    {
        this->setAutoDL( false );
        this->setProperties(new svt_properties(new svt_color(0.0f,1.0f,0.0f)));
        this->getProperties()->getColor()->setTransparency( 0.5f );
    }
    virtual ~svt_volume_slices()
    {
	if (m_pCTFDefault != NULL)
	    delete m_pCTFDefault;
	if (m_pOTFDefault != NULL)
	    delete m_pOTFDefault;
    };

public:

    /**
     * recalc the textures
     */
    void recalcTextures()
    {
	initVolume();
    };

protected:

    /**
     * init the volume volume
     */
    void initVolume()
    {
	m_iTexSizeX = 2; while(m_iTexSizeX < this->getVolume().getSizeX()) { m_iTexSizeX*=2; };
	m_iTexSizeY = 2; while(m_iTexSizeY < this->getVolume().getSizeY()) { m_iTexSizeY*=2; };
	m_iTexSizeZ = 2; while(m_iTexSizeZ < this->getVolume().getSizeZ()) { m_iTexSizeZ*=2; };
	// distances between the slices
	m_fFactXY = 1.0f / (float)(this->getVolume().getSizeZ());
	m_fFactXZ = 1.0f / (float)(this->getVolume().getSizeY());
	m_fFactZY = 1.0f / (float)(this->getVolume().getSizeX());

	// create a new color_model, if the user has not supplied one
	if (m_pCTF == NULL)
	{
	    m_pCTFDefault = new svt_float2color_bw(this->getVolume().getMinDensity(), this->getVolume().getMaxDensity());
	    m_pCTF = m_pCTFDefault;
	}
	if (m_pOTF == NULL)
	{
	    m_pOTFDefault = new svt_float2opacity_linear(this->getVolume().getMinDensity(), this->getVolume().getMaxDensity());
	    m_pOTF = m_pOTFDefault;
	}

	// jetzt koennen die texturen generiert werden
	generateTextures();
    };

    ///////////////////////////////////////////////////////////////////////////////
    // Texture Generation
    ///////////////////////////////////////////////////////////////////////////////

public:
    /**
     * generate all textures for all slices
     */
    void generateTextures()
    {
	m_bGenerateTextures = true;
    };

    /**
     * generate all textures for all slices (this is called within the drawGL routine, as some parts of the code need an opengl context!)
     */
    void generateTexturesGL()
    {
	unsigned int k;

	// save the old pointers
	svt_texture** pTexXY = new svt_texture*[this->getVolume().getSizeZ()];
	svt_texture** pTexXZ = new svt_texture*[this->getVolume().getSizeY()];
	svt_texture** pTexZY = new svt_texture*[this->getVolume().getSizeX()];
	// generate the textures
	for (k=0;k<this->getVolume().getSizeZ();k++)
	    pTexXY[k] = generateTexturesXY(k);
	for (k=0;k<this->getVolume().getSizeY();k++)
	    pTexXZ[k] = generateTexturesXZ(k);
	for (k=0;k<this->getVolume().getSizeX();k++)
	    pTexZY[k] = generateTexturesZY(k);

	// delete old textures
	for (k=0;k<this->getVolume().getSizeZ();k++)
	{
		if (m_pTexXY && m_pTexXY[k])
		    m_pTexXY[k]->destroyCD();
	    }
	    delete[] m_pTexXY;
	    for (k=0;k<this->getVolume().getSizeY();k++)
	    {
		if (m_pTexXZ && m_pTexXZ[k])
		    m_pTexXZ[k]->destroyCD();
	    }
	    delete[] m_pTexXZ;
	    for (k=0;k<this->getVolume().getSizeX();k++)
	    {
                if (m_pTexZY && m_pTexZY[k])
		    m_pTexZY[k]->destroyCD();
	    }
	    delete[] m_pTexZY;

            // activate the new ones
	    m_pTexXY = pTexXY;
	    m_pTexXZ = pTexXZ;
	    m_pTexZY = pTexZY;

	    // and rebuild the display list
            this->rebuildDL();

            m_bGenerateTextures = false;
	};

        /**
         * generate the textures for the xy slice
         * \param iK index of the slice
         */
        svt_texture* generateTexturesXY(int iK)
        {
            char* pData = new char[this->getVolume().getSizeX()*this->getVolume().getSizeY()*4];

            unsigned int iX, iY;
            for(iX=0; iX<this->getVolume().getSizeX(); iX++)
                for(iY=0; iY<this->getVolume().getSizeY(); iY++)
                {
                    pData[(iY*this->getVolume().getSizeX()*4)+(iX*4)+0] = m_pCTF->getR((float)this->getVolume().getValue(iX,iY,iK));
                    pData[(iY*this->getVolume().getSizeX()*4)+(iX*4)+1] = m_pCTF->getG((float)this->getVolume().getValue(iX,iY,iK));
                    pData[(iY*this->getVolume().getSizeX()*4)+(iX*4)+2] = m_pCTF->getB((float)this->getVolume().getValue(iX,iY,iK));

                    if (this->getVolume().getValue(iX,iY,iK) > m_fCutoff)
                        pData[(iY*this->getVolume().getSizeX()*4)+(iX*4)+3] = m_pOTF->getOpacity((float)this->getVolume().getValue(iX,iY,iK));
                    else
                        pData[(iY*this->getVolume().getSizeX()*4)+(iX*4)+3] = 0;
                }

            // scale the texture
	    char* pScaledData = new char[m_iTexSizeX*m_iTexSizeY*4];
	    gluScaleImage(GL_RGBA, this->getVolume().getSizeX(), this->getVolume().getSizeY(), GL_UNSIGNED_BYTE, pData, m_iTexSizeX, m_iTexSizeY, GL_UNSIGNED_BYTE, pScaledData);
            delete[] pData;
            // create texture object
            svt_texture* pTexXY = new svt_texture();
            pTexXY->setDataRGBA(pScaledData, m_iTexSizeX, m_iTexSizeY);

            return pTexXY;
        };
        /**
         * generate the textures for the xz slice
         * \param iK index of the slice
         */
        svt_texture* generateTexturesXZ(int iK)
        {
            char* pData = new char[this->getVolume().getSizeX()*this->getVolume().getSizeZ()*4];

            unsigned int iX, iZ;
            for(iX=0; iX<this->getVolume().getSizeX(); iX++)
                for(iZ=0; iZ<this->getVolume().getSizeZ(); iZ++)
                {
                    pData[(iZ*this->getVolume().getSizeX()*4)+(iX*4)+0] = m_pCTF->getR((float)this->getVolume().getValue(iX,iK,iZ));
                    pData[(iZ*this->getVolume().getSizeX()*4)+(iX*4)+1] = m_pCTF->getG((float)this->getVolume().getValue(iX,iK,iZ));
                    pData[(iZ*this->getVolume().getSizeX()*4)+(iX*4)+2] = m_pCTF->getB((float)this->getVolume().getValue(iX,iK,iZ));

                    if (this->getVolume().getValue(iX,iK,iZ) > m_fCutoff)
                        pData[(iZ*this->getVolume().getSizeX()*4)+(iX*4)+3] = m_pOTF->getOpacity((float)this->getVolume().getValue(iX,iK,iZ));
                    else
                        pData[(iZ*this->getVolume().getSizeX()*4)+(iX*4)+3] = 0;
                }

            // scale the texture
            char* pScaledData = new char[m_iTexSizeX*m_iTexSizeZ*4];
	    gluScaleImage(GL_RGBA, this->getVolume().getSizeX(), this->getVolume().getSizeZ(), GL_UNSIGNED_BYTE, pData, m_iTexSizeX, m_iTexSizeZ, GL_UNSIGNED_BYTE, pScaledData);
            delete[] pData;
            // create texture object
            svt_texture* pTexXZ = new svt_texture();
            pTexXZ->setDataRGBA(pScaledData, m_iTexSizeX, m_iTexSizeZ);

            return pTexXZ;
        };
        /**
         * generate the textures for the zy slice
         * \param iK index of the slice
         */
        svt_texture* generateTexturesZY(int iK)
        {
            char* pData = new char[this->getVolume().getSizeZ()*this->getVolume().getSizeY()*4];

            unsigned int iZ, iY;
            for(iZ=0; iZ<this->getVolume().getSizeZ(); iZ++)
                for(iY=0; iY<this->getVolume().getSizeY(); iY++)
                {
                    pData[(iY*this->getVolume().getSizeZ()*4)+(iZ*4)+0] = m_pCTF->getR((float)this->getVolume().getValue(iK,iY,iZ));
                    pData[(iY*this->getVolume().getSizeZ()*4)+(iZ*4)+1] = m_pCTF->getG((float)this->getVolume().getValue(iK,iY,iZ));
                    pData[(iY*this->getVolume().getSizeZ()*4)+(iZ*4)+2] = m_pCTF->getB((float)this->getVolume().getValue(iK,iY,iZ));

                    if (this->getVolume().getValue(iK,iY,iZ) > m_fCutoff)
                        pData[(iY*this->getVolume().getSizeZ()*4)+(iZ*4)+3] = m_pOTF->getOpacity((float)this->getVolume().getValue(iK,iY,iZ));
                    else
                        pData[(iY*this->getVolume().getSizeZ()*4)+(iZ*4)+3] = 0;
                }

            // scale the texture
            char* pScaledData = new char[m_iTexSizeZ*m_iTexSizeY*4];
	    gluScaleImage(GL_RGBA, this->getVolume().getSizeZ(), this->getVolume().getSizeY(), GL_UNSIGNED_BYTE, pData, m_iTexSizeZ, m_iTexSizeY, GL_UNSIGNED_BYTE, pScaledData);
            delete[] pData;
            // create texture object
            svt_texture* pTexZY = new svt_texture();
            pTexZY->setDataRGBA(pScaledData, m_iTexSizeZ, m_iTexSizeY);

            return pTexZY;
        };

///////////////////////////////////////////////////////////////////////////////
// Slices
///////////////////////////////////////////////////////////////////////////////

    public:

        /**
         * get current slice xy
         * \return the z position of the current slice
         */
        int getCurrentSliceXY() const
        {
            return m_iCurrSliceXY;
        };
        /**
         * get current slice xz
         * \return the y position of the current slice
         */
        int getCurrentSliceXZ() const
        {
            return m_iCurrSliceXZ;
        };
        /**
         * get current slice zy
         * \return the x position of the current slice
         */
        int getCurrentSliceZY() const
        {
            return m_iCurrSliceZY;
        };

        /**
         * set current slice xy
         * \param iCurrSliceXY z position of current slice
         * \return the z position of the current slice
         */
        int setCurrentSliceXY(int iCurrSliceXY)
        {
            m_iCurrSliceXY = iCurrSliceXY;
            if (m_iCurrSliceXY < 1)
                m_iCurrSliceXY = 1;
            if (m_iCurrSliceXY > this->getVolume().getSizeZ())
                m_iCurrSliceXY = this->getVolume().getSizeZ();
            this->rebuildDL();
            return m_iCurrSliceXY;
        };
        /**
         * set current slice xz
         * \param iCurrSliceXZ y position of current slice
         * \return the y position of the current slice
         */
        int setCurrentSliceXZ(int iCurrSliceXZ)
        {
            m_iCurrSliceXZ = iCurrSliceXZ;
            if (m_iCurrSliceXZ < 1)
                m_iCurrSliceXZ = 1;
            if (m_iCurrSliceXZ > this->getVolume().getSizeY())
                m_iCurrSliceXZ = this->getVolume().getSizeY();
            this->rebuildDL();
            return m_iCurrSliceXZ;
        };
        /**
         * set current slice zy
         * \param iCurrSliceZY x position of current slice
         * \return the x position of the current slice
         */
        int setCurrentSliceZY(int iCurrSliceZY)
        {
            m_iCurrSliceZY = iCurrSliceZY;
            if (m_iCurrSliceZY < 1)
                m_iCurrSliceZY = 1;
            if (m_iCurrSliceZY > this->getVolume().getSizeX())
                m_iCurrSliceZY = this->getVolume().getSizeX();
            this->rebuildDL();
            return m_iCurrSliceZY;
        };

///////////////////////////////////////////////////////////////////////////////
// Cutoff
///////////////////////////////////////////////////////////////////////////////

    public:
        /**
         * return the cutoff value
         * \return the cutoff value
         */
        T getCutoff() const
        {
            return m_fCutoff;
        };
        /**
         * set the cutoff value
         * \param fCutoff the cutoff value
         */
        T setCutoff(T fCutoff)
        {
            m_fCutoff = fCutoff;
            if (m_pCTF != NULL)
            {
                //m_pCTF->setMax(m_fCutoff);
                //m_pCTF->update();
                //m_pOTF->setMax(m_fCutoff);
                //m_pOTF->update();
            }
            return m_fCutoff;
        };

///////////////////////////////////////////////////////////////////////////////
// Transfer functions
///////////////////////////////////////////////////////////////////////////////

    public:

        /**
         * get the current color tranfer function
         * \return pointer to the current svt_float2color object
         */
        svt_float2color* getFloat2Color() const
        {
            return m_pCTF;
        };
        /**
         * set the current color transfer function
         * \param pCTF pointer to the current svt_float2color object
         */
        void setFloat2Color(svt_float2color* pCTF)
        {
            m_pCTF = pCTF;
        };
        /**
         * get the current opacity transfer function
         * \return pointer to the current svt_float2opacity object
         */
        svt_float2opacity* getFloat2Opacity() const
        {
            return m_pOTF;
        };
        /**
         * set the current opacity transfer function
         * \param pOTF pointer to the current svt_float2opacity object
         */
        void setFloat2Opacity(svt_float2opacity* pOTF)
        {
            m_pOTF = pOTF;
        };

///////////////////////////////////////////////////////////////////////////////
// Blocks
///////////////////////////////////////////////////////////////////////////////

    public:

        /**
         * switch volume mode on/off
         * \param bVolumeMode if true the volume mode is activated
         */
        void setVolumeMode(bool bVolumeMode)
        {
            m_bVolumeMode = bVolumeMode;
        }
        /**
         * get volume mode state
         * \return true the volume mode is activated
         */
        bool getVolumeMode()
        {
            return m_bVolumeMode;
        }

///////////////////////////////////////////////////////////////////////////////
// Drawing
///////////////////////////////////////////////////////////////////////////////

    public:

        /**
         * Enable or disable the surrounding box
         * \param bBox true if box should be shown
         */
        void setBox(bool bBox)
        {
            m_bBox = bBox;
            this->rebuildDL();
        }
        /**
         * Get if the surrounding box is drawn
         * \return true if box is shown
         */
        bool getBox()
        {
            return m_bBox;
        }
            
        /**
         * set state of drawing the xy slice
         * \param b if xy slice should be drawn
         */
        void setDrawXY(bool b)
        {
            m_bDrawXY = b;
            this->rebuildDL();
        }
        /**
         * set state of drawing the xz slice
         * \param b if xz slice should be drawn
         */
        void setDrawXZ(bool b)
        {
            m_bDrawXZ = b;
            this->rebuildDL();
        }
        /**
         * set state of drawing the zy slice
         * \param b if zy slice should be drawn
         */
        void setDrawZY(bool b)
        {
            m_bDrawZY = b;
            this->rebuildDL();
        }
        /**
         * get state of drawing the xy slice
         * \return if xy slice should be drawn
         */
        bool getDrawXY()
        {
            return m_bDrawXY;
        }
        /**
         * get state of drawing the xz slice
         * \return if xz slice should be drawn
         */
        bool getDrawXZ()
        {
            return m_bDrawXZ;
        }
        /**
         * get state of drawing the zy slice
         * \return if zy slice should be drawn
         */
        bool getDrawZY()
        {
            return m_bDrawZY;
        }

        /**
         * draw the xy slice
         * \param iSlice the z position of the slice
         */
        void drawXYSlice(int iSlice)
        {
            if (m_pTexXY)
            {
                m_pTexXY[iSlice-1]->BindTexture();
                float fTemp = -0.5f + ((float)(iSlice-1)*m_fFactXY) + 0.5f*m_fFactXY;
                glBegin(GL_QUADS);
                glTexCoord2f( 0.0f,  0.0f); glVertex3f(-0.5f, -0.5f, fTemp);
                glTexCoord2f( 1.0f,  0.0f); glVertex3f(+0.5f, -0.5f, fTemp);
                glTexCoord2f( 1.0f,  1.0f); glVertex3f(+0.5f, +0.5f, fTemp);
                glTexCoord2f( 0.0f,  1.0f); glVertex3f(-0.5f, +0.5f, fTemp);
                glEnd();
            }
        }

        /**
         * draw the xz slice
         * \param iSlice the y position of the slice
         */
        void drawXZSlice(int iSlice)
        {
            if (m_pTexXZ)
            {
                m_pTexXZ[iSlice-1]->BindTexture();
                float fTemp =  -0.5f + ((float)(iSlice-1)*m_fFactXZ) + 0.5f*m_fFactXZ;
                glBegin(GL_QUADS);
                glTexCoord2f( 0.0f,  0.0f); glVertex3f(-0.5f, fTemp, -0.5f);
                glTexCoord2f( 1.0f,  0.0f); glVertex3f(+0.5f, fTemp, -0.5f);
                glTexCoord2f( 1.0f,  1.0f); glVertex3f(+0.5f, fTemp, +0.5f);
                glTexCoord2f( 0.0f,  1.0f); glVertex3f(-0.5f, fTemp, +0.5f);
                glEnd();
            }
        }

        /**
         * draw the zy slice
         * \param iSlice the x position of the slice
         */
        void drawZYSlice(int iSlice)
        {
            if (m_pTexZY)
            {
                m_pTexZY[iSlice-1]->BindTexture();
                float fTemp =  -0.5f + ((float)(iSlice-1)*m_fFactZY) + 0.5f*m_fFactZY;
                glBegin(GL_QUADS);
                glTexCoord2f( 0.0f, 0.0f); glVertex3f(fTemp, -0.5f, -0.5f);
                glTexCoord2f( 0.0f, 1.0f); glVertex3f(fTemp, +0.5f, -0.5f);
                glTexCoord2f( 1.0f, 1.0f); glVertex3f(fTemp, +0.5f, +0.5f);
                glTexCoord2f( 1.0f, 0.0f); glVertex3f(fTemp, -0.5f, +0.5f);
                glEnd();
            }
        }

        /**
         * opengl drawing routine
         */
        virtual void drawGL()
        {
	    if (m_bGenerateTextures)
		generateTexturesGL();

	    // opengl settings
	    glDisable(GL_DITHER);
	    glDisable(GL_LIGHTING);
            glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	    glEnable(GL_TEXTURE_2D);
	    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glColor3f(1.0f,1.0f,1.0f);

	    if (!m_bVolumeMode)
            {
                glDisable(GL_DEPTH_TEST);
                glShadeModel(GL_FLAT);

		// draw xz slice
		if (m_bDrawXZ)
		    drawXZSlice(m_iCurrSliceXZ);
		// draw zy slice
		if (m_bDrawZY)
		    drawZYSlice(m_iCurrSliceZY);
		// draw xy slice
		if (m_bDrawXY)
                    drawXYSlice(m_iCurrSliceXY);

                glEnable(GL_DEPTH_TEST);
                glShadeModel(GL_SMOOTH);

                if (m_bBox)
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glDisable(GL_TEXTURE_2D);
                    glDisable(GL_BLEND);
                    glColor3f(1.0f,1.0f,1.0f);
                    // draw xz slice
                    if (m_bDrawXZ)
                        drawXZSlice(m_iCurrSliceXZ);
                    // draw zy slice
                    if (m_bDrawZY)
                        drawZYSlice(m_iCurrSliceZY);
                    // draw xy slice
                    if (m_bDrawXY)
                        drawXYSlice(m_iCurrSliceXY);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }

	    } else {

		// calculate vector from viewer to object
		Matrix4f glMat;
		glGetFloatv(GL_MODELVIEW_MATRIX, static_cast<GLfloat *>(&glMat[0][0]));
		svt_matrix4f oTrans("oTrans");
		oTrans.setFromMatrix(glMat);
		// eliminate translation to eye-position and rotation for display
		// that means that we are in world-coordinates now
		oTrans.postMult(__svt_getInvCanvasViewerMat());

		// viewer matrix
		svt_matrix4f oTmp(*__svt_getViewerMat(), "oTmp");

		// front-vector: vector from eye to object
                svt_vector4f oFront;
                //oFront.setPos(0.0, 0.0, -1.0);
		//oFront.setPos(oTmp(3,0)-oTrans(3,0), oTmp(3,1)-oTrans(3,1), oTmp(3,2)-oTrans(3,2));
		oFront.normalize();
		//cout << "svt_volume_slices: front:" << endl;
		//oFront.dump();

		// normals
		svt_vector4f oNormalX;
		oNormalX.setPos(1.0f,0.0f,0.0f);
		oNormalX.postMult(oTrans);
		oNormalX.normalize();
		svt_vector4f oNormalY;
		oNormalY.setPos(0.0f,1.0f,0.0f);
		oNormalY.postMult(oTrans);
		oNormalY.normalize();
		svt_vector4f oNormalZ;
		oNormalZ.setPos(0.0f,0.0f,-1.0f);
		oNormalZ.postMult(oTrans);
		oNormalZ.normalize();

		// calculate angle between normal vector and viewing vector
		float fAngleX =0.0f;
		fAngleX = acos(oNormalX.pointProduct(oFront));
		fAngleX = (fAngleX*180.0f) / PI;
		float fAngleY =0.0f;
		fAngleY = acos(oNormalY.pointProduct(oFront));
		fAngleY = (fAngleY*180.0f) / PI;
		float fAngleZ =0.0f;
		fAngleZ = acos(oNormalZ.pointProduct(oFront));
		fAngleZ = (fAngleZ*180.0f) / PI;

		// debug
		//cout << "svt_slices: pp (" << oNormalX.pointProduct(oFront) << "," << oNormalY.pointProduct(oFront) << "," << oNormalY.pointProduct(oFront) << ")" << endl;
		//cout << "svt_volume_slices: fAngle (" << fAngleX << "," << fAngleY << "," << fAngleZ << ")" << endl;

                unsigned int iX, iY, iZ;

		// draw block xy
                if (
                    ((fAngleX > 45.0f && fAngleX < 135.0f) || (fAngleX > 235.0f && fAngleX < 315.0f)) &&
                    ((fAngleY > 45.0f && fAngleY < 135.0f) || (fAngleY > 235.0f && fAngleY < 315.0f))
                   )
                {
                    if (fAngleZ > 90.0f && fAngleZ < 270.0f)
                        for(iZ=1;iZ<this->getVolume().getSizeZ();iZ++)
                            drawXYSlice(iZ);
                    else
                        for(iZ=this->getVolume().getSizeZ()-1;iZ>0;iZ--)
                            drawXYSlice(iZ);
                }
                // draw block xz
                if (fAngleY < 45.0f || (fAngleY > 135.0f && fAngleY < 235) || fAngleY > 315.0f)
                {
                    if (fAngleY < 45.0f)
                    //if (fAngleY > 90.0f && fAngleY < 270.0f)
                        for(iY=1;iY<this->getVolume().getSizeY();iY++)
                            drawXZSlice(iY);
                    else
                        for(iY=this->getVolume().getSizeY()-1;iY>0;iY--)
                            drawXZSlice(iY);
                }
                // draw block zy
                if (
                    ((fAngleX > 135.0f && fAngleX < 235.0f) || fAngleX > 315.0f || fAngleX < 45.0f) &&
                    ((fAngleY > 45.0f && fAngleY < 135.0f) || (fAngleY > 235.0f && fAngleY < 315.0f))
                   )
                {
                    if (fAngleX > 315.0f || fAngleX < 45.0f)
                    //if (fAngleX > 90.0f && fAngleX < 270.0f)
                        for(iX=1;iX<this->getVolume().getSizeX();iX++)
                            drawZYSlice(iX);
                    else
                        for(iX=this->getVolume().getSizeX()-1;iX>0;iX--)
                            drawZYSlice(iX);
		}

                /*
		// draw block xy
		if (
		    ((fAngleX > 45.0f && fAngleX < 135.0f) || (fAngleX > 235.0f && fAngleX < 315.0f)) &&
		    ((fAngleY > 45.0f && fAngleY < 135.0f) || (fAngleY > 235.0f && fAngleY < 315.0f))
		   )
		{
		    if (fAngleZ > 90.0f && fAngleZ < 270.0f)
			for(int iZ=1;iZ<m_iSizeZ;iZ++)
			    drawXYSlice(iZ);
		    else
			for(int iZ=m_iSizeZ-1;iZ>0;iZ--)
			    drawXYSlice(iZ);
		}
		// draw block xz
		if (fAngleY < 45.0f || (fAngleY > 135.0f && fAngleY < 235) || fAngleY > 315.0f)
		{
		    if (fAngleY < 45.0f)
			//if (fAngleY > 90.0f && fAngleY < 270.0f)
			for(int iY=1;iY<m_iSizeY;iY++)
			    drawXZSlice(iY);
		    else
			for(int iY=m_iSizeY-1;iY>0;iY--)
			    drawXZSlice(iY);
		}
		// draw block zy
		if (
		    ((fAngleX > 135.0f && fAngleX < 235.0f) || fAngleX > 315.0f || fAngleX < 45.0f) &&
		    ((fAngleY > 45.0f && fAngleY < 135.0f) || (fAngleY > 235.0f && fAngleY < 315.0f))
		   )
		{
		    if (fAngleX > 315.0f || fAngleX < 45.0f)
			//if (fAngleX > 90.0f && fAngleX < 270.0f)
			for(int iX=1;iX<m_iSizeX;iX++)
			    drawZYSlice(iX);
		    else
			for(int iX=m_iSizeX-1;iX>0;iX--)
			    drawZYSlice(iX);
		} */
	    }

	    // opengl settings
	    glDisable(GL_TEXTURE_2D);
            glDisable(GL_BLEND);
	    glEnable(GL_DITHER);
	    glEnable(GL_LIGHTING);
	}
};

#endif
