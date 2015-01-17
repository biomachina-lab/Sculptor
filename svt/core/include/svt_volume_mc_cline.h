/***************************************************************************
                          svt_volume_mc_cline
                          -- ----------------
    begin                : 21.04.2001
    author               : Fabrice Roth, Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_node.h>
#include <svt_box.h>
#include <svt_mesh.h>
#include <svt_const.h>
#include <svt_volume.h>
#include <svt_time.h>
#include <svt_volumeRenderer.h>
#include <svt_messages.h>
#include <svt_container.h>
#include <svt_palette.h>

#ifndef SVT_VOLUME_MC_CLINE_H
#define SVT_VOLUME_MC_CLINE_H

///////////////////////////////////////////////////////////////////////////////
// datastructures
///////////////////////////////////////////////////////////////////////////////

typedef struct
{
  float x;
  float y;
  float z;
} Point3D;

typedef struct
{
  Point3D p;
  /*unsigned char*/ float val;
} Xyz;              

typedef struct
{
  Xyz p[3];
} MC_Triangle;

///////////////////////////////////////////////////////////////////////////////
// class implementation
///////////////////////////////////////////////////////////////////////////////

/**
 * Displays volume data with a marching cube algorithm and a resulting isosurface. The marching cube algorithm is the W.E. Lorensen and H. Cline algorithm as described in Siggraph 87, Vol. 21 No. 4, p. 163-169
 *@author Stefan Birmanns
 */
