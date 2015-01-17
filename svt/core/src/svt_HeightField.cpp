
//
// includes

#include <svt_QuadTree.h>
#include <svt_HeightField.h>
#include <svt_fstream.h>
#include <svt_bfstream.h>
#include <svt_streamableVectors.h>
#include <svt_triStrip.h>


//
// includes of private classes
//



///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  Class Implementation: svt_HeightField                                //
//                                                                       //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

//
// constructor 
//=============
//   reads heightfield-data (must be specified)
//   reads precomputed quadtree data (if specified)
//   reads texture and prepares automatic texture coordinate generation (if specified)
//   finished per-quad precomputation with given error tolerance (fEpsilon<0 -> no simplification)
//   a approx. maximum of triangles per screen may be specified
//
svt_HeightField::svt_HeightField
(const char* pszHeightFile, const char* pszQTFile, const char* pszTextureFile, 
 Real32 fEpsilon, ResizeMode eResizeMode, int iMaxTriangles,
 ObjErrMode eObjErrMode, Real32 fGPDistX, Real32 fGPDistY, Real32 fHeightScale)
  : svt_node(), 
	m_oElevationData(0,0),
	m_iSizeX(-1), m_iSizeY(-1), m_n(-1), 
	m_fGPDistX(-1.0), m_fGPDistY(-1.0), 
	m_oQuadTree(0), 
	m_oContextData(), m_fEpsilon(fEpsilon), m_fCalcEpsilon(-1.0),
	m_bDrawStat(false), m_Box(3,2), 
	m_oTexture(pszTextureFile), 
	m_strQTFile(pszQTFile),
	m_eObjErrMode(eObjErrMode),
	m_eResizeMode(eResizeMode),
	m_iDownsample(1),
	m_bFreeze(false),
	m_bCulling(true),
	m_iTriangleLimit(iMaxTriangles),
	m_bBenchmarkRun(false)
{
  //
  // no auto DL creation
  //
  setAutoDL(false);

  //
  // read elevation data
  //
  readHeightfield(pszHeightFile, fGPDistX, fGPDistY, fHeightScale);

  //
  // pick a default qt-filename if none specified
  //
  if (!m_strQTFile)
	{
	  char szDefFilename[1024];
	  char* ptr=szDefFilename;
	  sprintf(ptr, "%s", pszHeightFile ? pszHeightFile : "testcase");
	  ptr=strrchr(ptr, '.');
	  if (!ptr)
		ptr=szDefFilename+strlen(szDefFilename);
	  sprintf(ptr, "_%s_%s_%s_%d_%d.qt", 
			  (sphereMode() == Small) ? "s" : "n",
			  (objErrMode() == Real ) ? "r" : "z",
			  (resizeMode() == Center ) ? "c" : "i",
			  m_iSizeX, m_iSizeY);
	  m_strQTFile=szDefFilename;
	  cout << "Created default qt filename " << m_strQTFile << endl;
	}

  //
  // read and prepare the quadtree data, if present
  //
  if (isPositive(fEpsilon))
	setEpsilon(fEpsilon);

  //
  // set parameters for automatic texture coordinate generation,
  // if texture was specified
  //
  if (pszTextureFile)
	{
	  m_oTexture.autoCoordGen(m_Box[0][0], m_Box[0][1], m_Box[1][0], m_Box[1][1]);
	  setTexture(&m_oTexture);
	}

  //
  // init data
  //
  forAllCanvas(m_oContextData, init(m_n, (iMaxTriangles>0) ? iMaxTriangles/6 : -1)); 
  svt_Quad::setSize(m_n, m_oElevationData.width());

}


//
// Copy Contructor
//=================
//

svt_HeightField::svt_HeightField(const svt_HeightField& that)
  : svt_node(that),
	m_oElevationData(that.m_oElevationData),
	m_iSizeX(that.m_iSizeX),
	m_iSizeY(that.m_iSizeY),
	m_n(that.m_n),
	m_fGPDistX(that.m_fGPDistX),
	m_fGPDistY(that.m_fGPDistY),
	m_oQuadTree(that.m_oQuadTree),
	m_oContextData(that.m_oContextData),
	m_fEpsilon(that.m_fEpsilon),
	m_fCalcEpsilon(that.m_fCalcEpsilon),
	m_bDrawStat(that.m_bDrawStat),
	m_Box(that.m_Box),
	m_strQTFile(that.m_strQTFile),
	m_eObjErrMode(that.m_eObjErrMode),
	m_eResizeMode(that.m_eResizeMode),
	m_iDownsample(that.m_iDownsample),
	m_bFreeze(that.m_bFreeze),
	m_bCulling(true),
	m_bBenchmarkRun(false)
  {}






