/***************************************************************************
                          svt_ga_ind
                          ----------
    begin                : 06/23/2004
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_ga_ind.h>

/**
 * print genes to cout
 */
void svt_ga_ind::printGenes()
{
    char pOut[1024], pOutTmp[256];
    
    sprintf(pOut, "%8.6f %2d - ", getFitness(), getOrigin());
    
    for (unsigned int i=0; i<m_oGenes.size(); i++)
    {
        sprintf(pOutTmp, "%5.3f ", m_oGenes[i] );
        strcat(pOut, pOutTmp);
    }
    cout << pOut << endl;
}

/**
 * print genes to cout
 */
void svt_ga_ind::printGenes(FILE *file)
{ 
    if (file!=NULL)
    {
	fprintf(file, "%8.6f - ", getFitness());
	
	for (unsigned int i=0; i<m_oGenes.size(); i++)
	    fprintf(file, "%8.6f ", m_oGenes[i] );
	
	fprintf(file,"\n"); 
    }
}

/**
 * print genes to cout
 */
void svt_ga_ind::printGenesPf()
{ 
    printf("%8.6f - ", getFitness());
    
    for (unsigned int i=0; i<m_oGenes.size(); i++)
	printf("%8.6f ", m_oGenes[i] );
    
    printf("\n");
}

/**
 * calculate the distance between two individuals
 * \param rOther reference to the other individual
 * \return vector distance between the two gene-vectors
 */
Real64 svt_ga_ind::distance( svt_ga_ind& rOther )
{
    Real64 fDist = 0.0;

    for(unsigned int i=0; i<m_oGenes.size(); i++)
        fDist += (m_oGenes[i] - rOther.m_oGenes[i])*(m_oGenes[i] - rOther.m_oGenes[i]);

    return sqrt( fDist )/(Real64)m_oGenes.size() ;
}
