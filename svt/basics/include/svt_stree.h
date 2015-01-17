/***************************************************************************
                          svt_stree.h
                          -----------
    begin                : 5/21/2004
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_STREE
#define SVT_STREE

#include <svt_basics.h>
#include <svt_binary_tree.h>
#include <svt_vector4.h>
#include <svt_array.h>

///////////////////////////////////////////////////////////////////////////////
// svt_stree_node class
///////////////////////////////////////////////////////////////////////////////

/**
 * svt_stree_node class (represents a single node in the stree binary tree)
 */
class svt_stree_node : public svt_binary_tree
{
    // codebook vector (w_j)
    svt_vector4<double> m_oW;
    // relative cost associated with the jth tree node (e_j)
    double m_fe;
    // number of times the node has been updated
    int m_iN;

    // minimal cost
    static double m_fMinCost;
    // node with minimal cost
    static svt_stree_node* m_pMinCostNode;

    // level
    static int m_iLevel;

public:

    /**
     * Constructor
     * \param oW initial codebook vector
     * \param fe initial error
     */
    svt_stree_node(svt_vector4<double> oW =svt_vector4<double>(0.0, 0.0, 0.0), double fe =0);

    /**
     * set codebook vector
     * \param oW data vector
     */
    inline void setVector( svt_vector4<double> oW )
    {
	m_oW = oW;
    };
    /**
     * get codebook vector
     * \return data vector
     */
    inline svt_vector4<double> getVector() const
    {
	return m_oW;
    };

    /**
     * get the relative cost at this node
     * \return e_j
     */
    inline double getCost() const
    {
	return m_fe;
    };
    /**
     * set the relative cost at this node
     * \param fe the new cost
     */
    inline void setCost(double fe)
    {
	m_fe = fe;
    };

    /**
     * set N_j (number of times the node has been updated)
     * \param iN new value for N
     */
    inline void setN( int iN )
    {
	m_iN = iN;
    };
    /**
     * get N_j
     * \return number of times the node has been updated
     */
    inline int getN( ) const
    {
	return m_iN;
    };

    /**
     * search the node with the minimal cost in the tree
     * \return pointer to svt_stree_node object
     */
    svt_stree_node* minCost();

    /**
     * find the winning leaf node
     * \param oVec training vector
     */
    svt_stree_node* findWinner(svt_vector4<double> oVec);

    /**
     * set the internal level counter (used in several recursive functions like findWinner)
     * \param iLevel the new level
     */
    inline void setLevel(int iLevel)
    {
        m_iLevel = iLevel;
    };
    /**
     * get the internal level counter (used in several recursive functions like findWinner)
     * \return level counter
     */
    inline int getLevel() const
    {
        return m_iLevel;
    };

    /**
     * get all nodes at a certain level
     * \param iLevel index of the level
     * \param oArray reference to svt_array object
     */
    void getNodes(int iLevel, svt_array< svt_stree_node* > &oArray);
    /**
     * get all nodes.
     * \param oArray reference to svt_array object
     */
    void getAllNodes(svt_array< svt_stree_node* > &oArray);

    /**
     * get all leaf nodes.
     * \param oArray reference to svt_array object
     */
    void getLeafNodes( svt_array< svt_stree_node* > &oArray );

    /**
     * calculate the number of levels in the tree (get result with getLevel())
     * \param iLevel set this parameter always to 0!
     */
    void calcNumLevels(int iLevel);

    /**
     * calculate the number of leaves in the tree (get the result with getLevels() - I know it's misleading but this is
     * an internal function anyway. As user you should call svt_stree::getNumLeaves()!
     */
    void calcNumLeaves();

protected:

    /**
     * internal, recursive helper function to calculate the minimal cost
     */
    void calcMinCost();
};

///////////////////////////////////////////////////////////////////////////////
// main algorithm class
///////////////////////////////////////////////////////////////////////////////

/**
 * self-organizing tree for data clustering and online vector quantization
 * Marcos M. Campos, Gail A. Carpenter
 * Neural Networks 14 (2001) 505-525
 */
class DLLEXPORT svt_stree
{
    // distrtion at the winning leaf (e_0)
    double m_fe_0;
    // average distortion of the winning leaf nodes (_e_0)
    double m_f_e_0;
    // relative cost at winning leaf (e_0 / _e_0)
    double m_fe;
    // maximum index of tree nodes
    int m_iu;
    // timestep
    int m_it;
    // splitting threshold
    double m_fE;
    // counter for online convergence criterion
    int m_iTau;
    // total cost for current window
    double m_fC;
    // learning rate for E
    double m_fBeta1;
    // learning rate for average cost
    double m_fBeta2;
    // maximum number of tree nodes
    int m_iU;
    // multiplicative offset used after the tree modification step
    double m_fgamma;
    // pruning threshold
    double m_fGamma;
    // multiplicative offset for initializing new child weights after a split
    double m_fdelta;

    // total cost for current window
    double m_fC_tau;
    // old cost (last cycle);
    double m_fC_tau_old;
    // convergence threshold
    double m_feta;
    // learning rate for C_tau
    double m_fBeta3;
    // did the algorithm converge?
    bool m_bConv;
    // window size
    int m_iT;
    // faithful representation yes/no
    bool m_bFR;

