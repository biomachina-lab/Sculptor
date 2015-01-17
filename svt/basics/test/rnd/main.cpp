/***************************************************************************
                          main
                          ----
    begin                : 02/12/06
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_types.h>
#include <svt_random.h>
#include <svt_point_cloud_pdb.h>

#define RMSD cout << svt_trimName("rmsd")

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
	cout << "usage: rnd <n> <avg> <sd>" << endl;
	return 1;
    }
    
    
    unsigned int iN = atoi( argv[1] );
    Real64 fAvg     = atof( argv[2] );
    Real64 fSd      = atof( argv[3] );
    
    Real64 fRnd;
    Real64 iIntPart;
    for (unsigned int iIndex=0; iIndex<iN; iIndex++)
    {
        fRnd = svt_ranCauchy(fAvg, fSd);
        cout << fRnd <<  "\t" <<  modf(fRnd,&iIntPart) << endl;
    }

    return 0;
};
