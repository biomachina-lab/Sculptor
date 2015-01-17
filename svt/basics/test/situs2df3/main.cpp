/***************************************************************************
                          main
                          ----
    begin                : 06/22/06
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_volume.h>

#define SITUS2DF3 cout << svt_trimName("situs2df3")

int main(int argc, char** argv)
{
    ///////////////////////////////////////////////////////////////////////////
    // Parsing options
    ///////////////////////////////////////////////////////////////////////////

    if (argc < 2)
    {
	cout << "usage: situs2df3 <situs file> <output df3 file>" << endl;
        cout << endl;
        return 0;
    }

    // load volume
    svt_volume< Real64 > oVol;
    oVol.loadSitus( argv[1] );

    // some stats about the volume
    SITUS2DF3 << "Min density: " << oVol.getMinDensity() << endl;
    SITUS2DF3 << "Max density: " << oVol.getMaxDensity() << endl;

    // save as df3
    SITUS2DF3 << "Save as df3 file " << argv[2] << endl;
    oVol.writeDF3( argv[2] );

    return 0;
};
