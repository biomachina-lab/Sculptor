/***************************************************************************
                          svt_stree.cpp
                          -------------
    begin                : 5/21/2004
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_stree.h>

///////////////////////////////////////////////////////////////////////////////
// static members
///////////////////////////////////////////////////////////////////////////////

// minimal cost
double svt_stree_node::m_fMinCost = 0.0;
// node with minimal cost
svt_stree_node* svt_stree_node::m_pMinCostNode = NULL;

// level (for printing tree structure)
int svt_stree_node::m_iLevel = 0;

///////////////////////////////////////////////////////////////////////////////
// svt_stree_node
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 * \param oW initial codebook vector
 * \param fe initial error
 */
svt_stree_node::svt_stree_node(svt_vector4<double> oW, double fe) : svt_binary_tree()
{
    m_oW = oW;
    m_fe = fe;
    m_iN = 0;
}

/**
 * search the node with the minimal cost in the tree
 * \return pointer to svt_stree_node object
 */
svt_stree_node* svt_stree_node::minCost()
{
    // initialize the minimal cost with a high value
    m_fMinCost = 10000000;
    m_pMinCostNode = this;

    calcMinCost();

    return m_pMinCostNode;
}

/**
 * find the winning leaf node
 * \param oVec training vector
 */
svt_stree_node* svt_stree_node::findWinner(svt_vector4<double> oVec)
{
    m_iLevel++;

    // end of tree not reached yet?
    if (getChildA() != NULL)
    {
	if (oVec.distanceSq( ((svt_stree_node*)getChildA())->getVector() ) < oVec.distanceSq( ((svt_stree_node*)getChildB())->getVector() ))
	    return ((svt_stree_node*)getChildA())->findWinner(oVec);
	else
	    return ((svt_stree_node*)getChildB())->findWinner(oVec);
    } else
	return this;
}

/**
 * get all nodes at a certain level. The level can be chosen by setLevel()! The first parameter is not used to choose the Level!! Internal function, use svt_stree::getNodes()!!
 * \param iLevel current level in recursion (start always with 0!)
 * \param oArray reference to svt_array object
 */
void svt_stree_node::getNodes(int iLevel, svt_array< svt_stree_node* > &oArray)
{
    if (m_iLevel == iLevel)
	oArray << this;

    // recursion
    if (getChildA() != NULL && getChildB() != NULL)
    {
	((svt_stree_node*)getChildA())->getNodes(iLevel+1, oArray);
	((svt_stree_node*)getChildB())->getNodes(iLevel+1, oArray);
    }
}

/**
 * get all nodes.
 * \param oArray reference to svt_array object
 */
void svt_stree_node::getAllNodes(svt_array< svt_stree_node* > &oArray)
{
    oArray << this;

    // recursion
    if (getChildA() != NULL && getChildB() != NULL)
    {
	((svt_stree_node*)getChildA())->getAllNodes(oArray);
	((svt_stree_node*)getChildB())->getAllNodes(oArray);
    }
}

/**
 * get all leaf nodes.
 * \param oArray reference to svt_array object
 */
void svt_stree_node::getLeafNodes( svt_array< svt_stree_node* > &oArray )
{
    // recursion
    if (getChildA() != NULL && getChildB() != NULL)
    {
	((svt_stree_node*)getChildA())->getLeafNodes( oArray );
	((svt_stree_node*)getChildB())->getLeafNodes( oArray );
    } else
        oArray << this;
}

/**
 * calculate the number of levels in the tree (get result with getLevel()). Internal function! Use svt_stree::getNumLevels() instead.
 * \param iLevel set this parameter always to 0!
 */
void svt_stree_node::calcNumLevels(int iLevel)
{
    if (m_iLevel < iLevel)
        m_iLevel = iLevel;

    // recursion
    if (getChildA() != NULL && getChildB() != NULL)
    {
	((svt_stree_node*)getChildA())->calcNumLevels(iLevel + 1);
	((svt_stree_node*)getChildB())->calcNumLevels(iLevel + 1);
    }
}

/**
 * calculate the number of leaves in the tree (get the result with getLevels() - I know it's misleading but this is
 * an internal function anyway. As user you should call svt_stree::getNumLeaves()!
 */
void svt_stree_node::calcNumLeaves()
{
    // recursion
    if (getChildA() != NULL && getChildB() != NULL)
    {
	((svt_stree_node*)getChildA())->calcNumLeaves();
	((svt_stree_node*)getChildB())->calcNumLeaves();
    } else
        m_iLevel++;
}