//
// readHeightfield(const char* pszFilename)
//=========================================
// read in .height-file 
// scale to have virtual height of 2m
// create bounding box
//
void svt_HeightField::readHeightfield
(const char* pszFilename,  
 Real32 fGPDistX, Real32 fGPDistY, Real32 fHeightScale)
{

  //
  // check if we are dealing with a .height-file
  //
  if (!pszFilename)
	{
	if (!m_strQTFile)
	  throw svt_exception("svt_HeightField::readHeightfield: no filename specified!");
	  
	//
	// create artifical test-data if file does not exist
	//
	if ( (sscanf(m_strQTFile, "%d", &m_iSizeX) !=1) || m_iSizeX<0)
	  throw svt_exception("svt_HeightField::readHeightfield: no filename specified!");

	m_iSizeY=m_iSizeX;
	char* ptr = strchr(m_strQTFile, 'x');
	if (ptr)
	  sscanf(ptr+1, "%d", &m_iSizeY);
	

	m_strQTFile=NULL;
	m_oElevationData.resize(m_iSizeY,m_iSizeX);
	int k=0;
	for (int i=0; i<m_iSizeY; i++)
	  for (int j=0; j<m_iSizeX; j++)
		m_oElevationData[i][j] = ((k++)%2)*20.0;
	m_oElevationData[m_iSizeY/2][m_iSizeX/2]=15.0;
	m_Box[2][0] = m_oElevationData.min();
	m_Box[2][1] = m_oElevationData.max();
	m_fGPDistX=fGPDistX;
	m_fGPDistY=fGPDistY;

	cout << "created aticifal data:" << endl << m_oElevationData;
	}
  else
	{
	if (strstr(pszFilename, ".height"))
	  {
	  //
	  // open file
	  //
	  svt_ibfstream s(pszFilename);
		
	  // 
	  // read gridpoint distances, elevation matrix and min/max z
	  //
	  s >> m_fGPDistX >> m_fGPDistY >> m_oElevationData 
		>> m_Box[2][0] >> m_Box[2][1];
	  
	  }
	else
	  {
	  svt_texture oTData(pszFilename);
	  unsigned const char* pRawData = oTData.data();
	  if (pRawData)
		throw svt_exception("svt_HeightField::readHeightfield: unrecognized file format");

	  m_fGPDistX=fGPDistX;
	  m_fGPDistY=fGPDistY;
	  m_oElevationData.resize(oTData.sizeY(), oTData.sizeX());
	  UInt32 k=0;
	  for (UInt32 i=0; i<oTData.sizeY(); i++)
		for (UInt32 j=0; j<oTData.sizeX(); j++)
		  m_oElevationData[i][j] = pRawData[k++]*fHeightScale;
	  m_Box[2][0] = m_oElevationData.min();
	  m_Box[2][1] = m_oElevationData.max();
	  }
	}

  //
  // resize elevation data to next usefull size s that 
  // satisfies s = 2^n+1
  //
  unsigned size_tmp=max(m_oElevationData.width(), m_oElevationData.height());
  UInt32 n_tmp;
  size_tmp = svt_next_pow2(size_tmp-1, n_tmp)+1;
  m_n = Int32(n_tmp);
  m_iSizeX=m_iSizeY=size_tmp;
  if ( (size_tmp != m_oElevationData.width()) || size_tmp != m_oElevationData.height())
	{
	  cout << "Resized elevation matrix from " << m_oElevationData.height()
		   << "x" << m_oElevationData.width() << " to " 
		   << size_tmp << "x" << size_tmp << endl;

	if (resizeMode()==Center)
	  m_oElevationData.resize(size_tmp, size_tmp, svt_matrix<Real32>::SaveCenterClamp);
	else
	  m_oElevationData.resize(size_tmp, size_tmp, svt_matrix<Real32>::Interpolate);
	}

  svt_Quad::setSize(m_n, m_oElevationData.width());

  //
  // scale to virtual height of 2m 
  // for better visual appearance in VR-Systems
  //
  m_oElevationData -= m_Box[2][0];
  m_Box[2][1] -= m_Box[2][0];

  m_oElevationData /= ((m_Box[2][1])/2);
  m_fGPDistX /= ((m_Box[2][1])/2);
  m_fGPDistY /= ((m_Box[2][1])/2);
  m_Box[2][0] = 0.0f;
  m_Box[2][1] = 2.0f;

  //
  // calculate rest of bounding box
  //
  m_Box[0][0]=0.0f; m_Box[0][1] = m_fGPDistX*(m_oElevationData.width()-1);  
  m_Box[1][0]=0.0f; m_Box[1][1] = m_fGPDistY*(m_oElevationData.height()-1);  


}



