/***************************************************************************
                          sculptor_lua
                          ------------
    begin                : 10/16/2009
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// sculptor includes
#include <sculptor_lua.h>
#include <sculptor_window.h>
#include <sculptor_prop_situs.h>
#include <sculptor_window_documentlist.h>
#include <sculptor_scene.h>
#include <sculptor_version.h>
#include <situs_document.h>
#include <sculptor_doc_marker.h>
// svt includes
#include <svt_powell_cc.h>
#include <svt_clustering_fam.h>
#include <svt_flexing_inverse_dist_weighting.h>
#include <svt_string.h>
// qt4 includes
#include <qregexp.h>
#include <QTextEdit>


extern void svtQtCallback();
extern sculptor_window* g_pWindow;

///////////////////////////////////////////////////////////////////////////////
// LUA BINDINGS: SPHERE
///////////////////////////////////////////////////////////////////////////////

/**
 * The sphere class is a very simple svt visualization node to mark positions
 */
sphere::sphere(lua_State *pState)
{
    m_pSphere = NULL;

    if (pState == NULL)
        return;

    if ( !lua_isnil( pState, -1 ) && !lua_isnil( pState, -2 ) && !lua_isnil( pState, -3 ) )
    {
        double fX = 1.0E-2 * luaL_checknumber(pState, -3);
        double fY = 1.0E-2 * luaL_checknumber(pState, -2);
        double fZ = 1.0E-2 * luaL_checknumber(pState, -1);

        svt_properties* m_pWhite = new svt_properties(new svt_color(0.8f,0.8f,0.8f));
        m_pSphere = new svt_sphere( new svt_pos(fX, fY, fZ), m_pWhite, 2.0E-2, 16,16);
    }
};
sphere::~sphere()
{
};

/**
 * Set sphere
 * \param pSphere pointer to svt_sphere object
 */
void sphere::set( svt_sphere* pSphere )
{
    m_pSphere = pSphere;
};
/**
 * Get Matrix
 * \return svt_matrix4 object
 */
svt_sphere* sphere::get()
{
    return m_pSphere;
};

/**
 * Is the given svt_node object a sphere?
 */
bool sphere::isSphere( svt_node* pSphere )
{
    return typeid( *pSphere ) == typeid( svt_sphere );
}

/**
 * Set the radius of the sphere.
 * Parameter: Number
 *
 * usage:
 * sph:setRadius( 15.0 )
 */
int sphere::setRadius(lua_State* pState)
{
    if (m_pSphere == NULL) return 0;

    double fRadius = luaL_checknumber(pState, -1);
    m_pSphere->setRadius( 1.0E-2 * fRadius );
    return 0;
};
/**
 * Set the color of the sphere.
 * Parameter: Number between 0.0 and 1.0 for red, green and blue.
 *
 * usage:
 * sph:setColor( 1.0, 0.0, 0.5 )
 */
int sphere::setColor(lua_State* pState)
{
    if (m_pSphere == NULL) return 0;

    if (m_pSphere->getProperties() == NULL)
        m_pSphere->setProperties( new svt_properties() );
    if (m_pSphere->getProperties()->getColor() == NULL)
        m_pSphere->getProperties()->setColor( new svt_color() );

    double fR = luaL_checknumber(pState, -3);
    double fG = luaL_checknumber(pState, -2);
    double fB = luaL_checknumber(pState, -1);

    m_pSphere->getProperties()->getColor()->setR( fR );
    m_pSphere->getProperties()->getColor()->setG( fG );
    m_pSphere->getProperties()->getColor()->setB( fB );
    return 0;
};
/**
 * Get the color of the sphere.
 * Returns: Numbers between 0.0 and 1.0 for red, green and blue.
 *
 * usage:
 * r,g,b = sph:getColor( )
 */
int sphere::getColor(lua_State* pState)
{
    if (m_pSphere == NULL) return 0;

    double fR = m_pSphere->getProperties()->getColor()->getR();
    double fG = m_pSphere->getProperties()->getColor()->getG();
    double fB = m_pSphere->getProperties()->getColor()->getB();

    lua_pushnumber(pState, fR );
    lua_pushnumber(pState, fG );
    lua_pushnumber(pState, fB );
    return 3;
};

/**
 * Get the position of the sphere.
 * Returns: Numbers (x,y,z)
 *
 * usage:
 * x,y,z = sph:getPos( )
 */
int sphere::getPos(lua_State* pState)
{
    if (m_pSphere == NULL) return 0;

    lua_pushnumber(pState, 1.0E2 * m_pSphere->getPos()->getX() );
    lua_pushnumber(pState, 1.0E2 * m_pSphere->getPos()->getY() );
    lua_pushnumber(pState, 1.0E2 * m_pSphere->getPos()->getZ() );
    return 3;
};
/**
 * Set the position of the sphere.
 * Parameters: Numbers (x,y,z)
 *
 * usage:
 * sph:setPos( x,y,z )
 */
int sphere::setPos(lua_State* pState)
{
    if (m_pSphere == NULL) return 0;

    m_pSphere->getPos()->setX( 1.0E-2 * luaL_checknumber(pState, -3) );
    m_pSphere->getPos()->setY( 1.0E-2 * luaL_checknumber(pState, -2) );
    m_pSphere->getPos()->setZ( 1.0E-2 * luaL_checknumber(pState, -1) );
    return 0;
};

/**
 * Get the radius of the sphere.
 * Returns: Number
 *
 * usage:
 * rad = sph:getRadius( )
 */
int sphere::getRadius(lua_State* pState)
{
    if (m_pSphere == NULL) return 0;

    lua_pushnumber(pState, 1.0E2 * m_pSphere->getRadius() );
    return 1;
};

/**
 * Set/get the x coordinate of the position of the atom
 * If no parameter is given, the function just returns a number, otherwise it will set the x coordinate using the parameter provided by the user.
 *
 * usage:
 * atm_a:x( 15.0 )
 * print( atm_a:x() )
 */
int sphere::x(lua_State* pState)
{
    if (m_pSphere == NULL) return 0;

    if (lua_isnumber( pState, -1 ))
    {
        double fX = luaL_checknumber(pState, -1);
        m_pSphere->getPos()->setX( 1.0E-2 * fX );
    }

    lua_pushnumber(pState, 1.0E2 * m_pSphere->getPos()->getX() );
    return 1;
};
/**
 * Set/get the y coordinate of the position of the atom
 * If no parameter is given, the function just returns a number, otherwise it will set the y coordinate using the parameter provided by the user.
 *
 * usage:
 * atm_a:y( 15.0 )
 * print( atm_a:y() )
 */
int sphere::y(lua_State* pState)
{
    if (m_pSphere == NULL) return 0;

    if (lua_isnumber( pState, -1 ))
    {
        double fY = luaL_checknumber(pState, -1);
        m_pSphere->getPos()->setY( 1.0E-2 * fY );
    }

    lua_pushnumber(pState, 1.0E2 * m_pSphere->getPos()->getY( ) );
    return 1;
};
/**
 * Set/get the z coordinate of the position of the atom
 * If no parameter is given, the function just returns a number, otherwise it will set the z coordinate using the parameter provided by the user.
 *
 * usage:
 * atm_a:z( 15.0 )
 * print( atm_a:z() )
 */
int sphere::z(lua_State* pState)
{
    if (m_pSphere == NULL) return 0;

    if (lua_isnumber( pState, -1 ))
    {
        double fZ = luaL_checknumber(pState, -1);
        m_pSphere->getPos()->setZ( 1.0E-2 * fZ );
    }

    lua_pushnumber(pState, 1.0E2 * m_pSphere->getPos()->getZ( ) );
    return 1;
};

const char sphere::className[] = "sphere";
const svt_luna<sphere>::RegType sphere::methods[] =
{
    { "x",            &sphere::x             },
    { "y",            &sphere::y             },
    { "z",            &sphere::z             },
    { "getRadius",    &sphere::getRadius     },
    { "setRadius",    &sphere::setRadius     },
    { "setColor",     &sphere::setColor      },
    { "getColor",     &sphere::getColor      },
    { "getPos",       &sphere::getPos        },
    { "setPos",       &sphere::setPos        },
    { 0,              0                      }
};

///////////////////////////////////////////////////////////////////////////////
// LUA BINDINGS: ARROW
///////////////////////////////////////////////////////////////////////////////

/**
 * The arrow class is a very simple svt visualization node to mark positions
 */
arrow::arrow(lua_State *pState)
{
    m_pArrow = NULL;

    if (pState == NULL)
        return;

    if ( !lua_isnil( pState, -1 ) && !lua_isnil( pState, -2 ) && !lua_isnil( pState, -3 ) )
    {
        double fX = 1.0E-2 * luaL_checknumber(pState, -3);
        double fY = 1.0E-2 * luaL_checknumber(pState, -2);
        double fZ = 1.0E-2 * luaL_checknumber(pState, -1);

        m_pArrow = new svt_arrow( new svt_pos(fX, fY, fZ), new svt_properties(new svt_color(0.8f,0.8f,0.8f)) );
        m_pArrow->setName("Lua Arrow");
    }
};
arrow::~arrow()
{
};

/**
 * Set arrow
 * \param pArrow pointer to svt_arrow object
 */
void arrow::set( svt_arrow* pArrow )
{
    m_pArrow = pArrow;
};
/**
 * Get Matrix
 * \return svt_matrix4 object
 */
svt_arrow* arrow::get()
{
    return m_pArrow;
};

/**
 * Is the given svt_node object a arrow?
 */
bool arrow::isArrow( svt_node* pArrow )
{
    return typeid( *pArrow ) == typeid( svt_arrow );
}

/**
 * Set the radius of the arrow.
 * Parameter: Number
 *
 * usage:
 * sph:setRadius( 15.0 )
 */
int arrow::setRadius(lua_State* pState)
{
    if (m_pArrow == NULL) return 0;

    double fRadius = luaL_checknumber(pState, -1);
    m_pArrow->setRadius( 1.0E-2 * fRadius );
    return 0;
};
/**
 * Set the color of the arrow.
 * Parameter: Number between 0.0 and 1.0 for red, green and blue.
 *
 * usage:
 * sph:setColor( 1.0, 0.0, 0.5 )
 */
int arrow::setColor(lua_State* pState)
{
    if (m_pArrow == NULL) return 0;

    if (m_pArrow->getProperties() == NULL)
        m_pArrow->setProperties( new svt_properties() );
    if (m_pArrow->getProperties()->getColor() == NULL)
        m_pArrow->getProperties()->setColor( new svt_color() );

    double fR = luaL_checknumber(pState, -3);
    double fG = luaL_checknumber(pState, -2);
    double fB = luaL_checknumber(pState, -1);

    m_pArrow->getProperties()->getColor()->setR( fR );
    m_pArrow->getProperties()->getColor()->setG( fG );
    m_pArrow->getProperties()->getColor()->setB( fB );
    return 0;
};
/**
 * Get the color of the arrow.
 * Returns: Numbers between 0.0 and 1.0 for red, green and blue.
 *
 * usage:
 * r,g,b = sph:getColor( )
 */
int arrow::getColor(lua_State* pState)
{
    if (m_pArrow == NULL) return 0;

    double fR = m_pArrow->getProperties()->getColor()->getR();
    double fG = m_pArrow->getProperties()->getColor()->getG();
    double fB = m_pArrow->getProperties()->getColor()->getB();

    lua_pushnumber(pState, fR );
    lua_pushnumber(pState, fG );
    lua_pushnumber(pState, fB );
    return 3;
};

/**
 * Get the position of the arrow.
 * Returns: Numbers (x,y,z)
 *
 * usage:
 * x,y,z = sph:getPos( )
 */
int arrow::getPos(lua_State* pState)
{
    if (m_pArrow == NULL) return 0;

    lua_pushnumber(pState, 1.0E2 * m_pArrow->getPos()->getX() );
    lua_pushnumber(pState, 1.0E2 * m_pArrow->getPos()->getY() );
    lua_pushnumber(pState, 1.0E2 * m_pArrow->getPos()->getZ() );
    return 3;
};
/**
 * Set the position of the arrow.
 * Parameters: Numbers (x,y,z)
 *
 * usage:
 * sph:setPos( x,y,z )
 */
int arrow::setPos(lua_State* pState)
{
    if (m_pArrow == NULL) return 0;

    m_pArrow->getPos()->setX( 1.0E-2 * luaL_checknumber(pState, -3) );
    m_pArrow->getPos()->setY( 1.0E-2 * luaL_checknumber(pState, -2) );
    m_pArrow->getPos()->setZ( 1.0E-2 * luaL_checknumber(pState, -1) );
    return 0;
};

/**
 * Get the radius of the arrow.
 * Returns: Number
 *
 * usage:
 * rad = sph:getRadius( )
 */
int arrow::getRadius(lua_State* pState)
{
    if (m_pArrow == NULL) return 0;

    lua_pushnumber(pState, 1.0E2 ); // * m_pArrow->getRadius() );
    return 1;
};

/**
 * Set/get the x coordinate of the position of the atom
 * If no parameter is given, the function just returns a number, otherwise it will set the x coordinate using the parameter provided by the user.
 *
 * usage:
 * atm_a:x( 15.0 )
 * print( atm_a:x() )
 */
int arrow::x(lua_State* pState)
{
    if (m_pArrow == NULL) return 0;

    if (lua_isnumber( pState, -1 ))
    {
        double fX = luaL_checknumber(pState, -1);
        m_pArrow->getPos()->setX( 1.0E-2 * fX );
    }

    lua_pushnumber(pState, 1.0E2 * m_pArrow->getPos()->getX() );
    return 1;
};
/**
 * Set/get the y coordinate of the position of the atom
 * If no parameter is given, the function just returns a number, otherwise it will set the y coordinate using the parameter provided by the user.
 *
 * usage:
 * atm_a:y( 15.0 )
 * print( atm_a:y() )
 */
int arrow::y(lua_State* pState)
{
    if (m_pArrow == NULL) return 0;

    if (lua_isnumber( pState, -1 ))
    {
        double fY = luaL_checknumber(pState, -1);
        m_pArrow->getPos()->setY( 1.0E-2 * fY );
    }

    lua_pushnumber(pState, 1.0E2 * m_pArrow->getPos()->getY( ) );
    return 1;
};
/**
 * Set/get the z coordinate of the position of the atom
 * If no parameter is given, the function just returns a number, otherwise it will set the z coordinate using the parameter provided by the user.
 *
 * usage:
 * atm_a:z( 15.0 )
 * print( atm_a:z() )
 */
int arrow::z(lua_State* pState)
{
    if (m_pArrow == NULL) return 0;

    if (lua_isnumber( pState, -1 ))
    {
        double fZ = luaL_checknumber(pState, -1);
        m_pArrow->getPos()->setZ( 1.0E-2 * fZ );
    }

    lua_pushnumber(pState, 1.0E2 * m_pArrow->getPos()->getZ( ) );
    return 1;
};

const char arrow::className[] = "arrow";
const svt_luna<arrow>::RegType arrow::methods[] =
{
    { "x",            &arrow::x             },
    { "y",            &arrow::y             },
    { "z",            &arrow::z             },
    { "getRadius",    &arrow::getRadius     },
    { "setRadius",    &arrow::setRadius     },
    { "setColor",     &arrow::setColor      },
    { "getColor",     &arrow::getColor      },
    { "getPos",       &arrow::getPos        },
    { "setPos",       &arrow::setPos        },
    { 0,              0                      }
};

///////////////////////////////////////////////////////////////////////////////
// LUA BINDINGS: LABEL
///////////////////////////////////////////////////////////////////////////////

/**
 * The label class is a very simple svt visualization node to put text labels into the 3D scene
 */
label::label(lua_State *pState)
{
    m_pLabel = NULL;

    if (pState == NULL)
        return;

    const char* pText = NULL;
    Real64 fX = 0.0;
    Real64 fY = 0.0;
    Real64 fZ = 0.0;

    if ( !lua_isnil( pState, -1 ) && !lua_isnil( pState, -2 ) && !lua_isnil( pState, -3 ) && !lua_isnil( pState, -4 ) )
    {
        fX = 1.0E-2 * luaL_checknumber(pState, -4);
        fY = 1.0E-2 * luaL_checknumber(pState, -3);
        fZ = 1.0E-2 * luaL_checknumber(pState, -2);
        pText =  luaL_checkstring(pState, -1);
    }

    m_pLabel = new svt_textnode( new svt_pos(fX, fY, fZ) );
    if (pText)
        m_pLabel->setText( (char*)pText );
};
label::~label()
{
};

/**
 * Set label
 * \param pLabel pointer to svt_textnode object
 */
void label::set( svt_textnode* pLabel )
{
    m_pLabel = pLabel;
};
/**
 * Get svt_node/svt_textnode object
 * \return svt_textnode object
 */
svt_textnode* label::get()
{
    return m_pLabel;
};

/**
 * Is the given svt_node object a label?
 */
bool label::isLabel( svt_node* pLabel )
{
    return typeid( *pLabel ) == typeid( svt_textnode );
}

/**
 * Set the color of the label.
 * Parameter: Number between 0.0 and 1.0 for red, green and blue.
 *
 * usage:
 * sph:setColor( 1.0, 0.0, 0.5 )
 */
int label::setColor(lua_State* pState)
{
    if (m_pLabel == NULL) return 0;

    if (m_pLabel->getProperties() == NULL)
        m_pLabel->setProperties( new svt_properties() );
    if (m_pLabel->getProperties()->getColor() == NULL)
        m_pLabel->getProperties()->setColor( new svt_color() );

    double fR = luaL_checknumber(pState, -3);
    double fG = luaL_checknumber(pState, -2);
    double fB = luaL_checknumber(pState, -1);

    m_pLabel->getProperties()->getColor()->setR( fR );
    m_pLabel->getProperties()->getColor()->setG( fG );
    m_pLabel->getProperties()->getColor()->setB( fB );
    return 0;
};
/**
 * Get the color of the label.
 * Returns: Numbers between 0.0 and 1.0 for red, green and blue.
 *
 * usage:
 * r,g,b = sph:getColor( )
 */
int label::getColor(lua_State* pState)
{
    if (m_pLabel == NULL) return 0;

    double fR = m_pLabel->getProperties()->getColor()->getR();
    double fG = m_pLabel->getProperties()->getColor()->getG();
    double fB = m_pLabel->getProperties()->getColor()->getB();

    lua_pushnumber(pState, fR );
    lua_pushnumber(pState, fG );
    lua_pushnumber(pState, fB );
    return 3;
};

/**
 * Get the position of the label.
 * Returns: Numbers (x,y,z)
 *
 * usage:
 * x,y,z = sph:getPos( )
 */
int label::getPos(lua_State* pState)
{
    if (m_pLabel == NULL) return 0;

    lua_pushnumber(pState, 1.0E2 * m_pLabel->getPos()->getX() );
    lua_pushnumber(pState, 1.0E2 * m_pLabel->getPos()->getY() );
    lua_pushnumber(pState, 1.0E2 * m_pLabel->getPos()->getZ() );
    return 3;
};
/**
 * Set the position of the label.
 * Parameters: Numbers (x,y,z)
 *
 * usage:
 * sph:setPos( x,y,z )
 */
