/***************************************************************************
                          svt_simple_text
			  ---------------
    begin                : 08.02.2008
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

//
// The font data itself is based on the "uglyfont" code by Soji Yamakawa.
//
#include <svt_basics.h>
#include <svt_types.h>
#include <svt_cmath.h>
#include <svt_matrix4.h>
//#include <GL/gl.h>

// The following integer arrays define the ugly font geometry
// Coordinate ranges in the arrays are 0<=x<=100 and 0<=y<=100.
const double g_fFontWidth=100.0;
const double g_fFontHeight=100.0;

///////////////////////////////////////////////////////////////////////////////
// Font Data
///////////////////////////////////////////////////////////////////////////////

/*   */
static int g_sChr032[] =
{
    32,
    -1
};
/* ! */
static int g_sChr033[] =
{
    33,
    0,3,50,100,75,100,50,25,
    0,4,50,16,62,16,62,0,50,0,
    -1
};
/* " */
static int g_sChr034[] =
{
    34,
    2,4,37,100,37,83,62,100,62,83,
    -1
};
/* # */
static int g_sChr035[] =
{   
    35,
    2,8,12,66,87,66,12,33,87,33,37,91,37,8,62,8,62,91,
    -1
};
/* $ */
static int g_sChr036[] =
{   
    36,
    1,12,87,75,75,83,25,83,12,75,12,58,25,50,75,50,87,41,87,25,75,16,25,16,12,25,
    2,4,37,91,37,8,62,8,62,91,
    -1
};
/* % */
static int g_sChr037[] =
{   
    37,
    1,2,87,100,12,0,
    1,5,12,100,37,100,37,75,12,75,12,100,
    1,5,87,0,87,25,62,25,62,0,87,0,
    -1
};
static int g_sChr038[] =
{   /* & */
    38,
    1,11,87,33,62,0,25,0,0,16,0,41,75,83,75,91,62,100,37,100,25,83,87,0,
    -1
};
static int g_sChr039[] =
{   /* ' */
    39,
    0,3,50,83,50,100,62,100,
    -1
};
static int g_sChr040[] =
{   /* ( */
    40,
    1,6,62,100,37,83,25,58,25,41,37,16,62,0,
    -1
};
static int g_sChr041[] =
{   /* ) */
    41,
    1,6,37,100,62,83,75,58,75,41,62,16,37,0,
    -1
};
static int g_sChr042[] =
{   /* * */
    42,
    2,8,50,100,50,0,0,50,100,50,87,91,12,8,87,8,12,91,
    -1
};
static int g_sChr043[] =
{   /* + */
    43,
    2,4,12,50,87,50,50,75,50,25,
    -1
};
static int g_sChr044[] =
{   /* , */
    44,
    1,4,37,25,62,25,62,8,37,-8,
    -1
};
static int g_sChr045[] =
{   /* - */
    45,
    1,2,12,50,87,50,
    -1
};
static int g_sChr046[] =
{   /* . */
    46,
    0,4,37,16,62,16,62,0,37,0,
    -1
};
static int g_sChr047[] =
{   /* / */
    47,
    1,2,100,100,0,0,
    -1
};
static int g_sChr048[] =
{   /* 0 */
    48,
    1,9,25,100,75,100,100,83,100,16,75,0,25,0,0,16,0,83,25,100,
    1,2,87,91,12,8,
    -1
};
static int g_sChr049[] =
{   /* 1 */
    49,
    1,3,25,83,50,100,50,0,
    -1
};
static int g_sChr050[] =
{   /* 2 */
    50,
    1,8,12,83,37,100,75,100,100,83,100,66,12,16,12,0,100,0,
    -1
};
static int g_sChr051[] =
{   /* 3 */
    51,
    1,11,12,83,37,100,75,100,100,83,100,66,75,50,100,33,100,16,75,0,25,0,0,16,
    -1
};
static int g_sChr052[] =
{   /* 4 */
    52,
    1,3,37,100,12,25,87,25,
    1,2,62,75,62,0,
    -1
};
static int g_sChr053[] =
{   /* 5 */
    53,
    1,10,87,100,12,100,12,41,37,58,62,58,87,41,87,16,62,0,37,0,12,16,
    -1
};
static int g_sChr054[] =
{   /* 6 */
    54,
    1,12,87,83,62,100,25,100,0,83,0,16,25,0,75,0,100,16,100,33,75,50,25,50,0,33,
    -1
};
static int g_sChr055[] =
{   /* 7 */
    55,
    1,5,12,83,12,100,87,100,50,33,50,0,
    -1
};
static int g_sChr056[] =
{   /* 8 */
    56,
    1,9,100,83,75,100,25,100,0,83,0,66,25,50,75,50,100,66,100,83,
    1,8,25,50,0,33,0,16,25,0,75,0,100,16,100,33,75,50,
    -1
};
static int g_sChr057[] =
{   /* 9 */
    57,
    1,12,0,16,25,0,75,0,100,16,100,83,75,100,25,100,0,83,0,58,25,41,75,41,100,58,
    -1
};
static int g_sChr058[] =
{   /* : */
    58,
    0,4,37,91,62,91,62,75,37,75,
    0,4,37,25,62,25,62,8,37,8,
    -1
};
static int g_sChr059[] =
{   /* ; */
    59,
    0,4,37,91,62,91,62,75,37,75,
    0,4,37,25,62,25,62,8,37,8,
    1,2,62,8,37,-8,
    -1
};
static int g_sChr060[] =
{   /* < */
    60,
    1,3,87,100,12,50,87,0,
    -1
};
static int g_sChr061[] =
{   /* = */
    61,
    2,4,12,66,87,66,12,33,87,33,
    -1
};
static int g_sChr062[] =
{   /* > */
    62,
    1,3,12,0,87,50,12,100,
    -1
};
static int g_sChr063[] =
{   /* ? */
    63,
    1,8,12,83,37,100,75,100,100,83,100,66,75,50,50,50,50,25,
    0,4,50,16,62,16,62,8,50,8,
    -1
};
static int g_sChr064[] =
{   /* @ */
    64,
    1,18,62,50,50,58,25,58,12,41,12,25,25,16,50,16,62,41,75,25,87,66,75,91,62,100,25,100,0,75,0,16,25,0,62,0,87,16,
    -1
};
static int g_sChr065[] =
{   /* A */
    65,
    1,3,0,0,50,100,100,0,
    1,2,25,50,75,50,
    -1
};
static int g_sChr066[] =
{   /* B */
    66,
    1,10,0,0,0,100,75,100,87,91,87,58,75,50,100,33,100,8,87,0,0,0,
    1,2,75,50,0,50,
    -1
};
static int g_sChr067[] =
{   /* C */
    67,
    1,8,100,83,75,100,25,100,0,83,0,16,25,0,75,0,100,16,
    -1
};
static int g_sChr068[] =
{   /* D */
    68,
    1,7,0,100,75,100,100,83,100,16,75,0,0,0,0,100,
    -1
};
static int g_sChr069[] =
{   /* E */
    69,
    1,4,100,100,0,100,0,0,100,0,
    1,2,0,50,87,50,
    -1
};
static int g_sChr070[] =
{   /* F */
    70,
    1,3,100,100,0,100,0,0,
    1,2,0,50,75,50,
    -1
};
static int g_sChr071[] =
{   /* G */
    71,
    1,10,100,83,75,100,25,100,0,83,0,16,25,0,75,0,100,16,100,41,62,41,
    -1
};
static int g_sChr072[] =
{   /* H */
    72,
    2,6,0,100,0,0,100,100,100,0,0,50,100,50,
    -1
};
static int g_sChr073[] =
{   /* I */
    73,
    2,6,37,100,62,100,37,0,62,0,50,0,50,100,
    -1
};
static int g_sChr074[] =
{   /* J */
    74,
    1,2,75,100,100,100,
    1,6,87,100,87,16,62,0,37,0,12,16,12,33,
    -1
};
static int g_sChr075[] =
{   /* K */
    75,
    1,2,12,100,12,0,
    1,2,12,33,100,100,
    1,2,25,41,100,0,
    -1
};
static int g_sChr076[] =
{   /* L */
    76,
    1,3,0,100,0,0,100,0,
    -1
};
static int g_sChr077[] =
{   /* M */
    77,
    1,5,0,0,0,100,50,50,100,100,100,0,
    -1
};
static int g_sChr078[] =
{   /* N */
    78,
    1,4,0,0,0,100,100,0,100,100,
    -1
};
static int g_sChr079[] =
{   /* O */
    79,
    1,9,0,83,25,100,75,100,100,83,100,16,75,0,25,0,0,16,0,83,
    -1
};
static int g_sChr080[] =
{   /* P */
    80,
    1,7,0,0,0,100,75,100,100,83,100,66,75,50,0,50,
    -1
};
static int g_sChr081[] =
{   /* Q */
    81,
    1,9,25,0,0,16,0,83,25,100,75,100,100,83,100,16,75,0,25,0,
    1,2,62,25,100,0,
    -1
};
static int g_sChr082[] =
{   /* R */
    82,
    1,7,0,0,0,100,75,100,100,83,100,66,75,50,0,50,
    1,3,75,50,100,33,100,0,
    -1
};
static int g_sChr083[] =
{   /* S */
    83,
    1,12,100,83,75,100,25,100,0,83,0,66,25,50,75,50,100,33,100,16,75,0,25,0,0,16,
    -1
};
static int g_sChr084[] =
{   /* T */
    84,
    2,4,0,100,100,100,50,100,50,0,
    -1
};
static int g_sChr085[] =
{   /* U */
    85,
    1,6,0,100,0,16,25,0,75,0,100,16,100,100,
    -1
};
static int g_sChr086[] =
{   /* V */
    86,
    1,3,0,100,50,0,100,100,
    -1
};
static int g_sChr087[] =
{   /* W */
    87,
    1,5,0,100,25,0,50,66,75,0,100,100,
    -1
};
static int g_sChr088[] =
{   /* X */
    88,
    2,4,0,0,100,100,100,0,0,100,
    -1
};
static int g_sChr089[] =
{   /* Y */
    89,
    1,3,0,100,50,50,50,0,
    1,2,50,50,100,100,
    -1
};
static int g_sChr090[] =
{   /* Z */
    90,
    1,4,0,100,100,100,0,0,100,0,
    -1
};
static int g_sChr091[] =
{   /* [ */
    91,
    1,4,62,100,37,100,37,0,62,0,
    -1
};
static int g_sChr092[] =
{   /* \ */
    92,
    1,2,0,100,100,0,
    -1
};
static int g_sChr093[] =
{   /* ] */
    93,
    1,4,37,100,62,100,62,0,37,0,
    -1
};
static int g_sChr094[] =
{   /* ^ */
    94,
    1,3,0,66,50,91,100,66,
    -1
};
static int g_sChr095[] =
{   /* _ */
    95,
    1,2,0,8,100,8,
    -1
};
static int g_sChr096[] =
{   /* ` */
    96,
    0,3,37,100,50,100,50,83,
    -1
};
static int g_sChr097[] =
{   /* a */
    97,
    1,5,12,50,25,58,75,58,87,50,87,0,
    1,7,87,33,25,33,12,25,12,8,25,0,75,0,87,8,
    -1
};
static int g_sChr098[] =
{   /* b */
    98,
    1,7,12,100,12,0,75,0,87,8,87,50,75,58,12,58,
    -1
};
static int g_sChr099[] =
{   /* c */
    99,
    1,8,87,50,75,58,25,58,12,50,12,8,25,0,75,0,87,8,
    -1
};
static int g_sChr100[] =
{   /* d */
    100,
    1,8,87,100,87,0,25,0,12,8,12,50,25,58,75,58,87,50,
    -1
};
static int g_sChr101[] =
{   /* e */
    101,
    1,10,12,33,87,33,87,50,75,58,25,58,12,50,12,8,25,0,75,0,87,8,
    -1
};
static int g_sChr102[] =
{   /* f */
    102,
    1,4,75,100,62,100,50,91,50,0,
    1,2,25,58,75,58,
    -1
};
static int g_sChr103[] =
{   /* g */
    103,
    1,5,87,58,87,0,75,-8,25,-8,12,0,
    1,8,87,50,75,58,25,58,12,50,12,33,25,25,75,25,87,33,
    -1
};
static int g_sChr104[] =
{   /* h */
    104,
    1,2,12,0,12,100,
    1,5,12,50,25,58,75,58,87,50,87,0,
    -1
};
static int g_sChr105[] =
{   /* i */
    105,
    1,2,50,75,50,66,
    1,2,50,58,50,0,
    -1
};
static int g_sChr106[] =
{   /* j */
    106,
    1,2,50,75,50,66,
    1,4,50,58,50,0,37,-8,12,-8,
    -1
};
static int g_sChr107[] =
{   /* k */
    107,
    1,2,12,100,12,0,
    1,3,87,0,12,33,75,58,
    -1
};
static int g_sChr108[] =
{   /* l */
    108,
    1,3,37,100,50,100,50,0,
    -1
};
static int g_sChr109[] =
{   /* m */
    109,
    1,5,12,0,12,58,75,58,87,50,87,0,
    1,3,37,58,50,50,50,0,
    -1
};
static int g_sChr110[] =
{   /* n */
    110,
    1,5,12,0,12,58,75,58,87,50,87,0,
    -1
};
static int g_sChr111[] =
{   /* o */
    111,
    1,9,25,0,12,8,12,50,25,58,75,58,87,50,87,8,75,0,25,0,
    -1
};
static int g_sChr112[] =
{   /* p */
    112,
    1,2,12,58,12,-16,
    1,7,12,50,25,58,75,58,87,50,87,8,75,0,12,0,
    -1
};
static int g_sChr113[] =
{   /* q */
    113,
    1,2,87,58,87,-16,
    1,7,87,50,75,58,25,58,12,50,12,8,25,0,87,0,
    -1
};
static int g_sChr114[] =
{   /* r */
    114,
    1,2,25,58,25,0,
    1,3,25,50,62,58,87,58,
    -1
};
static int g_sChr115[] =
{   /* s */
    115,
    1,10,87,50,75,58,25,58,12,50,12,41,87,16,87,8,75,0,25,0,12,8,
    -1
};
static int g_sChr116[] =
{   /* t */
    116,
    1,2,25,58,75,58,
    1,4,37,75,37,8,50,0,75,0,
    -1
};
static int g_sChr117[] =
{   /* u */
    117,
    1,5,12,58,12,8,25,0,62,0,87,8,
    1,2,87,58,87,0,
    -1
};
static int g_sChr118[] =
{
    /* v */
    118,
    1,3,12,58,50,0,87,58,
    -1
};
static int g_sChr119[] =
{   /* w */
    119,
    1,5,12,58,25,0,50,41,75,0,87,58,
    -1
};
static int g_sChr120[] =
{   /* x */
    120,
    2,4,87,0,12,58,87,58,12,0,
    -1
};
static int g_sChr121[] =
{   /* y */
    121,
    2,4,87,58,12,-25,12,58,50,16,
    -1
};
static int g_sChr122[] =
{   /* z */
    122,
    1,4,12,58,87,58,12,0,87,0,
    -1
};
static int g_sChr123[] =
{   /* { */
    123,
    1,6,75,100,50,100,37,91,37,8,50,0,75,0,
    1,2,37,50,25,50,
    -1
};
static int g_sChr124[] =
{   /* | */
    124,
    1,2,50,100,50,0,
    -1
};
static int g_sChr125[] =
{   /* } */
    125,
    1,6,25,0,50,0,62,8,62,91,50,100,25,100,
    1,2,62,50,75,50,
    -1
};
static int g_sChr126[] =
{   /* ~ */
    126,
    1,2,0,91,100,91,
    -1
};