//
// setEpsilon(Real32 fEpsilon) 
//=========================================
// read in .qt-file (precomputed quadtree data)
// do per-quad delta->gamma comutation
//
void svt_HeightField::setEpsilon(Real32 fEpsilon)
{
  static Real32 s_fCurrAlpha=-1.0;

  m_fEpsilon = fEpsilon;
  bool bForceReload=false;

  //
  // do nothing if epsilon<0
  //
  if (fEpsilon<0)
	return;


  //
  // precompute, if file does not exist
  //
  if (!svt_file_exisits(m_strQTFile))
	{
	precompute();
	bForceReload=true;
	}
  
  //
  // do nothing if current alpha and epsilon are the same than 
  // the ones with which the current quadtree settings were calculated
  //
  Real32 fAlpha=svt_getCanvasAlphaY();
  if (fAlpha<0)
	return;


  if (   !isPositive(fabs(fEpsilon-m_fCalcEpsilon)) 
	  && !isPositive(fabs(fAlpha-s_fCurrAlpha))        )
	return;


  if (m_oQuadTree.size() == 0 || bForceReload)
	{
	// 
	// read in precomputed data when called the first time
	//
	m_fCalcEpsilon = fEpsilon;
	s_fCurrAlpha = fAlpha;

	//
	// local variables
	//
	Int32         iSphereMode;
	Int32         iObjErrMode;
	

	//
	// read quadtree file
	//
	svt_streamableContainers::append(false);
	svt_ibfstream f(m_strQTFile);
	f >> iSphereMode >> iObjErrMode >> m_oQuadTree; 
	m_eObjErrMode = (ObjErrMode) iObjErrMode;
	


	//
	// check if sphere mode of file matches comiled executable version
	//
	if ( iSphereMode != (Int32) sphereMode() )
	  throw svt_exception("specified quadtree file has different shere mode than expected!");


	//
	// check size
	//
	if (m_oQuadTree.size() != ( ((1u<<(2*m_n))-1) / 3 ))
	  {
	  cout << "QT size is " << m_oQuadTree.size() << " but expected " 
		   <<  ((1u<<(2*m_n))-1) / 3 << " !!!" << endl;
	  throw svt_exception("specified quadtree file has different size than expected!");	
	  }

	//
	// delta -> gamma conversion for all quads
	//
	for (UInt32 i=0; i<m_oQuadTree.size();i++)
	  {
		Real32 tmp =  (fAlpha*m_oQuadTree[i].gamma) / fEpsilon + m_oQuadTree[i].r;
		m_oQuadTree[i].gamma = tmp*tmp;
	  }
	}
  else
	{
	// 
	// re-compute delta by inverse delta->gamma function
	// then compute new gamma
	//
	for (UInt32 i=0; i<m_oQuadTree.size();i++)
	  {
		Real32 delta = (sqrt(m_oQuadTree[i].gamma) - m_oQuadTree[i].r) * m_fCalcEpsilon / s_fCurrAlpha;
		Real32 tmp =  (fAlpha*delta) / fEpsilon + m_oQuadTree[i].r;
		m_oQuadTree[i].gamma = tmp*tmp;
	  }

	m_fCalcEpsilon = fEpsilon;
	s_fCurrAlpha = fAlpha;
	
	}


}




//
// svt_HeightField::precompute(const char* pszQTFile, ObjErrMode errMode)
//=======================================================================
// Do all per-quad precomputations dependant on errMode (real or z)
// and sphere mode (small or normal, destinguished at comile time)
// Per-Quad values are :
//   gamma  quad gamma value, see my diploma thesis paper
//   r      radius of bounding sphere
//   z      z-coordinate of bounding sphere's midpoint (only in sphere-small mode)
// 
// save precomputed quadtree data to given filename
//

void svt_HeightField::precompute()
{

  //
  // resize quadtree
  //
  m_oQuadTree.resize( ((1<<(2*m_n))-1) / 3 );

  cout << "Precomputing the quadtree with n=" << m_n << " -> " 
	   << m_oQuadTree.size() << " Quads" << endl;

  //
  // kick off recursive per-quad precalculation
  //

  precompute(svt_Quad::root(), objErrMode() ); 

  //
  // save current quadtree to disk
  //


  svt_obfstream f(m_strQTFile);
  f << (Int32) sphereMode() << (Int32) objErrMode() << m_oQuadTree; 

  

  cout << "Wrote precomputed Quadtree file " << m_strQTFile << endl;


}

//
// precompute(const svt_Quad& Q)
//==============================================
// recursive part of upper percompute() method
//

void svt_HeightField::precompute(const svt_Quad& Q, ObjErrMode eErrMode)
{
  if (Q.valid())
	{ 
	precomputeSphere(Q);
	precomputeObjErr(Q, eErrMode);
	
	for(int i=0;i<4;i++)
	  precompute(Q.child(i), eErrMode);
	}
}
	






