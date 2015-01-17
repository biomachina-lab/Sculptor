/***************************************************************************
                          svt_ga
                          ------
    begin                : 06/23/2004
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef __SVT_GA
#define __SVT_GA

#include <svt_stlVector.h>
#include <svt_random.h>
#include <svt_time.h>
#include <svt_threads.h>
#include <svt_semaphore.h>

#ifdef __GAFIT_FORK
#include <sys/wait.h>
#endif

#define EPS 1.0E-7
enum
{
    REINSERTION_ELITIST,
    REINSERTION_ELITIST_UNIQUE,
    REINSERTION_GLOBALRANKING,
    REINSERTION_GLOBALRANKING_UNIQUE,
    REINSERTION_SHARING
};

template<class T> class svt_population : public vector<T>{};

/**
 * Genetic Algorithm
 * \author Stefan Birmanns
 */
template<class T> class svt_ga
{
protected:

    // output debug messages
    bool m_bVerbose;
    
    // encoding: how many genes
    int m_iGenes;

    // the current population
    svt_population<T> m_oPop;
    
    // the next population
    svt_population<T> m_oNextPop;

    // a temporary population
    svt_population<T> m_oTempPop;
    
    // the best population - holds the best m_iBestPopSize individuals in the population
    svt_population<T> m_oBestPop;

    // the population size
    int m_iPopSize;
    
    // the number of best individuals to rememeber at each generation
    unsigned int m_iBestPopSize;

    // how many generations so far?
    int m_iGenerations;

    // selective pressure parameter [1.0 ... 2.0]
    Real64 m_fSP;
    // crossover possibility [0.0 .. 1.0]
    Real64 m_fCrossProb;
    // mutation possibility [0.0 .. 1.0]
    Real64 m_fMutationProb;

    // fixed mutation prob, for the dynamic updating
    Real64 m_fFixedMutationProb;
    Real64 m_fFixedMutationOffset;

    //mutation offset
    Real64 m_fMutationOffset;

    // type of reinsertion scheme used
    int m_iReinsertionScheme;

    // maximum number of generations
    int m_iMaxGen;
    
    //number of generations before syncronize
    int m_iSyncGen;
    
    //some statistics about the population's fitness;
    Real64 m_fAvgFitness, m_fMinFitness,  m_fMaxFitness;
    unsigned int m_iNoUniqueInd;

    //how many time did the algo find the same best individual
    int m_iIdentBestIsSame;
    
    //number of interations when best individual does not change - used to stop run
    int m_iIdentBestIsSameMax;

    // minimal distance cutoff towards the top individual
    Real64 m_fCutoffDistance;
    // minimal distance cutoff penalty factor
    Real64 m_fCutoffDistancePenalty;
    // shall we apply the mutation to all the individuals?
    bool m_bMutateAll;
    
    // how many should be mutated
    Real64 m_fMutateAllProportion;


    // probability for the transposition to happen
    Real64 m_fTranspositionProb;
    
    //is the GA done
    bool m_bDone;
    
    // indicates whether the thread is still running - needed once m_bDone was set true from true from exterior program
    bool m_bIsThreadRunning;
    
    // semaphore of the thread 
    svt_semaphore m_oThreadSema;

    // tabu search array with the tabu region centers
    unsigned int m_iTabuWindowSize;       // size of the tabu window over which we average the top individual distances
    svt_population<T> m_oTabus;              // the array with the tabu regions
    svt_array_real64 m_oTabuWindow;       // the tabu window with the distances of the top individuals
    Real64 m_fTabuThreshold;              // when do we consider an average distance as being too small?
    Real64 m_fTabuRegionSize;             // if the distance between an individual and a stored tabu region is smaller than this value, the individual is discarded

    // stopping criterion
    Real64 m_fStopScore;

    // current run
    unsigned int m_iRun;
    
    //current parallel run
    unsigned int m_iParallelRun;

    // current thread
    unsigned int m_iThread;
    
    //max thread
    unsigned int m_iMaxThread;
    
    //The size of a niche 
    Real64 m_fNicheSize;
    
    //The maximal allowed population per niche - expressed as a proportion
    Real64 m_fMaxPopPerNiche;
    
    //penalty for individuals in the same niche
    Real64 m_fSameNichePenalty;
    
    int m_iRefinementMaxMutPerGene;
    
    //how many times was the fitness updated
    unsigned int m_iFitnessUpdateCount;
    
    //time required for the update of one generation: in seconds
    Real64 m_fTimeGen;
    
    //array of GA that run in parallel
    vector< svt_ga* > m_oGA_Array;

    // a parent ga - null if this is the main thread; the thread that started the parallel threads
    svt_ga* m_pParentGA;

public:

    /**
     * Constructor
     * \param iGenes how many genes?
     */
    svt_ga( int iGenes );
    
    /**
     * Destructor
     */
    virtual ~svt_ga();

    /**
     * run the genetic algorithm
     * \return vector with fitness values
     */
    svt_array_real64 run();

    //
    // Population
    //

    /**
     * generate initial population
     * \param iNum number of inds in this population
     */
    virtual void initPopulation(int iNum);
    /**
     * create a new individual
     */
    virtual T initIndividual( );

    /**
     * generate new population (selection, recombination, mutation, reinsertion)
     */
    virtual void updatePopulation();

    /**
     * Get the current population
     */
    svt_population<T> getPopulation();

    /**
     * get best individual
     */
    T getBest();
    
    /**
     * Get the tabu regions
     */
    svt_population<T> getTabuRegions();

    /**
     *  get best tabu individual
     */
    T getBestTabu();
    
    /**
    * Set the tabu regions
    */
    void setTabuRegions(svt_population<T>& rTabuPop);
    /**
     * Delete all tabu Regions
     */
    void delTabuRegions();
    /**
     * Set the current population
     */
    void setPopulation(svt_population<T>& rPop);
    
    /**
     * Update the best population
     */
    Real64 updateBestPopulation();
    
    /**
    * get the fitness of the best population 
    */
    svt_array_real64 getBestPopFitness();

    /**
     * update fitness
     */
    void updateFitness();
    
    /**
     * get how many times the fitness was updated
     */
     unsigned int getFitnessUpdateCount();

    /**
     * get highest fitness
     */
    Real64 getHighestFitness();

    /**
     * print individual with highest fitness
     */
    virtual void printHighestFitness();

    /**
     * insert an individual
     */
    void insertInd( T& rInd );

    //
    // Parameters
    //

    /**
     * set population size 
     * \param iPopSize number of individuals in the population
     */
    void setPopSize( int iPopSize );
    
    /**
     * get population size 
     */
    int getPopSize();
    
    /**
     * set the size of the best population
     * \param iBestPopSize number of individuals in the population
     */
    void setBestPopSize( int iBestPopSize );
    
    /**
     * get size of the best population - population of best individuals
     */
    int getBestPopSize();
    
    /**
     * set maximum number of generations
     * \param iMaxGen maximum number of generations
     */
    void setMaxGen( int iMaxGen );

    /**
     * get maximum number of generations
     * \return maximum number of generations
     */
    int getMaxGen( ) const;
    
    /**
     * set the number of generations before update
     * \param iSyncGen number of generations
     */
    void setSyncGen( int iSyncGen );
    
     /**
     * get the number of generations before update
     * \return iSyncGen number of generations
     */
    int getSyncGen();

    /**
     * get the current generation
     * \return m_iGenerations
     */
    int getCurrGen() const;
    /**
     * set the current generation
     * \param iGenerations the new current generation index
     */
    void setCurrGen(int iGenerations);

    /**
     * set the threshold score value after which the ga will stop, even if the maximum number of generations was not reached
     * \param fStopScore ga will stop once that score is exceeded
     */
    void setStopScore( Real64 fStopScore );
    /**
     * get the threshold score value after which the ga will stop, even if the maximum number of generations was not reached
     * \return ga will stop once that score is exceeded
     */
    Real64 getStopScore( );

    /**
     * set run number
     * \param iRun run number
     */
    void setRun( int iRun );
    
    /**
     * get the run number
     * \return run number
     */
    int getRun( ) const;
    
    /**
     * set parallel run number
     * \param iRun run number
     */
    void setParallelRun( int iParallelRun );
    
    /**
     * get the run number
     * \return run number
     */
    int getParallelRun( ) const;

    
    /**
     * set thread number
     * \param iThread thread number
     */
    void setThread( int iThread );

    /**
     * get the thread number
     * \return thread number
     */
    int getThread( ) const;
    
    /**
     * set max thread number
     * \param iMaxThread thread number
     */
    void setMaxThread( int iMaxThread );

    /**
     * get the max thread number
     * \return max thread number
     */
    int getMaxThread( ) const;

    /**
     * set the selective pressure
     * \param fSP selective pressure factor [1.0 ... 2.0] (default: 1.2)
     */
    void setSelectivePressure(Real64 fSP);
    /**
     * get the selective pressure
     * \return selective pressure factor [1.0 ... 2.0] (default: 1.2)
     */
    Real64 getSelectivePressure() const;

    /**
     * set the crossover probability
     * \param fCrossProb crossover probability [0.0 ... 1.0] (default: 0.95)
     */
    void setCrossoverProb(Real64 fCrossProb);
    /**
     * get the crossover probability
     * \return crossover probability [0.0 ... 1.0] (default: 0.95)
     */
    Real64 getCrossoverProb() const;

    /**
     * set the mutation probability
     * \param fMutationProb mutation probability [0.0 ... 1.0] (default: 0.05)
     */
    void setMutationProb(Real64 fMutationProb);
    /**
     * get the mutation probability
     * \return mutation probability [0.0 ... 1.0] (default: 0.05)
     */
    Real64 getMutationProb();

    /**
     * set the mutation offset 
     * \param fMutationOffset
     */
    void setMutationOffset(Real64 fMutationOffset);
    /**
     * get the mutation offset
     * \return the mutation offset
     */
    Real64 getMutationOffset();
    
    /**
     * set the number of mutation that is applied on one gene durring local refinement
     */
    void setRefinementMaxMutPerGene(int iRefinementMaxMutPerGene);
        /**
     * get the number of mutation that is applied on one gene durring local refinement
     */
    int getRefinementMaxMutPerGene();
    

    /**
     * set the transposition probability
     * \param fTranspositionProb transposition probability [0.0 ... 1.0] (default: 0.05)
     */
    void setTranspositionProb(Real64 fTranspositionProb);
    /**
     * get the transposition probability
     * \return transposition probability [0.0 ... 1.0] (default: 0.05)
     */
    Real64 getTranspositionProb();

    /**
     * Set the cutoff distance parameter. All individuals with a distance lower than the one set here, will get penalized.
     * \param fCutoffDistance the cutoff distance
     */
    void setDistanceThreshold( Real64 fCutoffDistance );
    /**
     * Get the cutoff distance parameter. All individuals with a distance lower than the one set here, will get penalized.
     * \return the cutoff distance
     */
    Real64 getDistanceThreshold( );

    /**
     * Set the cutoff distance penalty parameter. All individuals with a distance lower than the one set here, will get multiplied with the factor set here.
     * \param fCutoffDistancePenalty the cutoff distance penalty
     */
    void setDistanceThresholdPenalty( Real64 fCutoffDistancePenalty );
    /**
     * Get the cutoff distance penalty parameter. All individuals with a distance lower than the one set here, will get multiplied with the factor set here.
     * \return the cutoff distance penalty
     */
    Real64 getDistanceThresholdPenalty( );

    /**
     * Shall the entire population be mutated? Normally, the GA mutates only few individuals of the new population, based on the mutationprobability. These individuals mostly are
     * new gene sets, created through crossover (crossover probability is typically very high).
     * If the parameter gets set to true, all old individuals get mutated, which can speed up the convergence of the algorithm.
     * \param bMutateAll if set to true all individuals get mutated.
     */
    void setMutateAll( bool bMutateAll );
    /**
     * Shall the entire population be mutated? Normally, the GA mutates only few individuals of the new population, based on the mutationprobability. These individuals mostly are
     * new gene sets, created through crossover (crossover probability is typically very high).
     * If the parameter gets set to true, all old individuals get mutated, which can speed up the convergence of the algorithm.
     * \return if true all individuals get mutated.
     */
    bool getMutateAll();
    
    /**
     * What proportion of the entire population should be mutated
     * \param bMutateAllProportion 1 means all individuals, 0 means none
     */
    void setMutateAllProportion( Real64 fMutateAllProportion );
    
    /**
     * What proportion of the entire population should be mutated
     * \return bMutateAllProportion 1 means all individuals, 0 means none
     */
    Real64 getMutateAllProportion();
    
    /**
     * get the time to compute one generation (as computed during the last generation)
     */
    Real64 getTimeGen();
   
    //
    // Tabu Search
    //

    /**
     * Set the tabu search window size. The tabu search computes the gene distances of the top-individual over time, with a moving window. It averages all those distance values.
     * If the distances vary a lot, because constantly completely new solutions get to the top, everything is considered fine. If the average drops, and only small differences
     * can be seen, this probably means premature convergence. The size of the window can be adjusted with this function.
     * \param iTabuWindowSize new size of the tabu-search window
     */
    void setTabuWindowSize( unsigned int iTabuWindowSize );
    /**
     * Get the tabu search window size. The tabu search computes the gene distances of the top-individual over time, with a moving window. It averages all those distance values.
     * If the distances vary a lot, because constantly completely new solutions get to the top, everything is considered fine. If the average drops, and only small differences
     * can be seen, this probably means premature convergence. The size of the window can be accessed with this function.
     * \return size of the tabu-search window
     */
    unsigned int getTabuWindowSize( );

    /**
     * At some point the distances of the top individuals get really small and we consider this as stagnation of the GA. With this function one can set the threshold, if the
     * average distance is lower, we store the top individual in an array and remove all individuals from this region.
     * \fTabuThreshold the new threshold below which we say the GA stagnates
     */
    void setTabuThreshold( Real64 fTabuThreshold );
    /**
     * At some point the distances of the top individuals get really small and we consider this as stagnation of the GA. With this function one can access the threshold, if the
     * average distance is lower, we store the top individual in an array and remove all individuals from this region.
     * \return threshold below which we say the GA stagnates
     */
    Real64 getTabuThreshold( );

    /**
     * If the distance between an individual and a stored tabu region is smaller than this value, the individual is discarded.
     *\param fTabuRegionSize the new size of the tabu regions
     */
    void setTabuRegionSize( Real64 fTabuRegionSize );
    /**
     * If the distance between an individual and a stored tabu region is smaller than this value, the individual is discarded.
     *\return the size of the tabu regions
     */
    Real64 getTabuRegionSize( );

    /**
     * check whether the ind is in one of the tabu regions
     * \param pInd
     **/
    bool isInTabuReg(T* pInd);

    /**
     * set the parent GA
     * \param pParentGa - the ga that started this thread ; NULL if the main thread
     */
    void setParentGA(svt_ga* pParentGA);

    /**
     * get the parent GA
     * \return pParentGa - the ga that started this thread ; NULL if the main thread
     */
    svt_ga* getParentGA();

    /**
     * refine an individual; 
     * \param the individual that will be refined
     */
    virtual void refineInd(T * pInd);
    
    //
    //  Sharing
    //
    
    /**
     * set the Niche size
     * \param the new nicheSize
     */
    void setNicheSize(Real64 fNicheSize);
    
    /**
     * get the Niche size
     * \return nicheSize
     */
    Real64 getNicheSize();

    /**
     * set the maximum allowed population per Niche - expressed as a proportion of the original population
     * Remarks: beyong this value (5-10%) - individuals have their fitness set to 0
     * \param proportion of individuals allowed in one niche
     */
    void setMaxPopPerNiche(Real64 fMaxPopPerNiche);
    
    /**
     * get the maximum allowed population per Niche - expressed as a proportion of the original population
     * \param proportion of individuals allowed in one niche
     */
    Real64 getMaxPopPerNiche();
    
    /**
     * set the Niche distance penalty - penalize individuals in the same niche according to their rank to the top individual
     * \param how much will individuals be penalized 
     */
    void setSameNichePenalty(Real64 fSameNichePenalty);
    
    /**
     * get the Niche distance penalty - penalize individuals in the same niche according to their rank to the top individual
     * \param how much will the 
     */
    Real64 getSameNichePenalty();

    

    //
    // Threads
    //

    /**
    * set running state of the thread;
    */ 
    void setIsThreadRunning( bool bIsThreadRunning);

    /**
     * \return whether the thread is running;
     */
    bool getIsThreadRunning();

    /**
     * set the variable m_bDone - (should the run of the GA stop cause it reached finish condition)
     * \param bDone - the state
     */
    void setDone( bool bDone);

    /**
     * get the variable m_bDone - (should the run of the GA stop cause it reached the finish condition?)
     * \return m_bDone - the state
     */
    bool getDone() const;

protected:

    //
    // Selection
    //

    /**
     * selection of a new generation
     */
    void selection();

    /**
     * sort population according to fitness
     */
    void sortPopulation();
    /**
     * sort next population according to fitness
     */
    void sortNextPopulation();

    //
    // Recombination
    //

    /**
     * recombination of the selected members of the old population to form a new generation
     */
    void recombination();
    
    /**
     * uniform (coin-flipping) crossover operator
     * \param rParentA reference to first parent object
     * \param rParentB reference to first parent object
     * \param pNewIndA pointer to new ind
     * \param pNewIndB pointer to second new ind
     */
    virtual void crossover(T& rParentA, T& rParentB, T* pNewIndA, T* pNewIndB );

    /**
     * 1 point crossover operator
     * \param rParentA reference to first parent object
     * \param rParentB reference to first parent object
     * \param pNewIndA pointer to new ind
     * \param pNewIndB pointer to second new ind
     */
    void crossover1Point( T& rParentA, T& rParentB, T* pNewIndA, T* pNewIndB );
    /**
     * 2 point crossover operator
     * \param rParentA reference to first parent object
     * \param rParentB reference to first parent object
     * \param pNewIndA pointer to new ind
     * \param pNewIndB pointer to second new ind
     */
    void crossover2Point( T& rParentA, T& rParentB, T* pNewIndA, T* pNewIndB );
    /**
     * 2 point shuffle crossover operator
     * \param rParentA reference to first parent object
     * \param rParentB reference to first parent object
     * \param pNewIndA pointer to new ind
     * \param pNewIndB pointer to second new ind
     */
    void crossover2PointShuffle( T& rParentA, T& rParentB, T* pNewIndA, T* pNewIndB );
    
    /**
     * uniform (coin-flipping) crossover operator
     * \param rParentA reference to first parent object
     * \param rParentB reference to first parent object
     * \param pNewIndA pointer to new ind
     * \param pNewIndB pointer to second new ind
     */
    void crossoverUniform(T& rParentA, T& rParentB, T* pNewIndA, T* pNewIndB );
    /**
     * intermediate crossover operator
     * \param rParentA reference to first parent object
     * \param rParentB reference to first parent object
     * \param pNewIndA pointer to new ind
     * \param pNewIndB pointer to second new ind
     */
    void crossoverIntermediate(T& rParentA, T& rParentB, T* pNewIndA, T* pNewIndB );
    /**
     * subgroup crossover operator
     * \param rParentA reference to first parent object
     * \param rParentB reference to first parent object
     * \param pNewInd pointer to new ind
     */
    void crossoverSubgroup( T& rParentA, T& rParentB, T* pNewInd );
    /**
     * arithmethic crossover operator - xoff = (alpha)*x1+(1-alpha)*x2 - alpha is always random
     * \param rParentA reference to first parent object
     * \param rParentB reference to first parent object
     * \param pNewIndA pointer to new ind
     * \param pNewIndB pointer to second new ind
     */
    void crossoverArithmetic(T& rParentA, T& rParentB, T* pNewIndA, T* pNewIndB );

    //
    // Mutation
    //

    /**
     * mutation
     */
    void mutation();

    /**
     * uniform mutation
     * \param iInd index of individual
     */
    void mutationRandom(int iInd);
    
    /**
     * uniform mutation
     * \param iInd index of individual
     */
    void mutationUniform(int iInd);
    /**
     * moving window mutation
     * \param iInd index of individual
     */
    void mutationMovingWindow(int iInd);
    
    /**
     * mutation with a normal distribution
     * \param iInd index of individual
     */
    void mutationNorm(int iInd);
    
    /**
     * mutation with a cauchy distribution
     * \param iInd index of individual
     */
    void mutationCauchy(int iInd);
    
    /**
     * mutation with a cauchy distribution
     * \param oInd the individual
     */
    void mutationCauchy(T *oInd);

    
    /**
    * mutation with a cauchy distribution - 3 mutation per individual 
    * \param iInd index of individual
    */
    void mutationMultiCauchy(int iInd);
    
    /**
     * uniform mutation for 1 in 7 genes
     * \param iInd index of individual
     */
    void mutationMultipoint(int iInd);
    
    /**
     * random mutation for 1 in 7 genes
     * \param iInd index of individual
     */
    void mutationMultiRandom(int iInd);

    /**
     * Mutate the entire population (by doubling its size).
     */
    void mutationAllPop();
    
    /**
     * custom mutation (can be changed by derived class, default implementation just calls mutationBGA)
     * \param iInd index of individual
     */
    virtual void mutationCustom(int iInd);

    //
    // Add new random individuals
    //

    /**
     * adds new random individuals in the population
     */
    void addRandomIndividuals();

    //
    // Transposition
    //

    /**
     * transposition
     */
    virtual void transposition();
    
    /**
     * flips two genes between the same individual
     * \param rParentA reference to object
     * \param pNewIndA pointer to new ind
     */
    virtual void transpositionUniform(T& rParentA, T* pNewIndA );

    //
    // Reinsertion
    //

public:

    /**
     * set Reinsertion scheme
     * \param iReinsertionScheme the reinsertion scheme to used 
     */
    void setReinsertionScheme( unsigned int iReinsertionScheme );

protected:

    /**
     * reinsertion
     */
    void reinsertion();
    /**
     * elitist reinsertion - replace the 50% worst parents with 50% best new individuals
     */
    void reinsertion_elitist();
    
    /**
     * elitist reinsertion - make duplicates fitness 0; replace the 50% worst parents with 50% best new individuals
     */
    void reinsertion_elitist_unique();
    
    /**
     * share fitness among multiple nishes 
     */
    void reinsertion_sharing();
    
    /**
     * share the fitness between the individuals of the population: a %percent of individuals are allowed in one niche - the rest are just killed and need to populate other niches
     * \param oPop - the individuals come form these population
     */
    void shareFitness(vector< T>& oPop);

    /**
     * reinsertion - global reinsertion based on the global ranking.
     * Both, the old and the new population are ranked together and only the best individuals are inserted into the next gen.
     */
    void reinsertion_globalranking();
    /**
     * reinsertion - global reinsertion based on the global ranking but only unique individuals are allowed
     */
    void reinsertion_globalranking_unique();

    //
    // Fitness
    //

    /**
     * update fitness - this function has to get overloaded!
     * \param pInd pointer to individual that should get updated
     */
    virtual void updateFitness(T* pInd) = 0;

    /**
     * Is the current individual a valid individual
     */
    virtual bool isValid(T* pInd);

    /**
     * Function to check verify whether the integrity of the genes is maintained
     * \param pInd the individual for which to check genes
     * \return the if individual correct
     */
    virtual void makeValid(T* pInd);
    
    
    /**
     * Create an object 
     */
    virtual svt_ga* createObject() = 0;

public:
  
    /**
     * Penalize individuals that are similar to allow a more diverse population
     * \param the population 
     * \param fCutoffDistance the gene distance between which they get penalized 
     * \param fCufoffDistancePenalty how much do they get penalized
     */
    void penalizeSimilar(svt_population<T> &oPop, Real64 fCutoffDistance, Real64 fCutoffDistancePenalty);
    
    /**
     * Discard invalid(fitness value=0) individuals
     * \param oPop the population
     */
    void discardNullInd(svt_population<T> &oPop);

    //
    // Print out diagnostic information
    //

    /**
     * print results (to cout)
     */
    virtual void printResults();
    
    /**
     * print population 
     * \param the population 
     */
    void printPop(svt_population<T>& oPop);
    
    /**
     * print results (to cout)
     */
    virtual void printNextPop();
    
    /**
     * Print the Min fitness, the avergage fitness and the Max fitness
     */
    void printStatistics();
    
    /**
     * print the fitness of each individual of the population 
     */
    void printPopFitness(char* pFname);
    
    /**
     * output results to files
     */
    virtual void outputResult(bool bTabuAdded=false) = 0;
    
    /**
     * output the configuration of the program
     */
    virtual void writeConfiguration(char * pFilename);

    /**
     * Write the top scoring solutions to the disk
     * \param oPop the population of solutions 
     * \param iWriteSolutions how many solutions to write
     */
    virtual void writeSolutions(svt_population<T> &oPop, unsigned int iWriteSolutions, char *pFilename)= 0;

    
    ///////////////////////////////////////////////////////////////////////////////
    // run ga in thread 
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * function to create the thread 
     */
    void initThread();
    
    /**
     * function to create the thread 
     */
    void initThreads();

    
    /**
     * function to create the thread 
     * \return a population
     */
    virtual svt_population<T> execute();
   

    /**
     * Clear the content of the threads
     */
    virtual void clearThreads();
        
    /**
     * Refine population
     * \param oPop what population
     * \param iNoInd4Refinement how many individuals are to be refined
     */
    void refine(svt_population<T> &oPop, unsigned int iNoInd4Refinement);
    

    
};