static int *g_sFontSet[]=
{
NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,
NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,
g_sChr032,g_sChr033,g_sChr034,g_sChr035,g_sChr036,g_sChr037,g_sChr038,g_sChr039,g_sChr040,g_sChr041,g_sChr042,g_sChr043,g_sChr044,g_sChr045,g_sChr046,g_sChr047,
g_sChr048,g_sChr049,g_sChr050,g_sChr051,g_sChr052,g_sChr053,g_sChr054,g_sChr055,g_sChr056,g_sChr057,g_sChr058,g_sChr059,g_sChr060,g_sChr061,g_sChr062,g_sChr063,
g_sChr064,g_sChr065,g_sChr066,g_sChr067,g_sChr068,g_sChr069,g_sChr070,g_sChr071,g_sChr072,g_sChr073,g_sChr074,g_sChr075,g_sChr076,g_sChr077,g_sChr078,g_sChr079,
g_sChr080,g_sChr081,g_sChr082,g_sChr083,g_sChr084,g_sChr085,g_sChr086,g_sChr087,g_sChr088,g_sChr089,g_sChr090,g_sChr091,g_sChr092,g_sChr093,g_sChr094,g_sChr095,
g_sChr096,g_sChr097,g_sChr098,g_sChr099,g_sChr100,g_sChr101,g_sChr102,g_sChr103,g_sChr104,g_sChr105,g_sChr106,g_sChr107,g_sChr108,g_sChr109,g_sChr110,g_sChr111,
g_sChr112,g_sChr113,g_sChr114,g_sChr115,g_sChr116,g_sChr117,g_sChr118,g_sChr119,g_sChr120,g_sChr121,g_sChr122,g_sChr123,g_sChr124,g_sChr125,g_sChr126,NULL  ,
NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,
NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,
NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,
NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,
NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,
NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,
NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,
NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL     ,NULL
};