//
// precomputeObjErr(const svt_Quad& Q, ObjErrMode eErrMode)
//==============================================
// Object Error for given Quad and Mode
//						
void svt_HeightField::precomputeObjErr(const svt_Quad& Q, ObjErrMode eErrMode)
{
  if (Q.valid()) 
	{
    
    int i,j,k,l, bound;
    Real32 fError  = 0;
    svt_vector4<Real32> c = centerPoint(Q);

	//
    // error and radius of upper triangle
	//

    svt_TrianglePlane oPlane( topLeftPoint(Q), c, topRightPoint(Q));
    for (j=Q.jLeft(), k=0; j<=Q.jRight(); j++,k++ )
	  {
	  bound = (j<=Q.j()) ? k : Q.width()-k;
      for (i=Q.iTop(), l=0; l<=bound; i++,l++)
		{
		if (eErrMode==Z)
		  fError=max(fError, oPlane.pointZDist(point(i,j)));
		else
		  fError=max(fError, oPlane.pointDist(point(i,j)));
		}
	  }
	
	//
    // error and radius of lower triangle
	//
    oPlane.init( bottomLeftPoint(Q), bottomRightPoint(Q), c );
    for (j=Q.jLeft(), k=0; j<=Q.jRight(); j++,k++ )
	  {
	  bound = (j<=Q.j()) ? k : Q.width()-k;
      for (i=Q.iBottom(), l=0; l<=bound; i--,l++)
		{
		if (eErrMode==Z)
		  fError=max(fError, oPlane.pointZDist(point(i,j)));
		else
		  fError=max(fError, oPlane.pointDist(point(i,j)));
		}
	  }

	//
    // error and radius of left triangle
	//
    oPlane.init( topLeftPoint(Q), bottomLeftPoint(Q), c );
    for ( i=Q.iTop(), k=0; i<=Q.iBottom(); i++,k++ )
	  {
	  bound = (i<=Q.i()) ? k : Q.width()-k;
      for (j=Q.jLeft(), l=0; l<=bound; j++,l++)
		{
		if (eErrMode==Z)
		  fError=max(fError, oPlane.pointZDist(point(i,j)));
		else
		  fError=max(fError, oPlane.pointDist(point(i,j)));
		}
	  }

	//
    // error and radius of right triangle
	//
    oPlane.init( topRightPoint(Q), c, bottomRightPoint(Q) );
    for (i=Q.iTop(), k=0; i<=Q.iBottom(); i++,k++ )
	  {
	  bound = (i<=Q.i()) ? k : Q.width()-k;
      for (j=Q.jRight(), l=0; l<=bound; j--,l++)
		{
		if (eErrMode==Z)
		  fError=max(fError, oPlane.pointZDist(point(i,j)));
		else
		  fError=max(fError, oPlane.pointDist(point(i,j)));
		}
	  }
	m_oQuadTree[Q.index()].gamma=fError;
	}

}


//
// drawAll()
//=============== 
// Render heightfield unsimplified
//

void svt_HeightField::drawAll() 
{

  //
  // create a strip drawer object
  //
  svt_triStrip oStrip;

  //
  // get the current instance of the context data
  //
  HF_CanvasData& oCData = m_oContextData;
  oCData.iNoTriangles = 0;
  
  //
  // draw row-wise: one triangle strip per row
  //
  for (int i=0; i<sizeY()-1;i+=m_iDownsample)
	{
	  oStrip.begin(getProperties()->getLighting());
	  for (int j=0; j<sizeX();j+=m_iDownsample)
		{
		  oStrip.point(m_fGPDistX*j, m_fGPDistY*(sizeY()-i-1), m_oElevationData[i][j]);
		  oStrip.point(m_fGPDistX*j, m_fGPDistY*(sizeY()-i-1-m_iDownsample), m_oElevationData[i+m_iDownsample][j]);
		}
	  oCData.iNoTriangles+=oStrip.end();
	}
	  
  oCData.iNoQuads=2*oCData.iNoTriangles;

}

 
//
// refineQT()
//============================
// performs iterative quad selection proccess
//
  
void svt_HeightField::refineQT()
{


  //
  // get the current instance of the context data
  //
  HF_CanvasData& oCData = m_oContextData;

  //
  // reset all former subdevision info
  //
  oCData.oSubdevInfo.advance();
  oCData.oSelectedQuads.clear();
  oCData.oBTStack.clear();
  oCData.oBTStack.push(svt_Quad::root(), 
					   svt_Camera::initialCullInfo());

  //
  // update viewing parameters
  //

  //  oCData.oCam.update();
  //  oCData.oQuadPerm.set(oCData.oCam.viewDir());
  oCData.oQuadPerm.set(m_oCam.viewDir());//@@@


  //
  // iterative quad selction proccess
  //
  svt_QuadExt oCurr = oCData.oBTStack.pop();
  while (oCurr.Q.valid())
	{
	  //
	  // extract parameters of current quad's bounding sphere parameters
	  //
	  svt_Quad& Q=oCurr.Q;


	  svt_vector4<Real32> vSphere;
	  vSphere[0]=m_fGPDistX*Q.j();
	  vSphere[1]=m_fGPDistY*(m_iSizeY-Q.i()-1);
#ifdef SVT_HF_USE_SMALL_SPHERES
	  vSphere[2]=m_oQuadTree[Q.index()].z;
#else
	  vSphere[2]=m_oElevationData[Q.i()][Q.j()];
#endif
	  vSphere[3]=m_oQuadTree[Q.index()].r;

	  //
	  // check if the quad is inside the current viewing volume
	  //
	  //	  if (!m_bCulling ||   (oCurr.cullInfo == FULLY_INSIDE) ||  oCData.oCam.sphereInViewingVolume(vSphere, oCurr.cullInfo))
	  if (!m_bCulling ||   (oCurr.cullInfo == FULLY_INSIDE) ||  m_oCam.sphereInViewingVolume(vSphere, oCurr.cullInfo)) //@@@
		{

		  //
		  // if we are at a leaf in the hirarchy 
		  // -> no more subdivision neccessary, 
		  //    simply mark quad as subdevided to indicate that a full 
		  //    fan has to be drawn,
		  //    add quad to render list
		  //
		  if ( Q.level() >= m_n-1 )
			{	 
			  oCData.oSubdevInfo.setSubdevided(Q.index());
			  oCData.oSelectedQuads.push(Q);
			}
		  
		  //
		  // if selected quad-buffer is almost full
		  // or approsimation error is good enough
		  // -> add quad to render list
		  //

		  //		  else if (  oCData.oSelectedQuads.almostFull() || 
		  //					 (vSphere.distanceSq(oCData.oCam.viewPos()) >= m_oQuadTree[Q.index()].gamma) )
		  else if (  oCData.oSelectedQuads.almostFull() || 
					 (vSphere.distanceSq(m_oCam.viewPos()) >= m_oQuadTree[Q.index()].gamma) ) //@@@
			oCData.oSelectedQuads.push(Q);

		  // 
		  // otherwise the quad needs to be further subdevided
		  // -> mark quad as subdevided 
		  //    enqueue its (permutated) childs to backtrace queue
		  //
		  else
			{
			  oCData.oSubdevInfo.setSubdevided(Q.index());
			  for (int i=3;i>=0;i--)
				oCData.oBTStack.push(Q.child(oCData.oQuadPerm[i]),oCurr.cullInfo);
			}
		}

	  oCurr = oCData.oBTStack.pop();
	}

}

