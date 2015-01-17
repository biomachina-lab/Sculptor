/***************************************************************************
                          main
                          ----
    begin                : 06/08/06
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_basics.h>
#include <svt_file_utils.h>

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
	cout << "usage: packandgo <files> <output file>" << endl;
	exit(1);
    }

    vector<string> oInFiles;
    for(int i=2; i<argc-1; i++)
        oInFiles.push_back( string(argv[i]) );

    //svt_createArchive ( string( argv[1] ), oInFiles );
    svt_extractArchive( string( argv[1] ), string("/tmp/") );
};
