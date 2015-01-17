/***************************************************************************
                          svt_container
                          -------------
    begin                : 12/05/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_BASIC_CONTAINER_H
#define SVT_BASIC_CONTAINER_H

#include <svt_basics.h>
#include <svt_stlVector.h>
#include <svt_node.h>
#include <svt_vertinterl.h>
#include <svt_vbo.h>

/**
 * \brief This is a simple container class for primitive objects like points, lines, spheres and cylinders. One can also directly add triangles and construct more complex objects.
 * \author Stefan Birmanns
 * The idea behind this class is that it stores basic primitives like spheres, which are used in
 * huge quantities in scientific visualizations. Compared to full scenegraph objects, they lack a lot of properties, but are rendered in the fastest possible way.
 * Right now the implementation can render them using interleaved VARs, CVAs or VBOs.
 */
class DLLEXPORT svt_container : public svt_node
{

protected:

    // This array holds all the colors and vertices for lines
    vector< svt_c3f_v3f >      m_aLineVertArray;
    // This array holds all the colors and vertices for points
    vector< svt_c3f_v3f >      m_aPointVertArray;

    // This array holds all the vertices. They are stored as c4ub_n3f_v3f interleaved vectors.
    vector< svt_c4ub_n3f_v3f > m_aVertArray;
    vector< unsigned int >     m_aVertIndices;
    // Start and End indices for the Triangles that are supposed to receive global color information
    vector< unsigned int >     m_aGloballyColoredTriStartEnd;
    // Start and End indices for the Triangles
    vector< unsigned int >     m_aTriStartEnd;
    // Start and End indices for the TriangleStrips
    vector< unsigned int >     m_aTriStripStartEnd;
    // Start and End indices for the TriangleFans
    vector< unsigned int >     m_aTriFanStartEnd;
    // Start and End indices for the Quads
    vector< unsigned int >     m_aQuadStartEnd;

    // static vertex array with coordinates of a sphere
    static vector< svt_c4ub_n3f_v3f > s_aSphereCoords;
    static vector< unsigned int > s_aSphereFaces;

    svt_vbo m_oVertexBuffer;
    svt_vbo m_oIndexBuffer;

    bool m_bVBO_Generated;

public:
    /**
     * Constructor
     */
    svt_container( svt_pos* pPos =NULL, svt_properties* pProp =NULL );
    /**
     * Destructor
     * Attention: The desctructor deletes all the elements of the container!!
     */
    virtual ~svt_container();

    /**
     * delete all elements
     */
    void delAll();

    /**
     * append another container
     * \param oContainer reference to the other container that should get appended
     */
    //void append(svt_container& oContainer);

    //inline void operator<<(svt_container& oContainer) { append(oContainer); };

    /**
     * draw the content of the container (OpenGL)
     */
    virtual void drawGL();

    /**
     * print the complete yafray code for the object (incl. properties)
     */
    //virtual void outputYafray();

    /**
     * draw the content of the container (OpenInventor)
     */
    //virtual void drawIV();
    /**
     * draw the content of the container (PovRay)
     */
    //virtual void drawPOV();

    /**
     * draw the content of the container (Yafray)
     */
    //virtual void drawYafray();

    /**
     * draw the content of the container (Alias/Wavefront)
     */
    virtual void drawOBJ();

    /**
     * Add a line to the line/vertex buffer
     */
    inline void addLine( const svt_c3f_v3f rVecA, const svt_c3f_v3f rVecB )
    {
	m_aLineVertArray.push_back( rVecA );
	m_aLineVertArray.push_back( rVecB );
    };

    /**
     * Add a point to the line/vertex buffer
     */
    inline void addPoint( const svt_c3f_v3f oVec )
    {
	m_aPointVertArray.push_back( oVec );
    };

    /**
     * Add a vertex, normal, color (used for everything except lines and points). It is assumed that the vertex is only part of a single face. DO NOT CALL addFace for this vertex!
     */
    inline void addVertex( const svt_c4ub_n3f_v3f rVert )
    {
        m_aVertIndices.push_back( m_aVertArray.size() );
	m_aVertArray.push_back( rVert);
    };

    /**
     * Add a vertex, normal, color (used for everything except lines and points). This vertex can be part of multiple faces. One also has to call addFace!
     */
    inline unsigned int addVertexMF( const svt_c4ub_n3f_v3f rVert )
    {
	m_aVertArray.push_back( rVert);

        return m_aVertArray.size() - 1;
    };
    /**
     * Add a vertex, normal, color (used for everything except lines and points). This vertex can be part of multiple faces. One also has to call addFace!
     */
    inline unsigned int addVerticesMF( vector<svt_c4ub_n3f_v3f>& rOther )
    {
        m_aVertArray.insert( m_aVertArray.end(), rOther.begin(), rOther.end() );

        return m_aVertArray.size() - 1;
    };