//
// svt_HeightField::renderQT
//==================================
// render the list of selected quads
//
void svt_HeightField::renderQT(void) 
{


  //
  // get the current instance of the context data
  //
  HF_CanvasData& oCData = m_oContextData;
  oCData.iNoTriangles = 0;

  //
  // create a triangel fan drawer
  //
  svt_triFan oFan(getProperties()->getLighting() && !getProperties()->getWireframe());


  
  //
  // render all quads as single triangle fans
  //
  unsigned iSize=oCData.oSelectedQuads.size();


  for (UInt32 i=0; i<iSize; i++)
	{
	  oFan.begin();
	  drawFan(oFan, oCData, oCData.oSelectedQuads[i]); 
	  oCData.iNoTriangles += oFan.end();
	}
  oCData.iNoQuads = iSize;
}




//
// svt_HeightField::drawStat
//==================================
// display rendering statistics:
//   #triangles
//   #quads
//   ratio triangles/quad
//   fps
//
void svt_HeightField::drawStat() 
{
  
  HF_CanvasData& oCData = m_oContextData;

  char   string[0x100];
  Int32  y = 4;

  svt_startTextBMP();

  if (simplify())
	{
	  int iOrigSize=sizeX()*sizeY()*2;
	  sprintf(string, "Orig. Data Size: %d - Compression: %.2f%%", 
			  iOrigSize, 100-float(oCData.iNoTriangles*100)/float(iOrigSize) );
	  svt_string_renderBMP(string, 4, y);
	  y += 16;


	}


  // Quads
  if (oCData.oSelectedQuads.unlimited())
	{
	sprintf(string, "Triangles: %d", oCData.iNoTriangles);
    svt_string_renderBMP(string, 4, y);
    y += 16;
	sprintf(string, "Quads: %d", oCData.iNoQuads);
    svt_string_renderBMP(string, 4, y);
    y += 16;
	}
  else
	{
	sprintf(string, "Triangles: %d / %d", 
			oCData.iNoTriangles, m_iTriangleLimit);
    svt_string_renderBMP(string, 4, y);
    y += 16;
	sprintf(string, "Quads: %d / %d", 
			oCData.iNoQuads, oCData.oSelectedQuads.limit());
    svt_string_renderBMP(string, 4, y);
    y += 16;
	}

  if (isPositive(m_fEpsilon))
	{
	sprintf(string, "Epsilon: %.1fmm", m_fEpsilon);
    svt_string_renderBMP(string, 4, y);
    y += 16;
	}
  else
	{
	if (m_iDownsample==1)
	  sprintf(string, "(not simplified)");
	else
	  sprintf(string, "downsampled x %d", m_iDownsample);
    svt_string_renderBMP(string, 4, y);
    y += 16;
	}

    
  sprintf(string, "Total time for frame: %d ms", svt_getTimePerFrame());
  svt_string_renderBMP(string, 4, y);
  y += 16;

  
  sprintf(string, "%d FPS", svt_getFPS());
  svt_string_renderBMP(string, 4, y);
  y += 16;

  svt_stopTextBMP();
}


void svt_HeightField::triangleLimit(int iMaxT)
{
  forAllCanvas(m_oContextData, setQuadLimit(iMaxT/6)); 
  m_iTriangleLimit=iMaxT;
}

void svt_HeightField::toggleQuadPermFreeze()
{
  forAllCanvas(m_oContextData, toggleQuadPermFreeze());
}


//
// precomputeSphere(const svt_Quad& Q)
//==============================================
// precompute bounding sphere parameters for small spheres
//						
#ifdef SVT_HF_USE_SMALL_SPHERES