///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 * \param iGenes how many genes?
 */
template<class T> 
svt_ga<T>::svt_ga( int iGenes ) :
    m_bVerbose( false ),
    m_iBestPopSize( 1 ),
    m_fSP( 1.3 ),
    m_fCrossProb( 0.95 ),
    m_fMutationProb( 0.05 ),
    m_fMutationOffset(0.05f),
    m_iReinsertionScheme( REINSERTION_ELITIST_UNIQUE ),
    m_iMaxGen( 1000 ),
    m_iSyncGen ( 100 ),
    m_fCutoffDistance(0.01875),
    m_fCutoffDistancePenalty(0.90),
    m_bMutateAll(true),
    m_fMutateAllProportion(1.00),
    m_fTranspositionProb( 0.05 ),
    m_bDone(false),
    m_iTabuWindowSize( 50 ),
    m_fTabuThreshold( 0.0015 ),
    m_fTabuRegionSize( 0.000625 ),
    m_fStopScore( 0.0 ),
    m_iRun( 0 ),
    m_iParallelRun( 0 ),
    m_fNicheSize( 0.035 ),
    m_fMaxPopPerNiche(0.05),
    m_fSameNichePenalty(0.99),
    m_iRefinementMaxMutPerGene(20),
    m_iFitnessUpdateCount(0),
    m_fTimeGen (0.0f),
    m_pParentGA (NULL) 
{
    m_iGenerations = 0;
    m_iGenes = iGenes;
    m_iIdentBestIsSameMax = 40;    
}

/**
 * Destructor
 */
template<class T> 
svt_ga<T>::~svt_ga() 
{
    for( unsigned int iThread=0; iThread < m_iMaxThread && iThread < m_oGA_Array.size() ; iThread++ )
	if (m_oGA_Array[iThread]!=NULL)
            delete(m_oGA_Array[iThread]);
    m_oGA_Array.clear();
}

/**
 * run the genetic algorithm
 * \return vector with fitness values
 */
