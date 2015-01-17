
#include <svt_core.h>
#include <svt_scene.h>
#include <svt_sphere.h>
#include <svt_pipette.h>
#include <svt_pos.h>
#include <svt_box.h>
#include <svt_move_tool.h>
#include <svt_sky.h>
#include <svt_HeightField.h>

#include <svt_scene.h>
#include <svt_init.h>

#include <ctype.h>


//
// internal prototypes
//
void usage(const char* exe_name);
void getArgs(int argc, char** argv, 
			 char*& in_file, char*& qt_file, char*& texture_file,
			 float& fEps, svt_HeightField::ResizeMode& eResizeMode, 
			 int& iMaxNoTri );

// tool_scene class
class hf_scene : public svt_scene
{

private:
  svt_HeightField m_oHF;
  svt_sky         m_oSky;
  svt_pipette*    m_poPipette;
  svt_move_tool*  m_pMoveTool;

  svt_str         m_oDataDir;

public:
  hf_scene(const char* height_file, const char* qt_file=NULL, 
		   const char* texture_file=NULL, Real32 fError=-1.0, 
		   svt_HeightField::ResizeMode eResizeMode=svt_HeightField::Center,
		   int iMaxTri=-1) 
	: svt_scene(), m_oHF(height_file, qt_file, texture_file, fError, eResizeMode, iMaxTri), 
	  m_oSky(), m_oDataDir(height_file)
  {
	if (m_oDataDir)
	  {
		char* ptr= strrchr(m_oDataDir, '/');
		if (!ptr)
		  ptr = strrchr(m_oDataDir, '\\');
		if (ptr)
		  {
			*ptr = '\0';
			cout << "setting data dir to " << m_oDataDir << endl;
		  }
	  }
  }


  virtual ~hf_scene() {delete m_poPipette;}

  virtual void keyPressed(unsigned char key, int x, int y) {
	static bool s_oldFreeze=false;
	static bool s_oldSky=true;
	static int s_oldTLimit=50000;

	char szFilename[1024];

	bool bFound=false;
	svt_properties* pProp = m_oHF.getProperties();
	int iT;
	int i;
	svt_vector4<Real32> vec;

	switch(tolower(key))
	  {
	  // toggle wireframe
	  case 'w': 
		pProp->toggleWireframe(); bFound=true; break;

	  case 'f': 
		pProp->toggleSolid(); bFound=true; break;

	  case 'd': 
		vec=m_pMoveTool->APpos();
		cout << "current spline point: " << vec << endl
			 << "height over ground: " << m_oHF.height(vec[0], vec[1]) << endl;
		bFound=true; break;

	  case 'b': 
		if (!m_oHF.getBird())
		  {
			s_oldFreeze=m_oHF.freeze();
			s_oldSky=m_oSky.getVisible();
			m_oHF.freeze(true);
			m_oHF.drawBox(true);
			m_oSky.setVisible(false);
		  }
		else
		  {
			m_oHF.freeze(s_oldFreeze);
			m_oHF.drawBox(false);
			m_oSky.setVisible(s_oldSky);
		  }
  
		m_oHF.toggleBird(); 
		bFound=true; 
		break;

	  case 'm': 
		m_poPipette->toggleVisible(); bFound=true; break;

	  case 't': 
		pProp->toggleTexture(); bFound=true; break;

	  case 'l': 
		pProp->toggleLighting(); bFound=true; break;

	  case 'g': 
		pProp->toggleSmooth(); bFound=true; break;

	  case 'p': 
		m_oHF.toggleStatistics(); bFound=true; break;

	  case 'x': 
		m_oHF.toggleFreeze(); bFound=true; break;

	  case 'c': 
		m_oHF.toggleCulling(); bFound=true; break;

	  case 'h': 
		m_oSky.setVisible(!m_oSky.getVisible()); bFound=true; break;

	  case 'q': 
		m_oHF.toggleSimplification(); bFound=true; break;

	  case 'y': 
		m_oHF.toggleQuadPermFreeze(); bFound=true; break;

	  case 'z': 
		if (m_oHF.simplify())
		  {
			iT = m_oHF.triangleLimit();
			if (iT<=0)
			  m_oHF.triangleLimit(s_oldTLimit);
			else
			  {
			  s_oldTLimit = iT;
			  m_oHF.triangleLimit(-1);
			  }
		  }
		bFound=true; 
		break;

	  case '.':
		iT = m_oHF.triangleLimit();
		if (m_oHF.simplify() && iT>0)
		  {
			m_oHF.triangleLimit(iT+5000);
		  }
		bFound=true; break;

	  case ',': 
		iT = m_oHF.triangleLimit();
		if (m_oHF.simplify() && iT>0)
		  {
			m_oHF.triangleLimit(max(iT-5000, 5000));
		  }
		bFound=true; break;

	  case '+': 
		m_oHF.decreaseError(); bFound=true; break;

	  case '-': 
		m_oHF.increaseError(); bFound=true; break;

	  case 'v':
		for (i=1;i<9;i++)
		  {
			cout << endl << m_oDataDir << "    " << i << endl;
			sprintf(szFilename, "%s/viewpoint%d", (const char*) m_oDataDir, i);

			if (!svt_file_exisits(szFilename))
			  {
				cout << "Saving current view as viewpoint " << i << endl;
				m_oHF.saveTransformation(szFilename);
				break;
			  }
		  }
		bFound=true; break;

	  case '1':
	  case '2':
	  case '3':
	  case '4':
	  case '5':
	  case '6':
	  case '7':
	  case '8':
	  case '9':
		sprintf(szFilename, "%s/viewpoint%c", (const char*) m_oDataDir, key);
		if (svt_file_exisits(szFilename) )
		  m_oHF.readTransformation(szFilename);
		else
		  cout << "Viewport " << key << " not defined yet!" << endl; 
		bFound=true; break;

	  case 'e': 
		m_oHF.benchmark(m_oDataDir); bFound=true; break;

	  case ' ':
		m_pMoveTool->toggleFlyMode(); bFound=true; break;

	  case 'n':
		m_pMoveTool->setSpeed(0); bFound=true; break;

	  case 'a':
		if (!m_pMoveTool->autopilot())
		  {
		  sprintf(szFilename, "%s/autopilot.dat", (const char*) m_oDataDir );
		  m_pMoveTool->startAutoPilot(szFilename, svt_vector4<Real32>(0,0,1,0)); 
		  m_pMoveTool->setFlyMode(false);
		  }
		else
		  m_pMoveTool->stopAutoPilot();
		bFound=true; break;
	  }
	if (!bFound)
	  svt_scene::keyPressed(key, x, y);
  }
  