#include <algorithm>

//class HF_QPoint2f : public svt_streamableObject { // @@@
class HF_QPoint2f {
public:
  Real32 x;
  Real32 z;

  HF_QPoint2f(Real32 x_=-2.0, Real32 z_=0.0) : x(x_), z(z_) {}
  bool valid () const {return (x>-1.0);}
  void setInvalid() { x=-2.0;}

//  protected:
//    virtual void streamOutA(ostream& s) const { s << "(" << x << "," << z << ")";}
//    virtual void streamInA(istream& s)  { s >> x >> z;}
//    virtual void streamOutB(svt_obfstream& s) const { s << x << z; }
//    virtual void streamInB(svt_ibfstream& s) { s >> x >> z; }

};

inline bool operator<(const HF_QPoint2f& A, const HF_QPoint2f& B)
{ 
  if (isPositive(fabs(A.x-B.x)))
	return (A.x-B.x)>0;
  return (A.z-B.z)>0;
}

inline bool operator!=(const HF_QPoint2f& A, const HF_QPoint2f& B)
{ 
  return ( isPositive(fabs(A.x-B.x)) || isPositive(fabs(A.z-B.z)) );
}

inline bool operator==(const HF_QPoint2f& A, const HF_QPoint2f& B)
{ 
  return !operator!=(A,B);
}





inline static bool allPointsInSphere(Real32 z, Real32 r_sq, const vector<HF_QPoint2f>& vec) 
{
  
  for (UInt32 i=0;i<vec.size();i++)
	if ( isPositive( vec[i].x * vec[i].x + (vec[i].z-z)*(vec[i].z-z) - r_sq  ))
	  return false;
  
  return true;
}
	
inline static void calcSphere(HF_QPoint2f& P1, HF_QPoint2f& P2, Real32& r_sq, Real32& z)
{
  HF_QPoint2f Pp((P1.x+P2.x)/2, (P1.z+P2.z)/2);
  HF_QPoint2f v((P1.z-P2.z)/2, (P2.x-P1.x)/2);

  Real32 alpha=-Pp.x/v.x;

  z=Pp.z+alpha*v.z;
  r_sq = P1.x*P1.x + (P1.z-z)*(P1.z-z);
}


