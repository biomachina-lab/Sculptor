/***************************************************************************
                          svt_volume
			  ----------
    begin                : Oct 23 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_VOLUME_H
#define SVT_VOLUME_H

#include <svt_basics.h>
#include <svt_types.h>
#include <svt_matrix4.h>
#include <svt_time.h>
#include <svt_vector4.h>
#include <svt_sampled.h>
#include <svt_rnd.h>
#include <svt_file_utils.h>
#include <svt_swap.h>
#include <svt_vector3.h>
#include <svt_stlStack.h>
#include <svt_stlVector.h>
#include <math.h> //For getRMS method
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif
 

/**
 * A container class for volumetric data.
 *@author Stefan Birmanns
 */
template<class T> class DLLEXPORT svt_volume : public svt_sampled< svt_vector4<Real64> >
{

public:

    enum GradientMode
    {
	CentralDistance,
	Sobel
    };

protected:

    T* m_pData;

    unsigned int m_iSizeX;
    unsigned int m_iSizeY;
    unsigned int m_iSizeZ;

    T m_fMaxDensity;
    T m_fMinDensity;
    T m_fAvgDensity;

    Real64 m_fWidth; // voxel width in angstroem

    Real64 m_fGridX; // x position of first voxel
    Real64 m_fGridY; // y position of first voxel
    Real64 m_fGridZ; // z position of first voxel

    bool m_bInternal; // if true, the data is allocated internally and will also get deleted when the object is destroyed!

    bool m_bChanged; // true signals that the content of the volume has changed and there a calcMinMaxDensity has to be called.

    Real64 m_fCutoff; // cutoff voxel value for the sampling

    Real64 m_fNorm;  // Norm of the volume = sqrt( sum( at(i) * at(i) ) );

public:

    /**
     * Constructor
     */
    svt_volume();

    /**
     * Constructor.
     * This constructor will create a volume of a certain size.
     * Attention: The memory will get delete when the object gets destroyed!
     * \param iSizeX size in x direction of the new volume
     * \param iSizeY size in y direction of the new volume
     * \param iSizeZ size in z direction of the new volume
     * \param fInit the value the voxels get initialized to (default: 0)
     */
    svt_volume(unsigned int iSizeX, unsigned int iSizeY, unsigned int iSizeZ, const T fInit = 0.0);

    /**
     * Constructor.
     * This constructor will create a volume of a certain size.
     * Attention: The memory will get delete when the object gets destroyed!
     * \param iSizeX size in x direction of the new volume
     * \param iSizeY size in y direction of the new volume
     * \param iSizeZ size in z direction of the new volume
     * \param pData pointer to memory block that gets copied into the new object.
     */
    svt_volume(unsigned int iSizeX, unsigned int iSizeY, unsigned int iSizeZ, const T* pData);

    /**
     * Copy constructor
     */
    svt_volume( svt_volume<T>& rVol );

    /**
     * Destructor
     */
    virtual ~svt_volume();

    ///////////////////////////////////////////////////////////////////////////
    // Operators
    ///////////////////////////////////////////////////////////////////////////

    /**
     * Assign a scalar value, e.g. A=1.
     * The size of A will be unchanged, value is assigned to all elements.
     */
    inline svt_volume<T>& operator=(const T& fValue);

    /**
     * Assign a volume B to a volume A, e.g. A=B.
     * Size and values will become same as B.
     * Attention: This makes a deep copy if the memory was allocated by the object B itself. If
     * the memory was just linked to B via a pointer (see setData()), then only the pointer
     * is copied but not the data itself. Please use deepCopy() if you wish to copy the data in such case.
     */
    svt_volume<T>& operator=(const svt_volume<T>& rThat);

    /**
     * Multiply with scalar.
     * \param fScalar the scalar the voxels of the volume are getting multiplied with.
     */
    svt_volume<T>& operator*(const T fScalar);

    /**
     * Add another volume.
     * \param rVol reference to the other volume
     */
    void operator+=( const svt_volume<T> & rVol);

    /**
     * Multiply with matrix - resulting volume has different dimensions that the original
     * \param rMat a transformation matrix that will translate/rotate the volume -
     */
    svt_volume<T> operator*(svt_matrix4< T > oMat);

    /**
     *  apply the transformation of the current volume (this) and project onto
     *  \param rDest the volume where to project
     *  \param the tranformation to apply
     *  ATTENTION: rDest should be already allocated
     */
    void applyTransformation(svt_volume<Real64> &rDest, svt_matrix4<Real64> oMat);

    /**
     * get dimension of the volume after appling transformation
     * \param the transformation to apply
     * \param rNewGrid the new origin
     * \return the new size
     */
    svt_vector4<unsigned int> getDimensions(svt_matrix4< T > oMat, svt_vector4<Real64>& rNewGrid);

    /**
     * update the dimensions, size and origin
     * \param rNewSize the size up to now, it will be updated if needed
     * \param rNewGrid the origin up to now, it will be updated
     * \param rDir gives the corner that will be consider
     */
    void updateDimensions(svt_matrix4< T > oMat, svt_vector4<Real64>& rNewGrid,  svt_vector4<unsigned int>& rNewSize, svt_vector4<unsigned int> oDir);


    ///////////////////////////////////////////////////////////////////////////
    // Public Methods
    ///////////////////////////////////////////////////////////////////////////

    
    
    /**
     * Allocate memory for a certain sized volume.
     * Attention: If there is already a dataset stored internally, then the memory will get deleted!!
     * \param iSizeX x size of the new volume
     * \param iSizeY y size of the new volume
     * \param iSizeZ z size of the new volume
     * \param fInit the value the voxels get initialized to (default: 0.0)
     * \param bInit if true the voxels get initialized to fInit, if false, the data stays uninitialized (default: true)
     */
    void allocate(unsigned int iSizeX, unsigned int iSizeY, unsigned int iSizeZ, const T fInit = 0.0, bool bInit = true);

    /**
     * Do a deep copy from another svt_volume object - regardless if internal memory or link to external memory.
     * \param rVolume reference to other object
     */
    void deepCopy( svt_volume<T>& rVolume );
    /**
     * Do a deep copy from a memory block
     * \param iSizeX x size of the new volume data
     * \param iSizeY y size of the new volume data
     * \param iSizeZ z size of the new volume data
     * \param pData pointer to data
     */
    void deepCopy( unsigned int iSizeX, unsigned int iSizeY, unsigned int iSizeZ, T* pData );

    /**
     * Set volume memory to internally allocated memory or not
     * \param bInternal if true the destructor of the object will delete memory!
     */
    void setInternal( bool bInternal );

    /**
     * Print the content of the volume
     */
    void print();

    /**
     * Links the complete volume data set to an external memory block.
     * \param iSizeX x size of the new volume data
     * \param iSizeY y size of the new volume data
     * \param iSizeZ z size of the new volume data
     * \param pData pointer to the external volume data
     */
    void setData( unsigned int iSizeX, unsigned int iSizeY, unsigned int iSizeZ, T* pData );

    /**
     * Returns the pointer to the memory location of the data.
     * Attention: If this is internally allocated memory, do not delete! The memory will get deleted automatically when the object is destroyed.
     * \return pointer to T
     */
    T* getData() const;

    /**
     * Changes one voxel value.
     * Does boundary checking!
     * \param iX x coordinate
     * \param iY y coordinate
     * \param iZ z coordinate
     * \param fValue new value
     */
    inline void setValue( unsigned int iX, unsigned int iY, unsigned int iZ, T fValue );
    /**
     * Changes all voxel values.
     * \param fValue new value
     */
    inline void setValue( T fValue );

    /**
     * Get the value at a position inside the volume.
     * Does boundary checking!
     * \param iX x coordinate
     * \param iY y coordinate
     * \param iZ z coordinate
     * \return value
     */
    virtual T getValue( unsigned int iX, unsigned int iY, unsigned int iZ ) const;

    /**
     * Get the value at a position inside the volume.
     * Does boundary checking!
     * \param iCount counter
     * \return value
     */
    virtual T getValue( unsigned int iCount ) const;

    /**
     * Get the value at a position inside the volume - real space positions, considers origin, trilinear interpolation.
     * \param fX x coordinate
     * \param fY y coordinate
     * \param fZ z coordinate
     * \return value
     */
    T getRealSpaceValue(Real64 fX, Real64 fY, Real64 fZ) const;

    /**
     * Get the value at a position inside the volume.
     * No boundary checking!
     * \param iX x coordinate
     * \param iY y coordinate
     * \param iZ z coordinate
     * \return value
     */
    T at( unsigned int iX, unsigned int iY, unsigned int iZ ) const;
    /**
     * Get the value at a position inside the volume.
     * No boundary checking!
     * \param iCount counter
     * \return value
     */
    T at( unsigned int iCount ) const;
    /**
     * Set the value at a position inside the volume.
     * No boundary checking!
     * \param iX x coordinate
     * \param iY y coordinate
     * \param iZ z coordinate
     * \param fValue new voxel value
     */
    void setAt( unsigned int iX, unsigned int iY, unsigned int iZ, T fValue );
    /**
     * Set the value at a position inside the volume.
     * No boundary checking!
     * \param iCount counter
     * \param fValue new voxel value
     */
    void setAt( unsigned int iCount, T fValue );

    /**
     * set the size
     */
    void setSize(unsigned int iSizeX, unsigned int iSizeY, unsigned int iSizeZ);
    /**
     * Get the x size of the volume.
     * \return x size of the volume
     */
    unsigned int getSizeX() const;
    /**
     * Get the y size of the volume.
     * \return y size of the volume
     */
    unsigned int getSizeY() const;
    /**
     * Get the z size of the volume.
     * \return z size of the volume
     */
    unsigned int getSizeZ() const;
    /**
     * Get the number of voxels of the volume.
     * \return number of voxels of the volume
     */
    unsigned int size() const;

    
    /**
     * set position of first voxel
     */
    inline void setGrid( Real64 fGridX, Real64 fGridY, Real64 fGridZ );
    /**
     * set X position of first voxel
     */
    inline void setGridX( Real64 fGridX );
    /**
     * set Y position of first voxel
     */
    inline void setGridY( Real64 fGridY );
    /**
     * set Z position of first voxel
     */
    inline void setGridZ( Real64 fGridZ );
    /**
     * get position of first voxel
     */
    inline Real64 getGridX() const;
    /**
     * get position of first voxel
     */
    inline Real64 getGridY() const;
    /**
     * get position of first voxel
     */
    inline Real64 getGridZ() const;

    /**
     * get the interpolated value at a position inside the volume
     * \param fX x coordinate
     * \param fY y coordinate
     * \param fZ z coordinate
     * \return value
     */
    virtual T getIntValue( Real64 fX, Real64 fY, Real64 fZ ) const;

    /**
     * get the minimum density.
     * \return the minimum density
     */
    T getMinDensity();
    /**
     * get the maximum density.
     *  \return the maximum density
     */
    T getMaxDensity();
    /**
     * get the average density.
     *  \return the average density
     */
    T getAvgDensity();

    /**
     * get norm of the volume = sqrt( sum( at(i) * at(i) ) );
     * \return norm
     */
    Real64 getNorm();

    /**
     * set voxel width (in Angstroem)
     * \param fWidth voxel width
     */
    inline void setWidth( Real64 fWidth );
    /**
     * get voxel width (in Angstroem)
     * \return voxel width
     */
    inline Real64 getWidth() const;

    /**
     * normalize volume
     */
    void normalize();

    /**
     * normalize volume
     * \param fSigma -  blurring kernel following local normalization (default fSigma=1.0 - don't blur) 
     */
    void locallyNormalize( T fSigma, bool bProgress = false );
 
    /**
     * normalize volume
     * \param fSigma -  blurring kernel following local normalization (default fSigma=1.0 - don't blur) 
     */
    void locallyNormalizeCorrectBorders( T fSigma, bool bProgress = false );

    /**
     * Threshold volume. All voxel values above a threshold value are cutoff and set to the threshold.
     * \param fMaxDensity new maximum density
     * \param fMinDensity new minimum density
     */
    void threshold( T fMinDensity, T fMaxDensity );

    /**
     * Crop volume - cuts out and returns a subvolume
     * \param iNewMinX new minimum x voxel index
     * \param iNewMaxX new maximal x voxel index
     * \param iNewMinY new minimum y voxel index
     * \param iNewMaxY new maximal y voxel index
     * \param iNewMinZ new minimum z voxel index
     * \param iNewMaxZ new maximal z voxel index
     */
    svt_volume<T>& crop(
		       unsigned int iNewMinX, unsigned int iNewMaxX,
		       unsigned int iNewMinY, unsigned int iNewMaxY,
		       unsigned int iNewMinZ, unsigned int iNewMaxZ
		      );

    /**
     * Delete a spherical subregion
     * \param fCenterX center voxel coordinate for the spherical region
     * \param fCenterY center voxel coordinate for the spherical region
     * \param fCenterZ center voxel coordinate for the spherical region
     * \param fRadius radius of the sphere
     * \return returns a new svt_volume object with the spherical region cut out
     */
    svt_volume<T>& cutSphere( Real64 fCenterX, Real64 fCenterY, Real64 fCenterZ, Real64 fRadius );

    /**
     * cuts out a spherical subregion
     * \param fCenterX center voxel coordinate for the spherical region
     * \param fCenterY center voxel coordinate for the spherical region
     * \param fCenterZ center voxel coordinate for the spherical region
     * \param fRadius radius of the sphere
     * \return returns a new svt_volume object with the cutted out spherical region
     */
    svt_volume<T>& copySphere( Real64 fCenterX, Real64 fCenterY, Real64 fCenterZ, Real64 fRadius );


    /**
     * Mask with another svt_volume object - all the voxels in this vol are multiplied with the mask volume voxels (multiplied by 0 for not overlapping voxels).
     * \param rMask reference to other object
     */
    svt_volume<T>& mask( svt_volume<T>& rMask );
    /**
     * Make a mask volume - if voxel > threshold it is set to 1, otherwise 0
     * \param fThreshold threshold value
     */
    svt_volume<T>& makeMask( T fThreshold );
    /**
     * Invert mask - all voxels have to be between 0 and 1. The voxel values are inverted.
     */
    svt_volume<T>& invertMask();

     /**
     * shrinks the original volume to the occupied Volume - redefine m_fGrid and m_iSize such that volume is not padded by zeros
     */
     void shrinkToOccupiedVolume( );

    /**
     * get the new border
     * \param iMinX the min value of X
     * \param iMaxX the max value of X
     * \param iMinY the min value of Y
     * \param iMaxY the max value of Y
     * \param iMinZ the min value of Z
     * \param iMaxZ the max value of Z
     * \return the new index (voxel value)
     */
     unsigned int getNewBorder( unsigned int iMinX, unsigned int iMaxX, unsigned int iMinY, unsigned int iMaxY, unsigned int iMinZ, unsigned int iMaxZ );

     /**
      * interpolate the map to another lattice
      */
    void interpolate_map(Real64 fWidthX, Real64 fWidthY=0.0, Real64 fWidthZ=0.0);

    /**
     * Calc gradient map
     * \param eMode gradient method selector: CentralDistance or Sobel
     * \param rGradientX reference to an svt_volume object for the gradient map in x direction
     * \param rGradientY reference to an svt_volume object for the gradient map in y direction
     * \param rGradientZ reference to an svt_volume object for the gradient map in z direction
     */
    void calcGradient( const GradientMode eMode, svt_volume<T>& rGradientX, svt_volume<T>& rGradientY, svt_volume<T>& rGradientZ ) const;

    /**
     * Set densities below limit to zero
     * \param fLimit the threshold
     */
    void threshold( T fLimit );

    /**
     * Convolve this volume with another one (typically a 3D kernel filter)
     * \param rKernel reference to kernel volume
     * \param bNormalize normalizes during convolution
     * \param bProgress show a progress bar
     */
    void convolve( svt_volume<T>& rKernel, bool bProgress );
    /**
     * Convolve this volume with another one, which is a 1D volume (only x axis). The 1D kernel will get convolved in all three directions, so this function should be
     * used with linear separable kernels.
     * \param rKernel reference to kernel volume
     * \param bNormalize normalizes during convolution
     * \param bProgress show a progress bar
     */
    void convolve1D3D( svt_volume<T>& rKernel, bool bProgress );

    /**
     * Create a Laplacian kernel volume.
     * Attention: This will overwrite the current content of the volume object with the filter kernel. It will allocate the memory internally.
     */
    void createLaplacian();

    /**
     * Create a Gaussian kernel volume within SigmaFactor*fSigma
     * Attention: This will overwrite the current content of the volume object with the filter kernel. It will allocate the memory internally.
     * \param fSigma1D sigma of map
     * \param fSigmaFactor sigma factor
     */
    void createGaussian( double fSigma1D, double fSigmaFactor );

    /**
     * Create a one-dimensional Gaussian blurring kernel volume (Situs scheme)
     * \param fWidth the voxel width of the target map one wants to convolve with the kernel
     * \param fResolution the target resolution
     * \param fVarp variance of map (if 0 no correction for lattice interpolation smoothing effects = default)
     * Attention: This will overwrite the current content of the volume object with the filter kernel. It will allocate the memory internally.
     */
    void create1DBlurringKernel( Real64 fWidth, Real64 fResolution, Real64 fVarp =0.0);

    /**
     * Create a Gaussian blurring kernel volume (Situs scheme)
     * \param fWidth the voxel width of the target map one wants to convolve with the kernel
     * \param fResolution the target resolution
     * \param fVarp variance of map (if 0 no correction for lattice interpolation smoothing effects = default)
     */
    void createSitusBlurringKernel( Real64 fWidth, Real64 fResolution, Real64 fVarp =0.0);

    /**
     * Create a Laplacian of a Gaussian kernel volume.
     * \param fWidth the voxel width of the target map one wants to convolve with the kernel
     * \param fResolution the target resolution
     * \param fVarp variance of map (if 0 no correction for lattice interpolation smoothing effects = default)
     * Attention: This will overwrite the current content of the volume object with the filter kernel. It will allocate the memory internally.
     */
    void createLaplacianOfGaussian( Real64 fWidth, Real64 fResolution, Real64 fVarp =0.0);

    /**
     *  Criterion for pairwise comparision of STL Vectors.
     * \param oVector1 - first  vector of pairs 
     * \param oVector2 - second vector of pairs
     */

    bool criterionPair(pair<T, T> oVector1, pair<T, T> oVector2);

    
    /**
      * calculate Fischer Determinant
      * \param fVectorVal_VarianceOnePath reference vector with variance
      * \param fVectorVal_VarianceOnePath_Order reference for vector with descending order of fVectorVal_VarianceOnePath
      * \param iK_star reference for iK_star that maximalizes Functional F(K) and divided fVectorVal_VarianceOnePath for two classes.
      */
    void FisherDiscriminant(vector <Real64> & fVectorVal_VarianceOnePath, vector <unsigned int > & iVectorVal_VarianceOnePath_Order,unsigned int & iK_star);

    
    /**
     * Filtrate Volume with Bilateral Filter
     * Attention: This will overwrite the current content of the volume object with the filter kernel. It will allocate the memory internally.
     * \param fSigma1D1 sigma of Gaussuian part of filter
     * \param fsigma1D2 sigma of intensity part of filter
     * \param iSize size of the kernel
     * \param bProgress show a progress bar
     */
    void filtrBilateral(double fSigma1D1, double fSigma1D2, unsigned int iSize,  bool bProgress);

    /**
     * calculate One Geodesic Step in path based on Volume
     * \param oPathVector store the output Geodesic path
     * \param oPathSoFar  store current Geodesic path
     * \param oToPos next (x,y,z) position of theoretical jumping particle
     * \param iFullLength the required length of path
     * \param iCubeSize the size of Kernel
     */

    void stepGeodesic(vector < vector<svt_vector3< int> > > & oPathVector, vector<svt_vector3< int> > & oPathSoFar, svt_vector3< int> oToPos, int iStepNumber, int iFullLength, int iCubeSize, int iNeigboorModel);

    /**
      * calculate all Geodesic Path for Volume
     * \param oPathVector store the output Geodesic path
     * \param oPathSoFar  store current Geodesic path
     * \param oToPos next (x,y,z) position of theoretical jumping particle
     * \param iFullLength the required length of path
     * \param iCubeSize the size of Kernel
     */

    void findGeodesicPaths(vector < vector<svt_vector3< int> > > & oPathVector, svt_vector3< int> oToPos, int iFullLength, int iCubeSize, int iNeigboorModel);


    /**
      * Filtrate Volume with Geodesic Path Filter
      * Attention: This will overwrite the current content of the volume object with the filter kernel. It will allocate the memory internally.
      * \param fBetha the filter equivalent of Gaussian Sigma
      * \param iMaskDim kernel size
      * \param iPathLength requred path length
      * \param bProgress show a progress bar
     */
    void filtrGeodesicPath(double fBetha, int iNeigboorModel, unsigned int iMaskDim,  unsigned int iPathLength,  bool bProgress);

    /**
     * Create an Identity kernel volume
     * Attention: This will overwrite the current content of the volume object with the filter kernel. It will allocate the memory internally.
     */
    void createIdentity();

    /**
     * Create sphere at the center of the volume
     * \param fRadius radius of the sphere
     */
    void createSphere( Real64 fRadius );

    /**
     * Applies a Laplacian filter.
     * \param bRelax if true the poisson relaxation procedure is applied
     */
    void applyLaplacian(bool bRelax=true)
    {
	svt_volume<T> oLaplace;
	oLaplace.createLaplacian();

	// relaxation procedure
	if (bRelax)
	{
	    //unsigned int iIgnored[3];  /* zero margin ignored in fast kernel convolution */
	    //iIgnored[0] = (m_iSizeX+1)/4; iIgnored[1] = (m_iSizeY+1)/4; iIgnored[2] = (m_iSizeZ+1)/4;
	    //pphiLaplace = new MYFLT[iNvox];
	    //int x;
	    //unsigned int iNVox = m_iSizeX*m_iSizeY*m_iSizeZ;
	    //for (x=0;x<=iNVox;x++)
	    //    pphiLaplace[x] = pphi[x];
	    //relax_laplacian(&pphiLaplace, m_iExtX, m_iExtY, m_iExtZ, iIgnored, 5.0);
	}

	convolve(oLaplace, false);
    };

    /**
     * Calculate correlation with other svt_volume object
     * \param rVol reference to other svt_volume object
     */
    Real64 correlation( svt_volume<T>& rVol, bool bMask = true );

    /**
     * Calculate correlation with other svt_volume object
     * \param rVol reference to other svt_volume object
     */
    Real64 correlationColores( svt_volume<T>& rVol );

    /**
     * Calculates the internal correlation of the voxel values.
     */
    svt_volume<Real64>* internalCorr( unsigned int iWidth );

    /**
     * Grow/Shrink - this function blows up/interpolates the volume at a different size (not a padding routine!).
     * \param iNewSizeX new size (x dimension)
     * \param iNewSizeY new size (y dimension)
     * \param iNewSizeZ new size (z dimension)
     */
    void resize(unsigned int iNewSeizeX, unsigned int iNewSizeY, unsigned int iNewSizeZ);

    /**
     * Calculate number of occupied voxels
     * \param fThreshold threshold value - only voxels higher than the threshold are counted.
     * \return number of voxels with a density higher than fThreshold
     */
    unsigned long getOccupied( T fThreshold ) const;

    /**
     * Load a file. This function looks at the extension to determine which function has to be used to actually load the file.
     * \param pFname pointer to array of char with the filename
     */
    svt_matrix4<T> load(const char* pFname);
    /**
     * Save a file. This function looks at the extension to determine which function has to be used to actually save the file.
     * \param pFname pointer to array of char with the filename
     */
    void save(const char* pFname);

    /**
     * Read situs file.
     * \param pFilename pointer to array of char with the filename
     * \returns matrix4 object with the transformation (situs only supports translations)
     */
    svt_matrix4<T> loadSitus( const char* pFilename );

    /**
     * Write situs file.
     * \param pFilename pointer to array of char with the filename
     */
    void saveSitus( const char* pFilename ) const;

    /**
     * Read raw volume file
     * \param pFilename pointer to array of char with the filename
     */
    void loadRaw(const char* pFilename);

    /**
     * Reads MRC/CCP4 maps from file
     * \param pFilename pointer to array of char with the filename
     */
    void loadMRC( const char *pFilename, unsigned int iMinLoadX =0, unsigned int iMinLoadY =0, unsigned int iMinLoadZ =0, unsigned int iMaxLoadX =0, unsigned int iMaxLoadY =0, unsigned int iMaxLoadZ =0 );
    /**
     * Writes MRC/CCP4 maps to file
     * \param pFilename pointer to array of char with the filename
     */
    void writeMRC( const char *pFilename );


    /**
     * Reads SPIDER maps from file
     * \param pFilename pointer to array of char with the filename
     */
    void loadSpider( const char *pFilename, float fVoxSize = 0.0f);
    /**
     * Writes SPIDER maps to file
     * \param pFilename pointer to array of char with the filename
     */
    void writeSpider( const char *pFilename );

    /**
     * Save povray df3 file
     * \param pFilename pointer to array of char with teh filename
     */
    void writeDF3( const char* pFilename );

    /**
     * Set cutoff for sampling
     * \param fCutoff a voxel value lower than this value is not considered for the sampling
     */
    void setCutoff( Real64 fCutoff );
    /**
     * Get cutoff for sampling
     * \return a voxel value lower than this value is not considered for the sampling
     */
    Real64 getCutoff( ) const;

    /**
     * sample the object randomly and return a vector that refrects the probability distribution of the object
     */
    svt_vector4<Real64> sample();

    /**
     * sample the object in the sphere randomly and return a vector that refrects the probability distribution of the object
     * \param oCenter keeps coordinates of center of sphere
     * \param fRadius is radius of sphere
     */
    svt_vector4<Real64> sampleSphere(svt_vector4<Real64> oCenter, Real64 fRadius );

    /**
     * Non-recursive flood-fill segmentation algorithm. All voxels that are connected to a start voxel and are above the threshold are kept, the others are removed.
     * The algorithm creates a mask that is later blurred by a Gaussian kernel. The sigma of the gaussian can be specified.
     * \param iStartX x index of the start voxel
     * \param iStartY y index of the start voxel
     * \param iStartZ z index of the start voxel
     * \param fTreshold threshold for the floodfill
     * \param fGaussian sigma of the gaussian the mask is convoluted with (if 0, no blurring of the mask is applied)
     */
    void floodfill_segmentation( unsigned int iStartX, unsigned int iStartY, unsigned int iStartZ, T fThreshold, Real64 fGaussian );

    /**
     * Non-recursive flood-fill algorithm. The algorithm fills the voxels that are above the threshold (and that are connected to the start voxel) with the specified value.
     * \param iStartX x index of the start voxel
     * \param iStartY y index of the start voxel
     * \param iStartZ z index of the start voxel
     * \param fTreshold threshold for the floodfill
     * \param fGaussian sigma of the gaussian the mask is convoluted with (if 0, no blurring of the mask is applied)
     */
    void floodfill( unsigned int iStartX, unsigned int iStartY, unsigned int iStartZ, T fThreshold, T fFillValue );

    /**
    * gives the best ISO threshold for fitting the object with a given volume
    */
    T bestISO( svt_volume<T>& rTarget, T fThreshold );

    /**
     * Calculates the corresponding isosurface threshold value for this volume. The algorithm tries to cover exactly the voxel values of the other volume with the ones from here, so that
     * both isosurfaces have a similar size. rVol_A would typically have a higher resolution but show the exact same system.
     *
     * \param rVol_A other volume
     * \param fThresh good known threshold value of rVol_A
     */
    T correspondingISO( svt_volume<T>& rVol_A, T fThreshold );

    /**
    * scales the entire map by a given factor
    */
    void scale(T fScale);

    /**
    * shifts the entire map by a given factor
    */
    void shift(T fShift);

    /**
    * scales by the slope
    */
    void scaleBySlope(T fSlope);

    /**
    * returns the slope given two points
    */
    T getSlope(T fX1, T fY1, T fX2, T fY2);

    /**
    * does an interpolated scaling of the object to match another
    */
    void interpolatedScale(T fThreshold1, T fThreshold2, T fNew1, T fNew2);

    /**
    * calculates the root mean square of the volume
    */
    T getRMS();

    /**
     * Remove every structure that has more than two neighbors
     * \param fThreshold voxel above that value are considered occupied
     */
    void removeNeighbors( T fThreshold );

    /**
     * How many direct neighbors does a voxel have?
     * \return number of occupied neighboring voxels
     */
    unsigned int numNeighbors( unsigned int iX, unsigned int iY, unsigned int iZ, T fThreshold );

private:

    /**
     * Calculate/update the minimum and maximum density values.
     */
    void calcMinMaxDensity();

    /**
     * Simple internal routine that calculates the index of a voxel, depending on the data order
     * \param iIndex number of the voxel
     * \param iDataOrder data order
     */
    unsigned int calc_xyz_index( unsigned int iIndex, unsigned int iDataOrder );

    /**
     * Simple internal routine that calculates the x,y,z index of a voxel
     * \param iIndex number of the voxel
     * \param iDataOrder data order
     * \param rX reference to unsigned int
     * \param rY reference to unsigned int
     * \param rZ reference to unsigned int
     */
    void calc_xyz( unsigned int iIndex, unsigned int iDataOrder, unsigned int iMaxX, unsigned int iMaxY, unsigned int iMaxZ, unsigned int& rX, unsigned int& rY, unsigned int& rZ );
};

///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 */
template<class T>
inline svt_volume<T>::svt_volume() :
m_pData( NULL ),
m_fWidth( 1.0 ),
m_fGridX( 0.0 ),
m_fGridY( 0.0 ),
m_fGridZ( 0.0 ),
m_bInternal( false ),
m_bChanged( true ),
m_fCutoff( -1.0E30 ),
m_fNorm( 0.0 )
{
    setData(0,0,0,(T*)NULL);
};

/**
 * Constructor.
 * This constructor will create a volume of a certain size. Attention: The memory will get delete when the object gets destroyed!
 * \param iSizeX size in x direction of the new volume
 * \param iSizeY size in y direction of the new volume
 * \param iSizeZ size in z direction of the new volume
 * \param fInit the value the voxels get initialized to
 */
template<class T>
inline svt_volume<T>::svt_volume( unsigned int iSizeX, unsigned int iSizeY, unsigned int iSizeZ, const T fInit ) :
m_pData( NULL),
m_fWidth( 1.0 ),
m_fGridX( 0.0 ),
m_fGridY( 0.0 ),
m_fGridZ( 0.0 ),
m_bInternal( true ),
m_bChanged( true ),
m_fCutoff( -1.0E30 ),
m_fNorm( 0.0 )
{
    allocate( iSizeX, iSizeY, iSizeZ, fInit );
};

/**
 * Constructor.
 * This constructor will create a volume of a certain size.
 * Attention: The memory will get delete when the object gets destroyed!
 * \param iSizeX size in x direction of the new volume
 * \param iSizeY size in y direction of the new volume
 * \param iSizeZ size in z direction of the new volume
 * \param pData pointer to memory block that gets copied into the new object.
 */
template<class T>
inline svt_volume<T>::svt_volume(unsigned int iSizeX, unsigned int iSizeY, unsigned int iSizeZ, const T* pData) :
m_pData( NULL),
m_fWidth( 1.0 ),
m_fGridX( 0.0 ),
m_fGridY( 0.0 ),
m_fGridZ( 0.0 ),
m_bInternal( true ),
m_bChanged( true ),
m_fCutoff( -1.0E30 ),
m_fNorm( 0.0 )
{
    allocate( iSizeX, iSizeY, iSizeZ );

    memcpy( m_pData, pData, sizeof(T)*iSizeX*iSizeY*iSizeZ );
};

/**
 * Copy constructor
 */
template<class T>
inline svt_volume<T>::svt_volume( svt_volume<T>& rVol ) : svt_sampled< svt_vector4<Real64> >(),
m_pData( NULL ),
m_fWidth( 1.0 ),
m_fGridX( 0.0 ),
m_fGridY( 0.0 ),
m_fGridZ( 0.0 ),
m_bInternal( true ),
m_bChanged( true ),
m_fCutoff( -1.0E30 ),
m_fNorm( 0.0 )
{
    *this = rVol;
};

/**
 * Destructor
 */
template<class T>
inline svt_volume<T>::~svt_volume()
{
    if (m_bInternal)
	delete[] m_pData;
};

///////////////////////////////////////////////////////////////////////////////
// Operators
///////////////////////////////////////////////////////////////////////////////

/**
 * Assign a scalar value, e.g. A=1.
 * The size of A will be unchanged, value is assigned to all elements.
 */
template<class T>
inline svt_volume<T>& svt_volume<T>::operator=(const T& fValue)
{
    setValue( fValue );

    return *this;
};

/**
 * Assign a volume B to a volume A, e.g. A=B.
 * Size and values will become same as B.
 * Attention: This makes a deep copy if the memory was allocated by the object B itself. If
 * the memory was just linked to B via a pointer (see setData()), then only the pointer
 * is copied but not the data itself. Please use deepCopy() if you wish to copy the data in such case.
 */
