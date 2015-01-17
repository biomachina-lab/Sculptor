/***************************************************************************
                          svt_property.h  -  description
                             -------------------
    begin                : Tue Mar 30 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>

#ifndef SVT_PROPERTY_H
#define SVT_PROPERTY_H

/** Represents a property of a svt object
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_property  
{
public:
    svt_property();
    virtual ~svt_property();

public:
    /** apply the property to a svt object */
    virtual void applyProperty(void);
};

#endif
