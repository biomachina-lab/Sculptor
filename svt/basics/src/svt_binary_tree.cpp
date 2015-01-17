/***************************************************************************
                          svt_binary_tree.cpp
                          -------------------
    begin                : 5/21/2004
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_binary_tree.h>

/**
 * Constructor
 */
svt_binary_tree::svt_binary_tree()
{
    m_pChildA = NULL;
    m_pChildB = NULL;
    m_pFather = NULL;
    m_bValid = false;
};

/**
 * add child
 * \param oChildA first child
 * \param oChildB second child
 */
void svt_binary_tree::setChildren(svt_binary_tree* pChildA, svt_binary_tree* pChildB)
{
    m_pChildA = pChildA;
    m_pChildB = pChildB;

    m_pChildA->setFather(this);
    m_pChildB->setFather(this);

    if (pChildA != NULL && pChildB != NULL)
	m_bValid = true;
    else
	m_bValid = false;
};

/**
 * set first child
 * \param pChildA pointer to new child (svt_binary_tree)
 */
void svt_binary_tree::setChildA(svt_binary_tree* pChildA)
{
    m_pChildA = pChildA;

    if (pChildA == NULL)
	m_bValid = false;
    else
	m_pChildA->setFather(this);
};
/**
 * set second child
 * \param pChildB pointer to new child (svt_binary_tree)
 */
void svt_binary_tree::setChildB(svt_binary_tree* pChildB)
{
    m_pChildB = pChildB;

    if (pChildB == NULL)
	m_bValid = false;
    else
	m_pChildB->setFather(this);
};
