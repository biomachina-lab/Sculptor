/***************************************************************************
                          svt_volume_compact
                          -- ----------------
    begin                : 12/12/2009
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_node.h>
#include <svt_box.h>
#include <svt_const.h>
#include <svt_volume.h>
#include <svt_time.h>
#include <svt_volumeRenderer.h>
#include <svt_volume_mc_cline.h>
#include <svt_messages.h>
#include <svt_container.h>
#include <svt_palette.h>

#ifndef SVT_VOLUME_COMPACT_H
#define SVT_VOLUME_COMPACT_H

#define CEPS 1.0E-6

///////////////////////////////////////////////////////////////////////////////
// class implementation
///////////////////////////////////////////////////////////////////////////////

/**
 * Displays volume data with a marching cube algorithm and a resulting isosurface. The marching cube algorithm is the W.E. Lorensen and H. Cline algorithm as described in Siggraph 87, Vol. 21 No. 4, p. 163-169
 *@author Stefan Birmanns
 */
template<class T> class DLLEXPORT svt_volume_compact : public svt_volumeRenderer<T>
{
protected:

    T                      m_fIsoLevel;
    T                      m_fRenderIsoLevel;
    int                    m_iStep;
    int                    m_iRenderStep;
    bool                   m_bFastMesh;
    bool                   m_bRecalcMesh;
    svt_container*         m_pCont;
    svt_palette            m_oPalette;
    svt_semaphore          m_oMCSema;
    int                    m_iTriangles;

    svt_volume<unsigned long>* m_pCompact;
    vector< unsigned char > m_aCompactCount;

public:

    /**
     * Constructor
     * \param reference to svt_volume object
     */
    svt_volume_compact(svt_volume<T>& rVolume) : svt_volumeRenderer<T>(rVolume),
        m_fIsoLevel(0),
        m_iStep(1),
        m_bRecalcMesh( false ),
        m_pCont( NULL ),
        m_pCompact( NULL )
    {
        m_pCont = new svt_container( new svt_pos(-0.5f,-0.5f,-0.5f), NULL );
        m_pCont->setName("(svt_volume_compact)m_pCont");
        this->add(m_pCont);

        m_oPalette.addColor( svt_color(0.90f, 0.90f, 0.90f ) );
        m_oPalette.addColor( svt_color(0.80f, 0.20f, 0.20f ) );
        m_oPalette.addColor( svt_color(0.25f, 0.25f, 1.00f ) );
        m_oPalette.addColor( svt_color(0.25f, 0.75f, 0.75f ) );
        m_oPalette.addColor( svt_color(0.80f, 0.80f, 0.00f ) );
        m_oPalette.addColor( svt_color(0.50f, 0.50f, 0.20f ) );
        m_oPalette.addColor( svt_color(0.60f, 0.60f, 0.60f ) );
    }
    virtual ~svt_volume_compact()
    {
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

                m_pCont->delAll();
                m_pCont->rebuildDL();

                int dims[3];
                dims[0] = this->getVolume().getSizeX();
                dims[1] = this->getVolume().getSizeY();
                dims[2] = this->getVolume().getSizeZ();

                m_iTriangles = 0;

                polygonise(dims);

                svt_redraw();

                m_oMCSema.V();

            } else {

                this->rebuildDL();
                svt_redraw();

            }
        }

    }

    /**
     * Get the container
     */
    svt_container* getCont()
    {
        return m_pCont;
    };

    /**
     * Get the number of triangles
     */
    int getTriNumber()
    {
        return m_iTriangles;
    };