int label::setPos(lua_State* pState)
{
    if (m_pLabel == NULL) return 0;

    m_pLabel->getPos()->setX( 1.0E-2 * luaL_checknumber(pState, -3) );
    m_pLabel->getPos()->setY( 1.0E-2 * luaL_checknumber(pState, -2) );
    m_pLabel->getPos()->setZ( 1.0E-2 * luaL_checknumber(pState, -1) );
    return 0;
};

/**
 * Get the text of the label
 * Returns: String
 *
 * usage:
 * text = lbl:getText( )
 */
int label::getText(lua_State* pState)
{
    if (m_pLabel == NULL) return 0;

    lua_pushstring( pState, m_pLabel->getText() );
    return 1;
};
/**
 * Set the text of the label
 * Parameters: String
 *
 * usage:
 * lbl:setText( "Hello World" )
 */
int label::setText(lua_State* pState)
{
    if (m_pLabel == NULL) return 0;

    m_pLabel->setText( (char*)luaL_checkstring(pState, -1) );
    return 0;
};

/**
 * Set/get the x coordinate of the position of the atom
 * If no parameter is given, the function just returns a number, otherwise it will set the x coordinate using the parameter provided by the user.
 *
 * usage:
 * atm_a:x( 15.0 )
 * print( atm_a:x() )
 */
int label::x(lua_State* pState)
{
    if (m_pLabel == NULL) return 0;

    if (lua_isnumber( pState, -1 ))
    {
        double fX = luaL_checknumber(pState, -1);
        m_pLabel->getPos()->setX( 1.0E-2 * fX );
    }

    lua_pushnumber(pState, 1.0E2 * m_pLabel->getPos()->getX() );
    return 1;
};
/**
 * Set/get the y coordinate of the position of the atom
 * If no parameter is given, the function just returns a number, otherwise it will set the y coordinate using the parameter provided by the user.
 *
 * usage:
 * atm_a:y( 15.0 )
 * print( atm_a:y() )
 */
int label::y(lua_State* pState)
{
    if (m_pLabel == NULL) return 0;

    if (lua_isnumber( pState, -1 ))
    {
        double fY = luaL_checknumber(pState, -1);
        m_pLabel->getPos()->setY( 1.0E-2 * fY );
    }

    lua_pushnumber(pState, 1.0E2 * m_pLabel->getPos()->getY( ) );
    return 1;
};
/**
 * Set/get the z coordinate of the position of the atom
 * If no parameter is given, the function just returns a number, otherwise it will set the z coordinate using the parameter provided by the user.
 *
 * usage:
 * atm_a:z( 15.0 )
 * print( atm_a:z() )
 */
int label::z(lua_State* pState)
{
    if (m_pLabel == NULL) return 0;

    if (lua_isnumber( pState, -1 ))
    {
        double fZ = luaL_checknumber(pState, -1);
        m_pLabel->getPos()->setZ( 1.0E-2 * fZ );
    }

    lua_pushnumber(pState, 1.0E2 * m_pLabel->getPos()->getZ( ) );
    return 1;
};

const char label::className[] = "label";
const svt_luna<label>::RegType label::methods[] =
{
    { "x",            &label::x             },
    { "y",            &label::y             },
    { "z",            &label::z             },
    { "getText",      &label::getText       },
    { "setText",      &label::setText       },
    { "setColor",     &label::setColor      },
    { "getColor",     &label::getColor      },
    { "getPos",       &label::getPos        },
    { "setPos",       &label::setPos        },
    { 0,              0                     }
};

///////////////////////////////////////////////////////////////////////////////
// LUA BINDINGS: SCULPTOR
///////////////////////////////////////////////////////////////////////////////

extern sculptor_window* g_pWindow;

/**
 * The sculptor class is supposed to encapsulate all general function calls and global settings for the sculptor application.
 */
sculptor::sculptor(lua_State *)
{
};
sculptor::~sculptor()
{
};

/**
 * Load a file.
 *
 * usage:
 * sclp = sculptor:new()
 * sclp:load("test.pdb")
 */
int sculptor::load(lua_State *pState)
{
    // get argument and check its type - will complain if
    const char *pFilename = luaL_checkstring(pState, -1);
    g_pWindow->loadDoc( QString( pFilename) , QString("NONE"));
    return 0;
};

/**
 * Save the sculptor state to a scl file.
 *
 * usage:
 * sculptor:save("current.scl")
 */
int sculptor::save(lua_State *pState)
{
    const char *pScr = luaL_checkstring(pState, -1);
    svtQtCallback();
    g_pWindow->saveSCL( string(pScr) );
    return 0;
};

/**
 * Get the number of currently loaded documents.
 * Returns a number.
 *
 * usage:
 * sclp = sculptor:new()
 * num = sclp:numDoc
 */
int sculptor::numDoc(lua_State* pState)
{
    // push one argument on stack that we would like to return
    lua_pushnumber( pState, g_pWindow->getDocumentList()->count() );

    // number of arguments we would like to return
    return 1;
};

/**
 * Get a document
 * Returns either a volume or molecule object
 *
 * usage:
 * sclp = sculptor:new()
 * mol = sclp:getDoc( 2 )
 */
int sculptor::getDoc(lua_State* pState)
{
    vector<sculptor_document*> aDocs = g_pWindow->getDocumentList()->getDocuments();
    int iDoc = luaL_checkinteger(pState, -1);

    if ( iDoc > (int)aDocs.size() || iDoc < 0 )
    {
        svtout << "Error: " << iDoc << " is an invalid document index!" << endl;
        return 0;
    }

    if ( aDocs[iDoc-1]->getType() == SENSITUS_DOC_PDB )
    {
        pdb_document* pPDB = (pdb_document*)aDocs[iDoc-1];
        molecule* pMol = new molecule( pState );
        pMol->set( (svt_pdb*)pPDB->getNode() );
        pMol->setDoc( pPDB );
        svt_luna<molecule>::pushToLua( pMol, pState );

        return 1;
    }

    if ( aDocs[iDoc-1]->getType() == SENSITUS_DOC_SITUS )
    {
        svt_volume<Real64>* pData = ((situs_document*)aDocs[iDoc-1])->getVolume();
        volume* pVolume = new volume( NULL );
        pVolume->set( pData, false );
        pVolume->setDoc( (situs_document*)aDocs[iDoc-1] );
        svt_luna<volume>::pushToLua( pVolume, pState );

        return 1;
    }

    if ( aDocs[iDoc-1]->getType() == SENSITUS_DOC_MARKER )
    {
        marker* pMarker = new marker( NULL );
        pMarker->setDoc( (sculptor_doc_marker*)aDocs[iDoc-1] );
        svt_luna<marker>::pushToLua( pMarker, pState );

        return 1;
    }

    svtout << "Error: This is not a document type one can access from Lua." << endl;
    return 0;
};

/**
 * Delete a document.
 *
 * usage:
 * sculptor:delDoc( 2 )
 */
int sculptor::delDoc(lua_State* pState)
{
    int iDoc = luaL_checkinteger(pState, -1);

    if ( iDoc > g_pWindow->getDocumentList()->count() || iDoc < 0 )
    {
        svtout << "Error: " << iDoc << " is an invalid document index!" << endl;
        return 0;
    }

    g_pWindow->delDoc( iDoc - 1 );

    return 0;
}

/**
 * Delete all documents.
 *
 * usage:
 * sculptor:delAllDocs( )
 */
int sculptor::delAllDocs(lua_State*)
{
    unsigned int iNum = g_pWindow->getDocumentList()->count();

    for(; iNum>1; iNum--)
        g_pWindow->delDoc( 1 );

    return 0;
}

/**
 * Get the sculptor version number.
 * Returns a string.
 *
 * usage:
 * sclp = sculptor:new()
 * print( sclp:version() )
 */
int sculptor::version(lua_State *pState)
{
    lua_pushstring( pState, SCULPTOR_VERSION );
    return 1;
};

/**
 * Print the current svt tree.
 *
 * usage:
 * sclp = sculptor:new()
 * sclp:printTree()
 */
int sculptor::printTree(lua_State*)
{
    g_pWindow->getScene()->getDataTopNode()->printTree();
    return 0;
};

/**
 * Triggers a redraw of the 3D window.
 *
 * usage:
 * sclp = sculptor:new()
 * sclp:redraw()
 */
int sculptor::redraw(lua_State* )
{
    svt_redraw();
    svtQtCallback();
    return 0;
};

/**
 * Lets sculptor sleep for a certain number of milliseconds.
 * Parameter: Number of milliseconds.
 *
 * usage:
 * sclp = sculptor:new()
 * sclp:sleep(10)
 */
int sculptor::sleep(lua_State *pState)
{
    // get argument and check its type - will complain if
    int iPause = luaL_checkinteger(pState, -1);
    svtQtCallback();
    svt_sleep( iPause );
    return 0;
};

/**
 * Makes a certain document visible.
 * Parameter: Number of document.
 *
 * usage:
 * sclp = sculptor:new()
 * sclp:showDoc(2)
 */
int sculptor::showDoc(lua_State *pState)
{
    // get argument and check its type - will complain if
    int iDoc = luaL_checkinteger(pState, -1);

    if ( iDoc >= 1 && iDoc <= g_pWindow->getDocumentList()->count() )
        g_pWindow->getDocumentList()->setVisible(iDoc-1);

    return 0;
};

/**
 * Hide a document.
 * Parameter: Number of document.
 *
 * usage:
 * sclp = sculptor:new()
 * sclp:hideDoc(2)
 */
int sculptor::hideDoc(lua_State *pState)
{
    // get argument and check its type - will complain if
    int iDoc = luaL_checkinteger(pState, -1);

    if ( iDoc >= 1 && iDoc <= g_pWindow->getDocumentList()->count() )
        g_pWindow->getDocumentList()->setVisible( iDoc-1, false );

    return 0;
};

/**
 * Get the current directory where Sculptor operates
 *
 * usage:
 * sclp = sculptor:new()
 * sclp:getCurrentDir()
 */
int sculptor::getCurrentDir(lua_State *pState)
{
    lua_pushstring( pState, g_pWindow->getCurrentDir().c_str() );
    return 1;
};

/**
 * Set the current directory where Sculptor operates
 *
 * usage:
 * sclp = sculptor:new()
 * sclp:setCurrentDir("/tmp")
 */
int sculptor::setCurrentDir(lua_State *pState)
{
    const char *pPath = luaL_checkstring(pState, -1);
    g_pWindow->setCurrentDir( pPath );
    return 0;
};

/**
 * Save a screenshot to disk (png or jpg format, depending on the extension of the file).
 *
 * usage:
 * sclp = sculptor:new()
 * sclp:saveScreenshot("scr.png")
 */
int sculptor::saveScreenshot(lua_State *pState)
{
    const char *pScr = luaL_checkstring(pState, -1);
    svtQtCallback();
    g_pWindow->saveScreenshot( pScr );
    return 0;
};

/**
 * Clear the log window
 *
 * usage:
 * sclp = sculptor:new()
 * sclp:clearLog()
 */
int sculptor::clearLog(lua_State*)
{
    g_pWindow->clearLog();
    return 0;
};

/**
 * Returns the global scene transformation matrix.
 * Returns matrix4 object.
 *
 * usage:
 * sclp = sculptor:new()
 * mat = sclp:getTrans()
 * mat:print()
 */
int sculptor::getTrans(lua_State *pState)
{
    // 1st step: Get Matrix from sculptor
    sculptor_scene* pScene = g_pWindow->getScene();
    svt_matrix4<Real64> oMat(  *pScene->getTopNode()->getTransformation() );

    // 2nd step: Create Lua-Binding object and set the matrix to this
    matrix4* pMat4 = new matrix4( pState );
    pMat4->set( oMat );

    // 3rd step: Push the new object into the lua interpreter
    svt_luna<matrix4>::pushToLua( pMat4, pState );

    return 1;
};

/**
 * Sets the global scene transformation matrix.
 * Parameter: matrix4 object.
 *
 * usage:
 * sclp = sculptor:new()
 * mat = sclp:getTrans()
 * mat:rotate(2, 10)
 * sclp:setTrans( mat )
 */
int sculptor::setTrans(lua_State *pState)
{
    // 1st step: Get matrix4 object from lua stack
    matrix4 *pMat = svt_luna<matrix4>::luaCheck( pState, -1 );

    // 2nd step: Get the svt_matrix4 object inside of the lua object
    svt_matrix4<Real64> oMat = pMat->get();

    // 3rd step: Set the matrix in sculptor
    oMat.copyToMatrix4f( *g_pWindow->getScene()->getTopNode()->getTransformation() );

    return 0;
};

/**
 * Closes sculptor. Hint: This function will still check for new documents that were created, but not saved yet and will prompt the user. In a test-script that might not
 * be desirable - just delAllDocs first to make sure that the quit() function will succeed without any user intervention.
 *
 * usage:
 * sculptor:quit( )
 */
int sculptor::quit(lua_State *)
{
    g_pWindow->quitNow();
    return 1;
};

/**
 * Opens a file-open-dialog. Returns a string with a file name. If user cancelled the dialog, the string will be empty.
 * Parameter: Message
 *
 * usage:
 * file = sculptor:guiFileOpenDlg("Please open a file for loading")
 * sculptor:load( file )
 */
int sculptor::guiFileOpenDlg(lua_State *pState)
{
    const char *pMessage = luaL_checkstring(pState, -1);
    string oFile = g_pWindow->guiFileOpenDlg( pMessage );

    lua_pushstring( pState, oFile.c_str() );
    return 1;
};

/**
 * Opens a file-save-dialog. Returns a string with a file name. If user cancelled the dialog, the string will be empty.
 * Parameter: Message
 *
 * usage:
 * file = sculptor:guiFileSaveDlg("Please open a file for saving")
 */
int sculptor::guiFileSaveDlg(lua_State *pState)
{
    const char *pMessage = luaL_checkstring(pState, -1);
    string oFile = g_pWindow->guiFileSaveDlg( pMessage );

    lua_pushstring( pState, oFile.c_str() );
    return 1;
};

/**
 * Opens a warning-dialog.
 * Parameter: Message
 *
 * usage:
 * sculptor:guiWarning("Your model only has CA atoms...")
 */
int sculptor::guiWarning(lua_State *pState)
{
    const char *pMessage = luaL_checkstring(pState, -1);
    g_pWindow->guiWarning( "Warning", pMessage, pMessage );
    return 0;
};

/**
 * Opens an information-dialog.
 * Parameter: Message
 *
 * usage:
 * sculptor:guiInfo("Your model only has CA atoms...")
 */
int sculptor::guiInfo(lua_State *pState)
{
    const char *pMessage = luaL_checkstring(pState, -1);
    g_pWindow->guiInfo( "Information", pMessage, pMessage );
    return 0;
};

/**
 * Opens a yes/no question-dialog.
 * Parameter: Message
 *
 * usage:
 * answer = sculptor:guiYesNo("Do you really want to quit?")
 */
int sculptor::guiYesNo(lua_State *pState)
{
    const char *pMessage = luaL_checkstring(pState, -1);
    bool bAnswer = g_pWindow->guiYesNo( string("Question"), string(pMessage) );
    lua_pushboolean( pState, bAnswer );
    return 1;
};

/**
 * Opens a free-text question-dialog.
 * Parameter: Message
 *
 * usage:
 * answer = sculptor:guiQuestion("Name of the author?")
 */
int sculptor::guiQuestion(lua_State *pState)
{
    string oAnswer;
    const char *pMessage = NULL;

    if ( lua_isstring(pState, -1) && lua_isstring(pState, -2) )
    {
        pMessage = luaL_checkstring(pState, -2);
        oAnswer = string( luaL_checkstring(pState, -1) );

    } else {
        pMessage = luaL_checkstring(pState, -1);
    }

    oAnswer = g_pWindow->guiQuestion( string("Question"), string(pMessage), oAnswer );
    lua_pushstring( pState, oAnswer.c_str() );
    return 1;
};

/**
 * Opens a value question-dialog.
 * Parameter: Message
 *
 * usage:
 * answer = sculptor:guiGetValue("Resolution of the map?")
 */
int sculptor::guiGetValue(lua_State *pState)
{
    double fAnswer = 0.0;
    const char *pMessage = NULL;

    if (lua_isnumber(pState, -1))
    {
        pMessage = luaL_checkstring(pState, -2);
        fAnswer  = luaL_checknumber(pState, -1);
    } else
        pMessage = luaL_checkstring(pState, -1);

    fAnswer = g_pWindow->guiGetValue( string("Question"), string(pMessage), fAnswer );
    lua_pushnumber( pState, fAnswer );
    return 1;
};

/**
 * Add a graphics primitive object (e.g. a sphere) to the 3D scene
 * Parameter: graphics object.
 *
 * usage:
 * sculptor:addGraphics( sphere:new( 1, 2, 1 ) )
 */
int sculptor::addGraphics(lua_State *pState)
{
    svt_node* pNode = NULL;

    // is it an arrow?
    if ( svt_luna<arrow>::luaIsType( pState, -1 ) )
    {
        arrow* pArr = svt_luna<arrow>::luaCheck( pState, -1 );
        if (pArr)
            pNode = pArr->get();
    }
    // is it a label?
    if ( svt_luna<label>::luaIsType( pState, -1 ) )
    {
        label* pLbl = svt_luna<label>::luaCheck( pState, -1 );
        if (pLbl)
            pNode = pLbl->get();
    }
    // is it a sphere?
    if ( svt_luna<sphere>::luaIsType( pState, -1 ) )
    {
        sphere* pSph = svt_luna<sphere>::luaCheck( pState, -1 );
        if (pSph)
            pNode = pSph->get();
    }

    if (pNode)
        g_pWindow->getScene()->getLuaNode()->add( pNode );
    else
        svtout << "Error: Unknown object type, not a graphics object!" << endl;

    return 0;
};

/**
 * Get a graphics primitive object that was added previously to the scene
 * Parameter: Index of graphics object.
 * Returns: Graphics object
 *
 * usage:
 * sphere = sculptor:getGraphics( 1 )
 */
int sculptor::getGraphics(lua_State *pState)
{
    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex >0)
    {
        int iCurrIndex = 1;
        svt_node* pNode = g_pWindow->getScene()->getLuaNode()->getSon();

        while( pNode != NULL && iCurrIndex < iIndex )
        {
            pNode = pNode->getNext();
            iCurrIndex++;
        }

        if (pNode == NULL)
        {
            svtout << "Error: Index out of range!" << endl;
            return 0;
        }

        // is it a sphere?
        if ( sphere::isSphere( pNode ) )
        {
            sphere* pSph = new sphere( NULL );
            pSph->set( (svt_sphere*)pNode );
            svt_luna<sphere>::pushToLua( pSph, pState );
            return 1;
        }
        // is it an arrow?
        if ( arrow::isArrow( pNode ) )
        {
            arrow* pArr = new arrow( NULL );
            pArr->set( (svt_arrow*)pNode );
            svt_luna<arrow>::pushToLua( pArr, pState );
            return 1;
        }
        // is it a label?
        if ( label::isLabel( pNode ) )
        {
            label* pLbl = new label( NULL );
            pLbl->set( (svt_textnode*)pNode );
            svt_luna<label>::pushToLua( pLbl, pState );
            return 1;
        }

        svtout << "Error: Unknown object type, not a graphics object!" << endl;
        return 0;
    }

    svtout << "Error: Please provide index, e.g. sphere = sculptor:getGraphics( 5 )!" << endl;
    return 0;
};

