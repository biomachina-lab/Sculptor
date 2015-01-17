/***************************************************************************
                          svt_aliasobj
			  ------------
    begin                : 08/22/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ALIASOBJ_H
#define SVT_ALIASOBJ_H

#include <svt_basics.h>
#include <svt_stlVector.h>
#include <svt_stlStack.h>
#include <svt_vector4.h>
#include <svt_matrix4.h>
#include <svt_color.h>

class svt_face
{
    // vertex index
    int m_aVI[3];
    // normal index
    int m_aNI[3];

public:

    /**
     * Constructor
     */
    svt_face()
    {
        m_aVI[0] = -1;
        m_aVI[1] = -1;
        m_aVI[2] = -1;
    };
    /**
     * Copy Constructor
     */
    svt_face(const svt_face& rOther)
    {
        m_aVI[0] = rOther.m_aVI[0];
        m_aVI[1] = rOther.m_aVI[1];
        m_aVI[2] = rOther.m_aVI[2];

        m_aNI[0] = rOther.m_aNI[0];
        m_aNI[1] = rOther.m_aNI[1];
        m_aNI[2] = rOther.m_aNI[2];
    };

    // set vertex index for the first corner point of the face
    void setVI_A( int i )
    {
	m_aVI[0] = i;
    };
    // set vertex index for the second corner point of the face
    void setVI_B( int i )
    {
	m_aVI[1] = i;
    };
    // set vertex index for the third corner point of the face
    void setVI_C( int i )
    {
	m_aVI[2] = i;
    };

    // set the normal index for the first corner point of the face
    void setNI_A( int i )
    {
	m_aNI[0] = i;
    };
    // set the normal index for the second corner point of the face
    void setNI_B( int i )
    {
	m_aNI[1] = i;
    };
    // set the normal index for the third corner point of the face
    void setNI_C( int i )
    {
	m_aNI[2] = i;
    };

    // get the normal index for the first corner point of the face
    int getNI_A( )
    {
	return m_aNI[0];
    };
    // get the normal index for the second corner point of the face
    int getNI_B( )
    {
	return m_aNI[1];
    };
    // get the normal index for the third corner point of the face
    int getNI_C( )
    {
	return m_aNI[2];
    };

    int operator[](unsigned int i)
    {
	return m_aVI[i];
    };

    // write the face information to cout
    void writeToCout()
    {
        cout << "f " << m_aVI[0] << "//" << m_aNI[0] << " ";
        cout         << m_aVI[1] << "//" << m_aNI[1] << " ";
        cout         << m_aVI[2] << "//" << m_aNI[2] << endl;
    };
};


/** 
 * This is an internal helper class for the Alias/Wavefront OBJ export.
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_aliasobj
{
    // vertex array
    vector< svt_vector4<Real32> > m_aVertices;
    // normal array
    vector< svt_vector4<Real32> > m_aNormals;
    // face array
    vector< svt_face > m_aFaces;

    // transformation matrix stack
    stack< svt_matrix4<Real32> > m_aStack;

    // color stack
    vector< svt_color > m_aColors;
    // color changes
    vector< unsigned int > m_aColorChange;
    // when to change the color
    vector< unsigned int > m_aColorChangePos;

public:

    /**
     * Constructor
     */
    svt_aliasobj();

    /**
     * Add a mesh
     * \param aVertices the new vertices that should be added
     * \param aNormals the new normal vectors that should be added
     * \param aFaces the new face indices that should be added
     */
    void addMesh( vector< svt_vector4< Real32 > > aVertices, vector< svt_vector4< Real32 > > aNormals, vector< svt_face > aFaces );

    /**
     * Output the object as a OBJ file (to stdout)
     */
    void drawOBJ();
    /**
     * Output the materials used
     */
    void drawMAT();


    /**
     * Set color.
     * The new color will be used from now on for all the outputs until a new color is set.
     */
    void setColor( svt_color& rColor );

    /**
     * Set color.
     * The new color will be used from now on for all the outputs until a new color is set.
     */
    void setColor( svt_color& rColor, unsigned int iFacePos );

    /**
     * Push a new transformation matrix on the stack
     */
    void pushMatrix( svt_matrix4<Real32> fMatrix );
    /**
     * Push a new transformation matrix on the stack
     */
    void pushMatrixLeft( svt_matrix4<Real32> fMatrix );
    /**
     * Pop the last transformation matrix from the stack
     */
    void popMatrix( );
    /**
     * Get the current transformation matrix
     */
    svt_matrix4<Real32> getMatrix();

    /**
     * Reset the alias object - delete all triangles, vertices, normals and the matrix stack
     */
    void reset();

    /**
     * Clean mesh - before streaming out the faces, vertices and normals, we have to remove identical vertices in order to obtain a correct topology of our meshes.
     */
    void cleanMesh();

    /**
     * Get the minimal coordinates of the point cloud - it will return a vector that has in each dimension the information about the minimal
     * coordinate it has found in the cloud.
     */
    svt_vector4<Real64> getMinCoord();

    /**
     * Get the maximal coordinates of the point cloud - it will return a vector that has in each dimension the information about the maximal
     * coordinate it has found in the cloud.
     */
    svt_vector4<Real64> getMaxCoord();

    /**
     * Find identical vertices.
     * \return Return a vector of indices. If the value/index at position i is i, then this is a unique vertex. Otherwise the value/index will point to the identical twin.
     */
    vector< unsigned int > findIdenticals(bool bShowProgress);
};


#endif