template<class T>
svt_volume<T>& svt_volume<T>::operator=(const svt_volume<T>& rVol)
{
    m_iSizeX = rVol.m_iSizeX;
    m_iSizeY = rVol.m_iSizeY;
    m_iSizeZ = rVol.m_iSizeZ;

    m_fMaxDensity = rVol.m_fMaxDensity;
    m_fMinDensity = rVol.m_fMinDensity;

    m_fWidth = rVol.m_fWidth;

    m_fGridX = rVol.m_fGridX;
    m_fGridY = rVol.m_fGridY;
    m_fGridZ = rVol.m_fGridZ;

    m_bInternal = rVol.m_bInternal;

    if (m_pData != NULL && m_bInternal)
    {
	//svtout << "Attention: memory for new volume gets allocated, " << endl;
	//svtout << "   but object had already content before (which gets destroyed)" << endl;
	delete[] m_pData;
    }

    // deep copy or just pointer copy
    if (rVol.m_bInternal == true)
    {
	unsigned int iNum = m_iSizeX * m_iSizeY * m_iSizeZ;
	m_pData = new T[iNum];
	memcpy( m_pData, rVol.m_pData, iNum*sizeof(T) );

    } else
	m_pData = rVol.m_pData;

    m_bChanged = rVol.m_bChanged;
    m_fCutoff = rVol.m_fCutoff;
    m_fNorm = rVol.m_fNorm;

    return *this;
};

/**
 * Multiply with scalar.
 * \param fScalar the scalar the voxels of the volume are getting multiplied with.
 */
template<class T>
svt_volume<T>& svt_volume<T>::operator*(const T fScalar)
{
    unsigned int iNum = m_iSizeX * m_iSizeY * m_iSizeZ;

    for(unsigned int i=0; i<iNum; i++)
	setAt(i, at(i) * fScalar);

    m_bChanged = true;

    return *this;
};
/**
 * Add another volume.
 * \param rVol reference to the other volume
 */
template<class T>
void svt_volume<T>::operator+=(const svt_volume<T> & rVol)
{
    if (m_fGridX != rVol.getGridX() || m_fGridY != rVol.getGridY() || m_fGridZ != rVol.getGridZ() ||
        m_iSizeX != rVol.getSizeX() || m_iSizeY != rVol.getSizeY() || m_iSizeZ != rVol.getSizeZ() ||
        m_fWidth != rVol.getWidth() )
    {
	svt_exception::ui()->error("Can not execute += operator as the maps have different size!");
        return;
    }

    unsigned int iNum = m_iSizeX * m_iSizeY * m_iSizeZ;

    for(unsigned int i=0; i<iNum; i++)
	setAt(i, at(i) + rVol.at(i));

    m_bChanged = true;
};

/**
 * Multiply with matrix - resulting volume has different dimensions that the original
 * \param rMat a transformation matrix that will translate/rotate the volume -
 */
template< class T>
svt_volume<T> svt_volume<T>::operator*(svt_matrix4< T > oMat)
{
    svt_vector4<Real64> oGrid;
    svt_vector4<unsigned int> oSize = getDimensions(oMat, oGrid);

    //first get the size of the new volume
    svt_volume<T> oVol( oSize.x(), oSize.y(), oSize.z() );
    oVol.setGridX( oGrid.x() );
    oVol.setGridY( oGrid.y() );
    oVol.setGridZ( oGrid.z() );
    oVol.setWidth(m_fWidth);

    //apply transformation to this and project on oVol
    applyTransformation(oVol, oMat);

    oVol.shrinkToOccupiedVolume();

    return oVol;
};

/**
 *  apply the transformation of the current volume (this) and project onto
 *  \param rDest the volume where to project
 *  \param the tranformation to apply
 *  ATTENTION: rDest should be already allocated
 */
template< class T>
void svt_volume<T>::applyTransformation(svt_volume<Real64> &rDest, svt_matrix4<Real64> oMat)
{
    oMat.invert(); // get the inverse transformation

    svt_vector4<Real64> oVec, oInvVec;
    int iX0, iY0, iZ0, iX1, iY1, iZ1;
    Real64 fGx, fGy, fGz, fA, fB, fC;
    Real64 fNewDensity;
    for (unsigned int iX=0; iX<rDest.getSizeX(); iX++)
    {
        oVec.x( rDest.getGridX()+iX*m_fWidth );
        for (unsigned int iY=0; iY<rDest.getSizeY(); iY++)
        {
            oVec.y( rDest.getGridY()+iY*m_fWidth );
            for (unsigned int iZ=0; iZ<rDest.getSizeZ(); iZ++)
            {
                oVec.z( rDest.getGridZ() + iZ*m_fWidth );
                oInvVec = oMat*oVec; // this vector is in the non rotated spate

                fGx = (oInvVec.x() - m_fGridX)/m_fWidth;
                fGy = (oInvVec.y() - m_fGridY)/m_fWidth;
                fGz = (oInvVec.z() - m_fGridZ)/m_fWidth;

                iX0 = floor( fGx );
                iY0 = floor( fGy );
                iZ0 = floor( fGz );

                iX1 = ceil( fGx );
                iY1 = ceil( fGy );
                iZ1 = ceil( fGz );

                fA = fGx-iX0;
		fB = fGy-iY0;
		fC = fGz-iZ0;

		/* interpolate */

                if (iX0 >= 0 && iX0 <= (int)m_iSizeX && iY0 >= 0 && iY0 <= (int)m_iSizeY && iZ0 >=0 && iZ0 <= (int)m_iSizeZ&&
                    iX1 >= 0 && iX1 <= (int)m_iSizeX && iY1 >= 0 && iY1 <= (int)m_iSizeY && iZ1 >=0 && iZ1 <= (int)m_iSizeZ ) // inside the original space
                {
                    fNewDensity =
                        fA 	* fB 		* fC 		* this->getValue(iX1, iY1, iZ1) +
                        (1-fA) 	* fB 		* fC 		* this->getValue(iX0, iY1, iZ1) +
                        fA 	* (1-fB) 	* fC 		* this->getValue(iX1, iY0, iZ1) +
                        fA 	* fB 		* (1-fC) 	* this->getValue(iX1, iY1, iZ0) +
                        fA 	* (1-fB) 	* (1-fC) 	* this->getValue(iX1, iY0, iZ0) +
                        (1-fA) 	* fB 		* (1-fC) 	* this->getValue(iX0, iY1, iZ0) +
                        (1-fA) 	* (1-fB) 	* fC 		* this->getValue(iX0, iY0, iZ1) +
                        (1-fA) 	* (1-fB) 	* (1-fC) 	* this->getValue(iX0, iY0, iZ0);
                }
                else
                    fNewDensity = 0.0;

                rDest.setValue( iX, iY, iZ, rDest.at(iX, iY, iZ) + fNewDensity );
            }
        }
    }

}

/**
 * get dimension of the volume after appling transformation
 * \param the transformation to apply
 * \param rNewGrid the new origin
 * \return the new size
 */
template< class T>
svt_vector4<unsigned int> svt_volume<T>::getDimensions(svt_matrix4< T > oMat, svt_vector4<Real64>& rNewGrid)
{
    svt_vector4<unsigned int> oSize;
    oSize.x( 0 );
    oSize.y( 0 );
    oSize.z( 0 );

    //init the first origin
    rNewGrid.x( m_fGridX );
    rNewGrid.y( m_fGridY );
    rNewGrid.z( m_fGridZ );
    rNewGrid = oMat*rNewGrid;

    //update grid and size
    updateDimensions(oMat, rNewGrid, oSize, svt_vector4<unsigned int>(1,0,0) );
    updateDimensions(oMat, rNewGrid, oSize, svt_vector4<unsigned int>(0,1,0) );
    updateDimensions(oMat, rNewGrid, oSize, svt_vector4<unsigned int>(0,0,1) );
    updateDimensions(oMat, rNewGrid, oSize, svt_vector4<unsigned int>(1,1,0) );
    updateDimensions(oMat, rNewGrid, oSize, svt_vector4<unsigned int>(0,1,1) );
    updateDimensions(oMat, rNewGrid, oSize, svt_vector4<unsigned int>(1,0,1) );
    updateDimensions(oMat, rNewGrid, oSize, svt_vector4<unsigned int>(1,1,1) );

    //rNewGrid is now the origin given by the rotated bounding box; not necessary on the same grid as this

    svt_vector4<Real64> oFarCorner;
    oFarCorner.x( rNewGrid.x()+m_fWidth*oSize.x() );
    oFarCorner.y( rNewGrid.y()+m_fWidth*oSize.y() );
    oFarCorner.z( rNewGrid.z()+m_fWidth*oSize.z() );

    //put volume on the original grid
    rNewGrid.x( m_fGridX + floor( (rNewGrid.x()- m_fGridX)/m_fWidth)*m_fWidth );
    rNewGrid.y( m_fGridY + floor( (rNewGrid.y()- m_fGridY)/m_fWidth)*m_fWidth );
    rNewGrid.z( m_fGridZ + floor( (rNewGrid.z()- m_fGridZ)/m_fWidth)*m_fWidth );

    oSize.x ( ceil((oFarCorner.x() - rNewGrid.x() )/m_fWidth) );
    oSize.y ( ceil((oFarCorner.y() - rNewGrid.y() )/m_fWidth) );
    oSize.z ( ceil((oFarCorner.z() - rNewGrid.z() )/m_fWidth) );

    return oSize;
}

/**
 * update the dimensions, size and origin
 * \param rNewSize the size up to now, it will be updated if needed
 * \param rNewGrid the origin up to now, it will be updated
 * \param rDir gives the corner that will be consider
 */
template< class T>
void svt_volume<T>::updateDimensions(svt_matrix4< T > oMat, svt_vector4<Real64>& rNewGrid,  svt_vector4<unsigned int>& rNewSize, svt_vector4<unsigned int> oDir)
{
    svt_vector4<Real64> oNewCorner, oCorner;
    oCorner.x( m_fGridX + m_fWidth*oDir.x()*m_iSizeX);
    oCorner.y( m_fGridY + m_fWidth*oDir.y()*m_iSizeY);
    oCorner.z( m_fGridZ + m_fWidth*oDir.z()*m_iSizeZ);
    //get the coordinates of the new corner
    oNewCorner = oMat*oCorner;

    //origin update
    if (rNewGrid.x() > oNewCorner.x())
        rNewGrid.x( oNewCorner.x() );

    if (rNewGrid.y() > oNewCorner.y())
        rNewGrid.y( oNewCorner.y() );

    if (rNewGrid.z() > oNewCorner.z())
        rNewGrid.z( oNewCorner.z() );

    //size update
    unsigned int iNewSize;
    iNewSize =  ceil( abs(rNewGrid.x()-oNewCorner.x())/m_fWidth);
    if ( iNewSize > rNewSize.x())
        rNewSize.x ( iNewSize );

    iNewSize =  ceil( abs(rNewGrid.y()-oNewCorner.y())/m_fWidth);
    if ( iNewSize > rNewSize.y())
        rNewSize.y ( iNewSize );

    iNewSize =  ceil( abs(rNewGrid.z()-oNewCorner.z())/m_fWidth);
    if ( iNewSize > rNewSize.z())
        rNewSize.z ( iNewSize );
}




///////////////////////////////////////////////////////////////////////////////
// Public functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Allocate memory for a certain sized volume.
 * Attention: If there is already a dataset stored internally, then the memory will get deleted!!
 * \param iSizeX x size of the new volume
 * \param iSizeY y size of the new volume
 * \param iSizeZ z size of the new volume
 * \param fInit the value the voxels get initialized to
 * \param bInit if true the voxels get initialized to fInit, if false, the data stays uninitialized.
 */
template<class T>
inline void svt_volume<T>::allocate(unsigned int iSizeX, unsigned int iSizeY, unsigned int iSizeZ, const T fInit, bool bInit)
{
    if (m_pData != NULL && m_bInternal)
    {
	//svtout << "Attention: memory for new volume gets allocated, " << endl;
	//svtout << "   but object had already content before (which gets destroyed)" << endl;
	delete[] m_pData;
    }
    else if (m_pData != NULL && !m_bInternal)
    {
	svtout << "Attention: memory for new volume gets allocated, " << endl;
	svtout << "   but object is still linked to something else..." << endl;
    }

    unsigned int iNum = iSizeX * iSizeY * iSizeZ;
    m_pData = new T[iNum];
    m_bInternal = true;

    m_iSizeX = iSizeX;
    m_iSizeY = iSizeY;
    m_iSizeZ = iSizeZ;

    if (bInit)
    {
        m_fMaxDensity = fInit;
        m_fMinDensity = fInit;

        unsigned int i;
        for(i=0; i<iNum; i++)
            m_pData[i] = fInit;
    }
};

/**
 * Do a deep copy from another svt_volume object - regardless if internal memory or link to external memory.
 * \param rVolume reference to other object
 */
template<class T>
inline void svt_volume<T>::deepCopy( svt_volume<T>& rVolume )
{
    // use assign operator to copy all the member vars
    *this = rVolume;

    // copy data anyway even if external
    if (m_bInternal == false)
    {
	unsigned int iNum = m_iSizeX * m_iSizeY * m_iSizeZ;
	m_pData = new T[iNum];
	memcpy( m_pData, rVolume.m_pData, iNum*sizeof(T) );
	m_bInternal = true;
    }

    m_bChanged = true;
};

/**
 * Do a deep copy from a memory block
 * \param iSizeX x size of the new volume data
 * \param iSizeY y size of the new volume data
 * \param iSizeZ z size of the new volume data
 * \param pData pointer to data
 */
template<class T>
inline void svt_volume<T>::deepCopy( unsigned int iSizeX, unsigned int iSizeY, unsigned int iSizeZ, T* pData )
{
    allocate( iSizeX, iSizeY, iSizeZ );
    unsigned int iNum = iSizeX * iSizeY * iSizeZ;
    memcpy( m_pData, pData, iNum*sizeof(T) );

    m_bChanged = true;
}

/**
 * Set volume memory to internally allocated memory or not
 * \param bInternal if true the destructor of the object will delete memory!
 */
template<class T>
void svt_volume<T>::setInternal( bool bInternal )
{
    m_bInternal = bInternal;
};

/**
 * Print the content of the volume
 */
template<class T>
inline void svt_volume<T>::print()
{

    unsigned int iX, iY, iZ;

    svtout << endl;
    for(iZ=0; iZ<m_iSizeZ; iZ++)
    {
	for(iY=0; iY<m_iSizeY; iY++)
	{
            svtout << " ";
	    for(iX=0; iX<m_iSizeX; iX++)
	    {
		cout << at(iX, iY, iZ);
		if (iX<m_iSizeX-1)
		    cout << ", ";
	    }
	    cout << endl;
	}
	svtout << endl;
    }
};

/**
 * Links the complete volume data set to an external memory block.
 * \param iSizeX x size of the new volume data
 * \param iSizeY y size of the new volume data
 * \param iSizeZ z size of the new volume data
 * \param pData pointer to the external volume data
 */
template<class T>
inline void svt_volume<T>::setData(unsigned int iSizeX, unsigned int iSizeY, unsigned int iSizeZ, T* pData)
{
    m_iSizeX = iSizeX;
    m_iSizeY = iSizeY;
    m_iSizeZ = iSizeZ;
    m_pData = pData;

    m_bChanged = true;
    m_bInternal = false;
};

/**
 * Returns the pointer to the memory location of the data
 * \return pointer to T
 */
template<class T>
inline T* svt_volume<T>::getData() const
{
    return m_pData;
};

/**
 * Changes one voxel value.
 * Does boundary checking!
 * \param iX x coordinate
 * \param iY y coordinate
 * \param iZ z coordinate
 * \param fValue new value
 */
template<class T>
inline void svt_volume<T>::setValue(unsigned int iX, unsigned int iY, unsigned int iZ, T fValue)
{
    if (iX < m_iSizeX && iY < m_iSizeY && iZ < m_iSizeZ)
    {
	m_pData[iX+iY*m_iSizeX+iZ*m_iSizeX*m_iSizeY] = fValue;
	m_bChanged = true;
    }
};
/**
 * Changes all voxel values.
 * \param fValue new value
 */
template<class T>
inline void svt_volume<T>::setValue( T fValue )
{
    unsigned int iNum = m_iSizeX * m_iSizeY * m_iSizeZ;
    unsigned int i;
    for(i=0; i<iNum; i++)
    	m_pData[i] = fValue;

    m_bChanged = true;
};

/**
 * Get the value at a position inside the volume
 * Does boundary checking!
 * \param iX x coordinate
 * \param iY y coordinate
 * \param iZ z coordinate
 * \return value
 */
template<class T>
inline T svt_volume<T>::getValue(unsigned int iX, unsigned int iY, unsigned int iZ) const
{
    if (iX < m_iSizeX && iY < m_iSizeY && iZ < m_iSizeZ)
	return m_pData[iX + (iY*m_iSizeX) + (iZ*m_iSizeX*m_iSizeY)];
    else
	return (T)(0);
};

/**
 * get the value at a position inside the volume
 * \param iCount counter
 * \return value
 */
template<class T>
inline T svt_volume<T>::getValue(unsigned int iCount) const
{
    if (iCount < m_iSizeX * m_iSizeY * m_iSizeZ)
	return m_pData[iCount];
    else
        return (T)(0.0);
};

/**
 * Get the value at a position inside the volume - real space positions, considers origin, trilinearly interpolated.
 * \param fX x coordinate
 * \param fY y coordinate
 * \param fZ z coordinate
 * \return value
 */
template<class T>
inline T svt_volume<T>::getRealSpaceValue(Real64 fX, Real64 fY, Real64 fZ) const
{
    fX -= m_fGridX;
    fY -= m_fGridY;
    fZ -= m_fGridZ;

    fX /= m_fWidth;
    fY /= m_fWidth;
    fZ /= m_fWidth;

    return getIntValue( fX, fY, fZ );
};


/**
 * Get the value at a position inside the volume.
 * No boundary checking!
 * \param iX x coordinate
 * \param iY y coordinate
 * \param iZ z coordinate
 * \return value
 */
template<class T>
inline T svt_volume<T>::at(unsigned int iX, unsigned int iY, unsigned int iZ) const
{
    return m_pData[iX + (iY*m_iSizeX) + (iZ*m_iSizeX*m_iSizeY)];
};
/**
 * Get the value at a position inside the volume.
 * No boundary checking!
 * \param iCount counter
 * \return value
 */
template<class T>
inline T svt_volume<T>::at(unsigned int iCount) const
{
    return m_pData[iCount];
};

/**
 * Set the value at a position inside the volume.
 * No boundary checking!
 * \param iX x coordinate
 * \param iY y coordinate
 * \param iZ z coordinate
 * \param fValue new voxel value
 */
template<class T>
inline void svt_volume<T>::setAt( unsigned int iX, unsigned int iY, unsigned int iZ, T fValue )
{
    m_pData[iX + (iY*m_iSizeX) + (iZ*m_iSizeX*m_iSizeY)] = fValue;
    m_bChanged = true;
};
/**
 * Set the value at a position inside the volume.
 * No boundary checking!
 * \param iCount counter
 * \param fValue new voxel value
 */
template<class T>
    inline void svt_volume<T>::setAt( unsigned int iCount, T fValue )
{
    m_pData[iCount] = fValue;
    m_bChanged = true;
};

/**
 * get the x size of the volume
 * \return x size of the volume
 */
template<class T>
inline unsigned int svt_volume<T>::getSizeX() const
{
    return m_iSizeX;
};
/**
 * get the y size of the volume
 * \return y size of the volume
 */
template<class T>
inline unsigned int svt_volume<T>::getSizeY() const
{
    return m_iSizeY;
};
/**
 * get the z size of the volume
 * \return z size of the volume
 */
template<class T>
inline unsigned int svt_volume<T>::getSizeZ() const
{
    return m_iSizeZ;
};
/**
 * Get the number of voxels of the volume.
 * \return number of voxels of the volume
 */
template<class T>
inline unsigned int svt_volume<T>::size() const
{
    return m_iSizeX * m_iSizeY * m_iSizeZ;
};

/**
 * set position of first voxel
 */
template<class T>
inline void svt_volume<T>::setGrid( Real64 fGridX, Real64 fGridY, Real64 fGridZ )
{
    m_fGridX = fGridX;
    m_fGridY = fGridY;
    m_fGridZ = fGridZ;
};
/**
 * set X position of first voxel
 */
template<class T>
inline void svt_volume<T>::setGridX( Real64 fGridX )
{
    m_fGridX = fGridX;
};
/**
 * set Y position of first voxel
 */
template<class T>
inline void svt_volume<T>::setGridY( Real64 fGridY )
{
    m_fGridY = fGridY;
};
/**
 * set Z position of first voxel
 */
template<class T>
inline void svt_volume<T>::setGridZ( Real64 fGridZ )
{
    m_fGridZ = fGridZ;
};
/**
 * get position of first voxel
 */
template<class T>
inline Real64 svt_volume<T>::getGridX() const
{
    return m_fGridX;
};
/**
 * get position of first voxel
 */
template<class T>
inline Real64 svt_volume<T>::getGridY() const
{
    return m_fGridY;
};
/**
 * get position of first voxel
 */
template<class T>
inline Real64 svt_volume<T>::getGridZ() const
{
    return m_fGridZ;
};

/**
 * get the interpolated value at a position inside the volume
 * \param fX x coordinate
 * \param fY y coordinate
 * \param fZ z coordinate
 * \return value
 */
template<class T>
inline T svt_volume<T>::getIntValue(Real64 fX, Real64 fY, Real64 fZ) const
{
    if (fX > (Real64)(m_iSizeX-1) || fY > (Real64)(m_iSizeY-1) || fZ > (Real64)(m_iSizeZ-1) || fX < 0 || fY < 0 || fZ < 0)
	return (T)(0);

    int iV0X = (int)(floor(fX));
    int iV0Y = (int)(floor(fY));
    int iV0Z = (int)(floor(fZ));
    int iV1X = (int)(ceil(fX));
    int iV1Y = (int)(ceil(fY));
    int iV1Z = (int)(ceil(fZ));
    T fA = (T)(iV1X) - (T)(fX);
    T fB = (T)(iV1Y) - (T)(fY);
    T fC = (T)(iV1Z) - (T)(fZ);
    T fValue = (T)(0);

    // 0 0 0
    fValue += getValue( iV0X, iV0Y, iV0Z ) * (fA * fB * fC);
    // 0 0 1
    fValue += getValue( iV1X, iV0Y, iV0Z ) * (((T)(1) - fA) * fB * fC);
    // 0 1 0
    fValue += getValue( iV0X, iV1Y, iV0Z ) * (fA * ((T)(1) - fB) * fC);
    // 0 1 1
    fValue += getValue( iV1X, iV1Y, iV0Z ) * (((T)(1) - fA) * ((T)(1) - fB) * fC);
    // 1 0 0
    fValue += getValue( iV0X, iV0Y, iV1Z ) * (fA * fB * ((T)(1) - fC));
    // 1 0 1
    fValue += getValue( iV1X, iV0Y, iV1Z ) * (((T)(1) - fA) * fB * ((T)(1) - fC));
    // 1 1 0
    fValue += getValue( iV0X, iV1Y, iV1Z ) * (fA * ((T)(1) - fB) * ((T)(1) - fC));
    // 1 1 1
    fValue += getValue( iV1X, iV1Y, iV1Z ) * (((T)(1) - fA) * ((T)(1) - fB) * ((T)(1) - fC));

    return fValue;
};

/**
 * get the minimum density.
 * \return the minimum density
 */
template<class T>
inline T svt_volume<T>::getMinDensity()
{
    if (m_bChanged)
	calcMinMaxDensity();

    return m_fMinDensity;
};
/**
 * get the maximum density.
 *  \return the maximum density
 */
template<class T>
inline T svt_volume<T>::getMaxDensity()
{
    if (m_bChanged)
	calcMinMaxDensity();

    return m_fMaxDensity;
};
/**
 * get the average density.
 *  \return the average density
 */
template<class T>
inline T svt_volume<T>::getAvgDensity()
{
    if (m_bChanged)
	calcMinMaxDensity();

    return m_fAvgDensity;
};

/**
 * get norm of the volume = sqrt( sum( at(i) * at(i) ) );
 * \return norm
 */
template<class T>
inline Real64 svt_volume<T>::getNorm()
{
    if (m_bChanged)
	calcMinMaxDensity();

    return m_fNorm;
};

/**
 * set voxel width (in Angstroem)
 * \param fWidth voxel width
 */
template<class T>
inline void svt_volume<T>::setWidth( Real64 fWidth )
{
    m_fWidth = fWidth;
};
/**
 * get voxel width (in Angstroem)
 * \return voxel width
 */
template<class T>
inline Real64 svt_volume<T>::getWidth() const
{
    return m_fWidth;
};

/**
 * normalize volume
 */
template<class T>
inline void svt_volume<T>::normalize()
{
    unsigned long iNVox, iCount;
    iNVox = m_iSizeX * m_iSizeY * m_iSizeZ;

    T fMax = getMaxDensity();
    T fMin = getMinDensity();
    T fLength = fMax - fMin;

    if (fLength == (T)(0.0))
        return;

    for(iCount=0; iCount<iNVox; iCount++)
    {
	m_pData[iCount] = (m_pData[iCount] - fMin) / fLength;
    };

    m_fMinDensity = (T)(0.0);
    m_fMaxDensity = (T)(1.0);

    m_bChanged = true;
};

/**
 * normalize volume
 * \param fSigma1 - sigma used in the gaussian calculation
 * \param fSigma2 - second  sigma used in the gaussian calculation
 * \param fSigma -  blurring kernel following local normalization (default fSigma=1.0 - don't blur) 
 */
template<class T>
inline void svt_volume<T>::locallyNormalize( T fSigma, bool bProgress )
{
    svt_volume<T> oGaussian;

    if (fSigma <=2.5)
        oGaussian.createGaussian(fSigma, fSigma); 
    else
        oGaussian.createGaussian(fSigma, 2.0); 

    svtout << "Applying local normalization using a gaussian of size (" << oGaussian.getSizeX() << " x " << oGaussian.getSizeY() << " x " << oGaussian.getSizeZ() << ") " <<  endl;

    svt_volume<T> oAverage, oSd;
    oAverage    = *this;
    oSd         = *this;

    //compute the average
    oAverage.convolve(oGaussian, bProgress);
    oAverage.scale( -1.0 );

    *this += oAverage;

    //compute the standard deviation
    unsigned int iNum = oSd.getSizeX() * oSd.getSizeY() * oSd.getSizeZ();
    for(unsigned int i=0; i<iNum; i++)
        oSd.setAt(i, oSd.at(i) * oSd.at(i));

    oSd.convolve(oGaussian, bProgress);

    for(unsigned int i=0; i<iNum; i++)
    {
        oSd.setAt(i, sqrt(oSd.at(i)  - oAverage.at(i)*oAverage.at(i)));
        if (oSd.at(i)!=0)
            setAt(i, at(i)/oSd.at(i));  
    }

};
 
/**
 * normalize volume
 * \param fSigma -  blurring kernel following local normalization (default fSigma=1.0 - don't blur) 
 */
template<class T>
inline void svt_volume<T>::locallyNormalizeCorrectBorders( T fSigma, bool bProgress )
{

    long iStart =  svt_getElapsedTime(); 
    svt_volume<T> oGaussian;

    if (fSigma <= 2.5)
        oGaussian.createGaussian(fSigma, fSigma); 
    else
        oGaussian.createGaussian(fSigma, 2.0); 

    //fsigma = 1 - the map remains the same
    if (oGaussian.getSizeX() == 1 || oGaussian.getSizeY() == 1 || oGaussian.getSizeZ() == 1)
        return;

    svtout << "Applying local normalization using a gaussian of size (" << oGaussian.getSizeX() << " x " << oGaussian.getSizeY() << " x " << oGaussian.getSizeZ() << ") " <<  endl;
  
    Real64 fHW = int (oGaussian.getSizeX()/2.0);
 
    
    #ifdef _OPENMP
    int iThreads = omp_get_max_threads();
    omp_set_num_threads( iThreads );
    svtout << "Starting parallel local normalization on " << iThreads << " cores" <<  endl;
    #endif
    
    if (bProgress)
        svt_exception::ui()->progressPopup("Locally Normalize...", 0, m_iSizeZ );

    svt_volume<Real64> oLocNormVol;
    oLocNormVol = *this;
    Real64 fCount;
    Real64 fAvg, fSqAvg, fVal, fW;

    #ifdef _OPENMP
    #pragma omp parallel for \
        shared(oLocNormVol, oGaussian) \
        private( fCount, fAvg, fSqAvg, fVal, fW ) \
        schedule(dynamic, 1)
    #endif
        
    for (unsigned int iIndexZ=0; iIndexZ< m_iSizeZ; iIndexZ++)
    {  
       try
       {
            //run block in separate threads  
            for (unsigned int iIndexX=0; iIndexX< m_iSizeX; iIndexX++)
            {
                for (unsigned int iIndexY=0; iIndexY< m_iSizeY; iIndexY++)
                    {
                        #ifdef _OPENMP
                        #pragma omp critical 
                        #endif
                        //process in window for voxel iIndexX, iIndexY, iIndexZ 
                        fCount = 0;
                        fAvg = 0;
                        fSqAvg = 0;
                        for (int i = iIndexX-fHW; i <= iIndexX+fHW; i++)
                        {
                            for (int j = iIndexY-fHW; j <= iIndexY+fHW; j++)
                                {
                                    for (int k = iIndexZ-fHW; k <= iIndexZ+fHW; k++)
                                    if (i>=0 && i< m_iSizeX && j>=0 && j< m_iSizeY && k>=0 && k< m_iSizeZ)
                                    {
                                        fW = oGaussian.at(i - iIndexX + fHW, j - iIndexY+fHW, k-iIndexZ+fHW);
                                        fVal = at(i, j, k);
                                        fAvg += fVal*fW; 
                                        fSqAvg += fVal*fVal*fW;
                                        fCount += fW;
                                    }
                                }
                        }
                        fAvg /= (Real64)fCount;
                        fSqAvg /= (Real64)fCount;
                        if (abs(sqrt(fSqAvg -fAvg*fAvg))>1e-6)
                            oLocNormVol.setAt(iIndexX,iIndexY,iIndexZ, (at(iIndexX,iIndexY,iIndexZ) - fAvg )/sqrt(fSqAvg -fAvg*fAvg) ); //the sd
                        else
                            oLocNormVol.setAt(iIndexX,iIndexY,iIndexZ, (at(iIndexX,iIndexY,iIndexZ) - fAvg )); //the sd
                   }
            }
         
            #ifdef _OPENMP
            if (omp_get_thread_num() == 0)
            if (bProgress)
                svt_exception::ui()->progress( iIndexZ, m_iSizeZ );
            #endif
       } catch (svt_userInterrupt&)
       {
           	if (bProgress)
#ifdef _OPENMP				   		   	
			if (omp_get_thread_num() == 0)
			svtout << "  Cancel button pressed, please wait for all threads to stop." << endl;
#endif	
			iIndexZ=m_iSizeZ; 

            if (bProgress)
                svt_exception::ui()->progressPopdown();

       }
    }
    
    *this = oLocNormVol;
 
    if (bProgress)
        svt_exception::ui()->progressPopdown();
  
    svtout << " Elapsed time : " <<  (((svt_getElapsedTime() - iStart)/(1000.0f))/60.0f) << " min" << endl;

};


/**
 * Threshold volume. All voxel values above a threshold value are cutoff and set to the threshold.
 * \param fMaxDensity new maximum density
 * \param fMinDensity new minimum density
 */
template<class T>
inline void svt_volume<T>::threshold( T fMinDensity, T fMaxDensity )
{
    unsigned long iNVox, iCount;
    iNVox = m_iSizeX * m_iSizeY * m_iSizeZ;

    for(iCount=0; iCount<iNVox; iCount++)
    {
	if ( m_pData[iCount] > fMaxDensity )
	    m_pData[iCount] = fMaxDensity;

	if ( m_pData[iCount] < fMinDensity )
	    m_pData[iCount] = fMinDensity;
    };

    m_fMinDensity = (T)(fMinDensity);
    m_fMaxDensity = (T)(fMaxDensity);

    m_bChanged = true;
};

/**
 * Crop volume - cuts out and returns a subvolume
 * \param iNewMinX new minimum x voxel index
 * \param iNewMaxX new maximal x voxel index
 * \param iNewMinY new minimum y voxel index
 * \param iNewMaxY new maximal y voxel index
 * \param iNewMinZ new minimum z voxel index
 * \param iNewMaxZ new maximal z voxel index
 */
template<class T>
inline svt_volume<T>& svt_volume<T>::crop(
					  unsigned int iNewMinX, unsigned int iNewMaxX,
					  unsigned int iNewMinY, unsigned int iNewMaxY,
					  unsigned int iNewMinZ, unsigned int iNewMaxZ
					)
{
    svt_volume<T> oVol;
    oVol.allocate(
		  iNewMaxX - iNewMinX + 1,
		  iNewMaxY - iNewMinY + 1,
		  iNewMaxZ - iNewMinZ + 1
		 );

    for(unsigned int iZ=iNewMinZ; iZ<=iNewMaxZ; iZ++)
	for(unsigned int iY=iNewMinY; iY<=iNewMaxY; iY++)
	    for(unsigned int iX=iNewMinX; iX<=iNewMaxX; iX++)
		oVol.setAt( iX-iNewMinX, iY-iNewMinY, iZ-iNewMinZ, this->at(iX,iY,iZ) );

    Real64 fNewGridX = m_fGridX + (iNewMinX * m_fWidth);
    Real64 fNewGridY = m_fGridY + (iNewMinY * m_fWidth);
    Real64 fNewGridZ = m_fGridZ + (iNewMinZ * m_fWidth);

    // delete old data
    if (m_bInternal)
        delete[] m_pData;

    m_pData = oVol.getData();
    m_bInternal = true;

    m_fGridX = fNewGridX;
    m_fGridY = fNewGridY;
    m_fGridZ = fNewGridZ;

    m_iSizeX = oVol.getSizeX();
    m_iSizeY = oVol.getSizeY();
    m_iSizeZ = oVol.getSizeZ();

    m_bChanged = true;

    oVol.m_bInternal = false;

    return *this;
};