/**
 * Delete a graphics primitive object that was added previously to the scene
 * Parameter: Index of graphics object.
 *
 * usage:
 * sculptor:delGraphics( 1 )
 */
int sculptor::delGraphics(lua_State *pState)
{
    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex >0)
    {
        int iCurrIndex = 1;
        svt_node* pNode = g_pWindow->getScene()->getLuaNode()->getSon();

        while( pNode != NULL && iCurrIndex < iIndex )
        {
            pNode = pNode->getNext();
            iCurrIndex++;
        }

        if (pNode == NULL)
        {
            svtout << "Error: Index out of range!" << endl;
            return 0;
        }

        g_pWindow->getScene()->getLuaNode()->del( pNode );
    }
    return 0;
};

/**
 * Delete all graphics primitive objects that were added previously to the scene
 *
 * usage:
 * sculptor:delAllGraphics( )
 */
int sculptor::delAllGraphics(lua_State *)
{
    g_pWindow->getScene()->getLuaNode()->delSubTree();
    return 0;
};

extern void dumpMemTable();

/**
 * Print memory table
 *
 * usage:
 * sculptor:printMemory()
 */
int sculptor::printMemory(lua_State *)
{
    dumpMemTable();
    return 0;
}

int sculptor::getDisplayModeEnum(lua_State *L)
{
    const char *pDM = luaL_checkstring(L, -1);

    int iDisplayMode = 0;
    lua_getglobal(L, "displaymodeenum");
    lua_pushstring(L, pDM);
    lua_gettable( L, -2 );
    iDisplayMode = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return iDisplayMode - 1;
}
void sculptor::registerDisplayModeEnum(lua_State *L)
{
    lua_newtable(L);
    lua_pushliteral(L, "OFF");        lua_pushnumber(L, 1);  lua_settable(L, -3);
    lua_pushliteral(L, "POINT");      lua_pushnumber(L, 2);  lua_settable(L, -3);
    lua_pushliteral(L, "LINE");       lua_pushnumber(L, 3);  lua_settable(L, -3);
    lua_pushliteral(L, "CPK");        lua_pushnumber(L, 4);  lua_settable(L, -3);
    lua_pushliteral(L, "VDW");        lua_pushnumber(L, 5);  lua_settable(L, -3);
    lua_pushliteral(L, "DOTTED");     lua_pushnumber(L, 6);  lua_settable(L, -3);
    lua_pushliteral(L, "LICORICE");   lua_pushnumber(L, 7);  lua_settable(L, -3);
    lua_pushliteral(L, "TUBE");       lua_pushnumber(L, 8);  lua_settable(L, -3);
    lua_pushliteral(L, "NEWTUBE");    lua_pushnumber(L, 9);  lua_settable(L, -3);
    lua_pushliteral(L, "CARTOON");    lua_pushnumber(L, 10); lua_settable(L, -3);
    lua_pushliteral(L, "SURF");       lua_pushnumber(L, 11); lua_settable(L, -3);
    lua_pushliteral(L, "TOON");       lua_pushnumber(L, 12); lua_settable(L, -3);
    lua_setglobal(L, "displaymodeenum" );
};

int sculptor::getColmapModeEnum(lua_State *L, string oColmap)
{
    int iColmapMode = 0;
    lua_getglobal(L, "colmapmodeenum");
    lua_pushstring(L, oColmap.c_str());
    lua_gettable( L, -2 );
    iColmapMode = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return iColmapMode - 1;
}
void sculptor::registerColmapModeEnum(lua_State *L)
{
    lua_newtable(L);
    lua_pushliteral(L, "TYPE");        lua_pushnumber(L, 1);  lua_settable(L, -3);
    lua_pushliteral(L, "RESNAME");     lua_pushnumber(L, 2);  lua_settable(L, -3);
    lua_pushliteral(L, "CHAINID");     lua_pushnumber(L, 3);  lua_settable(L, -3);
    lua_pushliteral(L, "SEGID");       lua_pushnumber(L, 4);  lua_settable(L, -3);
    lua_pushliteral(L, "SOLID");       lua_pushnumber(L, 5);  lua_settable(L, -3);
    lua_pushliteral(L, "STRUCTURE");   lua_pushnumber(L, 6);  lua_settable(L, -3);
    lua_pushliteral(L, "MODEL");       lua_pushnumber(L, 7);  lua_settable(L, -3);
    lua_pushliteral(L, "BETA");        lua_pushnumber(L, 8);  lua_settable(L, -3);
    lua_setglobal(L, "colmapmodeenum" );
};

int sculptor::runGa(lua_State *)
{
    window_documentlist* pDL = g_pWindow->getDocumentList();

    for (int i=1; i<pDL->count(); i++)
	pDL->setSelected(i, true);

    g_pWindow->startGa(true);

    return 1;
};

/**
 * Compute the powell off-lattice refinement against a volume object
 * Parameter: volume object. Boolean parameter: If true only a spar
 * parameter: an array of molecules
 *
 * usage:
 * sculptor:powell(pdb_array, vol, res, bBlur)
 */
int sculptor::powell(lua_State* pState)
{
    //should blur
    bool bBlur = lua_toboolean( pState, -1);

    //
    // resolution
    //
    double fRes = luaL_checknumber(pState, -2);

    //
    // Volume object: on the stack
    //
    volume *pVol = svt_luna<volume>::luaCheck( pState, -3 );
    svt_volume<Real64>* pMap = pVol->get();

    //
    // create a matrix for the volume object
    //
    svt_matrix4< Real64 > oVolMat;
    if ( pVol->getDoc() )
        oVolMat.set( *pVol->getDoc()->getNode()->getTransformation() );

    vector< svt_point_cloud_pdb< svt_vector4<Real64> > > oPDBs;
    vector< molecule* > oMols;
    vector<svt_matrix4< Real64 > > oPDBMats;
    vector<double> aDirection;


    //
    // the pdbs
    //
    int iLen = lua_objlen(pState, -4);
    for(int i=0;i<iLen;i++)
    {
        lua_pushinteger(pState, i+1);
        lua_gettable(pState, -5);

        molecule *pMol = svt_luna<molecule>::luaCheck( pState, -1 );
	if (!pMol->checkData()) return 0;

	svt_point_cloud_pdb< svt_vector4<Real64> >* pPDB = pMol->get();
	oMols.push_back( pMol );

	//
	// create transformation matrix for powell
	//
	oPDBs.push_back(*pPDB);
	
	svt_matrix4< Real64 > oPDBMat;
	if (pPDB)
	    oPDBMat = svt_matrix4<Real64>( *pMol->getDoc()->getNode()->getTransformation() );
    

        //
	// convert to angstroem
	//
	oPDBMat.setTranslationX( oPDBMat.translationX() * 1.0E2 );
	oPDBMat.setTranslationY( oPDBMat.translationY() * 1.0E2 );
	oPDBMat.setTranslationZ( oPDBMat.translationZ() * 1.0E2 );
	oPDBMats.push_back( oPDBMat );
    
	oVolMat.setTranslationX( oVolMat.translationX() * 1.0E2 );
	oVolMat.setTranslationY( oVolMat.translationY() * 1.0E2 );
	oVolMat.setTranslationZ( oVolMat.translationZ() * 1.0E2 );

        //
	// limit the optimizations if the users wishes to
	//
	aDirection.push_back( pMap->getWidth() * 0.25 );
	//aDirection.push_back( 0 );
	aDirection.push_back( pMap->getWidth() * 0.25 );
	//aDirection.push_back( 0 );
	aDirection.push_back( pMap->getWidth() * 0.25 );
	//aDirection.push_back( 0 );
	aDirection.push_back( deg2rad( 10.0 ) );
	//aDirection.push_back( 0 );
	aDirection.push_back( deg2rad( 10.0 ) );
	//aDirection.push_back( 0 );
	aDirection.push_back( deg2rad( 10.0 ) );
	//aDirection.push_back( 0 );
	
        lua_pop(pState, 1);
    }

    //
    // create powell object
    //
    svt_powell_cc oPowell( oPDBs, oPDBMats, *pMap, oVolMat );
    oPowell.setTolerance( 1.0E-6 );
    oPowell.setDirection( aDirection );
    oPowell.setMaxIter( 50 );
    oPowell.setResolution( fRes );

    oPowell.setLaplacian( false );
    oPowell.setFast( false );
    oPowell.setBlur( bBlur );
    oPowell.setMaxIter2Minimize( 10 );

    //
    // Go....
    //
    oPowell.maximize();

    //
    // Apply the resulting transformation
    //
    oPDBMats = oPowell.getTrans();

    for(int i=0;i<iLen;i++)
    {

	oPDBMats[i].setTranslationX( oPDBMats[i].translationX() * 1.0E-2 );
	oPDBMats[i].setTranslationY( oPDBMats[i].translationY() * 1.0E-2 );
	oPDBMats[i].setTranslationZ( oPDBMats[i].translationZ() * 1.0E-2 );

	oPDBMats[i].copyToMatrix4f( * oMols[i]->get()->getTransformation() );
    }

    return 0;
};

/**
 * Run command line
 * Parameter: a string representing the command line
 *
 * usage:
 * sculptor:system(Commandline)
 */
int sculptor::system(lua_State* pState)
{
    const char *pCommand = luaL_checkstring(pState, -1);
    
    svt_system((char*)pCommand);
    return 0;

}
 
const char sculptor::className[] = "sculptor";
const svt_luna<sculptor>::RegType sculptor::methods[] =
{
    { "load",            &sculptor::load             },
    { "save",            &sculptor::save             },
    { "numDoc",          &sculptor::numDoc           },
    { "getDoc",          &sculptor::getDoc           },
    { "delDoc",          &sculptor::delDoc           },
    { "delAllDocs",      &sculptor::delAllDocs       },
    { "version",         &sculptor::version          },
    { "printTree",       &sculptor::printTree        },
    { "getTrans",        &sculptor::getTrans         },
    { "setTrans",        &sculptor::setTrans         },
    { "redraw",          &sculptor::redraw           },
    { "sleep",           &sculptor::sleep            },
    { "getCurrentDir",   &sculptor::getCurrentDir    },
    { "setCurrentDir",   &sculptor::setCurrentDir    },
    { "saveScreenshot",  &sculptor::saveScreenshot   },
    { "showDoc",         &sculptor::showDoc          },
    { "hideDoc",         &sculptor::hideDoc          },
    { "clearLog",        &sculptor::clearLog         },
    { "guiFileOpenDlg",  &sculptor::guiFileOpenDlg   },
    { "guiFileSaveDlg",  &sculptor::guiFileSaveDlg   },
    { "guiWarning",      &sculptor::guiWarning       },
    { "guiInfo",         &sculptor::guiInfo          },
    { "guiYesNo",        &sculptor::guiYesNo         },
    { "guiQuestion",     &sculptor::guiQuestion      },
    { "guiGetValue",     &sculptor::guiGetValue      },
    { "quit",            &sculptor::quit             },
    { "runGa",           &sculptor::runGa            },
    { "powell",          &sculptor::powell           },
    { "system",		 &sculptor::system	     },
    { "addGraphics",     &sculptor::addGraphics      },
    { "getGraphics",     &sculptor::getGraphics      },
    { "delGraphics",     &sculptor::delGraphics      },
    { "delAllGraphics",  &sculptor::delAllGraphics   },
    { "printMemory",     &sculptor::printMemory      },
    { 0,                 0                           }
};

///////////////////////////////////////////////////////////////////////////////
// LUA BINDINGS: MATRIX4
///////////////////////////////////////////////////////////////////////////////

/**
 * Matrix class for Lua
 */
matrix4::matrix4(lua_State *)
{
};
matrix4::~matrix4()
{
};

/**
 * Set Matrix
 * \param oMat matrix
 */
void matrix4::set( svt_matrix4<Real64> oMatrix )
{
    m_oMatrix = oMatrix;
};
/**
 * Get Matrix
 * \return svt_matrix4 object
 */
svt_matrix4<Real64> matrix4::get()
{
    return m_oMatrix;
};

/**
 * Rotate around a single axis.
 * rotate( iAxis, fAngle ).
 * \param iAxis values: (0,1,2) = (x,y,z)
 * \param fAngle angle in degrees
 */
int matrix4::rotate(lua_State *pState)
{
    int iAxis     = luaL_checkinteger(pState, -2);
    double fAngle = luaL_checknumber(pState, -1);

    m_oMatrix.rotate( iAxis, deg2rad(fAngle) );
    return 0;
};

/**
 * Rotate around a single axis around the center.
 * rotate( iAxis, fAngle ).
 * \param iAxis values: (0,1,2) = (x,y,z)
 * \param fAngle angle in degrees
 */
int matrix4::rotateCenter(lua_State *pState)
{
    int iAxis     = luaL_checkinteger(pState, -2);
    double fAngle = luaL_checknumber(pState, -1);

    svt_matrix4<Real64> oTmpMat = m_oMatrix;
    svt_vector4<Real64> oTmpVec = oTmpMat.translation();
    oTmpMat.setTranslation( 0, 0, 0 );
    oTmpMat.rotate( iAxis, deg2rad(fAngle) );
    oTmpMat.translate( oTmpVec );
    m_oMatrix = oTmpMat;

    return 0;
};

/**
 * Rotate around Phi, theta and psi 
 * rotate( fPhiAngle,  fThetaAngle, fPsiAngle ).
 * \param fPhiAngle angle in degrees
 * \param fThetaAngle angle in degrees
 * \param fPsiAngle angle in degrees
 */
int matrix4::rotatePTP(lua_State *pState)
{
    double fPhiAngle   = luaL_checknumber(pState, -3);
    double fThetaAngle = luaL_checknumber(pState, -2);
    double fPsiAngle   = luaL_checknumber(pState, -1);

    svt_matrix4<Real64> oTmpMat = m_oMatrix;
    svt_vector4<Real64> oTmpVec = oTmpMat.translation();
    oTmpMat.setTranslation( 0, 0, 0 );
    oTmpMat.rotatePTP( deg2rad(fPhiAngle), deg2rad(fThetaAngle),deg2rad(fPsiAngle));
    oTmpMat.translate( oTmpVec );
    m_oMatrix = oTmpMat;

    return 0;
};

/**
 * Translate.
 * translate( fX, fY, fZ ).
 */
int matrix4::translate(lua_State *pState)
{
    // get arguments - as they are pushed onto a stack, they are in reverse order...
    double fX = luaL_checknumber(pState, -3);
    double fY = luaL_checknumber(pState, -2);
    double fZ = luaL_checknumber(pState, -1);

    m_oMatrix.translate( fX * 1.0E-2, fY * 1.0E-2, fZ * 1.0E-2 );
    return 0;
};

/**
 * Multiply with another matrix4 matrix.
 * Parameter: Matrix4 object
 * Returns: New matrix4 object.
 *
 * usage:
 * mat_c = mat_a:mult( mat_b )
 */
int matrix4::mult(lua_State* pState)
{
    matrix4 *pMat = svt_luna<matrix4>::luaCheck( pState, -1 );
    svt_matrix4<Real64> oMat = pMat->get();
    svt_matrix4<Real64> oTmpMat = m_oMatrix * oMat;

    matrix4* pMat4 = new matrix4( pState );
    pMat4->set( oTmpMat );
    svt_luna<matrix4>::pushToLua( pMat4, pState );
    return 1;
};

/**
 * Print content to stdout
 */
int matrix4::print( lua_State* )
{
    svtout << m_oMatrix;
    return 0;
}

const char matrix4::className[] = "matrix4";
const svt_luna<matrix4>::RegType matrix4::methods[] =
{
    { "print",        &matrix4::print        },
    { "translate",    &matrix4::translate    },
    { "rotate",       &matrix4::rotate       },
    { "rotateCenter", &matrix4::rotateCenter },
    { "rotatePTP",    &matrix4::rotatePTP    },
    { "mult",         &matrix4::mult         },
    { 0,              0                      }
};

///////////////////////////////////////////////////////////////////////////////
// Lua interpreter class
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 */
sculptor_lua::sculptor_lua() : svt_lua()
{
};

/**
 * Destructor
 */
sculptor_lua::~sculptor_lua()
{
};

/**
 * Initialize interpreter and register all classes and functions
 */
void sculptor_lua::init()
{
    svt_lua::init();

    // register all classes
    svt_luna<sculptor>::Register( m_pState );
    svt_luna<matrix4>::Register( m_pState );
    svt_luna<molecule>::Register( m_pState );
    svt_luna<atom>::Register( m_pState );
    svt_luna<volume>::Register( m_pState );
    svt_luna<sphere>::Register( m_pState );
    svt_luna<arrow>::Register( m_pState );
    svt_luna<label>::Register( m_pState );
    svt_luna<marker>::Register( m_pState );

    // add a global sculptor object
    sculptor* pSculptor = new sculptor( m_pState );
    svt_luna<sculptor>::pushToLua( pSculptor, m_pState );
    lua_setglobal( m_pState, "sculptor" );

    // register the displaymode enum
    sculptor::registerDisplayModeEnum( m_pState );
    sculptor::registerColmapModeEnum( m_pState );
}

///////////////////////////////////////////////////////////////////////////////
// LUA BINDINGS: ATOM
///////////////////////////////////////////////////////////////////////////////

/**
 * The atom class encapsulates a single atom
 */
atom::atom(lua_State *)
{
    m_iIndex = -1;
};
atom::~atom()
{
};

/**
 * Set the atom and position information
 */
void atom::set( svt_point_cloud_atom oAtom, svt_vector4<Real64> oPos, int iIndex )
{
    m_oAtom = oAtom;
    m_oPos  = oPos;
    m_iIndex = iIndex;
};

/**
 * Get the atom 
 */
svt_point_cloud_atom* atom::get( )
{
    return &m_oAtom;
};
/**
 * Get the position
 */
svt_vector4<Real64> atom::getPoint( )
{
    return m_oPos;
};

/**
 * Get the svt index
 */
int atom::getIndex()
{
    return m_iIndex;
};

/**
 * Get the position of the atom.
 * Returns: Numbers (x,y,z)
 *
 * usage:
 * x,y,z = atm:getPos( )
 */
int atom::getPos(lua_State* pState)
{
    lua_pushnumber(pState, m_oPos.x() );
    lua_pushnumber(pState, m_oPos.y() );
    lua_pushnumber(pState, m_oPos.z() );
    return 3;
};
/**
 * Set the position of the atom.
 * Parameters: Numbers (x,y,z)
 *
 * usage:
 * atm:setPos( x,y,z )
 */
int atom::setPos(lua_State* pState)
{
    m_oPos.x( luaL_checknumber(pState, -3) );
    m_oPos.y( luaL_checknumber(pState, -2) );
    m_oPos.z( luaL_checknumber(pState, -1) );
    return 0;
};


/**
 * Set/get the x coordinate of the position of the atom
 * If no parameter is given, the function just returns a number, otherwise it will set the x coordinate using the parameter provided by the user.
 *
 * usage:
 * atm_a:x( 15.0 )
 * print( atm_a:x() )
 */
