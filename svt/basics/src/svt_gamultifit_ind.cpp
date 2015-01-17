#include<svt_gamultifit_ind.h>

///////////////////////////////////////////////////////////////////////////////
// The Euler angle class
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 */
svt_eulerAngles::svt_eulerAngles( )
{
    m_pAngles = NULL;
    m_iAngles = 0;
};

/**
 * Destructor
 */
svt_eulerAngles::~svt_eulerAngles( )
{
};

/**
 * Fill the angle table.
 * \param fPsiFrom   lower boundary of the psi angles (in degrees)
 * \param fPsiTo     upper boundary of the psi angles (in degrees)
 * \param fThetaFrom lower boundary of the theta angles (in degrees)
 * \param fThetaTo   upper boundary of the theta angles (in degrees)
 * \param fPhiFrom   lower boundary of the phi angles (in degrees)
 * \param fPhiTo     upper boundary of the phi angles (in degrees)
 * \param fDelta     angular step size in degrees
 */
void svt_eulerAngles::initTable( double fPsiFrom, double fPsiTo, double fThetaFrom, double fThetaTo, double fPhiFrom, double fPhiTo, double fDelta )
{
    if (m_iAngles != 0)
    {
        free(m_pAngles);
        m_pAngles = NULL;
    }
    //m_iAngles = proportionalEulerAngles( fPsiFrom, fPsiTo, fThetaFrom, fThetaTo, fPhiFrom, fPhiTo, fDelta );
    m_iAngles = proportionalEulerAnglesThetaOrdering( fPsiFrom, fPsiTo, fThetaFrom, fThetaTo, fPhiFrom, fPhiTo, fDelta );
};

/**
 * remove angles from table
 * WARNING: not fully tested
 * \param fPsiFrom   lower boundary of the psi angles (in degrees)
 * \param fPsiTo     upper boundary of the psi angles (in degrees)
 * \param fThetaFrom lower boundary of the theta angles (in degrees)
 * \param fThetaTo   upper boundary of the theta angles (in degrees)
 * \param fPhiFrom   lower boundary of the phi angles (in degrees)
 * \param fPhiTo     upper boundary of the phi angles (in degrees)
 */
void svt_eulerAngles::removeAngles( double fPsiFrom, double fPsiTo, double fThetaFrom, double fThetaTo, double fPhiFrom, double fPhiTo )
{
    if ( m_pAngles == NULL)
    {
         svtout << "ERROR: the table was not yet initialized!" << endl;
    }
    
    int iNewCount = 0;
    Real64 fPhi, fTheta, fPsi;
    
    //count how many angles will remain in the table
    for (unsigned iIndex = 0; iIndex< m_iAngles; iIndex++)
    {
        fPhi = rad2deg( getPhi( iIndex ));
        fTheta = rad2deg( getTheta( iIndex ));
        fPsi = rad2deg( getPsi( iIndex ));
        if ((fPhi <= fPhiFrom || fPhi > fPhiTo ) && (fTheta <= fThetaFrom || fTheta > fThetaTo ) && (fPsi <= fPsiFrom || fPsi > fPsiTo ))
            iNewCount++;
    }
    
    // allocate memory for the new angles
    
    float *pAngles = (float *) malloc(iNewCount * 3 * sizeof(float));
    if (pAngles == NULL)
        svtout << "Error: svt_removeAngles, error in memory allocation..." << endl;
    
    // add am
    unsigned int j = 0;
    for (unsigned iIndex = 0; iIndex< m_iAngles; iIndex++)
    {
        fPhi = rad2deg( getPhi( iIndex ));
        fTheta = rad2deg( getTheta( iIndex ));
        fPsi = rad2deg( getPsi( iIndex ));
                
        if ((fPhi <= fPhiFrom || fPhi > fPhiTo ) && (fTheta <= fThetaFrom || fTheta > fThetaTo ) && (fPsi <= fPsiFrom || fPsi > fPsiTo ))
        {
            *(pAngles+j*3+0)=(float)deg2rad(fPsi);
            *(pAngles+j*3+1)=(float)deg2rad(fTheta);
            *(pAngles+j*3+2)=(float)deg2rad(fPhi);
            
            j++;
        }
    }
    
    //delete all angles
    if (m_iAngles != 0)
    {
        free(m_pAngles);
        m_pAngles = NULL;
    }
    
    //set the new table
    m_iAngles = iNewCount;
    m_pAngles = pAngles;
    
    
    
};

