/***************************************************************************
                          rawivVIP
                          --------
    begin                : 01/13/2004
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// THIS FILE IS PART OF THE SVT/SENSITUS VOL I/O PLUGIN SDK

// This is a demo plugin for the svt volumetric i/o plugin architecture.
// It realizes a reader for the rawiv format.

#include <demoVLP.h>
#include <stdio.h>
#include <stdlib.h>

extern "C" {
    char* getExtension()
    {
	return "rawiv";
    };
}

extern "C" {
    char* getExtensionDesc()
    {
	return "Raw file format with iv header";
    };
}

extern "C" {
    svt_vol loadVolume(char* pFile)
    {
	// do we have a big endian (e.g. SGI, SUN, etc) or little endian machine (e.g. Intel, AMD)?
	// raviv header is big endian!
        bool bSwap = !bigEndianMachine();

        // now open file
	FILE* pFile = fopen(pFile, "rb");
        // and init svt_vol object
	svt_vol oVol;
	oVol.iSizeX = 0;
	oVol.iSizeY = 0;
	oVol.iSizeZ = 0;
	oVol.pData = NULL;
        // was the file ok?
	if (pFile != NULL)
	{
            // read rawiv header
            float fMinX = readFloat(pFile, bSwap);
            float fMinY = readFloat(pFile, bSwap);
            float fMinZ = readFloat(pFile, bSwap);
            float fMaxX = readFloat(pFile, bSwap);
            float fMaxY = readFloat(pFile, bSwap);
	    float fMaxZ = readFloat(pFile, bSwap);

	    int iNumVerts = readUInt(pFile, bSwap);
	    int iNumCells = readUInt(pFile, bSwap);

	    int iDimX = readUInt(pFile, bSwap);
	    int iDimY = readUInt(pFile, bSwap);
            int iDimZ = readUInt(pFile, bSwap);

            float fOriginX = readFloat(pFile, bSwap);
            float fOriginY = readFloat(pFile, bSwap);
            float fOriginZ = readFloat(pFile, bSwap);

	    float fSpanX = readFloat(pFile, bSwap);
	    float fSpanY = readFloat(pFile, bSwap);
	    float fSpanZ = readFloat(pFile, bSwap);

	    // header corrupt?
	    if (iNumVerts = iDimX * iDimY * iDimZ)
	    {
		// size of volume
                oVol.iExtX = iDimX;
                oVol.iExtY = iDimY;
                oVol.iExtZ = iDimZ;

		// voxel size equal to span
                oVol.fSpanX = fSpanX;
                oVol.fSpanY = fSpanY;
                oVol.fSpanZ = fSpanZ;

		// allocate memory
                oVol.pData = new float[iDimX * iDimY * iDimZ];

		// now read raw data
                for(int iZ=0; iZ < iDimZ; iZ++)
		    for(int iY=0; iY < iDimY; iY++)
			for(int iX=0; iX < iDimX; iX++)
			    oVol.pData[iX + iY*iDimX + iZ*iDimX*iDimY] = readFloat(pFile, bSwap);

		fclose(pFile);

	    } else
		printf("svt_vip_rawiv: Sorry, cannot load file. The file has a corrupted header!\n");
	}

	return oVol;
    };
}

extern "C" {
    bool saveVolume(svt_vol oVol, char* pFile)
    {
	// do we have a big endian (e.g. SGI, SUN, etc) or little endian machine (e.g. Intel, AMD)?
	// raviv header is big endian!
	bool bSwap = !bigEndianMachine();

	// now open file
	FILE* pFile = fopen(pFile, "wb");
	// was the file ok?
	if (pFile != NULL)
	{
	    // write rawiv header
            writeFloat(pFile, bSwap, 0.0f); // fMinX
            writeFloat(pFile, bSwap, 0.0f); // fMinY
            writeFloat(pFile, bSwap, 0.0f); // fMinZ
            writeFloat(pFile, bSwap, oVol.iExtX * oVol.fSpanX); // fMaxX
            writeFloat(pFile, bSwap, oVol.iExtY * oVol.fSpanY); // fMaxY
            writeFloat(pFile, bSwap, oVol.iExtZ * oVol.fSpanZ); // fMaxZ

	    writeUInt(pFile, bSwap, oVol.iExtX * oVol.iExtY * oVol.iExtZ); // iNumVerts
	    writeUInt(pFile, bSwap, (oVol.iExtX-1)*(oVol.iExtY-1)*(oVol.iExtZ-1)); // iNumCells

	    writeUInt(pFile, bSwap, oVol.iExtX); // iDimX
	    writeUInt(pFile, bSwap, oVol.iExtY); // iDimY
            writeUInt(pFile, bSwap, oVol.iExtZ); // iDimZ

            writeFloat(pFile, bSwap, 0.0f); // fOriginX
            writeFloat(pFile, bSwap, 0.0f); // fOriginY
            writeFloat(pFile, bSwap, 0.0f); // fOriginZ

	    writeFloat(pFile, bSwap, oVol.fSpanX); // fSpanX
	    writeFloat(pFile, bSwap, oVol.fSpanY); // fSpanY
	    writeFloat(pFile, bSwap, oVol.fSpanZ); // fSpanZ

	    // now write raw data
	    for(int iZ=0; iZ < iDimZ; iZ++)
		for(int iY=0; iY < iDimY; iY++)
		    for(int iX=0; iX < iDimX; iX++)
			writeFloat(pFile, bSwap, oVol.pData[iX + iY*iDimX + iZ*iDimX*iDimY]);

	    fclose(pFile);

	} else
	    printf("svt_vip_rawiv: Sorry, cannot load file. The file has a corrupted header!\n");

	return oVol;
    };
}