int atom::x(lua_State* pState)
{
    if (lua_isnumber( pState, -1 ))
    {
        double fX = luaL_checknumber(pState, -1);
        m_oPos.x( fX );
    }

    lua_pushnumber(pState, m_oPos.x() );
    return 1;
};
/**
 * Set/get the y coordinate of the position of the atom
 * If no parameter is given, the function just returns a number, otherwise it will set the y coordinate using the parameter provided by the user.
 *
 * usage:
 * atm_a:y( 15.0 )
 * print( atm_a:y() )
 */
int atom::y(lua_State* pState)
{
    if (lua_isnumber( pState, -1 ))
    {
        double fY = luaL_checknumber(pState, -1);
        m_oPos.y( fY );
    }

    lua_pushnumber(pState, m_oPos.y() );
    return 1;
};
/**
 * Set/get the z coordinate of the position of the atom
 * If no parameter is given, the function just returns a number, otherwise it will set the z coordinate using the parameter provided by the user.
 *
 * usage:
 * atm_a:z( 15.0 )
 * print( atm_a:z() )
 */
int atom::z(lua_State* pState)
{
    if (lua_isnumber( pState, -1 ))
    {
        double fZ = luaL_checknumber(pState, -1);
        m_oPos.z( fZ );
    }

    lua_pushnumber(pState, m_oPos.z() );
    return 1;
};

/**
 * Get displaymode.
 */
int atom::getDisplayMode()
{
    return m_iDisplayMode;
};
/**
 * Set displaymode.
 */
void atom::setDisplayMode(int iDisplayMode)
{
    m_iDisplayMode = iDisplayMode;
};

/**
 * Get displaymode.
 */
int atom::getDisplayMode(lua_State* pState)
{
    switch(m_iDisplayMode)
    {
    case SVT_ATOM_OFF:
        lua_pushstring( pState, "OFF" );
        return 1;
        break;
    case SVT_ATOM_CPK:
        lua_pushstring( pState, "CPK" );
        return 1;
        break;
    case SVT_ATOM_POINT:
        lua_pushstring( pState, "POINT" );
        return 1;
        break;
    case SVT_ATOM_VDW:
        lua_pushstring( pState, "VDW" );
        return 1;
        break;
    case SVT_ATOM_DOTTED:
        lua_pushstring( pState, "DOTTED" );
        return 1;
        break;
    case SVT_ATOM_LICORIZE:
        lua_pushstring( pState, "LICORICE" );
        return 1;
        break;
    case SVT_ATOM_TUBE:
        lua_pushstring( pState, "TUBE" );
        return 1;
        break;
    case SVT_ATOM_CARTOON:
        lua_pushstring( pState, "CARTOON" );
        return 1;
        break;
    case SVT_ATOM_NEWTUBE:
        lua_pushstring( pState, "NEWTUBE" );
        return 1;
        break;
    case SVT_ATOM_TOON:
        lua_pushstring( pState, "TOON" );
        return 1;
        break;
    default:
        lua_pushstring( pState, "NOTVALID" );
        return 1;
        break;
    };

    return 0;
};

/**
 * Set displaymode.
 */
int atom::setDisplayMode(lua_State* pState)
{
    m_iDisplayMode = sculptor::getDisplayModeEnum( pState ) - 1;
    return 0;
};

/**
 * Print content to stdout
 */
int atom::print(lua_State *)
{
    svtout << "Atom: " << m_oAtom.getPDBIndex() << " - " << m_oAtom.getName() << " (" << m_oPos.x() << ", " << m_oPos.y() << ", " << m_oPos.z() << ")" << endl;
    return 0;
};

/**
 * Measure distance to another atom.
 * Parameter: Atom object.
 * Returns: Number.
 *
 * usage:
 * dist = mol:getAtom(123):distance( mol:getAtom(124) )
 */
int atom::distance(lua_State * pState)
{
   Real64 fDist = 0;

    if (svt_luna<atom>::luaIsType( pState, -1 ))
    {
    	atom *pAtom = svt_luna<atom>::luaCheck( pState, -1 );
	fDist = getPoint().distance( pAtom->getPoint() );
    }
    else 
    {
	marker *pMarker = svt_luna<marker>::luaCheck( pState, -1 );
	fDist = getPoint().distance( (pMarker->getDoc()->getMarkerNodePos())*1.0E2 );
    }
	
    lua_pushnumber(pState, fDist );
    return 1;
}

const char atom::className[] = "atom";
const svt_luna<atom>::RegType atom::methods[] =
{
    { "x",               &atom::x                  },
    { "y",               &atom::y                  },
    { "z",               &atom::z                  },
    { "print",           &atom::print              },
    { "getDisplayMode",  &atom::getDisplayMode     },
    { "setDisplayMode",  &atom::setDisplayMode     },
    { "distance",        &atom::distance           },
    { "getPos",          &atom::getPos             },
    { "setPos",          &atom::setPos             },
    { 0,                 0                         }
};

///////////////////////////////////////////////////////////////////////////////
// LUA BINDINGS: MOLECULE
///////////////////////////////////////////////////////////////////////////////

/**
 * The molecule class encapsulates a single molecule, atomic model document
 */
molecule::molecule(lua_State *)
{
    m_pPDB = NULL;
    m_pDoc = NULL;
    m_bDelete = false;
};
molecule::~molecule()
{
    if (m_bDelete)
        delete m_pPDB;
};

/**
 * Set the svt molecular data that is encapsulated by this object
 * \param pointer to svt_pdb object
 * \param boolean, if true, the data gets deleted if this object here gets deleted
 */
void molecule::set( svt_pdb* pPDB, bool bDelete )
{
    m_pPDB = pPDB;
    m_bDelete = bDelete;
    if (m_pPDB->getTransformation() == NULL)
        m_pPDB->setTransformation( new svt_matrix4f() );
}
/**
 * Set the sculptor document that is encapsulated by this object
 */
void molecule::setDoc( pdb_document* pDoc)
{
    m_pDoc = pDoc;
};

/**
 * Get the svt molecular data that is encapsulated by this object
 */
svt_pdb* molecule::get( )
{
    return m_pPDB;
}
/**
 * Get the sculptor document that is encapsulated by this object
 */
pdb_document* molecule::getDoc( )
{
    return m_pDoc;
};

/**
 * Local -> global transformation.
 */
svt_vector4<Real64> molecule::local2Global( svt_vector4<Real64> oLocalVec )
{
    // Get transformation matrix
    svt_matrix4< Real64 > oMat;
    if (m_pDoc != NULL)
    {
        oMat = *(m_pPDB->getTransformation());
	oMat[0][3] *= 1.0E2;
	oMat[1][3] *= 1.0E2;
	oMat[2][3] *= 1.0E2;
    }

    return oMat * oLocalVec;
};
/**
 * Global->local transformation.
 */
svt_vector4<Real64> molecule::global2Local( svt_vector4<Real64> oGlobalVec )
{
    // Get transformation matrix
    svt_matrix4< Real64 > oMat;
    if (m_pDoc != NULL)
    {
        oMat = *(m_pPDB->getTransformation());
	oMat[0][3] *= 1.0E2;
	oMat[1][3] *= 1.0E2;
	oMat[2][3] *= 1.0E2;
        oMat.invert();
    }

    return oMat * oGlobalVec;
};

/**
 * Check if the document is present. If the molecule is a local molecule, created in Lua, it will not be part of the document list and will not be rendered. All rendering
 * functions should therefore call this function here to make sure that everything is setup correctly.
 */
bool molecule::checkDoc()
{
    if (m_pDoc == NULL)
    {
        svtout << "Sorry, only documents get rendered, please use addDoc first." << endl;
        return false;
    }

    return true;
};

/**
 * Check if data is present.
 */
bool molecule::checkData()
{
    if (!m_pPDB)
    {
        svtout << "Error: The molecule object is empty, does not contain any data!" << endl;
        return false;
    }

    return true;
};

/**
 * Add the molecule to the list of documents. Displaymode is lost, has to be re-set using setDisplayMode after addDoc is called.
 * Parameter: String with the name
 *
 * Usage:
 * addDoc( "test.pdb" )
 */
int molecule::addDoc(lua_State* pState)
{
    if (!checkData()) return 0;

    const char* pFname = luaL_checkstring(pState, -1);

    g_pWindow->getScene()->switchToData();
    pdb_document* pSD = new pdb_document(g_pWindow->getScene()->getDataTopNode(), string(pFname), SVT_PDB, NULL, SVT_NONE, m_pPDB);

    // the local copy needs to be destroyed
    if (m_bDelete)
    {
        delete m_pPDB;
        m_bDelete = false;
    }
    m_pPDB = (svt_pdb*)pSD->getNode();

    // document position
    svt_vector4f oDocOrigin = pSD->getRealPos();
    oDocOrigin.multScalar( 1.0E8f );
    pSD->getNode()->setPos(new svt_pos(oDocOrigin.x(),oDocOrigin.y(),oDocOrigin.z()));
    pSD->storeInitMat();
    pSD->setChanged();

    // ok, now add the thing to sculptor
    g_pWindow->addDocSimple((sculptor_document*)pSD, pFname);
    g_pWindow->adjustOrigin();

    m_pDoc = pSD;

    return 0;
};

/**
 * Get the number of atoms in the molecule
 * Returns a number.
 */
int molecule::size(lua_State* pState)
{
    if (!checkData()) return 0;

    lua_pushnumber(pState, m_pPDB->size());
    return 1;
};

/**
 * Returns the transformation matrix of the svt object.
 * Returns matrix4 object.
 *
 * usage:
 * mat = mol:getTrans()
 * mat:print()
 */
int molecule::getTrans(lua_State *pState)
{
    if (!checkData()) return 0;

    svt_matrix4<Real64> oMat(  *m_pPDB->getTransformation() );

    matrix4* pMat4 = new matrix4( pState );
    pMat4->set( oMat );

    svt_luna<matrix4>::pushToLua( pMat4, pState );

    return 1;
};

/**
 * Sets the transformation matrix of the svt object.
 * Parameter: matrix4 object.
 *
 * usage:
 * mat = mol:getTrans()
 * mat:rotate(2, 10)
 * mol:setTrans( mat )
 */
int molecule::setTrans(lua_State *pState)
{
    if (!checkData()) return 0;

    matrix4 *pMat = svt_luna<matrix4>::luaCheck( pState, -1 );
    svt_matrix4<Real64> oMat = pMat->get();

    oMat.copyToMatrix4f( *m_pPDB->getTransformation() );

    return 0;
};

/**
 * Get an atom from the molecule.
 * Parameter: Index.
 *
 * usage:
 * atm = mol:getAtom( 123 )
 */
int molecule::getAtom(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    svt_vector4<Real64> oVec = local2Global( m_pPDB->getPoint( iIndex ) );
    atom* pAtom = new atom( pState );
    pAtom->set( m_pPDB->atom( iIndex ), oVec, iIndex );
    svt_luna<atom>::pushToLua( pAtom, pState );
    return 1;
};
/**
 * Get a series of atoms from the molecule.
 * Parameter: Two indices. The first is the starting atom index and the second the last atom that gets copied (meaning, it will be included). The indeces are 1-based, like usually in Lua.
 * The resulting array is also one based.
 *
 * usage:
 * atm = mol:getAtoms( 123, 135 )
 * atm[5]:print()
 */
int molecule::getAtoms(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndexFrom = luaL_checkinteger(pState, -2);
    int iIndexTo   = luaL_checkinteger(pState, -1);

    if (iIndexFrom <1 || iIndexFrom > (int)m_pPDB->size() || iIndexTo <1 || iIndexTo > (int)m_pPDB->size() || iIndexFrom > iIndexTo )
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    lua_newtable(pState);
    for(int iIndex = iIndexFrom; iIndex <= iIndexTo; iIndex++)
    {
        atom* pAtom = new atom( pState );
        svt_vector4<Real64> oVec = local2Global( m_pPDB->getPoint( iIndex-1 ) );
        pAtom->set( m_pPDB->atom( iIndex-1 ), oVec, iIndex-1 );
        lua_pushinteger(pState, iIndex-iIndexFrom+1);
        svt_luna<atom>::pushToLua( pAtom, pState );
        lua_settable(pState, -3);
    }
    return 1;
};

/**
 * Set an atom in the molecule. The atom contains the index, so it knows where it was retrieved from and will go back to exactly the same position. If a new atom is supposed to be stored in the
 * molecule, please use addAtom.
 * Parameter: Atom
 *
 * usage:
 * atm = mol:getAtom( 123 )
 * atm:x( 10.5 )
 * mol:setAtom( atm )
 */
int molecule::setAtom(lua_State* pState)
{
    if (!checkData()) return 0;

    atom *pAtom = svt_luna<atom>::luaCheck( pState, -1 );

    if (pAtom->getIndex() == -1)
    {
        svtout << "Error: This atom does not come from a molecule object and therefore cannot be used in a setAtom call. Use addAtom to add this atom to the molecule!" << endl;
        return 0;
    }

    svt_point_cloud_atom* pAtomSVT = m_pPDB->getAtom( pAtom->getIndex() );
    unsigned int iSVTIndex = pAtom->getIndex();
    svt_point_cloud_atom* pAtomLUA = pAtom->get();

    // is this actually the atom it was when it came out of the molecule?
    if ( pAtomSVT->getPDBIndex() != pAtomLUA->getPDBIndex() || pAtomSVT->getResidueSeq() != pAtomLUA->getResidueSeq() )
    {
        // OK, it is not the same, then the PDB index is more important, find the atom
        for(unsigned int i=0; i<m_pPDB->size(); i++)
            if (m_pPDB->getAtom(i)->getPDBIndex() == pAtomLUA->getPDBIndex())
            {
                pAtomSVT = m_pPDB->getAtom(i);
                iSVTIndex = i;
            }
    }

    // OK, copy atom
    *pAtomSVT = *pAtom->get();
    svt_vector4<Real64> oVec = global2Local( pAtom->getPoint() );
    m_pPDB->setPoint( iSVTIndex, oVec );
    m_pPDB->getRenAtom( iSVTIndex )->setDisplayMode( pAtom->getDisplayMode() );

    return 0;
};

/**
 * Set an array of atoms in the molecule. The atoms contain the index, so they know where they were retrieved from and will go back to exactly the same position. If a new atom is supposed to be stored in the
 * molecule, please use addAtom.
 * Parameter: Array of atoms
 *
 * usage:
 * atm = mol:getAtoms( 123, 135 )
 * atm[5]:x( 10.5 )
 * mol:setAtoms( atm )
 */
int molecule::setAtoms(lua_State* pState)
{
    if (!checkData()) return 0;

    if (!lua_istable(pState, -1))
    {
        svtout << "Error: Please provide a table/array with atoms to the function." << endl;
        return 0;
    }

    int iLen = lua_objlen(pState, -1);
    for(int i=0;i<iLen;i++)
    {
        lua_pushinteger(pState, i+1);
        lua_gettable(pState, -2);

        atom *pAtom = svt_luna<atom>::luaCheck( pState, -1 );

        svt_point_cloud_atom* pAtomSVT = m_pPDB->getAtom( pAtom->getIndex() );
        unsigned int iSVTIndex = pAtom->getIndex();
        svt_point_cloud_atom* pAtomLUA = pAtom->get();

        // is this actually the atom it was when it came out of the molecule?
        if ( pAtomSVT->getPDBIndex() != pAtomLUA->getPDBIndex() || pAtomSVT->getResidueSeq() != pAtomLUA->getResidueSeq() )
        {
            // OK, it is not the same, then the PDB index is more important, find the atom
            for(unsigned int i=0; i<m_pPDB->size(); i++)
                if (m_pPDB->getAtom(i)->getPDBIndex() == pAtomLUA->getPDBIndex())
                {
                    pAtomSVT = m_pPDB->getAtom(i);
                    iSVTIndex = i;
                }
        }

        // OK, copy atom
        *pAtomSVT = *pAtom->get();
        svt_vector4<Real64> oVec = global2Local( pAtom->getPoint() );
        m_pPDB->setPoint( iSVTIndex, oVec );
        m_pPDB->getRenAtom( iSVTIndex )->setDisplayMode( pAtom->getDisplayMode()-1 );

        lua_pop(pState, 1);
    }

    return 0;
}

/**
 * Add an atom to the molecule. The atom is stored as a new atom in the molecule, will therefore get a new svt index.
 * Parameter: Atom
 *
 * usage:
 * atm = mol:getAtom( 123 )
 * atm:x( 10.5 )
 * mol:addAtom( atm )
 */
int molecule::addAtom(lua_State* pState)
{
    atom *pAtom = svt_luna<atom>::luaCheck( pState, -1 );

    if (m_pPDB == NULL)
    {
        m_pPDB = new svt_pdb();
        m_pPDB->setTransformation( new svt_matrix4f() );
        m_bDelete = true;
        m_pDoc = NULL;
    }

    svt_vector4<Real64> oVec = global2Local( pAtom->getPoint() );
    unsigned int iIndex = m_pPDB->addAtom( *pAtom->get(), oVec );
    m_pPDB->getRenAtom( iIndex )->setDisplayMode( pAtom->getDisplayMode()-1 );
    return 0;
};

/**
 * Add an array of atoms to the molecule. The atoms are stored as new atoms in the molecule, and will therefore get a new svt index.
 * Parameter: Array of atoms.
 *
 * usage:
 * atm = mol:getAtoms( 123, 135 )
 * atm[1]:x( 10.5 )
 * mol:addAtoms( atm )
 */
int molecule::addAtoms(lua_State* pState)
{
    if (!lua_istable(pState, -1))
    {
        svtout << "Error: Please provide a table/array with atoms to the function." << endl;
        return 0;
    }

    if (m_pPDB == NULL)
    {
        m_pPDB = new svt_pdb();
        m_pPDB->setTransformation( new svt_matrix4f() );
        m_bDelete = true;
        m_pDoc = NULL;
    }

    int iLen = lua_objlen(pState, -1);
    for(int i=0;i<iLen;i++)
    {
        lua_pushinteger(pState, i+1);
        lua_gettable(pState, -2);

        atom *pAtom = svt_luna<atom>::luaCheck( pState, -1 );

        svt_vector4<Real64> oVec = global2Local( pAtom->getPoint() );
        unsigned int iIndex = m_pPDB->addAtom( *pAtom->get(), oVec );
        m_pPDB->getRenAtom( iIndex )->setDisplayMode( pAtom->getDisplayMode()-1 );

        lua_pop(pState, 1);
    }

    return 0;
};

/**
 * Blur a molecule
 * Parameter: voxelwidth, resolution
 * Returns: a volume
 *
 * usage:
 * vol = mol:blur( 3.0, 10.0 )
 */
