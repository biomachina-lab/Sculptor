/***************************************************************************
                          svt_binary_tree
                          ---------------
    begin                : 5/21/2004
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_BINARYTREE
#define SVT_BINARYTREE

#include <svt_basics.h>

/**
 * Binary tree. Each tree element can either have two or zero children.
 * \author Stefan Birmanns
 */
class svt_binary_tree
{
    // are the children valid or not?
    bool m_bValid;
    // children
    svt_binary_tree* m_pChildA;
    svt_binary_tree* m_pChildB;
    // pointer to father node
    svt_binary_tree* m_pFather;

public:

    /**
     * Constructor
     */
    svt_binary_tree();

    /**
     * add child
     * \param oChildA first child
     * \param oChildB second child
     */
    void setChildren(svt_binary_tree* pChildA, svt_binary_tree* pChildB);

    /**
     * set first child
     * \param pChildA pointer to new child (svt_binary_tree)
     */
    void setChildA(svt_binary_tree* pChildA);
    /**
     * set second child
     * \param pChildB pointer to new child (svt_binary_tree)
     */
    void setChildB(svt_binary_tree* pChildB);

    /**
     * get first child
     * \return pointer to svt_binary_tree object
     */
    inline svt_binary_tree* getChildA( ) const
    {
	return m_pChildA;
    };
    /**
     * get second child
     * \return pointer to svt_binary_tree object
     */
    inline svt_binary_tree* getChildB( ) const
    {
	return m_pChildB;
    };

    /**
     * get the father node
     * \return pointer to svt_binary_tree
     */
    inline svt_binary_tree* getFather() const
    {
	return m_pFather;
    }

private:

    /**
     * set the father node of this node
     * \param pFather pointer to svt_binary_tree
     */
    void setFather(svt_binary_tree* pFather)
    {
	m_pFather = pFather;
    }
};

#endif