/**
 * Delete a spherical subregion
 * \param fCenterX center voxel coordinate for the spherical region
 * \param fCenterY center voxel coordinate for the spherical region
 * \param fCenterZ center voxel coordinate for the spherical region
 * \param fRadius radius of the sphere
 * \return returns a new svt_volume object with the spherical region cut out
 */
template<class T>
inline svt_volume<T>& svt_volume<T>::cutSphere( Real64 fCenterX, Real64 fCenterY, Real64 fCenterZ, Real64 fRadius )
{
    svt_volume<T>* pVol = new svt_volume<T>();
    pVol->deepCopy( *this );
    Real64 fX, fY, fZ;
    Real64 fRadSq = fRadius * fRadius;

    for(unsigned int iZ=0; iZ<m_iSizeZ; iZ++)
    {
        fZ = (Real64)(iZ);

	for(unsigned int iY=0; iY<m_iSizeY; iY++)
	{
	    fY = (Real64)(iY);

	    for(unsigned int iX=0; iX<m_iSizeX; iX++)
	    {
		fX = (Real64)(iX);

		if (

		    ((fX-fCenterX)*(fX-fCenterX)) +
		    ((fY-fCenterY)*(fY-fCenterY)) +
		    ((fZ-fCenterZ)*(fZ-fCenterZ))

		    < fRadSq )

		    pVol->setAt( iX, iY, iZ, 0.0 );
	    }
	}
    }

    return *pVol;
};

/**
 * copy a spherical subregion
 * \param fCenterX center voxel coordinate for the spherical region
 * \param fCenterY center voxel coordinate for the spherical region
 * \param fCenterZ center voxel coordinate for the spherical region
 * \param fRadius radius of the sphere
 * \return returns a new svt_volume object with the cutted out spherical region
 */
template<class T>
inline svt_volume<T>& svt_volume<T>::copySphere( Real64 fCenterX, Real64 fCenterY, Real64 fCenterZ, Real64 fRadius )
{
    svt_volume<T>* pVol = new svt_volume<T>();
    pVol->deepCopy( *this );

    Real64 fDist;

    svt_vector4<Real64> oCenter;
    svt_vector4<Real64> oVoxel;

    oCenter.x(( fCenterX - m_fGridX ) / m_fWidth);
    oCenter.y(( fCenterY - m_fGridY ) / m_fWidth);
    oCenter.z(( fCenterZ - m_fGridZ ) / m_fWidth);


    for(unsigned int iZ=0; iZ<m_iSizeZ; iZ++)
    {
    	for(unsigned int iY=0; iY<m_iSizeY; iY++)
    	{
    		for(unsigned int iX=0; iX<m_iSizeX; iX++)
    		{
    			oVoxel.x( (Real64) iX );
    			oVoxel.y( (Real64) iY );
    			oVoxel.z( (Real64) iZ );
    			fDist = oVoxel.distance(oCenter);

    			if ( fDist > (fRadius/m_fWidth) )
    				pVol->setAt( iX, iY, iZ, 0.0 );
			}
    	}
    }

    unsigned int iMinX = 0;
    unsigned int iMaxX = m_iSizeX-1;

    unsigned int iMinY = 0;
    unsigned int iMaxY = m_iSizeY-1;

    unsigned int iMinZ = 0;
    unsigned int iMaxZ = m_iSizeZ-1;


    if ( floor(fCenterX + m_fGridX*m_fWidth- fRadius) >= 0 &&  floor(fCenterX + m_fGridX*m_fWidth - fRadius) < m_iSizeX )
        iMinX =  floor(  (fCenterX - fRadius - m_fGridX) / m_fWidth) ;

    if ( ceil(fCenterX + m_fGridX*m_fWidth + fRadius) >= 0 &&  ceil(fCenterX + m_fGridX*m_fWidth + fRadius) < m_iSizeX )
        iMaxX =  ceil( (fCenterX + fRadius - m_fGridX ) / m_fWidth)  ;

    if ( floor(fCenterY + m_fGridY*m_fWidth- fRadius) >= 0 &&  floor(fCenterY + m_fGridY*m_fWidth - fRadius) < m_iSizeY )
        iMinY =  floor(  (fCenterY - fRadius - m_fGridY) / m_fWidth) ;

    if ( ceil(fCenterY + m_fGridY*m_fWidth + fRadius) >= 0 &&  ceil(fCenterY + m_fGridY*m_fWidth + fRadius) < m_iSizeY )
        iMaxY =  ceil( (fCenterY + fRadius - m_fGridY ) / m_fWidth)  ;

    if ( floor(fCenterZ + m_fGridZ*m_fWidth- fRadius) >= 0 &&  floor(fCenterZ + m_fGridZ*m_fWidth - fRadius) < m_iSizeZ )
       iMinZ =  floor(  (fCenterZ - fRadius - m_fGridZ) / m_fWidth) ;

    if ( ceil(fCenterZ + m_fGridZ*m_fWidth + fRadius) >= 0 &&  ceil(fCenterZ + m_fGridZ*m_fWidth + fRadius) < m_iSizeZ )
       iMaxZ =  ceil( (fCenterZ + fRadius - m_fGridZ ) / m_fWidth)  ;

    pVol->crop( iMinX, iMaxX, iMinY, iMaxY, iMinZ, iMaxZ );

    return *pVol;
};


/**
 * Mask with another svt_volume object - all the voxels in this vol are multiplied with the mask volume voxels (multiplied by 0 for not overlapping voxels).
 * \param rMask reference to other object
 */
template<class T>
svt_volume<T>& svt_volume<T>::mask( svt_volume<T>& rMask )
{
    Real64 fX, fY, fZ;
    T fVal;

    if ( abs(m_fWidth - rMask.getWidth()) > 1e-6 )// || fmod((m_fGridX - rMask.getGridX()) , m_fWidth) != 0.0 || fmod((m_fGridY - rMask.getGridY()), m_fWidth) != 0.0 || fmod((m_fGridZ - rMask.getGridZ()), m_fWidth) != 0.0)
    {
	//if (m_fWidth != rMask.getWidth())
	//{
	    svtout << "mask(): Interpolation necessary as voxelwidth is different!" << endl;
	    svtout << "        Voxelwidth: " << m_fWidth << " Mask: " << rMask.getWidth() << endl;
	//}
	//else
	//{
	//    svtout << "mask(): Interpolation necessary as origin does not lie on the same lattice!" << endl;
	//    svtout << "        Origin: " << m_fGridX << ", " << m_fGridY << ", " << m_fGridZ << " Mask: " << rMask.getGridX() << ", " << rMask.getGridY() << ", " << rMask.getGridZ() << endl;
	//    svtout << "DEBUG: " << fmod((m_fGridX - rMask.getGridX()) , m_fWidth) << ", " << fmod((m_fGridY - rMask.getGridY()) , m_fWidth) << ", " << fmod((m_fGridZ - rMask.getGridZ()) , m_fWidth) << endl;
	//}

	for(unsigned int iZ=0; iZ<m_iSizeZ; iZ++)
	{
	    fZ = (Real64)(iZ);
	    fZ *= m_fWidth;
	    fZ += m_fGridZ;

	    for(unsigned int iY=0; iY<m_iSizeY; iY++)
	    {
		fY = (Real64)(iY);
		fY *= m_fWidth;
		fY += m_fGridY;

		for(unsigned int iX=0; iX<m_iSizeX; iX++)
		{
		    fX = (Real64)(iX);
		    fX *= m_fWidth;
		    fX += m_fGridX;

		    fVal = this->at(iX, iY, iZ ) * rMask.getRealSpaceValue( fX, fY, fZ );
		    this->setAt( iX, iY, iZ, fVal );
		}
	    }
	}

    shrinkToOccupiedVolume();

    } else {

        int iStartX = (int)((rMask.getGridX() - m_fGridX) / m_fWidth);
        int iStartY = (int)((rMask.getGridY() - m_fGridY) / m_fWidth);
        int iStartZ = (int)((rMask.getGridZ() - m_fGridZ) / m_fWidth);

	int iMinX = (int)m_iSizeX;
	int iMaxX = 0;
	int iMinY = (int)m_iSizeY;
	int iMaxY = 0;
	int iMinZ = (int)m_iSizeZ;
	int iMaxZ = 0;

	for(int iZ=0; iZ<(int)m_iSizeZ; iZ++)
	{
	    for(int iY=0; iY<(int)m_iSizeY; iY++)
	    {
		for(int iX=0; iX<(int)m_iSizeX; iX++)
		{
		    if (iZ < iStartZ || iY < iStartY || iX < iStartX || iX - iStartX >= (int)rMask.getSizeX() || iY - iStartY >= (int)rMask.getSizeY() || iZ - iStartZ >= (int)rMask.getSizeZ() )
            {
            	fVal = 0.0;
				this->setAt( iX, iY, iZ, 0.0 );
		    }
            else
		    {
			fVal = this->at(iX, iY, iZ ) * rMask.getValue( iX - iStartX, iY - iStartY, iZ - iStartZ );
			this->setAt( iX, iY, iZ, fVal );

			if (fVal != 0.0)
			{
			    if (iX > iMaxX)
					iMaxX = iX;
			    if (iX < iMinX)
                    iMinX = iX;

			    if (iY > iMaxY)
					iMaxY = iY;
			    if (iY < iMinY)
                    iMinY = iY;

			    if (iZ > iMaxZ)
					iMaxZ = iZ;
			    if (iZ < iMinZ)
					iMinZ = iZ;
			}
		    }
		}
	    }
	}

		iMinX = iMinX-1>=0?iMinX-1:0;
		iMinY = iMinY-1>=0?iMinY-1:0;
		iMinZ = iMinZ-1>=0?iMinZ-1:0;
	
		iMaxX = iMaxX+1<(int)getSizeX()? iMaxX+1:iMaxX;
		iMaxY = iMaxY+1<(int)getSizeY()? iMaxY+1:iMaxY;
		iMaxZ = iMaxZ+1<(int)getSizeZ()? iMaxZ+1:iMaxZ;

		//svtout << "New volume size: " << iMaxX - iMinX << " x " << iMaxY - iMinY << " x " << iMaxZ - iMinZ << endl;
		//svtout << "New volume size: " << iMaxX  << " " <<   iMinX << " x " << iMaxY  << " " <<  iMinY << " x " << iMaxZ  << " " <<  iMinZ << endl;
        //crop( iMinX, iMaxX, iMinY, iMaxY, iMinZ, iMaxZ );
    }

    m_bChanged = true;

    return *this;
};

/**
 * Make a mask volume - if voxel > threshold it is set to 1, otherwise 0
 * \param fThreshold threshold value
 */
template<class T>
svt_volume<T>& svt_volume<T>::makeMask( T fThreshold )
{
    unsigned int iNum = m_iSizeX * m_iSizeY * m_iSizeZ;

    for(unsigned int i=0; i<iNum; i++)
    {
	if (this->at(i) > fThreshold)
	    this->setAt(i, 1.0);
	else
            this->setAt(i, 0.0);
    }

    return *this;
};

/**
 * Invert mask - all voxels have to be between 0 and 1. The voxel values are inverted.
 */
template<class T>
svt_volume<T>& svt_volume<T>::invertMask()
{
    unsigned int iNum = m_iSizeX * m_iSizeY * m_iSizeZ;
    T fValue;

    for(unsigned int i=0; i<iNum; i++)
    {
	fValue = this->at(i);

	this->setAt(i, (T)(1.0) - fValue);
    }

    return *this;
};

/**
 * shrinks the original volume to the occupied Volume - redefine m_fGrid and m_iSize such that volume is not padded by zeros
 */
template<class T>
void svt_volume<T>::shrinkToOccupiedVolume()
{
    unsigned int iNewMinX = 0;
    unsigned int iNewMaxX = m_iSizeX;
    unsigned int iNewMinY = 0;
    unsigned int iNewMaxY = m_iSizeY;
    unsigned int iNewMinZ = 0;
    unsigned int iNewMaxZ = m_iSizeZ;

    iNewMinX = getNewBorder(0           , 1         , 0         , m_iSizeY  , 0         , m_iSizeZ);
    iNewMaxX = getNewBorder(m_iSizeX-1  , m_iSizeX  , 0         , m_iSizeY  , 0         , m_iSizeZ);

    iNewMinY = getNewBorder(0           , m_iSizeX  , 0         , 1         , 0         , m_iSizeZ);
    iNewMaxY = getNewBorder(0           , m_iSizeX  , m_iSizeY-1, m_iSizeY  , 0         , m_iSizeZ);

    iNewMinZ = getNewBorder(0           , m_iSizeX  , 0         , m_iSizeY  , 0         , 1);
    iNewMaxZ = getNewBorder(0           , m_iSizeX  , 0         , m_iSizeY  , m_iSizeZ-1, m_iSizeZ);

    crop(iNewMinX, iNewMaxX,iNewMinY, iNewMaxY, iNewMinZ, iNewMaxZ);
}

/**
 * get the new border
 * \param iMinX the min value of X
 * \param iMaxX the max value of X
 * \param iMinY the min value of Y
 * \param iMaxY the max value of Y
 * \param iMinZ the min value of Z
 * \param iMaxZ the max value of Z
 * \return the new index (voxel value)
 */
template<class T>
unsigned int svt_volume<T>::getNewBorder(
                                            unsigned int iMinX, unsigned int iMaxX,
                                            unsigned int iMinY, unsigned int iMaxY,
                                            unsigned int iMinZ, unsigned int iMaxZ
                                            )
{
    bool bIsNull;
    unsigned int iNewBorder = 0;
    int iIncrement=+1;

    //should go downwards
    if ((iMinX+1==iMaxX && iMinX+1==m_iSizeX) || (iMinY+1==iMaxY && iMinY+1==m_iSizeY) || (iMinZ+1==iMaxZ && iMinZ+1==m_iSizeZ) )
        iIncrement = -1;

    do
    {
        bIsNull = true;
        for (unsigned int iX=iMinX; iX<iMaxX; iX++)
            for (unsigned int iY=iMinY; iY<iMaxY; iY++)
                 for (unsigned int iZ=iMinZ; iZ<iMaxZ; iZ++)
                    if (this->getValue(iX, iY, iZ)!=0.0)
                        bIsNull = false;

        if (iMinX+1==iMaxX)
        {
            iNewBorder = iMinX;
            iMinX+=iIncrement;
            iMaxX+=iIncrement;
        }

        if (iMinY+1==iMaxY)
        {
            iNewBorder = iMinY;
            iMinY+=iIncrement;
            iMaxY+=iIncrement;
        }

        if (iMinZ+1==iMaxZ)
        {
            iNewBorder = iMinZ;
            iMinZ+=iIncrement;
            iMaxZ+=iIncrement;
        }
        //svtout << "DEBUG: " <<  iMinX  << "-" << iMaxX  << " " <<  iMinY  << "-" << iMaxY  <<  " " << iMinZ  << "-" << iMaxZ << " " << iNewBorder << endl;
    } while (bIsNull && (int)iMinX>=0 && iMaxX<=m_iSizeX && (int)iMinY>=0 && iMaxY<=m_iSizeY && (int)iMinZ>=0 && iMaxZ<=m_iSizeZ );

    return iNewBorder;
}

/**
 * change voxel spacings and bring map origin into register with coordinate system origin
 */
template<class T>
void svt_volume<T>::interpolate_map( Real64 fWidthX, Real64 fWidthY, Real64 fWidthZ )
{
    if (fWidthY == 0)
        fWidthY = fWidthX;
    if (fWidthZ == 0)
        fWidthZ = fWidthX;

    Real64 fDeltaX, fDeltaY, fDeltaZ;
    Real64 fXpos, fYpos, fZpos, fGx, fGy, fGz, fA, fB, fC;
    int iX0, iY0, iZ0, iX1, iY1, iZ1;
    int iSx, iSy, iSz, iEx, iEy, iEz;

    /* output start index rel. to coordinate system origin, asserting that outmap is fully embedded in inmap */
    iSx = ceil (m_fGridX/fWidthX);
    iSy = ceil (m_fGridY/fWidthY);
    iSz = ceil (m_fGridZ/fWidthZ);

    /* output end index rel. to coordinate system origin, asserting that outmap is fully embedded in inmap */
    iEx = floor ((m_fGridX+m_fWidth*(m_iSizeX-1))/fWidthX);
    iEy = floor ((m_fGridY+m_fWidth*(m_iSizeY-1))/fWidthY);
    iEz = floor ((m_fGridZ+m_fWidth*(m_iSizeZ-1))/fWidthZ);

    /* assign output grid size */
    int iSizeX = iEx-iSx+1;
    int iSizeY = iEy-iSy+1;
    int iSizeZ = iEz-iSz+1;
    if ( iSizeX < 2 || iSizeY < 2 || iSizeZ < 2)
    {
	svt_exception::ui()->error("Interpolation output map size underflow!");
	exit(1);
    }

    svt_volume<T> oVol(iSizeX, iSizeY, iSizeZ);
    oVol.setWidth( fWidthX );

    /* save origin shift */
    fDeltaX = iSx*fWidthX-m_fGridX;
    fDeltaY = iSy*fWidthY-m_fGridY;
    fDeltaZ = iSz*fWidthZ-m_fGridZ;

    Real64 fNewDensity;
    for (int iZ=0;iZ<iSizeZ;iZ++)
    {
	for (int iY=0;iY<iSizeY;iY++)
	{
	    for (int iX=0;iX<iSizeX;iX++)
	    {

		/* determine position of outmap voxel relative to start of inmap */
		fXpos = fDeltaX + iX * fWidthX;
		fYpos = fDeltaY + iY * fWidthY;
		fZpos = fDeltaZ + iZ * fWidthZ;

		/* compute position in inmap voxel units */
		fGx = (fXpos/m_fWidth);
		fGy = (fYpos/m_fWidth);
		fGz = (fZpos/m_fWidth);

		/* compute bounding box voxel indices and linear distances */
		iX0 = floor (fGx);
		iY0 = floor (fGy);
		iZ0 = floor (fGz);
		iX1 = ceil (fGx);
		iY1 = ceil (fGy);
		iZ1 = ceil (fGz);
		fA = fGx-iX0;
		fB = fGy-iY0;
		fC = fGz-iZ0;

		/* interpolate */
		fNewDensity =
		    fA 		* fB 		* fC 		* this->getValue(iX1, iY1, iZ1) +
		    (1-fA) 	* fB 		* fC 		* this->getValue(iX0, iY1, iZ1) +
		    fA 		* (1-fB) 	* fC 		* this->getValue(iX1, iY0, iZ1) +
		    fA 		* fB 		* (1-fC) 	* this->getValue(iX1, iY1, iZ0) +
		    fA 		* (1-fB) 	* (1-fC) 	* this->getValue(iX1, iY0, iZ0) +
		    (1-fA) 	* fB 		* (1-fC) 	* this->getValue(iX0, iY1, iZ0) +
		    (1-fA) 	* (1-fB) 	* fC 		* this->getValue(iX0, iY0, iZ1) +
		    (1-fA) 	* (1-fB) 	* (1-fC) 	* this->getValue(iX0, iY0, iZ0);

		oVol.setAt(iX, iY, iZ, fNewDensity );
	    }
	 }
    }

    oVol.setGrid( m_fGridX + fDeltaX, m_fGridY + fDeltaY , m_fGridZ + fDeltaZ);
    *this = oVol;

    m_bChanged = true;
}


/**
 * Calc gradient map.
 * \param eMode gradient method selector: CentralDistance or Sobel
 * \param rGradientX reference to an svt_volume object for the gradient map in x direction
 * \param rGradientY reference to an svt_volume object for the gradient map in y direction
 * \param rGradientZ reference to an svt_volume object for the gradient map in z direction
 */
template<class T>
inline void svt_volume<T>::calcGradient( const GradientMode eMode, svt_volume<T>& rGradientX, svt_volume<T>& rGradientY, svt_volume<T>& rGradientZ ) const
{
    rGradientX.allocate(m_iSizeX, m_iSizeY, m_iSizeZ);
    rGradientY.allocate(m_iSizeX, m_iSizeY, m_iSizeZ);
    rGradientZ.allocate(m_iSizeX, m_iSizeY, m_iSizeZ);

    unsigned int iX, iY, iZ;
    T fValX, fValY, fValZ;

    int iTime = svt_getElapsedTime();
    svtout << "Gradient calculation..." << endl;

    // central difference gradient
    if (eMode == CentralDistance)
    {
	for(iZ=0; iZ<=m_iSizeZ; iZ++)
	    for(iY=0; iY<=m_iSizeY; iY++)
		for(iX=0; iX<=m_iSizeX; iX++)
		{
		    if (iX != 0 && iX != m_iSizeX)
			fValX = (getValue(iX+1, iY, iZ) - getValue(iX-1, iY, iZ))/2;
		    else
			fValX = 0.0f;
		    rGradientX.setValue(iX, iY, iZ, fValX);

		    if (iY != 0 && iY != m_iSizeY)
			fValY = (getValue(iX, iY+1, iZ) - getValue(iX, iY-1, iZ))/2;
		    else
			fValY = 0.0f;
		    rGradientY.setValue(iX, iY, iZ, fValY);

		    if (iZ != 0 && iZ != m_iSizeZ)
			fValZ = (getValue(iX, iY, iZ+1) - getValue(iX, iY, iZ-1))/2;
		    else
			fValZ = 0.0f;
		    rGradientZ.setValue(iX, iY, iZ, fValZ);
		}

    }

    // sobel
    if (eMode == Sobel)
    {
	svt_vector4<Real64> oVec;
	int iKX, iKY, iKZ;
	// sobel kernels (fastest running index = z, then y and then x!!)
	Real64 xKernel[27] =
	{
	    1, 3, 1,
	    3, 6, 3,
	    1, 3, 1,
	    0, 0, 0,
	    0, 0, 0,
	    0, 0, 0,
	    -1, -3, -1,
	    -3, -6, -3,
	    -1, -3, -1
	};
	Real64 yKernel[27] =
	{
	    1, 3, 1,
	    0, 0, 0,
	    -1, -3, -1,
	    3, 6, 3,
	    0, 0, 0,
	    -3, -6, -3,
	    1, 3, 1,
	    0, 0, 0,
	    -1, -3, -1
	};
	Real64 zKernel[27] =
	{
	    1, 0, -1,
	    3, 0, -3,
	    1, 0, -1,
	    3, 0, -3,
	    6, 0, -6,
	    3, 0, -3,
	    1, 0, -1,
	    3, 0, -3,
	    1, 0, -1
	};

	for(iZ=0; iZ<m_iSizeZ; iZ++)
	{
	    for(iX=0; iX<m_iSizeX; iX++)
	    {
		for(iY=0; iY<m_iSizeY; iY++)
		{
		    // apply x kernel
		    Real64 xVal = 0.0;
		    for(iKZ = -1; iKZ < 2; iKZ++)
			for(iKY = -1; iKY < 2; iKY++)
			    for(iKX = -1; iKX < 2; iKX++)
			    {
				if ((((int)(iX)+iKX) < (int)(m_iSizeX) && ((int)(iX)+iKX) >= 0) && (((int)(iY)+iKY) < (int)(m_iSizeY) && ((int)(iY)+iKY) >= 0) && (((int)(iZ)+iKZ) < (int)(m_iSizeZ) && ((int)(iZ)+iKZ) >= 0))
				    xVal += xKernel[((iKX+1)*9)+((iKY+1)*3)+iKZ+1] * Real64(m_pData[(iX+iKX)+((iY+iKY)*m_iSizeX)+((iZ+iKZ)*m_iSizeX*m_iSizeY)]);
			    }

		    // apply y kernel
		    Real64 yVal = 0.0;
		    for(iKZ = -1; iKZ < 2; iKZ++)
			for(iKY = -1; iKY < 2; iKY++)
			    for(iKX = -1; iKX < 2; iKX++)
			    {
				if ((((int)(iX)+iKX) < (int)(m_iSizeX) && ((int)(iX)+iKX) >= 0) && (((int)(iY)+iKY) < (int)(m_iSizeY) && ((int)(iY)+iKY) >= 0) && (((int)(iZ)+iKZ) < (int)(m_iSizeZ) && ((int)(iZ)+iKZ) >= 0))
				    yVal += yKernel[((iKX+1)*9)+((iKY+1)*3)+iKZ+1] * Real64(m_pData[(iX+iKX)+((iY+iKY)*m_iSizeX)+((iZ+iKZ)*m_iSizeX*m_iSizeY)]);
			    }

		    // apply z kernel
		    Real64 zVal = 0.0;
		    for(iKX = -1; iKX < 2; iKX++)
			for(iKY = -1; iKY < 2; iKY++)
			    for(iKZ = -1; iKZ < 2; iKZ++)
			    {
				if ((((int)(iX)+iKX) < (int)(m_iSizeX) && ((int)(iX)+iKX) >= 0) && (((int)(iY)+iKY) < (int)(m_iSizeY) && ((int)(iY)+iKY) >= 0) && (((int)(iZ)+iKZ) < (int)(m_iSizeZ) && ((int)(iZ)+iKZ) >= 0))
				    zVal += zKernel[((iKX+1)*9)+((iKY+1)*3)+iKZ+1] * Real64(m_pData[(iX+iKX)+((iY+iKY)*m_iSizeX)+((iZ+iKZ)*m_iSizeX*m_iSizeY)]);
			    }

		    oVec.x( xVal );
		    oVec.y( yVal );
		    oVec.z( zVal );
		    oVec.w( 0.0 );

		    if (iX == 0 || iY == 0 || iZ == 0 || iX == m_iSizeX-1 || iY == m_iSizeY-1 || iZ == m_iSizeZ-1)
		    {
			oVec.x( 1.0 );
			oVec.y( 1.0 );
			oVec.z( 1.0 );
		    }

		    rGradientX.setValue( iX, iY, iZ, oVec.x() );
		    rGradientY.setValue( iX, iY, iZ, oVec.y() );
		    rGradientZ.setValue( iX, iY, iZ, oVec.z() );
		}
	    }
	}
    }

    iTime = svt_getElapsedTime() - iTime;
    svtout << "Gradient runtime: " << iTime << endl;
};

/**
 * Set densities below limit to zero
 * \param fLimit the threshold
 */
template<class T>
inline void svt_volume<T>::threshold( T fLimit )
{
    unsigned int i;
    unsigned int iNVox = m_iSizeX * m_iSizeY * m_iSizeZ;

    for ( i=0; i<iNVox; i++ )
	if (m_pData[i] < fLimit)
	    m_pData[i] = (T)(0.0);

    m_bChanged = true;
};

/**
 * Convolve this volume with another one, which is a 1D volume (only x axis). The 1D kernel will get convolved in all three directions, so this function should be
 * used with linear separable kernels.
 * \param rKernel reference to kernel volume
 * \param bNormalize normalizes during convolution
 * \param bProgress show a progress bar
 */
template<class T>
inline void svt_volume<T>::convolve1D3D( svt_volume<T>& rKernel, bool bProgress )
{
    unsigned int i, iX, iY, iZ;
    int iKX;
    T fVal;

    // allocate memory for temporary volume
    svt_volume<T> oTmp;
    oTmp.allocate( m_iSizeX, m_iSizeY, m_iSizeZ, 0.0 );
    // set the current volume to changed
    m_bChanged = true;

    // calculate dimension of kernel volume
    int iDim = (int)((Real32)(rKernel.getSizeX()) * 0.5f);
    int iStart = -iDim;
    int iEnd = iDim+1;

    try
    {
	if (bProgress)
	    svt_exception::ui()->progressPopup("Convolution with a Kernel...", 0, m_iSizeZ );

        // in x direction
	for(iZ=0; iZ<m_iSizeZ; iZ++)
	{
	    for(iX=0; iX<m_iSizeX; iX++)
	    {
		for(iY=0; iY<m_iSizeY; iY++)
		{
		    fVal = m_pData[(iX)+(iY*m_iSizeX)+(iZ*m_iSizeX*m_iSizeY)];

                    if (fVal != (T)(0))
                    {
                        for(iKX = iStart; iKX < iEnd; iKX++)
                            if ( ((int)(iX)+iKX) < (int)(m_iSizeX) && ((int)(iX)+iKX) >= 0 )
                                oTmp.setAt( iX+iKX, iY, iZ, oTmp.at( iX+iKX, iY, iZ ) + (rKernel.at(iKX-iStart) * fVal));
                    }
                }
            }

            if (bProgress)
	        svt_exception::ui()->progress( iZ, m_iSizeZ );

	}

        // set target volume to 0
        for(i=0;i<m_iSizeX*m_iSizeY*m_iSizeZ;i++)
            m_pData[i] = 0;

        // in y direction
	for(iZ=0; iZ<m_iSizeZ; iZ++)
	{
	    for(iX=0; iX<m_iSizeX; iX++)
	    {
		for(iY=0; iY<m_iSizeY; iY++)
		{
		    fVal = oTmp.at((iX)+(iY*m_iSizeX)+(iZ*m_iSizeX*m_iSizeY));

                    if (fVal != (T)(0))
                    {
                        for(iKX = iStart; iKX < iEnd; iKX++)
                            if ( ((int)(iY)+iKX) < (int)(m_iSizeY) && ((int)(iY)+iKX) >= 0 )
                                this->setAt( iX, iY+iKX, iZ, this->at( iX, iY+iKX, iZ ) + (rKernel.at(iKX-iStart) * fVal));
                    }
                }
            }

            if (bProgress)
	        svt_exception::ui()->progress( iZ, m_iSizeZ );

	}

        oTmp.setValue(0.0);

        // in z direction
	for(iZ=0; iZ<m_iSizeZ; iZ++)
	{
	    for(iX=0; iX<m_iSizeX; iX++)
	    {
		for(iY=0; iY<m_iSizeY; iY++)
		{
		    fVal = this->at((iX)+(iY*m_iSizeX)+(iZ*m_iSizeX*m_iSizeY));

                    if (fVal != (T)(0))
                    {
                        for(iKX = iStart; iKX < iEnd; iKX++)
                            if ( ((int)(iZ)+iKX) < (int)(m_iSizeZ) && ((int)(iZ)+iKX) >= 0 )
                                oTmp.setAt( iX, iY, iZ+iKX, oTmp.at( iX, iY, iZ+iKX ) + (rKernel.at(iKX-iStart) * fVal) );
                    }
                }
            }

            if (bProgress)
	        svt_exception::ui()->progress( iZ, m_iSizeZ );

	}

        memcpy( this->m_pData, oTmp.getData(), sizeof(T)*m_iSizeX*m_iSizeY*m_iSizeZ );

        if (bProgress)
	    svt_exception::ui()->progressPopdown();

    } catch (svt_userInterrupt&)
    {
	svt_exception::ui()->progressPopdown();
    }
};

/**
 * Convolve this volume with another one (typically a 3D kernel filter)
 * \param rKernel reference to kernel volume
 * \param bNormalize normalizes during convolution
 * \param bProgress show a progress bar
 */
template<class T>
inline void svt_volume<T>::convolve( svt_volume<T>& rKernel, bool bProgress)
{
    unsigned int iSize = rKernel.getSizeX();
    unsigned int iSizeS = iSize * iSize;
    unsigned int iX, iY, iZ;
    int iKX, iKY, iKZ;
    Real64 fVal;

    Real64 fOldWidth = m_fWidth;
    Real64 fOldGridX = m_fGridX;
    Real64 fOldGridY = m_fGridY;
    Real64 fOldGridZ = m_fGridZ;

    T* pData = m_pData;
    bool bInternal = m_bInternal;
    m_pData = NULL;
    m_bInternal = false;

    svt_volume<T> oTmp;
    oTmp.allocate( m_iSizeX, m_iSizeY, m_iSizeZ, 0.0 );

    m_pData = pData;
    bInternal = bInternal;
    m_bChanged = true;

    // calculate dimension of kernel volume
    int iDim = (int)((Real32)(rKernel.getSizeX()) * 0.5f);
    int iStart = -iDim;
    int iEnd = iDim+1;

    try
    {
	if (bProgress)
	    svt_exception::ui()->progressPopup("Convolution with a Kernel...", 0, m_iSizeZ );

	for(iZ=0; iZ<m_iSizeZ; iZ++)
	{
	    for(iX=0; iX<m_iSizeX; iX++)
	    {
		for(iY=0; iY<m_iSizeY; iY++)
		{
		    fVal = m_pData[(iX)+(iY*m_iSizeX)+(iZ*m_iSizeX*m_iSizeY)];

                    if (fVal != (T)(0))
                    {
                        for(iKZ = iStart; iKZ < iEnd; iKZ++)
                            for(iKY = iStart; iKY < iEnd; iKY++)
                                for(iKX = iStart; iKX < iEnd; iKX++)
                                {
                                    if ((((int)(iX)+iKX) < (int)(m_iSizeX) && ((int)(iX)+iKX) >= 0) && (((int)(iY)+iKY) < (int)(m_iSizeY) && ((int)(iY)+iKY) >= 0) && (((int)(iZ)+iKZ) < (int)(m_iSizeZ) && ((int)(iZ)+iKZ) >= 0))
                                        oTmp.setAt( iX+iKX, iY+iKY, iZ+iKZ, oTmp.at( iX+iKX, iY+iKY, iZ+iKZ ) + (rKernel.at(((iKX-iStart)*iSizeS)+((iKY-iStart)*iSize)+iKZ-iStart) * fVal));
                                }
                    }

                }
            }

            if (bProgress)
	        svt_exception::ui()->progress( iZ, m_iSizeZ );

	}
	if (bProgress)
	    svt_exception::ui()->progressPopdown();

	*this = oTmp;

	m_bChanged = true;

	// store map properties of old map in oTmp
	m_fWidth = fOldWidth;
	m_fGridX = fOldGridX;
	m_fGridY = fOldGridY;
	m_fGridZ = fOldGridZ;

    } catch (svt_userInterrupt&)
    {
	svt_exception::ui()->progressPopdown();
    }

};