/**
 * add the oppsite angles: if angle = (psi, theta, phi) 
 * add (-psi, theta, phi) (psi, theta, -phi) (-psi, theta, -phi)
 * eq with (2pi-psi, theta, phi) (psi, theta, 2pi-phi) (2pi-psi, theta, 2pi-phi)
 */
void svt_eulerAngles::addOppositeAngles( )
{   
     if ( m_pAngles == NULL)
    {
         svtout << "ERROR: the table was not yet initialized!" << endl;
    }
    
    
    // allocate memory for the new angles
    int iNewCount = 4*m_iAngles;
    float *pAngles = (float *) malloc(iNewCount * 3 * sizeof(float));
    
    if (pAngles == NULL)
        svtout << "Error: svt_removeAngles, error in memory allocation..." << endl;
    
    
    Real64 fPhi, fTheta, fPsi;
    unsigned int j = 0;
    
    for (unsigned iIndex = 0; iIndex< m_iAngles; iIndex++)
    {
        fPhi = rad2deg( getPhi( iIndex ));
        fTheta = rad2deg( getTheta( iIndex ));
        fPsi = rad2deg( getPsi( iIndex ));
        
        *(pAngles+j*3+0)=(float)deg2rad(fPsi);
        *(pAngles+j*3+1)=(float)deg2rad(fTheta);
        *(pAngles+j*3+2)=(float)deg2rad(fPhi);
        j++;
        
        *(pAngles+j*3+0)=(float)deg2rad(2*PI-fPsi);
        *(pAngles+j*3+1)=(float)deg2rad(fTheta);
        *(pAngles+j*3+2)=(float)deg2rad(fPhi);
        j++;
        
        *(pAngles+j*3+0)=(float)deg2rad(fPsi);
        *(pAngles+j*3+1)=(float)deg2rad(fTheta);
        *(pAngles+j*3+2)=(float)deg2rad(2*PI-fPhi);
        j++;
        
        *(pAngles+j*3+0)=(float)deg2rad(2*PI-fPsi);
        *(pAngles+j*3+1)=(float)deg2rad(fTheta);
        *(pAngles+j*3+2)=(float)deg2rad(2*PI-fPhi);
        j++;
        
    }
    
    m_iAngles = iNewCount;
    m_pAngles = pAngles;
    
};


/**
 * How many angles do we have stored in total?
 * \return unsigned long with the number of angles
 */
unsigned long svt_eulerAngles::getAngleCount()
{
    return m_iAngles;
};

/**
 * The angles follow the common PTP (Goldstein) convention. This function returns the Psi angle.
 * \param iIndex index into the table of angles
 * \return psi angle
 */
float svt_eulerAngles::getPsi( unsigned long iIndex )
{
    if (iIndex >= m_iAngles)
    {
        svtout << "ERROR: iIndex > m_iAngles: " << iIndex << " > " << m_iAngles << endl;
        return *(m_pAngles+(m_iAngles-1)*3+0);
    } else
        return *(m_pAngles+iIndex*3+0);
};

/**
 * The angles follow the common PTP (Goldstein) convention. This function returns the Theta angle.
 * \param iIndex index into the table of angles
 * \return theta angle
 */
float svt_eulerAngles::getTheta( unsigned long iIndex )
{
    if (iIndex >= m_iAngles)
    {
        svtout << "ERROR: iIndex > m_iAngles: " << iIndex << " > " << m_iAngles << endl;
        return *(m_pAngles+(m_iAngles-1)*3+1);
    } else
        return *(m_pAngles+iIndex*3+1);
};

/**
 * The angles follow the common PTP (Goldstein) convention. This function returns the Phi angle.
 * \param iIndex index into the table of angles
 * \return phi angle
 */
float svt_eulerAngles::getPhi( unsigned long iIndex )
{
    if (iIndex >= m_iAngles)
    {
        svtout << "ERROR: iIndex > m_iAngles: " << iIndex << " > " << m_iAngles << endl;
        return *(m_pAngles+(m_iAngles-1)*3+2);
    } else
        return *(m_pAngles+iIndex*3+2);
};

/**
 * This function precomputes the angle table. It is called automatically in the constructor.
 */
