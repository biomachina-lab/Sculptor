/***************************************************************************
                          test program
                          -------------------
    begin                : 12/07/09
    author               : Zbigniew Starosolski
    email                : sculptor@biomachina.org
 ***************************************************************************/


#include <svt_stride.h>
#include <iostream>
#include <string.h>
#include <svt_point_cloud_pdb.h>
#include <vector>
#define TE cout << svt_trimName("stride : ")
using namespace std;

int main(int argc, char* argv[])
{


if (argc < 2)
    {
	cout << "usage: test_stride <pdb input file> <pdb output file>" << endl;
        exit(1);
    }

svt_point_cloud_pdb<svt_vector4<Real64 > > oPDB1;

try
    {
        // load data
        oPDB1.loadPDB( argv[1]  );
    }
    catch(svt_exception oError)
    {
    	oError.uiMessage();
	    return 1;
	}


unsigned int iAtomNumber= oPDB1.getAtomsNumber(ALL);
unsigned int i;

printf("%d\n", oPDB1.size());
TE << " iAtomNumber     : " << iAtomNumber << endl;

oPDB1.writePDB(argv[2], false, false, true );

TE << "PDB file_out..  done..     : " << endl;

oPDB1.calcSecStruct();

char cChainID;
char pRecordName[6];
sprintf(pRecordName,"%s","ATOM  ");


// creates a buffor for input to svt_stride class


//char Buffor[1024];

char *pBuffer;
svt_stride oStride;
vector< char *> aBuffer;
vector< char *> vectorRaportString;


	for(i=0; i<iAtomNumber; i++)
	{
      cChainID = oPDB1.getAtom(i)->getChainID();
      if (cChainID == '-')
	  cChainID = ' ';

        if (oPDB1.getAtom(i)->getHetAtm())
            sprintf(pRecordName,"%s","HETATM");
        else
            sprintf(pRecordName,"%s","ATOM  ");

        pBuffer = new char[1024];
        sprintf(pBuffer, "%s%5i %2s%c%c%c%-2s %c%4i%c   %8.3f%8.3f%8.3f%6.2f%6.2f %3s  %4s%2s%2s\n",
			  pRecordName,
			  oPDB1.getAtom(i)->getPDBIndex(),
			  oPDB1.getAtom(i)->getName(),
			  oPDB1.getAtom(i)->getRemoteness(),
			  oPDB1.getAtom(i)->getBranch(),
			  oPDB1.getAtom(i)->getAltLoc(),
			  oPDB1.getAtom(i)->getResname(),
			  cChainID,
			  oPDB1.getAtom(i)->getResidueSeq(),
			  oPDB1.getAtom(i)->getICode(),
			  oPDB1[i].x(),
			  oPDB1[i].y(),
			  oPDB1[i].z(),
			  oPDB1.getAtom(i)->getOccupancy(),
			  oPDB1.getAtom(i)->getTempFact(),
			  oPDB1.getAtom(i)->getNote(),
			  oPDB1.getAtom(i)->getSegmentID(),
			  oPDB1.getAtom(i)->getElement(),
			  oPDB1.getAtom(i)->getCharge());

        aBuffer.push_back(pBuffer);
	}

	// first line of buffor:
	TE <<"aBuffer [0]: " << aBuffer[0] << endl;


	TE << "run external sseindexer..." << endl;

	oStride.sseindexer(aBuffer, vectorRaportString);

	TE << " sseindexer complete ... " << endl;
	TE << " returning vectorRaportString has size: " << vectorRaportString.size() << endl;

	for(i=0; i<vectorRaportString.size(); i++)
		{
		TE << vectorRaportString[i] << endl;
		}

	TE << "testing complete...     : " << endl;

};

