/***************************************************************************
                          svt_pdb_blur
			  ------------
    begin                : 05/26/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_pdb.h>

#define g1idz( z, y, x ) ((iKnlExtx*iKnlExty*(z)) + (iKnlExtx*(y)) + (x))
#define g2idz( z, y, x ) ((iStrExtx*iStrExty*(z)) + (iStrExtx*(y)) + (x))
#define g3idz( z, y, x ) ((iOutExtx*iOutExty*(z)) + (iOutExtx*(y)) + (x))

/**
 * blur the atomic structure and return a volumetric data set
 * \param bWater if true the water molecules are excluded from the blurring
 * \param bMassWeight if true the atoms are mass weighted
 * \param fBFactCutoff only atoms with a b-factor below this value are considered
 * \param fWidth voxelwidth of the new map
 * \param fResolution target resolution
 * \param iKernelType
 * \param fKernelAmpl
 * \param bAmplCorr if 1 then we correct for lattice interpolation smoothing effects, if 2 not
 * \return pointer to svt_volume object
 */
svt_volume<Real64>* svt_pdb::blur( bool bWater, bool bMassWeight, Real64 fBFactCutoff, Real64 fWidth, Real64 fResolution, int iKernelType, Real64 fKernelAmpl, bool bAmplCorr )
{
    int iAtomsNum = m_aAtoms.numElements();

    static char *aNames[] =
    {
	"H", "HE", "LI", "BE", "B", "C", "N", "O", "F", "NE",
	"NA", "MG", "AL", "SI", "P", "S", "CL", "AR", "K", "CA",
	"SC", "TI", "V", "CR", "MN", "FE", "CO", "NI", "CU", "ZN"
    };
    static float aAtomMass[30] =
    {
	1.008 ,  4.003,  6.940,  9.012, 10.810, 12.011, 14.007, 16.000,  18.998, 20.170,
	20.170, 24.305, 26.982, 28.086, 30.974, 32.060, 35.453, 39.948,  39.102, 40.080,
	44.956, 47.880, 50.040, 51.996, 54.938, 55.847, 58.933, 58.710,  63.546, 65.380
    };
    static int aStdElement[6] =
    {
	0,5,6,7,14,15
    };

    // mass weighting of the atoms - generate a list of indexes of the atoms, with the mass weighting a single index can appear multiple times...
    vector< Real64 > aMass;
    int j=0, i, iCount;

    for(i=0; i<iAtomsNum; i++)
    {
	// exclude waters?
	if (bWater == true && m_aAtoms[i]->isWater())
	    continue;
	// bfactor cutoff
	if ((0.01*floor(0.5+100*m_aAtoms[i]->getTempFact())) > fBFactCutoff + 0.001)
	    continue;
	// hydrogen or qpdb?
	if (m_aAtoms[i]->isQPDB() || m_aAtoms[i]->isHydrogen())
	    continue;

	if (!bMassWeight)
	{

	    aMass.push_back( 1.0 );

	} else {

	    // mass-weighting
	    for ( j=2; j<30; j++)
	    {
		if ( strcmp(m_aAtoms[i]->getAtomType(),aNames[j])==0 )
		{
                    aMass.push_back( aAtomMass[j] );
                    break;
		}
	    }

            // exact atom name was not found, so go by the first character...
	    if ( j==30 )
	    {
		for ( j=0; j<6; j++ )
		{
		    if ( strncmp(m_aAtoms[i]->getAtomType(), aNames[aStdElement[j]],1)==0 )
		    {
			aMass.push_back( aAtomMass[aStdElement[j]] );
			break;
		    }
		}

		// still not recognized!!! That is bad, but lets include the atom anyways...
		if ( j==6 )
		    aMass.push_back( 1.0 );
	    }
	}

    }

    // bring lattice into register with origin
    Real64 fMinx = fWidth * floor( (getMinXAtomCoord()*1.0E10) / fWidth );
    Real64 fMaxx = fWidth * ceil( (getMaxXAtomCoord()*1.0E10) / fWidth );
    Real64 fMiny = fWidth * floor( (getMinYAtomCoord()*1.0E10) / fWidth );
    Real64 fMaxy = fWidth * ceil( (getMaxYAtomCoord()*1.0E10) / fWidth );
    Real64 fMinz = fWidth * floor( (getMinZAtomCoord()*1.0E10) / fWidth );
    Real64 fMaxz = fWidth * ceil( (getMaxZAtomCoord()*1.0E10) / fWidth );

    // allocate protein density map
    int iMargin = 2;
    int iStrExtx = (int)(ceil( (fMaxx - fMinx) / fWidth )) + (2*iMargin) + 1;
    int iStrExty = (int)(ceil( (fMaxy - fMiny) / fWidth )) + (2*iMargin) + 1;
    int iStrExtz = (int)(ceil( (fMaxz - fMinz) / fWidth )) + (2*iMargin) + 1;

    int iStrNVox = iStrExtx * iStrExty * iStrExtz;

    double* pStrPhi = new double[iStrNVox];

    // target resolution or strict kernel width
    Real64 fSigma1, fSigma2, fSigma3, fSigma4, fSigma5;
    Real64 fRh1, fRh2, fRh3, fRh4, fRh5;
    Real64 fRc1, fRc2, fRc3, fRc4, fRc5;
    if (fResolution < 0)
    {
	fSigma1 = fResolution/(-2.0);
	fSigma2 = fResolution/(-2.0);
	fSigma3 = fResolution/(-2.0);
	fSigma4 = fResolution/(-2.0);
	fSigma5 = fResolution/(-2.0);

	fRh1 = fSigma1 * sqrt(log(2.0)) / sqrt(1.5);
	fRh2 = fSigma2 / (exp((1.0/1.0)*log(2.0)) * sqrt(3.0*(3.0+1.0)/(5.0*(5.0+1.0))));
	fRh3 = fSigma3 / (exp((1.0/1.5)*log(2.0)) * sqrt(3.0*(3.0+1.5)/(5.0*(5.0+1.5))));
	fRh4 = fSigma4 / (exp((1.0/2.0)*log(2.0)) * sqrt(3.0*(3.0+2.0)/(5.0*(5.0+2.0))));
	fRh5 = fSigma5 / (exp((1.0/60.0)*log(2.0)) * sqrt(3.0*(3.0+60.0)/(5.0*(5.0+60.0))));

    } else {

	fRh1 = fResolution;
	fRh2 = fResolution;
	fRh3 = fResolution;
	fRh4 = fResolution;
	fRh5 = fResolution;

	fSigma1 = fRh1 * sqrt(1.5) / sqrt(log(2.0));
	fSigma2 = fRh2 * (exp((1.0/1.0)*log(2.0)) * sqrt(3.0*(3.0+1.0)/(5.0*(5.0+1.0))));
	fSigma3 = fRh3 * (exp((1.0/1.5)*log(2.0)) * sqrt(3.0*(3.0+1.5)/(5.0*(5.0+1.5))));
	fSigma4 = fRh4 * (exp((1.0/2.0)*log(2.0)) * sqrt(3.0*(3.0+2.0)/(5.0*(5.0+2.0))));
	fSigma5 = fRh5 * (exp((1.0/60.0)*log(2.0)) * sqrt(3.0*(3.0+60.0)/(5.0*(5.0+60.0))));
    }
    fRc1 = sqrt( 3.0 ) * fSigma1;
    fRc2 = ( exp( (1.0/1.0 ) * log(2.0) ) ) * fRh2;
    fRc3 = ( exp( (1.0/1.5 ) * log(2.0) ) ) * fRh3;
    fRc4 = ( exp( (1.0/2.0 ) * log(2.0) ) ) * fRh4;
    fRc5 = ( exp( (1.0/60.0) * log(2.0) ) ) * fRh5;

    // if target resolution is too high for the lattice voxel width, just exploit lattice smoothing, no kernel convolution...
    if ( fRh1/fWidth < 1.0 )
    {
	iKernelType = 6;
	fKernelAmpl = 1.0;
    }

    //
    // interpolate structure to protein map and keep track of variability - i.e. create a volumetric map with peaks at the atomic positions...
    //
    for (iCount=0; iCount<iStrNVox; iCount++)
	*(pStrPhi+iCount) = 0;
    Real64 fVarp = 0.0;
    Real64 fGx, fGy, fGz, fA, fB, fC;
    Real64 fKmsd, fVarmap;
    int iX0, iY0, iZ0, iX1, iY1, iZ1;

    for (i=0; i<iAtomsNum; i++)
    {
	// compute position within grid
	fGx = iMargin + (((m_aAtoms[i]->getAtomPos().x()*1.0E10) - fMinx) / fWidth);
	fGy = iMargin + (((m_aAtoms[i]->getAtomPos().y()*1.0E10) - fMiny) / fWidth);
	fGz = iMargin + (((m_aAtoms[i]->getAtomPos().z()*1.0E10) - fMinz) / fWidth);

	iX0 = (int)(floor(fGx));
	iY0 = (int)(floor(fGy));
	iZ0 = (int)(floor(fGz));
	iX1 = iX0+1;
	iY1 = iY0+1;
	iZ1 = iZ0+1;

	// interpolate
	fA = (Real64)(iX1) - fGx;
	fB = (Real64)(iY1) - fGy;
	fC = (Real64)(iZ1) - fGz;

	*(pStrPhi + g2idz(iZ0,iY0,iX0)) += fA * fB * fC * aMass[i];                 fVarp += fA * fB * fC * ((1.0-fA)*(1.0-fA)+(1.0-fB)*(1.0-fB)+(1.0-fC)*(1.0-fC));
	*(pStrPhi + g2idz(iZ1,iY0,iX0)) += fA * fB * (1.0-fC) * aMass[i];           fVarp += fA * fB * (1.0-fC) * ((1.0-fA)*(1.0-fA)+(1.0-fB)*(1.0-fB)+fC*fC);
	*(pStrPhi + g2idz(iZ0,iY1,iX0)) += fA * (1-fB) * fC * aMass[i];             fVarp += fA * (1.0-fB) * fC * ((1.0-fA)*(1.0-fA)+fB*fB+(1.0-fC)*(1.0-fC));
	*(pStrPhi + g2idz(iZ0,iY0,iX1)) += (1.0-fA) * fB * fC * aMass[i];           fVarp += (1.0-fA) * fB * fC * (fA*fA+(1.0-fB)*(1.0-fB)+(1.0-fC)*(1.0-fC));
	*(pStrPhi + g2idz(iZ1,iY1,iX0)) += fA * (1-fB) * (1.0-fC) * aMass[i];       fVarp += fA * (1.0-fB) * (1.0-fC) * ((1.0-fA)*(1.0-fA)+fB*fB+fC*fC);
	*(pStrPhi + g2idz(iZ0,iY1,iX1)) += (1.0-fA) * (1-fB) * fC * aMass[i];       fVarp += (1.0-fA) * (1.0-fB) * fC * (fA*fA+fB*fB+(1.0-fC)*(1.0-fC));
	*(pStrPhi + g2idz(iZ1,iY0,iX1)) += (1.0-fA) * fB * (1.0-fC) * aMass[i];     fVarp += (1.0-fA) * fB * (1.0-fC) * (fA*fA+(1.0-fB)*(1.0-fB)+fC*fC);
	*(pStrPhi + g2idz(iZ1,iY1,iX1)) += (1.0-fA) * (1-fB) * (1.0-fC) * aMass[i]; fVarp += (1.0-fA) * (1.0-fB) * (1.0-fC) * (fA*fA+fB*fB+fC*fC);
    }
    fVarp /= (double)(iAtomsNum);

    cout << "svt_pdb_blur> Voxel-width: " << fWidth << endl;
    cout << "svt_pdb_blur> Varp: " << fVarp << endl;

    printf ("svt_pdb_blur> Lattice smoothing (sigma = atom rmsd): %6.3f Angstrom\n", fWidth * sqrt(fVarp) );

    //
    // compute lattice noise corrected kernel maps - these are just the kernel maps, the are later used for the convolution
    //
    double* pKnlPhi =0;
    int iKnlExtx =0;
    int iKnlExty =0;
    int iKnlExtz =0;
    int iKnlExth =0;
    int iKnlNVox =0;
    int iIndx, iIndy, iIndz;
    Real64 fBvalue, fCvalue;
    Real64 fSigmaMap;
    Real64 fDsqu, fDval;

    switch (iKernelType)
    {

    case 1:

        cout << "svt_pdb_blur> Gaussian kernel selected" << endl;

	fKmsd = fSigma1*fSigma1/(fWidth*fWidth);

	fResolution = 2.0*fSigma1;
	// variances are additive for uncorrelated samples
	if (bAmplCorr == false)
	    fResolution = 2.0*sqrt((fSigma1*fSigma1) + (fVarp*fWidth*fWidth));

	fVarmap = fKmsd;
	// variances are additive for uncorrelated samples
	if (bAmplCorr == true)
	    fVarmap -= fVarp; 

	if (fVarmap < 0.0)
	{
	    fprintf(stderr, "svt_pdb_blur> Error: lattice smoothing exceeds kernel size [e.c. 60130]\n");
	    exit(60130);
	}

	// sigma-1D
	fSigmaMap = sqrt(fVarmap / 3.0);

	// truncate at 3 sigma-1D == sqrt(3) sigma-3D
	iKnlExth = (int) ceil( 3.0*fSigmaMap );
	iKnlExtx = 2 * iKnlExth + 1;
	iKnlExty = iKnlExtx;
	iKnlExtz = iKnlExtx;
	iKnlNVox = iKnlExtx * iKnlExty * iKnlExtz;

	pKnlPhi = new double[iKnlNVox];

	if (pKnlPhi == NULL)
	{
	    fprintf(stderr, "svt_pdb_blur> Error: Unable to satisfy memory allocation request [e.c. 60140]\n");
	    exit(60140);
	}

	// write Gaussian within 3 sigma-1D to map
	fBvalue = -1.0 / ( 2.0*fSigmaMap*fSigmaMap );
	fCvalue = 9.0 * fSigmaMap * fSigmaMap;
	for (iCount=0; iCount< iKnlNVox; iCount++)
	    *(pKnlPhi+iCount) = 0;

	for (iIndz=0; iIndz<iKnlExtz; iIndz++)
	    for (iIndy=0; iIndy<iKnlExty; iIndy++)
		for (iIndx=0; iIndx<iKnlExtx; iIndx++)
		{
		    fDsqu = (iIndx-iKnlExth)*(iIndx-iKnlExth) + (iIndy-iKnlExth)*(iIndy-iKnlExth) + (iIndz-iKnlExth)*(iIndz-iKnlExth);
		    if (fDsqu < fCvalue)
			*(pKnlPhi+g1idz(iIndz,iIndy,iIndx)) = fKernelAmpl * exp(fDsqu * fBvalue);
		}

	break;

    case 2:

	fKmsd = fSigma2*fSigma2/(fWidth*fWidth);

	fResolution = 2.0*fSigma2;

	// variances are additive for uncorrelated samples
	if (bAmplCorr == false)
	    fResolution = 2.0*sqrt((fSigma2*fSigma2) + (fVarp*fWidth*fWidth));

	fVarmap = fKmsd;

	// variances are additive for uncorrelated samples
	if (bAmplCorr == true)
	    fVarmap -= fVarp;

	if (fVarmap < 0)
	{
	    fprintf(stderr, "svt_pdb_blur> Error: lattice smoothing exceeds kernel size [e.c. 60210]\n");
	    exit(60210);
	}

	iKnlExth = (int) ceil(fRc2/ fWidth);
	iKnlExtx = 2 * iKnlExth + 1;
	iKnlExty = iKnlExtx;
	iKnlExtz = iKnlExtx;
	iKnlNVox = iKnlExtx * iKnlExty * iKnlExtz;

	pKnlPhi = new double[iKnlNVox];
	if (pKnlPhi == NULL)
	{
	    fprintf(stderr, "svt_pdb_blur> Error: Unable to satisfy memory allocation request [e.c. 60220]\n");
	    exit(60220);
	}

	// write kernel to map
	fBvalue = 0.5 * exp(-1.0*log(fRh2)) * exp(1.0*log(fWidth));
	for(iCount=0; iCount<iKnlNVox; iCount++)
	    *(pKnlPhi+iCount) = 0;

	for (iIndz=0; iIndz<iKnlExtz; iIndz++)
	    for (iIndy=0;iIndy<iKnlExty;iIndy++)
		for (iIndx=0;iIndx<iKnlExtx;iIndx++)
		{
		    fDsqu = (iIndx-iKnlExth)*(iIndx-iKnlExth) + (iIndy-iKnlExth)*(iIndy-iKnlExth) + (iIndz-iKnlExth)*(iIndz-iKnlExth);
		    fDsqu = exp((1.0/2.0)*log(fDsqu));
		    fDval = fKernelAmpl * (1.0 - fDsqu * fBvalue);
		    if (fDval<0)
			fDval = 0;
		    *(pKnlPhi + g1idz(iIndz,iIndy,iIndx)) = fDval;
		}

	break;

    case 3:

	fKmsd = fSigma3 * fSigma3 / (fWidth * fWidth);

	fResolution = 2.0*fSigma3;
	// variances are additive for uncorrelated samples
	if (bAmplCorr == false)
	    fResolution = 2.0 * sqrt((fSigma3*fSigma3) + (fVarp*fWidth*fWidth));

	fVarmap = fKmsd;
        // variances are additive for uncorrelated samples
	if (bAmplCorr == true)
	    fVarmap -= fVarp;

	if (fVarmap < 0)
	{
	    fprintf(stderr, "svt_pdb_blur> Error: lattice smoothing exceeds kernel size [e.c. 60230]\n");
	    exit(60230);
	}

	iKnlExth = (int) ceil(fRc3 / fWidth);
	iKnlExtx = 2 * iKnlExth + 1;
	iKnlExty = iKnlExtx;
	iKnlExtz = iKnlExtx;
	iKnlNVox = iKnlExtx * iKnlExty * iKnlExtz;

	pKnlPhi = new double[iKnlNVox];

	if (pKnlPhi == NULL)
	{
	    fprintf(stderr, "svt_pdb_blur> Error: Unable to satisfy memory allocation request [e.c. 60240]\n");
	    exit(60240);
	}

	// write kernel to map
	fBvalue = 0.5 * exp(-1.5*log(fRh3)) * exp(1.5*log(fWidth));

	for(iCount=0; iCount<iKnlNVox; iCount++)
	    *(pKnlPhi+iCount) = 0;

	for (iIndz=0; iIndz<iKnlExtz; iIndz++)
	    for (iIndy=0;iIndy<iKnlExty;iIndy++)
		for (iIndx=0;iIndx<iKnlExtx;iIndx++)
		{
		    fDsqu = (iIndx-iKnlExth)*(iIndx-iKnlExth) + (iIndy-iKnlExth)*(iIndy-iKnlExth) + (iIndz-iKnlExth)*(iIndz-iKnlExth);
		    fDsqu = exp((1.5/2.0)*log(fDsqu));
		    fDval = fKernelAmpl * (1.0 - fDsqu * fBvalue);
		    if (fDval<0)
			fDval = 0;

		    *(pKnlPhi + g1idz(iIndz,iIndy,iIndx)) = fDval;
		}

	break;

    case 4:

	fKmsd = fSigma4*fSigma4 / (fWidth*fWidth);

	fResolution = 2.0*fSigma4;

	// variances are additive for uncorrelated samples
	if (bAmplCorr == false)
	    fResolution = 2.0 * sqrt((fSigma4*fSigma4) + (fVarp*fWidth*fWidth));

	fVarmap = fKmsd;
        // variances are additive for uncorrelated samples
	if (bAmplCorr == true)
	    fVarmap -= fVarp;

	if (fVarmap < 0)
	{
	    fprintf(stderr, "svt_pdb_blur> Error: lattice smoothing exceeds kernel size [e.c. 60230]\n");
	    exit(60230);
	}

	iKnlExth = (int) ceil(fRc4 / fWidth);
	iKnlExtx = 2 * iKnlExth + 1;
	iKnlExty = iKnlExtx;
	iKnlExtz = iKnlExtx;
	iKnlNVox = iKnlExtx * iKnlExty * iKnlExtz;

	pKnlPhi = new double[iKnlNVox];

	if (pKnlPhi == NULL)
	{
	    fprintf(stderr, "svt_pdb_blur> Error: Unable to satisfy memory allocation request [e.c. 60240]\n");
	    exit(60240);
	}

	/* write kernel to map */
	fBvalue = 0.5 * exp(-2.0*log(fRh4)) * exp(2.0*log(fWidth));

	for(iCount=0; iCount<iKnlNVox; iCount++)
	    *(pKnlPhi+iCount) = 0;

	for (iIndz=0; iIndz<iKnlExtz; iIndz++)
	    for (iIndy=0;iIndy<iKnlExty;iIndy++)
		for (iIndx=0;iIndx<iKnlExtx;iIndx++)
		{
		    fDsqu = (iIndx-iKnlExth)*(iIndx-iKnlExth) + (iIndy-iKnlExth)*(iIndy-iKnlExth) + (iIndz-iKnlExth)*(iIndz-iKnlExth);
		    fDsqu = exp((2.0/2.0)*log(fDsqu));
		    fDval = fKernelAmpl * (1.0 - fDsqu * fBvalue);
		    if (fDval<0)
			fDval = 0;

		    *(pKnlPhi + g1idz(iIndz,iIndy,iIndx)) = fDval;
		}

	break;

    case 5:

	fKmsd = fSigma5*fSigma5 / (fWidth*fWidth);

	fResolution = 2.0*fSigma5;
	// variances are additive for uncorrelated samples
	if (bAmplCorr == false)
	    fResolution = 2.0*sqrt((fSigma5*fSigma5) + (fVarp*fWidth*fWidth));
	fVarmap = fKmsd;
        // variances are additive for uncorrelated samples
	if (bAmplCorr == true)
	    fVarmap -= fVarp;

	if (fVarmap < 0)
	{
	    fprintf(stderr, "svt_pdb_blur> Error: lattice smoothing exceeds kernel size [e.c. 60240]\n");
	    exit(60240);
	}

	iKnlExth = (int) ceil(fRc5 / fWidth);
	iKnlExtx = 2 * iKnlExth + 1;
	iKnlExty = iKnlExtx;
	iKnlExtz = iKnlExtx;
	iKnlNVox = iKnlExtx * iKnlExty * iKnlExtz;

	pKnlPhi = new double[iKnlNVox];

	if (pKnlPhi == NULL)
	{
	    fprintf(stderr, "svt_pdb_blur> Error: Unable to satisfy memory allocation request [e.c. 60240]\n");
	    exit(60240);
	}

	// write kernel to map
	fBvalue = 0.5 * exp(-60.0*log(fRh5)) * exp(60.0*log(fWidth));

	for(iCount=0; iCount<iKnlNVox; iCount++)
	    *(pKnlPhi+iCount) = 0;

	for (iIndz=0; iIndz<iKnlExtz; iIndz++)
	    for (iIndy=0;iIndy<iKnlExty;iIndy++)
		for (iIndx=0;iIndx<iKnlExtx;iIndx++)
		{
		    fDsqu = (iIndx-iKnlExth)*(iIndx-iKnlExth) + (iIndy-iKnlExth)*(iIndy-iKnlExth) + (iIndz-iKnlExth)*(iIndz-iKnlExth);
		    fDsqu = exp((60.0/2.0)*log(fDsqu));
		    fDval = fKernelAmpl * (1.0 - fDsqu * fBvalue);
		    if (fDval<0)
			fDval = 0;
		    *(pKnlPhi + g1idz(iIndz,iIndy,iIndx)) = fDval;
		}

	break;
    }

    int iOutExtx;
    int iOutExty;
    int iOutExtz;
    Real64 fGridx;
    Real64 fGridy;
    Real64 fGridz;
    int iOutNVox;
    Real64* pOutPhi;
    int iStrExtxy;
    int iX, iY, iZ;
    unsigned long iIndv;

    svt_volume<Real64>* pVol = new svt_volume<Real64>;

    Real64 fMinxG = fWidth * floor( ((getMinXAtomCoord()+m_oUncenter.x())*1.0E10) / fWidth );
    Real64 fMinyG = fWidth * floor( ((getMinYAtomCoord()+m_oUncenter.y())*1.0E10) / fWidth );
    Real64 fMinzG = fWidth * floor( ((getMinZAtomCoord()+m_oUncenter.z())*1.0E10) / fWidth );

    cout << "svt_pdb_blur> Uncenter: " << m_oUncenter << endl;

    // convolve and write output
    switch (iKernelType)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:

	printf ("svt_pdb_blur> Convolving lattice with kernel... \n");

	// allocate output density map
	iOutExtx = (int)(ceil((fMaxx-fMinx)/fWidth)) + 2*iKnlExth + 2*iMargin + 1;
	iOutExty = (int)(ceil((fMaxy-fMiny)/fWidth)) + 2*iKnlExth + 2*iMargin + 1;
	iOutExtz = (int)(ceil((fMaxz-fMinz)/fWidth)) + 2*iKnlExth + 2*iMargin + 1;

	fGridx = fMinxG - ((iKnlExth+iMargin) * fWidth);
	fGridy = fMinyG - ((iKnlExth+iMargin) * fWidth);
	fGridz = fMinzG - ((iKnlExth+iMargin) * fWidth);

	iOutNVox = iOutExtx*iOutExty*iOutExtz;
	pOutPhi = new double[iOutNVox];

	if (pOutPhi == NULL)
	{
	    fprintf(stderr, "svt_pdb_blur> Error: Unable to satisfy memory allocation request [e.c. 60310]\n");
	    exit(60310);
	}

	for (iCount=0; iCount<iOutNVox; iCount++)
	    *(pOutPhi+iCount) = 0;

	iStrExtxy = iStrExtx * iStrExty;

	for (iCount=0; iCount<iStrNVox; iCount++)
	{
	    fDval = *(pStrPhi+iCount);

	    if (fDval != 0)
	    {
		iIndv = iCount;
		iZ = iIndv / iStrExtxy;
		iIndv -= (iZ * iStrExtxy);
		iY = iIndv / iStrExtx;
		iX = iIndv - (iY * iStrExtx);

		for (iIndz=0;iIndz<iKnlExtz;iIndz++)
		    for (iIndy=0;iIndy<iKnlExty;iIndy++)
			for (iIndx=0;iIndx<iKnlExtx;iIndx++)
			    *(pOutPhi+g3idz(iZ+iIndz,iY+iIndy,iX+iIndx)) += *(pKnlPhi+g1idz(iIndz,iIndy,iIndx)) * fDval;
	    }
	}

	pVol->setData( iOutExtx, iOutExty, iOutExtz, pOutPhi);
	pVol->setWidth( fWidth );
        pVol->setGrid( fGridx, fGridy, fGridz );
        pVol->normalize();

	delete[] pKnlPhi;
	delete[] pStrPhi;
	break;

    case 6:

	fGridx = fMinxG - (iMargin * fWidth);
	fGridy = fMinyG - (iMargin * fWidth);
	fGridz = fMinzG - (iMargin * fWidth);

	pVol->setData( iStrExtx, iStrExty, iStrExtz, pStrPhi );
	pVol->setWidth( fWidth );
        pVol->setGrid( fGridx, fGridy, fGridz );
        pVol->normalize();

	delete[] pKnlPhi;

	break;
    }

    return pVol;
}