unsigned long svt_eulerAngles::proportionalEulerAngles(double fPsiFrom, double fPsiTo, double fThetaFrom, double fThetaTo, double fPhiFrom, double fPhiTo, double fDelta)
{
    double psi, theta, phi;
    double psi_ang_dist, psi_real_dist;
    double theta_real_dist, phi_real_dist;
    double psi_steps, theta_steps, phi_steps;
    double psi_range, theta_range, phi_range;
    unsigned long u, j;

    unsigned long iCount;

    if (( fPsiTo   - fPsiFrom   )/fDelta < -1  ||
        ( fThetaTo - fThetaFrom )/fDelta < -1  ||
        ( fPhiTo   - fPhiFrom   )/fDelta < -1 )
    {
        svtout << "Error: svt_proportionalEulerAngles(), ranges wrong..." << endl;
        return 0;
    }

    psi_range   = fPsiTo   - fPsiFrom;
    theta_range = fThetaTo - fThetaFrom;
    phi_range   = fPhiTo   - fPhiFrom;

    // Use rounding instead of CEIL to avoid rounding up at x.001
    phi_steps       = rint((phi_range/fDelta) + 0.499);
    phi_real_dist   = phi_range / phi_steps;

    theta_steps     = rint((theta_range/fDelta) + 0.499);
    theta_real_dist = theta_range / theta_steps;

    // Computes the number of angles that will be generated
    u=0;
    for (phi=fPhiFrom; phi < 360.0 && phi <=fPhiTo;  phi+=phi_real_dist)
    {
        for (theta=fThetaFrom; theta <= 180.0 && theta <=fThetaTo;  theta+=theta_real_dist)
        {
            if (theta == 0.0 || theta == 180.0)
                psi_steps = 1;
            else
                psi_steps = rint(360.0*cos(deg2rad(90.0-theta))/fDelta);

            psi_ang_dist  = 360.0/psi_steps;
            psi_real_dist = psi_range / (ceil(psi_range/psi_ang_dist));

            for (psi=fPsiFrom; psi < 360.0 && psi <= fPsiTo;  psi+=psi_real_dist)
                u++;
        }
    }

    iCount = u;

    // allocate memory
    m_pAngles = (float *) malloc(iCount * 3 * sizeof(float));
    if (m_pAngles == NULL)
        svtout << "Error: svt_proportionalEulerAngles(), error in memory allocation..." << endl;

    j = 0;
    for (phi=fPhiFrom; phi < 360.0 && phi <= fPhiTo;  phi+=phi_real_dist)
    {
        for (theta=fThetaFrom; theta <= 180.0 && theta <= fThetaTo;  theta+=theta_real_dist)
        {
            if (theta == 0.0 || theta == 180.0)
                psi_steps = 1;
            else
                psi_steps = rint(360.0*cos(deg2rad(90.0-theta))/fDelta);

            psi_ang_dist  = 360.0/psi_steps;
            psi_real_dist = psi_range / (ceil(psi_range/psi_ang_dist));

            for (psi=fPsiFrom; psi < 360.0 && psi <= fPsiTo;  psi+=psi_real_dist)
            {

                *(m_pAngles+j*3+0)=(float)deg2rad(psi);
                *(m_pAngles+j*3+1)=(float)deg2rad(theta);
                *(m_pAngles+j*3+2)=(float)deg2rad(phi);
                
                //svtout << " [" << phi << " , " << theta << " , " << psi << "]" <<  endl;
                j++;
            }
        }
    }

    return iCount;
};


/**
 * This function precomputes the angle table. It is called automatically in the constructor.
 */