int molecule::blur(lua_State* pState)
{
    if (!checkData()) return 0;

    double fWidth      = luaL_checknumber(pState, -2);
    double fResolution = luaL_checknumber(pState, -1);

    // create new pdb document to incorporate the current transformation
    svt_point_cloud_pdb< svt_vector4<Real64> > oPC = *m_pPDB;
    // adjust coordinates
    svt_vector4<Real64> oVec;
    svt_matrix4<Real64> oMat( *(m_pPDB->getTransformation()) );
    oMat[0][3] *= 1.0E2; oMat[1][3] *= 1.0E2; oMat[2][3] *= 1.0E2;
    oPC = oMat * oPC;
    Real64 fMinX = oPC.getMinXCoord();
    Real64 fMinY = oPC.getMinYCoord();
    Real64 fMinZ = oPC.getMinZCoord();

    // center the structure
    // the center might be different because of the applied transformation (rotations)
    svt_matrix4 <Real64> oTranslation;
    oTranslation.loadIdentity();
    oTranslation.setTranslation(-oPC.geometricCenter());

    int iCurrentTimestep = oPC.getTimestep();
    for(int iTS=0; iTS<oPC.getMaxTimestep(); iTS++)
    {
	oPC.setTimestep( iTS );

	for( unsigned int i=0; i<oPC.size(); i++)
	    oPC[i] = oTranslation * oPC[i];
    }
    oPC.setTimestep( iCurrentTimestep );

    Real64 fDiffX = fMinX - oPC.getMinXCoord();
    Real64 fDiffY = fMinY - oPC.getMinYCoord();
    Real64 fDiffZ = fMinZ - oPC.getMinZCoord();
    svt_volume<Real64>* pVol = oPC.blur( fWidth, fResolution, fDiffX, fDiffY, fDiffZ, false );

    // create a new volume object and push it to the lua interpreter
    volume* pVolume = new volume( NULL );
    pVolume->set( pVol, true );
    svt_luna<volume>::pushToLua( pVolume, pState );

    return 1;
}

/**
 * Compute the rmsd with another molecule object.
 * Parameter: molecule object.
 * Parameter: boolean: align the two structures before rmsd (true/false).
 *
 * usage:
 * rmsd = mol:rmsd(mol2, false)
 */
int molecule::rmsd(lua_State* pState)
{
    if (!checkData()) return 0;

    bool bAlign = true;
    if (lua_toboolean(pState, -1))
        bAlign = true;
    else
        bAlign = false;

    sculptor_document* pDocOrg = g_pWindow->getDocumentList()->getDoc(1);
    svt_vector4<Real64> oOrigin;
    if (pDocOrg)
    {
        svt_vector4f oVec = pDocOrg->getRealPos();
        oVec.multScalar( 1.0E8f );
        oOrigin.set( oVec.x(), oVec.y(), oVec.z() );
    }

    // first document: me
    svt_point_cloud_pdb<svt_vector4<Real64> > oFirst = *this->m_pPDB;
    svt_matrix4< Real64 > oFirstMat;
    if (m_pPDB->getTransformation())
    {
        oFirstMat = *(m_pPDB->getTransformation());
        oFirstMat[0][3] *= 1.0E2;
        oFirstMat[1][3] *= 1.0E2;
        oFirstMat[2][3] *= 1.0E2;
    }
    oFirst = oFirstMat * oFirst;

    // second document: on the stack
    molecule *pMol = svt_luna<molecule>::luaCheck( pState, -2 );
    svt_point_cloud_pdb<svt_vector4<Real64> > oSecond = *pMol->get();

    svt_matrix4< Real64 > oSecondMat;
    if (pMol->get()->getTransformation())
    {
        oSecondMat = *(pMol->get()->getTransformation());
        oSecondMat[0][3] *= 1.0E2;
        oSecondMat[1][3] *= 1.0E2;
        oSecondMat[2][3] *= 1.0E2;
    }
    oSecond = oSecondMat * oSecond;

    Select iSelection = ALL;
    double fRMSD = oFirst.rmsd( oSecond, bAlign, iSelection);

    lua_pushnumber(pState, fRMSD);
    return 1;
};

/**
 * Save the molecule to disk.
 * Parameters: Filename
 *
 * usage:
 * mol:save("test.pdb")
 */
int molecule::save(lua_State* pState)
{
    if (!checkData()) return 0;

    const char *pScr = luaL_checkstring(pState, -1);
    svtQtCallback();

    if (m_pDoc)
        m_pDoc->save( pScr);
    else
        m_pPDB->writePDB( pScr);

    return 0;
};

/**
 * Load an atomic model from disk. The molecule will not get added to the document list of Sculptor (can be done later using the addDoc function). Typically one would prefer to use
 * the sculptor:load() function.
 * Parameters: Filename
 *
 * usage:
 * mol = molecule:new()
 * mol:load("test.pdb")
 */
int molecule::load(lua_State* pState)
{
    const char *pFname = luaL_checkstring(pState, -1);
    svtQtCallback();

    if (m_pPDB == NULL)
    {
        m_pPDB = new svt_pdb();
        m_pPDB->setTransformation( new svt_matrix4f() );
        m_bDelete = true;
        m_pDoc = NULL;
    }

    m_pPDB->loadPDB( pFname );
    return 0;
};

/** Extract the chains, frames and secondary structures of a molecule
 *  usage:  
 *  mol = molecule:new() 
 *  mol:extract(2, "chains", "2-4") --the first parameter is an index to the document list, the second is the 
 *  structure that one wants to extract, and third is the range
 */
// int molecule::extract(lua_State* pState)
// {
//   vector<string> range; 
//   int iSelMode;
//   int pDocidx = luaL_checknumber(pState, -1);
//   const char *pName = luaL_checkstring(pState, -2);
//   const char *pRange = luaL_checkstring(pState, -3);
//   
//   //declare a pdb object
//   svt_point_cloud_pdb< svt_vector4<Real64> >* rPDB;
//   
//   //load the pdb object from the document index
//   rPDB = getDoc(pDocidx);
//   
//   //construct a switch case to initialize iSelMode
//   
//   //create a range from pRange
//   range = svt_expandRange(string(pRange));
//   
//   //call the overloaded extract function
//   extract(range, iSelMode, rPDB);
//   
//   return 0;
// }

/** This function uses fetchPDB to get PDB files loaded to sculptor window
 *  This function can handle variable number of arguments
 *  Usage:
 *  mol = molecule:new()
 *  mol:fetchPDBs("1d3e.pdb","1c8g.pdb")
 */
int molecule::fetchPDBs(lua_State* pState)
{
  vector<string> pdb_list;
  const char *pdb_name;
  
  //check the size of lua stack
  int stack_size = lua_gettop(pState);
  //get the values one by one from lua stack to a string vector
  for (int i = 1; i <= stack_size; i++)
  {
    pdb_name = luaL_checkstring(pState, -(i));
    if ((unsigned int)(string(pdb_name)).rfind(".pdb") != (unsigned int)(string(pdb_name)).npos)
      pdb_list.push_back(string(pdb_name));
    else 
      continue;
  }
 
  //call g_pWindow->queryPDB(oPDB) for each pdb for display
  for (unsigned int j = 0; j < pdb_list.size(); j++)
  {
    g_pWindow->queryPDB(QString(pdb_list[j].c_str()));
  }
  
  return 0;
}

/**
 * Update the rendering of the molecule. Should be called if the data was manipulated internally.
 *
 * usage:
 * mol:updateRendering()
 */
int molecule::updateRendering(lua_State*)
{
    if (!checkData()) return 0;
    if (!checkDoc())  return 0;

    m_pPDB->rebuildDL();
    return 0;
}

/**
 * Create codebook/feature vectors using the neural gas / TRN algorithm. Returns a new "molecule" with the feature vectors.
 *
 * usage:
 * vectors = mol:trn(10)
 */
int molecule::vectorquant(lua_State* pState)
{
    if (!checkData()) return 0;

    svt_clustering_trn< svt_vector4<Real64> > oClusterAlgo;
    molecule *pStart = NULL;
    int iCV = 0;

    if (lua_isuserdata( pState, -1 ) )
    {
        iCV = luaL_checkinteger(pState, -2);
        pStart = svt_luna<molecule>::luaCheck( pState, -1 );
    } else
        iCV = luaL_checkinteger(pState, -1);

    // do the clustering
    svt_point_cloud_pdb<svt_vector4<Real64> > aCodebook = oClusterAlgo.cluster( iCV, 8, *m_pPDB );
    if ( aCodebook.size() != 0 )
        oClusterAlgo.calcConnections( *m_pPDB );
    aCodebook = oClusterAlgo.getCodebook();

    // the molecule data was centered, so the codebook vectors are relative to (0,0,0). We have to move them to the actual origin.
    svt_matrix4<Real64> oMat( *(m_pPDB->getTransformation()) );
    oMat[0][3] *= 1.0E2; oMat[1][3] *= 1.0E2; oMat[2][3] *= 1.0E2;
    aCodebook = oMat * aCodebook;

    // create new molecule
    molecule* pMol = new molecule( pState );
    svt_pdb* pPDB = new svt_pdb( aCodebook );
    pMol->set( pPDB, true );
    svt_luna<molecule>::pushToLua( pMol, pState );
    return 1;
};

/**
 * Set displaymode.
 *
 * usage:
 * mol:setDisplayMode("CARTOON")
 */
int molecule::setDisplayMode(lua_State* pState)
{
    if (!checkData()) return 0;
    if (!checkDoc())  return 0;

    int iDisplayMode = sculptor::getDisplayModeEnum( pState );
    if (m_pDoc)
        m_pDoc->setGlobalMode( iDisplayMode );
    else
        m_pPDB->setDisplayMode( iDisplayMode );
    return 0;
};
/**
 * Set colmapmode.
 *
 * usage:
 * mol:setColmapMode("SOLID", 5)
 */
int molecule::setColmapMode(lua_State* pState)
{
    if (!checkData()) return 0;
    if (!checkDoc())  return 0;

    int iColor = luaL_checkinteger(pState, -1);
    string oStr( luaL_checkstring(pState, -2) );
    int iColmapMode = sculptor::getColmapModeEnum( pState, oStr );
    if (m_pDoc)
        m_pDoc->setGlobalColmap( iColmapMode, iColor );

    return 0;
};

/**
 * ProjectMassCorr. This routine only projects the atoms onto the volume object and calculates the correlation - it will not convolute the molecule with a Gaussian to bring it
 * to the same resolution as the volumetric data.
 *
 * usage:
 * cc = mol:projectMassCorr( volume )
 */
int molecule::projectMassCorr(lua_State* pState)
{
    if (!checkData()) return 0;

    volume *pVol = svt_luna<volume>::luaCheck( pState, -1 );
    svt_volume<Real64>* pMap = pVol->get();
    svt_matrix4<Real64> oMat = *(m_pPDB->getTransformation());
    oMat[0][3] *= 1.0E2;
    oMat[1][3] *= 1.0E2;
    oMat[2][3] *= 1.0E2;
    Real64 fCC = m_pPDB->projectMassCorr(pMap, oMat, false);
    lua_pushnumber(pState, fCC );
    return 1;
}

/**
 * Get atom type information for atom i
 * Parameter: Index.
 *
 * usage:
 * ss = mol:getAtomType( 123 )
 */
int molecule::getAtomType(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    string oStr; oStr += m_pPDB->getAtom(iIndex)->getName();
    lua_pushstring( pState, oStr.c_str() );
    return 1;
}

/**
 * Set atom type information for atom i
 * Parameter: Index, string with type.
 *
 * usage:
 * mol:setAtomType( 123, "H" )
 */
int molecule::setAtomType(lua_State* pState)
{
    if (!checkData()) return 0;

    const char *pName = luaL_checkstring(pState, -1);
    int iIndex = luaL_checkinteger(pState, -2);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    m_pPDB->getAtom(iIndex)->setName(pName);
    return 0;
}

/**
 * Get atom model information for atom i
 * Parameter: Index.
 *
 * usage:
 * model = mol:getAtomModel( 123 )
 */
int molecule::getAtomModel(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    lua_pushinteger( pState, m_pPDB->getAtom(iIndex)->getModel() );
    return 1;
}
/**
 * Set atom model information for atom i
 * Parameter: Index, integer with the new model identifier
 *
 * usage:
 * mol:setAtomModel( 123, 3 )
 */
int molecule::setAtomModel(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -2);
    int iModel = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    m_pPDB->getAtom(iIndex)->setModel(iModel);
    return 0;
}

/**
 * Computes the array of different models
 *
 * usage:
 * mol:calcAtomModels()
 */
int molecule::calcAtomModels(lua_State*)
{
    if (!checkData()) return 0;
    
    m_pPDB->calcAtomModels();
    return 0;
}
 
/**
 * Get secondary structure information from atom i.
 * Parameter: Index.
 *
 * usage:
 * ss = mol:getAtomSecStruct( 123 )
 */
int molecule::getAtomSecStruct(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    string oStr; oStr += m_pPDB->getAtom(iIndex)->getSecStruct();
    lua_pushstring( pState, oStr.c_str() );
    return 1;
}

/**
 * Set remoteness information from atom i (e.g. alpha for a carbon alpha atom).
 * Parameter: Index, String.
 *
 * usage:
 * mol:setAtomRemoteness( 123, "A" )
 */
int molecule::setAtomRemoteness(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex       = luaL_checkinteger(pState, -2);
    const char *pRem = luaL_checkstring(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    m_pPDB->getAtom(iIndex)->setRemoteness(pRem[0]);
    return 0;
}
/**
 * Get remoteness information from atom i (e.g. alpha for a carbon alpha atom).
 * Parameter: Index.
 * Returns: String.
 *
 * usage:
 * rem = mol:getAtomRemoteness( 123 )
 */
int molecule::getAtomRemoteness(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    string oStr; oStr += m_pPDB->getAtom(iIndex)->getRemoteness();
    lua_pushstring( pState, oStr.c_str() );
    return 1;
}

/**
 * Get branch information of atom i.
 * Parameter: Index.
 * Returns: String.
 *
 * usage:
 * branch = mol:getAtomBranch( 123 )
 */
int molecule::getAtomBranch(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    string oStr; oStr += m_pPDB->getAtom(iIndex)->getBranch();
    lua_pushstring( pState, oStr.c_str() );
    return 1;
}
/**
 * Set branch information of atom i.
 * Parameter: Index.
 * Returns: String.
 *
 * usage:
 * branch = mol:setAtomBranch( 123 )
 */
int molecule::setAtomBranch(lua_State* pState)
{
    if (!checkData()) return 0;

    const char *pBra = luaL_checkstring(pState, -1);
    int iIndex       = luaL_checkinteger(pState, -2);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    m_pPDB->getAtom(iIndex)->setBranch( pBra[0] );
    return 0;
}

/**
 * Get alternate location indicator information of atom i.
 * Parameter: Index.
 * Returns: String.
 *
 * usage:
 * alt = mol:getAtomAltLoc( 123 )
 */
int molecule::getAtomAltLoc(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    string oStr; oStr += m_pPDB->getAtom(iIndex)->getAltLoc();
    lua_pushstring( pState, oStr.c_str() );
    return 1;
}
/**
 * Set alternate location indicator information of atom i.
 * Parameter: Index.
 *
 * usage:
 * mol:setAtomAltLoc( 123 )
 */
int molecule::setAtomAltLoc(lua_State* pState)
{
    if (!checkData()) return 0;

    const char *pAlt = luaL_checkstring(pState, -1);
    int iIndex       = luaL_checkinteger(pState, -2);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    m_pPDB->getAtom(iIndex)->setAltLoc( pAlt[0] );
    return 0;
}

/**
 * Get residue name information of atom i (e.g. ALA for alanin).
 * Parameter: Index.
 * Returns: String.
 *
 * usage:
 * res = mol:getAtomResName( 123 )
 */
int molecule::getAtomResName(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    string oStr; oStr += m_pPDB->getAtom(iIndex)->getResname();
    lua_pushstring( pState, oStr.c_str() );
    return 1;
}
/**
 * Set residue name information of atom i (e.g. ALA for alanin).
 * Parameter: Index.
 *
 * usage:
 * mol:getAtomResName( 123 )
 */
int molecule::setAtomResName(lua_State* pState)
{
    if (!checkData()) return 0;

    const char *pRes = luaL_checkstring(pState, -1);
    int iIndex       = luaL_checkinteger(pState, -2);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    m_pPDB->getAtom(iIndex)->setResname( pRes );
    return 0;
}

/**
 * Get residue number information of atom i.
 * Parameter: Index.
 * Returns: Integer.
 *
 * usage:
 * res = mol:getAtomResNum( 123 )
 */
int molecule::getAtomResNum(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    lua_pushinteger( pState, m_pPDB->getAtom(iIndex)->getResidueSeq() );
    return 1;
}
/**
 * Set residue number information of atom i.
 * Parameter: Index.
 *
 * usage:
 * mol:setAtomResNum( 123, 123 )
 */
int molecule::setAtomResNum(lua_State* pState)
{
    if (!checkData()) return 0;

    int iRes   = luaL_checkinteger(pState, -1);
    int iIndex = luaL_checkinteger(pState, -2);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    m_pPDB->getAtom(iIndex)->setResidueSeq( iRes );
    return 0;
}

/**
 * Get chain id information of atom i (e.g. A, B, C, ...).
 * Parameter: Index.
 * Returns: String.
 *
 * usage:
 * chain = mol:getAtomChain( 123 )
 */
int molecule::getAtomChain(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    string oStr; oStr += m_pPDB->getAtom(iIndex)->getChainID();
    lua_pushstring( pState, oStr.c_str() );
    return 1;
}
/**
 * Set chain id information of atom i (e.g. A, B, C, ...).
 * Parameter: Index.
 *
 * usage:
 * mol:setAtomChain( 123, "A" )
 */
int molecule::setAtomChain(lua_State* pState)
{
    if (!checkData()) return 0;

    const char *pCha = luaL_checkstring(pState, -1);
    int iIndex       = luaL_checkinteger(pState, -2);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    m_pPDB->getAtom(iIndex)->setChainID( pCha[0] );
    return 0;
}

/**
 * Get icode (insertion of residues) information of atom i.
 * Parameter: Index.
 * Returns: String.
 *
 * usage:
 * chain = mol:getAtomICode( 123 )
 */
int molecule::getAtomICode(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    string oStr; oStr += m_pPDB->getAtom(iIndex)->getICode();
    lua_pushstring( pState, oStr.c_str() );
    return 1;
}
/**
 * Set icode (insertion of residues) information of atom i.
 * Parameter: Index.
 *
 * usage:
 * mol:setAtomICode( 123 )
 */
int molecule::setAtomICode(lua_State* pState)
{
    if (!checkData()) return 0;

    const char *pIco = luaL_checkstring(pState, -1);
    int iIndex       = luaL_checkinteger(pState, -2);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    m_pPDB->getAtom(iIndex)->setICode( pIco[0] );
    return 0;
}

/**
 * Get occupancy information of atom i.
 * Parameter: Index.
 * Returns: Number.
 *
 * usage:
 * occ = mol:getAtomOccupancy( 123 )
 */
int molecule::getAtomOccupancy(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    lua_pushnumber( pState, m_pPDB->getAtom(iIndex)->getOccupancy() );
    return 1;
}
/**
 * Set occupancy information of atom i.
 * Parameter: Index, Number.
 *
 * usage:
 * mol:setAtomOccupancy( 123, 1.0 )
 */
int molecule::setAtomOccupancy(lua_State* pState)
{
    if (!checkData()) return 0;

    double fOcc = luaL_checknumber(pState, -1);
    int iIndex  = luaL_checkinteger(pState, -2);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    m_pPDB->getAtom(iIndex)->setOccupancy( fOcc );
    return 0;
}

/**
 * Get temperature factor information of atom i.
 * Parameter: Index.
 * Returns: Number.
 *
 * usage:
 * temp = mol:getAtomTempFact( 123 )
 */
int molecule::getAtomTempFact(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    lua_pushnumber( pState, m_pPDB->getAtom(iIndex)->getTempFact() );
    return 1;
}
/**
 * Set temperature factor information of atom i.
 * Parameter: Index, Number.
 *
 * usage:
 * mol:setAtomTempFact( 123, 1.0 )
 */
int molecule::setAtomTempFact(lua_State* pState)
{
    if (!checkData()) return 0;

    double fTmp = luaL_checknumber(pState, -1);
    int iIndex  = luaL_checkinteger(pState, -2);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    m_pPDB->getAtom(iIndex)->setTempFact( fTmp );
    return 0;
}

/**
 * Get note of atom i.
 * Parameter: Index.
 * Returns: String.
 *
 * usage:
 * note = mol:getAtomNote( 123 )
 */
int molecule::getAtomNote(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    string oStr; oStr += m_pPDB->getAtom(iIndex)->getNote();
    lua_pushstring( pState, oStr.c_str() );
    return 1;
}
/**
 * Set note of atom i.
 * Parameter: Index, String.
 *
 * usage:
 * mol:setAtomNote( 123, "ABC" )
 */
int molecule::setAtomNote(lua_State* pState)
{
    if (!checkData()) return 0;

    const char *pNot = luaL_checkstring(pState, -1);
    int iIndex       = luaL_checkinteger(pState, -2);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }
    if (strlen( pNot ) < 3)
    {
        svtout << "Error: Note has to be three characters long!" << endl;
        return 0;
    }

    iIndex--;
    m_pPDB->getAtom(iIndex)->setNote( pNot );
    return 0;
}

