/***************************************************************************
                          svt_array.h  -  description
                             -------------------
    begin                : 10.04.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_ARRAY_H
#define SVT_ARRAY_H

#include <stdlib.h>
#include <svt_basics.h>
#include <svt_stlVector.h>
#include <svt_iostream.h>

/** A resizeable array class. Now this class is based on the stl-vector.
  *@author Stefan Birmanns
  */
template<class dt> class  svt_array {

    protected:
        vector<dt> m_cData;

    public:
        /// create new array with n elements
        svt_array(int =0, int =1)
        {
        };

        ~svt_array() {
        };

    public:
        /// get the ith element of the array
        dt& getAt(int i)
        {
            return m_cData[i];
        }

        /// set the ith element of the array
        void setAt(int i, dt el)
        {
            m_cData[i] = el;
        };

        /// add an element to the array (the array will grow)
        int addElement(dt el)
        {
            m_cData.push_back(el);

            return m_cData.size()-1;
        };

        /// add i elements to the array (all elements will be initialized to el)
        void addElements(int n, dt el)
        {
            for (int i=0;i<n;i++)
                addElement(el);
        };

        /// insert an element into the array (just before element number i)
        void insElement(dt el, int i)
        {
            m_cData.insert(m_cData.begin()+i, el);
        };

        /// delete the ith element
        void delElement(int i =-1)
        {
            if (i != -1)
                m_cData.erase(m_cData.begin()+i);
            else
                m_cData.pop_back();
        };

        /// delete n elements at position i
        void delElements(int n, int i)
        {
            for(int k=0;k<n;k++)
                delElement(i);
        };

        /// delete all elements
        void delAllElements()
	{
            m_cData.clear();
            m_cData.erase(m_cData.begin(), m_cData.end());
        };

        /// find an element in the array and return the index. If the element exists more than one time, the function will only return the first one!
        int find(dt el)
        {
            unsigned int i;
            for(i=0;i<m_cData.size();i++)
                if (m_cData[i] == el)
                    return i;

            return -1;
        };

        /// return the number of elements
        int numElements()
        {
            return m_cData.size();
        };

        /// reserve space for elements if you know that you will need to add elements in the future. With this function you can avoid memory reallocations.
        void reserve(int iNum)
        {
            m_cData.reserve(iNum);
        }

        /// print array to stdout
        void print()
        {
            cout << "svt_array: containing " << m_cData.size() << " elements" << endl;
            unsigned int i;
            for(i=0;i<m_cData.size();i++)
                cout << "i:" << i << "=" << m_cData[i] << endl;
        };

        /// return a pointer to the data
        dt* getData()
        {
            return &m_cData[0];
        };

        /// operator +=
        svt_array<dt>* operator+=(dt a)
        {
            addElement(a);
            return this;
        };

        /// operator []
        dt& operator[](int i)
        {
            return m_cData[i];
        };

        /// operator <<
        inline void operator<<(dt a)
        {
            m_cData.push_back(a);
        };
};

#endif