unsigned long svt_eulerAngles::proportionalEulerAnglesThetaOrdering(double fPsiFrom, double fPsiTo, double fThetaFrom, double fThetaTo, double fPhiFrom, double fPhiTo, double fDelta)
{
    double psi, theta, phi;
    double psi_ang_dist, psi_real_dist;
    double theta_real_dist, phi_real_dist;
    double psi_steps, theta_steps, phi_steps;
    double psi_range, theta_range, phi_range;
    unsigned long u, j;

    unsigned long iCount;

    if (( fPsiTo   - fPsiFrom   )/fDelta < -1  ||
        ( fThetaTo - fThetaFrom )/fDelta < -1  ||
        ( fPhiTo   - fPhiFrom   )/fDelta < -1 )
    {
        svtout << "Error: svt_proportionalEulerAngles(), ranges wrong..." << endl;
        return 0;
    }

    psi_range   = fPsiTo   - fPsiFrom;
    theta_range = fThetaTo - fThetaFrom;
    phi_range   = fPhiTo   - fPhiFrom;

    // Use rounding instead of CEIL to avoid rounding up at x.001
    phi_steps       = rint((phi_range/fDelta) + 0.499);
    phi_real_dist   = phi_range / phi_steps;

    theta_steps     = rint((theta_range/fDelta) + 0.499);
    theta_real_dist = theta_range / theta_steps;

    // Computes the number of angles that will be generated
    u=0;
    for (theta=fThetaFrom; theta <= 180.0 && theta <=fThetaTo;  theta+=theta_real_dist)
    {
	if (theta == 0.0 || theta == 180.0)
	    psi_steps = 1;
	else
	    psi_steps = rint(360.0*cos(deg2rad(90.0-theta))/fDelta);

	for (phi=fPhiFrom; phi < 360.0 && phi <=fPhiTo;  phi+=phi_real_dist)
	{
	    psi_ang_dist  = 360.0/psi_steps;
            psi_real_dist = psi_range / (ceil(psi_range/psi_ang_dist));

            for (psi=fPsiFrom; psi < 360.0 && psi <= fPsiTo;  psi+=psi_real_dist)
                u++;
        }
    }

    iCount = u;

    // allocate memory
    m_pAngles = (float *) malloc(iCount * 3 * sizeof(float));
    if (m_pAngles == NULL)
        svtout << "Error: svt_proportionalEulerAngles(), error in memory allocation..." << endl;

    j = 0;
    for (theta=fThetaFrom; theta <= 180.0 && theta <= fThetaTo;  theta+=theta_real_dist)
    {
	if (theta == 0.0 || theta == 180.0)
	    psi_steps = 1;
	else
	    psi_steps = rint(360.0*cos(deg2rad(90.0-theta))/fDelta);

	for (phi=fPhiFrom; phi < 360.0 && phi <= fPhiTo;  phi+=phi_real_dist)
	    {

            psi_ang_dist  = 360.0/psi_steps;
            psi_real_dist = psi_range / (ceil(psi_range/psi_ang_dist));

            for (psi=fPsiFrom; psi < 360.0 && psi <= fPsiTo;  psi+=psi_real_dist)
            {

                *(m_pAngles+j*3+0)=(float)deg2rad(psi);
                *(m_pAngles+j*3+1)=(float)deg2rad(theta);
                *(m_pAngles+j*3+2)=(float)deg2rad(phi);
                
                //svtout << " [" << phi << " , " << theta << " , " << psi << "]" <<  endl;
                j++;
            }
        }
    }

    return iCount;
};

/**
 * searches the angles that are within fAngleRange from the angle indicated by iIndex
 * \param iIndex the reference angle around which to search
 * \param fRange how far away from the reference
 * \return a list of indexes that indicates the angles in the angle list that are close to the angle idicated by iIndex
 */
vector<long unsigned int> svt_eulerAngles::getNeighborAngles( unsigned long int iIndex, Real64 fAngleRange )
{
  
    vector<long unsigned int> oVec;
    if (iIndex>=m_iAngles)
    {
	svtout << "ERROR: iIndex > m_iAngles: " << iIndex << " > " << m_iAngles << endl;
	return oVec;
    }

    Real64 fPhiRef	= getPhi    (iIndex);
    Real64 fThetaRef	= getTheta  (iIndex);
    Real64 fPsiRef	= getPsi    (iIndex);
    //svtout << "DEBU : " << fPhiRef << " , " <<  fThetaRef << " , " << fPsiRef <<  endl; 
  
    Real64 fPhi, fTheta, fPsi;
    for (long unsigned int iAngle=0; iAngle<m_iAngles; iAngle++)
    {
	fPhi		= getPhi    (iAngle);
	fTheta		= getTheta  (iAngle);
	fPsi		= getPsi    (iAngle);

	if ( abs(fPhi-fPhiRef)<fAngleRange && abs(fTheta-fThetaRef)<fAngleRange && abs(fPsi-fPsiRef)<fAngleRange )
	{
	    //svtout << "DEBUG: " << fPhi << " , " <<  fTheta << " , " << fPsi << endl; 
	    oVec.push_back( iAngle );
	}

    }

    return oVec;
};


///////////////////////////////////////////////////////////////////////////////
// svt_gamultifit_ind class
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 */
svt_gamultifit_ind::svt_gamultifit_ind():svt_ga_ind(),
m_bWrote(false)
{
}

