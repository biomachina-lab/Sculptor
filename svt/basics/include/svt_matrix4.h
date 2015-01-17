/***************************************************************************
                          svt_matrix4.h  -  description
                          ------------------------------
    begin                : 08.08.2002
    author               : Stefan Birmanns / Frank Delonge
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_MATRIX4_H
#define SVT_MATRIX4_H

// svt includes
#include <svt_basics.h>
#include <svt_types.h>
#include <svt_cmath.h>
#include <svt_matrix.h>
#include <svt_matrix4f.h>
#include <svt_opengl.h>

///////////////////////////////////////////////////////////////////////////////
// declaration
///////////////////////////////////////////////////////////////////////////////


class DLLEXPORT svt_scaleInfo {
private:
  svt_scaleInfo() {};
  static bool sm_bEverScaled;

public:
  static bool everScaled();
  static void setEverScaled();
};

template<class T> class svt_vector4;
template<class T> class svt_matrix4;

typedef svt_matrix4<Real32> svt_mat4real32;
typedef svt_matrix4<Real64> svt_mat4real64;

// Sum / Difference / Product
template<class T> 
inline svt_matrix4<T> operator+(const svt_matrix4<T>& A, const svt_matrix4<T>& B);

template<class T> 
inline svt_matrix4<T> operator+(const svt_matrix4<T>& A, const T& value);

template<class T> 
inline svt_matrix4<T> operator+(const T& value, const svt_matrix4<T>& A);


template<class T> 
inline svt_matrix4<T> operator-(const svt_matrix4<T>& A, const svt_matrix4<T>& B);

template<class T> 
inline svt_matrix4<T> operator-(const svt_matrix4<T>& A, const T& value);

template<class T> 
inline svt_matrix4<T> operator-(const T& value, const svt_matrix4<T>& A);

template<class T> 
inline svt_matrix4<T> operator-(const svt_matrix4<T>& A);

template<class T> 
inline svt_matrix4<T> operator*(const svt_matrix4<T>& A, const T& value);

template<class T> 
inline svt_matrix4<T> operator*(const T& value, const svt_matrix4<T>& A);


template<class T> 
inline svt_matrix4<T> operator/(const svt_matrix4<T>& A, const T& value);

// matrix product
template<class T> 
inline svt_matrix4<T> operator*(const svt_matrix4<T>& A, const svt_matrix4<T>& B);

/**
 * A template 4x4 matrix
 *@author Stefan Birmanns, Frank Delonge
 */
template<class T> 
class svt_matrix4 : public svt_matrix<T>
{
private:

  T stack_data[16];

  bool m_bScaled;
  void inner_invert();

  void tmp_dump(T* rs, int*);

public:

  /**
   * Constructor
   */
  svt_matrix4() : svt_matrix<T>(4,4, stack_data), m_bScaled(false) {
	loadIdentity();
  }
  
  svt_matrix4( const svt_matrix4f& rMat ) : svt_matrix<T>(4,4,stack_data) {
	set( rMat );
  }

  svt_matrix4(const svt_matrix4<T>& that) 
	: svt_matrix<T>( that, stack_data ), m_bScaled(that.m_bScaled) {};

  svt_matrix4(const svt_matrix<T>& that) 
	: svt_matrix<T>( that, stack_data ), m_bScaled(true) {};

  
  virtual ~svt_matrix4(){ };

  /* operators */
  svt_matrix4<T>& operator=(const svt_matrix<T>& that) {
	m_bScaled=true;
 	return static_cast<svt_matrix4<T>&>(svt_matrix<T>::operator=(that));
  }
  
  svt_matrix4<T>& operator=(const T& value) {
	m_bScaled=true;
 	return static_cast<svt_matrix4<T>&>(svt_matrix<T>::operator=(value));
  }
  
  /* special assign operators 
   * note: it is assumed that matrix4 will only be instanciated with
           Real32 or Real64
  */
  svt_matrix4<T>& operator=(const svt_matrix4<Real64>& that);
  svt_matrix4<T>& operator=(const svt_matrix4<Real32>& that);
  svt_matrix4<T>& operator=(const svt_matrix4f& that);
  

  /** matrix multiplication.
   */
  svt_matrix4<T>& operator*=(const svt_matrix4<T>& B);
  

