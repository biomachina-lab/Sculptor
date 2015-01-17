/***************************************************************************
                          svt_pdb_surface  -  description
                             -------------------
    begin                : 28.8.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_pdb_surface.h>

#include <math.h>

#define SURF_STEP 0.125E10
#define INV_SURF_STEP 0.125E-10

/**
 * Constructor
 * \param pPDB pointer to the svt_pdb object containing the molecule
 */
svt_pdb_surface::svt_pdb_surface(svt_pdb* pPDB)
{
    m_pPDB = pPDB;

<<<<<<< svt_pdb_surface.cpp
    m_fMinX = pPDB->getMinXAtomCoord() - 3.0E-10;
    m_fMinY = pPDB->getMinYAtomCoord() - 3.0E-10;
    m_fMinZ = pPDB->getMinZAtomCoord() - 3.0E-10;
    m_fMaxX = pPDB->getMaxXAtomCoord() + 3.0E-10;
    m_fMaxY = pPDB->getMaxYAtomCoord() + 3.0E-10;
    m_fMaxZ = pPDB->getMaxZAtomCoord() + 3.0E-10;

    m_iSizeX = (int)((m_fMaxX - m_fMinX) * 5.0E10);
    m_iSizeY = (int)((m_fMaxY - m_fMinY) * 5.0E10);
    m_iSizeZ = (int)((m_fMaxZ - m_fMinZ) * 5.0E10);
=======
    m_fMinX = pPDB->getMinXAtomCoord() - 5.0E-10;
    m_fMinY = pPDB->getMinYAtomCoord() - 5.0E-10;
    m_fMinZ = pPDB->getMinZAtomCoord() - 5.0E-10;
    m_fMaxX = pPDB->getMaxXAtomCoord() + 5.0E-10;
    m_fMaxY = pPDB->getMaxYAtomCoord() + 5.0E-10;
    m_fMaxZ = pPDB->getMaxZAtomCoord() + 5.0E-10;

    cout << "fMaxX:" << m_fMaxX << endl;
    cout << "fMinX:" << m_fMinX << endl;

    m_iSizeX = (int)((m_fMaxX - m_fMinX) / INV_SURF_STEP);
    m_iSizeY = (int)((m_fMaxY - m_fMinY) / INV_SURF_STEP);
    m_iSizeZ = (int)((m_fMaxZ - m_fMinZ) / INV_SURF_STEP);
>>>>>>> 1.2

    cout << "m_iSizeX: " << m_iSizeX;
    cout << " m_iSizeY: " << m_iSizeY;
    cout << " m_iSizeZ: " << m_iSizeZ << endl;

    createAtomCubes();

    //setData(m_iSizeX, m_iSizeY, m_iSizeZ, NULL);
};

/**
 * get a value of the volume
 * \param iX x coordinate
 * \param iY y coordinate
 * \param iZ z coordinate
 * \return value
 */
float svt_pdb_surface::getValue(int iX, int iY, int iZ)
{
<<<<<<< svt_pdb_surface.cpp
<<<<<<< svt_pdb_surface.cpp
    int iAtoms = m_pPDB->getAtomsNum();
    int i;
    float fX = (5.0E-10 * (float)(iX)) + m_fMinX;
    float fY = (5.0E-10 * (float)(iY)) + m_fMinY;
    float fZ = (5.0E-10 * (float)(iZ)) + m_fMinZ;
    float fDistX, fDistY, fDistZ, fDist;
    float fMinDist = 10000.0f;
    svt_atom* pAtom;
=======
    return svt_volume<float>::getValue(iX, iY, iZ);
>>>>>>> 1.2
=======
    int i;
    float fX = (INV_SURF_STEP * (float)(iX)) + m_fMinX;
    float fY = (INV_SURF_STEP * (float)(iY)) + m_fMinY;
    float fZ = (INV_SURF_STEP * (float)(iZ)) + m_fMinZ;
    float fDistX, fDistY, fDistZ, fDist;
    float fMinDist = 10000.0f;
>>>>>>> 1.3

    svt_array<int>* pCube = getAtomCube(iX, iY, iZ);
    int iAtoms = pCube->numElements();
    svt_atom* pAtom;

    for(i=0;i<iAtoms;i++)
    {
        pAtom = m_pPDB->getAtom(pCube->getAt(i));

        fDistX = pAtom->getAtomPosX() - fX;
        fDistY = pAtom->getAtomPosY() - fY;
        fDistZ = pAtom->getAtomPosZ() - fZ;
        fDist = sqrt((fDistX*fDistX) + (fDistY*fDistY) + (fDistZ*fDistZ));

        if (fDist > pAtom->getVDWRadius())
            fDist -= pAtom->getVDWRadius();
        else
            fDist = pAtom->getVDWRadius();

        if (fDist < fMinDist)
            fMinDist = fDist;
    }

    return fMinDist;
}

/**
 * get the atom cube for iX and iY
 * \param iX x coordinate inside the complete volume
 * \param iY y coordinate inside the complete volume
 * \return the atom cube
 */
svt_array<int>* svt_pdb_surface::getAtomCube(int iX, int iY, int iZ)
{
    Real32 fVolX = (float)(INV_SURF_STEP * iX);
    Real32 fVolY = (float)(INV_SURF_STEP * iY);
    Real32 fVolZ = (float)(INV_SURF_STEP * iZ);

    int iCubeX = (int)(fVolX / m_fCubeSize);
    int iCubeY = (int)(fVolY / m_fCubeSize);
    int iCubeZ = (int)(fVolZ / m_fCubeSize);

    int iCubeIndex = (iCubeZ * m_iCubeXY) + (iCubeY * m_iCubeX) + iCubeX;

    if (iCubeIndex < 0)
        iCubeIndex = 0;
    //if (iCubeIndex > m_pCube->numElements())
    //    iCubeIndex = m_pCube->numElements();

    return &m_pCube[iCubeIndex];
}

/**
 * create the atom cubes
 */
void svt_pdb_surface::createAtomCubes()
{
    m_fCubeSize = 5.0E-10;

    m_iCubeX = (int)((m_fMaxX - m_fMinX) / m_fCubeSize)+1;
    m_iCubeY = (int)((m_fMaxY - m_fMinY) / m_fCubeSize)+1;
    m_iCubeXY = m_iCubeX * m_iCubeY;
    m_iCubeZ = (int)((m_fMaxZ - m_fMinZ) / m_fCubeSize)+1;

    cout << "svt_pdb_surface: creating atom cubes: " << m_iCubeX << "," << m_iCubeY << "," << m_iCubeZ << endl;

    int num = m_iCubeX*m_iCubeY*m_iCubeZ;
    m_pCube = new svt_array<int>[num];
    int atom_x, atom_y, atom_z, atom_index;

    // sorting all atoms into the grid
    int iAtomNum = m_pPDB->getAtomsNum();
    svt_atom* pAtom;
    int i;
    for(i=0;i<iAtomNum;i++)
    {
        pAtom = m_pPDB->getAtom(i);

        atom_x = (int)((pAtom->getAtomPosX() - m_fMinX) / m_fCubeSize);
        atom_y = (int)((pAtom->getAtomPosY() - m_fMinY) / m_fCubeSize);
        atom_z = (int)((pAtom->getAtomPosZ() - m_fMinZ) / m_fCubeSize);
        atom_index = (atom_z * m_iCubeXY) + (atom_y * m_iCubeX) + atom_x;
        m_pCube[atom_index].addElement(i);
    }
}