template<class T> 
svt_array_real64 svt_ga<T>::run()
{
    if (m_bVerbose) 
	printf("%s function: run() \n", svt_getName( typeid(this).name() ));
    
    m_bDone = false;
    svt_array_real64 oFitVec;

    // init first population (randomly)
    //initPopulation( m_iPopSize );

    m_oTempPop.reserve( m_oPop.size() * 3 );
    m_oNextPop.reserve( m_oPop.size() * 2 );

    long int iTime ;
    // main loop
    while( m_bDone == false )
    { 
	//get time 
	iTime = svt_getToD();
	
	updatePopulation();
	
	//stop ga if m_bDone was set true form exterior program during thread run
	if (m_bDone) return oFitVec ; 
	
	updateBestPopulation();
	outputResult();
	
	// store rmsd
	oFitVec.push_back( ((-1.0) * (getHighestFitness() - 1.0E10) ) );

	m_iGenerations++;
	if ( m_iGenerations > m_iMaxGen - 1 )
	    m_bDone = true;
	
	m_fTimeGen = (Real64) (svt_getToD() - iTime) / 1000.0f;
	//svtout << "Gen:" << m_iGenerations << " time: " << m_fTimeGen << endl; 
    }

    return oFitVec;
}

///////////////////////////////////////////////////////////////////////////////
// population
///////////////////////////////////////////////////////////////////////////////

/**
 * generate initial population
 * \param iNum number of inds in this population
 */
template<class T>
void svt_ga<T>::initPopulation(int iNum)
{
    m_oPop.clear();
    m_oTabuWindow.clear();

    for(int i=0; i<iNum; i++)
    {
        // create new object
        T oInd = initIndividual();
        m_oPop.push_back( oInd );
    }

    m_iPopSize = iNum;
    m_iGenerations = 0;
}

/**
 * create a new individual
 */
template<class T>
T svt_ga<T>::initIndividual( )
{
    // create new object
    T oInd;

    // random assignments
    for (int j=0; j<m_iGenes; j++)
        oInd.addGene( svt_genrand() );

    oInd.setOrigin(RANDOM);
    oInd.resetAge();

    makeValid( &oInd );

    return oInd;
}

/**
* generate new population (selection, recombination, mutation, reinsertion)
*/
template<class T>
void svt_ga<T>::updatePopulation()
{
    m_oThreadSema.P();
    for(unsigned int i=0; i<m_oPop.size(); i++)
        m_oPop[i].incAge();

    
    // only update the fitness of all individuals for the first generation - the rest is done in reinsertion
    if (m_iGenerations==0)
        svt_ga<T>::updateFitness();
    
    // selection
    selection();
    
    // recombination
    recombination();
    
    // mutation
    mutation();
    
    // shall we mutate the entire population?
    if (m_bMutateAll)
        mutationAllPop();
    
    //add random individuals
    //addRandomIndividuals();
    
    //flip genes between the same individual
    transposition();
    
    // reinsertion
    reinsertion();
    
    // bring all genes back to the [0.0 .. 1.0] interval
    for(unsigned int i=0; i<m_oPop.size(); i++)
    	makeValid(&m_oPop[i]);
        
    m_oThreadSema.V();
}

/**
 * Get the current population
 */
template<class T>
svt_population<T> svt_ga<T>::getPopulation()
{
    return m_oPop;
};

/**
 * get best individual
 */
template<class T>
T svt_ga<T>::getBest()
{
    T oInd;
    if (m_oBestPop.size()>0)
        oInd = m_oBestPop[0];

    return oInd;
};
    
/**
 * Get the tabu regions
 */
template<class T>
svt_population<T> svt_ga<T>::getTabuRegions()
{
    return m_oTabus;
};

/**
 *  get best tabu individual
 */
template<class T>
T  svt_ga<T>::getBestTabu()
{
    T oInd;
    if (m_oTabus.size()>0)
    {
	//sort(m_oTabus.begin(), m_oTabus.end());
	oInd =  m_oTabus[ m_oTabus.size() - 1 ]; // tabus should be sorted at all time  
    }

    return oInd;
};
    


/**
 * Set the tabu regions
 */
template<class T>
void svt_ga<T>::setTabuRegions(svt_population<T>& rTabuPop)
{
    m_oTabus = rTabuPop;
};


/**
 * Delete all tabu Regions
 */
template<class T>
void svt_ga<T>::delTabuRegions()
{
    m_oTabus.clear();
};

/**
 * Set the current population
 */
template<class T>
void svt_ga<T>::setPopulation(svt_population<T>& rPop)
{
    m_oTabuWindow.clear();
    m_oPop.clear();
    m_oPop = rPop;
    m_iPopSize = rPop.size();
};

/**
 * Update the best population
 */
template<class T>
Real64 svt_ga<T>::updateBestPopulation()
{
    m_oThreadSema.P();
    
    sortPopulation();
    
    m_oBestPop.clear();
    for (unsigned int iIndex=0; iIndex < m_iBestPopSize; iIndex++)
        m_oBestPop.push_back( m_oPop[ m_oPop.size() - iIndex -1 ]);
    
    m_oThreadSema.V();

    return m_oPop[ m_oPop.size() - 1 ].getFitness();
}


/**
 * get the fitness of the best population 
 */
template<class T>
svt_array_real64 svt_ga<T>::getBestPopFitness()
{
    svt_array_real64 oVec;
    if (m_oBestPop.size()==0) // population was not yet been initialized or no generation has been created
        return oVec;
    
    sort (m_oBestPop.rbegin(), m_oBestPop.rend());
    
    for (unsigned int iIndex=0; iIndex < m_iBestPopSize; iIndex++)
        oVec.push_back( m_oBestPop[iIndex].getFitness() );
    
    return oVec;
};

/**
 * get how many times the fitness was updated
 */
template<class T>
unsigned int svt_ga<T>::getFitnessUpdateCount()
{
    return m_iFitnessUpdateCount;
};


/**
 * get highest fitness
 */
template<class T>
Real64 svt_ga<T>::getHighestFitness()
{
    sortPopulation();
    return m_oPop[ m_oPop.size() -1 ].getFitness();
}

/**
 * print individual with highest fitness
 */
template<class T>
void svt_ga<T>::printHighestFitness()
{
    sortPopulation();
    printf("%3i [%2li] = %3.5f ", m_iGenerations, m_oPop.size() -1, (-1.0) * (getHighestFitness() - 1.0E10) );
    m_oPop[m_oPop.size() -1].printGenes();
}

///////////////////////////////////////////////////////////////////////////////
// Selection
///////////////////////////////////////////////////////////////////////////////

/**
 * selection
 * rank-based selection
 */
template<class T>
void svt_ga<T>::selection()
{
    if (m_bVerbose) 
	printf("%s function: selection() - in\n", svt_getName( typeid(this).name() ));

    // first step: sort the current population
    sortPopulation();

    m_fAvgFitness = 0.0f;
    m_fMaxFitness = 0.0f;
    m_fMinFitness = 1e10;
    Real64 fFitness;

    // second step: calculate the probability, based on the selective pressure parameter
    Real64 fAcc = 0.0;
    unsigned iPopSize = m_oPop.size();
    for(unsigned int i=0; i<iPopSize; i++)
    {
	m_oPop[i].setProbability( (2.0 - m_fSP) + 2.0 * (m_fSP  - 1.0) * i / (m_oPop.size() - 1) );
        fAcc += m_oPop[i].getProbability();
	m_oPop[i].setAccProbability( fAcc );

        //compute some statistics
        fFitness = m_oPop[i].getFitness();
        m_fAvgFitness+=fFitness;
        
        if (fFitness < m_fMinFitness && fFitness > 0.0)
            m_fMinFitness = fFitness;
        
        if (fFitness > m_fMaxFitness)
            m_fMaxFitness = fFitness;
    }
    m_fAvgFitness /= m_oPop.size(); 
    
    // third step: choose the individuals for the next generation according to the accumulated probabilities
    m_oNextPop.clear();
    Real64 fRand;
    unsigned int iIndex;
    for(unsigned int i=0; i<m_oPop.size(); i++)
    {
        fRand = svt_genrand();
	fRand *= m_oPop.size();

	iIndex = 0;
        while (iIndex < m_oPop.size() && m_oPop[iIndex++].getAccProbability() < fRand ) { };
	m_oNextPop.push_back(m_oPop[iIndex-1]);
        m_oNextPop[ m_oNextPop.size()-1 ].setOrigin( UNKNOWN );
    }

    //consider the TABU regions to support the building block hypothesis
    if( m_oTabus.size() > 10 )
    {
	sort(m_oTabus.begin(), m_oTabus.end());
	//svtout << m_oTabus[0].getFitness() << " " << m_oTabus[m_oTabus.size() -1 ].getFitness() << endl;
    }
    for (unsigned int i=0; i<m_oTabus.size() && i< 10; i++)
    {
	m_oNextPop.push_back( m_oTabus[m_oTabus.size() - 1 - i] );
        m_oNextPop[ m_oNextPop.size()-1 ].setOrigin( TABU );
    }
    
    if (m_bVerbose) 
	printf("%s function: selection() - out\n", svt_getName( typeid(this).name() ));
}

/**
 * sort population according to fitness
 */
template<class T>
void svt_ga<T>::sortPopulation()
{
    sort(m_oPop.begin(), m_oPop.end());
}

/**
 * sort next population according to fitness
 */
template<class T>
void svt_ga<T>::sortNextPopulation()
{
    sort(m_oNextPop.begin(), m_oNextPop.end());
}

/**
 * insert an individual
 */
template<class T>
void svt_ga<T>::insertInd( T& rInd )
{
    m_oPop.push_back( rInd );
}

///////////////////////////////////////////////////////////////////////////////
// Parameters
///////////////////////////////////////////////////////////////////////////////

/**
 * set population size
 * \param iPopSize number of individuals in the population
 */
template<class T>
void svt_ga<T>::setPopSize( int iPopSize )
{
    m_iPopSize = iPopSize;
};
    
/**
 * get population size
 */
template<class T>
int svt_ga<T>::getPopSize()
{
    return m_iPopSize;
};


/**
 * set the size of the best population
 * \param iBestPopSize number of individuals in the population
 */
template<class T>
void svt_ga<T>::setBestPopSize( int iBestPopSize )
{
    m_iBestPopSize = iBestPopSize;
};

/**
 * get size of the best population - population of best individuals
 */
template<class T>
int svt_ga<T>::getBestPopSize()
{
    return m_iBestPopSize;
};

    
/**
 * set maximum number of generations
 * \param iMaxGen maximum number of generations
 */
template<class T>
void svt_ga<T>::setMaxGen( int iMaxGen )
{
    m_iMaxGen = iMaxGen;
};
/**
 * get maximum number of generations
 * \return maximum number of generations
 */
template<class T>
int svt_ga<T>::getMaxGen( ) const
{
    return m_iMaxGen;
};


/**
 * set the number of generations before update
 * \param iSyncGen number of generations
 */
template<class T>
void svt_ga<T>::setSyncGen( int iSyncGen )
{
    m_iSyncGen = iSyncGen;
};

 /**
 * get the number of generations before update
 * \return iSyncGen number of generations
 */
template<class T>
int svt_ga<T>::getSyncGen( )
{
    return m_iSyncGen;
};


/**
 * get the current generation
 * \return m_iGenerations
 */
template<class T>
int svt_ga<T>::getCurrGen() const
{
    return m_iGenerations;
};
/**
 * set the current generation
 * \param iGenerations the new current generation index
 */
template<class T>
void svt_ga<T>::setCurrGen(int iGenerations)
{
    m_iGenerations = iGenerations;
};

/**
 * set the threshold score value after which the ga will stop, even if the maximum number of generations was not reached
 * \param fStopScore ga will stop once that score is exceeded
 */
template<class T>
void svt_ga<T>::setStopScore( Real64 fStopScore )
{
    m_fStopScore = fStopScore;
};
/**
 * get the threshold score value after which the ga will stop, even if the maximum number of generations was not reached
 * \return ga will stop once that score is exceeded
 */
template<class T>
Real64 svt_ga<T>::getStopScore( )
{
    return m_fStopScore;
};

/**
 * set run number
 * \param iRun run number
 */
template<class T>
void svt_ga<T>::setRun( int iRun )
{
    m_iRun = iRun;
};

/**
 * get the run number
 * \return run number
 */
template<class T>
int svt_ga<T>::getRun( ) const
{
    return m_iRun;
};

/**
 * set run number
 * \param iRun run number
 */
template<class T>
void svt_ga<T>::setParallelRun( int iParallelRun )
{
    m_iParallelRun = iParallelRun;
};

/**
 * get the run number
 * \return run number
 */
template<class T>
int svt_ga<T>::getParallelRun( ) const
{
    return m_iParallelRun;
};

/**
 * set thread number
 * \param iThread thread number
 */
template<class T>
void svt_ga<T>::setThread( int iThread )
{
    m_iThread = iThread;
};

/**
 * get the thread number
 * \return thread number
 */
template<class T>
int svt_ga<T>::getThread( ) const
{
    return m_iThread;
};

/**
 * set max thread number
 * \param iMaxThread thread number
 */
template<class T>
void svt_ga<T>::setMaxThread( int iMaxThread )
{
    m_iMaxThread = iMaxThread;
};

/**
 * get the max thread number
 * \return max thread number
 */
template<class T>
int svt_ga<T>::getMaxThread( ) const
{
    return m_iMaxThread;
};

/**
 * set the selective pressure
 * \param fSP selective pressure factor [1.0 ... 2.0] (default: 1.2)
 */
template<class T>
void svt_ga<T>::setSelectivePressure(Real64 fSP)
{
    m_fSP = fSP;
};
/**
 * get the selective pressure
 * \return selective pressure factor [1.0 ... 2.0] (default: 1.2)
 */
template<class T>
Real64 svt_ga<T>::getSelectivePressure() const
{
    return m_fSP;
};

/**
 * set the crossover probability
 * \param fCrossProb crossover probability [0.0 ... 1.0] (default: 0.95)
 */
template<class T>
void svt_ga<T>::setCrossoverProb(Real64 fCrossProb)
{
    m_fCrossProb = fCrossProb;
};
/**
 * get the crossover probability
 * \return crossover probability [0.0 ... 1.0] (default: 0.95)
 */
