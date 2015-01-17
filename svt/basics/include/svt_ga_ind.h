/***************************************************************************
                          svt_ga_ind
                          ----------
    begin                : 06/23/2004
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_GA_IND
#define __SVT_GA_IND

#include <vector>
#include <svt_basics.h>
#include <svt_types.h>
#include <svt_rnd.h>
using namespace std;

#include <math.h>

class svt_ga_ind;

//
// The STL can use different allocation patterns for its containers. Some might not be ideal for multi-threaded applications. Simply define __SVT_GA_XXXX, where XXXX might be MT, POOL, MALLOC, NEW or DEFAULT, and recompile basics.
//
/*#define __SVT_GA_DEFAULT

#ifdef __SVT_GA_MT
#include <ext/mt_allocator.h>
typedef vector< svt_ga_ind, __gnu_cxx::__mt_alloc<svt_ga_ind> > svt_population;
typedef vector< Real64, __gnu_cxx::__mt_alloc<svt_ga_ind> >     svt_array_real64;
#endif

#ifdef __SVT_GA_POOL
#include <ext/pool_allocator.h>
typedef vector< svt_ga_ind, __gnu_cxx::__pool_alloc<svt_ga_ind> > svt_population;
typedef vector< Real64, __gnu_cxx::__pool_alloc<svt_ga_ind> >     svt_array_real64;
#endif

#ifdef __SVT_GA_MALLOC
#include <ext/malloc_allocator.h>
typedef vector< svt_ga_ind, __gnu_cxx::malloc_allocator<svt_ga_ind> > svt_population;
typedef vector< Real64, __gnu_cxx::malloc_allocator<svt_ga_ind> >     svt_array_real64;
#endif

#ifdef __SVT_GA_NEW
#include <ext/new_allocator.h>
typedef vector< svt_ga_ind, __gnu_cxx::new_allocator<svt_ga_ind> > svt_population;
typedef vector< Real64, __gnu_cxx::new_allocator<svt_ga_ind> >     svt_array_real64;
#endif

#ifdef __SVT_GA_DEFAULT
typedef vector< T >	    svt_population;*/
typedef vector< Real64 >    svt_array_real64;
/*#endif*/

typedef enum
{
  UNKNOWN,        //0
  RANDOM,         //1
  CROSSOVER,      //2
  MUTATION,       //3
  MUTATIONALL,    //4
  GENEREFINEMENT, //5
  TRANSPOSITION,  //6
  TABU,	          //7
  AROUND_CENTER
} creation_method;

/**
 * individual class for the GA algorithm
 * \author Stefan Birmanns
 */
class svt_ga_ind
{
    // the genes of the individual (always from 0.0 - 1.0, should be remapped in the fitness function)
    svt_array_real64 m_oGenes;

    // fitness or objective function
    Real64 m_fFitness;
    // probability or sometimes also called fitness function
    Real64 m_fProbability;
    // accumulated probability
    Real64 m_fAccProbability;
    //age
    unsigned int m_iAge;
    
    //some other property value - can be a penalty, an reward etc
    Real64 m_fProp;
    
    //how was the individual generated: random
    creation_method m_eOrigin;
    
    // the niche ID of the individual
    int m_iNicheID;

public:

    /**
     * Constructor
     */
    svt_ga_ind( ) :
	m_fFitness(0.0),
	m_fProbability(0.0),
	m_fAccProbability(0.0),
        m_iAge(0),
        m_fProp(0.0),
        m_eOrigin(UNKNOWN),
	m_iNicheID(-1)
    {
    };

    /**
     * Destructor
     */
    virtual ~svt_ga_ind( )
    {
    };

    /**
     * get the number of genes in this ind
     * \return number of genes
     */
    inline int getGeneCount( ) const
    {
	return m_oGenes.size();
    };

    /**
     * add a gene
     * \param fGene the new gene
     */
    inline void addGene( Real64 fGene )
    {
	m_oGenes.push_back( fGene );
    };

    /**
     * get the genes of this ind
     * \return vector with genes
     */
    inline svt_array_real64 getGenes() const
    {
	return m_oGenes;
    };

    /**
     * set the genes of this ind
     * \param vector with genes
     */
    inline void setGenes( svt_array_real64 oGenes )
    {
	m_oGenes = oGenes;
    };

    /**
     * get a single gene of this ind
     * \param iIndex index of the gene
     * \return int with the gene
     */
    inline Real64 getGene(int iIndex) const
    {
	return m_oGenes[iIndex];
    };

    /**
     * set a single gene of this ind
     * \param iIndex index of gene
     * \param fValue new value
     */
    inline void setGene( int iIndex, Real64 fValue )
    {
	m_oGenes[iIndex] = fValue;
    };