template<class T> class DLLEXPORT svt_volume_mc_cline : public svt_volumeRenderer<T>
{
protected:

    T                   m_fIsoLevel;
    T                   m_fRenderIsoLevel;
    int                 m_iStep;
    int                 m_iRenderStep;
    svt_mesh*           m_pMesh;
    bool                m_bFastMesh;
    bool                m_bRecalcMesh;
    svt_container*      m_pCont;
    svt_palette         m_oPalette;
    svt_semaphore       m_oMCSema;

public:

    /**
     * Constructor
     * \param reference to svt_volume object
     */
    svt_volume_mc_cline(svt_volume<T>& rVolume) : svt_volumeRenderer<T>(rVolume),
        m_fIsoLevel(0),
        m_iStep(1),
        m_pMesh( NULL ),
        m_bRecalcMesh( false ),
        m_pCont( NULL )
    {
        m_pMesh = new svt_mesh(new svt_pos(-0.5f,-0.5f,-0.5f), NULL );
        m_pMesh->setName("(svt_volume_mc_cline)m_pMesh");
        this->add(m_pMesh);

        m_pCont = new svt_container( new svt_pos(-0.5f,-0.5f,-0.5f), NULL );
        m_pCont->setName("(svt_volume_mc_cline)m_pCont");
        this->add(m_pCont);

        m_oPalette.addColor( svt_color(0.90f, 0.90f, 0.90f ) );
        m_oPalette.addColor( svt_color(0.80f, 0.20f, 0.20f ) );
        m_oPalette.addColor( svt_color(0.25f, 0.25f, 1.00f ) );
        m_oPalette.addColor( svt_color(0.25f, 0.75f, 0.75f ) );
        m_oPalette.addColor( svt_color(0.80f, 0.80f, 0.00f ) );
        m_oPalette.addColor( svt_color(0.50f, 0.50f, 0.20f ) );
        m_oPalette.addColor( svt_color(0.60f, 0.60f, 0.60f ) );

    }
    virtual ~svt_volume_mc_cline()
    {
        //m_pMesh->deleteTriangles();
        //m_pCont->delAll();
    }

    virtual void drawGL()
    {
        if (m_bRecalcMesh)
            recalcMesh_DOIT();
    };

public:
    /**
     * set the isolevel
     */
    inline void setIsolevel(T iIsoLevel)
    {
        m_fIsoLevel = iIsoLevel;
    }
    /**
     * get the isolevel
     */
    inline T getIsolevel()
    {
        return m_fIsoLevel;
    }

    /**
     * get the step width
     */
    inline int getStep()
    {
        return m_iStep;
    }
    /**
     * set the step width
     */
    inline void setStep(int iStep)
    {
        if (iStep > 0)
            m_iStep = iStep;
    }
    /**
     * set if the mesh should be computed fast or not
     */
    inline void calcFastMesh(bool b)
    {
        m_bFastMesh = b;
    }

    /**
     * recalc the mesh. this function should be called after every setData call.
     */
    void recalcMesh()
    {
        m_bRecalcMesh = true;
        m_iRenderStep = m_iStep;
        this->rebuildDL();
    }

    /**
     * This function actually regenerates the mesh, do not call directly!
     */
    void recalcMesh_DOIT()
    {
        while( m_bRecalcMesh )
        {
            if (m_oMCSema.tryLock())
            {
                m_bRecalcMesh = false;
                m_fRenderIsoLevel = m_fIsoLevel;

                m_pMesh->deleteTriangles();
                m_pCont->delAll();
                m_pMesh->rebuildDL();
                m_pCont->rebuildDL();

                int dims[3];
                dims[0] = this->getVolume().getSizeX();
                dims[1] = this->getVolume().getSizeY();
                dims[2] = this->getVolume().getSizeZ();

                T ratio[3];
                ratio[0] = 1.0f / (this->getVolume().getSizeX()-1);
                ratio[1] = 1.0f / (this->getVolume().getSizeY()-1);
                ratio[2] = 1.0f / (this->getVolume().getSizeZ()-1);

                polygonise(dims, ratio);

                svt_redraw();

                m_oMCSema.V();

            } else {

                this->rebuildDL();
                svt_redraw();

            }
        }

    }

    /**
     * get the mesh object
     */
    svt_mesh* getMesh()
    {
        return m_pMesh;
    }

protected:

    T ABS(T x)
    {
        if ((x) > 0)
            return (x);
        else
            return -(x);
    }

    T getVolValue( unsigned int iX, unsigned int iY, unsigned int iZ)
    {
        //if (this->m_pSegVol != NULL && this->m_iSegSel != 0 && this->getSegVol()->getValue(iX,iY,iZ) != this->m_iSegSel)
        //    return 0;
        //else
            return this->getVolume().getValue(iX,iY,iZ);
    };

    /*********************************************************************************
     *                                                                               *
     * Calculate the Gradients at the vertices of the MC-Cube                        *
     *                                                                               *
     * Parameter:                                                                    *
     * ---------                                                                     *
     * x,y,z          : Actual Position in the grid (0,0,0) of the cube              *
     * buf            : Sampled data                                                 *
     * dims           : X,y,z Dimension                                              *
     * ration         : Aspect Ration of the different axes                          *
     *                                                                               *
     * Output:                                                                       *
     * -------                                                                       *
     * gradients      : 8 gradient-values of that cube                               *
     *                                                                               *
     *********************************************************************************/

    Point3D ComputePointGradient(int x, int y, int z, int *dims, T *ratio)
    {

	Point3D gradients;
	float a;

	if (x==0)
	{ // left border reached.
	    gradients.x = -(getVolValue(x+m_iRenderStep,y,z) - getVolValue(x,y,z))/ratio[0];
	}
	else if (x >= dims[0] - m_iRenderStep ) { // right border reached
	    gradients.x = -(getVolValue(x,y,z) - getVolValue(x-m_iRenderStep,y,z))/ratio[0];
	}
	else {
	    gradients.x = -(getVolValue(x+m_iRenderStep,y,z) - getVolValue(x-m_iRenderStep,y,z)) / (2*ratio[0]);
	}

	if (y==0) { // left border reached.
	    gradients.y = -(getVolValue(x,y+m_iRenderStep,z) - getVolValue(x,y,z))/ratio[1];
	}
	else if (y >= dims[1] - m_iRenderStep ) { // right border reached
	    gradients.y = -(getVolValue(x,y,z) - getVolValue(x,y-m_iRenderStep,z))/ratio[1];
	}
	else {
	    gradients.y = -(getVolValue(x,y+m_iRenderStep,z) - getVolValue(x,y-m_iRenderStep,z)) /  (2*ratio[1]);
	}

	if (z == 0) { // left border reached.
	    gradients.z = -(getVolValue(x,y,z+m_iRenderStep) - getVolValue(x,y,z))/ratio[2];
	}
	else if (z >= dims[2]-m_iRenderStep ) { // right border reached
	    gradients.z = -(getVolValue(x,y,z) - getVolValue(x,y,z-m_iRenderStep))/ratio[2];
	}
	else {
	    gradients.z = -(getVolValue(x,y,z+m_iRenderStep) - getVolValue(x,y,z-m_iRenderStep)) /  (2*ratio[2]);

	}

	a = sqrt(gradients.x*gradients.x + gradients.y*gradients.y + gradients.z*gradients.z);
	if (a != 0.0)
	{
	    gradients.x /= a;
	    gradients.y /= a;
	    gradients.z /= a;
	}

	return (gradients);
    };

    /*********************************************************************************
     *                                                                               *
     * Interpolate the gradient-vectors of two points in the 3D-Space                *
     *                                                                               *
     * Parameter:                                                                    *
     * ---------                                                                     *
     * p1, p2         : The two vertices                                             *
     * p1_val, p2_val : Values of the two vertices                                   *
     *                                                                               *
     *********************************************************************************/
    Point3D GradientInterp(Point3D p1, Point3D p2, T val1, T val2)
    {

	float mu;
	Point3D pt;

	float nlength;

	if (ABS(m_fRenderIsoLevel - val1) < 0.00001)  // p1 is nearly on the isosurface
	{   
	    pt.x = p1.x;
	    pt.y = p1.y;
	    pt.z = p1.z;
	    return(pt);
	}
	if (ABS(m_fRenderIsoLevel - val2) < 0.00001)  // p2 is nearly on the isosurface
	{   
	    pt.x = p2.x;
	    pt.y = p2.y;
	    pt.z = p2.z;
	    return(pt);
	}
	if (ABS(val2 - val1) < 0.00001)     // p1 and p2  is nearly on the isosurface
	{
	    pt.x = p1.x;
	    pt.y = p1.y;
	    pt.z = p1.z;
	    return(pt);
	}

	mu = (float)(m_fRenderIsoLevel - val1) / (val2 - val1);

	pt.x = p1.x + mu * (p2.x - p1.x);
	pt.y = p1.y + mu * (p2.y - p1.y);
	pt.z = p1.z + mu * (p2.z - p1.z);

	nlength = sqrt(pt.x*pt.x+pt.y*pt.y+pt.z*pt.z);

	if (nlength != 0.0)
	{
	    pt.x = pt.x / nlength;
	    pt.y = pt.y / nlength;
	    pt.z = pt.z / nlength;
	}
	return(pt);
    }

    /*********************************************************************************
     *                                                                               *
     * Interpolate the position where the isosurface cuts an edge between two        *
     * vertices, each with own scalar value                                          *
     *                                                                               *
     * Parameter:                                                                    *
     * ---------                                                                     *
     * p1, p2         : The two vertices                                             *
     * p1_val, p2_val : Values of the two vertices                                   *
     *                                                                               *
     *********************************************************************************/
    Point3D VertexInterp(Xyz p1, Xyz p2)
    {
	float mu;
	Point3D pt;

	if (ABS(m_fRenderIsoLevel - p1.val) < 0.00001) // p1 is nearly on the isosurface
	{
	    pt.x = p1.p.x;
	    pt.y = p1.p.y;
	    pt.z = p1.p.z;
	    return(pt);
	}
	if (ABS(m_fRenderIsoLevel - p2.val) < 0.00001) // p2 is nearly on the isosurface
	{
	    pt.x = p2.p.x;
	    pt.y = p2.p.y;
	    pt.z = p2.p.z;
	    return(pt);
	}
	if (ABS(p2.val - p1.val) < 0.00001)// p1 and p2  is nearly on the isosurface
	{
	    pt.x = p1.p.x;
	    pt.y = p1.p.y;
	    pt.z = p1.p.z;
	    return(pt);
	}

	mu = (float)(m_fRenderIsoLevel - p1.val) / (p2.val - p1.val);
	pt.x = p1.p.x + mu * (p2.p.x - p1.p.x);
	pt.y = p1.p.y + mu * (p2.p.y - p1.p.y);
	pt.z = p1.p.z + mu * (p2.p.z - p1.p.z);

	return(pt);
    }

    /*********************************************************************************
     *                                                                               *
     * Build the required tables and write the triangles grid = Mesh of              *
     * the input data-points                                                         *
     *                                                                               *
     * Parameter:                                                                    *
     * ---------                                                                     *
     * grid            : Input Mesh of the sampled data                              *
     * triangles      : Array of the needed triangles to polygonize of Volume        *
     *                                                                               *
     *********************************************************************************/

    void polygonise(int *dims, T* ratio)
    {

	/* we access this table with the vertex-index we formed. Here we get the index to
	 get the polygonization -> the triangle-configuration
	 */

	static int edge_table[256]=
	{
	    0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c, // 7
	    0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00, // 15
	    0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c, // 23
	    0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90, // 31
	    0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c, // 39
	    0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30, // 47
	    0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac, // 55
	    0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0, // 63
	    0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c, // 71
	    0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60, // 79
	    0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc, // 87
	    0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0, // 95
	    0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c, // 103
	    0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950, // 111
	    0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc , // 119
	    0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0, // 127

	    // ---------- Complementary cases -------------------------

	    0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc, // 135
	    0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0, // 143
	    0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c, // 151
	    0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650, // 159
	    0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc, // 167
	    0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0, // 175
	    0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c, // 183
	    0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460, // 191
	    0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac, // 199
	    0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0, // 207
	    0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c, // 215
	    0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230, // 223
	    0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c, // 231
	    0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190, // 239
	    0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c, // 247
	    0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0    // 255
	};

	/* Table where the intersection points are stored */
	static int tri_table[256][16] =
	{
            // 1
	    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
	    {3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},

            // 11
	    {1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
	    {3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
	    {3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
	    {9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},

            // 21
	    {1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
	    {9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
	    {2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
	    {8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
	    {9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
	    {4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
	    {3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
	    {1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},

            // 31
	    {4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
	    {4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
	    {9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
	    {1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
	    {5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
	    {2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},

            // 41
	    {9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
	    {0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
	    {2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
	    {10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
	    {4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
	    {5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
	    {5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
	    {9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},

            // 51
	    {0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
	    {1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
	    {10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
	    {8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
	    {2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
	    {7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
	    {9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
	    {2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
	    {11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},

            // 61
	    {9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
	    {5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
	    {11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
	    {11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
	    {1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},

            // 71
	    {9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
	    {5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
	    {2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
	    {0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
	    {5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
	    {6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
	    {0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
	    {3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
	    {6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},

            // 81
	    {5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
	    {1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
	    {10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
	    {6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
	    {1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
	    {8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
	    {7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
	    {3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
	    {5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},

            // 91
	    {0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
	    {9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
	    {8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
	    {5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
	    {0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
	    {6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
	    {10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
	    {10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
	    {8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},

            // 101
	    {1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
	    {3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
	    {0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
	    {10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
	    {0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
	    {3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
	    {6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
	    {9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
	    {8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},

            // 111
	    {3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
	    {6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
	    {0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
	    {10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
	    {10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
	    {1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
	    {2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
	    {7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
	    {7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},

            // 121
	    {2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
	    {2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
	    {1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
	    {11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
	    {8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
	    {0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
	    {7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},

            // 131
	    {0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
	    {10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
	    {2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
	    {6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
	    {7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
	    {2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
	    {1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},

            // 141
	    {10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
	    {10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
	    {0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
	    {7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
	    {6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
	    {8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
	    {9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
	    {6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
	    {1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},

            // 151
	    {4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
	    {10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
	    {8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
	    {0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
	    {1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
	    {8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
	    {10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
	    {4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
	    {10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},

            // 161
	    {4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
	    {5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
	    {11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
	    {9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
	    {6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
	    {7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
	    {3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
	    {7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
	    {9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},

            // 171
	    {3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
	    {6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
	    {9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
	    {1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
	    {4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
	    {7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
	    {6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
	    {3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
	    {0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
	    {6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},

            // 181
	    {1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
	    {0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
	    {11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
	    {6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
	    {5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
	    {9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
	    {1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
	    {1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
	    {10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},

            // 191
	    {0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
	    {5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
	    {10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
	    {11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
	    {0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
	    {9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
	    {7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},

            // 201
	    {2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
	    {8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
	    {9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
	    {9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
	    {1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
	    {9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
	    {9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
	    {5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},

            // 211
	    {0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
	    {10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
	    {2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
	    {0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
	    {0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
	    {9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
	    {5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
	    {3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
	    {5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},

            // 221
	    {8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
	    {0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
	    {9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
	    {0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
	    {1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
	    {3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
	    {4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
	    {9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},

            // 231
	    {11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
	    {11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
	    {2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
	    {9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
	    {3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
	    {1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
	    {4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
	    {4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},

            // 241
	    {9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
	    {0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
	    {3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
	    {3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
	    {0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
	    {9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},

            // 251
	    {2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
	    {1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
	};

	int x,y,z;                    // index for the loop

	int cubeindex;                // index for the edge_table

	int i;                        // countindex

	Xyz grid[8];                  // one cube in the area
	Point3D vert_list[12];        // List of interpolated cut-points

	Point3D n[12];                // normal_vectors on the cut-points
	Point3D grad[8];              // normal vecotors of the cube-vertices

	Triangle oTriangle;
        Triangle oNormals;

	// init storage for marching cube mesh generation
        if (m_bFastMesh)
            m_pMesh->setCompareFlag(false);
        else
            m_pMesh->initMarchingCube(dims[2]/m_iRenderStep, dims[0]/m_iRenderStep, dims[1]/m_iRenderStep, m_bFastMesh);

        unsigned int iTriCount = 0;
        bool bProgress = false;
        T fValue;

        svt_c4ub_n3f_v3f oVert;
        svt_color oCol(1.0, 0.0, 0.0);
        m_pCont->beginTriangles();

        try
        {
            for (z=0; z < dims[2] - m_iRenderStep; z=z+m_iRenderStep)
            {

                if (bProgress && z%5==0)
                    svt_exception::ui()->progress( z, dims[2] - m_iRenderStep );

                for (y=0; y < dims[1] - m_iRenderStep; y=y+m_iRenderStep)
                {
                    for (x=0; x < dims[0] - m_iRenderStep; x=x+m_iRenderStep)
                    {
                        if (m_fIsoLevel != m_fRenderIsoLevel)
                        {
                            m_pCont->endTriangles();

                            if (bProgress)
                                svt_exception::ui()->progressPopdown();

                            // free storage for special marching cube mesh generation
                            if (!m_bFastMesh)
                                m_pMesh->freeMarchingCube();

                            this->rebuildDL();
                            svt_redraw();

                            m_bRecalcMesh = true;
                            return;
                        }

                        fValue = getVolValue(x,y,z);

                        // vertex 0 000
                        grid[0].val = fValue;
                        grid[0].p.x = ratio[0]*x;
                        grid[0].p.y = ratio[1]*y;
                        grid[0].p.z = ratio[2]*z;
                        // vertex 1 100
                        grid[1].val = getVolValue(x+m_iRenderStep,y,z);
                        grid[1].p.x = ratio[0]*(x+m_iRenderStep);
                        grid[1].p.y = ratio[1]*y;
                        grid[1].p.z = ratio[2]*z;
                        // vertex 2 010
                        grid[3].val = getVolValue(x,y+m_iRenderStep,z);
                        grid[3].p.x = ratio[0]*x;
                        grid[3].p.y = ratio[1]*(y+m_iRenderStep);
                        grid[3].p.z = ratio[2]*z;
                        // vertex 3 110
                        grid[2].val = getVolValue(x+m_iRenderStep,y+m_iRenderStep,z);
                        grid[2].p.x = ratio[0]*(x+m_iRenderStep);
                        grid[2].p.y = ratio[1]*(y+m_iRenderStep);
                        grid[2].p.z = ratio[2]*z;
                        // vertex 4 001
                        grid[4].val = getVolValue(x,y,z+m_iRenderStep);
                        grid[4].p.x = ratio[0]*x;
                        grid[4].p.y = ratio[1]*y;
                        grid[4].p.z = ratio[2]*(z+m_iRenderStep);
                        // vertex 5 101
                        grid[5].val = getVolValue(x+m_iRenderStep,y,z+m_iRenderStep);
                        grid[5].p.x = ratio[0]*(x+m_iRenderStep);
                        grid[5].p.y = ratio[1]*y;
                        grid[5].p.z = ratio[2]*(z+m_iRenderStep);
                        // vertex 5 011
                        grid[7].val = getVolValue(x,y+m_iRenderStep,z+m_iRenderStep);
                        grid[7].p.x = ratio[0]*x;
                        grid[7].p.y = ratio[1]*(y+m_iRenderStep);
                        grid[7].p.z = ratio[2]*(z+m_iRenderStep);
                        // vertex 6 111
                        grid[6].val = getVolValue(x+m_iRenderStep,y+m_iRenderStep,z+m_iRenderStep);
                        grid[6].p.x = ratio[0]*(x+m_iRenderStep);
                        grid[6].p.y = ratio[1]*(y+m_iRenderStep);
                        grid[6].p.z = ratio[2]*(z+m_iRenderStep);

                        // Calculate the index for the edge_table: Take all the vertices and
                        // bind them by OR -> we get the 8-bit address
                        cubeindex = 0;

                        if (grid[0].val >= m_fRenderIsoLevel)
                            cubeindex = cubeindex | 1;
                        if (grid[1].val >= m_fRenderIsoLevel)
                            cubeindex = cubeindex | 2;
                        if (grid[2].val >= m_fRenderIsoLevel)
                            cubeindex = cubeindex | 4;
                        if (grid[3].val >= m_fRenderIsoLevel)
                            cubeindex = cubeindex | 8;
                        if (grid[4].val >= m_fRenderIsoLevel)
                            cubeindex = cubeindex | 16;
                        if (grid[5].val >= m_fRenderIsoLevel)
                            cubeindex = cubeindex | 32;
                        if (grid[6].val >= m_fRenderIsoLevel)
                            cubeindex = cubeindex | 64;
                        if (grid[7].val >= m_fRenderIsoLevel)
                            cubeindex = cubeindex | 128;

                        // Check which egdes are cut. With the "&" we put a mask on the edge_table entry
                        // to find out the cut edges. (2^11 = 2048)
                        if (edge_table[cubeindex] == 0 ) // cube entirely in-/outside the isosurface
                        {

                            continue;

                        } else {

                            //calculate gradients on the edges
                            grad[0]=ComputePointGradient(x          ,y          ,z          ,dims, ratio);
                            grad[1]=ComputePointGradient((x+m_iRenderStep),y          ,z          ,dims, ratio);
                            grad[3]=ComputePointGradient(x          ,(y+m_iRenderStep),z          ,dims, ratio);
                            grad[2]=ComputePointGradient((x+m_iRenderStep),(y+m_iRenderStep),z          ,dims, ratio);
                            grad[4]=ComputePointGradient(x          ,y          ,(z+m_iRenderStep),dims, ratio);
                            grad[5]=ComputePointGradient((x+m_iRenderStep),y          ,(z+m_iRenderStep),dims, ratio);
                            grad[7]=ComputePointGradient(x          ,(y+m_iRenderStep),(z+m_iRenderStep),dims, ratio);
                            grad[6]=ComputePointGradient((x+m_iRenderStep),(y+m_iRenderStep),(z+m_iRenderStep),dims, ratio);

                            if (edge_table[cubeindex] & 1) {    // check whether edge 0 is cut
                                vert_list[0]=VertexInterp(grid[0],grid[1]);
                                n[0] = GradientInterp(grad[0],grad[1],grid[0].val,grid[1].val);
                            }
                            if (edge_table[cubeindex] & 2) {   // check whether edge 1 is cut
                                vert_list[1]=VertexInterp(grid[1],grid[2]);
                                n[1] = GradientInterp(grad[1],grad[2],grid[1].val,grid[2].val);
                            }

                            if (edge_table[cubeindex] & 4) {   // check whether edge 2 is cut
                                vert_list[2]=VertexInterp(grid[2],grid[3]);
                                n[2] = GradientInterp(grad[2],grad[3],grid[2].val,grid[3].val);
                            }
                            if (edge_table[cubeindex] & 8) {  // check whether edge 3 is cut
                                vert_list[3]=VertexInterp(grid[3],grid[0]);
                                n[3] = GradientInterp(grad[3],grad[0],grid[3].val,grid[0].val);
                            }
                            if (edge_table[cubeindex] & 16) {  // check whether edge 4 is cut
                                vert_list[4]=VertexInterp(grid[4],grid[5]);
                                n[4] = GradientInterp(grad[4],grad[5],grid[4].val,grid[5].val);
                            }
                            if (edge_table[cubeindex] & 32) { // check whether edge 5 is cut
                                vert_list[5]=VertexInterp(grid[5],grid[6]);
                                n[5] = GradientInterp(grad[5],grad[6],grid[5].val,grid[6].val);
                            }
                            if (edge_table[cubeindex] & 64) {  // check whether edge 6 is cut
                                vert_list[6]=VertexInterp(grid[6],grid[7]);
                                n[6] = GradientInterp(grad[6],grad[7],grid[6].val,grid[7].val);
                            }
                            if (edge_table[cubeindex] & 128) { // check whether edge 7 is cut
                                vert_list[7]=VertexInterp(grid[7],grid[4]);
                                n[7] = GradientInterp(grad[7],grad[4],grid[7].val,grid[4].val);
                            }
                            if (edge_table[cubeindex] & 256) { // check whether edge 8 is cut
                                vert_list[8]=VertexInterp(grid[0],grid[4]);
                                n[8] = GradientInterp(grad[0],grad[4],grid[0].val,grid[4].val);
                            }
                            if (edge_table[cubeindex] & 512) { // check whether edge 9 is cut
                                vert_list[9]=VertexInterp(grid[1],grid[5]);
                                n[9] = GradientInterp(grad[1],grad[5],grid[1].val,grid[5].val);
                            }
                            if (edge_table[cubeindex] & 1024) { // check whether edge 10 is cut
                                vert_list[10]=VertexInterp(grid[2],grid[6]);
                                n[10] = GradientInterp(grad[2],grad[6],grid[2].val,grid[6].val);
                            }
                            if (edge_table[cubeindex] & 2048) { // check whether edge 11 is cut
                                vert_list[11]=VertexInterp(grid[3],grid[7]);
                                n[11] = GradientInterp(grad[3],grad[7],grid[3].val,grid[7].val);
                            }


                            // i is the run-variable that runs through the lines of tri_table.
                            // The for-loop stops as soon as a -1, indicating that no triangle points
                            // will follow, occur
                            for (i=0; tri_table[cubeindex][i] !=-1;i+=3)
                            {
                                oTriangle.m_fP[0][0] = vert_list[tri_table[cubeindex][i]].x;   // * 0.1f;
                                oTriangle.m_fP[0][1] = vert_list[tri_table[cubeindex][i]].y;   // * 0.1f;
                                oTriangle.m_fP[0][2] = vert_list[tri_table[cubeindex][i]].z;   // * 0.1f;
                                oTriangle.m_fP[2][0] = vert_list[tri_table[cubeindex][i+1]].x; // * 0.1f;
                                oTriangle.m_fP[2][1] = vert_list[tri_table[cubeindex][i+1]].y; // * 0.1f;
                                oTriangle.m_fP[2][2] = vert_list[tri_table[cubeindex][i+1]].z; // * 0.1f;
                                oTriangle.m_fP[1][0] = vert_list[tri_table[cubeindex][i+2]].x; // * 0.1f;
                                oTriangle.m_fP[1][1] = vert_list[tri_table[cubeindex][i+2]].y; // * 0.1f;
                                oTriangle.m_fP[1][2] = vert_list[tri_table[cubeindex][i+2]].z; // * 0.1f;

                                oNormals.m_fP[0][0] = n[tri_table[cubeindex][i]].x;
                                oNormals.m_fP[0][1] = n[tri_table[cubeindex][i]].y;
                                oNormals.m_fP[0][2] = n[tri_table[cubeindex][i]].z;
                                oNormals.m_fP[2][0] = n[tri_table[cubeindex][i+1]].x;
                                oNormals.m_fP[2][1] = n[tri_table[cubeindex][i+1]].y;
                                oNormals.m_fP[2][2] = n[tri_table[cubeindex][i+1]].z;
                                oNormals.m_fP[1][0] = n[tri_table[cubeindex][i+2]].x;
                                oNormals.m_fP[1][1] = n[tri_table[cubeindex][i+2]].y;
                                oNormals.m_fP[1][2] = n[tri_table[cubeindex][i+2]].z;

                                if (this->m_pSegVol != NULL && this->m_iSegSel != 0)
                                {
                                    oCol = *m_oPalette.getColor( this->getSegVol()->getValue(x,y,z) );

                                    oVert.setC( oCol.getR() * 255.0f, oCol.getG() * 255.0f, oCol.getB() * 255.0f );

                                    oVert.setN( oNormals.m_fP[0][0],  oNormals.m_fP[0][1],  oNormals.m_fP[0][2] );
                                    oVert.setV(oTriangle.m_fP[0][0], oTriangle.m_fP[0][1], oTriangle.m_fP[0][2] );

                                    m_pCont->addVertex( oVert );

                                    oVert.setN( oNormals.m_fP[1][0],  oNormals.m_fP[1][1],  oNormals.m_fP[1][2] );
                                    oVert.setV(oTriangle.m_fP[1][0], oTriangle.m_fP[1][1], oTriangle.m_fP[1][2] );

                                    m_pCont->addVertex( oVert );

                                    oVert.setN( oNormals.m_fP[2][0],  oNormals.m_fP[2][1],  oNormals.m_fP[2][2] );
                                    oVert.setV(oTriangle.m_fP[2][0], oTriangle.m_fP[2][1], oTriangle.m_fP[2][2] );

                                    m_pCont->addVertex( oVert );

                                } else {

                                    if (m_bFastMesh)
                                        m_pMesh->addTriangle(oTriangle, oNormals);
                                    else
                                        m_pMesh->addTriangle(oTriangle, oNormals, x/m_iRenderStep, y/m_iRenderStep, z/m_iRenderStep, i/3); // special mc mesh insertion

                                }


                                iTriCount++;

                                if (iTriCount == 200000)
                                {
                                    svt_exception::ui()->progressPopup("Marching cube", 0, dims[2] - m_iRenderStep);
                                    bProgress = true;
                                }


                            } // endfor i

                        } //endif

                    } // endfor x

                } // endfor y

            }  // endfor z

        } catch (svt_userInterrupt&)
        {
        }

        if (bProgress)
            svt_exception::ui()->progressPopdown();

        m_pCont->endTriangles();

        // free storage for special marching cube mesh generation
        if (!m_bFastMesh)
            m_pMesh->freeMarchingCube();
    }
};

#endif