  /**
   * sets the matrix to the identity matrix
   */
  void loadIdentity() {
	(*this) = T(0);
	(*this)[0][0] = (*this)[1][1] = (*this)[2][2] = (*this)[3][3] = T(1);
	m_bScaled=false;
  }
  
  
  /**
   * sets the content of the matrix from a svt_matrix4f
   */
  void set(const svt_matrix4f& m) {
	for(int iX=0; iX < 4; iX++)
	  for(int iY=0; iY < 4; iY++)
		(*this)[iX][iY] = (T)m.getAt(iY, iX);
	m_bScaled=true;
  }

  /**
   * sets the content of the matrix from a string with 16 real
   */
  void setFromString(const char*str) {
    unsigned int i = 0;
    char tmpstr[256];
    int tmpptr = 0;
    int valcnt = 0;
    
    while(i<strlen(str) && valcnt < 16)
    {
        if (str[i] != ' ' && i != strlen(str)-1)
            tmpstr[tmpptr++] = str[i];
        else 
        {
            if (i == strlen(str)-1)
            {
                tmpstr[tmpptr++] = str[i];
            }
            
            if (tmpptr != 0)
            {
                tmpstr[tmpptr] = 0;
                (*this)[valcnt / 4][valcnt % 4] = static_cast<T>(atof(tmpstr));
                valcnt++;
            }
            tmpptr = 0;
        }
        i++;
    }
	m_bScaled=true;
  }

  /**
   * copy the content of the matrix into a svt_matrix4f
   */
  void copyToMatrix4f(svt_matrix4f& m) const
  {
	for(int iX=0; iX < 4; iX++)
	    for(int iY=0; iY < 4; iY++)
		m.setMat(iY, iX, (Real32)((*this)[iX][iY]));
  }

  /**
   * adds a translation (from right)
   * \param fX x translation
   * \param fY y translation
   * \param fZ z translation
   */
  svt_matrix4<T>& translate(T fX, T fY, T fZ) {
	(*this)[0][3] +=fX;
	(*this)[1][3] +=fY;
	(*this)[2][3] +=fZ;
	return *this;
  }

  /**
   * adds a translation 
   * \param rVec reference to svt_vector4
   */
   svt_matrix4<T>& translate(const svt_vector4<T>& rVec);

  /**
   * get the translation component
   * \return svt_vector4 object
   */
  svt_vector4<T> translation() const;

  /**
   * get the x translation
   * \return x translation
   */
  T translationX() const {
	return (*this)[0][3];
  }

  /**
   * set the x translation
   * \param fX the new x translation
   */
  svt_matrix4<T>& setTranslationX(T fX) {
	(*this)[0][3] = fX;
	return *this;
  }

  /**
   * get the y translation
   * \return y translation
   */
  T translationY() const {
	return (*this)[1][3];
  }

  /**
   * set the y translation
   * \param fY the new y translation
   */
  svt_matrix4<T>& setTranslationY(T fY) {
	(*this)[1][3] = fY;
	return *this;
  }

  /**
   * get the z translation
   * \return z translation
   */
  T translationZ() const {
	return (*this)[2][3];
  }

  /**
   * set the z translation
   * \param fZ the new z translation
   */
  svt_matrix4<T>& setTranslationZ(T fZ) {
	(*this)[2][3] = fZ;
	return *this;
  }

  /**
   * set translation
   * \param fX x component
   * \param fY y component
   * \param fZ z component
   */
   svt_matrix4<T>& setTranslation( T fX, T fY, T fZ ) {
	setTranslationX( fX );
	setTranslationY( fY );
	return setTranslationZ( fZ );
  }

  /**
   * set the translation component
   * \param rVec reference to svt_vector4 object
   */
  svt_matrix4<T>& setTranslation(const svt_vector4<T>& rVec)
  {
      return setTranslation( rVec.x(), rVec.y(), rVec.z() );
  }

  /**
   * adds a rotation around a vector
   * \param rVec reference to Vector3f
   * \param fAlpha rotation angle in rad
   */
  svt_matrix4<T>& rotate(T x, T y, T z, T fAlpha) {
	svt_matrix4<T> oTmp; // == identity after creation
	
	T c  = cos(fAlpha); 
	T c_ = 1-c;
	T s  = sin(fAlpha);
	T tmp;

	oTmp[0][0] = c + x*x*c_;
	oTmp[1][1] = c + y*y*c_;
	oTmp[2][2] = c + z*z*c_;
	
	tmp=x*y*c_; oTmp[1][0] = tmp + z*s; oTmp[0][1] = tmp - z*s;
	tmp=x*z*c_; oTmp[2][0] = tmp - y*s; oTmp[0][2] = tmp + y*s;
	tmp=z*y*c_; oTmp[2][1] = tmp + x*s; oTmp[1][2] = tmp - x*s;

	*this = oTmp*(*this);

	return *this;
  }