    /**
     * Return a vertex from the vertex array
     */
    inline svt_c4ub_n3f_v3f& getVert( unsigned int i )
    {
        return m_aVertArray[i];
    }

    /**
     * Add a face.
     */
    inline void addFace( unsigned int iIndexA, unsigned int iIndexB, unsigned int iIndexC )
    {
        m_aVertIndices.push_back( iIndexA );
        m_aVertIndices.push_back( iIndexB );
        m_aVertIndices.push_back( iIndexC );
    };
    /**
     * Add a face.
     */
    inline void addFace( unsigned int iIndex )
    {
        m_aVertIndices.push_back( iIndex );
    };
    /**
     * Add a vector with face indexes
     */
    inline void addFaces( vector<unsigned int>& rIndexes )
    {
        m_aVertIndices.insert( m_aVertIndices.end(), rIndexes.begin(), rIndexes.end() );
    };

    /**
     * Get a single face
     */
    inline unsigned int getFaceIndex( unsigned int iIndex )
    {
        return m_aVertIndices[iIndex];
    }
    /**
     * Get number of faces
     */
    inline unsigned int getFaceCount( )
    {
        return m_aVertIndices.size();
    }

    /**
     * Start a set of triangles. After that command, please add vertices with addVertex. When done, call endTriangles.
     */
    void beginTriangles( );
    /**
     * End a triangle
     */
    void endTriangles( );

    /**
     * Start a set of triangles. After that command, please add vertices with addVertex. When done,
     * call endGloballyColoredTriangles. Differs from beginTriangles() in that it is meant for triangles
     * that receive global color information: color client state is disabled when these are rendered
     */
    void beginGloballyColoredTriangles( );
    /**
     * End a triangle
     */
    void endGloballyColoredTriangles( );

    /**
     * Start a triangle strip. See startTriangles.
     */
    void beginTriStrip( );
    /**
     * End a triangle strip. See endTriangles.
     */
    void endTriStrip( );

    /**
     * Start a triangle fan. See beginTriangles.
     */
    void beginTriFan( );
    /**
     * End a triangle fan. See endTriangles.
     */
    void endTriFan( );

    /**
     * Start a set of quads. See startTriangles.
     */
    void beginQuads();
    /**
     * End a set of quads. See endTriangles.
     */
    void endQuads();

    /**
     * Add a sphere.
     */
    void addSphere( const Real64& fX, const Real64& fY, const Real64& fZ, const Real64& rRadius, const svt_color& rColor );
    /**
     * Add a dotted sphere.
     */
    void addDottedSphere( const Real64& fX, const Real64& fY, const Real64& fZ, const Real64& rRadius, const svt_color& rColor );

    /**
     * Create a Conical Frustum(TruncatedCone) between two vectors (creates trianglestrips).
     * Used to add Cylinders and Cones
     */
    void addConicalFrustum(svt_vector4<Real32> oVecA, svt_vector4<Real32> oVecB, Real32 fRadiusA, Real32 fRadiusB, svt_color& rColor, int m, bool bClosedBase=false, bool bClosedTop=false);
    
    /**
     * Add a cylinder between two vectors (creates trianglestrips).
     */
    void addCylinder(svt_vector4<Real32> oVecA, svt_vector4<Real32> oVecB, Real32 fRadius, svt_color& rColor, int m = 4);

    /**
     * Add a cylinder with capped ends between two vectors (creates trianglestrips).
     */
    void addCappedCylinder(svt_vector4<Real32> oVecA, svt_vector4<Real32> oVecB, Real32 fRadius, svt_color& rColor, int m = 4);
    
    /**
     * Create a Cone between two vectors (creates trianglestrips).
     */
    void addCone(svt_vector4<Real32> oVecA, svt_vector4<Real32> oVecB, Real32 fRadius, svt_color& rColor, int m);

protected:

    /**
     * Create a sphere with a vertain resolution (creates trianglestrips).
     */
    void createSphere(int n);
    /**
     * Create a sphere
     */
    void createSphere();
    /**
     * refine sphere model by subdividing the triangles
     */
    void refineSphere();

    /**
     * Are the normals of a certain face consistent with the winding
     */
    bool normalCheck( unsigned int iA, unsigned int iB, unsigned int iC );
};

#endif