template<class T>
Real64 svt_ga<T>::getCrossoverProb() const
{
    return m_fCrossProb;
};

/**
 * set the mutation probability
 * \param fMutationProb mutation probability [0.0 ... 1.0] (default: 0.05)
 */
template<class T>
void svt_ga<T>::setMutationProb(Real64 fMutationProb)
{
    m_fMutationProb = fMutationProb;
    m_fFixedMutationProb = fMutationProb;
};
/**
 * get the mutation probability
 * \return mutation probability [0.0 ... 1.0] (default: 0.05)
 */
template<class T>
Real64 svt_ga<T>::getMutationProb()
{
    return m_fMutationProb;
};

/**
 * set the mutation offset
 * \param fMutationOffset
 */
template<class T>
void svt_ga<T>::setMutationOffset(Real64 fMutationOffset)
{
    m_fMutationOffset = fMutationOffset;
    m_fFixedMutationOffset = fMutationOffset;
};
/**
 * get the mutation offset
 * \return the mutation offset
 */
template<class T>
Real64 svt_ga<T>::getMutationOffset()
{
    return m_fMutationOffset;
};

/**
 * set the number of mutation that is applied on one gene durring local refinement
 */
template<class T>
void svt_ga<T>::setRefinementMaxMutPerGene(int iRefinementMaxMutPerGene)
{
    m_iRefinementMaxMutPerGene = iRefinementMaxMutPerGene;
};
/**
 * get the number of mutation that is applied on one gene durring local refinement
 */
template<class T>
int svt_ga<T>::getRefinementMaxMutPerGene()
{
    return m_iRefinementMaxMutPerGene;
};

/**
 * set the transposition probability
 * \param fTranspositionProb transposition probability [0.0 ... 1.0] (default: 0.05)
 */
template<class T>
void svt_ga<T>::setTranspositionProb(Real64 fTranspositionProb)
{
    m_fTranspositionProb = fTranspositionProb;
};
/**
 * get the transposition probability
 * \return transposition probability [0.0 ... 1.0] (default: 0.05)
 */
template<class T>
Real64 svt_ga<T>::getTranspositionProb()
{
    return m_fTranspositionProb;
};

/**
 * Set the cutoff distance parameter. All individuals with a distance lower than the one set here, will get penalized.
 * \param fCutoffDistance the cutoff distance
 */
template<class T>
void svt_ga<T>::setDistanceThreshold( Real64 fCutoffDistance )
{
    m_fCutoffDistance = fCutoffDistance;
};
/**
 * Get the cutoff distance parameter. All individuals with a distance lower than the one set here, will get penalized.
 * \return the cutoff distance
 */
template<class T>
Real64 svt_ga<T>::getDistanceThreshold( )
{
    return m_fCutoffDistance;
};
/**
 * Set the cutoff distance penalty parameter. All individuals with a distance lower than the one set here, will get multiplied with the factor set here.
 * \param fCutoffDistancePenalty the cutoff distance penalty
 */
template<class T>
void svt_ga<T>::setDistanceThresholdPenalty( Real64 fCutoffDistancePenalty )
{
    m_fCutoffDistancePenalty = fCutoffDistancePenalty;
};
/**
 * Get the cutoff distance penalty parameter. All individuals with a distance lower than the one set here, will get multiplied with the factor set here.
 * \return the cutoff distance penalty
 */
template<class T>
Real64 svt_ga<T>::getDistanceThresholdPenalty( )
{
    return m_fCutoffDistancePenalty;
};

/**
 * Shall the entire population be mutated? Normally, the GA mutates only few individuals of the new population, based on the mutationprobability. These individuals mostly are
 * new gene sets, created through crossover (crossover probability is typically very high).
 * If the parameter gets set to true, all old individuals get mutated, which can speed up the convergence of the algorithm.
 * \param bMutateAll if set to true all individuals get mutated.
 */
template<class T>
void svt_ga<T>::setMutateAll( bool bMutateAll )
{
    m_bMutateAll = bMutateAll;
};
/**
 * Shall the entire population be mutated? Normally, the GA mutates only few individuals of the new population, based on the mutationprobability. These individuals mostly are
 * new gene sets, created through crossover (crossover probability is typically very high).
 * If the parameter gets set to true, all old individuals get mutated, which can speed up the convergence of the algorithm.
 * \return if true all individuals get mutated.
 */
template<class T>
bool svt_ga<T>::getMutateAll()
{
    return m_bMutateAll;
};

/**
 * What proportion of the entire population should be mutated
 * \param bMutateAllProportion 1 means all individuals, 0 means none
 */
template<class T>
void svt_ga<T>::setMutateAllProportion( Real64 fMutateAllProportion )
{
    m_fMutateAllProportion = fMutateAllProportion;
};

/**
 * What proportion of the entire population should be mutated
 * \return bMutateAllProportion 1 means all individuals, 0 means none
 */
template<class T>
Real64 svt_ga<T>::getMutateAllProportion()
{
    return m_fMutateAllProportion;
};

/**
 * get the time to compute one generation (as computed during the last generation)
 */
template<class T>
Real64 svt_ga<T>::getTimeGen()
{
    return m_fTimeGen;
};

/**
 * set the variable m_bDone - (should the run of the GA stop cause it reached finish condition)
 * \param bDone - the state
 */
template<class T>
void svt_ga<T>::setDone( bool bDone)
{
    m_bDone = bDone;
    
    for( unsigned int iThread=0; iThread < m_oGA_Array.size(); iThread++ )
	m_oGA_Array[iThread]->setDone( bDone );
}

/**
 * get the variable m_bDone - (should the run of the GA stop cause it reached the finish condition?)
 * \return m_bDone - the state
 */
template<class T>
bool svt_ga<T>::getDone() const
{
    return m_bDone;
}

/**
 * set running state of the thread;
 */
template<class T>
void svt_ga<T>::setIsThreadRunning( bool bIsThreadRunning)
{
    m_bIsThreadRunning = bIsThreadRunning;
    
    for( unsigned int iThread=0; iThread < m_oGA_Array.size(); iThread++ )
	m_oGA_Array[iThread]->setIsThreadRunning( bIsThreadRunning );
};


/** 
 * \return whether the thread is running;
 */
template<class T>
bool svt_ga<T>::getIsThreadRunning()
{    
    return m_bIsThreadRunning;
};


///////////////////////////////////////////////////////////////////////////////
// Recombination
///////////////////////////////////////////////////////////////////////////////

/**
 * recombination
 */
template<class T>
void svt_ga<T>::recombination()
{
    if (m_bVerbose) 
	printf("%s function: recombination() - in\n", svt_getName( typeid(this).name() ));

    // select some individuals from the new population for recombination
    Real64 fRand;
    for(unsigned int i=0; i<m_oNextPop.size(); i++)
    {
	fRand = svt_genrand();

	if (fRand < m_fCrossProb)
	{
	    int iInd1, iInd2;
	    T oInd1, oInd2;

	    fRand = svt_genrand();
	    iInd1 = (int)(fRand * (Real64)(m_oNextPop.size()));
            
            do
	    {
		fRand = svt_genrand();
		iInd2 = (int)(fRand * (Real64)(m_oNextPop.size()));
	    } while (iInd1 == iInd2);
           
            // choose randomly which crossover operator we want to apply
	    fRand = svt_genrand();

            if (fRand < 0.16)
	        crossover1Point( m_oNextPop[iInd1], m_oNextPop[iInd2], &oInd1, &oInd2 );
            else if (fRand < 0.32)
	        crossover2Point( m_oNextPop[iInd1], m_oNextPop[iInd2], &oInd1, &oInd2 );
            else if (fRand < 0.48)
	        crossover2PointShuffle( m_oNextPop[iInd1], m_oNextPop[iInd2], &oInd1, &oInd2 );
            else if (fRand < 0.64)
	        crossoverArithmetic( m_oNextPop[iInd1], m_oNextPop[iInd2], &oInd1, &oInd2 );
            else if (fRand < 0.80)
	        crossoverIntermediate( m_oNextPop[iInd1], m_oNextPop[iInd2], &oInd1, &oInd2 );
            else if (fRand < 1.0)
                crossoverUniform( m_oNextPop[iInd1], m_oNextPop[iInd2], &oInd1, &oInd2 );

            m_oNextPop[iInd1] = oInd1;
            m_oNextPop[iInd2] = oInd2;
            
            m_oNextPop[iInd1].setOrigin(CROSSOVER);
            m_oNextPop[iInd1].resetAge();
	    makeValid( &m_oNextPop[iInd1] ); 
            m_oNextPop[iInd2].setOrigin(CROSSOVER);
            m_oNextPop[iInd2].resetAge();
            makeValid( &m_oNextPop[iInd2] ); 
    }


    }

    int iDel = 0;
    for(unsigned int i=0; i<m_oNextPop.size(); i++)
    {
	if ( m_oNextPop[i].getOrigin() == TABU ) 
	{
	    m_oNextPop.erase( m_oNextPop.begin() + i);
	    i--;
	    iDel ++;
	}
    }

    for(unsigned int i=0; i<m_oNextPop.size(); i++)
    {
	if ( m_oNextPop[i].getOrigin() == TABU ) 
	{
	    svtout << "Found tabu, but I shouldn't " << endl;
	}
    }
    

     
    if (m_bVerbose) 
	printf("%s function: recombination() - out\n", svt_getName( typeid(this).name() ));

}

/**
 * uniform (coin-flipping) crossover operator
 * \param rParentA reference to first parent object
 * \param rParentB reference to first parent object
 * \param pNewIndA pointer to new ind
 * \param pNewIndB pointer to second new ind
 */
template<class T>
 void svt_ga<T>::crossover(T& rParentA, T& rParentB, T* pNewIndA, T* pNewIndB )
{
    crossoverUniform(rParentA,rParentB, pNewIndA, pNewIndB);
};

/**
 * 2 point crossover operator
 * \param rParentA reference to first parent object
 * \param rParentB reference to first parent object
 * \param pNewIndA pointer to new ind
 * \param pNewIndB pointer to second new ind
 */
template<class T>
void svt_ga<T>::crossover2Point( T& rParentA, T& rParentB, T* pNewIndA, T* pNewIndB )
{
    svt_array_real64 oGenesA = rParentA.getGenes();
    svt_array_real64 oGenesB = rParentB.getGenes();
    svt_array_real64 oNewGenA;
    svt_array_real64 oNewGenB;
    int i, iPoint1, iPoint2;
    Real64 fRand;

    fRand = svt_genrand();
    iPoint1 = (int)(fRand * (Real64)(m_iGenes));
    do {
	fRand = svt_genrand();
	iPoint2 = (int)(fRand * (Real64)(m_iGenes));
    } while (iPoint1 == iPoint2);

    if (iPoint1 > iPoint2)
    {
	int iTemp = iPoint1;
	iPoint1 = iPoint2;
	iPoint2 = iTemp;
    }

    for(i=0; i<iPoint1; i++)
    {
	oNewGenA.push_back( oGenesA[i] );
	oNewGenB.push_back( oGenesB[i] );
    }

    for(i=iPoint1; i<iPoint2; i++)
    {
	oNewGenA.push_back( oGenesB[i] );
	oNewGenB.push_back( oGenesA[i] );
    }

    for(i=iPoint2; i<m_iGenes; i++)
    {
	oNewGenA.push_back( oGenesA[i] );
	oNewGenB.push_back( oGenesB[i] );
    }

    pNewIndA->setGenes( oNewGenA );
    pNewIndB->setGenes( oNewGenB );
};

/**
 * 2 point shuffle crossover operator
 * \param rParentA reference to first parent object
 * \param rParentB reference to first parent object
 * \param pNewIndA pointer to new ind
 * \param pNewIndB pointer to second new ind
 */
template<class T>
void svt_ga<T>::crossover2PointShuffle( T& rParentA, T& rParentB, T* pNewIndA, T* pNewIndB )
{
    svt_array_real64 oGenesA = rParentA.getGenes();
    svt_array_real64 oGenesB = rParentB.getGenes();
    svt_array_real64 oNewGenA;
    svt_array_real64 oNewGenB;
    Real64 fRand;

    // left/right shuffle of parent a
    fRand = svt_genrand();
    if (fRand < 0.5)
    {
	for (int i=1; i<m_iGenes; i++)
	    oNewGenA.push_back( rParentA.getGene(i) );
        oNewGenA.push_back( rParentA.getGene(0) );
    } else {
        oNewGenA.push_back( rParentA.getGene(m_iGenes-1) );
	for (int i=0; i<m_iGenes-1; i++)
	    oNewGenA.push_back( rParentA.getGene(i) );
    }

    // left/right shuffle of parent b
    fRand = svt_genrand();
    if (fRand < 0.5)
    {
	for (int i=1; i<m_iGenes; i++)
	    oNewGenB.push_back( rParentB.getGene(i) );
        oNewGenB.push_back( rParentB.getGene(0) );
    } else {
        oNewGenB.push_back( rParentB.getGene(m_iGenes-1) );
	for (int i=0; i<m_iGenes-1; i++)
	    oNewGenB.push_back( rParentB.getGene(i) );
    }

    // now let us do the crossover
    T oIndA; oIndA.setGenes( oNewGenA );
    T oIndB; oIndB.setGenes( oNewGenB );
    crossover2Point( oIndA, oIndB, pNewIndA, pNewIndB );
};

/**
 * 1 point crossover operator
 * \param rParentA reference to first parent object
 * \param rParentB reference to first parent object
 * \param pNewIndA pointer to new ind
 * \param pNewIndB pointer to second new ind
 */