/**
 * Get segment id of atom i.
 * Parameter: Index.
 * Returns: String.
 *
 * usage:
 * seg = mol:getAtomSegID( 123 )
 */
int molecule::getAtomSegID(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    string oStr; oStr += m_pPDB->getAtom(iIndex)->getSegmentID();
    lua_pushstring( pState, oStr.c_str() );
    return 1;
}
/**
 * Set segment id of atom i.
 * Parameter: Index, String.
 *
 * usage:
 * mol:setAtomSegID( 123, "ABCD" )
 */
int molecule::setAtomSegID(lua_State* pState)
{
    if (!checkData()) return 0;

    const char *pSeg = luaL_checkstring(pState, -1);
    int iIndex       = luaL_checkinteger(pState, -2);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }
    if (strlen( pSeg ) < 4)
    {
        svtout << "Error: SegmentID has to be four characters long!" << endl;
        return 0;
    }

    iIndex--;
    m_pPDB->getAtom(iIndex)->setSegmentID( pSeg );
    return 0;
}

/**
 * Get element information of atom i.
 * Parameter: Index.
 * Returns: String.
 *
 * usage:
 * element = mol:getAtomElement( 123 )
 */
int molecule::getAtomElement(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    string oStr; oStr += m_pPDB->getAtom(iIndex)->getElement();
    lua_pushstring( pState, oStr.c_str() );
    return 1;
}
/**
 * Set element information of atom i (at least two characters, please!)"
 * Parameter: Index, String.
 *
 * usage:
 * mol:setAtomElement( 123, " H" )
 */
int molecule::setAtomElement(lua_State* pState)
{
    if (!checkData()) return 0;

    const char *pEle = luaL_checkstring(pState, -1);
    int iIndex       = luaL_checkinteger(pState, -2);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }
    if (strlen( pEle ) < 2)
    {
        svtout << "Error: Element information has to be two characters long!" << endl;
        return 0;
    }

    iIndex--;
    m_pPDB->getAtom(iIndex)->setElement( pEle );
    return 0;
}

/**
 * Get charge of atom i.
 * Parameter: Index.
 * Returns: String.
 *
 * usage:
 * element = mol:getAtomCharge( 123 )
 */
int molecule::getAtomCharge(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    string oStr; oStr += m_pPDB->getAtom(iIndex)->getCharge();
    lua_pushstring( pState, oStr.c_str() );
    return 1;
}
/**
 * Set charge of atom i (two characters at least!).
 * Parameter: Index, String.
 *
 * usage:
 * mol:setAtomCharge( 123 )
 */
int molecule::setAtomCharge(lua_State* pState)
{
    if (!checkData()) return 0;

    const char *pCha = luaL_checkstring(pState, -1);
    int iIndex       = luaL_checkinteger(pState, -2);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    m_pPDB->getAtom(iIndex)->setCharge( pCha );
    return 0;
}

/**
 * Is atom i a hydrogen?
 * Parameter: Index.
 * Returns: Boolean.
 *
 * usage:
 * hydro = mol:isAtomHydrogen( 123 )
 */
int molecule::isAtomHydrogen(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    lua_pushboolean( pState, m_pPDB->getAtom(iIndex)->isHydrogen() );
    return 1;
}

/**
 * Is atom i a codebook vector?
 * Parameter: Index.
 * Returns: Boolean.
 *
 * usage:
 * hydro = mol:isAtomQPDB( 123 )
 */
int molecule::isAtomQPDB(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    lua_pushboolean( pState, m_pPDB->getAtom(iIndex)->isQPDB() );
    return 1;
}

/**
 * Is atom i part of a water molecule?
 * Parameter: Index.
 * Returns: Boolean.
 *
 * usage:
 * hydro = mol:isAtomWater( 123 )
 */
int molecule::isAtomWater(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    lua_pushboolean( pState, m_pPDB->getAtom(iIndex)->isWater() );
    return 1;
}

/**
 * Is atom i a carbon alpha?
 * Parameter: Index.
 * Returns: Boolean.
 *
 * usage:
 * hydro = mol:isAtomCA( 123 )
 */
int molecule::isAtomCA(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    lua_pushboolean( pState, m_pPDB->getAtom(iIndex)->isCA() );
    return 1;
}

/**
 * Is atom i on the backbone?
 * Parameter: Index.
 * Returns: Boolean.
 *
 * usage:
 * hydro = mol:isAtomBackbone( 123 )
 */
int molecule::isAtomBackbone(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    lua_pushboolean( pState, m_pPDB->getAtom(iIndex)->isBackbone() );
    return 1;
}

/**
 * Is atom i a nucleotide?
 * Parameter: Index.
 * Returns: Boolean.
 *
 * usage:
 * hydro = mol:isAtomNucleotide( 123 )
 */
int molecule::isAtomNucleotide(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    lua_pushboolean( pState, m_pPDB->getAtom(iIndex)->isNucleotide() );
    return 1;
}

/**
 * Get mass of atom i.
 * Parameter: Index.
 * Returns: Number.
 *
 * usage:
 * mass = mol:getAtomMass( 123 )
 */
int molecule::getAtomMass(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    lua_pushnumber( pState, m_pPDB->getAtom(iIndex)->getMass() );
    return 1;
}
/**
 * Set mass of atom i.
 * Parameter: Index, Number.
 *
 * usage:
 * mol:setAtomMass( 123, 1.2 )
 */
int molecule::setAtomMass(lua_State* pState)
{
    if (!checkData()) return 0;

    Real64 fMass = luaL_checknumber(pState, -1);
    int iIndex   = luaL_checkinteger(pState, -2);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    m_pPDB->getAtom(iIndex)->setMass( fMass );
    return 0;
}

/**
 * Adjust the atomic mass based on a (simple) periodic table.
 * Parameter: Index.
 *
 * usage:
 * mol:adjustAtomMass( 123 )
 */
int molecule::adjustAtomMass(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex   = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    m_pPDB->getAtom(iIndex)->adjustMass( );
    return 0;
}

/**
 * Get vdw radius of atom i.
 * Parameter: Index.
 * Returns: Number.
 *
 * usage:
 * rad = mol:getAtomVDWRadius( 123 )
 */
int molecule::getAtomVDWRadius(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex = luaL_checkinteger(pState, -1);

    if (iIndex <1 || iIndex > (int)m_pPDB->size())
    {
        svtout << "Error: Index out of range!" << endl;
        return 0;
    }

    iIndex--;
    lua_pushnumber( pState, m_pPDB->getAtom(iIndex)->getVDWRadius() );
    return 1;
}

/**
 * Get a model from the molecule.
 * Parameter: Modelnumber.
 * Returns: Molecule object.
 *
 * usage:
 * mol_b = mol:getModel( 1 )
 */
int molecule::getModel(lua_State* pState)
{
    if (!checkData()) return 0;

    int iModel = luaL_checkinteger(pState, -1);

    molecule* pMol = new molecule( pState );
    svt_point_cloud_pdb<svt_vector4<Real64> > oModel = *m_pPDB->getModel( iModel );

    svt_matrix4< Real64 > oMat( *(m_pPDB->getTransformation()) );
    oMat[0][3] *= 1.0E2;
    oMat[1][3] *= 1.0E2;
    oMat[2][3] *= 1.0E2;
    oModel = oMat * oModel;

    svt_pdb* pPDB = new svt_pdb( oModel );
    pMol->set( pPDB );
    svt_luna<molecule>::pushToLua( pMol, pState );
    return 1;
};

/**
 * Get a chain from the molecule.
 * Parameter: Chain ID.
 * Returns: Molecule object.
 *
 * usage:
 * mol_b = mol:getChain( "A" )
 */
int molecule::getChain(lua_State* pState)
{
    if (!checkData()) return 0;

    const char* pChain = luaL_checkstring(pState, -1);

    molecule* pMol = new molecule( pState );
    svt_point_cloud_pdb<svt_vector4<Real64> > oChain = m_pPDB->getChain( pChain[0] );

    svt_matrix4< Real64 > oMat( *(m_pPDB->getTransformation()) );
    oMat[0][3] *= 1.0E2;
    oMat[1][3] *= 1.0E2;
    oMat[2][3] *= 1.0E2;
    oChain = oMat * oChain;

    svt_pdb* pPDB = new svt_pdb( oChain );
    pMol->set( pPDB );
    svt_luna<molecule>::pushToLua( pMol, pState );
    return 1;
};

/**
 * Add another molecule.
 * Parameter: Molecule object.
 *
 * usage:
 * mol_a:add( mol_b )
 */
int molecule::add(lua_State* pState)
{
    molecule *pMol = svt_luna<molecule>::luaCheck( pState, -1 );

    if (m_pPDB == NULL)
    {
        m_pPDB = new svt_pdb( *pMol->get() );
        m_pPDB->setTransformation( new svt_matrix4f() );
        m_bDelete = true;
        m_pDoc = NULL;

        return 0;
    }

    m_pPDB->addPointCloud( *pMol->get() );
    return 0;
}

/**
 * Add a bond between two atoms.
 * Parameter: Index a and index b of the two atoms.
 *
 * usage:
 * mol:addBond( 5, 10 )
 */
int molecule::addBond(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex_A = luaL_checkinteger(pState, -2) -1;
    int iIndex_B = luaL_checkinteger(pState, -1) -1;

    m_pPDB->addBond( iIndex_A, iIndex_B );
    return 0;
};
/**
 * Is there a bond between two atoms?
 * Parameter: Index a and index b of the two atoms.
 *
 * usage:
 * bond = mol:isBond( 5, 10 )
 */
int molecule::isBond(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex_A = luaL_checkinteger(pState, -2) -1;
    int iIndex_B = luaL_checkinteger(pState, -1) -1;

    lua_pushboolean( pState, m_pPDB->isBond( iIndex_A, iIndex_B ) );
    return 1;
};
/**
 * Remove a bond between two atoms.
 * Parameter: Index a and index b of the two atoms.
 *
 * usage:
 * mol:delBond( 5, 10 )
 */
int molecule::delBond(lua_State* pState)
{
    if (!checkData()) return 0;

    int iIndex_A = luaL_checkinteger(pState, -2) -1;
    int iIndex_B = luaL_checkinteger(pState, -1) -1;

    if (m_pPDB->isBond( iIndex_A, iIndex_B ))
    {
        m_pPDB->delBond( iIndex_A, iIndex_B );
        return 0;
    }
    if (m_pPDB->isBond( iIndex_B, iIndex_A ))
    {
        m_pPDB->delBond( iIndex_B, iIndex_A );
        return 0;
    }

    svtout << "Error: There is no bond between atoms " << iIndex_A << " and " << iIndex_B << endl;
    return 0;
};

/**
 * Remove all atoms.
 *
 * usage:
 * mol:delAllAtoms( )
 */
int molecule::delAllAtoms(lua_State*)
{
    if (!checkData()) return 0;

    m_pPDB->deleteAllAtoms();
    return 0;
};
/**
 * Remove all bonds.
 *
 * usage:
 * mol:delAllBonds( )
 */
int molecule::delAllBonds(lua_State*)
{
    if (!checkData()) return 0;

    m_pPDB->deleteAllBonds();
    return 0;
};

/**
 * Match two point-clouds. This function should not be used directly on atomic models, but only on feature-point-clouds. First use vector-quantization to extract a small number of feature
 * points from both data sets / models and then match those feature-points. The resulting transformation matrix can then be applied to the original atomic models. Returns an array of matrices, with the
 * first matrix representing the most likely match.
 * Parameters: Other molecule object, tolerance for anchor point determination (15.0), nearest neighbor matching zone (12.0), zonesize (3), wildcards (0)
 *
 * usage:
 * mats = mol_a:match( mol_b, 15, 12, 3, 0 )
 * realmol_a.setTrans( mats[1] )
 */
int molecule::match(lua_State* pState)
{
    if (!checkData()) return 0;

    molecule *pMol = svt_luna<molecule>::luaCheck( pState, -5 );

    // parameters
    Real64 fLambda           = luaL_checknumber(pState, -4);
    Real64 fGamma            = luaL_checknumber(pState, -3);
    unsigned int iZoneSize   = luaL_checkinteger(pState, -2);
    unsigned int iMaxNoMatch = luaL_checkinteger(pState, -1);
    Real64 fSkipPenalty      = 1.0;

    m_pPDB->setLambda( fLambda );
    m_pPDB->setGamma( fGamma );
    m_pPDB->setZoneSize( iZoneSize );
    m_pPDB->setWildcards( iMaxNoMatch );
    m_pPDB->setSkipPenalty( fSkipPenalty );
    m_pPDB->setRuns( 3 );

    // now match
    vector< svt_matchResult<Real64> > oMatches;
    m_pPDB->match( *pMol->get(), oMatches );

    if (oMatches.size() == 0)
    {
        svtout << "Error: No matches found!" << endl;
        return 0;
    }

    lua_newtable(pState);
    for(unsigned int i=0; i < oMatches.size(); i++)
    {
        matrix4* pMat = new matrix4( pState );
        pMat->set( oMatches[i].getMatrix() );

        lua_pushinteger(pState, i+1);
        svt_luna<matrix4>::pushToLua( pMat, pState );
        lua_settable(pState, -3);
    }
    return 1;
}

/**
 * Flex a molecule according to the coordinates of two feature-point sets. The first set describes the molecule, the other one the target configuration (for example from an
 * em map).
 * Parameters: Two molecule objects
 * Returns: New molecule object
 *
 * usage:
 * flex = mol:flexing( trn_a, trn_b )
 */
int molecule::flexing(lua_State* pState)
{
    if (!checkData()) return 0;

    molecule *pTrnA = svt_luna<molecule>::luaCheck( pState, -2 );
    molecule *pTrnB = svt_luna<molecule>::luaCheck( pState, -1 );

    // interpolate/flexing works on svt_point_cloud_pdbs and not svt_pdb, so it does not know anything about the transformations, we have to apply those first...
    svt_point_cloud_pdb<svt_vector4<Real64> > oPDB( *m_pPDB );
    svt_matrix4< Real64 > oMat( *(m_pPDB->getTransformation()) );
    oMat[0][3] *= 1.0E2;
    oMat[1][3] *= 1.0E2;
    oMat[2][3] *= 1.0E2;
    oPDB = oMat * oPDB;

    // OK, now create an interpolate object...
    svt_flexing_inverse_dist_weighting oInterpolIDW( oPDB, *pTrnA->get(), *pTrnB->get(),false,"Cartesian","none","Hybrid",4 );

    // now interpolate and create new lua object
    molecule* pMol = new molecule( pState );
    svt_pdb* pPDB = new svt_pdb( oInterpolIDW.interpolate() );
    pMol->set( pPDB, true );
    svt_luna<molecule>::pushToLua( pMol, pState );
    return 1;
}

const char molecule::className[] = "molecule";
const svt_luna<molecule>::RegType molecule::methods[] =
{
    { "size",              &molecule::size               },
    { "save",              &molecule::save               },
    { "load",              &molecule::load               },
    { "rmsd",              &molecule::rmsd               },
    { "blur",              &molecule::blur               },
    { "getTrans",          &molecule::getTrans           },
    { "setTrans",          &molecule::setTrans           },
    { "getAtom",           &molecule::getAtom            },
    { "getAtoms",          &molecule::getAtoms           },
    { "setAtom",           &molecule::setAtom            },
    { "setAtoms",          &molecule::setAtoms           },
    { "updateRendering",   &molecule::updateRendering    },
    { "addAtom",           &molecule::addAtom            },
    { "addAtoms",          &molecule::addAtoms           },
    { "addDoc",            &molecule::addDoc             },
    { "vectorquant",       &molecule::vectorquant        },
    { "setDisplayMode",    &molecule::setDisplayMode     },
    { "setColmapMode",     &molecule::setColmapMode      },
    { "projectMassCorr",   &molecule::projectMassCorr    },
    { "getAtomSecStruct",  &molecule::getAtomSecStruct   },
    { "getAtomType",       &molecule::getAtomType        },
    { "setAtomType",       &molecule::setAtomType        },
    { "getAtomModel",      &molecule::getAtomModel       },
    { "setAtomModel",      &molecule::setAtomModel       },
    { "calcAtomModels",    &molecule::calcAtomModels     },
    { "getAtomRemoteness", &molecule::getAtomRemoteness  },
    { "setAtomRemoteness", &molecule::setAtomRemoteness  },
    { "getAtomBranch",     &molecule::getAtomBranch      },
    { "setAtomBranch",     &molecule::setAtomBranch      },
    { "getAtomAltLoc",     &molecule::getAtomAltLoc      },
    { "setAtomAltLoc",     &molecule::setAtomAltLoc      },
    { "getAtomResName",    &molecule::getAtomResName     },
    { "setAtomResName",    &molecule::setAtomResName     },
    { "getAtomResNum",     &molecule::getAtomResNum      },
    { "setAtomResNum",     &molecule::setAtomResNum      },
    { "getAtomChain",      &molecule::getAtomChain       },
    { "setAtomChain",      &molecule::setAtomChain       },
    { "getAtomICode",      &molecule::getAtomICode       },
    { "setAtomICode",      &molecule::setAtomICode       },
    { "getAtomOccupancy",  &molecule::getAtomOccupancy   },
    { "setAtomOccupancy",  &molecule::setAtomOccupancy   },
    { "getAtomTempFact",   &molecule::getAtomTempFact    },
    { "setAtomTempFact",   &molecule::setAtomTempFact    },
    { "getAtomNote",       &molecule::getAtomNote        },
    { "setAtomNote",       &molecule::setAtomNote        },
    { "getAtomSegID",      &molecule::getAtomSegID       },
    { "setAtomSegID",      &molecule::setAtomSegID       },
    { "getAtomElement",    &molecule::getAtomElement     },
    { "setAtomElement",    &molecule::setAtomElement     },
    { "getAtomCharge",     &molecule::getAtomCharge      },
    { "setAtomCharge",     &molecule::setAtomCharge      },
    { "isAtomHydrogen",    &molecule::isAtomHydrogen     },
    { "isAtomQPDB",        &molecule::isAtomQPDB         },
    { "isAtomWater",       &molecule::isAtomWater        },
    { "isAtomCA",          &molecule::isAtomCA           },
    { "isAtomBackbone",    &molecule::isAtomBackbone     },
    { "isAtomNucleotide",  &molecule::isAtomNucleotide   },
    { "getAtomMass",       &molecule::getAtomMass        },
    { "setAtomMass",       &molecule::setAtomMass        },
    { "adjustAtomMass",    &molecule::adjustAtomMass     },
    { "getAtomVDWRadius",  &molecule::getAtomVDWRadius   },
    { "getModel",          &molecule::getModel           },
    { "getChain",          &molecule::getChain           },
    { "add",               &molecule::add                },
    { "addBond",           &molecule::addBond            },
    { "isBond",            &molecule::isBond             },
    { "delBond",           &molecule::delBond            },
    { "delAllBonds",       &molecule::delAllBonds        },
    { "delAllAtoms",       &molecule::delAllAtoms        },
    { "match",             &molecule::match              },
    { "flexing",           &molecule::flexing            },
    { "fetchPDBs",         &molecule::fetchPDBs          },
    { 0,                   0                             }
};

