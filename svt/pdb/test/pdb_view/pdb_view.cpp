#include <svt_scenegraph.h>
#include <svt_light.h>
#include <svt_core.h>
#include <svt_init.h>
#include <svt_scene.h>
#include <svt_pdb.h>
#include <svt_pipette.h>
#include <svt_move_tool.h>
#include <svt_fps.h>
#include <svt_billboard.h>

#include <svt_stlVector.h>

// pdb_scene class
class pdb_scene : public svt_scene
{

protected:

    bool m_bUpdateTimesteps;
    bool m_bUseShaderCartoon;

    char* m_pFilename;

    int m_iDisplayMode;

    svt_scenegraph* m_pGraph;

    svt_pdb* m_pPDB;
    svt_node* m_pEmpty;
    svt_matrix4f m_oMatrix;

    float m_fMinX, m_fMaxX;
    float m_fMinY, m_fMaxY;
    float m_fMinZ, m_fMaxZ;

public:
    pdb_scene() : svt_scene(), m_oMatrix( "Test" )
    {
	m_bUpdateTimesteps  = false;
	m_bUseShaderCartoon = false;
    };

public:
    /** build the scene */
    void buildScene()
    {
        // construct a scenegraph
        m_pGraph = new svt_scenegraph();

        // Lighting
        svt_light* light = new svt_light();
        svt_lightsource* ls = new svt_lightsource(new svt_pos(0.0f,0.0f,2.0f));
        ls->setInfinite(true);
	light->setName("light");
        light->add(ls);
        setLight(light);

        // create empty node
	m_pEmpty = new svt_node();
	m_pEmpty->setTransformation( &m_oMatrix );
	m_pEmpty->setName("empty");
	m_pGraph->add(m_pEmpty);

        // create pdb object
	m_pPDB = new svt_pdb(m_pFilename, SVT_PDB);

        // calculate the center of mass
	m_fMinX = m_pPDB->getMinXCoord();
	m_fMaxX = m_pPDB->getMaxXCoord();
	m_fMinY = m_pPDB->getMinYCoord();
	m_fMaxY = m_pPDB->getMaxYCoord();
	m_fMinZ = m_pPDB->getMinZCoord();
	m_fMaxZ = m_pPDB->getMaxZCoord();
	Real32 fSizeX = (m_fMaxX - m_fMinX)/2;
	Real32 fSizeY = (m_fMaxY - m_fMinY)/2;
	Real32 fSizeZ = (m_fMaxZ - m_fMinZ)/2;
        Real32 fPosX = m_fMinX + fSizeX;
        Real32 fPosY = m_fMinY + fSizeY;
	Real32 fPosZ = m_fMinZ + fSizeZ;
        fPosX *= 1.0E-2f;
        fPosY *= 1.0E-2f;
        fPosZ *= 1.0E-2f;

        // move main coordinate system so that the center of mass is in the center of the screen
	m_oMatrix.setToId();
        m_oMatrix.setTranslationX( -fPosX );
        m_oMatrix.setTranslationY( -fPosY );
	m_oMatrix.setTranslationZ( -fPosZ );

        // output some stats
        cout << "svt_pdb: #atoms: " << m_pPDB->size() << endl;
        cout << "svt_pdb: #bonds: " << m_pPDB->getBondsNum() << endl;

        // adjust settings
        m_pPDB->setDisplayMode(m_iDisplayMode);
	if (m_bUseShaderCartoon)
	    m_pPDB->setCartoonParam(CARTOON_USE_SHADER, 1.0f);
        m_pEmpty->add(m_pPDB);

        // create fps
        //svt_billboard* bill = new svt_billboard();
        //bill->add(new svt_fps(new svt_pos(0.0f,0.0f,-1.0f)));
        //m_pGraph->add(bill);

        // create a tool
	svt_move_tool* tool = new svt_move_tool("Pointer", m_pEmpty);
        tool->setPivotPoint( svt_vector4<Real32>(fPosX, fPosY, fPosZ) );
        tool->setCenterMode(true);
        //tool->add(new svt_pipette(new svt_pos(0.0f,0.0f,0.0f), svt_properties_solid_white));
        m_pGraph->add(tool);

        // put the sg into the scene
        setSceneGraph(m_pGraph);
    };