void svt_HeightField::precomputeSphere(const svt_Quad& Q)
{
  if (Q.valid()) 
	{
    svt_vector4<Real32> c = centerPoint(Q);
    svt_vector4<Real32> p;

	list<HF_QPoint2f> lUpper;
	list<HF_QPoint2f> lLower;

	//
	// collect all points, transform them to 2D rotated space
	// and store them in a vector of HF_QPoint2f
	//
	int k=0;
	UInt32 i,j;


	vector <HF_QPoint2f> vPoints;
	vPoints.reserve((Q.width()+1)*(Q.width()+1));
	vPoints.resize((Q.width()+1)*(Q.width()+1));

	for (i=UInt32(Q.iTop()); i<=UInt32(Q.iBottom()); i++)
	  for (j=UInt32(Q.jLeft()); j<=UInt32(Q.jRight()); j++ )
		{
		  p=point(i,j);
		  p[0] -= c[0];
		  p[1] -= c[1];
		  vPoints[k++]= HF_QPoint2f( sqrt(p.x()*p.x() + p.y()*p.y()), p.z());
		}

	sort(vPoints.begin(), vPoints.end());
	//
	// build lists pf upper/lower points
	//



	k=0;
	while (k < int(vPoints.size()))
	  {
		Real32 fPos=vPoints[k].x;
		int iMin = k;
		int iMax = k;
		k++;
		while ( k < int(vPoints.size()) && !isPositive(fabs(vPoints[k].x-fPos )) )
		  {
		  if (vPoints[k].z<vPoints[iMin].z) iMin=k; 
		  if (vPoints[k].z>vPoints[iMax].z) iMax=k; 
		  k++;
		  }
		lUpper.push_back(vPoints[iMax]);
		lLower.push_back(vPoints[iMin]);
	  }
	

	//
	// build convex hull
	//===================

	// upper list: only keep list of increasing points
	list <HF_QPoint2f>::iterator il = lUpper.begin();
	Real32 fMax= il->z;
	il++;
	list<HF_QPoint2f>::iterator il2=il;
	il2++;

	while (il!=lUpper.end())
	  {
		if (isPositive(il->z - fMax))
		  fMax=il->z;
		else
		  lUpper.erase(il);
		il=il2++;
	  }

	// lower list: only keep list of decreasing points
	il = lLower.begin();
	Real32 fMin= il->z;
	il++;
	il2=il;
	il2++;

	while (il!=lLower.end())
	  {
		if (isPositive(fMin - il->z))
		  fMin=il->z;
		else
		  lLower.erase(il);
		il=il2++;
	  }


	// check remaining lists for convexity


	// UPPER LIST
	// set starting point -> ilStart, iStart
	// this list pointer will be moved forward till size()-3
	list <HF_QPoint2f>::iterator ilStart = lUpper.begin();
	int iStart=0;
	while (iStart < int(lUpper.size())-2)
	  {
	  // set ending point -> ilEnd, iEnd
	  // this list pointer will be moved backward till iStart+2
	  list <HF_QPoint2f>::iterator ilEnd = lUpper.end();
	  ilEnd--;
	  int iEnd=lUpper.size()-1;
	  while (iEnd>iStart+1)
		{
		  if ( ilStart->valid() && ilEnd->valid() )
			{
			  //
			  // now check convexity for all points between ilStart and ilEnd
			  //
			  il=ilStart;
			  il++;
			  while( il != ilEnd )
				{
				  if (il->valid())
					{
					  Real32 ref_z=ilStart->z + (ilEnd->z - ilStart->z) * (il->x - ilStart->x) / (ilEnd->x - ilStart->x);
					  if (!isPositive(il->z-ref_z)) il->setInvalid();
					}
				  il++;
				}
			}
		  iEnd--;
		  ilEnd--;
		}

	  iStart++;
	  ilStart++;
	  }

	// LOWER LIST
	// set starting point -> ilStart, iStart
	// this list pointer will be moved forward till size()-3
	ilStart = lLower.begin();
	iStart=0;
	while (iStart < int(lLower.size())-2)
	  {
	  // set ending point -> ilEnd, iEnd
	  // this list pointer will be moved backward till iStart+2
	  list <HF_QPoint2f>::iterator ilEnd = lLower.end();
	  ilEnd--;
	  int iEnd=lLower.size()-1;
	  while (iEnd>iStart+1)
		{
		  if ( ilStart->valid() && ilEnd->valid() )
			{
			  //
			  // now check convexity for all points between ilStart and ilEnd
			  //
			  il=ilStart;
			  il++;
			  while( il != ilEnd )
				{
				  if (il->valid())
					{
					  Real32 ref_z=ilStart->z + (ilEnd->z - ilStart->z) * (il->x - ilStart->x) / (ilEnd->x - ilStart->x);
					  if (!(isPositive(ref_z-il->z))) il->setInvalid();
					}
				  il++;
				}
			}
		  iEnd--;
		  ilEnd--;
		}

	  iStart++;
	  ilStart++;
	  }


	//
	// copy all valid list elements of both lists back to the vector
	//
	k=0;
	il=lUpper.begin();
	il2=lLower.begin();


	while (il!=lUpper.end() || il2 != lLower.end())
	  {
		if (il!=lUpper.end())
		  {
		  if (il->valid())
			vPoints[k++]=*il;		  
		  il++;
		  }
		if (il2!=lLower.end())
		  {
		  if (il2->valid())
			vPoints[k++]=*il2;		  
		  il2++;
		  }
	  }

	vPoints.resize(k);
	vPoints.reserve(k);
	//
	// check if we have a dominant point
	//
	Real32 lLimit_sq=0.0;
	Real32 uLimit_sq=-1.0;

	iStart=0;
	if ( vPoints[0] == vPoints[1])
	  {
		if (allPointsInSphere(vPoints[0].z, vPoints[0].x * vPoints[0].x, vPoints))
		  {
			m_oQuadTree[Q.index()].r=vPoints[0].x;
			m_oQuadTree[Q.index()].z=vPoints[0].z;
			return;
		  }
		lLimit_sq=vPoints[0].x*vPoints[0].x;
		iStart=1;
	  }



	// check sphere of every point combination
	Real32 r_sq,z;

	for (i=iStart; i<vPoints.size()-1;i++)
	  for (j=i+1; j<vPoints.size();j++)
		{
		  calcSphere(vPoints[i], vPoints[j], r_sq, z);
		  if ( (r_sq<uLimit_sq || uLimit_sq<0.0) && r_sq>lLimit_sq &&
			   allPointsInSphere(z, r_sq, vPoints)) 
			  {
				uLimit_sq=r_sq;
				m_oQuadTree[Q.index()].z=z;
			  }
		}
	

	if (isPositive(fabs(uLimit_sq)))
	  m_oQuadTree[Q.index()].r=sqrt(uLimit_sq);
	else
	  cout << "ATTENTION! NO SPHERE FOUND!!!" << endl;

	}
}
	 

#else

//
// precomputeSphere(const svt_Quad& Q)
//==============================================
// precompute bounding sphere parameters for normal spheres
//						

void svt_HeightField::precomputeSphere(const svt_Quad& Q)
{
  if (Q.valid()) 
	{
    Real32 fRadiusSq = 0;
    svt_vector4<Real32> c = centerPoint(Q);
	
    for (int i=Q.iTop(); i<=Q.iBottom(); i++)
	  for (int j=Q.jLeft(); j<=Q.jRight(); j++ )
		fRadiusSq=max(fRadiusSq, (c-point(i,j)).lengthSq());		

	m_oQuadTree[Q.index()].r = sqrt(fRadiusSq);
	}
}


#endif