    /**
     * get fitness of this ind
     * \return fitness value
     */
    inline Real64 getFitness() const
    {
	return m_fFitness;
    };

    /**
     * set fitness of this ind
     * \param fFitness new fitness value
     */
    inline void setFitness(Real64 fFitness)
    {
	m_fFitness = fFitness;
    };

    /**
     * compare to inds according to their fitness
     */
    inline bool operator<(const svt_ga_ind& oInd) const
    {
        return m_fFitness < oInd.getFitness();
    };

    /**
     * compare to inds according to their fitness
     */
    inline bool operator==(const svt_ga_ind& oInd) const
    {
        return m_fFitness == oInd.getFitness();
    };

    /**
     * compare to inds according to their genes
     * \return true if both are equal, false otherwise
     */
    inline bool compareGenes(const svt_ga_ind& oInd) const
    {
        svt_array_real64 oGenes = oInd.getGenes();

	for(unsigned int i=0; i<m_oGenes.size(); i++)
	    if (m_oGenes[i] != oGenes[i])
		return false;

        return true;
    };

    /**
     * get probability of the ind to get selected during the roulette wheel process
     * \return probability
     */
    inline Real64 getProbability() const
    {
	return m_fProbability;
    };

    /**
     * set probability of the ind to get selected during the roulette wheel process
     * \param fProbability probability
     */
    inline void setProbability(Real64 fProbability)
    {
	m_fProbability = fProbability;
    };

    /**
     * get accumulated probability of the ind to get selected during the roulette wheel process
     * \return probability
     */
    inline Real64 getAccProbability() const
    {
	return m_fAccProbability;
    };

    /**
     * set accumulated probability of the ind to get selected during the roulette wheel process
     * \param fProbability probability
     */
    inline void setAccProbability(Real64 fAccProbability)
    {
	m_fAccProbability = fAccProbability;
    };

    /**
     * print genes to cout
     */
    virtual void printGenes();
    
    /**
     * print genes to stdout
     */
    void printGenesPf();
    
    /**
     * print genes to file
     */
    void printGenes(FILE *file);


    /**
     * interpret genes of individual as single number
     * \return number
     */
    inline Real64 getValue()
    {
        Real64 fValue = 0.0f;

	for(unsigned int i=0; i<m_oGenes.size(); i++)
            fValue += m_oGenes[i];

        return fValue;

    }
    
    /**
     * Get the Property 
     * \return the property value
     */
    inline Real64 getProp() const
    {
        return m_fProp;
    }
    
     /**
     * set Property 
     * \param the property value
     */
    inline void setProp(Real64 fProp) 
    {
        m_fProp = fProp;
    }
    
    /**
     * get Origin
     * \return method used to generate the individual
     */
    inline creation_method getOrigin() const
    {
        return m_eOrigin;
    }
    
    /**
     * set Origin
     * \return method used to generate the individual
     */
    inline void setOrigin(creation_method eOrigin)
    {
        m_eOrigin = eOrigin;
    }


    /**
     * Get the age
     * \return the age of the individue  = how many generations did it lived
     */
    inline unsigned int getAge() const
    {
        return m_iAge;
    }
    
    /**
     * Get the age
     * \return the age of the individue  = how many generations did it lived
     */
    inline void setAge(unsigned int iAge) 
    {
        m_iAge = iAge;
    }

    

    
    /**
     * increase the age with one year;
     * \ the age of the individue  = how many generations did it lived
     */
    inline void incAge()
    {
       m_iAge++;
    }
    
    /**
     * make age 0 - new born individue
     * 
     */
    inline void resetAge()
    {
       m_iAge=0;
    }
    

    /**
     * cutoff - cuts all genes off that are outside the [0.0 .. 1.0] interval.
     */
    inline void cutoff()
    {
	for(unsigned int i=0; i<m_oGenes.size(); i++)
	{
	    m_oGenes[i] = fabs(m_oGenes[i]);
	    if ( m_oGenes[i] > 1.0f )
                m_oGenes[i] = 1.0f;
	}
    }

    /**
     * calculate the distance between two individuals
     * \param rOther reference to the other individual
     * \return vector distance between the two gene-vectors
     */
    virtual Real64 distance( svt_ga_ind& rOther );
    
    //
    // share fitness
    // 
    
    /**
     * Set niche ID
     */
    inline void setNiche(int iNicheID)
    {
	m_iNicheID = iNicheID;
    };
    
    /**
     * Get niche ID
     */
    inline int getNiche()
    {
	return m_iNicheID;
    };
    
};

#endif
