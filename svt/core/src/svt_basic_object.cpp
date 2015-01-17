/***************************************************************************
                          svt_basic_object.cpp
                          -------------------
    begin                : 6/14/2003
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basic_object.h>
#include <svt_iostream.h>

extern bool g_bPoint;

/**
 * Constructor
 * \param fRed red color component
 * \param fGreen green color component
 * \param fBlue blue color component
 */
svt_basic_object::svt_basic_object( Real32 fRed, Real32 fGreen, Real32 fBlue )
{
    setColor( fRed, fGreen, fBlue );
}

/**
 * draw the object (OpenGL)
 * \param iWin index of window (opengl context)
 * \param rCont reference to parent container object
 */
void svt_basic_object::drawGL(int iWin, svt_basic_container&)
{
    if (m_aMatAmbient[0] != -1.0f)
    {
	// set color (with lighting)
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, &m_aMatAmbient[0]);
	// set color (no lighting)
	glColor3f( m_aMatAmbient[0], m_aMatAmbient[1], m_aMatAmbient[2] );
    }

    // point mode?
    if (g_bPoint != m_bPoint)
    {
        if (m_bPoint == true)
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        else
            glPolygonMode(GL_FRONT, GL_FILL);

        g_bPoint = m_bPoint;
    }
};

/**
 * draw the object (OpenInventor)
 */
void svt_basic_object::drawIV()
{
    // coloring
    if (m_aMatAmbient[0] != -1.0f)
    {
	cout << "    Material {" << endl;
	cout << "      ambientColor   0.0 0.0 0.0" << endl;
	cout << "      diffuseColor   " << m_aMatAmbient[0] << " " << m_aMatAmbient[1] << " " << m_aMatAmbient[2] << endl;
        cout << "      specularColor  1.0 1.0 1.0" << endl;
	cout << "      shininess      0.1" << endl;
        cout << "      transparency   0.0" << endl;
	cout << "    }" << endl;
    }
}

/**
 * draw the object (PovRay)
 */
void svt_basic_object::drawPOV()
{
    cout << "      pigment {" << endl;
    cout << "        color rgb< " << m_aMatAmbient[0] << ", " << m_aMatAmbient[1] << ", " << m_aMatAmbient[2] << " >" << endl;
    cout << "      }" << endl;
    cout << "      finish {" << endl;
    cout << "        ambient 0.00" << endl;
    cout << "        diffuse 1.0  brilliance 1.50" << endl;
    cout << "        specular 1.0 roughness 0.005" << endl;
    cout << "      }" << endl;
}

/**
 * draw the object (Yafray)
 */
void svt_basic_object::drawYafray()
{
    // coloring
    if (m_aMatAmbient[0] != -1.0f)
    {
	cout << "    <shader type = \"generic\" name = \"shader." << (this) << "\">" << endl;
	cout << "      <attributes>" << endl;
	cout << "        <color  r=\"" << m_aMatAmbient[0] << "\" g=\"" << m_aMatAmbient[1] << "\" b=\"" << m_aMatAmbient[2] << "\" />" << endl;
	//cout << "        <reflected  r=\"" << pDiffuse[0] << "\" g=\"" << pDiffuse[1] << "\" b=\"" << pDiffuse[2] << "\" />" << endl;
	cout << "        <specular r=\"1.0\" g=\"1.0\" b=\"1.0\" />" << endl;
	cout << "      </attributes>" << endl;
	cout << "    </shader>" << endl;
    }
}

/**
 * draw the object (Alias/Wavefront)
 */
void svt_basic_object::drawOBJ()
{

}