template<class T>
void svt_ga<T>::crossover1Point( T& rParentA, T& rParentB, T* pNewIndA, T* pNewIndB )
{
    svt_array_real64 oGenesA = rParentA.getGenes();
    svt_array_real64 oGenesB = rParentB.getGenes();
    svt_array_real64 oNewGenA;
    svt_array_real64 oNewGenB;

    Real64 fRand = svt_genrand();
    int iPoint1 = (int)(fRand * (Real64)(m_iGenes));

    if (svt_genrand() > 0.5)
    {
	for(int i=0; i<iPoint1; i++)
	{
	    oNewGenA.push_back( oGenesA[i] );
	    oNewGenB.push_back( oGenesB[i] );
	}

	for(int i=iPoint1; i<m_iGenes; i++)
	{
	    oNewGenA.push_back( oGenesB[i] );
	    oNewGenB.push_back( oGenesA[i] );
	}
    } else {
	for(int i=0; i<iPoint1; i++)
	{
	    oNewGenA.push_back( oGenesB[i] );
	    oNewGenB.push_back( oGenesA[i] );
	}

	for(int i=iPoint1; i<m_iGenes; i++)
	{
	    oNewGenA.push_back( oGenesA[i] );
	    oNewGenB.push_back( oGenesB[i] );
	}
    }

    pNewIndA->setGenes( oNewGenA );
    pNewIndB->setGenes( oNewGenB );
};

/**
 * uniform (coin-flipping) crossover operator
 * \param rParentA reference to first parent object
 * \param rParentB reference to first parent object
 * \param pNewIndA pointer to new ind
 * \param pNewIndB pointer to second new ind
 */
template<class T>
void svt_ga<T>::crossoverUniform(T& rParentA, T& rParentB, T* pNewIndA, T* pNewIndB )
{
    for(int j=0;j<m_iGenes;j++)
    {
	Real64 fRand = svt_genrand();

	if (fRand < 0.5)
	{
	    pNewIndA->addGene( rParentA.getGene(j) );
	    pNewIndB->addGene( rParentB.getGene(j) );
	} else {
	    pNewIndA->addGene( rParentB.getGene(j) );
	    pNewIndB->addGene( rParentA.getGene(j) );
	}
    }
}

/**
 * intermediate crossover operator
 * \param rParentA reference to first parent object
 * \param rParentB reference to first parent object
 * \param pNewIndA pointer to new ind
 * \param pNewIndB pointer to second new ind
 */
template<class T>
void svt_ga<T>::crossoverIntermediate(T& rParentA, T& rParentB, T* pNewIndA, T* pNewIndB )
{
    for(int j=0;j<m_iGenes;j++)
    {
	Real64 fRand = svt_genrand();
	fRand *= 1.5;
        fRand -= 0.25;

	pNewIndA->addGene( rParentA.getGene(j) + (fRand*(rParentB.getGene(j) - rParentA.getGene(j))) );
	pNewIndB->addGene( rParentB.getGene(j) + (fRand*(rParentA.getGene(j) - rParentB.getGene(j))) );
    }
}

/**
 * arithmethic crossover operator - xoff = (alpha)*x1+(1-alpha)*x2 - alpha is always random
 * \param rParentA reference to first parent object
 * \param rParentB reference to first parent object
 * \param pNewIndA pointer to new ind
 * \param pNewIndB pointer to second new ind
 */
template<class T>
void svt_ga<T>::crossoverArithmetic(T& rParentA, T& rParentB, T* pNewIndA, T* pNewIndB )
{
    for(int j=0;j<m_iGenes;j++)
    {
	Real64 fRand = svt_genrand();

        pNewIndA->addGene( fRand*rParentA.getGene(j)+ (1.0f-fRand)*rParentB.getGene(j) );
        pNewIndB->addGene( (1-fRand)*rParentA.getGene(j)+ (fRand)*rParentB.getGene(j) );

    }
};

/**
 * subgroup crossover operator
 * \param rParentA reference to first parent object
 * \param rParentB reference to first parent object
 * \param pNewInd pointer to new ind
 */
template<class T>
void svt_ga<T>::crossoverSubgroup( T& rParentA, T& rParentB, T* pNewInd )
{
    svt_array_real64 oGenesA = rParentA.getGenes();
    svt_array_real64 oGenesB = rParentB.getGenes();
    svt_array_real64 oNewGen;
    int i, iPoint1, iPoint2, iPoint3;
    Real64 fRand;

    // first step: cut out a part from ind A
    fRand = svt_genrand();
    iPoint1 = (int)(fRand * (Real64)(m_iGenes));
    do {
	fRand = svt_genrand();
	iPoint2 = (int)(fRand * (Real64)(m_iGenes));
    } while (iPoint1 == iPoint2);

    if (iPoint1 > iPoint2)
    {
	int iTemp = iPoint1;
	iPoint1 = iPoint2;
	iPoint2 = iTemp;
    }

    for(i=iPoint1; i<iPoint2; i++)
	oNewGen.push_back( oGenesA[i] );

    // now cut out a part from ind B
    do {
	fRand = svt_genrand();
	iPoint3 = (int)(fRand * (Real64)(m_iGenes));
    } while (iPoint3 > iPoint2 - iPoint1);

    int iEnd = iPoint3 + (m_iGenes - (iPoint2 - iPoint1));

    //cout << "iPoint1: " << iPoint1 << " iPoint2: " << iPoint2 << " iPoint3: " << iPoint3 << " iEnd: " << iEnd << endl;
    for(i=iPoint3; i<iEnd; i++)
	oNewGen.push_back( oGenesB[i] );

    pNewInd->setGenes( oNewGen );
};

///////////////////////////////////////////////////////////////////////////////
// Mutation
///////////////////////////////////////////////////////////////////////////////

/**
 * mutation
 */
template<class T>
void svt_ga<T>::mutation()
{
    if (m_bVerbose) 
	printf("%s function: mutation() - in \n", svt_getName( typeid(this).name() ));

    Real64 fRand;
    for(unsigned int i=0; i<m_oNextPop.size(); i++)
    {
	fRand = svt_genrand();
	if (fRand < m_fMutationProb || m_oNextPop[i].getOrigin() != CROSSOVER) // mutate if individual was not modified through cross over
	{
	    mutationCustom( i );
	    m_oNextPop[i].setOrigin(MUTATION);
	    m_oNextPop[i].resetAge();
	    makeValid(&m_oNextPop[i]);
	}
    }
    
    if (m_bVerbose) 
	printf("%s function: mutation() - out \n", svt_getName( typeid(this).name() ));

}

/**
 * uniform mutation
 * \param iInd index of individual
 */
template<class T>
void svt_ga<T>::mutationRandom(int iInd)
{
    // select gene for mutation svt_genrandomly
    Real64 fRand = svt_genrand();
    int iRandIndex = (int)(fRand * (Real64)(m_iGenes));

    m_oNextPop[iInd].setGene( iRandIndex, svt_genrand() );
}

/**
 * uniform mutation
 * \param iInd index of individual
 */
template<class T>
void svt_ga<T>::mutationUniform(int iInd)
{
    // select gene for mutation svt_genrandomly
    Real64 fRand = svt_genrand();
    int iRandIndex = (int)(fRand * (Real64)(m_iGenes));

    // generate offset
    fRand = svt_genrand() * 0.05;

    if (svt_genrand() > 0.5)
        fRand *= -1.0;
    
    m_oNextPop[iInd].setGene( iRandIndex, m_oNextPop[iInd].getGene( iRandIndex) + fRand );
}

/**
 * mutation with a normal distribution
 * \param iInd index of individual
 */
template<class T>
void svt_ga<T>::mutationNorm(int iInd)
{
   // select gene for mutation svt_genrandomly
    Real64 fRand = svt_genrand();
    int iRandIndex = (int)(fRand * (Real64)(m_iGenes));

    // generate offset
    fRand = svt_ranNormal(0.0, m_fMutationOffset);

    m_oNextPop[iInd].setGene( iRandIndex, m_oNextPop[iInd].getGene( iRandIndex) + fRand );
};

/**
 * mutation with a cauchy distribution
 * \param iInd index of individual
 */
template<class T>
void svt_ga<T>::mutationCauchy(int iInd)
{
    // select gene for mutation svt_genrandomly
    Real64 fRand = svt_genrand();
    int iRandIndex = (int)(fRand * (Real64)(m_iGenes));
    Real64 fIntPart;

    // generate offset
    fRand = svt_ranCauchy(0.0, m_fMutationOffset);
    fRand = modf(fRand, &fIntPart); // keep only the fractional part of the Random number
    
    m_oNextPop[iInd].setGene( iRandIndex, m_oNextPop[iInd].getGene( iRandIndex) + fRand );
};

/**
 * mutation with a cauchy distribution
 * \param oInd the individual
 */
template<class T>
void svt_ga<T>::mutationCauchy(T * oInd)
{
    // select gene for mutation svt_genrandomly
    Real64 fRand = svt_genrand();
    int iRandIndex = (int)(fRand * (Real64)(m_iGenes));
    Real64 fIntPart;

    // generate offset
    fRand = svt_ranCauchy(0.0, m_fMutationOffset);
    fRand = modf(fRand, &fIntPart); // keep only the fractional part of the Random number
    
    oInd->setGene( iRandIndex, oInd->getGene(iRandIndex) + fRand );
};

/**
 * mutation with a cauchy distribution - 3 mutation per individual 
 * \param iInd index of individual
 */
template<class T>
void svt_ga<T>::mutationMultiCauchy(int iInd)
{
    Real64 fRand;
    Real64 fMutRatio = 4.0f; // 1 in iMutRatio will be mutated
    unsigned int iGenes = m_oNextPop[iInd].getGeneCount( );
    Real64 fMutCount = iGenes/fMutRatio; // how many mutations per individual
    
    Real64 fIntPart;
    
    for (unsigned int iIndex=0; iIndex<iGenes; iIndex++)
    {
        fRand = svt_genrand();
        
        if (fRand<1.0/fMutRatio)// do mutate - mutate 1 genes in 7
        {
            fRand = svt_ranCauchy(0.0, m_fMutationOffset/fMutCount);
            fRand = modf(fRand, &fIntPart); // keep only the fractional part of the Random number

            m_oNextPop[iInd].setGene( iIndex, m_oNextPop[iInd].getGene( iIndex) + fRand );
        }
    }
    
};

/**
 * uniform mutation for 1 in 7 genes
 * \param iInd index of individual
 */
template<class T>
void svt_ga<T>::mutationMultipoint(int iInd)
{
    Real64 fRand;
    Real64 fMutRatio = 4.0f; // 1 in iMutRatio will be mutated
    unsigned int iGenes = m_oNextPop[iInd].getGeneCount( );
    Real64 fMutCount = iGenes/fMutRatio; // how many mutations per individual
    
    for (unsigned int iIndex=0; iIndex<iGenes; iIndex++)
    {
        fRand = svt_genrand();
        
        if (fRand<1.0/fMutRatio)// do mutate - mutate 1 genes in 7
        {
            fRand = svt_genrand() *  m_fMutationOffset/fMutCount;
            m_oNextPop[iInd].setGene( iIndex, m_oNextPop[iInd].getGene( iIndex) + fRand );
        }
    }
    
};

/**
 * random mutation for 1 in 7 genes
 * \param iInd index of individual
 */
template<class T>
void svt_ga<T>::mutationMultiRandom(int iInd)
{
    Real64 fRand;
    Real64 fMutRatio = 4.0f; // 1 in iMutRatio will be mutated
    unsigned int iGenes = m_oNextPop[iInd].getGeneCount( );
    
    for (unsigned int iIndex=0; iIndex<iGenes; iIndex++)
    {
        fRand = svt_genrand();
        
        if (fRand<1.0/fMutRatio)// do mutate - mutate 1 genes in 7
        {
            fRand = svt_genrand();
            m_oNextPop[iInd].setGene( iIndex, fRand );
        }
    }
    
};

/**
 * moving window mutation
 * \param iInd index of individual
 */
template<class T>
void svt_ga<T>::mutationMovingWindow(int iInd)
{
    // select gene for mutation svt_genrandomly
    Real64 fRand = svt_genrand();
    int iRandIndex = (int)(fRand * (Real64)(m_iGenes));

    // generate offset
    fRand = svt_genrand();
    fRand /= 2.0;
    fRand += (1.0f - (m_iGenerations / m_iMaxGen)) * 0.5;

    if (svt_genrand() > 0.5)
        fRand *= -1.0;

    m_oNextPop[iInd].setGene( iRandIndex, m_oNextPop[iInd].getGene( iRandIndex) + fRand );
}

/**
 * custom mutation (can be changed by derived class, default implementation just calls mutationUniform)
 * \param iInd index of individual
 */
template<class T>
void svt_ga<T>::mutationCustom(int iInd)
{
    mutationCauchy(iInd);
}

/**
 * Mutate the entire population (by doubling its size).
 */
template<class T>
void svt_ga<T>::mutationAllPop()
{
    if (m_bVerbose) 
	printf("%s function: mutationAllPop() - in\n", svt_getName( typeid(this).name() ));

    sortPopulation();
    
    unsigned int iSize = m_oPop.size();
    Real64 fDist;
    //svtout << " MutateAll between" << iSize -1 << " : " << iSize-1-iSize*m_fMutateAllProportion+1 << endl;
    for(int i=iSize-1; i>iSize-1-iSize*m_fMutateAllProportion+1 && i >= 0; i--)
    {
        m_oNextPop.push_back(m_oPop[i]);
        mutationCustom( m_oNextPop.size()-1 );
	m_oNextPop[ m_oNextPop.size()-1].setOrigin(MUTATIONALL);
        m_oNextPop[ m_oNextPop.size()-1].resetAge();
	
	//check if the new individuals is "identical" with its parent  and discard then
	fDist = ((svt_ga_ind)m_oPop[i]).distance( (svt_ga_ind&)(m_oNextPop[ m_oNextPop.size()-1 ]) );
	if (fDist < EPS)
	    m_oNextPop.pop_back();
    }
    
    if (m_bVerbose) 
	printf("%s function: mutationAllPop() - out\n", svt_getName( typeid(this).name() ));

}

///////////////////////////////////////////////////////////////////////////////
// Add random individuals
///////////////////////////////////////////////////////////////////////////////