/**
 * internal, recursive helper function to calculate the minimal cost
 */
void svt_stree_node::calcMinCost()
{
    // is that a new minimal cost?
    if (getChildA() == NULL && m_fe < m_fMinCost)
    {
	m_fMinCost = m_fe;
	m_pMinCostNode = this;
    }

    // recursion
    if (getChildA() != NULL && getChildB() != NULL)
    {
	((svt_stree_node*)getChildA())->calcMinCost();
	((svt_stree_node*)getChildB())->calcMinCost();
    }
}

///////////////////////////////////////////////////////////////////////////////
// svt_stree
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 */
svt_stree::svt_stree()
{
    // variables

    m_bConv = false;
    m_bFR = false;
    m_iNumLeaves = 0;
    m_iNumLevels = 0;

    m_it = 1;
    m_iu = 1;
    m_fe_0 = 0.0;
    m_f_e_0 = 0.0;
    m_iTau = 0;
    m_fC = 0.0f;
    m_fC_tau = 0.0f;
    m_fC_tau_old = 0.0f;

    // parameters

    m_fE = 500;         // default value for E_0!   FIXME: Make adjustable!
    m_fBeta1 = 0.01;    // default value for Beta1! FIXME: Make adjustable!
    m_fBeta2 = 0.075;   // default value for Beta2! FIXME: Make adjustable!
    m_fBeta3 = 0.2;     // default value for Beta3! FIXME: Make adjustable!
    m_fgamma = 1.1;     // default value for gamma! FIXME: Make adjustable!
    m_fGamma = 0.35;    // default value for Gamma! FIXME: Make adjustable!
    m_fdelta = 0.0001;  // default value for delta! FIXME: Make adjustable!
    m_feta = 0.0001;    // default value for eta!   FIXME: Make adjustable!
    m_iU = 21;          // default value for U!     FIXME: Make adjustable!
    m_iT = 10;          // default value for T!     FIXME: Make adjustable!
};

/**
 * set maximum number of nodes
 * \param iMaxNodex maximum number of nodes
 */
void svt_stree::setMaxNodes(int iMaxNodes)
{
    m_iU = iMaxNodes;
    m_iT = m_iU / 20;
    if (m_iT < 100)
	m_iT = 100;
};

/**
 * get maximum number of nodes
 * \return maximum number of nodes
 */
int svt_stree::getMaxNodes()
{
    return m_iU;
}

/**
 * get all nodes at a certain level
 * \param iLevel index of the level
 * \return array of nodes
 */
svt_array< svt_stree_node* > svt_stree::getNodes(int iLevel)
{
    svt_array< svt_stree_node* > oArray;

    m_oTree.setLevel(iLevel);
    m_oTree.getNodes(0, oArray);

    return oArray;
};

/**
 * get all nodes
 * \return array of nodes
 */
svt_array< svt_stree_node* > svt_stree::getAllNodes()
{
    svt_array< svt_stree_node* > oArray;

    m_oTree.getAllNodes( oArray );

    return oArray;
};

/**
 * get all leaf nodes.
 * \return svt_array object with pointers to the leaf nodes
 */
svt_array< svt_stree_node* > svt_stree::getLeafNodes( )
{
    svt_array< svt_stree_node* > oArray;

    m_oTree.getLeafNodes( oArray );

    return oArray;
}

/**
 * train binary tree with the stree2 algorithm.
 * function will set the convergence flag if the tree is not changing anymore.
 * \param oVec training vector
 */
