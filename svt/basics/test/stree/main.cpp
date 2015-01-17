// the classes of the algorithm
#include <svt_basics.h>
#include <svt_binary_tree.h>
#include <svt_stree.h>
#include <svt_iostream.h>

// lib_pio
#include "situs.h"
#include "lib_pio.h"

// main routine
int main(int argc, char* argv[])
{
    // check command line arguments
    if (argc < 4)
    {
	cout << "usage: stree <pdb input file> <output file> <max number of nodes>" << endl;
	return 10;
    }

    // argument 3 - number of nodes in the tree
    int iMaxNodes = atoi(argv[3]);
    // argument 1 - PDB file
    PDB* pPDB;
    unsigned iNum;
    read_pdb( argv[1], &iNum, &pPDB );

    // ok, now lets start the training
    if (iNum != 0)
    {
	svt_stree oTree;
	oTree.setMaxNodes( iMaxNodes );
        //oTree.setFaithful(true);

	while (oTree.getConv() == false)
	{
	    for(unsigned i=0; i<iNum; i++)
		oTree.trainStree2( svt_vector4<double>( pPDB[i].x, pPDB[i].y, pPDB[i].z ) );
	}

        //oTree.exportPDB(argv[2]);
    }

    return 0;
}