    // the tree (parent node)
    svt_stree_node m_oTree;

    // statistics

    // number of leaves in the tree
    int m_iNumLeaves;
    // number of levels in the tree
    int m_iNumLevels;

public:

    /**
     * Constructor
     */
    svt_stree();

    /**
     * set maximum number of nodes
     * \param iMaxNodex maximum number of nodes
     */
    void setMaxNodes(int iMaxNodes);
    /**
     * get maximum number of nodes
     * \return maximum number of nodes
     */
    int getMaxNodes();

    /**
     * get all nodes at a certain level
     * \param iLevel index of the level
     * \return array of nodes
     */
    svt_array< svt_stree_node* > getNodes(int iLevel);
    /**
     * get all nodes
     * \return array of nodes
     */
    svt_array< svt_stree_node* > getAllNodes();
    /**
     * get all leaf nodes.
     * \return svt_array object with pointers to the leaf nodes
     */
    svt_array< svt_stree_node* > getLeafNodes( );

    /**
     * train binary tree with the stree2 algorithm.
     * function will set the convergence flag if the tree is not changing anymore.
     * \param oVec training vector
     */
    void trainStree2(svt_vector4<double> oVec);

    /**
     * train binary tree with stree1 algorithm (stree2 performs much better).
     * function will set the convergence flag if the tree is not changing anymore.
     * \param oVec training vector
     */
    void trainStree1(svt_vector4<double> oVec);

    /**
     * did the algorithm converge?
     * \return false if the algorithm did not converge yet
     */
    inline bool getConv() const
    {
	return m_bConv;
    };

    /**
     * get progress
     * \return value between 1 and 100
     */
    int getProgress();

    /**
     * set faithful flag
     * \param bFR if true the algorithm will create a faithful representation
     */
    inline void setFaithful(bool bFR)
    {
        m_bFR = bFR;
    };

    /**
     * get faithful flag
     * \return true if the algorithm is creating a faithful representation
     */
    inline bool getFaithful() const
    {
        return m_bFR;
    };

    /**
     * get number of leaves. See updateStats first.
     */
    inline int getNumLeaves()
    {
        return m_iNumLeaves;
    };

    /**
     * get number of levels in the tree. See updateStats first.
     */
    inline int getNumLevels()
    {
        return m_iNumLevels;
    };

    /**
     * get Beta1 parameter (see Campos&Carpenter paper)
     * \return Beta1 parameter
     */
    inline double getBeta1()
    {
	return m_fBeta1;
    };
    /**
     * set Beta1 parameter (see Campos&Carpenter paper)
     * \param fBeta1 the Beta1 parameter
     */
    inline void setBeta1(double fBeta1)
    {
	m_fBeta1 = fBeta1;
    };

    /**
     * get Beta2 parameter (see Campos&Carpenter paper)
     * \return Beta2 parameter
     */
    inline double getBeta2()
    {
	return m_fBeta2;
    };
    /**
     * set Beta2 parameter (see Campos&Carpenter paper)
     * \param fBeta2 the Beta1 parameter
     */
    inline void setBeta2(double fBeta2)
    {
	m_fBeta2 = fBeta2;
    };

    /**
     * get Beta3 parameter (see Campos&Carpenter paper)
     * \return Beta3 parameter
     */
    inline double getBeta3()
    {
	return m_fBeta3;
    };
    /**
     * set Beta3 parameter (see Campos&Carpenter paper)
     * \param fBeta3 the Beta3 parameter
     */
    inline void setBeta3(double fBeta3)
    {
	m_fBeta3 = fBeta3;
    };

    /**
     * get delta parameter (see Campos&Carpenter paper)
     * \return delta parameter
     */
    inline double getdelta()
    {
	return m_fdelta;
    };
    /**
     * set delta parameter (see Campos&Carpenter paper)
     * \param fdelta the Delta parameter
     */
    inline void setdelta(double fdelta)
    {
	m_fdelta = fdelta;
    };

    /**
     * get gamma parameter (see Campos&Carpenter paper)
     * \return gamma parameter
     */
    inline double getgamma()
    {
	return m_fgamma;
    };
    /**
     * set Beta3 parameter (see Campos&Carpenter paper)
     * \param fBeta3 the Beta3 parameter
     */
    inline void setgamma(double fgamma)
    {
	m_fgamma = fgamma;
    };

    /**
     * get the root node of the stree
     * \return pointer to svt_stree_node object
     */
    inline svt_stree_node* getRootNode()
    {
	return &m_oTree;
    };

protected:

    /**
     * update the internal statistics (like number of leafs in the tree etc). Call this function
     * after the training has finished and then use the functions like getNumLeafes() to aquire information
     * about the created vector quantization tree.
     */
    void updateStats();

    /**
     * recursive training function
     * \param oVec training vector
     * \param pTree pointer to binary tree
     */
    void train(svt_vector4<double> oVec, svt_stree_node* pTree);
};

#endif
