#include <svt_spline.h>
#include <svt_str.h>
#include <svt_fstream.h>
#include <svt_streamableVectors.h>

//#include <svt_streamableVectors.h>



svt_spline::svt_spline(const char* pszFilename)
{
  m_Mcr[0][0] = -1.0; m_Mcr[0][1] =  3.0; m_Mcr[0][2] = -3.0; m_Mcr[0][3] =  1.0;
  m_Mcr[1][0] =  2.0; m_Mcr[1][1] = -5.0; m_Mcr[1][2] =  4.0; m_Mcr[1][3] = -1.0;
  m_Mcr[2][0] = -1.0; m_Mcr[2][1] =  0.0; m_Mcr[2][2] =  1.0; m_Mcr[2][3] =  0.0;
  m_Mcr[3][0] =  0.0; m_Mcr[3][1] =  2.0; m_Mcr[3][2] =  0.0; m_Mcr[3][3] =  0.0;

  m_Mcr /= 2;
  m_McrGbs=m_Mcr;

  m_Pi.resize(4);
  m_bAtEnd=true;

  if (pszFilename)
	load(pszFilename);

}



bool svt_spline::load(const char* pszFilename)
{
  bool success=false;

  svt_ifstream s(pszFilename);

  if (s)
	{
	svt_str str;
	svt_streamableContainers::streamSize(true);
	svt_streamableContainers::append(false);

	s >> m_oPoints >> str >> m_iLoop;


	if (!s ||  strcmp(str, "goto") )
	  m_iLoop = -1;

	if ( m_oPoints.size() >= 3)
	  {
	  success=true;
	  m_bAtEnd=false;

	  //
	  // fill in missing z-coordinates
	  //
	  unsigned i=1, iLeft=0, iRight=m_oPoints.size()-1;
	  //	  int iLeft=0, iRight=m_oPoints.size()-1;
	  Real32 fDist, fDist_tot;

	  
	  while (i<m_oPoints.size()-1)
		{
		//
		// check if z-coord of current point needs to be computed
		//
		if (m_oPoints[i][2] < 0)
		  {
			//
			// search for next valid point -> iRight
			// and calculate xy-distance from left to right
			//

			iRight=i+1;
			fDist_tot = xy_dist(iLeft, i);
			fDist_tot += xy_dist(i,i+1);

			while (m_oPoints[iRight][2] < 0) 
			  {
			  iRight++;
			  fDist_tot+=xy_dist(iRight, iRight-1);
			  }

			//
			// now fill in z-values inbetween iLeft and iRight
			//
			fDist=0;
			while (i<iRight)
			  {
			  fDist+=xy_dist(i, i-1);
			  m_oPoints[i][2]=m_oPoints[iLeft][2] + fDist/fDist_tot * (m_oPoints[iRight][2]-m_oPoints[iLeft][2]);
			  i++;
			  }
		  }
		iLeft=i;
		i++;
		}
	  //
	  // fill in point-to-point distances as w-coordiante
	  //

	  for (i=0; i<m_oPoints.size()-1; i++)
		m_oPoints[i][3]=m_oPoints[i].distance(m_oPoints[i+1]);

	  if (m_iLoop >=0 )
		m_oPoints[i][3]=m_oPoints[i].distance(m_oPoints[m_iLoop]);
	  else
		m_oPoints[i][3]=-1.0;

	  initStart();
	  }
	}
  return success;
}


void svt_spline::initStart()
{

  if (m_oPoints.size() >= 3)
	{
	  m_Pi[0]=0;
	  m_Pi[1]=0;
	  m_Pi[2]=1;
	  m_Pi[3]=2;
  
	  m_iCurrIndex=0;
	  m_fCurrTotDist=0.0;

	  recalcMat();
	  m_bAtEnd=false;
	}
  else
	{
	  m_iCurrIndex=-1;
	  m_fCurrTotDist=-1.0;
	  m_Pi[0]=-1;
	  m_Pi[1]=-1;
	  m_Pi[2]=-1;
	  m_Pi[3]=-1;
	  m_bAtEnd=true;
	}


}

bool svt_spline::advance()
{
  if (m_bAtEnd)
	return false;


  m_fCurrTotDist+=m_oPoints[m_Pi[1]][3];


  m_Pi[0] = m_Pi[1];
  m_Pi[1] = m_Pi[2];
  m_Pi[2] = m_Pi[3];

  m_Pi[3]++;

  if (m_Pi[3] >= (int)m_oPoints.size())
  {
      if (m_iLoop>=0)
      {
          m_Pi[3]=m_iLoop;

      } else {

	  if (!m_bAtEnd)
          {
              m_Pi[3]=m_Pi[2];
              m_bAtEnd=true;

          } else {

              return false;
          }
      }
  }

  m_iCurrIndex=m_Pi[1];

  return true;

}

svt_vector4<Real32>& svt_spline::point(Real32 fDist, svt_vector4<Real32>& p)
{


  if (fDist<m_fCurrTotDist)
	initStart();



  bool bRecalc=false;
  while ( m_fCurrTotDist + m_oPoints[m_iCurrIndex][3] < fDist)
	{
	bRecalc=true;
	if (!advance())
	  return m_oPoints[m_oPoints.size()-1];
	}

  if (bRecalc)
	recalcMat();


  Real32 fW= (fDist-m_fCurrTotDist) / m_oPoints[m_iCurrIndex][3];

  //
  // now calculate intermediate point
  //

  p[0] = fW * (fW * (fW * m_McrGbs[0][0] + m_McrGbs[1][0]) + m_McrGbs[2][0]) + m_McrGbs[3][0];
  p[1] = fW * (fW * (fW * m_McrGbs[0][1] + m_McrGbs[1][1]) + m_McrGbs[2][1]) + m_McrGbs[3][1];
  p[2] = fW * (fW * (fW * m_McrGbs[0][2] + m_McrGbs[1][2]) + m_McrGbs[2][2]) + m_McrGbs[3][2];
  p[3] = 1.0;

  return p;

}

void svt_spline::recalcMat()
{
    
    for (int iRow=0; iRow<4; iRow++) 
	  for (int iCol=0; iCol<3; iCol++)
		{
		m_McrGbs[iRow][iCol] = 0.0;
		for (int i=0;i<4;i++)
		  m_McrGbs[iRow][iCol] += m_Mcr[iRow][i] * m_oPoints[m_Pi[i]][iCol];
		}
}



Real32 svt_spline::xy_dist(int i, int j)
{
  Real32 x = m_oPoints[i][0]-m_oPoints[j][0];
  Real32 y = m_oPoints[i][1]-m_oPoints[j][1];

  return sqrt(x*x+y*y);

}