/**
 * Create a Laplacian kernel volume
 */
template<class T>
void svt_volume<T>::createLaplacian()
{
    T aLap[3][3][3] =
    { { { 0,  0,      0},  {      0,  1/12.,      0},  { 0,      0, 0} },
    { { 0,  1/12.,  0},  {  1/12., -6/12., 1/12.0},  { 0,  1/12., 0} },
    { { 0,  0,      0},  {      0,  1/12.,      0},  { 0,      0, 0} } };

    deepCopy(3,3,3, (T*)aLap);
};

/**
 * Create a Gaussian kernel volume within SigmaFactor*fSigma
 * \param fSigma1D sigma of map
 * \param fSigmaFactor sigma factor
 */
template<class T>
void svt_volume<T>::createGaussian( double fSigma1D, double fSigmaFactor )
{
    // truncate at fSigmaFactor * fSigma1D
    unsigned int iSizeH = (int) ceil( fSigmaFactor * fSigma1D );
    unsigned int iSize = 2 * iSizeH - 1;

    allocate( iSize, iSize, iSize );

    // write Gaussian within fSigmaFactor * fSigma1D
    Real64 fBValue = -1 / (2.0*fSigma1D*fSigma1D);
    Real64 fCValue = fSigmaFactor*fSigmaFactor * fSigma1D*fSigma1D;
    Real64 fScale =0;
    Real64 fDSqu;

    unsigned int iX, iY, iZ;
    for (iZ=0; iZ<iSize; iZ++)
	for (iY=0; iY<iSize; iY++)
	    for (iX=0; iX<iSize; iX++)
	    {
		fDSqu = (iX-iSizeH+1)*(iX-iSizeH+1)+
		        (iY-iSizeH+1)*(iY-iSizeH+1)+
		        (iZ-iSizeH+1)*(iZ-iSizeH+1);

		if (fDSqu <= fCValue)
		    setValue( iX, iY, iZ, exp(fDSqu * fBValue ) );

		fScale += getValue( iX, iY, iZ );
	    }

    for (iZ=0; iZ<iSize; iZ++)
	for (iY=0; iY<iSize; iY++)
	    for (iX=0; iX<iSize; iX++)
		setValue( iX, iY, iZ, getValue( iX, iY, iZ ) / fScale );
};

/**
 * Create a Gaussian blurring kernel volume (Situs scheme)
 * \param fWidth the voxel width of the target map one wants to convolve with the kernel
 * \param fResolution the target resolution
 * \param fVarp variance of map (if 0 no correction for lattice interpolation smoothing effects = default)
 * Attention: This will overwrite the current content of the volume object with the filter kernel. It will allocate the memory internally.
 */
template<class T>
void svt_volume<T>::createSitusBlurringKernel( Real64 fWidth, Real64 fResolution, Real64 fVarp)
{
    Real64 fSigma1 = fResolution / 2.0;
    Real64 fKmsd = fSigma1*fSigma1/(fWidth*fWidth);

    fResolution = 2.0*sqrt((fSigma1*fSigma1) + (fVarp*fWidth*fWidth));

    Real64 fVarmap = fKmsd;
    fVarmap -= fVarp;

    if (fVarmap < 0.0)
    {
	svtout << "Error: lattice smoothing exceeds kernel size" << endl;
	exit(1);
    }

    // sigma-1D
    Real64 fSigmaMap = sqrt(fVarmap / 3.0);

    // truncate at 3 sigma-1D == sqrt(3) sigma-3D
    unsigned int iSizeH = (int) ceil( 3.0*fSigmaMap );
    unsigned int iSize = 2 * iSizeH + 1;

    allocate( iSize, iSize, iSize );
    setValue( 0.0 );

    // kernel width
    setWidth( fWidth );

    // write Gaussian within 3 sigma-1D to map
    Real64 fBValue = -1.0 / ( 2.0*fSigmaMap*fSigmaMap );
    Real64 fCValue = 9.0 * fSigmaMap * fSigmaMap;
    Real64 fScale =0;
    Real64 fDSqu;

    unsigned int iX, iY, iZ;
    for (iZ=0; iZ<iSize; iZ++)
	for (iY=0; iY<iSize; iY++)
	    for (iX=0; iX<iSize; iX++)
	    {

		fDSqu = (iX-iSizeH)*(iX-iSizeH)+
		        (iY-iSizeH)*(iY-iSizeH)+
		        (iZ-iSizeH)*(iZ-iSizeH);

		if (fDSqu <= fCValue)
		    setValue( iX, iY, iZ, exp(fDSqu * fBValue ) );

		fScale += getValue( iX, iY, iZ );
	    }

    for (iZ=0; iZ<iSize; iZ++)
	for (iY=0; iY<iSize; iY++)
	    for (iX=0; iX<iSize; iX++)
		setValue( iX, iY, iZ, getValue( iX, iY, iZ ) / fScale );
};

/**
 * Create a one-dimensional Gaussian blurring kernel volume (Situs scheme)
 * \param fWidth the voxel width of the target map one wants to convolve with the kernel
 * \param fResolution the target resolution
 * \param fVarp variance of map (if 0 no correction for lattice interpolation smoothing effects = default)
 * Attention: This will overwrite the current content of the volume object with the filter kernel. It will allocate the memory internally.
 */
template<class T>
void svt_volume<T>::create1DBlurringKernel( Real64 fWidth, Real64 fResolution, Real64 fVarp)
{
    Real64 fSigma1 = fResolution / 2.0;
    Real64 fKmsd = fSigma1*fSigma1 / (fWidth*fWidth);

    fResolution = 2.0*sqrt((fSigma1*fSigma1) + (fVarp*fWidth*fWidth));

    Real64 fVarmap = fKmsd;
    fVarmap -= fVarp;

    if (fVarmap < 0.0)
    {
        svtout << "Error: lattice smoothing exceeds kernel size" << endl;
        exit(1);
    }

    // sigma-1D
    Real64 fSigmaMap = sqrt(fVarmap / 3.0);

    // truncate at 3 sigma-1D == sqrt(3) sigma-3D
    unsigned int iSizeH = (int) ceil( 3.0*fSigmaMap );
    unsigned int iSize = 2 * iSizeH + 1;

    allocate( iSize, 1, 1 );
    setValue( 0.0 );

    // kernel width
    setWidth( fWidth );

    // write Gaussian within 3 sigma-1D to map
    Real64 fBValue = -1.0 / ( 2.0*fSigmaMap*fSigmaMap );
    Real64 fCValue = 9.0 * fSigmaMap * fSigmaMap;
    Real64 fScale =0;
    Real64 fDSqu;

    unsigned int iX;
    for (iX=0; iX<iSize; iX++)
    {
        fDSqu = (iX-iSizeH)*(iX-iSizeH);

        if (fDSqu <= fCValue)
            setValue( iX, 0, 0, exp(fDSqu * fBValue ) );

        fScale += getValue( iX, 0, 0 );
    }

    for (iX=0; iX<iSize; iX++)
        setValue( iX, 0, 0, getValue( iX, 0, 0 ) / fScale );
};

/**
 * Create a Laplacian of a Gaussian kernel volume.
 * \param fWidth the voxel width of the target map one wants to convolve with the kernel
 * \param fResolution the target resolution
 * \param fVarp variance of map (if 0 no correction for lattice interpolation smoothing effects = default)
 * Attention: This will overwrite the current content of the volume object with the filter kernel. It will allocate the memory internally.
 */
template<class T>
void svt_volume<T>::createLaplacianOfGaussian( Real64 fWidth, Real64 fResolution, Real64 fVarp )
{
    Real64 fSigma1 = fResolution / 2.0;
    Real64 fKmsd = fSigma1*fSigma1/(fWidth*fWidth);

    fResolution = 2.0*sqrt((fSigma1*fSigma1) + (fVarp*fWidth*fWidth));

    Real64 fVarmap = fKmsd;
    fVarmap -= fVarp;

    if (fVarmap < 0.0)
    {
	svtout << "Error: lattice smoothing exceeds kernel size" << endl;
	exit(1);
    }

    // sigma-1D
    Real64 fSigmaMap = sqrt(fVarmap / 3.0);

    // truncate at 3 sigma-1D == sqrt(3) sigma-3D
    unsigned int iSizeH = (int) ceil( 3.0*fSigmaMap );
    unsigned int iSize = 2 * iSizeH + 1;

    allocate( iSize, iSize, iSize );
    setValue( 0.0 );

    // kernel width
    setWidth( fWidth );

    // write Gaussian within 3 sigma-1D to map
    Real64 fBValue = -1.0 / ( 2.0*fSigmaMap*fSigmaMap );
    Real64 fCValue = 9.0 * fSigmaMap * fSigmaMap;
    Real64 fScale =0;
    Real64 fDSqu;
    Real64 fLapl = -1.0 / (PI*fSigmaMap*fSigmaMap*fSigmaMap*fSigmaMap );
    Real64 fTerm;

    unsigned int iX, iY, iZ;
    for (iZ=0; iZ<iSize; iZ++)
	for (iY=0; iY<iSize; iY++)
	    for (iX=0; iX<iSize; iX++)
	    {

		fDSqu = (iX-iSizeH)*(iX-iSizeH)+
		        (iY-iSizeH)*(iY-iSizeH)+
		        (iZ-iSizeH)*(iZ-iSizeH);

                fTerm = exp(fDSqu * fBValue );

		if (fDSqu <= fCValue)
		    setValue( iX, iY, iZ, fLapl * ((1.0 - fTerm)*fTerm) );

		fScale += getValue( iX, iY, iZ );
	    }

    for (iZ=0; iZ<iSize; iZ++)
	for (iY=0; iY<iSize; iY++)
	    for (iX=0; iX<iSize; iX++)
		setValue( iX, iY, iZ, getValue( iX, iY, iZ ) / fScale );
};

/**
 * Filtrate Volume with Bilateral Filter
 * Attention: This will overwrite the current content of the volume object with the filter kernel. It will allocate the memory internally.
 * \param fSigma1D1 sigma of Gaussuian part of filter
 * \param fsigma1D2 sigma of Intensity part of filter
 * \param iSize size of the kernel
 * \param bProgress show a progress bar
 */
template<class T>
inline void svt_volume<T>::filtrBilateral(double fSigma1D1, double fSigma1D2, unsigned int iSize,  bool bProgress)
{
    //unsigned int iSize = rKernel.getSizeX();
    //unsigned int iSizeS = iSize * iSize;
    unsigned int iX, iY, iZ;
    int iKX, iKY, iKZ;
    Real64 fVal, fValInc, fValVol,fDeltaInc, fDeltaVol,  fDeltaIncSq, fDeltaVolSq, fKernel ;

    Real64 fOldWidth = m_fWidth;
    Real64 fOldGridX = m_fGridX;
    Real64 fOldGridY = m_fGridY;
    Real64 fOldGridZ = m_fGridZ;

    T* pData = m_pData;
    bool bInternal = m_bInternal;
    m_pData = NULL;
    m_bInternal = false;

    svt_volume<T> oTmp;
    oTmp.allocate( m_iSizeX, m_iSizeY, m_iSizeZ, 0.0 );

    m_pData = pData;
    bInternal = bInternal;
    m_bChanged = true;

    Real64 fScale = 0;

    // calculate dimension of kernel volume

    // truncate at fSigmaFactor * fSigma1D
    //unsigned int iSizeH1 = (int) ceil( fSigmaFactor * fSigma1D1 );
    //unsigned int iSizeH2 = (int) ceil( fSigmaFactor * fSigma1D2 );
    //max of iSizeH1 iSizeH2
    //unsigned int iSizeH = max(iSizeH1,iSizeH2);


    iSize = 2 * iSize - 1;

    int iDim = (int)((Real32)(iSize) * 0.5f);
    int iStart = -iDim;
    int iEnd = iDim+1;

    try
    {
	if (bProgress)
	    svt_exception::ui()->progressPopup("Applying Bilateral Filter...", 0, m_iSizeZ );

	for(iZ=0; iZ<m_iSizeZ; iZ++)
	{
	    for(iX=0; iX<m_iSizeX; iX++)
	    {
		for(iY=0; iY<m_iSizeY; iY++)
		{
		    fVal = m_pData[(iX)+(iY*m_iSizeX)+(iZ*m_iSizeX*m_iSizeY)];

                    if (fVal != (T)(0))
                    {
                        for(iKZ = iStart; iKZ < iEnd; iKZ++)
                        for(iKY = iStart; iKY < iEnd; iKY++)
                        for(iKX = iStart; iKX < iEnd; iKX++)
                        {
                           if ((((int)(iX)+iKX) < (int)(m_iSizeX) && ((int)(iX)+iKX) >= 0) && (((int)(iY)+iKY) < (int)(m_iSizeY) && ((int)(iY)+iKY) >= 0) && (((int)(iZ)+iKZ) < (int)(m_iSizeZ) && ((int)(iZ)+iKZ) >= 0))
                              {
                                    	fValInc 	= 	m_pData[(iX+iKX)+((iY+iKY)*m_iSizeX)+((iZ+iKZ)*m_iSizeX*m_iSizeY)];
                                    	fDeltaInc	=	sqrt(iKX*iKX+iKY*iKY+iKZ*iKZ);
                                    	fDeltaIncSq	=	fDeltaInc * fDeltaInc;

                                    	fValVol 	= 	m_pData[(iX)+((iY)*m_iSizeX)+((iZ)*m_iSizeX*m_iSizeY)];
                                    	fDeltaVol	=	sqrt((fValVol-fValInc)*(fValVol-fValInc));
                                    	fDeltaVolSq	=	fDeltaVol * fDeltaVol;

                                    	fKernel= exp(-1/(2.0*fSigma1D1*fSigma1D1) * fDeltaIncSq) *exp(-1/(2.0*fSigma1D2*fSigma1D2)   * fDeltaVolSq );
                                    	oTmp.setAt( iX, iY, iZ, oTmp.at( iX, iY, iZ ) + fKernel*fValInc);
                                        fScale += fKernel*fValInc;

                               }

                       }
                    }

                }
            }

            if (bProgress)
	        svt_exception::ui()->progress( iZ, m_iSizeZ );

	}

    for (iZ=0; iZ<iSize; iZ++)
	for (iY=0; iY<iSize; iY++)
	    for (iX=0; iX<iSize; iX++)
	    	oTmp.setAt( iX, iY, iZ,  oTmp.at( iX, iY, iZ ) /fScale);

	if (bProgress)
	    svt_exception::ui()->progressPopdown();

	*this = oTmp;

	m_bChanged = true;

	// store map properties of old map in oTmp
	m_fWidth = fOldWidth;
	m_fGridX = fOldGridX;
	m_fGridY = fOldGridY;
	m_fGridZ = fOldGridZ;

    } catch (svt_userInterrupt&)
    {
	svt_exception::ui()->progressPopdown();
    }

};
template<class T>
void svt_volume<T>::stepGeodesic(vector < vector<svt_vector3< int> > > & oPathVector, vector<svt_vector3< int> > & oPathSoFar, svt_vector3< int> oToPos, int iStepNumber, int iFullLength, int iCubeSize, int iNeigboorModel)  // next stepGeodesic at path to oToPos,
{
	if ( (oToPos.x()<0 || oToPos.x()>=iCubeSize || oToPos.y()<0 || oToPos.y()>=iCubeSize || oToPos.z()<0 || oToPos.z()>=iCubeSize))
		return;

	svt_vector3< int> oOrigin(0,0,0);
	for (int i=0; i<iStepNumber; i++)
		if ((oPathSoFar[i].distanceSq(oToPos) == 0)) //  || (i > 0 &&  (oOrigin.distance(oToPos) <= oOrigin.distance(oPathSoFar[i]))))
		{
			return;
		}

	oPathSoFar[iStepNumber]=oToPos;  // remember current stepGeodesic
	svt_vector3< int>  oMargin((int)iCubeSize*(0.5),(int)iCubeSize*(0.5),(int)iCubeSize*(0.5));
	if (iStepNumber==iFullLength)   // if this path has full length
	{
		vector <svt_vector3< int> > newpath(iFullLength+1);  //temporary vector for the found path to be added to oPathVector
		for (int i=0; i<=iFullLength; i++)
			newpath[i] = (oPathSoFar[i]-oMargin);

		oPathVector.push_back(newpath);
		return;
	}

	svt_vector3< int> oTempPosX(1,0,0); svt_vector3< int> oTempPosY(0,1,0); svt_vector3< int> oTempPosZ(0,0,1);
	svt_vector3< int> oTempPosXY(1,1,0); svt_vector3< int> oTempPosXZ(1,0,1); svt_vector3< int> oTempPoYZ(0,1,1);
	svt_vector3< int> oTempPosXmY(1,-1,0); svt_vector3< int> oTempPosXmZ(1,0,-1); svt_vector3< int> oTempPoYmZ(0,1,-1);
	svt_vector3 <int> oTempPosLoop;

    switch (iNeigboorModel)
    {
    case 0://SITUS_DOC_NEIGHBOOR4
		stepGeodesic(oPathVector, oPathSoFar,oToPos+oTempPosY, iStepNumber+1, iFullLength, iCubeSize, iNeigboorModel);  // next stepGeodesic: x, y+1, z
		stepGeodesic(oPathVector, oPathSoFar,oToPos+oTempPosX, iStepNumber+1, iFullLength, iCubeSize, iNeigboorModel);  // next stepGeodesic: x+1, y, z
		stepGeodesic(oPathVector, oPathSoFar,oToPos-oTempPosY, iStepNumber+1, iFullLength, iCubeSize, iNeigboorModel);  // next stepGeodesic: x, y-1, z
		stepGeodesic(oPathVector, oPathSoFar,oToPos-oTempPosX, iStepNumber+1, iFullLength, iCubeSize, iNeigboorModel);  // next stepGeodesic: x-1, y, z
    	break; 
    case 1://SITUS_DOC_NEIGHBOOR8
    	stepGeodesic(oPathVector, oPathSoFar,oToPos+oTempPosY, iStepNumber+1, iFullLength, iCubeSize, iNeigboorModel);  // next stepGeodesic: x, y+1, z
    	stepGeodesic(oPathVector, oPathSoFar,oToPos+oTempPosX, iStepNumber+1, iFullLength, iCubeSize, iNeigboorModel);  // next stepGeodesic: x+1, y, z
    	stepGeodesic(oPathVector, oPathSoFar,oToPos-oTempPosY, iStepNumber+1, iFullLength, iCubeSize, iNeigboorModel);  // next stepGeodesic: x, y-1, z
    	stepGeodesic(oPathVector, oPathSoFar,oToPos-oTempPosX, iStepNumber+1, iFullLength, iCubeSize, iNeigboorModel);  // next stepGeodesic: x-1, y, z
    	stepGeodesic(oPathVector, oPathSoFar,oToPos+oTempPosXY, iStepNumber+1, iFullLength, iCubeSize, iNeigboorModel);  // next stepGeodesic: x+1, y+1, z
    	stepGeodesic(oPathVector, oPathSoFar,oToPos-oTempPosXY, iStepNumber+1, iFullLength, iCubeSize, iNeigboorModel);  // next stepGeodesic: x-1, y-1, z
    	stepGeodesic(oPathVector, oPathSoFar,oToPos+oTempPosXmY, iStepNumber+1, iFullLength, iCubeSize, iNeigboorModel);  // next stepGeodesic: x+1, y-1, z
    	stepGeodesic(oPathVector, oPathSoFar,oToPos-oTempPosXmY, iStepNumber+1, iFullLength, iCubeSize, iNeigboorModel);  // next stepGeodesic: x-1, y+1, z
    	break;
    case 2://SITUS_DOC_NEIGHBOOR6
    	stepGeodesic(oPathVector, oPathSoFar,oToPos+oTempPosZ, iStepNumber+1, iFullLength, iCubeSize, iNeigboorModel);  // next stepGeodesic: x, y, z+1
    	stepGeodesic(oPathVector, oPathSoFar,oToPos+oTempPosY, iStepNumber+1, iFullLength, iCubeSize, iNeigboorModel);  // next stepGeodesic: x, y+1, z
    	stepGeodesic(oPathVector, oPathSoFar,oToPos+oTempPosX, iStepNumber+1, iFullLength, iCubeSize, iNeigboorModel);  // next stepGeodesic: x+1, y, z
    	stepGeodesic(oPathVector, oPathSoFar,oToPos-oTempPosZ, iStepNumber+1, iFullLength, iCubeSize, iNeigboorModel);  // next stepGeodesic: x, y, z-1
    	stepGeodesic(oPathVector, oPathSoFar,oToPos-oTempPosY, iStepNumber+1, iFullLength, iCubeSize, iNeigboorModel);  // next stepGeodesic: x, y-1, z
    	stepGeodesic(oPathVector, oPathSoFar,oToPos-oTempPosX, iStepNumber+1, iFullLength, iCubeSize, iNeigboorModel);  // next stepGeodesic: x-1, y, z
    	break;
    case 3://SITUS_DOC_NEIGHBOOR26
    	
    	for (int tx=(oToPos-oTempPosX).x(); tx<=(oToPos+oTempPosX).x(); tx++)
    	for (int ty=(oToPos-oTempPosY).y(); ty<=(oToPos+oTempPosY).y(); ty++)
    	for (int tz=(oToPos-oTempPosZ).z(); tz<=(oToPos+oTempPosZ).z(); tz++)
    	   	{
    	  	oTempPosLoop.x(tx);oTempPosLoop.y(ty);oTempPosLoop.z(tz);
    	   	stepGeodesic(oPathVector, oPathSoFar,oTempPosLoop, iStepNumber+1, iFullLength, iCubeSize, iNeigboorModel);  // next stepGeodesic: x, y, z-1
    	   	}
    	break;
    }
};

template<class T>
void svt_volume<T>::findGeodesicPaths(vector < vector<svt_vector3< int> > > & oPathVector, svt_vector3< int> oToPos, int iFullLength, int iCubeSize, int iNeigboorModel)
{
	vector< svt_vector3< int> > oPathSoFar(iFullLength+1);
	oPathVector.empty();
	stepGeodesic(oPathVector, oPathSoFar, oToPos, 0, iFullLength , iCubeSize, iNeigboorModel);
}

template<class T>
bool svt_volume<T>::criterionPair(pair<T, T>  oVector1, pair<T, T> oVector2)
{
	return oVector1.first < oVector2.first ;
}



template<class T>
void svt_volume<T>::FisherDiscriminant(vector <Real64> & fVectorVal_VarianceOnePath, vector <unsigned int > & iVectorVal_VarianceOnePath_Order,unsigned int & iK_star)
{
	unsigned int iN = fVectorVal_VarianceOnePath.size();
	vector<Real64> fVectorVal_VarianceOnePath_Sorted; 
	
	vector< pair<Real64, int> > fVectorVal_VarianceOnePath_Sorted_Pair;	
	
	for (unsigned int i=0;i<fVectorVal_VarianceOnePath.size();i++)
		fVectorVal_VarianceOnePath_Sorted_Pair.push_back (pair<Real64, int>( fVectorVal_VarianceOnePath[i] ,i ));
		
	sort(fVectorVal_VarianceOnePath_Sorted_Pair.begin(),fVectorVal_VarianceOnePath_Sorted_Pair.end());
	
	for (unsigned int i=0;i<fVectorVal_VarianceOnePath.size();i++)
	{
	iVectorVal_VarianceOnePath_Order[i]=fVectorVal_VarianceOnePath_Sorted_Pair[i].second;
	fVectorVal_VarianceOnePath_Sorted.push_back(fVectorVal_VarianceOnePath_Sorted_Pair[i].first);
	}
   
    Real64 fM1, fM2, fV1, fV2;

	vector<Real64> fVectorF;
	fVectorF.resize(fVectorVal_VarianceOnePath_Sorted.size());

    for (unsigned int iK=0;iK<iN; iK++)
        {
    	fM1=0.0;    	fM2=0.0;    	fV1=0.0;    	fV2=0.0;
     	   for(unsigned int iKtmpM1=0; iKtmpM1 <iK; iKtmpM1++)
			   fM1=fM1+fVectorVal_VarianceOnePath_Sorted[iKtmpM1];
		   fM1=(1/double(iK+1))*fM1;
     	   for(unsigned int iKtmpM2=iK+1; iKtmpM2 <iN; iKtmpM2++)
			   fM2=fM2+fVectorVal_VarianceOnePath_Sorted[iKtmpM2];
		   fM2=(1/double(iN-iK+1))*fM2;
     	   for(unsigned int iKtmpV1=0; iKtmpV1 <iK; iKtmpV1++)
     	   	   fV1= fV1 +( fVectorVal_VarianceOnePath_Sorted[iKtmpV1]-fM1 ) * ( fVectorVal_VarianceOnePath_Sorted[iKtmpV1]-fM1 );
     	   for(unsigned int iKtmpV2=iK+1; iKtmpV2 <iN; iKtmpV2++)
     		   fV2= fV2 +( fVectorVal_VarianceOnePath_Sorted[iKtmpV2]-fM2 ) * ( fVectorVal_VarianceOnePath_Sorted[iKtmpV2]-fM2 );
       	   // Fisher discriminant ratio
     	   fVectorF[iK]=( (fM1-fM2)*(fM1-fM2) )/(fV1+fV2);
        }
    //max element of vector fVectorF
    iK_star=max_element(fVectorF.begin(),fVectorF.end()) - fVectorF.begin();
}




template<class T>
inline void svt_volume<T>::filtrGeodesicPath(double fBetha, int iNeigboorModel, unsigned int iMaskDim,  unsigned int iPathLength,  bool bProgress)
{

	    unsigned int iX, iY, iZ, iMargin; 
	    Real64 fOldWidth = m_fWidth;
	    Real64 fOldGridX = m_fGridX;
	    Real64 fOldGridY = m_fGridY;
	    Real64 fOldGridZ = m_fGridZ;

	    unsigned int iNoOfChangedtoAverage = 0;
	    unsigned int iNoOfAllPath = 0;

	    svt_volume<T> oWalk3d;
	    oWalk3d.allocate( m_iSizeX, m_iSizeY, m_iSizeZ, 0.0 );

	    T* pData = m_pData;
	    bool bInternal = m_bInternal;
	    m_pData = NULL;
	    m_bInternal = false;

	    m_pData = pData;
		// flag for cancel pressed to stop all threads
	    bool bFlagUserInterrupt = false;
	    
	    unsigned int iXmax, iYmax, iZmax;
	    iXmax = (unsigned int) this ->m_iSizeX;
	    iYmax = (unsigned int) this ->m_iSizeY;
	    iZmax = (unsigned int) this ->m_iSizeZ;
	    int iTotalVoxelNum=iXmax*iYmax*iZmax;


#ifdef _OPENMP
		int inumTreads =omp_get_max_threads();
		omp_set_num_threads( inumTreads );
		svtout << "  Now parallel processing " << iZmax << " Z-sections with " << inumTreads << " threads, please wait." << endl;
	    if(omp_get_thread_num() == 0)
#endif	    
		if (bProgress)
				svt_exception::ui()->progressPopup("Applying parallel DPSV filter ...", 0, iTotalVoxelNum );


	    

	    vector < vector<svt_vector3< int> > > oPathVectorInt;  // paths filled in recursive stepGeodesic()
	    vector < vector<svt_vector3<Real64> > > oPathVector;  // paths filled in recursive stepGeodesic()
	    	    
	    vector <double> oSum2RoValPo;		       // for given path stores precomputed sum of distances from element [1] to [2], [3] and all following ones
	    vector <int> oPathFirstIndexVector;        // indexes in oPathVector of first path for a neighbor
	    vector <int> oPathLastIndexVector;         // indexes in oPathVector of last path for a neighbor


	    iMargin =(int)(iMaskDim*0.5);

	    svt_vector3<int> oFirstStep1(iMargin,iMargin,iMargin);
	
		findGeodesicPaths(oPathVectorInt,  oFirstStep1, iPathLength,  iMaskDim, iNeigboorModel);

	    int iNoPath = oPathVectorInt.size();
		svtout << "  Number of paths:  "<<  iNoPath << endl;
		
		//convert oPathVectorInt to oPathVector
		vector<svt_vector3< double> > oVectorTemp2;
		
		for(int iSetPath=0; iSetPath<iNoPath; iSetPath++)
			{
			for(unsigned int iV=0; iV<=iPathLength; iV++)
		  		{
				svt_vector3<double > oVectorTemp1(double(oPathVectorInt[iSetPath][iV].x()),double(oPathVectorInt[iSetPath][iV].y()),double(oPathVectorInt[iSetPath][iV].z()));
		        oVectorTemp2.push_back(oVectorTemp1);
		       	}	
		    oPathVector.push_back(oVectorTemp2);
		    oVectorTemp2.clear();
		    }
		
	   //fill oSum2RoValPo
	    oSum2RoValPo.resize(iNoPath);
	    for(int iSetPath=0; iSetPath<iNoPath; iSetPath++ )
	    	{
	    	double fSumDistance=0.0;
	    	for(unsigned int iV=2; iV<=iPathLength; iV++)
	    		fSumDistance += oPathVector[iSetPath][0].distance(oPathVector[iSetPath][iV]);
			oSum2RoValPo[iSetPath] =  fSumDistance;   
	    	}

	   //fill oPathFirstIndexVector and oPathLastIndexVector
		for(int iSetPath=0; iSetPath<iNoPath; iSetPath++ )
			{
		    if(iSetPath==0 || !(oPathVectorInt[iSetPath-1][1]==oPathVectorInt[iSetPath][1]))
		    	oPathFirstIndexVector.push_back(iSetPath);
		   	if(iSetPath==iNoPath-1 || !(oPathVectorInt[iSetPath][1]==oPathVectorInt[iSetPath+1][1])) // last path through this neighbor
		   		oPathLastIndexVector.push_back(iSetPath);
			}

		int iNoNeighborPixel= oPathFirstIndexVector.size();
		svtout << "  Neighboorhood model: " << iNoNeighborPixel << endl;
	
		vector <Real64> fVectorVal_MaxABSDiffInPath;
		vector <unsigned int> iVectorVal_MaxABSDiffInPath_Order;
		
		double fSetLambda, fHatP0Temp, fValP0, fSetLambdaTemp, fValP1, fVal_MaxABSDiffInPath, fValCurrMAX, fValPi_0, fValPi_1, fDist_P0_Pi_0, fVoxelValTmp;
	    int iNeighbor, iSetPath;
	    unsigned int iV, iK_star;
        long iProgresBarIndex=0;    
        
#ifdef _OPENMP
#pragma omp parallel for \
	  shared(iTotalVoxelNum,oWalk3d,iNoOfChangedtoAverage,iNoOfAllPath,oPathFirstIndexVector,oPathLastIndexVector,oPathVector,iPathLength,bFlagUserInterrupt) \
	  private(iX,iY,fSetLambda,fHatP0Temp,fValP0,iNeighbor,fSetLambdaTemp,fValP1,iSetPath,fVal_MaxABSDiffInPath,fValCurrMAX,iV,fValPi_0,fValPi_1,fDist_P0_Pi_0,iK_star,fVoxelValTmp,fVectorVal_MaxABSDiffInPath,iVectorVal_MaxABSDiffInPath_Order) \
	  schedule(dynamic, 1)
#endif
	     //trougth all volume
		for(iZ=0; iZ<iZmax; iZ++)      // Z size of volume
		 {
			try{
			if (bFlagUserInterrupt ==0)
			for(iX=0; iX<iXmax; iX++)   // X size of volume
				{
				for(iY=0; iY<iYmax; iY++)   // Y size of volume
				   {
				  	fSetLambda = 0.0; fHatP0Temp = 0.0;
				    fValP0   = this ->at(iX,iY,iZ);
				    for(iNeighbor=0; iNeighbor<iNoNeighborPixel; iNeighbor++) // for all neighbors
						{
						fSetLambdaTemp=0.0;
						fValP1   = this -> getValue(oPathVectorInt[oPathFirstIndexVector[iNeighbor]][1].x() +iX,
													oPathVectorInt[oPathFirstIndexVector[iNeighbor]][1].y() +iY,
						        					oPathVectorInt[oPathFirstIndexVector[iNeighbor]][1].z() +iZ);
						// Loop: among path throught iNeighbor - typically ther is 6 or 26 neighbors.
						fVectorVal_MaxABSDiffInPath.clear(); 
						for(iSetPath=oPathFirstIndexVector[iNeighbor]; iSetPath<=oPathLastIndexVector[iNeighbor]; iSetPath++ ) //for all paths through iNeighbor
							{
						    fVal_MaxABSDiffInPath= 0.0;
						    fValCurrMAX=fabs(fValP0-fValP1)/(oPathVector[iSetPath][0].distance(oPathVector[iSetPath][1]));
						    							    
						    for(iV=2; iV<=iPathLength; iV++)					// caclulation Lambda and mean for path (elements 2 up to iPathLength)
							   {
						       fValPi_0 = this ->getValue(oPathVectorInt[iSetPath][iV].x() +iX,    oPathVectorInt[iSetPath][iV].y() +iY,  oPathVectorInt[iSetPath][iV].z() +iZ);
						       fValPi_1 = this ->getValue(oPathVectorInt[iSetPath][iV-1].x() +iX,  oPathVectorInt[iSetPath][iV-1].y() +iY,oPathVectorInt[iSetPath][iV-1].z() +iZ);						       
						       fDist_P0_Pi_0=oPathVector[iSetPath][0].distance(oPathVector[iSetPath][iV]);
						       
						       if (fValCurrMAX < fabs(fValPi_1 - fValPi_0) /(fDist_P0_Pi_0))  	// choosing Current Maximal increment for path.
						          fValCurrMAX=fabs(fValPi_1 - fValPi_0) /(fDist_P0_Pi_0);
						       if (fValCurrMAX < fabs(fValP0-fValPi_0) /(fDist_P0_Pi_0))  // comparing with increment to center voxel and take maximal.
						          fValCurrMAX = fabs(fValP0-fValPi_0) /(fDist_P0_Pi_0);
						       fVal_MaxABSDiffInPath = fValCurrMAX;
							   }
						    fVectorVal_MaxABSDiffInPath.push_back(fVal_MaxABSDiffInPath); 	// vector with variances for all path through iNeighbor
							}
						    // FDA analysis for fVectorVal_MaxABSDiffInPath
						    iK_star=0; 	                  
						    iVectorVal_MaxABSDiffInPath_Order.clear(); iVectorVal_MaxABSDiffInPath_Order.resize(fVectorVal_MaxABSDiffInPath.size(),0);
						    FisherDiscriminant(fVectorVal_MaxABSDiffInPath, iVectorVal_MaxABSDiffInPath_Order, iK_star);
						    if(  (iK_star == 0) || (iK_star == fVectorVal_MaxABSDiffInPath.size()-1))  // condition for filtering ( if is FIRST or LAST thats means there is no Partition)
						    	fSetLambdaTemp += exp((-fBetha)*fVectorVal_MaxABSDiffInPath[iVectorVal_MaxABSDiffInPath_Order[0]]);
						    if( (iK_star > 0) && (iK_star <  fVectorVal_MaxABSDiffInPath.size()-1))// condition for filtering ( if it is beteen FIRST and LAST ther is patrition, and we took only that is below iK_star )
								{
						        fSetLambdaTemp += exp((-fBetha)*fVectorVal_MaxABSDiffInPath[iVectorVal_MaxABSDiffInPath_Order[iK_star]]);
#ifdef _OPENMP
#pragma omp critical 
#endif
								iNoOfChangedtoAverage += 1;
								}
#ifdef _OPENMP
#pragma omp critical
#endif				        
						        
						iNoOfAllPath += 1;
						fHatP0Temp += fSetLambdaTemp * fValP1;
						fSetLambda += fSetLambdaTemp;
						}
				    if  ( abs(fSetLambda) < 1.0E-200 ) fVoxelValTmp =0.0; //  avoid division by zero
				    else fVoxelValTmp = fHatP0Temp / fSetLambda;


				    
#ifdef _OPENMP
#pragma omp atomic  
				    iProgresBarIndex++;
#endif				    
				    oWalk3d.setAt(iX,iY,iZ,fVoxelValTmp);
				   }
				   
#ifdef _OPENMP				   
			if (omp_get_thread_num() == 0)
#endif			
			if (bProgress){
				svt_exception::ui()->progress( iProgresBarIndex, iTotalVoxelNum );
				}	
           }

  } catch (svt_userInterrupt&)
		{
			if (bProgress)
#ifdef _OPENMP				   		   	
			if (omp_get_thread_num() == 0)
			svtout << "  Cancel button pressed, please wait for all threads to stop." << endl;
#endif	
			iZ=iZmax; 
			svt_exception::ui()->progressPopdown();
			bFlagUserInterrupt  = true;
	   }

}
	

if(  bFlagUserInterrupt  == false)
	{
     svtout << "  DPSV case, parameter beta: " << fBetha <<endl;
     svtout << "  Fraction of paths changed by FDA: "  << iNoOfChangedtoAverage << " / " << iNoOfAllPath << " = " << (((double)iNoOfChangedtoAverage) / ((double)iNoOfAllPath )) << endl;
     if (bProgress)
#ifdef _OPENMP
     if (omp_get_thread_num() == 0)
#endif     
 			svt_exception::ui()->progressPopdown();
	 m_bChanged = true;
	 *this = oWalk3d;
	 // store map properties of old map
	 m_fWidth = fOldWidth;
	 m_fGridX = fOldGridX;
	 m_fGridY = fOldGridY;
	 m_fGridZ = fOldGridZ;
	}
else
	m_bChanged = false;

};