  virtual void specialKeyPressed(int key, int x, int y)
  {

	bool bFound=false;
    switch(key)
	  {
	  case SVT_KEY_UP:
        m_pMoveTool->flyFaster();
		bFound=true;
        break;

	  case SVT_KEY_DOWN:
        m_pMoveTool->flySlower();
		bFound=true;
        break;

    case SVT_KEY_RIGHT:
        m_pMoveTool->turnRight();
		bFound=true;
        break;

    case SVT_KEY_LEFT:
        m_pMoveTool->turnLeft();
		bFound=true;
        break;

    case SVT_KEY_PAGE_UP:
        m_pMoveTool->up();
		bFound=true;
        break;

    case SVT_KEY_PAGE_DOWN:
        m_pMoveTool->down();
		bFound=true;
        break;

	  }

	if (!bFound)
	  svt_scene::specialKeyPressed(key, x, y);

  }



  /** build the scene */
  void buildScene() {
	m_pSceneGraph = new svt_scenegraph();
	
	// construct empty node
	svt_node* empty = new svt_node();
	svt_matrix4f* mat = new svt_matrix4f();
	mat->setToId();

	empty->setTransformation(mat);
	m_pSceneGraph->add(empty);
	
	// add height field
	empty->add(&m_oHF);

	svt_pos* pPos = new svt_pos( -(m_oHF.box()[0][1]/2),  -(m_oHF.box()[1][1]/2), -(m_oHF.box()[2][1]*3.5));
	m_oHF.setPos(pPos);

	// color / material properties / drawing properties of m_oHF

	svt_color* c = new svt_color(0.875, 0.625, 0.5);
	c->setAmbient(0.125,0.125,0.125,1);
	c->setSpecular(0.125,0.125,0.125,1);
	c->set(0.875, 0.625, 0.5);
	c->setShininess(32.0);
	svt_properties* p = new svt_properties(c);
	p->setSmooth(false);
	
	if (m_oHF.getTexture())
		p->setLighting(false);

	m_oHF.setProperties(p);


	// set sky size and add it as a child of the height field
	//	m_oSky.setRadius(sqrt(m_oHF.box()[0][1]*m_oHF.box()[0][1] + m_oHF.box()[1][1]*0.8));
	m_oSky.setRadius(m_oHF.box()[0][1], m_oHF.box()[1][1]);
	m_oSky.setHeight(m_oHF.box()[2][1]*30);
	m_oSky.setGround(true);
	m_oHF.add(&m_oSky);



	// set sky transformation
	svt_matrix4f* pMat = new svt_matrix4f;

	pMat->postAddRotation(0,-fPI/2);
	pMat->postAddTranslation(m_oHF.box()[0][1]/2,m_oHF.box()[1][1]/2, -(m_oHF.box()[2][1])*2);
	
	m_oSky.setTransformation(pMat);
	//	m_oSky.getProperties()->setLighting(true);

	// create a manipulator tool
	m_pMoveTool = new svt_move_tool("Pointer", &m_oHF);
	m_pMoveTool->setGlobalMatrix(mat);
	m_pMoveTool->setCenterMode(false);
	m_pMoveTool->setFlyMode(true);

	svt_properties* red = new svt_properties(new svt_color(1.0f,0.0f,0.0f));
	m_poPipette = new svt_pipette(new svt_pos(0.0f,0.0f,0.0f), red);
	m_poPipette->setVisible(false);
	m_pMoveTool->add(m_poPipette);
	m_pSceneGraph->add(m_pMoveTool);

	// Lighting
	svt_light* light = new svt_light();
	svt_lightsource* source = new svt_lightsource(new svt_pos(0.0f,1.0f,2.0f));
	source->setInfinite(true);
	source->setAmbient(1.0,1.0,1.0,1.0);
	source->setDiffuse(1.0,1.0,1.0,1.0);
	source->setSpecular(1.0,1.0,1.0,1.0);
	light->add(source);
	setLight(light);

	svt_color* bg_col=new svt_color(0.5, 0.625, 0.75);
    svt_setBackgroundColor(bg_col);

    };