///////////////////////////////////////////////////////////////////////////////
// Output Routines
///////////////////////////////////////////////////////////////////////////////

DLLEXPORT void svt_drawText(Real32 fX, Real32 fY, Real32 fZ, Real32 fR, Real32 fG, Real32 fB, const char pStr[])
{
    svt_vector4<Real32> oVec( fX, fY, fZ );

    svt_matrix4<Real32> oTrans;
    oTrans.loadModelviewMatrix();

    svt_matrix4<Real32> oProj;
    oProj.loadProjectionMatrix();

    oVec = oTrans * oVec;
    oVec = oProj * oVec;

    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    oTrans.loadIdentity();
    oTrans.setTranslationX(oVec.x() / oVec.w());
    oTrans.setTranslationY(oVec.y() / oVec.w());
    oTrans.setTranslationZ(oVec.z() / oVec.w());

    oTrans.applyToGL();

    Real32 fScale = 3.0;

    glDisable( GL_LIGHTING );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glDepthFunc(GL_ALWAYS);

    glLineWidth( 2.0 );
    glColor3f( 0.0f, 0.0f, 0.0f );

    for( int i=0; pStr[i]!=0; i++ )
    {
        int *pChr = g_sFontSet[((unsigned char *)pStr)[i]];

        if( pChr!=NULL )
        {
            int j,*pChrCpy;
            pChrCpy = pChr;

            // Skip charactor code
            pChrCpy++;

            while( pChrCpy[0]!=-1 )
            {
                switch(pChrCpy[0])
                {
                default:
                case 0:
                    glBegin(GL_POLYGON);
                    break;
                case 1:
                    glBegin(GL_LINE_STRIP);
                    break;
                case 2:
                    glBegin(GL_LINES);
                    break;
                }

                for( j=0; j<pChrCpy[1]; j++ )
                    glVertex2f( ((pChrCpy[2+j*2]) / 10000.0f) * fScale, ((pChrCpy[3+j*2]) / 10000.0f) * fScale );

                glEnd();

                pChrCpy = pChrCpy+2+pChrCpy[1]*2;
            }
        }
        glTranslated( ((g_fFontWidth*8.0/7.0) / 9000.0f) * fScale ,0.0,0.0);
    }

    glLoadIdentity();
    oTrans.translate( 0.0, 0.0, -0.01f );
    oTrans.applyToGL();

    glLineWidth( 1.5 );
    glColor3f( fR, fG, fB );

    for( int i=0; pStr[i]!=0; i++ )
    {
        int *pChr = g_sFontSet[((unsigned char *)pStr)[i]];

        if( pChr!=NULL )
        {
            int j,*pChrCpy;
            pChrCpy = pChr;

            // Skip charactor code
            pChrCpy++;

            while( pChrCpy[0]!=-1 )
            {
                switch(pChrCpy[0])
                {
                default:
                case 0:
                    glBegin(GL_POLYGON);
                    break;
                case 1:
                    glBegin(GL_LINE_STRIP);
                    break;
                case 2:
                    glBegin(GL_LINES);
                    break;
                }

                for( j=0; j<pChrCpy[1]; j++ )
                    glVertex2f( ((pChrCpy[2+j*2]) / 10000.0f) * fScale, ((pChrCpy[3+j*2]) / 10000.0f) * fScale );

                glEnd();

                pChrCpy = pChrCpy+2+pChrCpy[1]*2;
            }
        }
        glTranslated( ((g_fFontWidth*8.0/7.0) / 9000.0f) * fScale ,0.0,0.0);
    }

    glEnable( GL_LIGHTING );
    glDisable( GL_BLEND );
    glDisable( GL_LINE_SMOOTH );
    glDepthFunc(GL_LESS);

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