///////////////////////////////////////////////////////////////////////////////
// LUA BINDINGS: VOLUME
///////////////////////////////////////////////////////////////////////////////

/**
 * The volume class encapsulates a single volumetric data set
 */
volume::volume(lua_State * pState)
{
    m_pVol = NULL;
    m_pDoc = NULL;
    m_bDelete = false;

    if (pState == NULL)
        return;

    if ( !lua_isnil( pState, -1 ) && !lua_isnil( pState, -2 ) && !lua_isnil( pState, -3 ) )
    {
        int iX = luaL_checkinteger(pState, -3);
        int iY = luaL_checkinteger(pState, -2);
        int iZ = luaL_checkinteger(pState, -1);

        m_pVol = new svt_volume<Real64>(iX, iY, iZ);
        m_bDelete = true;
    }
};
volume::~volume()
{
    if (m_bDelete && m_pVol != NULL)
        delete m_pVol;
};

/**
 * Allocate memory. Attention, the old content of the molecule will get erased!
 * Parameter: x,y,z size
 *
 * usage:
 * vol:allocate(10,8,12)
 */
int volume::allocate(lua_State* pState)
{
    int iX = luaL_checkinteger(pState, -3);
    int iY = luaL_checkinteger(pState, -2);
    int iZ = luaL_checkinteger(pState, -1);

    m_bDelete = true;

    if (m_pVol == NULL)
        m_pVol = new svt_volume<Real64>(iX, iY, iZ);
    else
        m_pVol->allocate(iX,iY,iZ);
    return 0;
};

/**
 * Set the svt volume data that is encapsulated by this object
 */
void volume::set( svt_volume<Real64>* pVol, bool bDelete)
{
    m_bDelete = bDelete;
    m_pVol = pVol;
};
/**
 * Set the sculptor document that is encapsulated by this object
 */
void volume::setDoc( situs_document* pDoc)
{
    m_pDoc = pDoc;
};

/**
 * Get the svt volume data that is encapsulated by this object
 */
svt_volume<Real64>* volume::get( )
{
    return m_pVol;
};
/**
 * Get the sculptor document that is encapsulated by this object
 */
situs_document* volume::getDoc( )
{
    return m_pDoc;
};

/**
 * Check if the document is present. If the molecule is a local molecule, created in Lua, it will not be part of the document list and will not be rendered. All rendering
 * functions should therefore call this function here to make sure that everything is setup correctly.
 */
bool volume::checkDoc()
{
    if (m_pDoc == NULL)
    {
        svtout << "Sorry, only documents get rendered, please use addDoc first." << endl;
        return false;
    }

    return true;
};

/**
 * Check if data is present.
 */
bool volume::checkData()
{
    if (!m_pVol)
    {
        svtout << "Error: The object is empty, does not contain any data!" << endl;
        return false;
    }

    return true;
};


/**
 * Set the isosurface threshold level.
 * Parameter: threshold level
 *
 * usage:
 * vol:setIsoThreshold( 1.0 )
 */
int volume::setIsoThreshold(lua_State* pState)
{
    if (!checkData()) return 0;
    if (!checkDoc())  return 0;

    double fLevel = luaL_checknumber(pState, -1);

    svt_volume_compact<Real64>* pVolumeMC = m_pDoc->getIso();

    pVolumeMC->setIsolevel(fLevel);
    pVolumeMC->setStep(1);
    pVolumeMC->recalcMesh();
    pVolumeMC->setVisible(true);

    if ( m_pDoc->getPropDlg() != NULL )
	( (prop_situs*)(m_pDoc->getPropDlg()) )->updateTabIso();

    return 0;
}

/**
 * Set wireframe
 * Parameter: bool true/false
 *
 * usage:
 * vol:setWireframe( true )
 */
int volume::setWireframe(lua_State* pState)
{
    if (!checkData()) return 0;
    if (!checkDoc())  return 0;

    if ( lua_isboolean( pState, -1 ) )
    {
        bool bWire = lua_toboolean( pState, -1 );

        m_pDoc->sSetIsoWireframe( bWire );

	if ( m_pDoc->getPropDlg() != NULL )
	    ( (prop_situs*)(m_pDoc->getPropDlg()) )->updateTabIso();

    } else

        svtout << "Error: parameter missing: setWireframe( boolean )" << endl;

    return 0;
};

/**
 * Set color
 * Parameter: red, green, blue (0.0 - 1.0)
 *
 * usage:
 * vol:setColor( 1.0, 1.0, 1.0 )
 */
int volume::setColor(lua_State* pState)
{
    if (!checkData()) return 0;
    if (!checkDoc())  return 0;

    double fR = luaL_checknumber(pState, -3);
    double fG = luaL_checknumber(pState, -2);
    double fB = luaL_checknumber(pState, -1);
    svt_volume_compact<Real64>* pVolumeMC = m_pDoc->getIso();

    svt_color* pColor = pVolumeMC->getProperties()->getColor();
    pColor->setR(fR);
    pColor->setG(fG);
    pColor->setB(fB);
    pColor = pVolumeMC->getProperties()->getWireframeColor();
    pColor->setR(fR);
    pColor->setG(fG);
    pColor->setB(fB);
    pVolumeMC->rebuildDL();

    if ( m_pDoc->getPropDlg() != NULL )
	( (prop_situs*)(m_pDoc->getPropDlg()) )->updateTabIso();

    return 0;
};

/**
 * Get a voxel value.
 * Parameter: x,y,z index
 *
 * usage:
 * voxel = vol:getValue(10,8,12)
 */
int volume::getValue(lua_State* pState)
{
    if (!checkData()) return 0;

    int iX = luaL_checkinteger(pState, -3);
    int iY = luaL_checkinteger(pState, -2);
    int iZ = luaL_checkinteger(pState, -1);

    double fValue = m_pVol->getValue( iX, iY, iZ );

    lua_pushnumber(pState, fValue);
    return 1;
}

/**
 * Get a voxel value. This function takes a normal angstroem world coordinate and interpolates the voxel values tri-linearly.
 * Parameter: x,y,z angstroem coordinate
 *
 * usage:
 * voxel = vol:getIntValue(11.5,8.24,12.21)
 */
int volume::getIntValue(lua_State* pState)
{
    if (!checkData()) return 0;

    double fX = luaL_checknumber(pState, -3);
    double fY = luaL_checknumber(pState, -2);
    double fZ = luaL_checknumber(pState, -1);

    fX -= m_pVol->getGridX();
    fY -= m_pVol->getGridY();
    fZ -= m_pVol->getGridZ();

    fX /= m_pVol->getWidth();
    fY /= m_pVol->getWidth();
    fY /= m_pVol->getWidth();

    double fValue = m_pVol->getIntValue( fX, fY, fZ );

    lua_pushnumber(pState, fValue);
    return 1;
}

/**
 * Set a voxel value.
 * Parameter: x,y,z index
 * Parameter: value
 *
 * usage:
 * vol:setValue(10,8,12, 0.567)
 */
int volume::setValue(lua_State* pState)
{
    if (!checkData()) return 0;

    int iX = luaL_checkinteger(pState, -4);
    int iY = luaL_checkinteger(pState, -3);
    int iZ = luaL_checkinteger(pState, -2);
    double fValue = luaL_checknumber(pState, -1);

    m_pVol->setValue( iX, iY, iZ, fValue );
    if (m_pDoc)
        m_pDoc->dataChanged();
    return 0;
}

/**
 * Add the volume data set to the list of documents.
 * Parameter: String with the name
 *
 * Usage:
 * addDoc( "test.situs" )
 */
int volume::addDoc(lua_State* pState)
{
    if (!checkData()) return 0;

    const char* pFname = luaL_checkstring(pState, -1);

    g_pWindow->getScene()->switchToData();
    situs_document* pSD = new situs_document(g_pWindow->getScene()->getDataTopNode(), string(pFname), m_pVol);

    // the local copy needs to be destroyed
    if (m_bDelete)
    {
        delete m_pVol;
        m_bDelete = false;
    }
    m_pVol = pSD->getVolume();

    // document position
    svt_vector4f oDocOrigin = pSD->getRealPos();
    oDocOrigin.multScalar( 1.0E8f );
    pSD->getNode()->setPos(new svt_pos(oDocOrigin.x(),oDocOrigin.y(),oDocOrigin.z()));
    pSD->storeInitMat();
    pSD->setChanged();

    // ok, now add the thing to sculptor
    g_pWindow->addDocSimple((sculptor_document*)pSD);
    g_pWindow->adjustOrigin();

    m_pDoc = pSD;

    return 0;
};

/**
 * Get the number of voxels in the volume
 * Returns a number.
 */
int volume::size(lua_State* pState)
{
    if (!checkData()) return 0;

    lua_pushnumber(pState, m_pVol->getSizeX() * m_pVol->getSizeY() * m_pVol->getSizeZ() );
    return 1;
}

/**
 * Copies the current volume data set. It will not automatically get added to the list of loaded documents.
 *
 * usage:
 * volCopy = vol:copy()
 */
int volume::copy(lua_State* pState)
{
    if (!checkData()) return 0;

    svt_volume<Real64>* pData = new svt_volume<Real64>(*m_pVol);
    volume* pVolume = new volume( NULL );
    pVolume->set( pData, true );
    svt_luna<volume>::pushToLua( pVolume, pState );
    return 1;
}

/**
 * Crop the volume.
 * Parameters: min/max x, min/max y, min/max z. Indices starting with 1, consistently with the practice in Lua, but different to C!
 *
 * usage:
 * vol:crop(2,12,5,24,3,34)
 */
int volume::crop(lua_State* pState)
{
    if (!checkData()) return 0;

    int iMinX = luaL_checkinteger(pState, -6);
    int iMaxX = luaL_checkinteger(pState, -5);
    int iMinY = luaL_checkinteger(pState, -4);
    int iMaxY = luaL_checkinteger(pState, -3);
    int iMinZ = luaL_checkinteger(pState, -2);
    int iMaxZ = luaL_checkinteger(pState, -1);

    if (iMinX < 1 || iMinX > iMaxX || iMinY < 1 || iMinY > iMaxY || iMinZ < 1 || iMinZ > iMaxZ || iMaxX > (int)m_pVol->getSizeX() || iMaxY > (int)m_pVol->getSizeY() || iMaxZ > (int)m_pVol->getSizeZ() )
    {
        svtout << "Error: Can only crop within the limits of the volume!" << endl;
        return 0;
    }

    if (m_pDoc == NULL)
        m_pVol->crop( iMinX-1, iMaxX-1, iMinY-1, iMaxY-1, iMinZ-1, iMaxZ-1 );
    else
        m_pDoc->crop( iMinX-1, iMaxX-1, iMinY-1, iMaxY-1, iMinZ-1, iMaxZ-1 );

    return 0;
}

/**
 * Update the rendering of the volume data. Should be called if the data was manipulated internally.
 *
 * usage:
 * vol:updateRendering()
 */
int volume::updateRendering(lua_State*)
{
    if (!checkData()) return 0;
    if (!checkDoc())  return 0;

    m_pDoc->updateRedraw( true);
    return 0;
}

/**
 * Save the volume to disk.
 * Parameters: Filename
 *
 * usage:
 * vol:save("test.situs")
 */
int volume::save(lua_State* pState)
{
    if (!checkData()) return 0;

    const char *pScr = luaL_checkstring(pState, -1);
    svtQtCallback();

    if (m_pDoc)
        m_pDoc->save( pScr);
    else
        m_pVol->save( pScr);

    return 0;
};

/**
 * Load a volume from disk. The volume will not get added to the document list of Sculptor (can be done later using the addDoc function). Typically one would prefer to use
 * the sculptor:load() function.
 * Parameters: Filename
 *
 * usage:
 * vol = volumw:new()
 * vol:load("test.situs")
 */
int volume::load(lua_State* pState)
{
    const char *pFname = luaL_checkstring(pState, -1);
    svtQtCallback();

    if (m_pVol == NULL)
    {
        m_pVol = new svt_volume<Real64>;
        m_bDelete = true;
    }

    m_pVol->load( pFname );
    return 0;
};

/**
 * Create codebook/feature vectors using the neural gas / TRN algorithm. Returns a new "molecule" with the feature vectors. Can start from an already existing configuration and
 * will in that case also return a matched feature point set (can be used to flex the molecule).
 *
 * usage:
 * vectors = vol:vectorquant(10)
 */
int volume::vectorquant(lua_State* pState)
{
    if (!checkData()) return 0;

    molecule *pStart = NULL;
    int iCV = 0;

    if (lua_isuserdata( pState, -1 ) )
    {
        iCV = luaL_checkinteger(pState, -2);
        pStart = svt_luna<molecule>::luaCheck( pState, -1 );
    } else
        iCV = luaL_checkinteger(pState, -1);

    svt_point_cloud_pdb<svt_vector4<Real64> > aCodebook;

    // do the normal trn clustering
    if (pStart == NULL)
    {
        svt_clustering_trn< svt_vector4<Real64> > oClusterAlgo;
        aCodebook = oClusterAlgo.cluster( iCV, 8, *m_pVol );
        if ( aCodebook.size() != 0 )
            oClusterAlgo.calcConnections( *m_pVol );
        aCodebook = oClusterAlgo.getCodebook();
    }
    // or do the fam clustering
    if (pStart != NULL)
    {
        svt_point_cloud_pdb<svt_vector4<Real64> > oPDB( *pStart->get() );
        svt_matrix4< Real64 > oMat( *(pStart->get()->getTransformation()) );
        oMat[0][3] *= 1.0E2;
        oMat[1][3] *= 1.0E2;
        oMat[2][3] *= 1.0E2;
        oPDB = oMat * oPDB;

        svt_clustering_fam< svt_vector4<Real64> > oClusterAlgo;
        oClusterAlgo.setSystem( oPDB );
        oClusterAlgo.train( *m_pVol );
        aCodebook = oClusterAlgo.getCodebook();
    }

    // create new molecule
    molecule* pMol = new molecule( pState );
    svt_pdb* pPDB = new svt_pdb( aCodebook );
    pMol->set( pPDB, true );
    svt_luna<molecule>::pushToLua( pMol, pState );
    return 1;
};

/**
 * Calculate the correlation with another volume object.
 * Parameters: Other volume object. The second parameter specifies if the correlation coefficient should only be computed under the current molecule (aka local correlation), or over the entire volume.
 *
 * usage:
 * cc = vol_a:correlation( vol_b )
 */
int volume::correlation(lua_State* pState)
{
    if (!checkData()) return 0;

    svt_volume<Real64>* pMap = NULL;
    bool bMask = false;
    if ( lua_isboolean( pState, -1 ) )
    {
        bMask = lua_toboolean( pState, -1 );
        volume *pVol = svt_luna<volume>::luaCheck( pState, -2 );
        pMap = pVol->get();
    } else {
        volume *pVol = svt_luna<volume>::luaCheck( pState, -1 );
        pMap = pVol->get();
    }

    if (pMap->getWidth() != m_pVol->getWidth())
    {
        svtout << "Error: Voxelwidth are different between the two maps. Please re-interpolate one of the maps to match the other one, using the interpolate command!" << endl;
        return 0;
    }

    Real64 fCC = pMap->correlation( *m_pVol, bMask );
    lua_pushnumber(pState, fCC );
    return 1;
};

/**
 * Delete a spherical subregion
 * Parameters: center voxel coordinate for the spherical region x,y,z and radius of sphere
 *
 * usage:
 * vol:cutSphere( 10,12,8, 3.5 )
 */
int volume::cutSphere( lua_State* pState )
{
    if (!checkData()) return 0;

    int iX = luaL_checkinteger(pState, -4);
    int iY = luaL_checkinteger(pState, -3);
    int iZ = luaL_checkinteger(pState, -2);
    double fRadius = luaL_checknumber(pState, -1);

    *m_pVol = m_pVol->cutSphere( (Real64)(iX), (Real64)(iY), (Real64)(iZ), fRadius );
    return 0;
};

/**
 * Threshold the volumetric map. All voxel below and above certain values are cut off and set to those values.
 * Parameters: New minimum value, new maximum value.
 *
 * usage:
 * vol:threshold( 3.5 )
 */
int volume::threshold( lua_State* pState )
{
    if (!checkData()) return 0;

    double fNewMin = luaL_checknumber(pState, -2);
    double fNewMax = luaL_checknumber(pState, -1);

    m_pVol->threshold( fNewMin, fNewMax );
    return 0;
};

/**
 * Normalize the volumetric map to [0..1]. 
 *
 * usage:
 * vol:normalize( )
 */
int volume::normalize( lua_State* )
{
    if (!checkData()) return 0;

    m_pVol->normalize( );
    return 0;
};

/**
 * Get the maximal voxel value in the map.
 *
 * usage:
 * max = vol:getMaxDensity( )
 */
int volume::getMaxDensity( lua_State* pState )
{
    if (!checkData()) return 0;

    lua_pushnumber(pState, m_pVol->getMaxDensity() );
    return 1;
};
/**
 * Get the minimal voxel value in the map.
 *
 * usage:
 * max = vol:getMinDensity( )
 */
int volume::getMinDensity( lua_State* pState )
{
    if (!checkData()) return 0;

    lua_pushnumber(pState, m_pVol->getMinDensity() );
    return 1;
};
/**
 * Get the voxelwidth of the volume.
 *
 * usage:
 * vw = vol:getVoxelwidth( )
 */
int volume::getVoxelwidth( lua_State* pState )
{
    if (!checkData()) return 0;

    lua_pushnumber(pState, m_pVol->getWidth() );
    return 1;
};
/**
 * Set the voxelwidth of the volume. This will not re-interpolate the map, but only set the internal voxelwidth variable! See interpolate function.
 *
 * usage:
 * vol:setVoxelwidth( 2.0 )
 */