void svt_HeightField::run_benchmark()
{

  cout << "running benchmark..." << endl;

  char filename[256];

  sprintf(filename, "%s/e_r_c.dat", m_pDataDir);
  svt_ofstream s_1(filename);
  cout << "creating file " << filename << endl;


  sprintf(filename, "%s/e_r_u.dat", m_pDataDir);
  svt_ofstream s_2(filename);
  cout << "creating file " << filename << endl;

  sprintf(filename, "%s/e_t1_c.dat", m_pDataDir);
  svt_ofstream s_3(filename);
  cout << "creating file " << filename << endl;

  sprintf(filename, "%s/e_t1_u.dat", m_pDataDir);
  svt_ofstream s_4(filename);
  cout << "creating file " << filename << endl;

  sprintf(filename, "%s/e_t2_c.dat", m_pDataDir);
  svt_ofstream s_5(filename);
  cout << "creating file " << filename << endl;

  sprintf(filename, "%s/e_t2_u.dat", m_pDataDir);
  svt_ofstream s_6(filename);
  cout << "creating file " << filename << endl;

  sprintf(filename, "%s/e_t_rel_c.dat", m_pDataDir);
  svt_ofstream s_7(filename);
  cout << "creating file " << filename << endl;

  sprintf(filename, "%s/e_t_rel_u.dat", m_pDataDir);
  svt_ofstream s_8(filename);
  cout << "creating file " << filename << endl;


  int iOrigSize=sizeX()*sizeY()*2;

  HF_CanvasData& oCData = m_oContextData;


  for (Real32 fEps=0.1; fEps < 2.02; fEps+=0.05)
	{
	  int iBegin;

	  int iPositions=0;
	  int iFrames = 0;
	  
	  int iT1_C = 0;
	  int iT1_U = 0;
	  
	  int iT2_C = 0;
	  int iT2_U = 0;

	  Real32 fCompRateC = 0.0;
	  Real32 fCompRateU = 0.0;

	  setEpsilon(fEps);

	  // measure all defined viewports
	  for (int i=1;i<9;i++)
		{
		  sprintf(filename, "%s/viewpoint%d", m_pDataDir , i);
		  if (svt_file_exisits(filename) )
			{
			  int j;
			  cout << "start measuring for viewpoint " << i << endl;
			  iPositions++;
			  readTransformation(filename);
			  glPopMatrix();
			  glPushMatrix();
			  applyGL();
			  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			  
			  // measure refinement time (culled)
			  culling(true);
			  iBegin=svt_getElapsedTime();
			  for (j=0;j<100;j++)
				refineQT();
			  iT1_C += (svt_getElapsedTime()-iBegin);


			  // measure rendering time (culled)
			  iBegin=svt_getElapsedTime();
			  for (j=0;j<100;j++)
				renderQT();
			  iT2_C += (svt_getElapsedTime()-iBegin);
			  fCompRateC += oCData.iNoTriangles / float(iOrigSize);

			  // measure refinement time (unculled)
			  culling(false);
			  int iBegin=svt_getElapsedTime();
			  for (j=0;j<100;j++)
				refineQT();
			  iT1_U += (svt_getElapsedTime()-iBegin);
			  
			  // measure rendering time (unculled)
			  iBegin=svt_getElapsedTime();
			  for (j=0;j<100;j++)
				renderQT();
			  iT2_U += (svt_getElapsedTime()-iBegin);
			  fCompRateU += oCData.iNoTriangles / float(iOrigSize);
			  iFrames+=100;
			  svt_swapCurrentWindow();
			}
		  
		}
	  cout << "measured " << iPositions << " positions:" << endl
		   << "  av. refinement time   (culled)   : " << float(iT1_C)/iFrames << endl
		   << "  av. rendering  time   (culled)   : " << float(iT2_C)/iFrames << endl
		   << "  av. refinement time   (unculled) : " << float(iT1_U)/iFrames << endl
		   << "  av. rendering  time   (unculled) : " << float(iT2_U)/iFrames << endl
		   << "  av. compresssion rate (culled)   : " << 100* (1-fCompRateC/iPositions) << endl
		   << "  av. compresssion rate (unculled) : " << 100* (1-fCompRateU/iPositions) << endl << endl;

	  s_1 << fEps << "  " << 100* (1-fCompRateC/iPositions) << endl;
	  s_2 << fEps << "  " << 100* (1-fCompRateU/iPositions) << endl;
	  s_3 << fEps << "  " << float(iT1_C)/iFrames << endl;
	  s_4 << fEps << "  " << float(iT1_U)/iFrames << endl;
	  s_5 << fEps << "  " << float(iT2_C)/iFrames << endl;
	  s_6 << fEps << "  " << float(iT2_U)/iFrames << endl;
	  s_7 << fEps << "  " << float(100*iT1_C)/(iT1_C+iT2_C) << endl;
	  s_8 << fEps << "  " << float(100*iT1_U)/(iT1_U+iT2_U) << endl;


	}

}

Real32 svt_HeightField::height(Real32 x, Real32 y)
{
  int ix= int( x / m_fGPDistX +0.5);
  int iy= sizeY()-1-int( y / m_fGPDistY +0.5);

  return m_oElevationData[iy][ix];

}