/**
 * Destructor
 */
svt_gamultifit_ind::~svt_gamultifit_ind()
{
}

/**
 * create the coarse phenotype (equivalent simple pdb) but don't fill yet atomic coordinates
 * \param number of units
 */
void svt_gamultifit_ind::buildCoarsePhenotype( unsigned int iNoUnits )
{
    svt_vector4<Real64> oVec;
    oVec.x(0.0f); oVec.y(0.0f); oVec.z(0.0f);
    while ( (Real64)m_oCoarsePhenotype.size()/4.0 <= iNoUnits ) 
    {
	for (unsigned int i=0; i<4; i++)
	    m_oCoarsePhenotype.push_back( oVec );
    }
};

/**
 * update the coarse phenotype for the given unit 
 * \param transformation matrix for that unit
 * \param number of units
 */
void svt_gamultifit_ind::updateCoarsePhenotype(svt_ga_mat oMat, unsigned int iUnit)
{
    if (m_oCoarsePhenotype.size() < iUnit*4+3 )
	buildCoarsePhenotype( iUnit );

    m_oCoarsePhenotype[iUnit*4 + 0].x( oMat[0][0] + oMat[0][3] );
    m_oCoarsePhenotype[iUnit*4 + 0].y( oMat[1][0] + oMat[1][3] );
    m_oCoarsePhenotype[iUnit*4 + 0].z( oMat[2][0] + oMat[2][3] );
    
    m_oCoarsePhenotype[iUnit*4 + 1].x( oMat[0][1] + oMat[0][3] );
    m_oCoarsePhenotype[iUnit*4 + 1].y( oMat[1][1] + oMat[1][3] );
    m_oCoarsePhenotype[iUnit*4 + 1].z( oMat[2][1] + oMat[2][3] );
 
    m_oCoarsePhenotype[iUnit*4 + 2].x( oMat[0][2] + oMat[0][3] );
    m_oCoarsePhenotype[iUnit*4 + 2].y( oMat[1][2] + oMat[1][3] );
    m_oCoarsePhenotype[iUnit*4 + 2].z( oMat[2][2] + oMat[2][3] );
 
    m_oCoarsePhenotype[iUnit*4 + 3].x( oMat[0][3] );
    m_oCoarsePhenotype[iUnit*4 + 3].y( oMat[1][3] );
    m_oCoarsePhenotype[iUnit*4 + 3].z( oMat[2][3] );
}


/**
 * get the coarse phenotype
 */
svt_point_cloud_pdb< svt_vector4<Real64 > > svt_gamultifit_ind::getCoarsePhenotype()
{
    svt_point_cloud_pdb<svt_vector4<Real64> > oCoarsePhenotype;    
    svt_point_cloud_atom oAtom;

    for (unsigned int iIndex=0; iIndex < (Real64)m_oCoarsePhenotype.size()/4.0; iIndex++ ) 
	for (unsigned int i=0; i<4; i++)
	    oCoarsePhenotype.addAtom(oAtom, m_oCoarsePhenotype[iIndex*4+i] );

    return oCoarsePhenotype;
};

/**
 * calculate the distance between two individuals
 * \param rOther reference to the other individual
 * \return vector distance between the two gene-vectors
 */
Real64 svt_gamultifit_ind::distance( svt_gamultifit_ind& rOther )
{
    Real64 fDist = 0.0;
    
    if (m_oCoarsePhenotype.size()!= rOther.m_oCoarsePhenotype.size() || m_oCoarsePhenotype.size()==0)
    {
	svtout << "Can not compute distance" << endl;
	return 0.0;
    }

    for(unsigned int i=0; i<m_oCoarsePhenotype.size(); i++)
        fDist += m_oCoarsePhenotype[i].distanceSq(rOther.m_oCoarsePhenotype[i]);

    return sqrt( fDist/(Real64)((Real64)m_oCoarsePhenotype.size()/4.0f) );
};

/**
 * set Wrote on disk
 * \param bWrote whether it was already wrote on disk
 */
void svt_gamultifit_ind::setWrote( bool bWrote )
{
    m_bWrote = bWrote;
};

/**
 * get Wrote on disk
 * \return bWrote whether it was already wrote on disk
 */
bool svt_gamultifit_ind::getWrote()
{
    return m_bWrote;
};