/**
 * Create an Identity kernel volume
 */
template<class T>
void svt_volume<T>::createIdentity()
{
    allocate(3,3,3,0.0);
    setAt(1,1,1,1.0);
};

/**
 * Create sphere at the center of the volume
 * \param fRadius radius of the sphere
 */
template<class T>
void svt_volume<T>::createSphere( Real64 fRadius )
{
    svt_vector4<Real64> oCenter;
    svt_vector4<Real64> oVoxel;
    Real64 fDist;
    oCenter.x( (Real64)(m_iSizeX) * 0.5 * m_fWidth );
    oCenter.y( (Real64)(m_iSizeY) * 0.5 * m_fWidth );
    oCenter.z( (Real64)(m_iSizeZ) * 0.5 * m_fWidth );

    unsigned int iX, iY, iZ;

    for (iZ=0; iZ<m_iSizeZ; iZ++)
	for (iY=0; iY<m_iSizeY; iY++)
	    for (iX=0; iX<m_iSizeX; iX++)
	    {
		oVoxel.x( (Real64)(iX) * m_fWidth);
		oVoxel.y( (Real64)(iY) * m_fWidth);
		oVoxel.z( (Real64)(iZ) * m_fWidth);

                fDist = oVoxel.distance(oCenter);

                if ( fDist < fRadius)
		    setValue( iX, iY, iZ, 1.0 - (fDist / fRadius) );
		else
                    setValue( iX, iY, iZ, 0.0 );
	    }

};

///////////////////////////////////////////////////////////////////////////////
// Sampling functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Set cutoff for sampling
 * \param fCutoff a voxel value lower than this value is not considered for the sampling
 */
template<class T>
void svt_volume<T>::setCutoff( Real64 fCutoff )
{
    m_fCutoff = fCutoff;
};
/**
 * Get cutoff for sampling
 * \return a voxel value lower than this value is not considered for the sampling
 */
template<class T>
Real64 svt_volume<T>::getCutoff( ) const
{
    return m_fCutoff;
};

/**
 * sample the object randomly and return a vector that refrects the probability distribution of the object
 */
template<class T>
svt_vector4<Real64> svt_volume<T>::sample( )
{
    T fMax = getMaxDensity();
    Real64 fNumV = (Real64)(m_iSizeX * m_iSizeY * m_iSizeZ);
    bool bFound = false;
    unsigned int iIndex;
    T fVoxel, fDensity;

    while ( bFound == false )
    {
        iIndex = (unsigned int)(svt_genrand() * fNumV);
        fVoxel = at(iIndex);
        fDensity = (T)(svt_genrand()*fMax);

	if (fVoxel > fDensity && fVoxel > m_fCutoff)
	    bFound = true;
    }

    svt_vector4<Real64> oVec;

    unsigned int iSizeXY = m_iSizeX * m_iSizeY;
    unsigned int iIndz = iIndex / iSizeXY;
    iIndex -= iIndz * iSizeXY;
    unsigned int iIndy = iIndex / m_iSizeX;
    unsigned int iIndx = iIndex - iIndy * m_iSizeX;

    oVec.x( getGridX() + (iIndx * m_fWidth) );
    oVec.y( getGridY() + (iIndy * m_fWidth) );
    oVec.z( getGridZ() + (iIndz * m_fWidth) );

    //oVec.x( (iIndx * m_fWidth) - ( m_iSizeX * m_fWidth * 0.5 ) );
    //oVec.y( (iIndy * m_fWidth) - ( m_iSizeY * m_fWidth * 0.5 ) );
    //oVec.z( (iIndz * m_fWidth) - ( m_iSizeZ * m_fWidth * 0.5 ) );

    return oVec;
};

/**
 * Sample inside sphere the object randomly and return a vector that refrects the probability distribution of the object
 * \param oCenter object class svt_vector4 with xyz coordinates of center of sphere
 * \param fRadius radius of the sphere
 */
template<class T>
svt_vector4<Real64> svt_volume<T>::sampleSphere(svt_vector4<Real64> oCenter, Real64 fRadius )
{
    T fMax = getMaxDensity();
    Real64 fNumV = (Real64)(m_iSizeX * m_iSizeY * m_iSizeZ);
    Real64 fDist, fGridX,fGridY, fGridZ;

    bool bFound = false;
    unsigned int iIndex;
    T fVoxel, fDensity;

    unsigned int iSizeXY = m_iSizeX * m_iSizeY;
    unsigned int iIndx, iIndy, iIndz;

    fGridX = getGridX();
    fGridY = getGridY();
    fGridZ = getGridZ();


    while ( bFound == false )
    {
        iIndex = (unsigned int)(svt_genrand() * fNumV);
        fVoxel = at(iIndex);
        fDensity = (T)(svt_genrand()*fMax);


        iIndz = (iIndex / iSizeXY);
        iIndex -= iIndz * iSizeXY;
        iIndy = (iIndex / m_iSizeX);
        iIndx = (iIndex - iIndy * m_iSizeX);


		fDist =	(( fGridX + (iIndx * m_fWidth) )-oCenter.x())*(( fGridX + (iIndx * m_fWidth) )-oCenter.x())+
				(( fGridY + (iIndy * m_fWidth) )-oCenter.y())*(( fGridY + (iIndy * m_fWidth) )-oCenter.y())+
				(( fGridZ + (iIndz * m_fWidth) )-oCenter.z())*(( fGridZ + (iIndz * m_fWidth) )-oCenter.z());
		fDist = sqrt(fDist);

	if (fVoxel > fDensity && fVoxel > m_fCutoff && fDist< fRadius )
	    bFound = true;
    }

    svt_vector4<Real64> oVec;

    oVec.x( fGridX + (iIndx * m_fWidth) );
    oVec.y( fGridY + (iIndy * m_fWidth) );
    oVec.z( fGridZ + (iIndz * m_fWidth) );

    return oVec;
};

/**
* gives the best ISO threshold for fitting the object with a given volume
* this outputs the corresponding threshold for the data set
* this algorithm adjusts for the voxel spacing of each object
*/
template<class T>
T svt_volume<T>::bestISO(svt_volume<T>& rTarget, T fThreshold)
{
	double fTargetVol = 0; //Holds target volume
	vector<T> oVoxels; //Holds the voxel data

	for(unsigned int i=0; i < rTarget.getSizeX(); i++)//Get target voxel amount
		for(unsigned int j=0; j < rTarget.getSizeY(); j++)
			for(unsigned int k=0; k < rTarget.getSizeZ(); k++)
				if(rTarget.at(i, j, k) >= fThreshold)
					fTargetVol++;

	T fTargetCubed = rTarget.getWidth() * rTarget.getWidth() * rTarget.getWidth();
	T fCubed = m_fWidth * m_fWidth * m_fWidth;
	fTargetVol *= fTargetCubed / fCubed; //Adjust for voxel spacing to get volume

	for(unsigned int i=0; i < getSizeX(); i++)//Organize voxel data
		for(unsigned int j=0; j < getSizeY(); j++)
			for(unsigned int k=0; k < getSizeZ(); k++)
				if(getValue(i, j, k)>0) oVoxels.push_back(getValue(i, j, k));
	sort(oVoxels.begin(), oVoxels.end()); //Sort the voxel information

	int fIndex = oVoxels.size() - fTargetVol;
	if(fIndex < 0) fIndex = 0;
	if(fIndex > oVoxels.size()-1) fIndex = oVoxels.size()-1;
	return oVoxels.at(fIndex); //Return the ISO threshold
};

/**
 * Calculates the corresponding isosurface threshold value for this volume. The algorithm tries to cover exactly the voxel values of the other volume with the ones from here, so that
 * both isosurfaces have a similar size. rVol_A would typically have a higher resolution but show the exact same system.
 *
 * \param rVol_A other volume
 * \param fThresh good known threshold value of rVol_A
 */
template<class T>
T svt_volume<T>::correspondingISO( svt_volume<T>& rVol_A, T fThresh_A )
{
    Real64 fX, fY, fZ;
    T fThresh_B = (T)(1.0E10);

    for(unsigned int iX=0; iX < this->getSizeX(); iX++)
        for(unsigned int iY=0; iY < this->getSizeY(); iY++)
            for(unsigned int iZ=0; iZ < this->getSizeZ(); iZ++)
            {
                fX = ((Real64)(iX) * m_fWidth) + m_fGridX;
                fY = ((Real64)(iY) * m_fWidth) + m_fGridY;
                fZ = ((Real64)(iZ) * m_fWidth) + m_fGridZ;

                if ( rVol_A.getRealSpaceValue( fX, fY, fZ ) >= fThresh_A && this->getValue(iX,iY,iZ) < fThresh_B )
                    fThresh_B = this->getValue(iX,iY,iZ);
            }

    return fThresh_B;
};

/**
* scales the map by a given factor
*/
template<class T>
void svt_volume<T>::scale(T fScale)
{
    for(unsigned int i=0; i < getSizeX(); i++)
        for(unsigned int j=0; j < getSizeY(); j++)
            for(unsigned int k=0; k < getSizeZ(); k++)
                setAt(i, j, k, at(i, j, k) * fScale);
    return;
};

/**
* shifts the map by a given factor
*/
template<class T>
void svt_volume<T>::shift(T fShift)
{
    for(unsigned int i=0; i < getSizeX(); i++)
        for(unsigned int j=0; j < getSizeY(); j++)
            for(unsigned int k=0; k < getSizeZ(); k++)
                    setAt(i, j, k, at(i, j, k) + fShift);
    return;
};

/**
* scales using a given slope (for testing)
*/
template<class T>
void svt_volume<T>::scaleBySlope(T fSlope)
{
    for(unsigned int i=0; i < getSizeX(); i++)
        for(unsigned int j=0; j < getSizeY(); j++)
            for(unsigned int k=0; k < getSizeZ(); k++)
                setAt(i, j, k, at(i, j, k) * at(i, j, k) * fSlope);
    return;
};

/**
* returns the slope given two points
*/
template<class T>
T svt_volume<T>::getSlope(T fX1, T fY1, T fX2, T fY2)
{
	return ((fY2 - fY1) / (fX2 - fX1));
};

/**
* interpolated scaling of the volume based on given information
* takes in two old thresholds and two new thresholds (from bestISO method)
*/
template<class T>
void svt_volume<T>::interpolatedScale(T fThreshold1, T fThreshold2, T fNew1, T fNew2)
{
	T fScale1 = fNew1 / fThreshold1;
	T fScale2 = fNew2 / fThreshold2;
	T fSlope = getSlope(fThreshold1, fScale1, fThreshold2, fScale2);
	T fShift = fNew1 - fSlope * fThreshold1;
	for(unsigned int i=0; i < getSizeX(); i++)
		for(unsigned int j=0; j < getSizeY(); j++)
			for(unsigned int k=0; k < getSizeZ(); k++)
				if(at(i, j, k)>0)
				{
					setAt(i, j, k, (sqrt(at(i, j, k)) / sqrt(fSlope)));
				}
	shift(fShift); //Adjust for shift value
	return;
};

/**
* calculates the root mean square of the volume
*/
template<class T>
T svt_volume<T>::getRMS()
{
	T fSum = 0;
	for(unsigned int i=0; i < getSizeX(); i++)
		for(unsigned int j=0; j < getSizeY(); j++)
			for(unsigned int k=0; k < getSizeZ(); k++)
				fSum += pow(at(i, j, k), 2);
	fSum /= (getSizeX() * getSizeY() * getSizeZ());
	return sqrt(fSum);
};

/**
 * Remove every structure that has more than two neighbors
 * \param fThreshold voxel above that value are considered occupied
 */
template<class T>
void svt_volume<T>::removeNeighbors( T fThreshold )
{
    for(unsigned int iX=0; iX < (unsigned int)getSizeX(); iX++)
        for(unsigned int iY=0; iY < (unsigned int)getSizeY(); iY++)
            for(unsigned int iZ=0; iZ < (unsigned int)getSizeZ(); iZ++)
                if ( getValue( iX, iY, iZ ) > fThreshold  && numNeighbors( iX, iY, iZ, fThreshold ) == 1 )
                    setValue( iX, iY, iZ, 1 );
                else
                    setValue( iX, iY, iZ, 0 );
};

/**
 * How many direct neighbors does a voxel have?
 * \return number of occupied neighboring voxels
 */
template<class T>
unsigned int svt_volume<T>::numNeighbors( unsigned int iX, unsigned int iY, unsigned int iZ, T fThreshold )
{
    int iVX = (int)iX;
    int iVY = (int)iY;
    int iVZ = (int)iZ;

    unsigned int iNeighb = 0;

    for(int iKX=-1; iKX < 2; iKX++)
        for(int iKY=-1; iKY < 2; iKY++)
            for(int iKZ=-1; iKZ < 2; iKZ++)
            {
                if (
                    ( iVX+iKX >= 0 && iVX+iKX < getSizeX() ) &&
                    ( iVY+iKY >= 0 && iVY+iKY < getSizeY() ) &&
                    ( iVZ+iKZ >= 0 && iVZ+iKZ < getSizeZ() ) &&
                    ( getValue( iVX+iKX, iVY+iKY, iVZ+iKZ ) > fThreshold )
                   )
                    iNeighb++;
            }

    return iNeighb;
};

///////////////////////////////////////////////////////////////////////////////
// Internal functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Calculate/update the minimum and maximum density values.
 */
template<class T>
inline void svt_volume<T>::calcMinMaxDensity()
{
    unsigned int iCount;
    unsigned int iNVox = m_iSizeX*m_iSizeY*m_iSizeZ;

    m_fMaxDensity = getValue(0,0,0);
    m_fMinDensity = getValue(0,0,0);

    double fAvgDensity = (double)(0.0);
    m_fNorm = 0.0;
    T fValue;

    for (iCount=0; iCount<iNVox; iCount++)
    {
        fValue = getValue(iCount);

	if (m_fMaxDensity < fValue)
	    m_fMaxDensity = fValue;

	if (m_fMinDensity > fValue)
	    m_fMinDensity = fValue;

        m_fNorm += fValue * fValue;
        fAvgDensity += (double)(getValue(iCount));
    }

    m_fNorm = sqrt( m_fNorm );
    fAvgDensity /= (double)(iNVox);

    m_fAvgDensity = (T)(fAvgDensity);

    m_bChanged = false;
};

/**
 * Calculate correlation with other svt_volume object
 * \param rVol reference to other svt_volume object
 */
template<class T>
Real64 svt_volume<T>::correlation( svt_volume<T>& rVol, bool bMask)
{
    // is there actually data in the two objects?
    if (size() == 0 || rVol.size() == 0)
        return -1.0e9;

    // is the voxelwidth indeed identical?
    if (fabs(m_fWidth - rVol.getWidth()) > EPSILON)
    {
        svtout << "Voxel width different: " << m_fWidth << " != " << rVol.getWidth() << endl;
	svt_exception::ui()->error("The cross-correlation coefficient could not be computed due to different voxel width!");
	return -1.0e9;
    }

    // calculate correlation
    int iX, iY, iZ;
    Real64 fCorrelation = 0.0;
    Real64 fOrgX = this->getGridX() - rVol.getGridX();
    Real64 fOrgY = this->getGridY() - rVol.getGridY();
    Real64 fOrgZ = this->getGridZ() - rVol.getGridZ();
    // use of (int)floor to make the rounding correct for negative numbers
    int iOrgX = (int)floor(fOrgX / m_fWidth + 0.5f);
    int iOrgY = (int)floor(fOrgY / m_fWidth + 0.5f);
    int iOrgZ = (int)floor(fOrgZ / m_fWidth + 0.5f);
    Real64 fValueA, fValueB;
    Real64 fValueA_Sum = 0,  fValueB_Sum = 0, fValueA_Sq_Sum = 0, fValueB_Sq_Sum = 0, fCoSum = 0;
    unsigned int iAddedVoxels = 0;

    for(iZ=0; iZ<(int)(m_iSizeZ); iZ++)
	for(iY=0; iY<(int)(m_iSizeY); iY++)
	    for(iX=0; iX<(int)(m_iSizeX); iX++)
                if (iX+iOrgX >= 0 && iY+iOrgY >= 0 && iZ+iOrgZ >= 0 && iX+iOrgX < (int)(rVol.getSizeX()) && iY+iOrgY < (int)(rVol.getSizeY()) && iZ+iOrgZ < (int)(rVol.getSizeZ()) )
                {
                    fValueA = (Real64)(this->at(iX, iY, iZ));
                    fValueB = (Real64)(rVol.at(iX+iOrgX, iY+iOrgY, iZ+iOrgZ));

		    if ( !bMask || ( bMask && fValueA != 0) )
		    {
			fCoSum 		+= fValueA * fValueB;
			fValueA_Sq_Sum 	+= fValueA * fValueA;
			fValueB_Sq_Sum 	+= fValueB * fValueB;
			fValueA_Sum 	+= fValueA;
			fValueB_Sum 	+= fValueB;
			iAddedVoxels ++;
		    }
                }
    if (sqrt((iAddedVoxels*fValueA_Sq_Sum - fValueA_Sum*fValueA_Sum)*(iAddedVoxels*fValueB_Sq_Sum - fValueB_Sum*fValueB_Sum))!=0)
	fCorrelation = (iAddedVoxels*fCoSum - fValueA_Sum*fValueB_Sum)/sqrt((iAddedVoxels*fValueA_Sq_Sum - fValueA_Sum*fValueA_Sum)*(iAddedVoxels*fValueB_Sq_Sum - fValueB_Sum*fValueB_Sum));
    else
	fCorrelation = 0.0;

    return fCorrelation;
};

/**
 * Calculate correlation with other svt_volume object
 * \param rVol reference to other svt_volume object
 */
template<class T>
Real64 svt_volume<T>::correlationColores(svt_volume<T>& rVol)
{
    // calculate norm
    Real64 fNormA = this->getNorm();
    Real64 fNormB = rVol.getNorm();

    if (fNormA == 0.0 || fNormB == 0.0)
        return 0.0;

    // calculate correlation
    int iX, iY, iZ;
    Real64 fCorrelation = 0.0;
    Real64 fOrgX = this->getGridX() - rVol.getGridX();
    Real64 fOrgY = this->getGridY() - rVol.getGridY();
    Real64 fOrgZ = this->getGridZ() - rVol.getGridZ();
    // use of (int)floor to make the rounding correct for negative numbers
    int iOrgX = (int)floor(fOrgX / m_fWidth + 0.5f);
    int iOrgY = (int)floor(fOrgY / m_fWidth + 0.5f);
    int iOrgZ = (int)floor(fOrgZ / m_fWidth + 0.5f);

    for(iZ=0; iZ<(int)(m_iSizeZ); iZ++)
	for(iY=0; iY<(int)(m_iSizeY); iY++)
	    for(iX=0; iX<(int)(m_iSizeX); iX++)
		if (iX+iOrgX >= 0 && iY+iOrgY >= 0 && iZ+iOrgZ >= 0 && iX+iOrgX < (int)(rVol.getSizeX()) && iY+iOrgY < (int)(rVol.getSizeY()) && iZ+iOrgZ < (int)(rVol.getSizeZ()) )
		    fCorrelation += this->at(iX, iY, iZ) * rVol.at(iX+iOrgX, iY+iOrgY, iZ+iOrgZ);

    fCorrelation /= fNormA * fNormB;

    return fCorrelation;
};

/**
 * Calculates the internal correlation of the voxel values.
 */
template<class T>
svt_volume<Real64>* svt_volume<T>::internalCorr( unsigned int iWidth )
{
    Real64 fCorr;
    svt_volume<Real64>* pCorrVol = new svt_volume<Real64>( this->getSizeX(), this->getSizeY(), this->getSizeZ() );

    try
    {
	svt_exception::ui()->progressPopup("Computing internal correlation...", 0, this->getSizeX() );

        for(int iX=0; iX < (int)this->getSizeX(); iX++)
        {
            svt_exception::ui()->progress( iX, this->getSizeX() );

            for(int iY=0; iY < (int)this->getSizeY(); iY++)
                for(int iZ=0; iZ < (int)this->getSizeZ(); iZ++)
                {
                    fCorr = 0.0;

                    for(int iKX=-iWidth; iKX < (int)iWidth; iKX++)
                        for(int iKY=-iWidth; iKY < (int)iWidth; iKY++)
                            for(int iKZ=-iWidth; iKZ < (int)iWidth; iKZ++)

                                if (
                                    (iKX != 0 || iKY != 0 || iKZ != 0) &&
                                    (iX+iKX >= 0 && iX+iKX < (int)this->getSizeX()) &&
                                    (iY+iKY >= 0 && iY+iKY < (int)this->getSizeY()) &&
                                    (iZ+iKZ >= 0 && iZ+iKZ < (int)this->getSizeZ())
                                   )

                                    fCorr += getValue( iX, iY, iZ ) * getValue( iX+iKX, iY+iKY, iZ+iKZ );

                    pCorrVol->setValue( iX, iY, iZ, fCorr );
                }
        }

    } catch (svt_userInterrupt&)
    {
	svtout << "Warning: Internal correlation calculation was aborted, volume data might be incomplete..." << endl;
    }

    svt_exception::ui()->progressPopdown();

    return pCorrVol;
};


/**
 * Grow - this function blows up the volume at a different size ( only padding routine!).
 * \param iNewSizeX new size (x dimension)
 * \param iNewSizeY new size (y dimension)
 * \param iNewSizeZ new size (z dimension)
 */
template<class T>
void svt_volume<T>::resize(unsigned int iNewSizeX, unsigned int iNewSizeY, unsigned int iNewSizeZ)
{


    unsigned int iOldSizeX = getSizeX();
    unsigned int iOldSizeY = getSizeY();
    unsigned int iOldSizeZ = getSizeZ();

    if ((iOldSizeX <= iNewSizeX) && (iOldSizeY <= iNewSizeY) && (iOldSizeZ <= iNewSizeZ))
    {

        svt_volume<T> *pVol = new svt_volume<T>( iNewSizeX, iNewSizeY, iNewSizeZ, 0.0 );

        Real64 fVoxelValue;
        Real64 fWidth = getWidth();

    	Real64 fOldGridX = getGridX();
    	Real64 fOldGridY = getGridY();
    	Real64 fOldGridZ = getGridZ();

    	Real64 fNewGridX = (fOldGridX/fWidth - abs(double(iOldSizeX) - double(iNewSizeX))/(2.0))*fWidth;
    	Real64 fNewGridY = (fOldGridY/fWidth - abs(double(iOldSizeY) - double(iNewSizeY))/(2.0))*fWidth;
    	Real64 fNewGridZ = (fOldGridZ/fWidth - abs(double(iOldSizeZ) - double(iNewSizeZ))/(2.0))*fWidth;

    	unsigned int iCurrX, iCurrY, iCurrZ;


    	for(unsigned int iX=0; iX<iNewSizeX; iX++)
    		for(unsigned int iY=0; iY<iNewSizeY; iY++)
    			for(unsigned int iZ=0; iZ<iNewSizeZ; iZ++)
    			  if (	( iX >= floor(iNewSizeX/2 - iOldSizeX/2)) && ( iX < floor(iNewSizeX/2 + iOldSizeX/2)) &&
    					( iY >= floor(iNewSizeY/2 - iOldSizeY/2)) && ( iY < floor(iNewSizeY/2 + iOldSizeY/2)) &&
    					( iZ >= floor(iNewSizeZ/2 - iOldSizeZ/2)) && ( iZ < floor(iNewSizeZ/2 + iOldSizeZ/2)) )
    			  {
    			  iCurrX= iX-floor(iNewSizeX / 2 - iOldSizeX / 2);
    			  iCurrY= iY-floor(iNewSizeY / 2 - iOldSizeY / 2);
    			  iCurrZ= iZ-floor(iNewSizeZ / 2 - iOldSizeZ / 2);

    			  fVoxelValue = getValue( iCurrX, iCurrY, iCurrZ );
    			  pVol->setValue( iX, iY, iZ, fVoxelValue );
    			  }



    pVol -> setWidth(fWidth);
    pVol -> setGrid(fNewGridX, fNewGridY, fNewGridZ	);

    *this = (*pVol);
   	m_bChanged = true;

    }
    else
   {
    svtout << "Cannot resize to smaller volume size" << endl;
    }

};

/**
 * Calculate number of occupied voxels
 * \param fThreshold threshold value - only voxels higher than the threshold are counted.
 * \return number of voxels with a density higher than fThreshold
 */
template<class T>
unsigned long svt_volume<T>::getOccupied( T fThreshold ) const
{
    unsigned long iCount = 0;
    unsigned long iNVox = m_iSizeX*m_iSizeY*m_iSizeZ;
    unsigned long i;

    for (i=0; i<iNVox; i++)
	if (at(i) >= fThreshold)
            iCount++;

    return iCount;
}

///////////////////////////////////////////////////////////////////////////////
// File formats
///////////////////////////////////////////////////////////////////////////////

/**
 * Load a file. This function looks at the extension to determine which function has to be used to actually load the file.
 * \param pFname pointer to array of char with the filename
 */
template<class T>
inline svt_matrix4<T> svt_volume<T>::load(const char* pFname)
{
    const char* pExt = strrchr( pFname , '.' );

    if (pExt != NULL)
    {
        // situs?
        if (
            strcmp( pExt, ".situs" ) == 0 || strcmp( pExt, ".sit" ) == 0 ||
            strcmp( pExt, ".SITUS" ) == 0 || strcmp( pExt, ".SIT" ) == 0
           )
            return loadSitus( pFname );

        // mrc?
        if (
            strcmp( pExt, ".mrc" ) == 0 || strcmp( pExt, ".ccp4" ) == 0 ||
            strcmp( pExt, ".MRC" ) == 0 || strcmp( pExt, ".CCP4" ) == 0 ||
            strcmp( pExt, ".map" ) == 0 || strcmp( pExt, ".MAP" )  == 0
           )
        {
            loadMRC( pFname );
            svt_matrix4<T> oMatrix;
            return oMatrix;
        }

        // SPIDER?
        if (
            strcmp( pExt, ".spi" ) == 0 || strcmp( pExt, ".SPI" ) == 0 ||
            strcmp( pExt, ".spider" ) == 0 || strcmp( pExt, ".SPIDER" )  == 0
           )
        {
            loadSpider( pFname );
            svt_matrix4<T> oMatrix;
            return oMatrix;
        }

        // raw?
        if (
            strcmp( pExt, ".vol" ) == 0 || strcmp( pExt, ".raw" ) == 0 || strcmp( pExt, ".dat" ) == 0 ||
            strcmp( pExt, ".vol" ) == 0 || strcmp( pExt, ".RAW" ) == 0 || strcmp( pExt, ".DAT" ) == 0
           )
        {
            loadRaw( pFname );
        }
    }

    svt_matrix4<T> oMatrix;
    return oMatrix;
};

/**
 * Save a file. This function looks at the extension to determine which function has to be used to actually save the file.
 * \param pFname pointer to array of char with the filename
 */
template<class T>
inline void svt_volume<T>::save(const char* pFname)
{
    const char* pExt = strrchr( pFname , '.' );

    if (pExt != NULL)
    {
        // situs?
        if (
            strcmp( pExt, ".situs" ) == 0 || strcmp( pExt, ".sit" ) == 0 ||
            strcmp( pExt, ".SITUS" ) == 0 || strcmp( pExt, ".SIT" ) == 0
           )
            saveSitus( pFname );

        // mrc?
        if (
            strcmp( pExt, ".mrc" ) == 0 || strcmp( pExt, ".ccp4" ) == 0 ||
            strcmp( pExt, ".MRC" ) == 0 || strcmp( pExt, ".CCP4" ) == 0 ||
            strcmp( pExt, ".map" ) == 0 || strcmp( pExt, ".MAP" )  == 0
           )
            writeMRC( pFname );

        // SPIDER?
        if (
            strcmp( pExt, ".spi" ) == 0 || strcmp( pExt, ".SPI" ) == 0 ||
            strcmp( pExt, ".spider" ) == 0 || strcmp( pExt, ".SPIDER" )  == 0
           )
            writeSpider( pFname );
    }
};


/**
 * read situs file
 * \param pFilename pointer to array of char with the filename
 * \returns matrix4 object with the transformation (situs only supports translations)
 */
