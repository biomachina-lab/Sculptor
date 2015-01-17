


#include <svt_sky.h>



svt_sky::svt_sky(Real32 fRadiusX, Real32 fRadiusY, Real32 fHeight, bool bGround) 
  : svt_node(), m_bGround(bGround), m_fRadiusX(fRadiusX), m_fRadiusY(fRadiusY), m_fHeight(fHeight), 
	m_oTexture("../data/bmp/sky.bmp"), m_oPrivateProp(),
	m_oTriFan(), m_oTriStrip()
{
  setTexture(&m_oTexture);
  setProperties(&m_oPrivateProp);
  m_pProp->setLighting(false);
}


svt_sky::svt_sky(svt_node* pParentNode, Real32 fRadiusX, Real32 fRadiusY, Real32 fHeight, bool bGround)
  : svt_node(), m_bGround(bGround), m_fRadiusX(fRadiusX), m_fRadiusY(fRadiusY), m_fHeight(fHeight),
	m_oTexture("../data/bmp/sky.bmp"), m_oPrivateProp(),
	m_oTriFan(), m_oTriStrip()
{

  if (pParentNode)
	pParentNode->add(this);

  setTexture(&m_oTexture);
  setProperties(&m_oPrivateProp);
  m_pProp->setLighting(false);

}

void svt_sky::drawGL()
{
  Real32 fTheta;

  m_oTriFan->begin(m_pProp->getLighting());
  glTexCoord2f(0.5,0.5);
  m_oTriFan->point(0.0, m_fHeight, 0.0);

  for (fTheta=0.0; fTheta<2.1*fPI; fTheta+= fPI/8)
	{
	  Real32 c=cos(fTheta);
	  Real32 s=sin(fTheta);

	  glTexCoord2f(0.5 + 0.3*c, 0.5+0.3*s);
	  m_oTriFan->point(c*m_fRadiusX*0.7071, m_fHeight*0.7071, s*m_fRadiusY*0.7071);
	}
  m_oTriFan->end();


  
  m_oTriStrip->begin(m_pProp->getLighting());
  for (fTheta=0.0; fTheta<2.1*fPI; fTheta+= fPI/8)
	{
	  Real32 c=cos(fTheta);
	  Real32 s=sin(fTheta);

	  glTexCoord2f(0.5 + 0.3*c, 0.5+0.3*s);
	  m_oTriStrip->point(c*m_fRadiusX*0.7071, m_fHeight*0.7071, s*m_fRadiusY*0.7071);

	  glTexCoord2f(0.5 + 0.49*c, 0.5+0.49*s);
	  m_oTriStrip->point(c*m_fRadiusX, 0.0, s*m_fRadiusY);
	}
  m_oTriStrip->end();

  // draw ground only if requested
  if (m_bGround)
	{
	  m_oTriFan->begin(m_pProp->getLighting());
	  //	  glTexCoord2f(0.999,0.001);
	  glTexCoord2f(0.5,0.5);
	  m_oTriFan->point(0.0, 0.0, 0.0);

	  for (fTheta=0.0; fTheta<2.1*fPI; fTheta+= fPI/8)
		{
		  Real32 c=cos(fTheta);
		  Real32 s=sin(fTheta);

		  glTexCoord2f(0.5 + 0.49*c, 0.5+0.49*s);
		  //		  glTexCoord2f(0.5 + 0.3*c, 0.5+0.3*s);
		  m_oTriFan->point(c*m_fRadiusX, 0.0, s*m_fRadiusY);
		}
	  m_oTriFan->end();
	}
}


void svt_sky::drawGL_Normals()
{
  Real32 fTheta;

  m_oTriFan->beginNormals(0.3);
  m_oTriFan->pointNormal(0.0, m_fHeight, 0.0);

  for (fTheta=0.0; fTheta<2.1*fPI; fTheta+= fPI/8)
	{
	  Real32 c=cos(fTheta);
	  Real32 s=sin(fTheta);

	  m_oTriFan->pointNormal(c*m_fRadiusX*0.7071, m_fHeight*0.7071, s*m_fRadiusY*0.7071);
	}
  m_oTriFan->endNormals();


  
  m_oTriStrip->beginNormals(0.3);
  for (fTheta=0.0; fTheta<2.1*fPI; fTheta+= fPI/8)
	{
	  Real32 c=cos(fTheta);
	  Real32 s=sin(fTheta);

	  m_oTriStrip->pointNormal(c*m_fRadiusX*0.7071, m_fHeight*0.7071, s*m_fRadiusY*0.7071);
	  m_oTriStrip->pointNormal(c*m_fRadiusX, 0.0, s*m_fRadiusY);
	}
  m_oTriStrip->endNormals();

}


void svt_sky::printName()
{
  cout << "SkyDome" << endl;
}
