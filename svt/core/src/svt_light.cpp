/***************************************************************************
                          svt_light.cpp  -  description
                             -------------------
    begin                : Jul 21 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_core.h>
#include <svt_light.h>
#include <svt_lightsource.h>
#include <svt_init.h>

GLint svt_light::m_iMaxLights = -1;

/**
 * Constructor
 */
svt_light::svt_light() : svt_node_noautodl()
{
};

/**
 * Destructor
 */
svt_light::~svt_light()
{
};

/**
 * draw all attached lightsources
 * \param iMode mode for the drawing (e.g. SVT_MODE_GL, SVT_MODE_VRML)
 */
void svt_light::draw(int iMode)
{
    svt_lightsource* pNode;

    switch(iMode)
    {
    case SVT_MODE_POV:
        pNode = (svt_lightsource*)getSon();
        while(pNode != NULL)
        {
	    const Real32* pAmbient = pNode->getAmbient();
	    cout << " light_source" << endl;
	    cout << " {" << endl;
	    cout << "   < " << pNode->getTransformation()->getTranslationX() << ", " << pNode->getTransformation()->getTranslationY() << ", " << (1.0f * pNode->getTransformation()->getTranslationZ()) << " >" << endl;
	    cout << "   color rgb< " << pAmbient[0] * 10.0 << ", " << pAmbient[1] * 10.0 << ", " << pAmbient[2] * 10.0 << " >" << endl;
	    cout << " }" << endl;

	    pNode = (svt_lightsource*)pNode->getNext();

            cout << endl;
	}
	break;

    case SVT_MODE_IV:

	cout << "  Separator {" << endl;
	cout << "    Info {" << endl;
	cout << "      string \"Lightsources.\"" << endl;
	cout << "    }" << endl;

        pNode = (svt_lightsource*)getSon();
        while(pNode != NULL)
        {
	    const Real32* pAmbient = pNode->getAmbient();
	    cout << "    PointLight {" << endl;
	    cout << "      on TRUE" << endl;
	    cout << "      intensity 1" << endl;
	    cout << "      color " << pAmbient[0] << " " << pAmbient[1] << " " << pAmbient[2] << endl;
            cout << "      location ";
            cout << pNode->getTransformation()->getTranslationX() << " ";
            cout << pNode->getTransformation()->getTranslationY() << " ";
	    cout << pNode->getTransformation()->getTranslationZ() << " " << endl;
	    cout << "    }" << endl;

	    pNode = (svt_lightsource*)pNode->getNext();

            cout << endl;
	}

	cout << "  }" << endl;
        break;

    case SVT_MODE_YAFRAY:

        pNode = (svt_lightsource*)getSon();
        while(pNode != NULL)
        {
	    //const Real32* pAmbient = pNode->getAmbient();

	    cout << "<light type = \"sunlight\" name= \"mainLight\" power= \"1.000000\" cast_shadows=\"on\" >" << endl;
	    cout << "<from x=\"" << pNode->getTransformation()->getTranslationX() << "\" y=\"" << pNode->getTransformation()->getTranslationY() << "\" z=\"" << pNode->getTransformation()->getTranslationZ() << "\" />" << endl;
	    cout << "<color r=\"1.0\" g=\"1.0\" b=\"1.0\" />" << endl;
	    //cout << "<color r=\"" << pAmbient[0] << "\" g=\"" << pAmbient[1] << "\" b=\"" << pAmbient[2] << "\" /> " << endl;
	    cout << "</light>" << endl;

	    pNode = (svt_lightsource*)pNode->getNext();
            cout << endl;
	}

        break;

    default:
    case SVT_MODE_GL:

        glPushMatrix();
        drawGL();

        svt_node* pNode = getSon();

        while(pNode != NULL)
        {
            pNode->drawGL();
            pNode = pNode->getNext();
        }

        glPopMatrix();

        break;
    }
}

/**
 * draw the light
 */
void svt_light::drawGL()
{
    if (m_iMaxLights == -1)
    {
        glGetIntegerv(GL_MAX_LIGHTS, &m_iMaxLights);

        if (m_iMaxLights <= 0)
            svt_fatalError("svt_light: no lights available (something is wrong with your opengl implementation, there should be at least 8 lightsources)");

        m_pLightFlags = new int[m_iMaxLights];

        // all lightsources unbound
        for(int i=0;i<m_iMaxLights;i++)
            setBound(i, SVT_UNBOUND);
    }
}

/**
 * add a lightsource to the container
 * \param pLight pointer to svt_lightsource
 */
void svt_light::add(svt_lightsource* pLight)
{
    pLight->setLight(this);
    svt_node::add(pLight);
};
/**
 * del a lightsource from the container
 * \param pLight pointer to svt_lightsource
 */
void svt_light::del(svt_lightsource* pLight)
{
    setBound(pLight->getLightNumber(), SVT_UNBOUND);
    svt_node::del(pLight);
};

/**
 * find free light
 */
int svt_light::registerLightsource()
{
    if (m_iMaxLights == -1)
        return -1;

    int i=-1;
    for(i=0;i < m_iMaxLights;i++)
    {
        if (getBound(i) == SVT_UNBOUND)
        {
            setBound(i,SVT_BOUND);
            return i;
        }
    }
    return i;
}


/**
 * is this light free or bound to a lightsource
 * \param iIndex number of the lightsource
 * \param iFlag new status for the lightsource (e.g. SVT_BOUND)
 */
void svt_light::setBound(int iIndex, int iFlag)
{
    m_pLightFlags[iIndex] = iFlag;
};


/**
 * is this light free or bound to a lightsource?
 * \param iIndex number of the lightsource
 * \return status for the lightsource (e.g. SVT_BOUND)
 */
int svt_light::getBound(int iIndex)
{
    return m_pLightFlags[iIndex];
};