template<class T>
inline svt_matrix4<T> svt_volume<T>::loadSitus(const char* pFilename)
{
    svt_matrix4<T> oMatrix;
    unsigned long iNVox, iCount;
    Real64 fTemp, fTest;

    FILE *pFile = fopen(pFilename, "r");
    if( pFile == NULL )
    {
        char pFname[2560];
        sprintf(pFname, "Can't open Situs file: \"%s\"!", pFilename );
        svt_exception::ui()->error(pFname);
        return oMatrix;
    }

    // read header and print information
    if (fscanf(pFile, "%le %le %le %le %d %d %d", &m_fWidth, &m_fGridX, &m_fGridY, &m_fGridZ, &m_iSizeX, &m_iSizeY, &m_iSizeZ) < 7)
    {
        char pStr[2560];
        sprintf(pStr, "Header of Situs file \"%s\" corrupted!", pFilename );
        svt_exception::ui()->error(pStr);
        return oMatrix;
    }

    svtout << "Loading situs file: " << pFilename << endl;
    svtout << "Header information: " << endl;
    svtout << "   Columns, rows, and sections: x=1-" << m_iSizeX << ", y=1-" << m_iSizeY << ", z=1-" << m_iSizeZ << endl;
    svtout << "   3D coordinates of first voxel (1,1,1): (" << m_fGridX << "," << m_fGridY << "," << m_fGridZ << ")" << endl;
    svtout << "   Voxel size in Angstrom: " << m_fWidth << endl;
    iNVox = m_iSizeX * m_iSizeY * m_iSizeZ;

    // allocate memory and read data
    T *pPhi = NULL;
    try
    {
        pPhi = new T[iNVox];

    } catch(bad_alloc& ex)
    {
        svt_exception::ui()->error("While loading Situs file: Unable to satisfy memory allocation request");
        return oMatrix;
    }

    // read data
    try
    {
        svt_exception::ui()->progressPopup("Loading Situs File...", 0, iNVox);

        for (iCount=0; iCount<iNVox; iCount++)
        {
            if (iCount%10000 == 0)
                svt_exception::ui()->progress( iCount, iNVox);

            if (fscanf(pFile,"%le", &fTemp) != 1)
            {
                char oError[2048];
                sprintf(oError, "Warning: The situs file is too short (%li / %li voxels)\nnot compatible with header information or data unreadable.", iCount, iNVox);
                svt_exception::ui()->error(oError);
                iCount = iNVox;
            } else
                *(pPhi+iCount) = (T)fTemp;
        }
    } catch (svt_userInterrupt&)
    {
        delete[] pPhi;
        m_iSizeX = m_iSizeY = m_iSizeZ = 0;
        m_fWidth = 0;
        fclose(pFile);
        m_bInternal = false;
        svt_matrix4<T> oMatrix;
        return oMatrix;
    }

    svt_exception::ui()->progressPopdown();

    if (fscanf(pFile, "%le", &fTest) != EOF)
    {
        char pStr[2560];
        sprintf(pStr, "Situs file \"%s\" is too long, not compatible with header information", pFilename );
        svt_exception::ui()->error(pStr);
    }

    setData( m_iSizeX, m_iSizeY, m_iSizeZ, pPhi );
    m_bInternal = true;

    fclose(pFile);

    // create transformation (translation) matrix
    oMatrix.translate( (T)m_fGridX, (T)m_fGridY, (T)m_fGridZ );
    return oMatrix;
};

/**
 * write situs file
 * \param pFilename pointer to array of char with the filename
 */
template<class T>
inline void svt_volume<T>::saveSitus( const char* pFilename ) const
{
    unsigned long iNVox, iCount;
    iNVox = m_iSizeX * m_iSizeY * m_iSizeZ;
    FILE* pFile = fopen(pFilename, "w");

    if( pFile == NULL )
    {
        char pStr[2560];
        sprintf(pStr, "Can't write to file: \"%s\", file was NOT saved!", pFilename );
        svt_exception::ui()->error(pStr);
        return;
    }

    fprintf(pFile, "  %f %f %f %f %d %d %d\n", m_fWidth, m_fGridX, m_fGridY, m_fGridZ, m_iSizeX, m_iSizeY, m_iSizeZ);
    fprintf(pFile, "\n");

    try
    {
	svt_exception::ui()->progressPopup("Saving Situs File...", 0, iNVox);

	for(iCount=0; iCount<iNVox; iCount++)
	{
            if (iCount%1000 == 0)
		svt_exception::ui()->progress( iCount, iNVox);

	    if ((iCount+1)%10 == 0)
		fprintf (pFile," %10.6f \n", (Real64)(m_pData[iCount]));
	    else
		fprintf (pFile," %10.6f ", (Real64)(m_pData[iCount]));
	}

    } catch (svt_userInterrupt&)
    {
	svtout << "Warning: Saving Situs file was aborted, file might be incomplete..." << endl;
    }

    fclose(pFile);
    svt_exception::ui()->progressPopdown();

    return;
};

/**
 * Save povray df3 file
 * \param pFilename pointer to array of char with teh filename
 */
template<class T>
void svt_volume<T>::writeDF3( const char* pFilename )
{
    unsigned long iNVox, iCount;
    iNVox = m_iSizeX * m_iSizeY * m_iSizeZ;
    FILE* pFile = fopen(pFilename, "wb");

    if( pFile == NULL )
    {
	svtout << "Error: Can't write to file: " << pFilename << endl;
	exit(1);
    }

    Int16 iSX = m_iSizeX;
    Int16 iSY = m_iSizeY;
    Int16 iSZ = m_iSizeZ;

    svtout << "Writing df3 file: " << iSX << " x " << iSY << " x " << iSZ << " - bytes: " << iNVox+6 << endl;

    svt_swapInt16(&iSX);
    svt_swapInt16(&iSY);
    svt_swapInt16(&iSZ);

    size_t iT;

    iT = fwrite(&iSX, 2, 1, pFile);
    iT = fwrite(&iSY, 2, 1, pFile);
    iT = fwrite(&iSZ, 2, 1, pFile);

    T fMax = getMaxDensity();
    T fMin = getMinDensity();
    T fLength = fMax - fMin;

    T tData;
    UInt8 iData;
    for(iCount=0; iCount<iNVox; iCount++)
    {
        tData = ((at(iCount) - fMin) / fLength) * 255.0;
        iData = (Int8)tData;
	iT = fwrite(&iData, 1, 1, pFile);
    }

    fclose(pFile);

    return;
};

/**
 * Non-recursive flood-fill segmentation algorithm. All voxels that are connected to a start voxel and are above the threshold are kept, the others are removed.
 * The algorithm creates a mask that is later blurred by a Gaussian kernel. The sigma of the gaussian can be specified.
 * \param iStartX x index of the start voxel
 * \param iStartY y index of the start voxel
 * \param iStartZ z index of the start voxel
 * \param fTreshold threshold for the floodfill
 * \param fGaussian sigma of the gaussian the mask is convoluted with (if 0, no blurring of the mask is applied)
 */
template<class T>
void svt_volume<T>::floodfill_segmentation( unsigned int iStartX, unsigned int iStartY, unsigned int iStartZ, T fThreshold, Real64 fGaussian )
{
    svt_volume<T> oVisited( this->m_iSizeX, this->m_iSizeY, this->m_iSizeZ );
    oVisited = (T)(0);

    stack< svt_vector3<int> > oQueue;
    stack< svt_vector3<int> > oQueueNotIn;
    svt_vector3<int> oStart( iStartX, iStartY, iStartZ );
    oQueue.push( oStart );

    if (this->getValue( iStartX, iStartY, iStartZ ) < fThreshold)
	svtout << "Warning: Start voxel is lower than threshold in floodfill!" << endl;

    svt_vector3<int> oCurrent;
    svt_vector3<int> oTemp;

    svtout << "Floodfill: Generating mask..." << endl;

    unsigned int iRegion = 0;
    unsigned int iNVox = m_iSizeX * m_iSizeY * m_iSizeZ;
    unsigned int iTotalRegion = iNVox;
    unsigned int iRadius = 10;

    try
    {
        svt_exception::ui()->progressPopup("Floodfill...", 0, iTotalRegion);
        while( (oQueue.size() > 0 || oQueueNotIn.size() > 0 ) )
        {
            // select new region to be manipulated, if no voxels are left in the current region
            if (iRegion == 0 || oQueue.size() == 0)
            {
                for(unsigned int i=0; i<oQueueNotIn.size(); i++)
                {
                    oQueue.push( oQueueNotIn.top() );
                    oQueueNotIn.pop();
                }

                unsigned int iSX = 0;
                unsigned int iEX = m_iSizeX;
                unsigned int iSY = 0;
                unsigned int iEY = m_iSizeY;
                unsigned int iSZ = 0;
                unsigned int iEZ = m_iSizeZ;

                if (iStartX > iRadius )
                    iSX = iStartX - iRadius;
                if (iStartY > iRadius )
                    iSY = iStartY - iRadius;
                if (iStartZ > iRadius )
                    iSZ = iStartZ - iRadius;
                if (iStartX + iRadius < m_iSizeX)
                    iEX = iStartX + iRadius;
                if (iStartY + iRadius < m_iSizeY)
                    iEY = iStartY + iRadius;
                if (iStartZ + iRadius < m_iSizeZ)
                    iEZ = iStartZ + iRadius;

                for(unsigned int iX = iSX; iX < iEX; iX++)
                    for(unsigned int iY = iSY; iY < iEY; iY++)
                        for(unsigned int iZ = iSZ; iZ < iEZ; iZ++)
                            if ( sqrt((iX - iStartX)*(iX - iStartX) + (iY - iStartY)*(iY - iStartY) + (iZ - iStartZ)*(iZ - iStartZ) ) < iRadius )
                                if (oVisited.getValue(iX, iY, iZ) == 0)
                                {
                                    oVisited.setValue(iX, iY, iZ, 3);
                                    iRegion++;
                                    iTotalRegion--;
                                }
                iRadius += 10;

                svt_exception::ui()->progress( iNVox - iTotalRegion, iNVox );
            }

            oCurrent = oQueue.top(); oQueue.pop();

            if (oVisited.getValue(oCurrent.x(), oCurrent.y(), oCurrent.z()) == 0)
            {
                oQueueNotIn.push( oCurrent );

            } else if (oVisited.getValue(oCurrent.x(), oCurrent.y(), oCurrent.z()) == 3)
            {
                iRegion--;

                if (this->getValue(oCurrent.x(), oCurrent.y(), oCurrent.z()) > fThreshold)
                {
                    oVisited.setValue( oCurrent.x(), oCurrent.y(), oCurrent.z(), 1 );

                    // z = 0

                    // x-1 y+1 z+0
                    if (oCurrent.x()-1 > 0 && oCurrent.y()+1 < (int)this->m_iSizeY && (oVisited.getValue(oCurrent.x()-1, oCurrent.y()+1, oCurrent.z()) == 0 || oVisited.getValue(oCurrent.x()-1, oCurrent.y()+1, oCurrent.z()) == 3))
                    {
                        oTemp = oCurrent;
                        oTemp.x( oTemp.x() - 1 );
                        oTemp.y( oTemp.y() + 1 );
                        oQueue.push( oTemp );
                    }
                    // x+0 y+1 z+0
                    if (oCurrent.y()+1 < (int)this->m_iSizeY && (oVisited.getValue(oCurrent.x(), oCurrent.y()+1, oCurrent.z()) == 0 || oVisited.getValue(oCurrent.x(), oCurrent.y()+1, oCurrent.z()) == 3))
                    {
                        oTemp = oCurrent;
                        oTemp.y( oTemp.y() + 1 );
                        oQueue.push( oTemp );
                    }
                    // x+1 y+1 z+0
                    if (oCurrent.x()+1 < (int)this->m_iSizeX && oCurrent.y()+1 < (int)this->m_iSizeY && (oVisited.getValue(oCurrent.x()+1, oCurrent.y()+1, oCurrent.z()) == 0 || oVisited.getValue(oCurrent.x()+1, oCurrent.y()+1, oCurrent.z()) == 3))
                    {
                        oTemp = oCurrent;
                        oTemp.x( oTemp.x() + 1 );
                        oTemp.y( oTemp.y() + 1 );
                        oQueue.push( oTemp );
                    }

                    // x-1 y+0 z+0
                    if (oCurrent.x()-1 > 0 && (oVisited.getValue(oCurrent.x()-1, oCurrent.y(), oCurrent.z()) == 0 || oVisited.getValue(oCurrent.x()-1, oCurrent.y(), oCurrent.z()) == 3))
                    {
                        oTemp = oCurrent;
                        oTemp.x( oTemp.x() - 1 );
                        oQueue.push( oTemp );
                    }
                    // x+1 y+0 z+0
                    if (oCurrent.x()+1 < (int)this->m_iSizeX && (oVisited.getValue(oCurrent.x()+1, oCurrent.y(), oCurrent.z()) == 0 || oVisited.getValue(oCurrent.x()+1, oCurrent.y(), oCurrent.z()) == 3))
                    {
                        oTemp = oCurrent;
                        oTemp.x( oTemp.x() + 1 );
                        oQueue.push( oTemp );
                    }

                    // x-1 y-1 z+0
                    if (oCurrent.x()-1 > 0 && oCurrent.y()-1 > 0 && (oVisited.getValue(oCurrent.x()-1, oCurrent.y(), oCurrent.z()) == 0 || oVisited.getValue(oCurrent.x()-1, oCurrent.y(), oCurrent.z()) == 3))
                    {
                        oTemp = oCurrent;
                        oTemp.x( oTemp.x() - 1 );
                        oTemp.y( oTemp.y() - 1 );
                        oQueue.push( oTemp );
                    }
                    // x+0 y-1 z+0
                    if (oCurrent.y()-1 > 0 && (oVisited.getValue(oCurrent.x(), oCurrent.y()-1, oCurrent.z()) == 0 || oVisited.getValue(oCurrent.x(), oCurrent.y()-1, oCurrent.z()) == 3))
                    {
                        oTemp = oCurrent;
                        oTemp.y( oTemp.y() - 1 );
                        oQueue.push( oTemp );
                    }
                    // x+1 y-1 z+0
                    if (oCurrent.x()+1 < (int)this->m_iSizeX && oCurrent.y()-1 > 0 && (oVisited.getValue(oCurrent.x()+1, oCurrent.y(), oCurrent.z()) == 0 || oVisited.getValue(oCurrent.x()+1, oCurrent.y(), oCurrent.z()) == 3))
                    {
                        oTemp = oCurrent;
                        oTemp.x( oTemp.x() + 1 );
                        oTemp.y( oTemp.y() - 1 );
                        oQueue.push( oTemp );
                    }

                    // z = 1
                    if (oCurrent.z()+1 < (int)this->m_iSizeZ)
                    {

                        // x-1 y+1 z+1
                        if (oCurrent.x()-1 > 0 && oCurrent.y()+1 < (int)this->m_iSizeY && (oVisited.getValue(oCurrent.x()-1, oCurrent.y()+1, oCurrent.z()+1) == 0 || oVisited.getValue(oCurrent.x()-1, oCurrent.y()+1, oCurrent.z()+1) == 3))
                        {
                            oTemp = oCurrent;
                            oTemp.x( oTemp.x() - 1 );
                            oTemp.y( oTemp.y() + 1 );
                            oTemp.z( oTemp.z() + 1 );
                            oQueue.push( oTemp );
                        }
                        // x+0 y+1 z+1
                        if (oCurrent.y()+1 < (int)this->m_iSizeY && (oVisited.getValue(oCurrent.x(), oCurrent.y()+1, oCurrent.z()+1) == 0 || oVisited.getValue(oCurrent.x(), oCurrent.y()+1, oCurrent.z()+1) == 3))
                        {
                            oTemp = oCurrent;
                            oTemp.y( oTemp.y() + 1 );
                            oTemp.z( oTemp.z() + 1 );
                            oQueue.push( oTemp );
                        }
                        // x+1 y+1 z+1
                        if (oCurrent.x()+1 < (int)this->m_iSizeX && oCurrent.y()+1 < (int)this->m_iSizeY && (oVisited.getValue(oCurrent.x()+1, oCurrent.y()+1, oCurrent.z()+1) == 0 || oVisited.getValue(oCurrent.x()+1, oCurrent.y()+1, oCurrent.z()+1) == 3))
                        {
                            oTemp = oCurrent;
                            oTemp.x( oTemp.x() + 1 );
                            oTemp.y( oTemp.y() + 1 );
                            oTemp.z( oTemp.z() + 1 );
                            oQueue.push( oTemp );
                        }

                        // x-1 y+0 z+1
                        if (oCurrent.x()-1 > 0 && (oVisited.getValue(oCurrent.x()-1, oCurrent.y(), oCurrent.z()+1) == 0 || oVisited.getValue(oCurrent.x()-1, oCurrent.y(), oCurrent.z()+1) == 3))
                        {
                            oTemp = oCurrent;
                            oTemp.x( oTemp.x() - 1 );
                            oTemp.z( oTemp.z() + 1 );
                            oQueue.push( oTemp );
                        }
                        // x+0 y+0 z+1
                        if (oVisited.getValue(oCurrent.x(), oCurrent.y(), oCurrent.z()+1) == 0 || oVisited.getValue(oCurrent.x(), oCurrent.y(), oCurrent.z()+1) == 3)
                        {
                            oTemp = oCurrent;
                            oTemp.z( oTemp.z() + 1 );
                            oQueue.push( oTemp );
                        }
                        // x+1 y+0 z+1
                        if (oCurrent.x()+1 < (int)this->m_iSizeX && (oVisited.getValue(oCurrent.x()+1, oCurrent.y(), oCurrent.z()+1) == 0 || oVisited.getValue(oCurrent.x()+1, oCurrent.y(), oCurrent.z()+1) == 3))
                        {
                            oTemp = oCurrent;
                            oTemp.x( oTemp.x() + 1 );
                            oTemp.z( oTemp.z() + 1 );
                            oQueue.push( oTemp );
                        }

                        // x-1 y-1 z+1
                        if (oCurrent.x()-1 > 0 && oCurrent.y()-1 > 0 && (oVisited.getValue(oCurrent.x()-1, oCurrent.y()-1, oCurrent.z()+1) == 0 || oVisited.getValue(oCurrent.x()-1, oCurrent.y()-1, oCurrent.z()+1) == 3))
                        {
                            oTemp = oCurrent;
                            oTemp.x( oTemp.x() - 1 );
                            oTemp.y( oTemp.y() - 1 );
                            oTemp.z( oTemp.z() + 1 );
                            oQueue.push( oTemp );
                        }
                        // x+0 y-1 z+1
                        if (oCurrent.y()-1 > 0 && (oVisited.getValue(oCurrent.x(), oCurrent.y()-1, oCurrent.z()+1) == 0 || oVisited.getValue(oCurrent.x(), oCurrent.y()-1, oCurrent.z()+1) == 3))
                        {
                            oTemp = oCurrent;
                            oTemp.y( oTemp.y() - 1 );
                            oTemp.z( oTemp.z() + 1 );
                            oQueue.push( oTemp );
                        }
                        // x+1 y-1 z+1
                        if (oCurrent.x()+1 < (int)this->m_iSizeX && oCurrent.y()-1 > 0 && (oVisited.getValue(oCurrent.x()+1, oCurrent.y()-1, oCurrent.z()+1) == 0 || oVisited.getValue(oCurrent.x()+1, oCurrent.y()-1, oCurrent.z()+1) == 3))
                        {
                            oTemp = oCurrent;
                            oTemp.x( oTemp.x() + 1 );
                            oTemp.y( oTemp.y() - 1 );
                            oTemp.z( oTemp.z() + 1 );
                            oQueue.push( oTemp );
                        }

                    }

                    // z = -1
                    if (oCurrent.z()-1 > 0)
                    {

                        // x-1 y+1 z-1
                        if (oCurrent.x()-1 > 0 && oCurrent.y()+1 < (int)this->m_iSizeY && (oVisited.getValue(oCurrent.x()-1, oCurrent.y()+1, oCurrent.z()-1) == 0 || oVisited.getValue(oCurrent.x()-1, oCurrent.y()+1, oCurrent.z()-1) == 3))
                        {
                            oTemp = oCurrent;
                            oTemp.x( oTemp.x() - 1 );
                            oTemp.y( oTemp.y() + 1 );
                            oTemp.z( oTemp.z() - 1 );
                            oQueue.push( oTemp );
                        }
                        // x+0 y+1 z-1
                        if (oCurrent.y()+1 < (int)this->m_iSizeY && (oVisited.getValue(oCurrent.x(), oCurrent.y()+1, oCurrent.z()-1) == 0 || oVisited.getValue(oCurrent.x(), oCurrent.y()+1, oCurrent.z()-1) == 3))
                        {
                            oTemp = oCurrent;
                            oTemp.y( oTemp.y() + 1 );
                            oTemp.z( oTemp.z() - 1 );
                            oQueue.push( oTemp );
                        }
                        // x+1 y+1 z-1
                        if (oCurrent.x()+1 < (int)this->m_iSizeX && oCurrent.y()+1 < (int)this->m_iSizeY && (oVisited.getValue(oCurrent.x()+1, oCurrent.y()+1, oCurrent.z()-1) == 0 || oVisited.getValue(oCurrent.x()+1, oCurrent.y()+1, oCurrent.z()-1) == 3))
                        {
                            oTemp = oCurrent;
                            oTemp.x( oTemp.x() + 1 );
                            oTemp.y( oTemp.y() + 1 );
                            oTemp.z( oTemp.z() - 1 );
                            oQueue.push( oTemp );
                        }

                        // x-1 y+0 z-1
                        if (oCurrent.x()-1 > 0 && (oVisited.getValue(oCurrent.x()-1, oCurrent.y(), oCurrent.z()-1) == 0 || oVisited.getValue(oCurrent.x()-1, oCurrent.y(), oCurrent.z()-1) == 3))
                        {
                            oTemp = oCurrent;
                            oTemp.x( oTemp.x() - 1 );
                            oTemp.z( oTemp.z() - 1 );
                            oQueue.push( oTemp );
                        }
                        // x+0 y+0 z-1
                        if (oVisited.getValue(oCurrent.x(), oCurrent.y(), oCurrent.z()-1) == 0 || oVisited.getValue(oCurrent.x(), oCurrent.y(), oCurrent.z()-1) == 3)
                        {
                            oTemp = oCurrent;
                            oTemp.z( oTemp.z() - 1 );
                            oQueue.push( oTemp );
                        }
                        // x+1 y+0 z-1
                        if (oCurrent.x()+1 < (int)this->m_iSizeX && (oVisited.getValue(oCurrent.x()+1, oCurrent.y(), oCurrent.z()-1) == 0 || oVisited.getValue(oCurrent.x()+1, oCurrent.y(), oCurrent.z()-1) == 3))
                        {
                            oTemp = oCurrent;
                            oTemp.x( oTemp.x() + 1 );
                            oTemp.z( oTemp.z() - 1 );
                            oQueue.push( oTemp );
                        }

                        // x-1 y-1 z-1
                        if (oCurrent.x()-1 > 0 && oCurrent.y()-1 > 0  && (oVisited.getValue(oCurrent.x()-1, oCurrent.y(), oCurrent.z()-1) == 0 || oVisited.getValue(oCurrent.x()-1, oCurrent.y(), oCurrent.z()-1) == 3))
                        {
                            oTemp = oCurrent;
                            oTemp.x( oTemp.x() - 1 );
                            oTemp.y( oTemp.y() - 1 );
                            oTemp.z( oTemp.z() - 1 );
                            oQueue.push( oTemp );
                        }
                        // x+0 y-1 z-1
                        if (   (oCurrent.y()-1 > 0 && oVisited.getValue(oCurrent.x(), oCurrent.y()-1, oCurrent.z()-1) == 0)
                            || (oCurrent.y()-1 > 0 && oVisited.getValue(oCurrent.x(), oCurrent.y()-1, oCurrent.z()-1) == 3) )
                        {
                            oTemp = oCurrent;
                            oTemp.y( oTemp.y() - 1 );
                            oTemp.z( oTemp.z() - 1 );
                            oQueue.push( oTemp );
                        }
                        // x+1 y-1 z-1
                        if (oCurrent.x()+1 < (int)this->m_iSizeX && oCurrent.y()-1 > 0 && (oVisited.getValue(oCurrent.x()+1, oCurrent.y()-1, oCurrent.z()-1) == 0 || oVisited.getValue(oCurrent.x()+1, oCurrent.y()-1, oCurrent.z()-1) == 3))
                        {
                            oTemp = oCurrent;
                            oTemp.x( oTemp.x() + 1 );
                            oTemp.y( oTemp.y() - 1 );
                            oTemp.z( oTemp.z() - 1 );
                            oQueue.push( oTemp );
                        }

                    }

                } else
                    oVisited.setValue( oCurrent.x(), oCurrent.y(), oCurrent.z(), 2 );
            }
        }
    } catch (svt_userInterrupt&)
    {
        svtout << "Floodfill canceled..." << endl;
        svt_exception::ui()->progressPopdown();
        return;
    }
    svt_exception::ui()->progressPopdown();

    unsigned int iNum = this->m_iSizeX * this->m_iSizeY * this->m_iSizeZ;
    bool bOnlyNull = true;
    for(unsigned i=0; i<iNum; i++)
    {
        if (oVisited.at(i) != 0 )
        {
            bOnlyNull = false;
            if (oVisited.at(i) == 2 || oVisited.at(i) == 3)
                oVisited.setAt(i, T(0));
        }
    }

    if (bOnlyNull == false && fGaussian > 0.0)
    {
        svtout << "   Convolving mask with Gaussian kernel..." << endl;

	svt_volume<T> oGaussian;
	oGaussian.createGaussian(fGaussian,1);
	oVisited.convolve(oGaussian, true);
	oVisited.normalize();
    }

    svtout << "   Applying mask..." << endl;

    for(unsigned i=0; i<iNum; i++)
	this->setAt(i, this->at(i) * oVisited.at(i));

    this->m_bChanged = true;

    svtout << "   Done." << endl;
};

/**
 * Non-recursive flood-fill algorithm. The algorithm only fills the voxels that are above the threshold (and that are connected to the start voxel) with the specified value.
 * \param iStartX x index of the start voxel
 * \param iStartY y index of the start voxel
 * \param iStartZ z index of the start voxel
 * \param fTreshold threshold for the floodfill
 * \param fGaussian sigma of the gaussian the mask is convoluted with (if 0, no blurring of the mask is applied)
 */
template<class T>
void svt_volume<T>::floodfill( unsigned int iStartX, unsigned int iStartY, unsigned int iStartZ, T fThreshold, T fFillValue )
{
    svt_volume<T> oVisited( this->m_iSizeX, this->m_iSizeY, this->m_iSizeZ );
    oVisited = (T)(0);

    stack< svt_vector3<int> > oQueue;
    svt_vector3<int> oStart( iStartX, iStartY, iStartZ );
    oQueue.push( oStart );

    if (this->getValue( iStartX, iStartY, iStartZ ) < fThreshold)
	svtout << "Warning: Start voxel is lower than threshold in floodfill!" << endl;

    svt_vector3<int> oCurrent;
    svt_vector3<int> oTemp;

    svtout << "Floodfill: Generating mask..." << endl;

    while( oQueue.size() > 0)
    {
	oCurrent = oQueue.top(); oQueue.pop();

	if (oVisited.getValue(oCurrent.x(), oCurrent.y(), oCurrent.z()) == 0)
	{
	    if (this->getValue(oCurrent.x(), oCurrent.y(), oCurrent.z()) > fThreshold)
	    {
		oVisited.setValue( oCurrent.x(), oCurrent.y(), oCurrent.z(), 1 );

		// z = 0

		// x-1 y+1 z+0
		if (oCurrent.x()-1 >=0 && oCurrent.y()+1 < (int)this->m_iSizeY)
		{
		    oTemp = oCurrent;
		    oTemp.x( oTemp.x() - 1 );
		    oTemp.y( oTemp.y() + 1 );
		    oQueue.push( oTemp );
		}
		// x+0 y+1 z+0
		if (oCurrent.y()+1 < (int)this->m_iSizeY)
		{
		    oTemp = oCurrent;
		    oTemp.y( oTemp.y() + 1 );
		    oQueue.push( oTemp );
		}
		// x+1 y+1 z+0
		if (oCurrent.x()+1 < (int)this->m_iSizeX && oCurrent.y()+1 < (int)this->m_iSizeY)
		{
		    oTemp = oCurrent;
		    oTemp.x( oTemp.x() + 1 );
		    oTemp.y( oTemp.y() + 1 );
		    oQueue.push( oTemp );
		}

		// x-1 y+0 z+0
		if (oCurrent.x()-1 >=0)
		{
		    oTemp = oCurrent;
		    oTemp.x( oTemp.x() - 1 );
		    oQueue.push( oTemp );
		}
		// x+1 y+0 z+0
		if (oCurrent.x()+1 < (int)this->m_iSizeX)
		{
		    oTemp = oCurrent;
		    oTemp.x( oTemp.x() + 1 );
		    oQueue.push( oTemp );
		}

		// x-1 y-1 z+0
		if (oCurrent.x()-1 >=0 && oCurrent.y()-1 >=0)
		{
		    oTemp = oCurrent;
		    oTemp.x( oTemp.x() - 1 );
		    oTemp.y( oTemp.y() - 1 );
		    oQueue.push( oTemp );
		}
		// x+0 y-1 z+0
		if (oCurrent.y()-1 >=0)
		{
		    oTemp = oCurrent;
		    oTemp.y( oTemp.y() - 1 );
		    oQueue.push( oTemp );
		}
		// x+1 y-1 z+0
		if (oCurrent.x()+1 < (int)this->m_iSizeX && oCurrent.y()-1 >=0)
		{
		    oTemp = oCurrent;
		    oTemp.x( oTemp.x() + 1 );
		    oTemp.y( oTemp.y() - 1 );
		    oQueue.push( oTemp );
		}

		// z = 1
		if (oCurrent.z()+1 < (int)this->m_iSizeZ)
		{

		    // x-1 y+1 z+1
		    if (oCurrent.x()-1 >=0 && oCurrent.y()+1 < (int)this->m_iSizeY)
		    {
			oTemp = oCurrent;
			oTemp.x( oTemp.x() - 1 );
			oTemp.y( oTemp.y() + 1 );
			oTemp.z( oTemp.z() + 1 );
			oQueue.push( oTemp );
		    }
		    // x+0 y+1 z+1
		    if (oCurrent.y()+1 < (int)this->m_iSizeY)
		    {
			oTemp = oCurrent;
			oTemp.y( oTemp.y() + 1 );
			oTemp.z( oTemp.z() + 1 );
			oQueue.push( oTemp );
		    }
		    // x+1 y+1 z+1
		    if (oCurrent.x()+1 < (int)this->m_iSizeX && oCurrent.y()+1 < (int)this->m_iSizeY)
		    {
			oTemp = oCurrent;
			oTemp.x( oTemp.x() + 1 );
			oTemp.y( oTemp.y() + 1 );
			oTemp.z( oTemp.z() + 1 );
			oQueue.push( oTemp );
		    }

		    // x-1 y+0 z+1
		    if (oCurrent.x()-1 >=0)
		    {
			oTemp = oCurrent;
			oTemp.x( oTemp.x() - 1 );
			oTemp.z( oTemp.z() + 1 );
			oQueue.push( oTemp );
		    }
		    // x+0 y+0 z+1
		    {
			oTemp = oCurrent;
			oTemp.z( oTemp.z() + 1 );
			oQueue.push( oTemp );
		    }
		    // x+1 y+0 z+1
		    if (oCurrent.x()+1 < (int)this->m_iSizeX)
		    {
			oTemp = oCurrent;
			oTemp.x( oTemp.x() + 1 );
			oTemp.z( oTemp.z() + 1 );
			oQueue.push( oTemp );
		    }

		    // x-1 y-1 z+1
		    if (oCurrent.x()-1 >=0 && oCurrent.y()-1 >=0)
		    {
			oTemp = oCurrent;
			oTemp.x( oTemp.x() - 1 );
			oTemp.y( oTemp.y() - 1 );
			oTemp.z( oTemp.z() + 1 );
			oQueue.push( oTemp );
		    }
		    // x+0 y-1 z+1
		    if (oCurrent.y()-1 >=0)
		    {
			oTemp = oCurrent;
			oTemp.y( oTemp.y() - 1 );
			oTemp.z( oTemp.z() + 1 );
			oQueue.push( oTemp );
		    }
		    // x+1 y-1 z+1
		    if (oCurrent.x()+1 < (int)this->m_iSizeX && oCurrent.y()-1 >=0)
		    {
			oTemp = oCurrent;
			oTemp.x( oTemp.x() + 1 );
			oTemp.y( oTemp.y() - 1 );
			oTemp.z( oTemp.z() + 1 );
			oQueue.push( oTemp );
		    }

		}

		// z = -1
		if (oCurrent.z()-1 >=0)
		{

		    // x-1 y+1 z-1
		    if (oCurrent.x()-1 >=0 && oCurrent.y()+1 < (int)this->m_iSizeY)
		    {
			oTemp = oCurrent;
			oTemp.x( oTemp.x() - 1 );
			oTemp.y( oTemp.y() + 1 );
			oTemp.z( oTemp.z() - 1 );
			oQueue.push( oTemp );
		    }
		    // x+0 y+1 z-1
		    if (oCurrent.y()+1 < (int)this->m_iSizeY)
		    {
			oTemp = oCurrent;
			oTemp.y( oTemp.y() + 1 );
			oTemp.z( oTemp.z() - 1 );
			oQueue.push( oTemp );
		    }
		    // x+1 y+1 z-1
		    if (oCurrent.x()+1 < (int)this->m_iSizeX && oCurrent.y()+1 < (int)this->m_iSizeY)
		    {
			oTemp = oCurrent;
			oTemp.x( oTemp.x() + 1 );
			oTemp.y( oTemp.y() + 1 );
			oTemp.z( oTemp.z() - 1 );
			oQueue.push( oTemp );
		    }

		    // x-1 y+0 z-1
		    if (oCurrent.x()-1 >=0)
		    {
			oTemp = oCurrent;
			oTemp.x( oTemp.x() - 1 );
			oTemp.z( oTemp.z() - 1 );
			oQueue.push( oTemp );
		    }
		    // x+0 y+0 z-1
		    {
			oTemp = oCurrent;
			oTemp.z( oTemp.z() - 1 );
			oQueue.push( oTemp );
		    }
		    // x+1 y+0 z-1
		    if (oCurrent.x()+1 < (int)this->m_iSizeX)
		    {
			oTemp = oCurrent;
			oTemp.x( oTemp.x() + 1 );
			oTemp.z( oTemp.z() - 1 );
			oQueue.push( oTemp );
		    }

		    // x-1 y-1 z-1
		    if (oCurrent.x()-1 >=0 && oCurrent.y()-1 >=0)
		    {
			oTemp = oCurrent;
			oTemp.x( oTemp.x() - 1 );
			oTemp.y( oTemp.y() - 1 );
			oTemp.z( oTemp.z() - 1 );
			oQueue.push( oTemp );
		    }
		    // x+0 y-1 z-1
		    if (oCurrent.y()-1 >=0)
		    {
			oTemp = oCurrent;
			oTemp.y( oTemp.y() - 1 );
			oTemp.z( oTemp.z() - 1 );
			oQueue.push( oTemp );
		    }
		    // x+1 y-1 z-1
		    if (oCurrent.x()+1 < (int)this->m_iSizeX && oCurrent.y()-1 >=0)
		    {
			oTemp = oCurrent;
			oTemp.x( oTemp.x() + 1 );
			oTemp.y( oTemp.y() - 1 );
			oTemp.z( oTemp.z() - 1 );
			oQueue.push( oTemp );
		    }

		}

	    } else
		oVisited.setValue( oCurrent.x(), oCurrent.y(), oCurrent.z(), 2 );
	}
    }

    svtout << "   Applying mask..." << endl;

    unsigned int iNum = this->m_iSizeX * this->m_iSizeY * this->m_iSizeZ;
    for(unsigned i=0; i<iNum; i++)
	if (oVisited.at(i) == 1)
	    this->setAt(i, fFillValue);

    this->m_bChanged = true;

    svtout << "   Done." << endl;
};

