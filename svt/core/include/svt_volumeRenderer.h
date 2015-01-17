/***************************************************************************
                          svt_volumeRenderer
			  ------------------
    begin                : 07/07/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_node_noautodl.h>
#include <svt_volume.h>

#ifndef SVT_VOLUME_RENDERER
#define SVT_VOLUME_RENDERER

/**
 * Abstract volume-rendering class - it does not implement a certain volumerenderer but forms a base class for svt_volRenSlices, etc...
 *@author Stefan Birmanns
 */
template<class T> class DLLEXPORT svt_volumeRenderer : public svt_node
{

protected:

    // the data volume object
    svt_volume<T>& m_rVolume;

    // the segmentation volume object
    svt_volume<unsigned int>* m_pSegVol;

    // currently selected segment
    unsigned int m_iSegSel;

public:

    /**
     * Constructor
     */
    svt_volumeRenderer( svt_volume<T>& rVolume ) : svt_node(),
        m_rVolume( rVolume ),
        m_pSegVol( NULL ),
        m_iSegSel( 0 )
    {
    };
    virtual ~svt_volumeRenderer(){};

    /**
     * get volume data
     */
    svt_volume<T>& getVolume();

    /**
     * set the segmentation volume data
     * \param pSegVol pointer to a volume of unsigned ints of the same size than the underlying data set
     */
    void setSegVol( svt_volume< unsigned int >* pSegVol );
    /**
     * get the segmentation volume data
     * \return pointer to a volume of unsigned ints of the same size than the underlying data set
     */
    svt_volume< unsigned int >* getSegVol( );

    /**
     * set the currently selected segment
     * \param iSegSel number of segment to be rendered
     */
    void setSegSel( unsigned int iSegSel );
    /**
     * get the currently selected segment
     * \return number of segment which is selected
     */
    unsigned int getSegSel( );

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
};

/**
 * get volume data
 */
template<class T>
inline svt_volume<T>& svt_volumeRenderer<T>::getVolume()
{
    return m_rVolume;
};

/**
 * set the segmentation volume data
 * \param pSgVol pointer to a volume of unsigned ints of the same size than the underlying data set
 */
template<class T>
void svt_volumeRenderer<T>::setSegVol( svt_volume< unsigned int >* pSegVol )
{
    m_pSegVol = pSegVol;
};
/**
 * get the segmentation volume data
 * \return pointer to a volume of unsigned ints of the same size than the underlying data set
 */
template<class T>
svt_volume< unsigned int >* svt_volumeRenderer<T>::getSegVol( )
{
    return m_pSegVol;
};

/**
 * set the currently selected segment
 * \param iSegSel number of segment to be rendered
 */
template<class T>
void svt_volumeRenderer<T>::setSegSel( unsigned int iSegSel )
{
    m_iSegSel = iSegSel;
}
/**
 * get the currently selected segment
 * \return number of segment which is selected
 */
template<class T>
unsigned int svt_volumeRenderer<T>::getSegSel( )
{
    return m_iSegSel;
};

/**
 * get the x size of the volume
 * \return x size of the volume
 */
template<class T>
inline unsigned int svt_volumeRenderer<T>::getSizeX() const
{
    return m_rVolume.getSizeX();
};
/**
 * get the y size of the volume
 * \return y size of the volume
 */
template<class T>
inline unsigned int svt_volumeRenderer<T>::getSizeY() const
{
    return m_rVolume.getSizeY();
};
/**
 * get the z size of the volume
 * \return z size of the volume
 */
template<class T>
inline unsigned int svt_volumeRenderer<T>::getSizeZ() const
{
    return m_rVolume.getSizeZ();
};


#endif