int volume::setVoxelwidth( lua_State* pState )
{
    if (!checkData()) return 0;

    m_pVol->setWidth( luaL_checknumber(pState, -1) );
    return 0;
};

/**
 * Get the size of the volume in x dimension.
 *
 * usage:
 * sizex = vol:getSizeX( )
 */
int volume::getSizeX( lua_State* pState )
{
    if (!checkData()) return 0;

    lua_pushnumber(pState, m_pVol->getSizeX() );
    return 1;
};
/**
 * Get the size of the volume in y dimension.
 *
 * usage:
 * sizey = vol:getSizeY( )
 */
int volume::getSizeY( lua_State* pState )
{
    if (!checkData()) return 0;

    lua_pushnumber(pState, m_pVol->getSizeY() );
    return 1;
};
/**
 * Get the size of the volume in z dimension.
 *
 * usage:
 * sizez = vol:getSizeZ( )
 */
int volume::getSizeZ( lua_State* pState )
{
    if (!checkData()) return 0;

    lua_pushnumber(pState, m_pVol->getSizeZ() );
    return 1;
};

/**
 * Mask with another volume object - all the voxels in this vol are multiplied with the mask volume voxels (multiplied by 0 for not overlapping voxels).
 *
 * usage:
 * vol:mask( maskvol )
 */
int volume::mask( lua_State* pState )
{
    if (!checkData()) return 0;

    volume *pVol = svt_luna<volume>::luaCheck( pState, -1 );
    svt_volume<Real64>* pMap = pVol->get();

    if (pMap == NULL)
    {
        svtout << "Error: The parameter object does not contain any data!" << endl;
        return 0;
    }

    m_pVol->mask( *pMap );
    return 0;
};

/**
 * Create a binary mask using a threshold value. Every voxel below the threshold will get set to 0, the rest to 1.
 *
 * usage:
 * vol:makeMask( 1.0 )
 */
int volume::makeMask( lua_State* pState )
{
    if (!checkData()) return 0;

    double fThreshold = luaL_checknumber(pState, -1);
    m_pVol->makeMask( fThreshold );
    return 0;
};

/**
 * Re-interpolate the map to a different voxelsize.
 *
 * usage:
 * vol:interpolate( 2.0 )
 */
int volume::interpolate( lua_State* pState )
{
    if (!checkData()) return 0;

    double fWidth = luaL_checknumber(pState, -1);
    m_pVol->interpolate_map( fWidth );

    if (m_pDoc)
        m_pDoc->setChanged();

    return 0;
};

/**
 * Convolve map with another volume (kernel).
 *
 * usage:
 * vol:convolve( kernelvol )
 */
int volume::convolve( lua_State* pState )
{
    if (!checkData()) return 0;

    volume *pVol = svt_luna<volume>::luaCheck( pState, -1 );
    svt_volume<Real64>* pMap = pVol->get();

    if (pMap == NULL)
    {
        svtout << "Error: The parameter object does not contain any data!" << endl;
        return 0;
    }

    m_pVol->convolve( *pMap, true );
    return 0;
};

/**
 * Create a laplacian kernel volume (3x3x3).
 *
 * usage:
 * kernel = volume:new()
 * kernel:createLaplacian()
 */
int volume::createLaplacian( lua_State* )
{
    if (m_pVol == NULL)
        m_pVol = new svt_volume<Real64>;

    m_bDelete = true;
    m_pVol->createLaplacian( );
    return 0;
};

/**
 * Create a Gaussian kernel volume within SigmaFactor*fSigma.
 * Attention: This will overwrite the current content of the volume object with the filter kernel. It will allocate the memory internally.
 * Parameters: sigma of map and sigma factor
 *
 * usage:
 * kernel:createGaussian( sigma, sigmafactor )
 */
int volume::createGaussian( lua_State* pState )
{
    if (m_pVol == NULL)
        m_pVol = new svt_volume<Real64>;

    double fSigma  = luaL_checknumber(pState, -2);
    double fFactor = luaL_checknumber(pState, -1);

    m_bDelete = true;
    m_pVol->createGaussian( fSigma, fFactor );
    return 0;
};

/**
 * Set/get the x coordinate of the position of the map
 * If no parameter is given, the function just returns a number, otherwise it will set the x coordinate using the parameter provided by the user.
 *
 * usage:
 * vol:x( 15.0 )
 * print( vol:x() )
 */
int volume::x(lua_State* pState)
{
    if (lua_isnumber( pState, -1 ))
    {
        double fX = luaL_checknumber(pState, -1);
        m_pVol->setGridX( fX );
    }

    lua_pushnumber(pState, m_pVol->getGridX() );
    return 1;
};
/**
 * Set/get the y coordinate of the position of the map
 * If no parameter is given, the function just returns a number, otherwise it will set the y coordinate using the parameter provided by the user.
 *
 * usage:
 * vol:y( 15.0 )
 * print( vol:y() )
 */
int volume::y(lua_State* pState)
{
    if (lua_isnumber( pState, -1 ))
    {
        double fY = luaL_checknumber(pState, -1);
        m_pVol->setGridY( fY );
    }

    lua_pushnumber(pState, m_pVol->getGridY() );
    return 1;
};
/**
 * Set/get the z coordinate of the position of the map
 * If no parameter is given, the function just returns a number, otherwise it will set the z coordinate using the parameter provided by the user.
 *
 * usage:
 * vol:z( 15.0 )
 * print( vol:z() )
 */
int volume::z(lua_State* pState)
{
    if (lua_isnumber( pState, -1 ))
    {
        double fZ = luaL_checknumber(pState, -1);
        m_pVol->setGridZ( fZ );
    }

    lua_pushnumber(pState, m_pVol->getGridZ() );
    return 1;
};

const char volume::className[] = "volume";
const svt_luna<volume>::RegType volume::methods[] =
{
    { "size",                 &volume::size                },
    { "copy",                 &volume::copy                },
    { "crop",                 &volume::crop                },
    { "allocate",             &volume::allocate            },
    { "addDoc",               &volume::addDoc              },
    { "setIsoThreshold",      &volume::setIsoThreshold     },
    { "setWireframe",         &volume::setWireframe        },
    { "getValue",             &volume::getValue            },
    { "getIntValue",          &volume::getIntValue         },
    { "setValue",             &volume::setValue            },
    { "updateRendering",      &volume::updateRendering     },
    { "setColor",             &volume::setColor            },
    { "save",                 &volume::save                },
    { "load",                 &volume::load                },
    { "vectorquant",          &volume::vectorquant         },
    { "correlation",          &volume::correlation         },
    { "cutSphere",            &volume::cutSphere           },
    { "threshold",            &volume::threshold           },
    { "getMaxDensity",        &volume::getMaxDensity       },
    { "getMinDensity",        &volume::getMinDensity       },
    { "getVoxelwidth",        &volume::getVoxelwidth       },
    { "setVoxelwidth",        &volume::setVoxelwidth       },
    { "getSizeX",             &volume::getSizeX            },
    { "getSizeY",             &volume::getSizeY            },
    { "getSizeZ",             &volume::getSizeZ            },
    { "normalize",            &volume::normalize           },
    { "mask",                 &volume::mask                },
    { "makeMask",             &volume::makeMask            },
    { "interpolate",          &volume::interpolate         },
    { "convolve",             &volume::convolve            },
    { "createGaussian",       &volume::createGaussian      },
    { "createLaplacian",      &volume::createLaplacian     },
    { "x",                    &volume::x                   },
    { "y",                    &volume::y                   },
    { "z",                    &volume::z                   },
    { 0,                      0                            }
};

///////////////////////////////////////////////////////////////////////////////
// LUA BINDINGS: MARKER
///////////////////////////////////////////////////////////////////////////////

/**
 * The marker class encapsulates a single 3D marker
 */
marker::marker(lua_State *pState)
{
    m_pDoc = NULL;

    if (pState == NULL)
        return;

    if ( !lua_isnil( pState, -1 ) && !lua_isnil( pState, -2 ) && !lua_isnil( pState, -3 ) )
    {
        double fX = 1.0E-2 * luaL_checknumber(pState, -3);
        double fY = 1.0E-2 * luaL_checknumber(pState, -2);
        double fZ = 1.0E-2 * luaL_checknumber(pState, -1);

        m_pDoc = new sculptor_doc_marker(g_pWindow->getScene()->getDataTopNode());
        m_pDoc->setMarkerNodePos( fX, fY, fZ );
        m_pDoc->storeInitMat();
        // as we cannot save the markers yet, there is also no point in setting this flag...
        //m_pDoc->setChanged();

        // ok, now add the thing to sculptor
        g_pWindow->addDocSimple((sculptor_document*)m_pDoc, "Marker" );
        g_pWindow->adjustOrigin();
    }
};
marker::~marker()
{
};

/**
 * Set the sculptor document that is encapsulated by this object
 */
void marker::setDoc( sculptor_doc_marker* pDoc)
{
    m_pDoc = pDoc;
};

/**
 * Get the sculptor document that is encapsulated by this object
 */
sculptor_doc_marker* marker::getDoc()
{
    return m_pDoc;
};

/**
 * Set/get the x coordinate of the position of the marker
 * If no parameter is given, the function just returns a number, otherwise it will set the x coordinate using the parameter provided by the user.
 *
 * usage:
 * mar:x( 15.0 )
 * print( mar:x() )
 */
int marker::x(lua_State* pState)
{
    if (lua_isnumber( pState, -1 ))
    {
        double fX = luaL_checknumber(pState, -1);
        m_pDoc->setMarkerNodePosX( fX*1.0e-2 );
        m_pDoc->updatePropDlg();
    }

    lua_pushnumber(pState, m_pDoc->getMarkerNodePosX()*1.0e2 );
    return 1;
};

/**
 * Set/get the y coordinate of the position of the marker
 * If no parameter is given, the function just returns a number, otherwise it will set the y coordinate using the parameter provided by the user.
 *
 * usage:
 * mar:y( 15.0 )
 * print( mar:y() )
 */
int marker::y(lua_State* pState)
{
    if (lua_isnumber( pState, -1 ))
    {
        double fY = luaL_checknumber(pState, -1);
        m_pDoc->setMarkerNodePosY( fY*1.0e-2 );
        m_pDoc->updatePropDlg();
    }

    lua_pushnumber(pState, m_pDoc->getMarkerNodePosY()*1.0e2 );
    return 1;
};
/**
 * Set/get the z coordinate of the position of the marker
 * If no parameter is given, the function just returns a number, otherwise it will set the z coordinate using the parameter provided by the user.
 *
 * usage:
 * mar:z( 15.0 )
 * print( mar:z() )
 */
int marker::z(lua_State* pState)
{
    if (lua_isnumber( pState, -1 ))
    {
        double fZ = luaL_checknumber(pState, -1);
        m_pDoc->setMarkerNodePosZ( fZ*1.0e-2 );
        m_pDoc->updatePropDlg();
    }

    lua_pushnumber(pState, m_pDoc->getMarkerNodePosZ()*1.0e2 );
    return 1;
};

/**
 * Get the position of the marker.
 * Returns: Numbers (x,y,z)
 *
 * usage:
 * x,y,z = mar:getPos( )
 */
int marker::getPos(lua_State* pState)
{
    lua_pushnumber(pState, m_pDoc->getMarkerNodePosX()*1.0e2 );
    lua_pushnumber(pState, m_pDoc->getMarkerNodePosY()*1.0e2 );
    lua_pushnumber(pState, m_pDoc->getMarkerNodePosZ()*1.0e2 );
    return 3;
};
/**
 * Set the position of the marker.
 * Parameters: Numbers (x,y,z)
 *
 * usage:
 * mar:setPos( x,y,z )
 */
int marker::setPos(lua_State* pState)
{
    m_pDoc->setMarkerNodePosX( luaL_checknumber(pState, -3)*1.0e-2 );
    m_pDoc->setMarkerNodePosY( luaL_checknumber(pState, -2)*1.0e-2 );
    m_pDoc->setMarkerNodePosZ( luaL_checknumber(pState, -1)*1.0e-2 );
    return 0;
};

/**
 * Set the scale of the marker node
 *
 * usage:
 * mar:setScale( 2.0 )
 */
int marker::setScale( lua_State* pState )
{
    m_pDoc->getNode()->setScale( luaL_checknumber(pState, -1) );
    m_pDoc->getNode()->rebuildDL();
    m_pDoc->updatePropDlg();
    return 0;
};

/**
 * Set the color of the marker node
 *
 * usage:
 * mar:setColor( R,G,B )
 */
int marker::setColor( lua_State* pState )
{
    double fR = luaL_checknumber(pState, -3);
    double fG = luaL_checknumber(pState, -2);
    double fB = luaL_checknumber(pState, -1);

    svt_color oDocCol = m_pDoc->getColor();

    oDocCol.setR( (float)fR );
    oDocCol.setG( (float)fG );
    oDocCol.setB( (float)fB );

    m_pDoc->setColor(oDocCol);
    m_pDoc->updatePropDlg();

    return 0;
};

/**
 * Measure distance to another marker.
 * Parameter: Marker object.
 * Returns: Number.
 *
 * usage:
 * dist = mar1:distance( mar2 )
 */
int marker::distance(lua_State * pState)
{
    Real64 fDist = 0;

    if (svt_luna<marker>::luaIsType( pState, -1 ))
    {
    	marker *pMarker = svt_luna<marker>::luaCheck( pState, -1 );
	fDist = m_pDoc->getMarkerNodePos().distance( pMarker->m_pDoc->getMarkerNodePos() );
	fDist*=1.0E2;
    }
    else 
    {
	atom *pAtom = svt_luna<atom>::luaCheck( pState, -1 );
	fDist = (m_pDoc->getMarkerNodePos()*1.0E2).distance( pAtom->getPoint() );
    }
	
    lua_pushnumber(pState, fDist );
    return 1;
}


const char marker::className[] = "marker";
const svt_luna<marker>::RegType marker::methods[] =
{
    { "x",                    &marker::x                   },
    { "y",                    &marker::y                   },
    { "z",                    &marker::z                   },
    { "setPos",               &marker::setPos              },
    { "getPos",               &marker::getPos              },
    { "setScale",             &marker::setScale            },
    { "setColor",             &marker::setColor            },
    { "distance",	      &marker::distance		   },
    { 0,                      0                            }
};

///////////////////////////////////////////////////////////////////////////////
// LUA SYNTAX HIGHLIGHTER FOR QTEXTEDIT
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 */
sculptor_lua_syntax::sculptor_lua_syntax(QTextEdit *oTextEdit)
    : QSyntaxHighlighter( oTextEdit->document() )
{
    SVT_LUA_HIGHLIGHTINGRULE oRule;

    // lua keywords
    QTextCharFormat oKeywordFormat;
    QStringList oKeywordPatterns;
    oKeywordFormat.setForeground(Qt::blue);
    oKeywordFormat.setFontWeight(QFont::Bold);
    oKeywordPatterns << "\\band\\b"
                     << "\\bbreak\\b"
                     << "\\bdo\\b"
                     << "\\belse\\b"
                     << "\\belseif\\b"
                     << "\\bend\\b"
                     << "\\bfor\\b"
                     << "\\bfalse\\b"
                     << "\\bfunction\\b"
                     << "\\bif\\b"
                     << "\\bin\\b"
                     << "\\blocal\\b"
                     << "\\bnil\\b"
                     << "\\bnot\\b"
                     << "\\bor\\b"
                     << "\\brepeat\\b"
                     << "\\breturn\\b"
                     << "\\bthen\\b"
                     << "\\btrue\\b"
                     << "\\buntil\\b"
                     << "\\bwhile\\b";

    foreach (const QString &pattern, oKeywordPatterns)
    {
        oRule.pattern = QRegExp(pattern);
        oRule.format = oKeywordFormat;
        m_aHighlightingRules.append(oRule);
    }

    // lua classes
    QTextCharFormat oClassFormat;
    oClassFormat.setForeground(Qt::blue);
    oRule.pattern = QRegExp("\\b(coroutine|debug|file|io|math|os|package|string|table)\\b");
    oRule.format = oClassFormat;
    m_aHighlightingRules.append(oRule);

    // sculptor classes
    QTextCharFormat oSculptorClassFormat;
    oSculptorClassFormat.setForeground(Qt::blue);
    oRule.pattern = QRegExp("\\b(arrow|atom|label|marker|matrix4|molecule|sculptor|sphere|volume)\\b");
    oRule.format = oSculptorClassFormat;
    m_aHighlightingRules.append(oRule);

    // quotations
    QTextCharFormat oQuotationFormat;
    oQuotationFormat.setForeground(Qt::darkRed);
    oRule.pattern = QRegExp("\".*\"");
    oRule.format = oQuotationFormat;
    m_aHighlightingRules.append(oRule);

    // numbers
    // says A: "i have a problem. i will solve it with a regular expression!"
    // says B: "then you have two problems."
    QTextCharFormat oNumberFormat;
    oNumberFormat.setForeground(Qt::magenta); // if that's too pink, how about darkMagenta
    oRule.format = oNumberFormat;
    //
    oRule.pattern = QRegExp("[0-9]+");
    m_aHighlightingRules.append(oRule);
    //
    oRule.pattern = QRegExp("\\.[0-9]+f?");
    m_aHighlightingRules.append(oRule);
    //
    oRule.pattern = QRegExp("[0-9]*\\.[0-9]+f?");
    m_aHighlightingRules.append(oRule);
    //
    oRule.pattern = QRegExp("[0-9]+\\.[0-9]*f?");
    m_aHighlightingRules.append(oRule);

    // lua comments
    m_oCommentFormat.setForeground(Qt::darkGreen);
    oRule.pattern = QRegExp("--[^\n]*");
    oRule.format = m_oCommentFormat;
    m_aHighlightingRules.append(oRule);
    m_oCommentStartExpression = QRegExp("--\\[\\[");
    m_oCommentEndExpression   = QRegExp("--\\]\\]");
}

/**
 * Destructor
 */
sculptor_lua_syntax::~sculptor_lua_syntax()
{
}

/**
 *
 */
void sculptor_lua_syntax::highlightBlock(const QString &oText)
{
    foreach (const SVT_LUA_HIGHLIGHTINGRULE& oRule, m_aHighlightingRules)
    {
        QRegExp oExpression(oRule.pattern);
        int iIndex = oExpression.indexIn(oText);
        while (iIndex >= 0)
        {
            int iLength = oExpression.matchedLength();
            setFormat(iIndex, iLength, oRule.format);
            iIndex = oExpression.indexIn(oText, iIndex + iLength);
        }
    }
    setCurrentBlockState(0);

    int iStartIndex = 0;

    if (previousBlockState() != 1)
        iStartIndex = m_oCommentStartExpression.indexIn(oText);

    while (iStartIndex >= 0)
    {
        int iEndIndex = m_oCommentEndExpression.indexIn(oText, iStartIndex);
        int iCommentLength;
        if (iEndIndex == -1)
        {
            setCurrentBlockState(1);
            iCommentLength = oText.length() - iStartIndex;
        }
        else
        {
            iCommentLength = iEndIndex - iStartIndex
                + m_oCommentEndExpression.matchedLength();
        }

        setFormat(iStartIndex, iCommentLength, m_oCommentFormat);
        iStartIndex = m_oCommentStartExpression.indexIn(oText, iStartIndex + iCommentLength);
    }
}