    virtual void keyPressed(unsigned char key, int x, int y)
    {
        svt_atom* pAtom;

        if (m_pPDB == NULL)
            return;

        switch(key)
        {
        case '+':
            m_pPDB->setTubeSegs( m_pPDB->getTubeSegs() + 1 );
            m_pPDB->rebuildDL();
            break;
        case '-':
            m_pPDB->setTubeSegs( m_pPDB->getTubeSegs() - 1 );
            m_pPDB->rebuildDL();
            break;

        case '.':
	    m_pPDB->setTubeDiameter( m_pPDB->getTubeDiameter() + 0.3f );
	    svtout << "DEBUG: TubeDiameter: " << m_pPDB->getTubeDiameter() << endl;
            m_pPDB->rebuildDL();
            break;
        case ',':
            m_pPDB->setTubeDiameter( m_pPDB->getTubeDiameter() - 0.3f );
	    svtout << "DEBUG: TubeDiameter: " << m_pPDB->getTubeDiameter() << endl;
            m_pPDB->rebuildDL();
            break;

        case 'p':
        case 'P':
            m_pPDB->setDisplayMode(SVT_PDB_POINT);
            break;
        case 'l':
        case 'L':
            m_pPDB->setDisplayMode(SVT_PDB_LINE);
            break;
        case 'c':
        case 'C':
            m_pPDB->setDisplayMode(SVT_PDB_CPK);
            break;
        case 'k':
        case 'K':
            m_pPDB->setDisplayMode(SVT_PDB_CARTOON);
            break;
        case 'd':
        case 'D':
            m_pPDB->setDisplayMode(SVT_PDB_DOTTED);
            break;
        case 'v':
        case 'V':
            m_pPDB->setDisplayMode(SVT_PDB_VDW);
            break;
        case 'o':
        case 'O':
            m_pPDB->setDisplayMode(SVT_PDB_OFF);
            m_pPDB->resetAtomEnum();
            while((pAtom = (svt_atom*)m_pPDB->enumAtomType("CA")) != NULL)
            {
                if (pAtom->getSecStruct() == 'E' || pAtom->getSecStruct() == 'B')
                    pAtom->setDisplayMode(SVT_PDB_VDW);
            }
            break;
        case 'i':
        case 'I':
            m_pPDB->setDisplayMode(SVT_PDB_LICORIZE);
            break;
        case 't':
        case 'T':
            m_pPDB->setDisplayMode(SVT_PDB_TUBE);
            break;
        case 'y':
        case 'Y':
            m_pPDB->setDisplayMode(SVT_PDB_NEWTUBE);
            break;
        case '1':
            m_pEmpty->getTransformation()->setFromTranslation(-0.5f,-0.5f,-0.5f);
	    break;
	case 'r':
	    outputOBJ("pdbview.obj");
	    outputMAT("pdbview.mtl");
            break;
	case 'q':
	    m_pPDB->setNewTubeProfileCorners(m_pPDB->getNewTubeProfileCorners()+2);
	    svtout << "DEBUG: NewTubeProfileCorners: " << m_pPDB->getNewTubeProfileCorners() << endl;
            break;
	case 'w':
	    m_pPDB->setNewTubeProfileCorners(m_pPDB->getNewTubeProfileCorners()-2);
	    svtout << "DEBUG: NewTubeProfileCorners: " << m_pPDB->getNewTubeProfileCorners() << endl;
            break;
        case 'm':
        case 'M':
            m_pPDB->setCartoonParam(CARTOON_USE_SHADER, (float)(!((bool)m_pPDB->getCartoonParam(CARTOON_USE_SHADER))));
            m_pPDB->rebuildDL();
            break;
	case 'b':
	    // it is recommended to compile svt with SVT_BACKEND_X11 when you want to make performance measurements
	    if (m_bUpdateTimesteps)
	    {
		getPDB()->setAutoDL(true);
		svt_setAlwaysRedraw(false);
		updateTimesteps(false);
		m_pPDB->rebuildDL();
	    }
	    else
	    {
		getPDB()->setAutoDL(false);
		svt_setAlwaysRedraw(true);
		updateTimesteps(true);
		m_pPDB->rebuildDL();
	    }
	    break;
        }
        svt_scene::keyPressed(key, x, y);
    }

    /**
     * make some opengl settings here
     */
    void prepareGL()
    {
        // no local lighting and two side lighting
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);

        Real32 m_aMatSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        Real32 m_aMatShininess[1] = { 20.0f };
        glMaterialfv(GL_FRONT, GL_SPECULAR, m_aMatSpecular);
        glMaterialfv(GL_FRONT, GL_SHININESS, m_aMatShininess);

        glEnable(GL_NORMALIZE);