  /**
   * adds a rotation around a vector
   * \param rVec reference to svt_vector4<T>
   * \param fAlpha rotation angle in rad
   */
  svt_matrix4<T>& rotate(const svt_vector4<T>& rVec, T fAlpha);

  /**
   * adds a rotation around a vector
   * \param rVec reference to svt_vector4<T> 
   *             wherethe w-coordinate is taken as rotation angle (in rad)
   * 
   */
  svt_matrix4<T>& rotate(const svt_vector4<T>& rVec);

  
  // meaning of axis values: (0,1,2) = (x, y, z)
  svt_matrix4<T>&  rotate(int iAxis, T fAlpha) {
	svt_matrix4<T> oTmp;  // oTmp == identity after creation
	T c = cos(fAlpha);
	T s = sin(fAlpha);
	
	switch( iAxis )
	  {
	  case 0:
		oTmp[1][1] = oTmp[2][2] = c;
		oTmp[2][1] = s;
		oTmp[1][2] = -s;
		break;
 	  case 1:
		oTmp[0][0] = oTmp[2][2] = c;
		oTmp[0][2] = s;
		oTmp[2][0] = -s;
 		break;
 	  case 2:
		oTmp[0][0] = oTmp[1][1] = c;
		oTmp[1][0] = s;
		oTmp[0][1] = -s;
 		break;
 	  }
	*this = oTmp*(*this);
	return *this;
  }

  /**
   * add rotation around phi, theta, psi following the Goldstein convention (rotation around z,x,z)
   * \param fPhi phi angle
   * \param fTheta theta angle
   * \param fPsi psi angle
   */
  svt_matrix4<T>& rotatePTP( T fPhi, T fTheta, T fPsi )
  {
      svt_matrix4<T> oTmp;  // oTmp == identity after creation

      double fSin_psi   = sin( fPsi );
      double fCos_psi   = cos( fPsi );
      double fSin_theta = sin( fTheta );
      double fCos_theta = cos( fTheta );
      double fSin_phi   = sin( fPhi);
      double fCos_phi   = cos( fPhi );

      oTmp[0][0] = fCos_psi * fCos_phi - fCos_theta * fSin_phi * fSin_psi;
      oTmp[0][1] = fCos_psi * fSin_phi + fCos_theta * fCos_phi * fSin_psi;
      oTmp[0][2] = fSin_psi * fSin_theta;
      oTmp[1][0] = -fSin_psi * fCos_phi  - fCos_theta * fSin_phi * fCos_psi;
      oTmp[1][1] = -fSin_psi * fSin_phi  + fCos_theta * fCos_phi * fCos_psi;
      oTmp[1][2] =  fCos_psi * fSin_theta;
      oTmp[2][0] =  fSin_theta * fSin_phi;
      oTmp[2][1] = -fSin_theta * fCos_phi;
      oTmp[2][2] =  fCos_theta;

      *this = oTmp*(*this);
      return *this;
  };

  //
  // scaling
  //
 
  /**
   * scale one axis by given factor
   */
  svt_matrix4<T>& scale(int iAxis, T factor) {
	m_bScaled=true;
	svt_scaleInfo::setEverScaled();
	(*this)[0][iAxis] *= factor;
	(*this)[1][iAxis] *= factor;
	(*this)[2][iAxis] *= factor;
	return *this;
  }


  /**
   * scale all 3 axis uniformly by factor value
   */
  svt_matrix4<T>& scale(T value) {
	return scale(0,value).scale(1,value).scale(2,value);
  }

  /**
   * scale all 3 axis differently by given factors
   */
  svt_matrix4<T>& scale(T x, T y, T z) {
	return scale(0,x).scale(1,y).scale(2,z);
  }


  /**
   * scale all axis by differenttly by factors of given vector
   */
  svt_matrix4<T>& scale(const svt_vector4<T>& vec); 


  /**
   * compute the inverse matrix
   */
  svt_matrix4<T>& invert(void);