/**
 * adds new random individuals in the population
 */
template<class T>
void svt_ga<T>::addRandomIndividuals()
{
    unsigned int iPopSize =  m_oNextPop.size();
    unsigned int iCount = 0;
    for (unsigned int iIndex = 0; iIndex < iPopSize; iIndex++)
    {
        if (svt_genrand() < m_fMutationProb )
        {
            T oInd = initIndividual();
            updateFitness(&oInd);
            
            m_oNextPop.push_back(oInd);
            m_oNextPop[ m_oNextPop.size()-1].setOrigin(MUTATION);
            m_oNextPop[ m_oNextPop.size()-1].resetAge();
            
            iCount++;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Transposition
///////////////////////////////////////////////////////////////////////////////

/**
 * transposition
 */
template<class T>
void svt_ga<T>::transposition()
{
    if (m_bVerbose) 
	printf("%s function: transposition() - in\n", svt_getName( typeid(this).name() ));

    Real64 fRand;
    for(unsigned int i=0; i<m_oNextPop.size(); i++)
    {
	fRand = svt_genrand();

	if (fRand < m_fTranspositionProb)
	{
	    T oInd;
	    transpositionUniform( m_oNextPop[i], &oInd );
            
            oInd.setOrigin(TRANSPOSITION);
            oInd.resetAge();
            
            m_oNextPop.push_back(oInd);
	}
    }

    if (m_bVerbose) 
	printf("%s function: transposition() - out\n", svt_getName( typeid(this).name() ));
}

/**
 * flips two genes between the same individual
 * \param rParentA reference to object
 * \param pNewIndA pointer to new ind
 */
template<class T>
void svt_ga<T>::transpositionUniform(T& rParentA, T* pNewIndA )
{
    if (m_bVerbose) 
	printf("%s function: transpositionUniform() - in\n", svt_getName( typeid(this).name() ));
    
    Real64 fRand;
    int iPoint1, iPoint2;
    
    fRand = svt_genrand();
    iPoint1 = (int)(fRand * (Real64)(m_iGenes));
    
    do 
    {
	fRand = svt_genrand();
        iPoint2 = (int)(fRand * (Real64)(m_iGenes));
    } while (iPoint1 == iPoint2);

    (*pNewIndA) = rParentA;

    pNewIndA->setGene( iPoint1, rParentA.getGene(iPoint2) );
    pNewIndA->setGene( iPoint2, rParentA.getGene(iPoint1) );
    
    if (m_bVerbose) 
	printf("%s function: transpositionUniform() - out\n", svt_getName( typeid(this).name() ));

};

///////////////////////////////////////////////////////////////////////////////
// Reinsertion
///////////////////////////////////////////////////////////////////////////////

/**
 * set Reinsertion scheme
 * \param iReinsertionScheme the reinsertion scheme to used
 */
template<class T>
void svt_ga<T>::setReinsertionScheme( unsigned int iReinsertionScheme )
{
    m_iReinsertionScheme = iReinsertionScheme;
};

/**
 * reinsertion
 */
template<class T>
void svt_ga<T>::reinsertion()
{
    if (m_bVerbose) 
	printf("%s function: reinsertion() - in\n", svt_getName( typeid(this).name() ));

    for(unsigned int i=0; i<m_oNextPop.size(); i++)
        updateFitness( &m_oNextPop[i] );

    // which method should we use
    switch(m_iReinsertionScheme)
    {
    default:
    case REINSERTION_ELITIST:
	reinsertion_elitist();
        break;
    case REINSERTION_ELITIST_UNIQUE:
	reinsertion_elitist_unique();
        break;
    case REINSERTION_GLOBALRANKING:
	reinsertion_globalranking();
        break;
    case REINSERTION_GLOBALRANKING_UNIQUE:
	reinsertion_globalranking_unique();
        break;
    case REINSERTION_SHARING:
	reinsertion_sharing();
        break;
    }
    
    if (m_bVerbose) 
	printf("%s function: reinsertion() - out\n", svt_getName( typeid(this).name() ));

}

/**
 * elitist reinsertion - replace the 50% worst parents with 50% best new individuals
 */
template<class T>
void svt_ga<T>::reinsertion_elitist()
{
    sortPopulation();
    sortNextPopulation();
    
    // temp population
    svt_population<T> oTempPop;

    // copy the best of the old population...
    for(unsigned int i=0; i<m_oPop.size(); i++)
	oTempPop.push_back( m_oPop[ m_oPop.size() - i - 1] );
    // and the best of the new population
    for(unsigned int i=0; i<m_oNextPop.size(); i++)
        oTempPop.push_back( m_oNextPop[ m_oNextPop.size() - i - 1] );

    sort( oTempPop.begin(), oTempPop.end() );

    // do we actually have enough?
    while((int)oTempPop.size() < m_iPopSize) oTempPop.push_back( m_oPop[ m_oPop.size() - 1] );

    // copy everything over
    m_oPop.clear();
    for(int i=0; i<m_iPopSize; i++)
        m_oPop.push_back( oTempPop[ oTempPop.size() - i - 1] );
}

/**
 * elitist reinsertion. Only the best individuals from both, the old and the new generations, are copied over to the next population. Some enhancements: Distance penalty and insertion of a few, fully random individuals.
 */
template<class T>
void svt_ga<T>::reinsertion_elitist_unique()
{
    if (m_bVerbose) 
	printf("%s function: reinsertion_elitist_unique() - in\n", svt_getName( typeid(this).name() ));
   
    sortPopulation();
   
    // create a couple of best individual mutations
    T oInd;
    T oIndAdd = m_oPop[ m_oPop.size() - 1];
    bool bAdd = false;
    Real64 fRand, fIntPart;
    for(unsigned int i=0; i<(unsigned int)m_iGenes; i++)
    {
        for(int j=0; j<m_iRefinementMaxMutPerGene; j++)
        {
            oInd = m_oPop[ m_oPop.size() - 1];

            fRand = svt_ranCauchy(0.0, m_fMutationOffset);
            fRand = modf(fRand, &fIntPart);

            oInd.setGene( i, oInd.getGene( i ) + fRand );
	    makeValid( &oInd );
	    updateFitness( &oInd );
	    if (oInd.getFitness() > oIndAdd.getFitness())
            {
		oIndAdd = oInd;
                bAdd = true;
            } 
        }
    }

    if (bAdd)
    {
	//oIndAdd.resetAge();
	oIndAdd.setOrigin(GENEREFINEMENT);
        m_oNextPop.push_back( oIndAdd );
    }
    sortNextPopulation();
    
    // temp population
    m_oTempPop.clear();
    // copy the best of the old population...
    m_oTempPop.insert( m_oTempPop.begin(), m_oPop.end() - m_iPopSize, m_oPop.end() );
    // ...and the next population
    m_oTempPop.insert( m_oTempPop.begin(), m_oNextPop.end() - m_oNextPop.size(), m_oNextPop.end() );
    // do we actually have enough?
    while((int)m_oTempPop.size() < m_iPopSize) m_oTempPop.push_back( m_oPop[ m_oPop.size() - 1] );
    // now sort everything
    sort( m_oTempPop.begin(), m_oTempPop.end() );
   
    //
    //Check which individuals are in tabu regions
    //
    for(unsigned int i=0; i<m_oTempPop.size(); i++)
    {
        if (isInTabuReg( &m_oTempPop[i] ))
            m_oTempPop[i].setFitness( 0.0 );
    }
    sort( m_oTempPop.begin(), m_oTempPop.end() );
    penalizeSimilar(m_oTempPop,m_fCutoffDistance,m_fCutoffDistancePenalty );
    sort( m_oTempPop.begin(), m_oTempPop.end() );

    //
    // add tabu region
    //
    m_oTabuWindow.push_back( m_oTempPop[m_oTempPop.size()-1].distance( m_oPop[m_oPop.size()-1] ) );
    
    if (m_oTabuWindow.size() > m_iTabuWindowSize)
        m_oTabuWindow.erase( m_oTabuWindow.begin() );

    Real64 fAvg = 0.0;
    for(unsigned int i=0; i<m_oTabuWindow.size(); i++)
        fAvg += m_oTabuWindow[i];
    fAvg = fAvg / m_oTabuWindow.size();

    int i = m_oTempPop.size()-1;
    if (( m_oTabuWindow.size() >= m_iTabuWindowSize-2 && fAvg < m_fTabuThreshold) )
    {
        bool bFound = false;
        for(unsigned int j=0; j<m_oTabus.size(); j++)
            if (m_oTempPop[i].distance( m_oTabus[j] ) < m_fTabuRegionSize)
                bFound = true;

        //add to tabu and refine 
	if (!bFound)
            refineInd( &m_oTempPop[i] );
    }

    //
    // Sort the newly created population and copy it over into the main array...
    //
    sort( m_oTempPop.begin(), m_oTempPop.end() );
    m_oPop.clear();
    m_oPop.insert( m_oPop.begin(), m_oTempPop.end() - m_iPopSize, m_oTempPop.end() );
    
    if (m_bVerbose) 
	printf("%s function: reinsertion_elitist_unique() - out\n", svt_getName( typeid(this).name() ));
}

/**
 * share fitness among multiple nishes 
 *\FIXME add more here
 */
template<class T>
void svt_ga<T>::reinsertion_sharing()
{
    vector< T > oTempPop;
    
    // copy the best of the old population...
    oTempPop.insert( oTempPop.begin(), m_oPop.end() - m_iPopSize, m_oPop.end() );
    // ...and the next population
    oTempPop.insert( oTempPop.begin(), m_oNextPop.end() - m_iPopSize, m_oNextPop.end() );
   
   // do we actually have enough?
    while((int)oTempPop.size() < m_iPopSize) oTempPop.push_back( m_oPop[ m_oPop.size() - 1] );
    
    sort( oTempPop.begin(), oTempPop.end() );
    
    shareFitness( oTempPop );
    
    sort( oTempPop.begin(), oTempPop.end() );
    
    
    // add a certain amount of random individuals
    Real64 fOrigPop = 0.9;
    Real64 fRandPop = 0.1;
    m_oPop.clear();
    for(int i=0; i<m_iPopSize * fOrigPop; i++)
        m_oPop.push_back( oTempPop[ oTempPop.size() - i - 1] );

    for(int i=0; i<m_iPopSize * fRandPop; i++)
    {
        T oInd = initIndividual( );
        updateFitness( &oInd );
        m_oPop.push_back( oInd );
    }
   
    sortPopulation();
}

/**
 * share the fitness between the individuals of the population: a %percent of individuals are allowed in one niche - the rest are just killed and need to populate other niches
 * \param oPop - the individuals come form these population
 */
template<class T>
void svt_ga<T>::shareFitness(vector< T>& oPop)
{
    vector<int> oNiche;
    vector<int> oRank;
    vector<int> oCountsPerNiche;
    int iPopSize = oPop.size();
    Real64 fDist;
    svt_matrix<Real64> oDistMat(iPopSize, iPopSize);
    
    for(int i=(int)oPop.size()-1; i>0; i--)
    {
        for(int j=i-1; j>0; j--)
	{
	    fDist = oPop[i].distance( oPop[j] );
	    
	    oDistMat[i][j] = fDist;
	    oDistMat[j][i] = fDist;
	    
            if ( fDist < EPS)
                oPop[j].setFitness( 0.0 );
	}
    }
        
    // set Niche to all individuals to 0
    for(int i=(int)iPopSize-1; i>0; i--)
    {
        oNiche.push_back(0);
        oRank.push_back(0); 
    }
    
    // the best individual is in niche 1
    oNiche[ iPopSize - 1 ] = 1;
    oRank [ iPopSize - 1 ] = 0;
    
    //current niche
    int iNiche = 1;
    unsigned int iCount = 1;
    int i=(int)iPopSize-1;
    
    // compute distances - get niches
    while (i>=0)
    {
	for(int j=i-1; j>0; j--)
	{
	    if (oNiche[j]==0) // j does not have a niche
	    {
		// get distance to the top of the niche
		fDist = oDistMat[i][j];
		
		if (fDist < m_fNicheSize && oPop[j].getFitness() > 0 )
		{
		    oNiche[ j ] = iNiche;
		    oRank[ j ] = iCount; // the rank 
		    iCount++;
		}
	    }
	}
	
	//done counting how many are in this niche - so add them
	oCountsPerNiche.push_back(iCount);
	
        i--;
	
       // don't make niche heads if individuals are already placed in a niche or its fitness is 0
       while (i >= 0 && (oNiche[i]!=0 || oPop[i].getFitness()==0) )
           i--;
	
        // next niche
       if (i>=0)
       {
        iNiche++;
        oNiche[i] = iNiche;
        oRank[i] = 0;
        iCount = 1;
       }
    }
    
    for (i = 0; i < (int)oCountsPerNiche.size(); i++)
	oCountsPerNiche[ i ] = 0;

    //here share fitness 
    for (i = iPopSize-1; i>=0; i--)
    {
	// if ind is not identical with other
	if (oPop[i].getFitness() > 0 )
	{
	    oCountsPerNiche[ oNiche[i] ]++ ;
	    
	    //decrease fitness inside niche and discard individuals if to many in the niche
	    //svtout << i <<  " " << oNiche[i] << " " << oCountsPerNiche[ oNiche[i] ] <<  " " <<  m_iPopSize * m_fMaxPopPerNiche << " " << oPop[i].getFitness() << " ";
	    if (oCountsPerNiche[ oNiche[i] ] < m_iPopSize * m_fMaxPopPerNiche)
	    {
		oPop[i].setFitness( oPop[i].getFitness()*pow(m_fSameNichePenalty,oRank[i]));
		oPop[i].setNiche( oNiche[i] );
	    }
	    else
	    {
		oPop[i].setFitness( 0 );
		oPop[i].setNiche( 0 );
	    }
	    //cout << oPop[i].getFitness() << " " << oRank[i] << " " <<  m_fSameNichePenalty << endl;
	}else
	{
	    oPop[i].setNiche( 0 );
	}
    }
}


/**
 * reinsertion - global reinsertion based on the global ranking.
 * Both, the old and the new population are ranked together and only the best individuals are inserted into the next gen.
 */
template<class T>
void svt_ga<T>::reinsertion_globalranking()
{
    for(unsigned int i=0; i<m_oNextPop.size(); i++)
        m_oPop.push_back( m_oNextPop[i] );

    sortPopulation();

    // erase the worst individuals
    m_oNextPop.clear();
    for(int i=0; i<m_iPopSize; i++)
    {
        m_oNextPop.push_back( m_oPop[ m_oPop.size() - i - 1] );
    }

    m_oPop = m_oNextPop;
}

/**
 * reinsertion - global reinsertion based on the global ranking but only unique individuals are allowed
 */
template<class T>
void svt_ga<T>::reinsertion_globalranking_unique()
{
    for(unsigned int i=0; i<m_oNextPop.size(); i++)
        m_oPop.push_back( m_oNextPop[i] );

    sortPopulation();

    // remove identical individuals
    m_oNextPop.clear();
    svt_array_real64 oUsed;
    Real64 fItem;
    for(int i=m_oPop.size()-1; (i>=0 && (int)m_oNextPop.size() < m_iPopSize); i--)
    {
	fItem = m_oPop[i].getValue();

	if (oUsed.size() == 0 || find(oUsed.begin(), oUsed.end(), fItem) == oUsed.end())
	{
	    m_oNextPop.push_back(m_oPop[i]);
	    oUsed.push_back(fItem);
	}
    }

    m_oPop = m_oNextPop;
}

///////////////////////////////////////////////////////////////////////////////
// Fitness
///////////////////////////////////////////////////////////////////////////////

/**
 * update fitness
 */
template<class T>
void svt_ga<T>::updateFitness()
{
    if (m_bVerbose) 
	printf("%s function: updateFitness() - out\n", svt_getName( typeid(this).name() ));

    for(unsigned int i=0; i<m_oPop.size(); i++)
    {
        m_oPop[i].incAge();
        updateFitness( &m_oPop[i] );
	//cout << "[" << i << " - " << m_iGenerations << " - " << m_iThread <<  "] " ;
	//m_oPop[i].printGenes();
    }

    if (m_bVerbose) 
	printf("%s function: updateFitness() - out\n", svt_getName( typeid(this).name() ));

}

/**
 * check if the ind fullfills the validity requirements 
 */
template<class T>
bool svt_ga<T>::isValid(T* pInd)
{
    for(int j=0;j<m_iGenes;j++)
        if (pInd->getGene(j) > 1.0 || pInd->getGene(j) < 0.0)
            return false;

    return true;
};

/**
 * Function to check whether the integrity of the genes is maintained
 * \param pInd the individual for which to check genes
 * \return the if individual correct
 */
template<class T>
void svt_ga<T>::makeValid(T* pInd)
{
//     if (isValid(pInd))
//         return;

    svt_vector4<Real64> oVec;
    Real64 fNewGene;

    for(int j=0;j<m_iGenes;j++)
    {
        fNewGene = pInd->getGene(j);

        if (fabs(fNewGene)>1.0f)
            fNewGene -= floor(fNewGene);

        if (fNewGene<0.0f)
            fNewGene=fabs(fNewGene);

        if (fabs(fNewGene)>=1.0f)
            svtout << "Error: Gene Value: " << j%4 << ": " << fNewGene << " Origin: " << pInd->getOrigin() << endl;

        pInd->setGene( j, fNewGene ); 
    }
}

/**
 * Penalize individuals that are similar to allow a more diverse population
 * \param the population 
 * \param fCutoffDistance the gene distance between which they get penalized 
 * \param fCufoffDistancePenalty how much do they get penalized
 */
template<class T>
void svt_ga<T>::penalizeSimilar(svt_population<T> &oPop, Real64 fCutoffDistance, Real64 fCutoffDistancePenalty)
{
    Real64 fDist = 0.0;
    unsigned int iReduced = 0;
    unsigned int iZero = 0;
    int iSize = oPop.size();
    
    iReduced = 0;
    for(int i=iSize-1; i>0; i--)
    {
        for(int j=i-1; j>=0; j--)
        {
            fDist = oPop[i].distance( oPop[j] );
	   
            // distance small?
            if (fDist < fCutoffDistance)
            {
		
                iReduced++;
                if (iReduced < iSize * 0.7)
                    oPop[j].setFitness( oPop[j].getFitness() * fCutoffDistancePenalty );
                else
                    oPop[j].setFitness( 0.0 );
            }

            // distance zero?
            if (fDist < EPS)
            {
                oPop[j].setFitness( 0.0 );
                iZero++;
            }
        }

    }
    //svtout << "iReduced: "<< iReduced << " iZero:" << iZero << endl;
}

/**
 * Discard invalid(fitness value=0) individuals
 * \param oPop the population
 */
template<class T>
void svt_ga<T>::discardNullInd(svt_population<T> &oPop)
{
    //sort such the 0 are at the end of the list
    sort(oPop.rbegin(), oPop.rend());
 
    //discard 0
    int iIndex = oPop.size()-1;
    while (iIndex >= 0 && oPop[iIndex].getFitness()==0.0)
    {
	oPop.pop_back();
	iIndex = oPop.size()-1;
    }   
} 

///////////////////////////////////////////////////////////////////////////////
// Print/output diagnostic information
///////////////////////////////////////////////////////////////////////////////

/**
 * print results (to cout)
 */
template<class T>
void svt_ga<T>::printResults()
{
    char pOut[1024];
    for(unsigned int i=0; i<m_oPop.size(); i++)
    {
        sprintf(pOut, "[%2i] = ", i);
        svtout << pOut ;
	m_oPop[i].printGenes();
    }
}

/**
 * print population 
 * \param the population 
 */
template<class T>
void svt_ga<T>::printPop(svt_population<T>& oPop)
{
    for(unsigned int i=0; i<oPop.size(); i++)
    {
        printf("[%2i] = ", i);
	oPop[i].printGenesPf();
    }
}

/**
 * print results (to cout)
 */
template<class T>
void svt_ga<T>::printNextPop()
{
    char pOut[1024];
    unsigned int iSize = m_oNextPop.size();
    for(int i=iSize-1; i>=0; i--)
    {
        sprintf(pOut, "[%3i] = %1d %1d %8.3f", i, m_oNextPop[i].getOrigin(), m_oNextPop[i].getAge(), m_oNextPop[i].getProp() );
        svtout << pOut ;
	m_oNextPop[i].printGenes();
    }
}

/**
 * print the fitness of each individual of the population 
 */
template<class T>
void svt_ga<T>::printPopFitness(char* pFname)
{
    
    FILE* file;
    if (m_iGenerations==0)
        file = fopen(pFname, "w");
    else
        file = fopen(pFname, "a");
    
    for(unsigned int i=0; i<m_oPop.size(); i++)
        fprintf(file, " %10.8f", -1.0 * (-1.0E10 + m_oPop[i].getFitness()) );
    fprintf(file, "\n");
    
    fclose(file);
}

/**
 * Print the Min fitness, the avergage fitness and the Max fitness
 */
template<class T>
void svt_ga<T>::printStatistics()
{
    printf("%d\t%10.8f\t%10.8f\t%10.8f\n", m_iNoUniqueInd, -1.0 * (-1.0E10 +m_fMinFitness), -1.0 * (-1.0E10 +m_fAvgFitness),-1.0 * (-1.0E10 + m_fMaxFitness));
};

/**
 * output the configuration of the program
 */
template<class T>
void svt_ga<T>::writeConfiguration(char *pFnameParam)
{
    FILE* pFileParam = fopen( pFnameParam, "a" );

    fprintf( pFileParam, "PopulationSize = %i\n",           getPopSize() );
    fprintf( pFileParam, "ReinsertionScheme = %i\n",        m_iReinsertionScheme );
    fprintf( pFileParam, "MutationProbability = %f\n",      getMutationProb() );
    fprintf( pFileParam, "MutationOffset = %f\n",           getMutationOffset() );
    fprintf( pFileParam, "CrossoverProbability = %f\n",     getCrossoverProb() );
    fprintf( pFileParam, "SelectivePressure = %f\n",        getSelectivePressure() );
    fprintf( pFileParam, "MaxGenerations = %i\n",           getMaxGen() );
    fprintf( pFileParam, "MaxThreads = %i\n",               getMaxThread() );
    fprintf( pFileParam, "TranspositionProbability = %f\n", getTranspositionProb() );
    fprintf( pFileParam, "DistanceThreshold = %f\n",        getDistanceThreshold() );
    fprintf( pFileParam, "DistancePenalty = %f\n",          getDistanceThresholdPenalty() );
    if (m_bMutateAll)
        fprintf( pFileParam, "MutateAll = TRUE\n" );
    else
        fprintf( pFileParam, "MutateAll = FALSE\n" );
    fprintf( pFileParam, "MutateAllProportion = %f\n",      getMutateAllProportion() );  
    fprintf( pFileParam, "StopScore = %f\n",                getStopScore() );
    fprintf( pFileParam, "TabuWindowSize = %i\n",           getTabuWindowSize() );
    fprintf( pFileParam, "TabuThreshold = %f\n",            getTabuThreshold() );
    fprintf( pFileParam, "TabuRegionSize = %f\n",           getTabuRegionSize() );
    fprintf( pFileParam, "NicheSize = %f\n",     	    getNicheSize() );
    fprintf( pFileParam, "MapPopPerNiche = %f\n",     	    getMaxPopPerNiche() );
    fprintf( pFileParam, "SameNichePenalty = %f\n",         getSameNichePenalty() );
    fprintf( pFileParam, "RefinementMaxMutPerGene = %d\n",  getRefinementMaxMutPerGene());
	
    fclose( pFileParam );
};

///////////////////////////////////////////////////////////////////////////////
// run ga in thread 
///////////////////////////////////////////////////////////////////////////////

/**
 * Thread function - it starts the ga in a thread
 */
template<class T>
void* runThread(void* pData)
{
    if (!pData)
        return NULL;
   
    svt_ga<T>* pGA = (svt_ga<T>*)pData;
    pGA->setIsThreadRunning( true );
    pGA->run();
    pGA->setIsThreadRunning( false );
    
    return NULL;
}


/**
 * function to create the thread 
 */
template<class T>
void svt_ga<T>::initThread()
{
    svt_createThread(&runThread<T>, (void*)this, SVT_THREAD_PRIORITY_NORMAL);
}

///////////////////////////////////////////////////////////////////////////////
// tabu search functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Set the tabu search window size. The tabu search computes the gene distances of the top-individual over time, with a moving window. It averages all those distance values.
 * If the distances vary a lot, because constantly completely new solutions get to the top, everything is considered fine. If the average drops, and only small differences
 * can be seen, this probably means premature convergence. The size of the window can be adjusted with this function.
 * \param iTabuWindowSize new size of the tabu-search window
 */
template<class T>
void svt_ga<T>::setTabuWindowSize( unsigned int iTabuWindowSize )
{
    m_iTabuWindowSize = iTabuWindowSize;
};
/**
 * Get the tabu search window size. The tabu search computes the gene distances of the top-individual over time, with a moving window. It averages all those distance values.
 * If the distances vary a lot, because constantly completely new solutions get to the top, everything is considered fine. If the average drops, and only small differences
 * can be seen, this probably means premature convergence. The size of the window can be accessed with this function.
 * \return size of the tabu-search window
 */
template<class T>
unsigned int svt_ga<T>::getTabuWindowSize( )
{
    return m_iTabuWindowSize;
};

/**
 * At some point the distances of the top individuals get really small and we consider this as stagnation of the GA. With this function one can set the threshold, if the
 * average distance is lower, we store the top individual in an array and remove all individuals from this region.
 * \fTabuThreshold the new threshold below which we say the GA stagnates
 */
template<class T>
void svt_ga<T>::setTabuThreshold( Real64 fTabuThreshold )
{
    m_fTabuThreshold = fTabuThreshold;
};
/**
 * At some point the distances of the top individuals get really small and we consider this as stagnation of the GA. With this function one can access the threshold, if the
 * average distance is lower, we store the top individual in an array and remove all individuals from this region.
 * \return threshold below which we say the GA stagnates
 */
template<class T>
Real64 svt_ga<T>::getTabuThreshold( )
{
    return m_fTabuThreshold;
};

/**
 * If the distance between an individual and a stored tabu region is smaller than this value, the individual is discarded.
 *\param fTabuRegionSize the new size of the tabu regions
 */
template<class T>
void svt_ga<T>::setTabuRegionSize( Real64 fTabuRegionSize )
{
    m_fTabuRegionSize = fTabuRegionSize;
};

/**
 * If the distance between an individual and a stored tabu region is smaller than this value, the individual is discarded.
 *\return the size of the tabu regions
 */
template<class T>
Real64 svt_ga<T>::getTabuRegionSize( )
{
    return m_fTabuRegionSize;
};

/**
 * check whether the ind is in one of the tabu regions
 * \param pInd
 **/
template<class T>
bool svt_ga<T>::isInTabuReg(T* pInd)
{
    bool bInTabuReg = false;
    for(unsigned int j=0; j<this->m_oTabus.size(); j++)
    {
        if (pInd->distance( this->m_oTabus[j] ) < this->m_fTabuRegionSize)
            bInTabuReg = true;
    }
    return bInTabuReg;
}


/**
 * set the parent GA
 * \param pParentGa - the ga that started this thread ; NULL if the main thread
 */
template<class T>
void svt_ga<T>::setParentGA(svt_ga* pParentGA)
{
    m_pParentGA = pParentGA;
};

/**
 * get the parent GA
 * \return pParentGa - the ga that started this thread ; NULL if the main thread
 */
template<class T>
svt_ga<T>* svt_ga<T>::getParentGA()
{
    return m_pParentGA;
};

 
/**
 * refine an individual; 
 * \param the individual that will be refined
 */
template<class T>
void svt_ga<T>::refineInd( T* pInd)
{
    char pOut[1024];
    sprintf(pOut, "[%02d-%04d] Added new tabu region (now %d): ", m_iThread, m_iGenerations, (int)m_oTabus.size() );
    svtout << pOut << endl;
    
    pInd->setAge(m_oTabus.size() + 1);
            
    m_oTabus.push_back( *pInd );
    sort(m_oTabus.begin(), m_oTabus.end());
    m_oTabuWindow.clear();
    
    //any of the individuals are in the new added tabu region
    for(unsigned int i=0; i<m_oTempPop.size(); i++)
    {
        if (m_oTempPop[i].distance( m_oTabus[m_oTabus.size()-1] ) < m_fTabuRegionSize)
            m_oTempPop[i].setFitness( 0.0 );
    }
    //outputResult(true); 

    //if added tabu just before merge , then let the ga run for a little longer to refine this solution
    if (m_iMaxGen-m_iGenerations < 15)
 	m_iGenerations = m_iMaxGen - 15;

    return;
};
 
///////////////////////////////////////////////////////////////////////////////
//  Sharing
///////////////////////////////////////////////////////////////////////////////

/**
 * set the Niche size
 * \param the new nicheSize
 */
template<class T>
void svt_ga<T>::setNicheSize(Real64 fNicheSize)
{
    m_fNicheSize = fNicheSize;
};

/**
 * get the Niche size
 * \return nicheSize
 */
template<class T>
Real64 svt_ga<T>::getNicheSize()
{
    return m_fNicheSize;
};

/**
 * set the maximum allowed population per Niche - expressed as a proportion of the original population
 * Remarks: beyong this value (5-10%) - individuals have their fitness set to 0
 * \param proportion of individuals allowed in one niche
 */
template<class T>
void svt_ga<T>::setMaxPopPerNiche(Real64 fMaxPopPerNiche)
{
    m_fMaxPopPerNiche = fMaxPopPerNiche;
};

/**
 * get the maximum allowed population per Niche - expressed as a proportion of the original population
 * \param proportion of individuals allowed in one niche
 */
template<class T>
Real64 svt_ga<T>::getMaxPopPerNiche()
{
    return m_fMaxPopPerNiche;
};

/**
 * set the Niche distance penalty - penalize individuals in the same niche according to their rank to the top individual
 * \param how much will individuals be penalized 
 */
template<class T>
void svt_ga<T>::setSameNichePenalty(Real64 fSameNichePenalty)
{
    m_fSameNichePenalty = fSameNichePenalty;
};

/**
 * get the Niche distance penalty - penalize individuals in the same niche according to their rank to the top individual
 * \param how much will the 
 */
template<class T> 
Real64 svt_ga<T>::getSameNichePenalty()
{

    return m_fSameNichePenalty;
};

/**
 * function to create the thread
 * it assumes that the object that calls it already has the pop initialized 
 */
template<class T>
svt_population<T> svt_ga<T>::execute()
{
    static svt_semaphore oSema;

    if (!oSema.tryLock())
        return m_oPop;

    //clear arrays/lists if runs were already executed

    for( unsigned int iThread=0; iThread < m_oGA_Array.size() && iThread < m_iMaxThread; iThread++ )
    {
	if (m_oGA_Array[iThread]!=NULL)
            delete(m_oGA_Array[iThread]);
    }
    m_oGA_Array.clear();

    delTabuRegions();
    setDone(false);
 
    vector< svt_ga* > oGA_Array;
    for(unsigned int iThread=0; iThread < m_iMaxThread; iThread++ )
    {
	svt_ga* pGA_Tmp = (*this).createObject() ;
	pGA_Tmp->setThread( iThread );
	pGA_Tmp->setRun( m_iRun );
	
	if (m_iMaxThread == 1)
	{
	    pGA_Tmp->setMaxGen( m_iMaxGen  );
	    m_iSyncGen = m_iMaxGen;
	}
	else
	    pGA_Tmp->setMaxGen( m_iSyncGen );
	
	//(*this).initPopulation( m_iPopSize );
	//pGA_Tmp->setPopulation( m_oPop );
	pGA_Tmp->setParentGA( this );
	oGA_Array.push_back( pGA_Tmp );
    }
    m_oGA_Array = oGA_Array;
    
    long iStartTimeMerge, iStartTime = svt_getElapsedTime();    
    unsigned int iThreadGenerations = 0;

#ifdef __GAFIT_FORK
    vector<pid_t> oPid;
    pid_t iPid;
#endif
    bool bDone = false;
    svt_population<T> oFinalPop, oTotalPop, oTotalTabuPop;	
    char pOut[1024];

    for( m_iParallelRun = 0; (int)m_iParallelRun <  m_iMaxGen/m_iSyncGen && bDone == false; m_iParallelRun++)
    {
	iStartTimeMerge	= svt_getElapsedTime();
	
	svtout << "Starting " << m_iMaxThread << " threads..." << endl;
	
	// start the threads
	for( unsigned int iThread=0; iThread < m_iMaxThread; iThread++ )
	{
	    m_oGA_Array[iThread]->setCurrGen( 0 );
	    m_oGA_Array[iThread]->setDone( false );
#ifndef __GAFIT_FORK
	    m_oGA_Array[iThread]->initThread();
#else
	    if ( (iPid = fork()) == 0 )
	    {
		oPid.push_back( iPid );
		m_oGA_Array[iThread]->run();
		exit( 0 );
	    }
#endif
	}

#ifdef __GAFIT_FORK
	while( oPid.size() < (unsigned int)m_iMaxThread );
#endif

	// make sure threads are really running
	bool bSomeNotRunning = true;
	while(bSomeNotRunning)
	{
	    bSomeNotRunning = false;
	    for( unsigned int iThread=0; iThread < m_iMaxThread; iThread++ )
		if (!m_oGA_Array[iThread]->getIsThreadRunning())
		    bSomeNotRunning = true;
	}

	sprintf(pOut,"Waiting for the threads to finish...");
	svtout << pOut << endl;
	
	// wait until they are finished
	bool bAllFinished = false;
	while(!bAllFinished)
	{
	    bAllFinished = true;
	    for( unsigned int iThread=0; iThread < m_iMaxThread && bDone == false; iThread++ )
	    {
		if ( !m_oGA_Array[iThread]->getDone() )
		    bAllFinished = false;
		else
		    if (m_oGA_Array[iThread]->getCurrGen() < m_iSyncGen)
		    {
			//svtout << "Thread " << iThread << " finished with stopping criterion (score higher than)!" << endl;
			bDone = true;
			for(unsigned int i=0; i < m_iMaxThread; i++ )
			    if (i!=iThread)
				m_oGA_Array[i]->setDone( true );
			iThreadGenerations += m_oGA_Array[iThread]->getCurrGen();
		    }
	    }
	    if (!bAllFinished)
		svt_sleep( 50 );
	}
	

	// make sure threads are really not running anymore
	bool bSomeRunning = true;
	while(bSomeRunning)
	{
	    bSomeRunning = false;
	    for( unsigned int iThread=0; iThread < m_iMaxThread; iThread++ )
		if (m_oGA_Array[iThread]->getIsThreadRunning())
		    bSomeRunning = true;
	}

        if (m_bDone) // the user stop it most probably and by now all threads are done
        {
            oSema.V();
            return m_oPop;
        }
	
	//delete the content of the populations and start again
	oTotalPop.clear(); 
	oTotalTabuPop.clear();
	
	// combine populations
	for( unsigned int iThread=0; iThread < m_iMaxThread; iThread++ )
	{
	    svt_population<T> oTempPop = m_oGA_Array[iThread]->getPopulation();
	    svt_population<T> oTabuTempPop = m_oGA_Array[iThread]->getTabuRegions();
	    
	    oTotalPop.insert(oTotalPop.begin(), oTempPop.begin(), oTempPop.end());
	    oTotalTabuPop.insert(oTotalTabuPop.begin(), oTabuTempPop.begin(), oTabuTempPop.end());
	}
	
	// keep track of how many generations we have so far
	if (!bDone)
	    iThreadGenerations+=m_iSyncGen;

	sort( oTotalPop.begin(), oTotalPop.end() );
	m_oGA_Array[0]->penalizeSimilar(oTotalPop, 2.0*getDistanceThreshold(), (1-2.0*(1-getDistanceThresholdPenalty())) ); //penalty twice larger than usual
	sort( oTotalPop.begin(), oTotalPop.end() );

	sort( oTotalTabuPop.begin(), oTotalTabuPop.end() );
        m_oGA_Array[0]->penalizeSimilar(oTotalTabuPop, 0.0, 1.0 );
	m_oGA_Array[0]->discardNullInd(oTotalTabuPop);
	sort( oTotalTabuPop.begin(), oTotalTabuPop.end() );

	m_oTabus = oTotalTabuPop;

        svt_sleep( 1000 );

	svtout << "Recombination of the populations:" << endl;
        svtout << "  Highest fitness: "<<  oTotalPop[oTotalPop.size()-1].getFitness() << endl;
        svtout << "  Number of generations (per thread): " <<  iThreadGenerations << endl;
	svtout << "  Number of tabu regions  "<<  (int)oTotalTabuPop.size() << endl;
	svtout << "  Partial Elapsed time : " <<  (((svt_getElapsedTime() - iStartTimeMerge)/(1000.0f))/60.0f) << " min" << endl;
        	
	if (!bDone)
	{
	    svt_population<T> oNewPop;
	    oNewPop.insert( oNewPop.begin(), oTotalPop.end()-m_iPopSize, oTotalPop.end());
	    for( unsigned int iThread=0; iThread < m_iMaxThread; iThread++ )
	    {
		m_oGA_Array[iThread]->setPopulation( oNewPop );
		m_oGA_Array[iThread]->setTabuRegions( oTotalTabuPop );
		if (iThread == 0)
		    m_oGA_Array[iThread]->outputResult( true );
	    }
	}
    }

    //Output runtime stats
    long iETime = svt_getElapsedTime() - iStartTime;
    svtout << "Number of generations (per thread): " << iThreadGenerations <<  endl;
    svtout << "Elapsed time: " << ((iETime/(1000.0f))/60.0f) << " min" <<  endl; 
 
    //clear the content of the threads
    clearThreads();
    
    //Now combine the final population with the tabu regions
    oFinalPop = oTotalPop;
    if (oTotalTabuPop.size()>0) 
	oFinalPop.insert( oFinalPop.begin(), oTotalTabuPop.begin(), oTotalTabuPop.end() );
    
    sort( oFinalPop.rbegin(), oFinalPop.rend() );
    
    char pFilename[1024];
    strcpy(pFilename,"GlobalSearchSolution");
    writeSolutions(oFinalPop, 1, pFilename);

    oSema.V();

    return oFinalPop;
}

/**
 * Clear the content of the threads
 */
template<class T>
void svt_ga<T>::clearThreads()
{
    unsigned int iFitnessUpdateCount=0;
    for( unsigned int iThread=0; iThread < m_iMaxThread; iThread++ )
    {
	iFitnessUpdateCount += m_oGA_Array[iThread]->getFitnessUpdateCount();
	
	//clear for the next run
	//m_oGA_Array[iThread]->delTabuRegions();   
	//delete(m_oGA_Array[iThread]);
    }
    //delete clear all
    //m_oGA_Array.clear();

    svtout << "Fitness computed  " << iFitnessUpdateCount << " times" << endl; 
    svtout << endl;
}

/**
 * Refine population
 * \param oPop what population
 * \param iNoInd4Refinement how many individuals are to be refined
 */
template<class T>
void svt_ga<T>::refine(svt_population<T> &oPop, unsigned int iNoInd4Refinement)
{
    if (oPop.size() > 0)
    {
	delTabuRegions();
	
	unsigned int iEffectivePopSize = iNoInd4Refinement<oPop.size()?iNoInd4Refinement:oPop.size();
	svtout<< endl;
	svtout << "Restart GA only with " << iEffectivePopSize << " tabus" << endl;
	svtout << endl;
	
	long iStartTime = svt_getElapsedTime();
	
	//store settings
	unsigned int iPopSize		 = m_iPopSize;
	unsigned int iMaxGen  		 = m_iMaxGen;
	unsigned int iMaxThread  	 = m_iMaxThread;
	Real64 fTabuThreshold 		 = m_fTabuThreshold;
	
	
	
	//set population here to assess the new scores of this populaiton
	setPopulation( oPop );
	updateFitness();
	oPop = getPopulation();
	sort( oPop.rbegin(), oPop.rend() );
	oPop.erase( oPop.begin()+iEffectivePopSize, oPop.end() );

	setPopulation( oPop ); 

	setMaxGen( 101 );
	setCurrGen( 0 );
	setThread( 0 );
	setMaxThread( 0 );
	setTabuThreshold(  0.0 );
	
	run();
	
	long iETime = svt_getElapsedTime() - iStartTime;
	svtout << "Elapsed time: " << ((iETime/(1000.0f))/60.0f) << " min" <<  endl;
	
	//set settings used before refinement
	setPopSize ( iPopSize );
	setMaxGen( iMaxGen );
	setMaxThread( iMaxThread );
	setTabuThreshold ( fTabuThreshold );

	svtout << "Number of tabu regions at the end of refine " << m_oTabus.size() << endl;

   }
}



/**
 * Thread function - it starts the ga in a thread
 */
template<class T>
void* runThreads(void* pData)
{
    if (!pData)
        return NULL;
    
    svt_ga<T>* pGA = (svt_ga<T>*)pData;
    
    pGA->setIsThreadRunning( true );
    pGA->execute();
    pGA->setIsThreadRunning( false );
    
    return NULL;
}


/**
 * function to create the thread 
 */
template<class T>
void svt_ga<T>::initThreads()
{
   svt_createThread(&runThreads<T>, (void*)this, SVT_THREAD_PRIORITY_NORMAL);
}

#endif