///////////////////////////////////////////////////////////////////////////////
// MRC File Format
///////////////////////////////////////////////////////////////////////////////

/* Header for MRC file format */
/* Adapted from EMAN2 nightly build 12/05/05 by Steve Ludtke */

/* Label sizes */
enum { MRC_LABEL_SIZE = 80 };
enum { MRC_USER       = 25 };
enum { MRC_NUM_LABELS = 10 };

/*  The different modes supported by the MRC format. */
enum { MRC_MODE_char          = 0 };
enum { MRC_MODE_short         = 1 };
enum { MRC_MODE_float         = 2 };
enum { MRC_MODE_short_COMPLEX = 3 };
enum { MRC_MODE_float_COMPLEX = 4 };
enum { MRC_MODE_long          = 6 };

/* Data order */
enum { MRC_DATA_ORDER_XYZ,
       MRC_DATA_ORDER_XZY,
       MRC_DATA_ORDER_YXZ,
       MRC_DATA_ORDER_YZX,
       MRC_DATA_ORDER_ZXY,
       MRC_DATA_ORDER_ZYX };

#define EPSILON 0.02

/**
 * Simple internal routine that calculates the x,y,z index of a voxel
 * \param iIndex number of the voxel
 * \param iDataOrder data order
 * \param rX reference to unsigned int
 * \param rY reference to unsigned int
 * \param rZ reference to unsigned int
 */
template<class T>
void svt_volume<T>::calc_xyz( unsigned int iIndex, unsigned int iDataOrder, unsigned int iMaxX, unsigned int iMaxY, unsigned int, unsigned int& rX, unsigned int& rY, unsigned int& rZ )
{
    int xo = 0;
    int yo = 0;
    int zo = 0;

    zo = iIndex / (iMaxX*iMaxY);
    yo = (iIndex - zo*iMaxX*iMaxY) / iMaxX;
    xo = iIndex - zo*iMaxX*iMaxY - yo*iMaxX;

    switch (iDataOrder)
    {

    case MRC_DATA_ORDER_XYZ:
        rX  = xo;
        rY  = yo;
        rZ  = zo;
        break;

    case MRC_DATA_ORDER_XZY:
        rX  = xo;
        rY  = zo;
        rZ  = yo;
        break;

    case MRC_DATA_ORDER_YXZ:
        rX  = yo;
        rY  = xo;
        rZ  = zo;
        break;

    case MRC_DATA_ORDER_YZX:
        rX  = yo;
        rY  = zo;
        rZ  = xo;
        break;

    case MRC_DATA_ORDER_ZXY:
        rX  = zo;
        rY  = xo;
        rZ  = yo;
        break;

    case MRC_DATA_ORDER_ZYX:
        rX  = zo;
        rY  = yo;
        rZ  = xo;
        break;

    default:
        svtout << "calc_xyz_index_map: Unknown data order" << endl;
    }
};


/**
 * Simple internal routine that calculates the index of a voxel, depending on the data order
 * \param iIndex number of the voxel
 * \param iDataOrder data order
 */
template<class T>
unsigned int svt_volume<T>::calc_xyz_index( unsigned int iIndex, unsigned int iDataOrder )
{
    int xo = 0;
    int yo = 0;
    int zo = 0;
    int x = 0;
    int y = 0;
    int z = 0;
    int idx;
    int nx = 0;
    int ny = 0;
    int nz = 0;

    zo = iIndex / (m_iSizeX*m_iSizeY);
    yo = (iIndex - zo*m_iSizeX*m_iSizeY) / m_iSizeX;
    xo = iIndex - zo*m_iSizeX*m_iSizeY - yo*m_iSizeX;

    switch (iDataOrder)
    {

    case MRC_DATA_ORDER_XYZ:
        x  = xo;
        y  = yo;
        z  = zo;
        nx = m_iSizeX;
        ny = m_iSizeY;
        nz = m_iSizeZ;
        break;

    case MRC_DATA_ORDER_XZY:
        x  = xo;
        y  = zo;
        z  = yo;
        nx = m_iSizeX;
        ny = m_iSizeZ;
        nz = m_iSizeY;
        break;

    case MRC_DATA_ORDER_YXZ:
        x  = yo;
        y  = xo;
        z  = zo;
        nx = m_iSizeY;
        ny = m_iSizeX;
        nz = m_iSizeZ;
        break;

    case MRC_DATA_ORDER_YZX:
        x  = yo;
        y  = zo;
        z  = xo;
        nx = m_iSizeY;
        ny = m_iSizeZ;
        nz = m_iSizeX;
        break;

    case MRC_DATA_ORDER_ZXY:
        x  = zo;
        y  = xo;
        z  = yo;
        nx = m_iSizeZ;
        ny = m_iSizeX;
        nz = m_iSizeY;
        break;

    case MRC_DATA_ORDER_ZYX:
        x  = zo;
        y  = yo;
        z  = xo;
        nx = m_iSizeZ;
        ny = m_iSizeY;
        nz = m_iSizeX;
        break;

    default:
        svtout << "calc_xyz_index_map: Unknown data order" << endl;
    }

    idx = x + nx*y + nx*ny*z;

    return idx;
};

/*******************************************************************************
Protected Data Structures
*******************************************************************************/
typedef struct {
    int         nx;                 /* Number of columns -      */
                                    /* Fastest changing in map. */
    int         ny;

    int         nz;                 /* Number of sections -     */
                                    /* slowest changing in map. */

    int         mode;               /* See modes above. */

    int         nxstart;            /* No. of first column in map, default 0.*/
    int         nystart;            /* No. of first row in map, default 0.*/
    int         nzstart;            /* No. of first section in map,default 0.*/

    int         mx;                 /* Number of intervals along X. */
    int         my;                 /* Number of intervals along Y. */
    int         mz;                 /* Number of intervals along Z. */

    float       xlen;               /* Cell dimensions (Angstroms). */
    float       ylen;               /* Cell dimensions (Angstroms). */
    float       zlen;               /* Cell dimensions (Angstroms). */

    float       alpha;              /* Cell angles (Degrees). */
    float       beta;               /* Cell angles (Degrees). */
    float       gamma;              /* Cell angles (Degrees). */

    int         mapc;               /* Which axis corresponds to Columns.  */
                                    /* (1,2,3 for X,Y,Z.                   */
    int         mapr;               /* Which axis corresponds to Rows.     */
                                    /* (1,2,3 for X,Y,Z.                   */
    int         maps;               /* Which axis corresponds to Sections. */
                                    /* (1,2,3 for X,Y,Z.                   */

    float       amin;               /* Minimum density value. */
    float       amax;               /* Maximum density value. */
    float       amean;              /* Mean density value.    */

    int         ispg;               /* Space group number (0 for images). */

    int         nsymbt;             /* Number of chars used for storing   */
                                    /* symmetry operators. */


    int         user[MRC_USER];

    float       xorigin;            /* X origin. */
    float       yorigin;            /* Y origin. */
    float       zorigin;            /* Y origin. */

    char        map[4];	  	    /* constant string "MAP "  */

    int         machinestamp;	    /* machine stamp in CCP4 convention:   */
				    /*    big endian=0x11110000            */
                                    /*    little endian=0x4444000          */

    float       rms;                /* rms deviation of map from mean density */

    int         nlabl;              /* Number of labels being used. */
                                    /* 10 text labels of 80 characters each. */
    char        labels[MRC_NUM_LABELS][MRC_LABEL_SIZE];

} MRCHEADER;

typedef struct {

    float       nslice;             // # sections
    float       nrow;               // # rows
    float       irec;               // total # records (incl. header)
    float       nhistrec;           // obsolete
    float       iform;              // file type specifier
    float       imami;              // maximum/minimum flag (1 if values are valid)
    float       fmax;               // maximum
    float       fmin;               // minimum
    float       av;                 // average
    float       sig;                // standard deviation
    float       ihist;              // obsolete
    float       nsam;               // # pixels per line
    float       labrec;             // # records in header
    float       iangle;             // flag if tilt angles are present
    float       phi;                // tilt angle
    float       theta;              // tilt angle
    float       gamma;              // tilt angle (also phi)
    float       xoff;               // x translation
    float       yoff;               // y translation
    float       zoff;               // y translation
    float       scale;              // scale factor
    float       labbyt;             // total number of bytes in header
    float       lenbyt;             // record length in bytes
    float       istack;             // if > 0, indicates a stack of images
    float       inuse;              // obsolete
    float       maxim;              // only used for overall header in a stack
    float       imgnum;             // only used for overall header in a stack
    float       lastindx;           // only used for overall header in a stack
    float       unused[2];          // unused
    float       kangle;             // flag for additional angles
    float       phi1;               // additional angles
    float       theta1;
    float       psi1;
    float       phi2;
    float       theta2;
    float       psi2;

} SPIDERHEADER;

/**
 * Reads MRC/CCP4 maps from file
 * \param pFilename pointer to array of char with the filename
 */
template<class T>
void svt_volume<T>::loadMRC( const char *pFilename, unsigned int iMinLoadX, unsigned int iMinLoadY, unsigned int iMinLoadZ, unsigned int iMaxLoadX, unsigned int iMaxLoadY, unsigned int iMaxLoadZ )
{
    FILE          *pFile;
    MRCHEADER     *pMRCHeader;
    unsigned int  i, iCount;
    int           file_endian=0;  // 1 if the file endianess is MSB first
    int           machine_endian=0; // 1 if the machine endianess is MSB first
    char          *m2,w;
    unsigned char *char_data,c;

    svtout << "Reading MRC/CCP4 map from file " << pFilename << endl;

    // Init some vars
    iCount  = 0;

    // Read file header
    pMRCHeader = new MRCHEADER;
    pFile = fopen(pFilename, "rb");

    if (!fread(pMRCHeader,sizeof(MRCHEADER),1,pFile))
    {
        svt_exception::ui()->error("Cannot read header of MRC/CCP4 file.");
        return;
    }

    // Print first label in file
    pMRCHeader->labels[0][79]=0;
    svtout << "  Label: " << pMRCHeader->labels[0] << endl;

    // Swap the byte endianess in the header depending on map and machine endianess
    if (svt_bigEndianMachine())
    {
        machine_endian=1;
    }
    else {
        machine_endian=0;
    }

    m2=(char *) pMRCHeader;
    if (m2[0]==0 && m2[1]==0)
        file_endian=1;
    if (machine_endian ^ file_endian)
    {
        for (i=0; i<56*4; i+=4)
        {
            w=m2[i]; m2[i]=m2[i+3]; m2[i+3]=w;
            w=m2[i+1]; m2[i+1]=m2[i+2]; m2[i+2]=w;
        }
    }

    // Sanity checks for file
    if ( pMRCHeader->ny    > 0xffff || pMRCHeader->nz    > 0xffff || pMRCHeader->mode  == 5 || pMRCHeader->mode  > 6 || pMRCHeader->nlabl > 11)
    {
        svt_exception::ui()->error("Invalid header, cannot load MRC/CCP4 file.");
        return;
    }

    if ( pMRCHeader->nx == 1 || pMRCHeader->ny == 1 || pMRCHeader->nz == 1)
    {
        svt_exception::ui()->error("File does not contain a 3D map, cannot load MRC/CCP4 file.");
        return;
    }

    // Fix number of intervals if none is given
    if ( pMRCHeader->mx == 0 )
        pMRCHeader->mx = pMRCHeader->nx - 1;
    if ( pMRCHeader->my == 0 )
        pMRCHeader->my = pMRCHeader->ny - 1;
    if ( pMRCHeader->mz == 0 )
        pMRCHeader->mz = pMRCHeader->nz - 1;

    // Deal with intervals
    if ((pMRCHeader->mx != pMRCHeader->nx && pMRCHeader->mx != pMRCHeader->nx - 1)
        || (pMRCHeader->my != pMRCHeader->ny && pMRCHeader->my != pMRCHeader->ny - 1)
        || (pMRCHeader->mz != pMRCHeader->nz && pMRCHeader->my != pMRCHeader->ny - 1))
    {

        /*
         warning("read_mrc_map: Number of entries N(%d,%d,%d) -- Intervals M(%d,%d,%d)\n",
         pMRCHeader->nx, pMRCHeader->ny, pMRCHeader->nz,
         pMRCHeader->mx, pMRCHeader->my, pMRCHeader->mz);
         warning("              Adjusting voxel sizes accordingly\n");
         */
    }

    // Error for complex maps
    if ( pMRCHeader->mode == MRC_MODE_float_COMPLEX || pMRCHeader->mode == MRC_MODE_short_COMPLEX)
    {
        svt_exception::ui()->error("Complex maps are not supported right now, cannot load MRC/CCP4 file.");
        m_iSizeX = m_iSizeY = m_iSizeZ = 0;
        return;
    }

    // If lengths are not specified, set voxel size to 1 Angstrom
    if (fabs(pMRCHeader->xlen) < EPSILON)
        pMRCHeader->xlen = (float) pMRCHeader->nx;
    if (fabs(pMRCHeader->ylen) < EPSILON)
        pMRCHeader->ylen = (float) pMRCHeader->ny;
    if (fabs(pMRCHeader->zlen) < EPSILON)
        pMRCHeader->zlen = (float) pMRCHeader->nz;

    // voxel width
    float fWidthX = (pMRCHeader->xlen / (float) pMRCHeader->mx);
    float fWidthY = (pMRCHeader->ylen / (float) pMRCHeader->my);
    float fWidthZ = (pMRCHeader->zlen / (float) pMRCHeader->mz);

    svtout << "  Voxel widths: " << fWidthX << ", " << fWidthY << ", " << fWidthZ << endl;

    if ( fabs(fWidthX - fWidthY) < EPSILON && fabs(fWidthX - fWidthZ) < EPSILON)
        m_fWidth = (fWidthX + fWidthY + fWidthZ) / 3.0;
    else
    {
        svt_exception::ui()->error("Anisotropic maps are not supported right now, cannot load MRC/CCP4 file.");
        m_iSizeX = m_iSizeY = m_iSizeZ = 0;
        return;
    }

    svtout << "  Angles   : " << pMRCHeader->alpha   << ", " << pMRCHeader->beta    << ", " << pMRCHeader->gamma   << endl;
    svtout << "  XYZorigin: " << pMRCHeader->xorigin << ", " << pMRCHeader->yorigin << ", " << pMRCHeader->zorigin << endl;
    svtout << "  NCRSstart: " << pMRCHeader->nxstart << ", " << pMRCHeader->nystart << ", " << pMRCHeader->nzstart << endl;

    // Determine data order
    int iDataOrder = MRC_DATA_ORDER_XYZ;
    if ( pMRCHeader->mapc == 1 && pMRCHeader->mapr == 2 && pMRCHeader->maps == 3)
    {
        iDataOrder = MRC_DATA_ORDER_XYZ;
        svtout << "  Data order: x y z" << endl;

    } else if ( pMRCHeader->mapc == 1 && pMRCHeader->mapr == 3 && pMRCHeader->maps == 2)
    {
        iDataOrder = MRC_DATA_ORDER_XZY;
        svtout << "  Data order: x z y (swapping to xyz while reading)" << endl;

    } else if ( pMRCHeader->mapc == 2 && pMRCHeader->mapr == 1 && pMRCHeader->maps == 3)
    {
        iDataOrder = MRC_DATA_ORDER_YXZ;
        svtout << "  Data order: y x z (swapping to xyz while reading)" << endl;

    } else if ( pMRCHeader->mapc == 2 && pMRCHeader->mapr == 3 && pMRCHeader->maps == 1)
    {
        iDataOrder = MRC_DATA_ORDER_YZX;
        svtout << "  Data order: y z x (swapping to xyz while reading)" << endl;

    } else if ( pMRCHeader->mapc == 3 && pMRCHeader->mapr == 1 && pMRCHeader->maps == 2)
    {
        iDataOrder = MRC_DATA_ORDER_ZXY;
        svtout << "  Data order: z x y (swapping to xyz while reading)" << endl;

    } else if ( pMRCHeader->mapc == 3 && pMRCHeader->mapr == 2 && pMRCHeader->maps == 1)
    {
        iDataOrder = MRC_DATA_ORDER_ZYX;
        svtout << "  Data order: z y x (swapping to xyz while reading)" << endl;
    } else
    {
        svt_exception::ui()->error("Unknown data order, cannot load MRC/CCP4 file.");
        m_iSizeX = m_iSizeY = m_iSizeZ = 0;
        return;
    }

    // Set parameters for map
    m_fGridX = pMRCHeader->xorigin;
    m_fGridY = pMRCHeader->yorigin;
    m_fGridZ = pMRCHeader->zorigin;

    if (
        m_fGridX <=  EPSILON && m_fGridY <=  EPSILON && m_fGridZ <=  EPSILON &&
        m_fGridX >= -EPSILON && m_fGridY >= -EPSILON && m_fGridZ >= -EPSILON
       )
    {
        switch( iDataOrder )
        {
        default:
        case MRC_DATA_ORDER_XYZ:
            m_fGridX = (Real64)(pMRCHeader->nxstart) * m_fWidth;
            m_fGridY = (Real64)(pMRCHeader->nystart) * m_fWidth;
            m_fGridZ = (Real64)(pMRCHeader->nzstart) * m_fWidth;
            break;
        case MRC_DATA_ORDER_XZY:
            m_fGridX = (Real64)(pMRCHeader->nxstart) * m_fWidth;
            m_fGridY = (Real64)(pMRCHeader->nzstart) * m_fWidth;
            m_fGridZ = (Real64)(pMRCHeader->nystart) * m_fWidth;
            break;
        case MRC_DATA_ORDER_YXZ:
            m_fGridX = (Real64)(pMRCHeader->nystart) * m_fWidth;
            m_fGridY = (Real64)(pMRCHeader->nxstart) * m_fWidth;
            m_fGridZ = (Real64)(pMRCHeader->nzstart) * m_fWidth;
            break;
        case MRC_DATA_ORDER_YZX:
            m_fGridX = (Real64)(pMRCHeader->nystart) * m_fWidth;
            m_fGridY = (Real64)(pMRCHeader->nzstart) * m_fWidth;
            m_fGridZ = (Real64)(pMRCHeader->nxstart) * m_fWidth;
            break;
        case MRC_DATA_ORDER_ZXY:
            m_fGridX = (Real64)(pMRCHeader->nzstart) * m_fWidth;
            m_fGridY = (Real64)(pMRCHeader->nxstart) * m_fWidth;
            m_fGridZ = (Real64)(pMRCHeader->nystart) * m_fWidth;
            break;
        case MRC_DATA_ORDER_ZYX:
            m_fGridX = (Real64)(pMRCHeader->nzstart) * m_fWidth;
            m_fGridY = (Real64)(pMRCHeader->nystart) * m_fWidth;
            m_fGridZ = (Real64)(pMRCHeader->nxstart) * m_fWidth;
            break;
        }
    }
    svtout << "  Map origin: " << m_fGridX << ", " << m_fGridY << ", " << m_fGridZ << endl;

    /*if ( fabs(pMRCHeader->alpha - 90.0) < EPSILON && fabs(pMRCHeader->beta - 90.0) < EPSILON && fabs(pMRCHeader->gamma - 90.0) < EPSILON)
    {
        svtout << "  Map is rectangular" << endl;
    } else {
        svt_exception::ui()->error("Map is not rectangular, cannot load MRC/CCP4 file.");
        m_iSizeX = m_iSizeY = m_iSizeZ = 0;
        return;
    }*/

    // Allocate new map
    bool bPartial = false;
    if (iMaxLoadX == 0)
        iMaxLoadX = pMRCHeader->nx;
    if (iMaxLoadY == 0)
        iMaxLoadY = pMRCHeader->ny;
    if (iMaxLoadZ == 0)
        iMaxLoadZ = pMRCHeader->nz;
    try
    {
        allocate( iMaxLoadX - iMinLoadX, iMaxLoadY - iMinLoadY, iMaxLoadZ - iMinLoadZ, (T)(0), false);

    } catch(bad_alloc& ex)
    {
        svt_exception::ui()->info("Unable to satisfy memory allocation request");
        bPartial = svt_exception::ui()->question("Do you want to partially load the file?");
        if (bPartial == false)
        {
            m_iSizeX = m_iSizeY = m_iSizeZ = 0;
            return;
        }
        else
        {
            string oAns;
            oAns = svt_exception::ui()->textInput("Please enter min. x index to load:", "0" );
            iMinLoadX = atoi( oAns.c_str() );

            oAns = svt_exception::ui()->textInput("Please enter max. x index to load:", "64" );
            iMaxLoadX = atoi( oAns.c_str() );

            oAns = svt_exception::ui()->textInput("Please enter min. y index to load:", "0" );
            iMinLoadY = atoi( oAns.c_str() );

            oAns = svt_exception::ui()->textInput("Please enter max. y index to load:", "64" );
            iMaxLoadY = atoi( oAns.c_str() );

            oAns = svt_exception::ui()->textInput("Please enter min. z index to load:", "0" );
            iMinLoadZ = atoi( oAns.c_str() );

            oAns = svt_exception::ui()->textInput("Please enter max. z index to load:", "64" );
            iMaxLoadZ = atoi( oAns.c_str() );

            svtout << "  Allocate space for " << iMaxLoadX - iMinLoadX << " x " << iMaxLoadY - iMinLoadY << " x " << iMaxLoadZ - iMinLoadZ << endl;

            try
            {
                allocate( iMaxLoadX - iMinLoadX, iMaxLoadY - iMinLoadY, iMaxLoadZ - iMinLoadZ, (T)(0), false );

            } catch(bad_alloc& ex2)
            {
                svt_exception::ui()->info("Was again unable to satisfy memory allocation request");
                m_iSizeX = m_iSizeY = m_iSizeZ = 0;
                return;
            }
        }
    }

    svtout << "  Map dimensions: " << m_iSizeX << " x " << m_iSizeY << " x " << m_iSizeZ << endl;

    // Skip symmetry information
    svtout << "  Skipping header and symmetry information (" << sizeof(MRCHEADER)+pMRCHeader->nsymbt << " bytes)" << endl;
    fseek(pFile,sizeof(MRCHEADER)+pMRCHeader->nsymbt,SEEK_SET);

    unsigned int iNVox = pMRCHeader->nx * pMRCHeader->ny * pMRCHeader->nz;

    // Read in actual data
    //signed char   scnumber;
    unsigned char cnumber;
    short         snumber;
    long          lnumber;
    float         fnumber;
    unsigned int  iX, iY, iZ;
    switch(pMRCHeader->mode)
    {

    case MRC_MODE_char:
        svtout <<"  8-bit char data" << endl;

        svt_exception::ui()->progressPopup("Loading MRC/CCP4 File...", 0, iNVox);
        try
        {
            for(i=0; i<iNVox; i++)
            {
                iCount += fread(&cnumber, sizeof(char), 1, pFile);

                calc_xyz(i, iDataOrder, pMRCHeader->nx, pMRCHeader->ny, pMRCHeader->nz, iX, iY, iZ);

                if (
                    iX < iMaxLoadX && iX >= iMinLoadX &&
                    iY < iMaxLoadY && iY >= iMinLoadY &&
                    iZ < iMaxLoadZ && iZ >= iMinLoadZ
                   )
                {
                    setAt( iX - iMinLoadX, iY - iMinLoadY, iZ - iMinLoadZ, (T)( cnumber ) );
                }

                if (iCount%10000 == 0)
                    svt_exception::ui()->progress( iCount, iNVox);
            }

        } catch (svt_userInterrupt&)
        {
            svt_exception::ui()->error("User interrupt, loading aborted.");
            m_iSizeX = m_iSizeY = m_iSizeZ = 0;
            svt_exception::ui()->progressPopdown();
            delete pMRCHeader;
            allocate( 0,0,0, (T)(0), false);
            return;
        }
        svt_exception::ui()->progressPopdown();

        if (iCount != iNVox)
            svtout << "Read only " << iCount << " chars, expecting " << iNVox << endl;
        break;

/*
    case MRC_MODE_char:
        svtout <<"  8-bit char data" << endl;

	svt_exception::ui()->progressPopup("Loading MRC/CCP4 File...", 0, iNVox);
        try
        {
            for(i=0; i<iNVox; i++)
            {
                //iCount += fread(&scnumber, sizeof(signed char), 1, pFile);
                //setAt( calc_xyz_index(i, iDataOrder), (T)scnumber );
                iCount += fread(&cnumber, sizeof(char), 1, pFile);
                setAt( calc_xyz_index(i, iDataOrder), (T)cnumber );

                if (iCount%1000 == 0)
                    svt_exception::ui()->progress( iCount, iNVox);
            }

        } catch (svt_userInterrupt&)
        {
            svt_exception::ui()->error("User interrupt, loading aborted.");
            m_iSizeX = m_iSizeY = m_iSizeZ = 0;
            svt_exception::ui()->progressPopdown();
            delete pMRCHeader;
            allocate( 0,0,0, (T)(0), false);
            return;
        }
        svt_exception::ui()->progressPopdown();

        if (iCount != iNVox)
        {
            svtout << "Read only " << iCount << " signed chars, expecting " << iNVox << endl;
        }
        break;
        */

    case MRC_MODE_short:
        svtout <<"  16 bit integer data" << endl;

        svt_exception::ui()->progressPopup("Loading MRC/CCP4 File...", 0, iNVox);
        try
        {
            for(i=0; i<iNVox; i++)
            {
                iCount += fread(&snumber, sizeof(short), 1, pFile);
                // swap endianess if machine and pFile have different endianess
                if (file_endian + machine_endian == 1)
                {
                    char_data = (unsigned char *) &snumber;
                    c = char_data[0]; char_data[0] = char_data[1]; char_data[1]=c;
                }
                setAt( calc_xyz_index(i, iDataOrder), (T)snumber );

                if (iCount%10000 == 0)
                    svt_exception::ui()->progress( iCount, iNVox);

            }
        } catch (svt_userInterrupt&)
        {
            svt_exception::ui()->error("User interrupt, loading aborted.");
            m_iSizeX = m_iSizeY = m_iSizeZ = 0;
            svt_exception::ui()->progressPopdown();
            delete pMRCHeader;
            allocate( 0,0,0, (T)(0), false);
            return;
        }
        svt_exception::ui()->progressPopdown();

        if (iCount != iNVox)
        {
            svtout << "Read only " << iCount << " shorts, expecting " << iNVox << endl;
        }
        break;

    case MRC_MODE_long:
        svtout <<"  32 bit integer data" << endl;

        svt_exception::ui()->progressPopup("Loading MRC/CCP4 File...", 0, iNVox);
        try
        {
            for(i=0; i<iNVox; i++){
                iCount += fread(&lnumber, sizeof(long), 1, pFile);
                // swap endianess if machine and pFile have different endianess
                if (file_endian + machine_endian == 1)
                {
                    char_data = (unsigned char *) &lnumber;
                    c = char_data[0]; char_data[0] = char_data[3]; char_data[3] = c;
                    c = char_data[1]; char_data[1] = char_data[2]; char_data[2] = c;
                }
                setAt( calc_xyz_index(i, iDataOrder), (T)lnumber );

                if (iCount%10000 == 0)
                    svt_exception::ui()->progress( iCount, iNVox);

            }
        } catch (svt_userInterrupt&)
        {
            svt_exception::ui()->error("User interrupt, loading aborted.");
            m_iSizeX = m_iSizeY = m_iSizeZ = 0;
            svt_exception::ui()->progressPopdown();
            delete pMRCHeader;
            allocate( 0,0,0, (T)(0), false);
            return;
        }
        svt_exception::ui()->progressPopdown();

        if (iCount != iNVox)
        {
            svtout << "Read only " << iCount << " longs, expecting " << iNVox << endl;
        }
        break;

    case MRC_MODE_float:
        svtout <<"  32 bit floating point data" << endl;

        if (file_endian + machine_endian == 1)
            svtout << "  Endianess of file and machine is different, need to swap the bytes." << endl;

        svt_exception::ui()->progressPopup("Loading MRC/CCP4 File...", 0, iNVox);
        try
        {
            for(i=0; i<iNVox; i++)
            {
                iCount += fread(&fnumber, sizeof(Real32), 1, pFile);

                calc_xyz(i, iDataOrder, pMRCHeader->nx, pMRCHeader->ny, pMRCHeader->nz, iX, iY, iZ);

                if (
                    iX < iMaxLoadX && iX >= iMinLoadX &&
                    iY < iMaxLoadY && iY >= iMinLoadY &&
                    iZ < iMaxLoadZ && iZ >= iMinLoadZ
                   )
                {
                    // swap endianess if machine and file have different endianess
                    if (file_endian + machine_endian == 1)
                        svt_swapReal32( &fnumber );

                    setAt( iX - iMinLoadX, iY - iMinLoadY, iZ - iMinLoadZ, (T)( fnumber ) );
                }

                if (iCount%10000 == 0)
                    svt_exception::ui()->progress( iCount, iNVox);
            }

        } catch (svt_userInterrupt&)
        {
            svt_exception::ui()->error("User interrupt, loading aborted.");
            m_iSizeX = m_iSizeY = m_iSizeZ = 0;
            svt_exception::ui()->progressPopdown();
            delete pMRCHeader;
            allocate( 0,0,0, (T)(0), false);
            return;
        }
        svt_exception::ui()->progressPopdown();

        if (iCount != iNVox)
        {
            svtout << "Read only " << iCount << " floats, expecting " << iNVox << endl;
        }
        break;

    default:
        svt_exception::ui()->error("Bad MRC/CCP4 file, loading aborted.");
        m_iSizeX = m_iSizeY = m_iSizeZ = 0;
    }

    delete pMRCHeader;
};

/**
 * Read raw volume file
 * \param pFilename pointer to array of char with the filename
 */