  /**
   * Jacobi transformation
   * \param rEigenvectors svt_matrix object to store the eigenvectors (columns)
   * \param rEigenvalues svt_vector object to store the eigenvalues
   */
  bool jacobi(svt_matrix4<T>& rEigenvectors, svt_vector4<T>& rEigenvalues);

  /**
   * multiplies the matrix to the opengl modelview-matrix
   */
  void applyToGL() const;

  /**
   * Load the current modelview matrix.
   * NOTE: since OpenGL stores matrixes column-first, the 
   *       loaded matrix appears transposed.
   */
  void loadModelviewMatrix();

  /**
   * Load the current projection matrix.
   * NOTE: since OpenGL stores matrixes column-first, the 
   *       loaded matrix appears transposed.
   */
  void loadProjectionMatrix();

  bool scaled() const {return m_bScaled;}

  /**
   * Transpose the matrix
   */
  void transpose();
};



///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////




#include <svt_vector4.h>


template<class T>
inline svt_matrix4<T>& svt_matrix4<T>::operator=(const svt_matrix4<Real64>& that)
{
  m_bScaled=that.scaled();
  for (int i=0; i<4; i++)
	for (int j=0; j<4; j++)
	  (*this)[i][j] = (T) that[i][j];
  return *this;
}

#ifndef WIN32
template<>
inline svt_matrix4<Real64>& svt_matrix4<Real64>::operator=(const svt_matrix4<Real64>& that)
{
  m_bScaled=that.m_bScaled;
  memcpy(m_pData, that.c_data(), 16*sizeof(Real64));
  return *this;
}
#endif

template<class T>
inline svt_matrix4<T>& svt_matrix4<T>::operator=(const svt_matrix4<Real32>& that)
{
  m_bScaled=that.scaled();
  for (int i=0; i<4; i++)
	for (int j=0; j<4; j++)
	  (*this)[i][j] = (T) that[i][j];
  return *this;
}

#ifndef WIN32
template<>
inline svt_matrix4<Real32>& svt_matrix4<Real32>::operator=(const svt_matrix4<Real32>& that)
{
  m_bScaled=that.m_bScaled;
  memcpy(m_pData, that.c_data(), 16*sizeof(Real32));
  return *this;
}
#endif

template<class T>
inline svt_matrix4<T>& svt_matrix4<T>::operator=(const svt_matrix4f& that)
{
  set(that);
  return *this;
}


/**
 * adds a rotation around a vector
 * \param rVec reference to svt_vector4<T>
 * \param fAlpha rotation angle
 */
template<class T> 
inline svt_matrix4<T>& svt_matrix4<T>::rotate(const svt_vector4<T>& rVec, T fAlpha)
{
  return rotate(rVec[0], rVec[1], rVec[2], fAlpha);
}

/**
 * adds a translation (from right)
 * \param rVec reference to svt_vector4
 */
template<class T> 
inline svt_matrix4<T>& svt_matrix4<T>::rotate(const svt_vector4<T>& rVec)
{
  return rotate(rVec[0], rVec[1], rVec[2], rVec[3]);
}

/**
 * get the translation component
 * \return svt_vector4 object
 */
template<class T> 
inline svt_vector4<T> svt_matrix4<T>::translation() const
{
  return svt_vector4<T>( translationX(), translationY(), translationZ() );
}

/**
 * set the translation component
 * \param rVec reference to svt_vector4 object
 */
template<class T> 
inline svt_matrix4<T>& svt_matrix4<T>::translate(const svt_vector4<T>& rVec)
{
  return translate(rVec[0], rVec[1], rVec[2]);
}


template<class T> 
inline svt_matrix4<T>& scale(const svt_vector4<T>& rVec) 
{
  return scale(rVec[0], rVec[1], rVec[2]);
}

/**
 * Performs the Lu decomposition for Gaussian elimination with partial
 * pivoting.
 * Changed by fd: only examine first 3 rows since the last row is always (0 0 0 1)
 * \param pivot - contains the pivot history...the original position numbers of the pivoted rows.
 */