  virtual void resizeScene(int, int, int, int)
  {
	//	cout << "resizing the scene!!!!" << endl;

	// new alpa values have already been set by canvas manager
	// now simply call setEpsilon() to enforce recaluculation
	// of gamma values with new alpha-value
	m_oHF.setEpsilon(m_oHF.getEpsilon());
  }

};

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//                                                                //
//                                                                //
//                      M A I N                                   //
//                                                                //
//                                                                //
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{

  try
	{

	//
	// get command line args
	//
	char* in_file=NULL;
	char* qt_file=NULL;
	char* tex_file=NULL;

	int iMaxNoTri = -1;
	float fEps=-1.0;
	svt_HeightField::ResizeMode eResizeMode=svt_HeightField::Center;

	getArgs(argc, argv, in_file, qt_file, tex_file, fEps, eResizeMode, iMaxNoTri);
	
	//
	// correct / throw if inforamtion is incomplete
	//
	if (!in_file)
	  {
	  usage(argv[0]);
	  throw svt_exception("Incorrect Arguments");
	  }


	if (!qt_file)
	  fEps=-1.0;

	cout << endl
		 << "       VHF - Virtual Height Fields" << endl
		 << "--------------------------------------------------" << endl
		 << "(c) Frank Delonge, Forschungszentrum Juelich, 2002" << endl << endl
		 << "Key Bindings: " << endl
		 << " w     : toggle wireframe" << endl
		 << " f     : toggle faces" << endl
		 << " t     : toggle texture (if specified)" << endl
		 << " l     : toggle lighting" << endl
		 << " g     : toggle shading mode (smooth/flat)" << endl
		 << " p     : print statistics" << endl
		 << " m     : toggle visibility of move tool" << endl
		 << " h     : toggle sky visibility" << endl
		 << " b     : toggle bird perspective" << endl
		 << " y     : freeze/unfreeze current quad permutation" << endl
		 << " x     : freeze scene" << endl
		 << " z     : toggle triangle limitation" << endl
		 << " ,     : reduce max. amount of triangles" << endl
		 << " .     : increase max. amount of triangles" << endl
		 << " v     : save current viewpoint/direction to next free viewpoint number" << endl
		 << "         (up to 9 available)" << endl
		 << " 1     : jump to viewpoint 1 (if defined)" << endl
		 << " 2     : jump to viewpoint 2 (if defined)" << endl
		 << " 3     : jump to viewpoint 3 (if defined)" << endl
		 << " 4     : jump to viewpoint 4 (if defined)" << endl
		 << " 5     : jump to viewpoint 5 (if defined)" << endl
		 << " 6     : jump to viewpoint 6 (if defined)" << endl
		 << " 7     : jump to viewpoint 7 (if defined)" << endl
		 << " 8     : jump to viewpoint 8 (if defined)" << endl
		 << " 9     : jump to viewpoint 9 (if defined)" << endl
		 << " q     : toggle simplification" << endl
		 << " n     : set speed to 0" << endl
		 << " a     : toggle autopilot" << endl
		 << " +     : higher resolution" << endl
		 << "         in simplification mode: decrease screen-space error tolerance" << endl
		 << "         else                  : upsample by factor 2" << endl
		 << " -     : lower resolution" << endl
		 << "         in simplification mode: increase screen-space error tolerance" << endl
		 << "         else                  : downsample by factor 2" << endl
		 << " UP    : increase flight speed" << endl
		 << " DOWN  : decrease flight speed" << endl
		 << " LEFT  : turn left" << endl
		 << " RIGHT : turn right" << endl
		 << " MB1   : rotate" << endl
		 << " MB2   : move foreward/backward" << endl
		 << " MB3   : move up/down/left/right" << endl
		 << " SPACE : toggle flying" << endl;




	//
	// init svt and create the heightfield
	//

	svt_setWindowCaption("VHF - Virtual Height Fields");

	svt_init(argc, argv);

    // build the scene
	hf_scene oScene(in_file, qt_file, tex_file, fEps, eResizeMode, iMaxNoTri);

    oScene.buildScene();



    // ...and now start svt
    svt_start(&oScene);
	}

  catch (svt_exception& oErr) {
	oErr.uiMessage();
  }

  return 0;

}


