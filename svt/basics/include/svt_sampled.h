/***************************************************************************
                          svt_sampled
                          -----------
    begin                : 02/13/2006
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_SAMPLED_H
#define __SVT_SAMPLED_H

/**
 * Pure abstract base class of an object that can be sampled
 * \author Stefan Birmanns
 */
template<class T> class svt_sampled
{
public:

    /**
     */
    virtual ~svt_sampled()
    {
    };

    /**
     * sample the object randomly and return a vector that reflects the probability distribution of the object
     */
    virtual T sample()=0;
};


#endif