template <class T>
void svt_matrix4<T>::inner_invert()
{

  if (!m_bScaled)
	{
	// just transpose if no scaling has been performed
	svt_swap_values((*this)[0][1],(*this)[1][0]);
	svt_swap_values((*this)[0][2],(*this)[2][0]);
	svt_swap_values((*this)[1][2],(*this)[2][1]);
	}
  else 
	{
	// invert upper submatrix with gauss-algorithm and pivot-search 
	T   rMat[9]  = {1,0,0, 0,1,0, 0,0,1};
	int pivot[3] = {0,1,2};
	int iRow, iCol, i;
	
	// forward substitution
	for (iRow=0;iRow<3;iRow++)
	  {
	  // find pivot element
	  int iPR=iRow; // pivot row
	  T tPivotValue=fabs((*this)[pivot[iRow]][iRow]);
	  for (i=iRow+1; i<3; i++)
		if (fabs((*this)[pivot[i]][iRow]) > tPivotValue)
		  {
		  iPR=i;
		  tPivotValue=fabs((*this)[pivot[i]][iRow]);
		  }
	  if (iPR != iRow)
		svt_swap_values(pivot[iRow], pivot[iPR]);
	  iPR=pivot[iRow];

	  // devide current pivot row + right side by pivot elemtent
	  tPivotValue=(*this)[iPR][iRow];
	  for (iCol=iRow; iCol<3;iCol++)
		(*this)[iPR][iCol] /= tPivotValue;
	  for (iCol=0; iCol<3;iCol++)
		 rMat[3*iPR+iCol] /= tPivotValue;

	  // rest rows: row operations to eliminate leading entry
	  for (i=iRow+1; i<3; i++)
		{
		T factor=(*this)[pivot[i]][iRow];
		for (iCol=iRow; iCol<3;iCol++)
		  (*this)[pivot[i]][iCol] -= factor*(*this)[iPR][iCol];
		for (iCol=0; iCol<3;iCol++)
		  rMat[3*pivot[i]+iCol] -= factor*rMat[3*iPR+iCol];
		}
	  }

	// backward substituation
	for (iRow=2;iRow>0;iRow--)
	  for (i=iRow-1; i>=0; i--)
		{
		  T factor=(*this)[pivot[i]][iRow];
		  (*this)[pivot[i]][iRow]=T(0);
		  for (int iCol=0;iCol<3;iCol++)
			rMat[3*pivot[i]+iCol] -= factor*rMat[3*pivot[iRow]+iCol];
		}
	
	// copy solution from right side into *this 
	for (iRow=0;iRow<3;iRow++)
	  for (iCol=0;iCol<3;iCol++)
		(*this)[iRow][iCol]=rMat[3*pivot[iRow]+iCol];

	}

}

/**
 * compute the inverse matrix
 */
template<class T>
inline svt_matrix4<T>& svt_matrix4<T>::invert()
{
  // compute and store inverse of upper 3x3 submatrix R
  inner_invert();
  
  // the new translation vector can be calculated as -R^(-1)*T 
  // where T is the existing translation vector
  T vec[3];
  int i;
  for (i=0;i<3;i++)
	{
	vec[i]=T(0);
	for (int j=0;j<3;j++)
	  vec[i]-=(*this)[i][j]*(*this)[j][3];
	}
  for (i=0;i<3;i++)
	(*this)[i][3]=vec[i];

  return *this;
}

/**
 * Jacobi transformation
 * \param rEigenvectors svt_matrix object to store the eigenvectors (columns)
 * \param rEigenvalues svt_vector object to store the eigenvalues
 */
