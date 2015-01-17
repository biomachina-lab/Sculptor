/***************************************************************************
                          svt_member2
                          -------------------
    begin                : 10/19/2006
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_MEMBER2_H
#define __SVT_MEMBER2_H

#ifdef WIN32
#include <string.h>
#define strcasecmp _stricmp
#endif

///////////////////////////////////////////////////////////////////////////////
// member2
///////////////////////////////////////////////////////////////////////////////

/**
 * This is a class with two members: first numerical which will be used mainly as a sort criteria
 * and a second one who stores information 
 * \author Mirabela Rusu
 */
template<class T, class E> class DLLEXPORT svt_member2
{
    protected:
    // numerical member
    T m_tNumber;  
    //member storing the  information 
    E m_tInfo;
    
    public:
	/**
	 * Constructor
	 * \param tNumber the numerical part of the class
	 * \param tInfo the information that needs to be stored
	 */
	svt_member2(T tNumber, E tInfo);
	
	/**
	 * Overloading of the operator < 
	 */ 
	bool operator<(svt_member2<T,E> oOther) const;
	 
	/**
	 * Overloading operator =, assignation
	 */
	void operator=(svt_member2<T,E> oOther);
	
	/**
	 * Overloading operator ==, equality
	 */
	void operator==(svt_member2<T,E> oOther) const;
	
	/**
	 * get the numerical member
	 */
	T getNumber();
	
	/**
	 * get the numerical member
	 */
	E getInfo();
	
	/**
	 * set the numerical member
	 * \param tNumber the numerical value to be assigned 
	 */
	void setNumber(T tNumber);
	
	/**
	 * set the numerical member
	 * \param  tInfo the information to be set
	 */
	void setInfo(E tInfo);

	
};


///////////////////////////////////////////////////////////////////////////////
/////            Implementation
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 * \param tNumber the numerical part of the class
 * \param tInfo the information that needs to be stored
 */
template<class T, class E>
svt_member2<T,E>::svt_member2(T tNumber, E tInfo)
{
    m_tNumber = tNumber;
    m_tInfo = tInfo;
} 

/**
 * Overloadinf operator =, assignation
 */
template<class T, class E>
void svt_member2<T,E>::operator=(svt_member2<T,E> oOther)
{
    m_tNumber = oOther.getNumber();
    m_tInfo = oOther.getInfo();
}


/**
 * Overloading operator ==, equality
 */
template<class T, class E>
void svt_member2<T,E>::operator==(svt_member2<T,E> oOther) const
{
    return (m_tNumber==oOther.getNumber() && m_tInfo==m_tInfo);
}

/**
 * Overloading of the operator < 
 */ 
template<class T, class E>
bool svt_member2<T,E>::operator<(svt_member2<T,E> oOther) const 
{
    return (m_tNumber < oOther.getNumber());
}

/**
 * get the numerical member
 */
template<class T, class E>
T svt_member2<T,E>::getNumber(){return m_tNumber;}

/**
 * get the numerical member
 */
template<class T, class E>
E svt_member2<T,E>::getInfo(){return m_tInfo;}

/**
 * set the numerical member
 * \param tNumber the numerical value to be assigned 
 */
 template<class T, class E>
void svt_member2<T,E>::setNumber(T tNumber)
{
    m_tNumber = tNumber;
}

/**
 * set the numerical member
 * \param  tInfo the information to be set
 */
template<class T, class E>
void svt_member2<T,E>::setInfo(E tInfo)
{
    m_tInfo = tInfo;
}

#endif