template<class T>
inline void svt_volume<T>::loadRaw(const char* pFilename)
{
    FILE *pFile = fopen(pFilename, "rb");
    if( pFile == NULL )
    {
        char pFname[2560];
        sprintf(pFname, "Can't open raw volume file: \"%s\"!", pFilename );
        svt_exception::ui()->error(pFname);
        return;
    }

    unsigned int iSizeX, iSizeY, iSizeZ;

    string oAns;
    oAns = svt_exception::ui()->textInput("Please enter number of voxels in x dimension:", "0" );
    iSizeX = atoi( oAns.c_str() );
    oAns = svt_exception::ui()->textInput("Please enter number of voxels in y dimension:", "0" );
    iSizeY = atoi( oAns.c_str() );
    oAns = svt_exception::ui()->textInput("Please enter number of voxels in z dimension:", "0" );
    iSizeZ = atoi( oAns.c_str() );

    try
    {
	allocate( iSizeX, iSizeY, iSizeZ, (T)(0), false );

    } catch(bad_alloc& ex2)
    {
	svt_exception::ui()->info("Was unable to satisfy memory allocation request");
	m_iSizeX = m_iSizeY = m_iSizeZ = 0;
	return;
    }

    oAns = svt_exception::ui()->textInput("Please enter the voxelsize:", "1.0" );
    m_fWidth = atof( oAns.c_str() );

    unsigned int iNVox = iSizeX * iSizeY * iSizeZ;

    oAns = svt_exception::ui()->textInput("Please enter datatype (0=char, 1=int16, 2=int32, 3=float):", "3" );
    unsigned int iMode = atoi( oAns.c_str() );

    oAns = svt_exception::ui()->textInput("Please enter size of header to be skipped:", "0" );
    unsigned int iHeader = atoi( oAns.c_str() );

    // Read in actual data
    unsigned int iCount = 0;

    svtout << "Loading the raw volume data file..." << endl;

    // jump over header
    iCount = fread( m_pData, sizeof(char), iHeader, pFile );
    iCount = 0;
    unsigned int i;
    unsigned int iLeft = 0;

    // loading
    float fData;
    unsigned int iData;
    unsigned short sData;
    unsigned char cData;

    switch(iMode)
    {
    case 0:
        svtout <<"  8-bit char data" << endl;

	svt_exception::ui()->progressPopup("Loading raw volume file...", 0, iNVox);

	for(i=0; i<iNVox; i++)
	{
	    iCount += fread(&cData, sizeof(unsigned char), 1, pFile);
            setAt( i,  (T)(cData) );

            if (iCount % 10000 == 0)
		svt_exception::ui()->progress( iCount, iNVox);
	}

	svt_exception::ui()->progressPopdown( );

        if (iCount != iNVox)
        {
            svtout << "Read only " << iCount << " signed chars, expecting " << iNVox << endl;
        }
        break;

    case 1:
        svtout <<"  16-bit integer data" << endl;

	svt_exception::ui()->progressPopup("Loading raw volume file...", 0, iNVox);

	for(i=0; i<iNVox; i++)
	{
	    iCount += fread(&sData, sizeof(unsigned short), 1, pFile);
            setAt( i,  (T)(sData) );

            if (iCount % 10000 == 0)
		svt_exception::ui()->progress( iCount, iNVox);
	}

	svt_exception::ui()->progressPopdown( );

        if (iCount != iNVox)
        {
            svtout << "Read only " << iCount << " shorts, expecting " << iNVox << endl;
        }
        break;

    case 2:
        svtout <<"  32-bit integer data" << endl;

	svt_exception::ui()->progressPopup("Loading raw volume file...", 0, iNVox);

	for(i=0; i<iNVox; i++)
	{
	    iCount += fread(&iData, sizeof(int), 1, pFile);
            setAt( i,  (T)(iData) );

            if (iCount % 10000 == 0)
		svt_exception::ui()->progress( iCount, iNVox);
	}

	svt_exception::ui()->progressPopdown( );

        if (iCount != iNVox)
        {
            svtout << "Read only " << iCount << " longs, expecting " << iNVox << endl;
        }
        break;

    case 3:
        svtout <<"  32-bit floating point data" << endl;

	svt_exception::ui()->progressPopup("Loading raw volume file...", 0, iNVox);

	for(i=0; i<iNVox; i++)
	{
	    iCount += fread(&fData, sizeof(float), 1, pFile);
            setAt( calc_xyz_index(i, MRC_DATA_ORDER_XYZ),  (T)(fData) );

            if (iCount % 10000 == 0)
		svt_exception::ui()->progress( iCount, iNVox);
	}

	while(!feof(pFile))
	    iLeft += fread(&cData, sizeof(char), 1, pFile);

	if (iLeft > 0)
            svtout << "  Still " << iLeft << " bytes in file not read!" << endl;


	svt_exception::ui()->progressPopdown( );

        if (iCount != iNVox)
        {
            svtout << "Read only " << iCount << " floats, expecting " << iNVox << endl;
        }
        break;

    default:
        svt_exception::ui()->error("Bad raw volume file, loading aborted.");
        m_iSizeX = m_iSizeY = m_iSizeZ = 0;
    }

    fclose( pFile );
}

/**
 * Write MRC volume file
 * \param pFilename pointer to array of char with the filename
 */
template<class T>
inline void svt_volume<T>::writeMRC(const char* pFilename)
{
    MRCHEADER *pMRC_Header;
    FILE *pFile;
    unsigned int i;
    float fNumber;
    int iStamp;
    char *pStampBytes;
    unsigned int iNVox = m_iSizeX * m_iSizeY * m_iSizeZ;

    // Allocate header
    pMRC_Header = new MRCHEADER;
    memset(pMRC_Header,0,sizeof(pMRC_Header));

    // Prepare machine stamp (only endianess)
    pStampBytes = (char*)(&iStamp);
    if(svt_bigEndianMachine())
    {
        pStampBytes[0]=0x11;
        pStampBytes[1]=0x11;
        pStampBytes[2]=0;
        pStampBytes[3]=0;

    } else {

        pStampBytes[0]=0x44;
        pStampBytes[1]=0x44;
        pStampBytes[2]=0;
        pStampBytes[3]=0;
    }
    pMRC_Header->machinestamp = iStamp;

    // Data type
    pMRC_Header->mode = MRC_MODE_float;
    //pMRC_Header->mode = MRC_MODE_long;
    //pMRC_Header->mode = MRC_MODE_short;
    //pMRC_Header->mode = MRC_MODE_char;

    // Descriptive information about map
    pMRC_Header->nx      = (int) m_iSizeX;
    pMRC_Header->ny      = (int) m_iSizeY;
    pMRC_Header->nz      = (int) m_iSizeZ;
    pMRC_Header->mx      = (int) m_iSizeX;
    pMRC_Header->my      = (int) m_iSizeY;
    pMRC_Header->mz      = (int) m_iSizeZ;
    pMRC_Header->xlen    = (float) m_iSizeX * m_fWidth;
    pMRC_Header->ylen    = (float) m_iSizeY * m_fWidth;
    pMRC_Header->zlen    = (float) m_iSizeZ * m_fWidth;
    pMRC_Header->xorigin = (float) m_fGridX;
    pMRC_Header->yorigin = (float) m_fGridY;
    pMRC_Header->zorigin = (float) m_fGridZ;
    pMRC_Header->alpha   = (float) 90.0f;
    pMRC_Header->beta    = (float) 90.0f;
    pMRC_Header->gamma   = (float) 90.0f;
    pMRC_Header->amin  = (float) this->getMinDensity();
    pMRC_Header->amax  = (float) this->getMaxDensity();
    pMRC_Header->amean = (float) this->getAvgDensity();

    // Label with date
    pMRC_Header->nlabl = 1;
    sprintf(&pMRC_Header->labels[0][0], "Created by SVT/Sculptor - %02i.%02i.%i", svt_getDay(), svt_getMonth(), svt_getYear());
    pMRC_Header->labels[0][79] = 0;
    pMRC_Header->labels[1][0] = 0;

    // Fixed entries
    pMRC_Header->mapc    = 1;
    pMRC_Header->mapr    = 2;
    pMRC_Header->maps    = 3;

    // if origin / width is an integer, write it into the nxstart fields...
    int iOriginX = (int)(m_fGridX / m_fWidth);
    int iOriginY = (int)(m_fGridY / m_fWidth);
    int iOriginZ = (int)(m_fGridZ / m_fWidth);
    float fOriginX = (float)(m_fGridX / m_fWidth);
    float fOriginY = (float)(m_fGridY / m_fWidth);
    float fOriginZ = (float)(m_fGridZ / m_fWidth);

    if (
        (float)(iOriginX) - fOriginX < EPSILON &&
        (float)(iOriginY) - fOriginY < EPSILON &&
        (float)(iOriginZ) - fOriginZ < EPSILON
       )
    {
        pMRC_Header->nxstart = iOriginX;
        pMRC_Header->nystart = iOriginY;
        pMRC_Header->nzstart = iOriginZ;
    }
    else
    {
        pMRC_Header->nxstart = 0;
        pMRC_Header->nystart = 0;
        pMRC_Header->nzstart = 0;
    }

    pMRC_Header->ispg    = 1;
    pMRC_Header->nsymbt  = 0;
    pMRC_Header->map[0]  = 'M';
    pMRC_Header->map[1]  = 'A';
    pMRC_Header->map[2]  = 'P';
    pMRC_Header->map[3]  = ' ';

    // Write header and data to file
    pFile = fopen(pFilename, "wb");
    fwrite((void *) pMRC_Header,sizeof(MRCHEADER),1,pFile);

    // Write data
    svt_exception::ui()->progressPopup("Writing map to MRC/CCP4 File...", 0, iNVox);
    try
    {
        for(i=0; i<iNVox; i++)
        {
            fNumber = (float) at(i);
            fwrite(&fNumber, sizeof(float), 1, pFile);

            if (i%10000 == 0)
                svt_exception::ui()->progress( i, iNVox);
        }

    } catch (svt_userInterrupt&)
    {
        svt_exception::ui()->error("User interrupt, saving aborted, file incomplete!");
    }
    svt_exception::ui()->progressPopdown();

    //fwrite(map->ldata, sizeof(long), map->nvox, pFile);
    //fwrite(map->sdata, sizeof(short), map->nvox, pFile);
    //fwrite(map->scdata, sizeof(signed char), map->nvox, pFile);

    if(ferror(pFile))
        svt_exception::ui()->error("writeMRC: Cannot write to file.");

    fclose(pFile);
    free(pMRC_Header);

    return;
}

/**
 * Write MRC volume file
 * \param pFilename pointer to array of char with the filename
 */
template<>
inline void svt_volume<signed char>::writeMRC(const char* pFilename)
{
    MRCHEADER *pMRC_Header;
    FILE *pFile;
    //int i;
    //float fNumber;
    int iStamp;
    char *pStampBytes;
    unsigned int iNVox = m_iSizeX * m_iSizeY * m_iSizeZ;

    svtout << "Writing map to MRC/CCP4 file " << pFilename << endl;

    // Allocate header
    pMRC_Header = new MRCHEADER;
    memset(pMRC_Header,0,sizeof(pMRC_Header));

    // Prepare machine stamp (only endianess)
    pStampBytes = (char*)(&iStamp);
    if(svt_bigEndianMachine())
    {
        pStampBytes[0]=0x11;
        pStampBytes[1]=0x11;
        pStampBytes[2]=0;
        pStampBytes[3]=0;

    } else {

        pStampBytes[0]=0x44;
        pStampBytes[1]=0x44;
        pStampBytes[2]=0;
        pStampBytes[3]=0;
    }
    pMRC_Header->machinestamp = iStamp;

    // Data type
    //pMRC_Header->mode = MRC_MODE_float;
    //pMRC_Header->mode = MRC_MODE_long;
    //pMRC_Header->mode = MRC_MODE_short;
    pMRC_Header->mode = MRC_MODE_char;

    // Descriptive information about map
    pMRC_Header->nx      = (int) m_iSizeX;
    pMRC_Header->ny      = (int) m_iSizeY;
    pMRC_Header->nz      = (int) m_iSizeZ;
    pMRC_Header->mx      = (int) m_iSizeX;
    pMRC_Header->my      = (int) m_iSizeY;
    pMRC_Header->mz      = (int) m_iSizeZ;
    pMRC_Header->xlen    = (float) m_iSizeX * m_fWidth;
    pMRC_Header->ylen    = (float) m_iSizeY * m_fWidth;
    pMRC_Header->zlen    = (float) m_iSizeZ * m_fWidth;
    pMRC_Header->xorigin = (float) m_fGridX;
    pMRC_Header->yorigin = (float) m_fGridY;
    pMRC_Header->zorigin = (float) m_fGridZ;
    pMRC_Header->alpha   = (float) 90.0f;
    pMRC_Header->beta    = (float) 90.0f;
    pMRC_Header->gamma   = (float) 90.0f;
    pMRC_Header->amin  = (float) this->getMinDensity();
    pMRC_Header->amax  = (float) this->getMaxDensity();
    pMRC_Header->amean = (float) this->getAvgDensity();

    // Label with date
    pMRC_Header->nlabl = 1;
    sprintf(&pMRC_Header->labels[0][0], "Created by SVT/Sculptor - %02i.%02i.%i", svt_getDay(), svt_getMonth(), svt_getYear());
    pMRC_Header->labels[0][79] = 0;
    pMRC_Header->labels[1][0] = 0;

    // Fixed entries
    pMRC_Header->mapc    = 1;
    pMRC_Header->mapr    = 2;
    pMRC_Header->maps    = 3;

    // if origin / width is an integer, write it into the nxstart fields...
    int iOriginX = (int)(m_fGridX / m_fWidth);
    int iOriginY = (int)(m_fGridY / m_fWidth);
    int iOriginZ = (int)(m_fGridZ / m_fWidth);
    float fOriginX = (float)(m_fGridX / m_fWidth);
    float fOriginY = (float)(m_fGridY / m_fWidth);
    float fOriginZ = (float)(m_fGridZ / m_fWidth);

    if (
        (float)(iOriginX) - fOriginX < EPSILON &&
        (float)(iOriginY) - fOriginY < EPSILON &&
        (float)(iOriginZ) - fOriginZ < EPSILON
       )
    {
        pMRC_Header->nxstart = iOriginX;
        pMRC_Header->nystart = iOriginY;
        pMRC_Header->nzstart = iOriginZ;
    }
    else
    {
        pMRC_Header->nxstart = 0;
        pMRC_Header->nystart = 0;
        pMRC_Header->nzstart = 0;
    }

    pMRC_Header->ispg    = 0;
    pMRC_Header->nsymbt  = 0;
    pMRC_Header->map[0]  = 'M';
    pMRC_Header->map[1]  = 'A';
    pMRC_Header->map[2]  = 'P';
    pMRC_Header->map[3]  = ' ';

    // Write header and data to file
    pFile = fopen(pFilename, "wb");
    fwrite((void *) pMRC_Header,sizeof(MRCHEADER),1,pFile);

    // Write data
    //for(i=0; i<iNVox; i++)
    //{
    //    fNumber = (float) at(i);
    //    fwrite(&fNumber, sizeof(float), 1, pFile);
    //}
    //fwrite(map->ldata, sizeof(long), iNVox, pFile);
    //fwrite(map->sdata, sizeof(short), iNVox, pFile);
    fwrite(m_pData, sizeof(signed char), iNVox, pFile);

    if(ferror(pFile))
        svt_exception::ui()->error("writeMRC: Cannot write to file.");

    fclose(pFile);
    free(pMRC_Header);

    return;
}

/**
 * Write MRC volume file
 * \param pFilename pointer to array of char with the filename
 */
template<>
inline void svt_volume<long>::writeMRC(const char* pFilename)
{
    MRCHEADER *pMRC_Header;
    FILE *pFile;
    //int i;
    //float fNumber;
    int iStamp;
    char *pStampBytes;
    unsigned int iNVox = m_iSizeX * m_iSizeY * m_iSizeZ;

    svtout << "Writing map to MRC/CCP4 file " << pFilename << endl;

    // Allocate header
    pMRC_Header = new MRCHEADER;
    memset(pMRC_Header,0,sizeof(pMRC_Header));

    // Prepare machine stamp (only endianess)
    pStampBytes = (char*)(&iStamp);
    if(svt_bigEndianMachine())
    {
        pStampBytes[0]=0x11;
        pStampBytes[1]=0x11;
        pStampBytes[2]=0;
        pStampBytes[3]=0;

    } else {

        pStampBytes[0]=0x44;
        pStampBytes[1]=0x44;
        pStampBytes[2]=0;
        pStampBytes[3]=0;
    }
    pMRC_Header->machinestamp = iStamp;

    // Data type
    //pMRC_Header->mode = MRC_MODE_float;
    pMRC_Header->mode = MRC_MODE_long;
    //pMRC_Header->mode = MRC_MODE_short;
    //pMRC_Header->mode = MRC_MODE_char;

    // Descriptive information about map
    pMRC_Header->nx      = (int) m_iSizeX;
    pMRC_Header->ny      = (int) m_iSizeY;
    pMRC_Header->nz      = (int) m_iSizeZ;
    pMRC_Header->mx      = (int) m_iSizeX;
    pMRC_Header->my      = (int) m_iSizeY;
    pMRC_Header->mz      = (int) m_iSizeZ;
    pMRC_Header->xlen    = (float) m_iSizeX * m_fWidth;
    pMRC_Header->ylen    = (float) m_iSizeY * m_fWidth;
    pMRC_Header->zlen    = (float) m_iSizeZ * m_fWidth;
    pMRC_Header->xorigin = (float) m_fGridX;
    pMRC_Header->yorigin = (float) m_fGridY;
    pMRC_Header->zorigin = (float) m_fGridZ;
    pMRC_Header->alpha   = (float) 90.0f;
    pMRC_Header->beta    = (float) 90.0f;
    pMRC_Header->gamma   = (float) 90.0f;
    pMRC_Header->amin  = (float) this->getMinDensity();
    pMRC_Header->amax  = (float) this->getMaxDensity();
    pMRC_Header->amean = (float) this->getAvgDensity();

    // Label with date
    pMRC_Header->nlabl = 1;
    sprintf(&pMRC_Header->labels[0][0], "Created by SVT/Sculptor - %02i.%02i.%i", svt_getDay(), svt_getMonth(), svt_getYear());
    pMRC_Header->labels[0][79] = 0;
    pMRC_Header->labels[1][0] = 0;

    // Fixed entries
    pMRC_Header->mapc    = 1;
    pMRC_Header->mapr    = 2;
    pMRC_Header->maps    = 3;

    // if origin / width is an integer, write it into the nxstart fields...
    int iOriginX = (int)(m_fGridX / m_fWidth);
    int iOriginY = (int)(m_fGridY / m_fWidth);
    int iOriginZ = (int)(m_fGridZ / m_fWidth);
    float fOriginX = (float)(m_fGridX / m_fWidth);
    float fOriginY = (float)(m_fGridY / m_fWidth);
    float fOriginZ = (float)(m_fGridZ / m_fWidth);

    if (
        (float)(iOriginX) - fOriginX < EPSILON &&
        (float)(iOriginY) - fOriginY < EPSILON &&
        (float)(iOriginZ) - fOriginZ < EPSILON
       )
    {
        pMRC_Header->nxstart = iOriginX;
        pMRC_Header->nystart = iOriginY;
        pMRC_Header->nzstart = iOriginZ;
    }
    else
    {
        pMRC_Header->nxstart = 0;
        pMRC_Header->nystart = 0;
        pMRC_Header->nzstart = 0;
    }

    pMRC_Header->ispg    = 0;
    pMRC_Header->nsymbt  = 0;
    pMRC_Header->map[0]  = 'M';
    pMRC_Header->map[1]  = 'A';
    pMRC_Header->map[2]  = 'P';
    pMRC_Header->map[3]  = ' ';

    // Write header and data to file
    pFile = fopen(pFilename, "wb");
    fwrite((void *) pMRC_Header,sizeof(MRCHEADER),1,pFile);

    // Write data
    //for(i=0; i<iNVox; i++)
    //{
    //    fNumber = (float) at(i);
    //    fwrite(&fNumber, sizeof(float), 1, pFile);
    //}
    fwrite(m_pData, sizeof(long), iNVox, pFile);
    //fwrite(map->sdata, sizeof(short), iNVox, pFile);
    //fwrite(m_pData, sizeof(signed char), iNVox, pFile);

    if(ferror(pFile))
        svt_exception::ui()->error("writeMRC: Cannot write to file.");

    fclose(pFile);
    free(pMRC_Header);

    return;
}

/**
 * Write MRC volume file
 * \param pFilename pointer to array of char with the filename
 */
template<>
inline void svt_volume<short>::writeMRC(const char* pFilename)
{
    MRCHEADER *pMRC_Header;
    FILE *pFile;
    //int i;
    //float fNumber;
    int iStamp;
    char *pStampBytes;
    unsigned int iNVox = m_iSizeX * m_iSizeY * m_iSizeZ;

    svtout << "Writing map to MRC/CCP4 file " << pFilename << endl;

    // Allocate header
    pMRC_Header = new MRCHEADER;
    memset(pMRC_Header,0,sizeof(pMRC_Header));

    // Prepare machine stamp (only endianess)
    pStampBytes = (char*)(&iStamp);
    if(svt_bigEndianMachine())
    {
        pStampBytes[0]=0x11;
        pStampBytes[1]=0x11;
        pStampBytes[2]=0;
        pStampBytes[3]=0;

    } else {

        pStampBytes[0]=0x44;
        pStampBytes[1]=0x44;
        pStampBytes[2]=0;
        pStampBytes[3]=0;
    }
    pMRC_Header->machinestamp = iStamp;

    // Data type
    //pMRC_Header->mode = MRC_MODE_float;
    //pMRC_Header->mode = MRC_MODE_long;
    pMRC_Header->mode = MRC_MODE_short;
    //pMRC_Header->mode = MRC_MODE_char;

    // Descriptive information about map
    pMRC_Header->nx      = (int) m_iSizeX;
    pMRC_Header->ny      = (int) m_iSizeY;
    pMRC_Header->nz      = (int) m_iSizeZ;
    pMRC_Header->mx      = (int) m_iSizeX;
    pMRC_Header->my      = (int) m_iSizeY;
    pMRC_Header->mz      = (int) m_iSizeZ;
    pMRC_Header->xlen    = (float) m_iSizeX * m_fWidth;
    pMRC_Header->ylen    = (float) m_iSizeY * m_fWidth;
    pMRC_Header->zlen    = (float) m_iSizeZ * m_fWidth;
    pMRC_Header->xorigin = (float) m_fGridX;
    pMRC_Header->yorigin = (float) m_fGridY;
    pMRC_Header->zorigin = (float) m_fGridZ;
    pMRC_Header->alpha   = (float) 90.0f;
    pMRC_Header->beta    = (float) 90.0f;
    pMRC_Header->gamma   = (float) 90.0f;
    pMRC_Header->amin  = (float) this->getMinDensity();
    pMRC_Header->amax  = (float) this->getMaxDensity();
    pMRC_Header->amean = (float) this->getAvgDensity();

    // Label with date
    pMRC_Header->nlabl = 1;
    sprintf(&pMRC_Header->labels[0][0], "Created by SVT/Sculptor - %02i.%02i.%i", svt_getDay(), svt_getMonth(), svt_getYear());
    pMRC_Header->labels[0][79] = 0;
    pMRC_Header->labels[1][0] = 0;

    // Fixed entries
    pMRC_Header->mapc    = 1;
    pMRC_Header->mapr    = 2;
    pMRC_Header->maps    = 3;

    // if origin / width is an integer, write it into the nxstart fields...
    int iOriginX = (int)(m_fGridX / m_fWidth);
    int iOriginY = (int)(m_fGridY / m_fWidth);
    int iOriginZ = (int)(m_fGridZ / m_fWidth);
    float fOriginX = (float)(m_fGridX / m_fWidth);
    float fOriginY = (float)(m_fGridY / m_fWidth);
    float fOriginZ = (float)(m_fGridZ / m_fWidth);

    if (
        (float)(iOriginX) - fOriginX < EPSILON &&
        (float)(iOriginY) - fOriginY < EPSILON &&
        (float)(iOriginZ) - fOriginZ < EPSILON
       )
    {
        pMRC_Header->nxstart = iOriginX;
        pMRC_Header->nystart = iOriginY;
        pMRC_Header->nzstart = iOriginZ;
    }
    else
    {
        pMRC_Header->nxstart = 0;
        pMRC_Header->nystart = 0;
        pMRC_Header->nzstart = 0;
    }

    pMRC_Header->ispg    = 0;
    pMRC_Header->nsymbt  = 0;
    pMRC_Header->map[0]  = 'M';
    pMRC_Header->map[1]  = 'A';
    pMRC_Header->map[2]  = 'P';
    pMRC_Header->map[3]  = ' ';

    // Write header and data to file
    pFile = fopen(pFilename, "wb");
    fwrite((void *) pMRC_Header,sizeof(MRCHEADER),1,pFile);

    // Write data
    fwrite(m_pData, sizeof(short), iNVox, pFile);

    if(ferror(pFile))
        svt_exception::ui()->error("writeMRC: Cannot write to file.");

    fclose(pFile);
    free(pMRC_Header);

    return;
}

/**
 * Reads SPIDER maps from file
 * \param pFilename pointer to array of char with the filename
 */
template<class T>
void svt_volume<T>::loadSpider( const char *pFilename, float fVoxSize)
{
    FILE  *pFile;
    int   iHeaderLength;
    int   iNVox;
    int   iExtX, iExtY, iExtZ;
    int   i;
    bool  bSwap = false;

    svtout << "Loading SPIDER file: " << pFilename << endl;

    pFile = fopen(pFilename, "rb");
    if( pFile == NULL ) {
    	svtout << "   Error: Could not open file" << endl;
	return;
    }

    // Determine total size of file
    int   iNFloat = 0;
    fseek (pFile, 0, SEEK_END);
    iNFloat = ftell(pFile) / 4;
    rewind(pFile);

    // Read file header
    SPIDERHEADER *pSPIDERHeader;
    pSPIDERHeader = new SPIDERHEADER;

    if (!fread(pSPIDERHeader,sizeof(SPIDERHEADER),1,pFile))
    {
        svt_exception::ui()->error("Cannot read header of SPIDER file.");
	fclose(pFile);
        return;
    }

    // Sanity check the header. The byte order might be wrong.
    // The Spider header doesn't have a dedicate field to indicate the byte order.
    // That's why we can only check if some of the header fields make sense.
    if (  (((int) floor(100*(4*(pSPIDERHeader->nsam * ceil(256/(pSPIDERHeader->nsam*1.0)))
		             -pSPIDERHeader->labbyt)+0.5)) != 0)
	|| (pSPIDERHeader->nslice > 2000) || (pSPIDERHeader->nslice < 0)
 	|| (pSPIDERHeader->nrow > 2000)   || (pSPIDERHeader->nrow < 0)
 	|| (pSPIDERHeader->nsam > 2000)   || (pSPIDERHeader->nsam < 0)                     )
    {
        // Swap the byte order of the header
	Real32 *pTemp = (Real32 *) pSPIDERHeader;
	for (i=0; i < (int) (sizeof(SPIDERHEADER)/sizeof(Real32)); i++)
	    svt_swapReal32(pTemp+i);
	bSwap = true;
    }

    if (  (((int) floor(100*(4*(pSPIDERHeader->nsam * ceil(256/(pSPIDERHeader->nsam*1.0)))
		             -pSPIDERHeader->labbyt)+0.5)) != 0)
	|| (pSPIDERHeader->nslice > 2000) || (pSPIDERHeader->nslice < 0)
 	|| (pSPIDERHeader->nrow > 2000)   || (pSPIDERHeader->nrow < 0)
 	|| (pSPIDERHeader->nsam > 2000)   || (pSPIDERHeader->nsam < 0)                     )
    {
        svt_exception::ui()->info("Unable to read SPIDER file header");
	fclose(pFile);
	return;
    }

    // Determine size of volume
    iExtX = (int) pSPIDERHeader->nsam;
    iExtY = (int) pSPIDERHeader->nrow;
    iExtZ = (int) pSPIDERHeader->nslice;
    iNVox = iExtX * iExtY * iExtZ;


    // The actual length of the header depends on the X dimension (!? Must come from FORTRAN...)
    iHeaderLength = iExtX * (int) ceil(256.0f/(iExtX*1.0f));

    // Additional sanity checks
    if (fabs(pSPIDERHeader->iform - 3.0f) > EPSILON)
    {
        svt_exception::ui()->info("SPIDER file is not a 3D map");
	fclose(pFile);
	return;
    }

    if (((int) floor(100*(sizeof(float)*iHeaderLength-pSPIDERHeader->labbyt)+0.5)) != 0) {
        svt_exception::ui()->info("SPIDER file header length is incompatible with map size");
	fclose(pFile);
	return;
    }

    if (iNFloat>iNVox+iHeaderLength) {
        svt_exception::ui()->info("SPIDER file is too long or data is unreadable");
	fclose(pFile);
	return;
    }

    if (iNFloat<iNVox+iHeaderLength) {
        svt_exception::ui()->info("SPIDER file is too short or data is unreadable");
	fclose(pFile);
	return;
    }

    // Place read index after header
    fseek (pFile, iHeaderLength*sizeof(float), SEEK_SET);

    // Allocate new map
    try
    {
        allocate( iExtX, iExtY, iExtZ, (T)(0), true);

    } catch(bad_alloc& ex)
    {
        svt_exception::ui()->info("Unable to satisfy memory allocation request");
	fclose(pFile);
	return;
    }

    // Now read the data

    float fnumber;
    int   iCount = 0;
    svt_exception::ui()->progressPopup("Loading SPIDER File...", 0, iNVox);
    try
    {
	for(i=0; i<iNVox; i++)
	{
	    iCount += fread(&fnumber, sizeof(Real32), 1, pFile);
	    if (bSwap) svt_swapReal32( &fnumber );
	    setAt( i, (T)( fnumber ) );

	    if (iCount%10000 == 0)
		svt_exception::ui()->progress( iCount, iNVox);
	}

    } catch (svt_userInterrupt&)
    {
	svt_exception::ui()->error("User interrupt, loading aborted.");
	m_iSizeX = m_iSizeY = m_iSizeZ = 0;
	svt_exception::ui()->progressPopdown();
	allocate( 0,0,0, (T)(0), false);
	fclose(pFile);
	return;
    }
    svt_exception::ui()->progressPopdown();

    if (iCount != iNVox)
    {
	svtout << "WARNING: Read " << iCount << " floats, expecting " << iNVox << endl;
    }

    fclose(pFile);

    // Set parameters for map
    m_fGridX = 0.0;
    m_fGridY = 0.0;
    m_fGridZ = 0.0;

    if (fabs(fVoxSize) < EPSILON)
    {
	//string oAns = svt_exception::ui()->textInput("Please enter the voxelsize:", "1.0" );
	//m_fWidth = atof( oAns.c_str() );
	m_fWidth = 1.0f;
    }
    else
	m_fWidth = fVoxSize;

    m_iSizeX = iExtX;
    m_iSizeY = iExtY;
    m_iSizeZ = iExtZ;

    // Output some map info
    svtout << "   Map dimensions: " << m_iSizeX << " x " << m_iSizeY << " x " << m_iSizeZ << endl;
    svtout << "   Map origin: " << m_fGridX << ", " << m_fGridY << ", " << m_fGridZ << endl;
    svtout << "   Voxel size in Angstrom: " << m_fWidth << endl;

    return;
}

/**
 * Write SPIDER volume file
 * \param pFilename pointer to array of char with the filename
 */
template<class T>
inline void svt_volume<T>::writeSpider(const char* pFilename)
{
    SPIDERHEADER *pSPIDERHeader;
    FILE  *pFile;
    int   i;
    float fNumber;
    int   iNVox = m_iSizeX * m_iSizeY * m_iSizeZ;

    // Allocate header
    pSPIDERHeader = new SPIDERHEADER;
    memset(pSPIDERHeader,0,sizeof(pSPIDERHeader));

    // Descriptive information about map
    pSPIDERHeader->nsam    = (float) m_iSizeX;
    pSPIDERHeader->nrow    = (float) m_iSizeY;
    pSPIDERHeader->nslice  = (float) m_iSizeZ;
    pSPIDERHeader->fmin    = (float) this->getMinDensity();
    pSPIDERHeader->fmax    = (float) this->getMaxDensity();
    pSPIDERHeader->av      = (float) this->getAvgDensity();

    // Fixed entries
    pSPIDERHeader->iform   = 3.0f;
    pSPIDERHeader->imami   = 1.0f;
    pSPIDERHeader->inuse   = -1.0f;

    // Descriptive entries
    pSPIDERHeader->labrec  = ceil(256.0f/(m_iSizeX*1.0f));
    pSPIDERHeader->lenbyt  = pSPIDERHeader->nsam * 4.0f;
    pSPIDERHeader->labbyt  = pSPIDERHeader->labrec * pSPIDERHeader->lenbyt;
    pSPIDERHeader->irec    = pSPIDERHeader->nslice*pSPIDERHeader->nrow + pSPIDERHeader->labrec;

    // Write header to file
    pFile = fopen(pFilename, "wb");
    if (! fwrite((void *) pSPIDERHeader,sizeof(SPIDERHEADER),1,pFile) )
    {
	svt_exception::ui()->error("Cannot write header of SPIDER file.");
	fclose(pFile);
	delete pSPIDERHeader;
	return;
    }

    // Write the spacer between header and data

    float fDummy = 0.0f;
    for(i=0; i<(pSPIDERHeader->nsam*pSPIDERHeader->labrec - sizeof(SPIDERHEADER)/4); i++)
    {
	if (! fwrite(&fDummy,sizeof(fDummy),1,pFile) )
	{
	    svt_exception::ui()->error("Cannot write spacer between header and data of SPIDER file");
	    fclose(pFile);
	    delete pSPIDERHeader;
	    return;
	}
    }

    // Write data
    svt_exception::ui()->progressPopup("Writing map to SPIDER File...", 0, iNVox);
    try
    {
        for(i=0; i<iNVox; i++)
        {
            fNumber = (float) at(i);
            fwrite(&fNumber, sizeof(float), 1, pFile);

            if (i%10000 == 0)
                svt_exception::ui()->progress( i, iNVox);
        }

    } catch (svt_userInterrupt&)
    {
        svt_exception::ui()->error("User interrupt, saving aborted, file incomplete!");
    }
    svt_exception::ui()->progressPopdown();

    if(ferror(pFile))
        svt_exception::ui()->error("writeSPIDER: Cannot write to file.");

    fclose(pFile);
    delete pSPIDERHeader;

    return;
}

#endif