void svt_stree::trainStree2(svt_vector4<double> oVec)
{
    svt_stree_node* pWinA = NULL;
    svt_stree_node* pWinB = NULL;
    int iLevelA =0;
    int iLevelB =0;

    // search for the winning leaf
    if (m_oTree.getChildA() != NULL)
    {
	m_oTree.setLevel(0);
	pWinA = ((svt_stree_node*)m_oTree.getChildA())->findWinner(oVec);
	iLevelA = m_oTree.getLevel();
    }
    if (m_oTree.getChildB() != NULL)
    {
	m_oTree.setLevel(0);;
	pWinB = ((svt_stree_node*)m_oTree.getChildB())->findWinner(oVec);
	iLevelB = m_oTree.getLevel();
    }

    // ok, nothing was found, so do a normal training
    if (pWinA == NULL || pWinB == NULL)
    {
	trainStree1(oVec);
	return;
    }

    // which one to choose?

    // case 1: tree is still growing
    if (m_iu < m_iU-1)
    {
	// path length is equal
	if (iLevelA == iLevelB)
	{
	    if (oVec.distanceSq(pWinA->getVector()) < oVec.distanceSq(pWinB->getVector()))
		train(oVec, (svt_stree_node*)m_oTree.getChildA());
	    else
		train(oVec, (svt_stree_node*)m_oTree.getChildB());
	} else {
	    // no, one has a shorter path
	    if (iLevelA < iLevelB)
		train(oVec, (svt_stree_node*)m_oTree.getChildA());
	    else
		train(oVec, (svt_stree_node*)m_oTree.getChildB());
	}
    } else {
	// case 2: no, tree not growing anymore, only distance decides
	if (oVec.distanceSq(pWinA->getVector()) < oVec.distanceSq(pWinB->getVector()))
	    train(oVec, (svt_stree_node*)m_oTree.getChildA());
	else
	    train(oVec, (svt_stree_node*)m_oTree.getChildB());
    }

    // check convergence

    // step 15
    m_fC += m_fe_0;
    // step 16
    m_it++;

    // step 17
    if (m_it % m_iT == 0)
    {
	m_fC_tau_old = m_fC_tau;

	// step 18
	m_iTau++;
	// step 19
	if (m_iTau == 1)
	    m_fC_tau = m_fC;
	else {
	    // step 20
	    m_fC_tau = m_fC_tau_old + m_fBeta3*(m_fC - m_fC_tau_old);

	    // step 21
	    if (m_fC_tau_old != 0.0f && (fabs(m_fC_tau_old - m_fC_tau) / fabs(m_fC_tau_old) < m_feta))
	    {
		m_bConv = true;
		updateStats();
	    }
	}

	// step 22
	m_fC = 0;
    }
};

/**
 * train binary tree with stree1 algorithm (stree2 performs much better).
 * function will set the convergence flag if the tree is not changing anymore.
 * \param oVec training vector
 */
void svt_stree::trainStree1(svt_vector4<double> oVec)
{
    // training
    train(oVec, &m_oTree);

    // check convergence

    // step 15
    m_fC += m_fe_0;
    // step 16
    m_it++;

    // step 17
    if (m_it % m_iT == 0)
    {
	m_fC_tau_old = m_fC_tau;

	// step 18
	m_iTau++;
	// step 19
	if (m_iTau == 1)
	    m_fC_tau = m_fC;
	else {
	    // step 20
	    m_fC_tau = m_fC_tau_old + m_fBeta3*(m_fC - m_fC_tau_old);

	    // step 21
	    if (m_fC_tau_old != 0.0f && (fabs(m_fC_tau_old - m_fC_tau) / fabs(m_fC_tau_old) < m_feta))
	    {
		m_bConv = true;
                updateStats();
	    }
	}

	// step 22
	m_fC = 0;
    }
};

/**
 * update the internal statistics (like number of leafs in the tree etc). This function
 * is called after the training has finished and then one can use the functions like getNumLeafes() to aquire information
 * about the created vector quantization tree.
 */
void svt_stree::updateStats()
{
    m_oTree.setLevel(0);
    m_oTree.calcNumLevels(0);
    m_iNumLevels = m_oTree.getLevel();

    m_oTree.setLevel(0);
    m_oTree.calcNumLeaves();
    m_iNumLeaves = m_oTree.getLevel();
}

/**
 * recursive training function
 * \param oVec training vector
 * \param pTree pointer to binary tree
 */