template<class T>
bool svt_matrix4<T>::jacobi(svt_matrix4<T>& rEigenvectors, svt_vector4<T>& rEigenvalues)
{
    Real64  fSM;
    Real64  fTheta;
    Real64  fC, fS, fT;
    Real64  fTau;
    Real64  fH, fG;
    Real64  fThresh;
    Real64  aB[4];
    Real64  aZ[4];
    UInt32  iP, iQ, i, j;
    Real64  oA[4][4];
    int iRots;

    // initializations
    for (i = 0; i < 4; i++)
    {
        aB[i] = rEigenvalues[i] = (*this)[i][i];
        aZ[i] = 0.0;

        for (j = 0; j < 4; j++)
        {
            rEigenvectors[i][j] = (i == j) ? 1.0f : 0.0f;
            oA[i][j] = (*this)[i][j];
        }
    }

    iRots = 0;

    for(i = 0; i < 50; i++)
    {
        fSM = 0.0;

        for(iP = 0; iP < 4 - 1; iP++)
        {
            for(iQ = iP+1; iQ < 4; iQ++)
            {
                fSM += fabs(oA[iP][iQ]);
            }
        }

        if (fSM == 0.0)
            return false;

        fThresh = (i < 3 ? (.2 * fSM / (4 * 4)) : 0.0);

        for (iP = 0; iP < 4 - 1; iP++)
        {
            for (iQ = iP + 1; iQ < 4; iQ++)
            {
                fG = 100.0 * fabs(oA[iP][iQ]);

                if (i > 3 &&
                    (fabs(rEigenvalues[iP]) + fG == fabs(rEigenvalues[iP])) &&
                    (fabs(rEigenvalues[iQ]) + fG == fabs(rEigenvalues[iQ])))
                {
                    oA[iP][iQ] = 0.0;
                }
                else if (fabs(oA[iP][iQ]) > fThresh)
                {
                    fH = rEigenvalues[iQ] - rEigenvalues[iP];

                    if (fabs(fH) + fG == fabs(fH))
                    {
                        fT = oA[iP][iQ] / fH;
                    }
                    else
                    {
                        fTheta = .5 * fH / oA[iP][iQ];
                        fT = 1.0 / (fabs(fTheta) + sqrt(1 + fTheta * fTheta));
                        if (fTheta < 0.0)  fT = -fT;
                    }
                    // End of computing tangent of rotation angle

                    fC = 1.0 / sqrt(1.0 + fT * fT);
                    fS = fT * fC;

                    fTau = fS / (1.0 + fC);
                    fH   = fT * oA[iP][iQ];

                    aZ[iP]    -= fH;
                    aZ[iQ]    += fH;

                    rEigenvalues[iP] -= T(fH);
                    rEigenvalues[iQ] += T(fH);

                    oA[iP][iQ] = 0.0;

                    for (j = 0; j < iP; j++)
                    {
                        fG = oA[j][iP];
                        fH = oA[j][iQ];

                        oA[j][iP] = fG - fS * (fH + fG * fTau);
                        oA[j][iQ] = fH + fS * (fG - fH * fTau);
                    }

                    for (j = iP+1; j < iQ; j++)
                    {
                        fG = oA[iP][j];
                        fH = oA[j][iQ];

                        oA[iP][j] = fG - fS * (fH + fG * fTau);
                        oA[j][iQ] = fH + fS * (fG - fH * fTau);
                    }

                    for (j = iQ+1; j < 4; j++)
                    {
                        fG = oA[iP][j];
                        fH = oA[iQ][j];

                        oA[iP][j] = fG - fS * (fH + fG * fTau);
                        oA[iQ][j] = fH + fS * (fG - fH * fTau);
                    }

                    for (j = 0; j < 4; j++)
                    {
                        fG = rEigenvectors[j][iP];
                        fH = rEigenvectors[j][iQ];

                        rEigenvectors[j][iP] = T(fG - fS * (fH + fG * fTau));
                        rEigenvectors[j][iQ] = T(fH + fS * (fG - fH * fTau));
                    }
                }
                iRots++;
            }
        }
        for (iP = 0; iP < 4; iP++)
        {
            rEigenvalues[iP] = T(aB[iP] += aZ[iP]);
            aZ[iP] = 0;
        }
    }

    return true;
}

template<class T> 
inline void svt_matrix4<T>::applyToGL() const
{
  Real32 gl_mat[16];

  for(int iX=0; iX < 4; iX++)
	for(int iY=0; iY < 4; iY++)
	  gl_mat[4*iY+iX] = (Real32) ((*this)[iX][iY]);
  
  glMultMatrixf(gl_mat);
}

template<> 
inline void svt_matrix4<Real64>::applyToGL() const
{
  Real64 gl_mat[16];
  
  for(int iX=0; iX < 4; iX++)
	for(int iY=0; iY < 4; iY++)
	  gl_mat[4*iY+iX] = (Real64) ((*this)[iX][iY]);
  
  glMultMatrixd(gl_mat);
}

template<class T> 
inline void svt_matrix4<T>::loadModelviewMatrix() 
{
  Real32 gl_mat[16];

  glGetFloatv(GL_MODELVIEW_MATRIX, gl_mat);  

  for(int iX=0; iX < 4; iX++)
	for(int iY=0; iY < 4; iY++)
	  (*this)[iX][iY] = (T) gl_mat[4*iY+iX];

  m_bScaled=svt_scaleInfo::everScaled();
}