//
// process command line args
//
void getArgs(int argc, char** argv, 
			 char*& in_file, char*& qt_file, char*& tex_file,
			 float& fEps, svt_HeightField::ResizeMode& eResizeMode,
			 int& iMaxNoTri)
{

  int reg_arg_count=0;
  char* ptr;
  for (int i=1; i<argc; i++)
	{
	if (*(argv[i]) == '-' )

	  //
	  // options
	  //
	  switch (argv[i][1])
		{
		//
		// -m => resize-mode = Center
		//
		case 'm': 
		case 'M':
		  eResizeMode=svt_HeightField::Center;
		  break;

		//
		// -i => resize-mode = Interpolate
		//
		case 'i': 
		case 'I':
		  eResizeMode=svt_HeightField::Interpolate;
		  break;

		//
		// -L, -l  => limit amount of triangles
		//
		case 'L': 
		case 'l': 
		  ptr=argv[i]+2;
		  if (strlen(ptr)<=0)
			ptr=argv[++i];
		  sscanf(ptr, "%d", &iMaxNoTri);
		  break;

		//
		// -e, -E  => Epsilon
		//
		case 'E': 
		case 'e': 
		  ptr=argv[i]+2;
		  if (strlen(ptr)<=0)
			ptr=argv[++i];
		  sscanf(ptr, "%f", &fEps);
		  break;

		//
		// -T, -t  => texture file
		//
		case 'T': 
		case 't': 
		  ptr=argv[i]+2;
		  if (strlen(ptr)<=0)
			ptr=argv[++i];
		  tex_file=ptr;
		  break;



		default: 
		  cerr << "Unknown option -" << argv[i][1] << " - ignoring..." 
			   << endl; 
		  break;
		}

	else
	  {
	  switch (reg_arg_count)
		{
		case 0: 
		  in_file  = argv[i]; 
		  break;
		case 1: 
		  qt_file = argv[i]; 
		  break;
		default: 
		  cerr << "Unknown argument " << argv[i] << " - ignoring..." << endl;
		  break;
		}
	  reg_arg_count++;
	  }
	}
}

void usage(const char* exe_name)
{
  cerr << "usage: " << exe_name << "<in_file> [<qt_file>] [-e <epsilon>] [-t <texture_file>] [-l <MaxTri>] [-m] [-i]" << endl
	   << " with" << endl  
	   << "  in_file           : filename of elevation input data (.height-file)" << endl
	   << "  qt_file           : filename of quadtree output data (.qt-file)." << endl 
	   << "                      If omitted, no simplification can be performed!" << endl
	   << "  -t <texture_file> : specified a texture file" << endl
	   << "  -e <epsilon>      : specified error tolerance epsilon." << endl
	   << "                      If omitted, no simplification will be performed!" << endl 
	   << "  -m                : set resize-mode to CENTER" << endl
	   << "  -i                : set resize-mode to INTERPOLATE" << endl
	   << "  -l <MaxTri>       : sets approximate maximum of triangles per window" << endl
	   << "                      (unlimited if omitted)" << endl;
  return;
}
