/***************************************************************************
                          svt_channel.cpp  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <live.h>
#include <svt_channel.h>
#include <svt_init.h>

/**
 * Constructor
 * You should not create a svt_channel object by yourself, you should get the channel object from the svt_device object.
 * \param pChannel pointer to a live_channel (see live_channel struct in live documentation)
 */
svt_channel::svt_channel(live_channel* pChannel)
{
    m_pChannel = pChannel;
    m_pMatrix = new svt_matrix4f("svt_channel matrix");
};
svt_channel::~svt_channel()
{
    delete m_pMatrix;
};

/**
 * get the integer data (if the channel is a integer input channel)
 * \return a Int16 value
 */
Int16 svt_channel::getInt()
{
    if (m_pChannel->type == (UInt8)LIVE_CHN_INT)
        return m_pChannel->data.ival;
    else
        return -1;
};

/**
 * get the float data (if the channel is a float input channel)
 * \return a Real32 value
 */
Real32 svt_channel::getFloat()
{
    if (m_pChannel->type == (UInt8)LIVE_CHN_FLOAT)
        return m_pChannel->data.fval;
    else
        return -1.0f;
};

/**
 * get the char data (if the channel is a char input channel)
 * \return a char
 */
char svt_channel::getChar()
{
    if (m_pChannel->type == (UInt8)LIVE_CHN_CHAR)
        return m_pChannel->data.cval;
    else
        return -1;
};

/**
 * get the matrix data (if the channel is a matrix input channel)
 * \return a pointer to a svt_matrix
 */
svt_matrix4f* svt_channel::getMatrix()
{
    if (m_pChannel->type == LIVE_CHN_MATRIX)
        m_pMatrix->setFromMatrix(*(Matrix4f *)(m_pChannel->data.mval));
    return m_pMatrix;
};

/**
 * write an integer value to the device (the data will get written the next time you call the update function of your svt_device object)
 * \param i an integer value
 */
void svt_channel::setInt(Int16 i)
{
    if (m_pChannel->type == (UInt8)LIVE_CHN_CHAR)
        m_pChannel->data.ival = i;
};

/**
 * write a float value to the device (the data will get written the next time you call the update function of your svt_device object)
 * \param f a Real32 value
 */
void svt_channel::setFloat(Real32 f)
{
    if (m_pChannel->type == (UInt8)LIVE_CHN_FLOAT)
        m_pChannel->data.fval = f;
};

/**
 * write a char value to the device (the data will get written the next time you call the update function of your svt_device object)
 * \param c a char
 */
void svt_channel::setChar(char f)
{
    if (m_pChannel->type == (UInt8)LIVE_CHN_CHAR)
        m_pChannel->data.cval = (UInt8) f;
};

/**
 * write a matrix to the device (the data will get written the next time you call the update function of your svt_device object)
 * \param pMatrix pointer to a svt_matrix4f object
 */
void svt_channel::setMatrix(svt_matrix4f*)
{
};