template<> 
inline void svt_matrix4<Real64>::loadModelviewMatrix()
{
  Real64 gl_mat[16];

  glGetDoublev(GL_MODELVIEW_MATRIX, gl_mat);  

  for(int iX=0; iX < 4; iX++)
	for(int iY=0; iY < 4; iY++)
	  (*this)[iX][iY] = gl_mat[4*iY+iX];

  m_bScaled=svt_scaleInfo::everScaled();
}


template<class T> void svt_matrix4<T>::loadProjectionMatrix() 
{
  Real32 gl_mat[16];

  glGetFloatv(GL_PROJECTION_MATRIX, gl_mat);  

  for(int iX=0; iX < 4; iX++)
	for(int iY=0; iY < 4; iY++)
	  (*this)[iX][iY] = (T) gl_mat[4*iY+iX];

  m_bScaled=true;
}

template<> 
inline void svt_matrix4<Real64>::loadProjectionMatrix()
{
  Real64 gl_mat[16];

  glGetDoublev(GL_PROJECTION_MATRIX, gl_mat);  

  for(int iX=0; iX < 4; iX++)
	for(int iY=0; iY < 4; iY++)
	  (*this)[iX][iY] = gl_mat[4*iY+iX];

  m_bScaled=true;
}

template<class T>
inline void svt_matrix4<T>::transpose()
{
 int iX,iY;

  T gl_mat[16];
  
  for(iX=0; iX < 4; iX++)
	for(iY=0; iY < 4; iY++)
	  gl_mat[4*iY+iX] = (Real64) ((*this)[iX][iY]);

  for(iX=0; iX < 4; iX++)
	for(iY=0; iY < 4; iY++)
	  (*this)[iY][iX] = gl_mat[4*iY+iX];
}

template <class T>
inline svt_matrix4<T>& svt_matrix4<T>::operator*=(const svt_matrix4<T>& B) 
{
  // 
  // use *(matrix4, matrix4)-operator
  //
  *this=(*this)*B;
  return *this;
}





//
// binary operators

// Sum / Difference / Product
template<class T> 
inline svt_matrix4<T> operator+(const svt_matrix4<T>& A, const svt_matrix4<T>& B)
{
  svt_matrix4<T> C(A);
  C+=B;
  return C;
}



template<class T> 
inline svt_matrix4<T> operator+(const svt_matrix4<T>& A, const T& value)
{
  svt_matrix4<T> C(A);
  C += value;
  return C;
}

template<class T> 
inline svt_matrix4<T> operator+(const T& value, const svt_matrix4<T>& A)
{
  svt_matrix4<T> C(A);
  C += value;
  return C;
}


template<class T> 
inline svt_matrix4<T> operator-(const svt_matrix4<T>& A, const svt_matrix4<T>& B)
{
  svt_matrix4<T> C(A);
  C -= B;
  return C;
}


template<class T> 
inline svt_matrix4<T> operator-(const svt_matrix4<T>& A, const T& value)
{
  svt_matrix4<T> C(A);
  C -= value;
  return C;
}


template<class T> 
inline svt_matrix4<T> operator-(const T& value, const svt_matrix4<T>& A)
{
  return -A+value;
}


template<class T> 
inline svt_matrix4<T> operator-(const svt_matrix4<T>& A)
{
  svt_matrix4<T> C;
  for (unsigned i=0; i<C.height()*C.width(); i++)
	C.c_data()[i] = -A.c_data()[i];
  return C;
}


template<class T> 
inline svt_matrix4<T> operator*(const svt_matrix4<T>& A, const T& value)
{
  svt_matrix4<T> C(A);
  C *= value;
  return C;
}


template<class T> 
inline svt_matrix4<T> operator*(const T& value, const svt_matrix4<T>& A)
{
  svt_matrix4<T> C(A);
  C *= value;
  return C;
}


template<class T> 
inline svt_matrix4<T> operator/(const svt_matrix4<T>& A, const T& value)
{
  svt_matrix4<T> C(A);
  C /= value;
  return C;
}


// matrix product
template<class T> 
inline svt_matrix4<T> operator*(const svt_matrix4<T>& A, const svt_matrix4<T>& B)
{
  svt_matrix4<T> C;
  return mat_mult(A,B,C);
}





#endif