void svt_stree::train(svt_vector4<double> oVec, svt_stree_node* pTree)
{
    // end of tree not reached yet?
    if (pTree->getChildA() != NULL)
    {
	// ok, which child is the winning child for this level?
	if (oVec.distanceSq( ((svt_stree_node*)pTree->getChildA())->getVector() ) < oVec.distanceSq( ((svt_stree_node*)pTree->getChildB())->getVector() ))
	    train(oVec, (svt_stree_node*)pTree->getChildA());
	else
	    train(oVec, (svt_stree_node*)pTree->getChildB());

    } else {

	// pTree is the winning leaf!
	bool bNoAdd = false;

	// do we have to split/prune?
	if (pTree->getCost() > m_fE)
	{
	    // maximum tree size reached - first we have to remove two nodes
	    if (m_iu >= m_iU-1)
	    {
		// prune

		// first step: search node with minimal cost
		svt_stree_node* pTmp = m_oTree.minCost();

		// should not be the root node && step 4.3
		if (pTmp == &m_oTree || pTmp->getCost() / ((svt_stree_node*)pTree)->getCost() > m_fGamma)
		    bNoAdd = true;
		else {

		    // sibling of that node
		    svt_stree_node* pTmpB = (svt_stree_node*)pTmp->getFather()->getChildB();
		    if (pTmpB == pTmp)
			pTmpB = (svt_stree_node*)pTmp->getFather()->getChildA();

		    // step 4.4 - is sibling not a leaf?
		    if (pTmpB->getChildA() != NULL)
		    {
			svt_stree_node* pZ = NULL;

			// step 4.4.2
			if (pTmp->getFather() != NULL)
			    pZ = (svt_stree_node*)pTmp->getFather()->getFather();
			if (pZ != NULL)
			{
			    // step 4.4.4
			    if (pZ->getChildA() == pTmp->getFather())
				pZ->setChildA( pTmpB );
			    else
				pZ->setChildB( pTmpB );

			    // step 4.4.3
			    delete pTmp->getFather();
			    delete pTmp;
			    // missing in paper!
			    m_iu -= 2;
			} else {
			    bNoAdd = true;
			}

		    } else {
			// step 4.5 - is sibling the winning leaf?
			if (pTmpB == pTree)
			{
			    if (pTmp->getFather() != NULL)
			    {
				// step 4.5.4
				pTree = (svt_stree_node*)pTmp->getFather();
				// step 4.5.3
				pTmp->getFather()->setChildA(NULL);
				pTmp->getFather()->setChildB(NULL);
				// step 4.5.2
				delete pTmp;
				delete pTmpB;
				// missing in paper!
				m_iu -= 2;
			    } else
				bNoAdd = true;

			} else {
			    // step 4.6 - other than winning leaf...
			    if (pTmp->getFather() != NULL)
			    {
				// step 4.6.2
				((svt_stree_node*)pTmp->getFather())->setCost( ((svt_stree_node*)pTmp->getFather())->getCost() / 2 );
				// step 4.6.3
				pTmp->getFather()->setChildA(NULL);
				pTmp->getFather()->setChildB(NULL);
				// step 4.6.1
				delete pTmp;
				delete pTmpB;
				// missing in paper!
				m_iu -= 2;
			    } else
				bNoAdd = true;
			}
		    }
		}
	    }

	    // now add two nodes by splitting
	    if (bNoAdd == false)
	    {
		// step 5.4
		svt_stree_node* pChildA = new svt_stree_node;
		pChildA->setVector( ((svt_stree_node*)pTree)->getVector() );
		// step 5.5
		svt_stree_node* pChildB = new svt_stree_node;
		pChildB->setVector( ((svt_stree_node*)pTree)->getVector() * (1.0 + m_fdelta) );
		// step 5.2
		pChildA->setN(1);
		pChildB->setN(1);
		// step 5.3
		pChildA->setCost( ((svt_stree_node*)pTree)->getCost() / 2 );
		pChildB->setCost( ((svt_stree_node*)pTree)->getCost() / 2 );
		// step 5.1
		pTree->setChildren( pChildA, pChildB );
		// step 5.6
		m_iu += 2;
		// step 5.7
		pTree = (svt_stree_node*)pTree->getChildA();
	    }

	    // adjust E
	    m_fE *= m_fgamma;
	}

	// step 7 - calculate distance at winning leaf
	m_fe_0 = pTree->getVector().distanceSq(oVec);
	// step 8 - adjust average distortion
	if (m_it == 1)
	    m_f_e_0 = m_fe_0;
	else
	    m_f_e_0 += m_fBeta2 * (m_fe_0 - m_f_e_0);
	// step 9 - compute relative cost
	m_fe = m_fe_0 / m_f_e_0;
	// step 10 - adjust E
	m_fE += m_fBeta1 * (pTree->getCost() - m_fE);
    }

    // OK, this is a node in the path to the winning leaf

    // adjust relative error (step 12)
    if (m_bFR == false)
	pTree->setCost( pTree->getCost() + m_fe );
    else
	pTree->setCost( pTree->getCost() + 1 );
    // adjust N (step 13)
    pTree->setN( pTree->getN() + 1 );
    // adjust codebook vector (step 14)
    svt_vector4<double> oTmp = pTree->getVector();
    oTmp = oTmp + ((oVec - oTmp) / (double)pTree->getN());
    pTree->setVector( oTmp );

}

/**
 * get progress
 * \return value between 1 and 100
 */
int svt_stree::getProgress()
{
    return (int)(100*((fabs(m_fC_tau_old - m_fC_tau) / fabs(m_fC_tau_old))/m_feta));
}