 	if (m_bUpdateTimesteps)
 	    m_pPDB->setTimestep( (m_pPDB->getTimestep() + 1) % m_pPDB->getMaxTimestep() );
    }

    void setFilename(char* _tf)
    {
        m_pFilename = _tf;
    }

    void setDisplayMode(int i)
    {
        m_iDisplayMode = i;
    }

    svt_pdb * getPDB()
    {
	return m_pPDB;
    }

    void updateTimesteps(bool b)
    {
	m_bUpdateTimesteps = b;
    }

    void useShaderCartoon(bool b)
    {
	m_bUseShaderCartoon = b;
    }
};

// main
int main(int argc, char *argv[])
{
    // init svt
    svt_init(argc,argv);

    // create a new scene object
    pdb_scene* scene = new pdb_scene();
    scene->setDisplayMode(SVT_PDB_LINE);

    // parameter checking
    if (argc == 1)
    {
        cout << endl;
        cout << "pdb_view usage: ./pdb_view <file.pdb> [ -line | -cpk | -point | -vdw | -dotted | -tube | -newtube | -snewtube | -cartoon | -scartoon ]" << endl;
        cout << endl;
        svt_exit(1);
    }

    if (argc == 2)
        scene->setFilename(argv[1]);

    if (argc == 3)
    {
        scene->setFilename(argv[1]);
        if (strcmp(argv[2], "-cpk") == 0)
            scene->setDisplayMode(SVT_PDB_CPK);
        if (strcmp(argv[2], "-line") == 0)
            scene->setDisplayMode(SVT_PDB_LINE);
        if (strcmp(argv[2], "-tube") == 0)
            scene->setDisplayMode(SVT_PDB_TUBE);
        if (strcmp(argv[2], "-point") == 0)
            scene->setDisplayMode(SVT_PDB_POINT);
        if (strcmp(argv[2], "-vdw") == 0)
            scene->setDisplayMode(SVT_PDB_VDW);
        if (strcmp(argv[2], "-dotted") == 0)
            scene->setDisplayMode(SVT_PDB_DOTTED);
        if (strcmp(argv[2], "-licorize") == 0)
            scene->setDisplayMode(SVT_PDB_LICORIZE);
        if (strcmp(argv[2], "-newtube") == 0)
            scene->setDisplayMode(SVT_PDB_NEWTUBE);
        if (strcmp(argv[2], "-snewtube") == 0)
	{
            scene->setDisplayMode(SVT_PDB_NEWTUBE);
	    scene->useShaderCartoon(true);
	}
        if (strcmp(argv[2], "-cartoon") == 0)
            scene->setDisplayMode(SVT_PDB_CARTOON);
        if (strcmp(argv[2], "-scartoon") == 0)
	{
            scene->setDisplayMode(SVT_PDB_CARTOON);
	    scene->useShaderCartoon(true);
	}
        if (strcmp(argv[2], "-toon") == 0)
            scene->setDisplayMode(SVT_PDB_TOON);
    }

    if (argc == 4)
        scene->setFilename(argv[3]);

    if (argc == 5)
    {
        scene->setFilename(argv[3]);
        if (strcmp(argv[4], "-cpk") == 0)
            scene->setDisplayMode(SVT_PDB_CPK);
        if (strcmp(argv[4], "-line") == 0)
            scene->setDisplayMode(SVT_PDB_LINE);
        if (strcmp(argv[4], "-tube") == 0)
            scene->setDisplayMode(SVT_PDB_TUBE);
        if (strcmp(argv[4], "-point") == 0)
            scene->setDisplayMode(SVT_PDB_POINT);
        if (strcmp(argv[4], "-vdw") == 0)
            scene->setDisplayMode(SVT_PDB_VDW);
        if (strcmp(argv[4], "-dotted") == 0)
            scene->setDisplayMode(SVT_PDB_DOTTED);
        if (strcmp(argv[4], "-licorize") == 0)
            scene->setDisplayMode(SVT_PDB_LICORIZE);
        if (strcmp(argv[4], "-newtube") == 0)
            scene->setDisplayMode(SVT_PDB_NEWTUBE);
        if (strcmp(argv[2], "-snewtube") == 0)
	{
            scene->setDisplayMode(SVT_PDB_NEWTUBE);
	    scene->useShaderCartoon(true);
	}
        if (strcmp(argv[4], "-cartoon") == 0)
            scene->setDisplayMode(SVT_PDB_CARTOON);
        if (strcmp(argv[4], "-scartoon") == 0)
	{
            scene->setDisplayMode(SVT_PDB_CARTOON);
	    scene->useShaderCartoon(true);
	}
        if (strcmp(argv[4], "-toon") == 0)
            scene->setDisplayMode(SVT_PDB_TOON);
    }

    // and build the scene
    scene->buildScene();

    // ...and now start svt
    svt_start(scene);
    
    return 0;
}