protected:

    inline T ABS(T x)
    {
        if ((x) > 0)
            return (x);
        else
            return -(x);
    }

    /**
     * Add a vertex to our internal vertex array
     */
    inline int addCompVert( unsigned int iX, unsigned int iY, unsigned int iZ, float fX, float fY, float fZ )
    {
        int iVert = m_pCompact->at( iX, iY, iZ );

        // there is no vertex yet, at this position
        if ( iVert == -1 )
        {
            svt_c4ub_n3f_v3f p3D;
            p3D.m_oV[0] = fX;
            p3D.m_oV[1] = fY;
            p3D.m_oV[2] = fZ;
            m_pCont->addVertexMF( p3D );
            m_aCompactCount.push_back( 1 );
            m_pCompact->setAt( iX, iY, iZ, m_aCompactCount.size() -1 );
            return m_aCompactCount.size() -1;
        }

        // OK, there is already a vertex there
        m_aCompactCount[ iVert ]++;
        m_pCont->getVert( iVert ).m_oV[0] += fX;
        m_pCont->getVert( iVert ).m_oV[1] += fY;
        m_pCont->getVert( iVert ).m_oV[2] += fZ;

        return iVert;
    };

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
    inline int VertexInterp(Xyz p1, Xyz p2)
    {
	float mu;
        Point3D pt;

	if (ABS(m_fRenderIsoLevel - p1.val) < CEPS) // p1 is nearly on the isosurface
	{
	    pt.x = p1.p.x;
	    pt.y = p1.p.y;
            pt.z = p1.p.z;

            return addCompVert( p1.p.x, p1.p.y, p1.p.z, pt.x, pt.y, pt.z );

        }
        if (ABS(m_fRenderIsoLevel - p2.val) < CEPS) // p2 is nearly on the isosurface
	{
	    pt.x = p2.p.x;
	    pt.y = p2.p.y;
            pt.z = p2.p.z;

            return addCompVert( p2.p.x, p2.p.y, p2.p.z, pt.x, pt.y, pt.z );

        }
        if (ABS(p2.val - p1.val) < CEPS)// p1 and p2  is nearly on the isosurface
	{
	    pt.x = p1.p.x;
	    pt.y = p1.p.y;
            pt.z = p1.p.z;

            return addCompVert( p1.p.x, p1.p.y, p1.p.z, pt.x, pt.y, pt.z );

        }

        mu = (float)(m_fRenderIsoLevel - p1.val) / (p2.val - p1.val);
        pt.x = p1.p.x + mu * (p2.p.x - p1.p.x);
        pt.y = p1.p.y + mu * (p2.p.y - p1.p.y);
        pt.z = p1.p.z + mu * (p2.p.z - p1.p.z);

        if (mu < 0.5)
            return addCompVert( p1.p.x, p1.p.y, p1.p.z, pt.x, pt.y, pt.z );
        else
            return addCompVert( p2.p.x, p2.p.y, p2.p.z, pt.x, pt.y, pt.z );
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

    void polygonise(int *dims)
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
	int vert_list[12];            // List of interpolated cut-points

        unsigned int iTriCount = 0;
        bool bProgress = false;
        T fValue;

        m_pCompact = new svt_volume<unsigned long>( svt_volumeRenderer<T>::m_rVolume.getSizeX(), svt_volumeRenderer<T>::m_rVolume.getSizeY(), svt_volumeRenderer<T>::m_rVolume.getSizeZ(), -1 );
        m_pCont->beginGloballyColoredTriangles();

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
                            m_pCont->endGloballyColoredTriangles();
                            m_aCompactCount.clear();

                            if (bProgress)
                                svt_exception::ui()->progressPopdown();

                            this->rebuildDL();
                            svt_redraw();

                            m_bRecalcMesh = true;
                            return;
                        }

                        fValue = svt_volumeRenderer<T>::m_rVolume.at(x,y,z);

                        // vertex 0 000
                        grid[0].val = fValue;
                        grid[0].p.x = x;
                        grid[0].p.y = y;
                        grid[0].p.z = z;
                        // vertex 1 100
                        grid[1].val = svt_volumeRenderer<T>::m_rVolume.at(x+m_iRenderStep,y,z);
                        grid[1].p.x = (x+m_iRenderStep);
                        grid[1].p.y = y;
                        grid[1].p.z = z;
                        // vertex 2 010
                        grid[3].val = svt_volumeRenderer<T>::m_rVolume.at(x,y+m_iRenderStep,z);
                        grid[3].p.x = x;
                        grid[3].p.y = (y+m_iRenderStep);
                        grid[3].p.z = z;
                        // vertex 3 110
                        grid[2].val = svt_volumeRenderer<T>::m_rVolume.at(x+m_iRenderStep,y+m_iRenderStep,z);
                        grid[2].p.x = (x+m_iRenderStep);
                        grid[2].p.y = (y+m_iRenderStep);
                        grid[2].p.z = z;
                        // vertex 4 001
                        grid[4].val = svt_volumeRenderer<T>::m_rVolume.at(x,y,z+m_iRenderStep);
                        grid[4].p.x = x;
                        grid[4].p.y = y;
                        grid[4].p.z = (z+m_iRenderStep);
                        // vertex 5 101
                        grid[5].val = svt_volumeRenderer<T>::m_rVolume.at(x+m_iRenderStep,y,z+m_iRenderStep);
                        grid[5].p.x = (x+m_iRenderStep);
                        grid[5].p.y = y;
                        grid[5].p.z = (z+m_iRenderStep);
                        // vertex 5 011
                        grid[7].val = svt_volumeRenderer<T>::m_rVolume.at(x,y+m_iRenderStep,z+m_iRenderStep);
                        grid[7].p.x = x;
                        grid[7].p.y = (y+m_iRenderStep);
                        grid[7].p.z = (z+m_iRenderStep);
                        // vertex 6 111
                        grid[6].val = svt_volumeRenderer<T>::m_rVolume.at(x+m_iRenderStep,y+m_iRenderStep,z+m_iRenderStep);
                        grid[6].p.x = (x+m_iRenderStep);
                        grid[6].p.y = (y+m_iRenderStep);
                        grid[6].p.z = (z+m_iRenderStep);

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

                            // check whether edge 0 is cut
                            if (edge_table[cubeindex] & 1)
                                vert_list[0]=VertexInterp(grid[0],grid[1]);

                            // check whether edge 1 is cut
                            if (edge_table[cubeindex] & 2)
                                vert_list[1]=VertexInterp(grid[1],grid[2]);

                            // check whether edge 2 is cut
                            if (edge_table[cubeindex] & 4)
                                vert_list[2]=VertexInterp(grid[2],grid[3]);

                            // check whether edge 3 is cut
                            if (edge_table[cubeindex] & 8)   
                                vert_list[3]=VertexInterp(grid[3],grid[0]);

                            // check whether edge 4 is cut
                            if (edge_table[cubeindex] & 16)   
                                vert_list[4]=VertexInterp(grid[4],grid[5]);

                            // check whether edge 5 is cut
                            if (edge_table[cubeindex] & 32)  
                                vert_list[5]=VertexInterp(grid[5],grid[6]);

                            // check whether edge 6 is cut
                            if (edge_table[cubeindex] & 64)   
                                vert_list[6]=VertexInterp(grid[6],grid[7]);

                            // check whether edge 7 is cut
                            if (edge_table[cubeindex] & 128)  
                                vert_list[7]=VertexInterp(grid[7],grid[4]);

                            // check whether edge 8 is cut
                            if (edge_table[cubeindex] & 256)  
                                vert_list[8]=VertexInterp(grid[0],grid[4]);

                            // check whether edge 9 is cut
                            if (edge_table[cubeindex] & 512)  
                                vert_list[9]=VertexInterp(grid[1],grid[5]);

                            // check whether edge 10 is cut
                            if (edge_table[cubeindex] & 1024)  
                                vert_list[10]=VertexInterp(grid[2],grid[6]);

                            // check whether edge 11 is cut
                            if (edge_table[cubeindex] & 2048)  
                                vert_list[11]=VertexInterp(grid[3],grid[7]);

                            // i is the run-variable that runs through the lines of tri_table.
                            // The for-loop stops as soon as a -1, indicating that no triangle points
                            // will follow, occur
                            for (i=0; tri_table[cubeindex][i] !=-1;i+=3)
                            {
                                if ( vert_list[tri_table[cubeindex][i]] != vert_list[tri_table[cubeindex][i+1]] && vert_list[tri_table[cubeindex][i+1]] != vert_list[tri_table[cubeindex][i+2]] && vert_list[tri_table[cubeindex][i]] != vert_list[tri_table[cubeindex][i+2]])
                                {
                                    m_pCont->addFace(
                                                     vert_list[tri_table[cubeindex][i]],
                                                     vert_list[tri_table[cubeindex][i+1]],
                                                     vert_list[tri_table[cubeindex][i+2]]
                                                    );

                                    iTriCount++;

                                    if (iTriCount == 200000)
                                    {
                                        svt_exception::ui()->progressPopup("Marching cube", 0, dims[2] - m_iRenderStep);
                                        bProgress = true;
                                    }
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

        delete m_pCompact;

        //
        // Now we have to output the faces
        //
        unsigned int iVerts = m_aCompactCount.size();

        svt_c4ub_n3f_v3f oVert;

        Real32 fRed   = 255.0f;
        Real32 fGreen = 0.0f;
        Real32 fBlue  = 0.0f;
        Real32 fAlpha = 255.0f;

        if (this->getProperties() && this->getProperties()->getColor())
        {
            fRed   = this->getProperties()->getColor()->getR() * 255.0f;
            fGreen = this->getProperties()->getColor()->getG() * 255.0f;
            fBlue  = this->getProperties()->getColor()->getB() * 255.0f;
            fAlpha = this->getProperties()->getColor()->getTransparency() * 255.0f;
        }

        // scale the vertices first
        Real32 fScaleX = 1.0f / (this->getVolume().getSizeX()-1);
        Real32 fScaleY = 1.0f / (this->getVolume().getSizeY()-1);
        Real32 fScaleZ = 1.0f / (this->getVolume().getSizeZ()-1);
        for( unsigned int i=0; i<iVerts; i++ )
        {
            m_pCont->getVert(i).m_oV[0] = (m_pCont->getVert(i).m_oV[0] / m_aCompactCount[i]) * fScaleX;
            m_pCont->getVert(i).m_oV[1] = (m_pCont->getVert(i).m_oV[1] / m_aCompactCount[i]) * fScaleY;
            m_pCont->getVert(i).m_oV[2] = (m_pCont->getVert(i).m_oV[2] / m_aCompactCount[i]) * fScaleZ;
            m_pCont->getVert(i).m_oC[0] = fRed;
            m_pCont->getVert(i).m_oC[1] = fGreen;
            m_pCont->getVert(i).m_oC[2] = fBlue;
            m_pCont->getVert(i).m_oC[3] = fAlpha;
        }

        // compute the normals
        unsigned int iA, iB, iC;
        svt_v3f oL1, oL2, oN;
        unsigned int iFaces = m_pCont->getFaceCount();
        for( unsigned int i=0; i<iFaces; i+=3 )
        {
            iA = m_pCont->getFaceIndex(i+2);
            iB = m_pCont->getFaceIndex(i+1);
            iC = m_pCont->getFaceIndex(i);

            oL1[0] = m_pCont->getVert( iB ).m_oV[0] - m_pCont->getVert( iA ).m_oV[0];
            oL1[1] = m_pCont->getVert( iB ).m_oV[1] - m_pCont->getVert( iA ).m_oV[1];
            oL1[2] = m_pCont->getVert( iB ).m_oV[2] - m_pCont->getVert( iA ).m_oV[2];

            oL2[0] = m_pCont->getVert( iC ).m_oV[0] - m_pCont->getVert( iB ).m_oV[0];
            oL2[1] = m_pCont->getVert( iC ).m_oV[1] - m_pCont->getVert( iB ).m_oV[1];
            oL2[2] = m_pCont->getVert( iC ).m_oV[2] - m_pCont->getVert( iB ).m_oV[2];

            oN[0] = (oL1[1] * oL2[2]) - (oL1[2] * oL2[1]);
            oN[1] = (oL1[2] * oL2[0]) - (oL1[0] * oL2[2]);
            oN[2] = (oL1[0] * oL2[1]) - (oL1[1] * oL2[0]);

            m_pCont->getVert(iA).m_oN[0] += oN[0];
            m_pCont->getVert(iA).m_oN[1] += oN[1];
            m_pCont->getVert(iA).m_oN[2] += oN[2];
            m_pCont->getVert(iB).m_oN[0] += oN[0];
            m_pCont->getVert(iB).m_oN[1] += oN[1];
            m_pCont->getVert(iB).m_oN[2] += oN[2];
            m_pCont->getVert(iC).m_oN[0] += oN[0];
            m_pCont->getVert(iC).m_oN[1] += oN[1];
            m_pCont->getVert(iC).m_oN[2] += oN[2];
        }
        for( unsigned int i=0; i<iVerts; i++ )
            m_pCont->getVert(i).normalizeNorm();

        // delete internal arrays
        m_aCompactCount.clear();

        m_pCont->endGloballyColoredTriangles();

        m_iTriangles = iTriCount;
    }
};

#endif
