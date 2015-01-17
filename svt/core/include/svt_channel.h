/***************************************************************************
                          svt_channel.h  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_CHANNEL_H
#define SVT_CHANNEL_H

#include <live.h>
#include <svt_matrix4f.h>

/**
 * This class encapsulates a live device channel. See the l.i.v.e. documentation.
 *@author Stefan Birmanns
 */
class DLLEXPORT svt_channel
{
protected:
    live_channel* m_pChannel;
    svt_matrix4f* m_pMatrix;

public:
    /**
     * Constructor
     * You should not create a svt_channel object by yourself, you should get the channel object from the svt_device object.
     * \param pChannel pointer to a live_channel (see live_channel struct in live documentation)
     */
    svt_channel(live_channel* pChannel);
    virtual ~svt_channel();

public:
    /**
     * get the integer data (if the channel is a integer input channel)
     * \return a Int16 value
     */
    Int16 getInt();
    /**
     * get the float data (if the channel is a float input channel)
     * \return a Real32 value
     */
    Real32 getFloat();
    /**
     * get the char data (if the channel is a char input channel)
     * \return a char
     */
    char getChar();
    /**
     * get the matrix data (if the channel is a matrix input channel)
     * \return a pointer to a svt_matrix
     */
    svt_matrix4f* getMatrix();

    /**
     * write an integer value to the device (the data will get written the next time you call the update function of your svt_device object)
     * \param i an integer value
     */
    void setInt(Int16 i);
    /**
     * write a float value to the device (the data will get written the next time you call the update function of your svt_device object)
     * \param f a Real32 value
     */
    void setFloat(Real32 f);
    /**
     * write a char value to the device (the data will get written the next time you call the update function of your svt_device object)
     * \param c a char
     */
    void setChar(char c);
    /**
     * write a matrix to the device (the data will get written the next time you call the update function of your svt_device object)
     * \param pMatrix pointer to a svt_matrix4f object
     */
    void setMatrix(svt_matrix4f* pMatrix);
};

#endif
