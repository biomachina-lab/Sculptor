/***************************************************************************
                          svt_pdb_surface  -  description
                             -------------------
    begin                : 28.8.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_PDB_SURFACE
#define SVT_PDB_SURFACE

#include <svt_core.h>
#include <svt_pdb.h>
#include <svt_volume_mc_cline.h>

/** Surface generator for molecules
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_pdb_surface : public svt_volume_mc_cline<float>
{
protected:
    svt_pdb* m_pPDB;

    float m_fMinX;
    float m_fMinY;
    float m_fMinZ;
    float m_fMaxX;
    float m_fMaxY;
    float m_fMaxZ;

    // cube variables
    // the cubes
    svt_array<int> *m_pCube;
    // size of the complete volume
    int m_iCubeX;
    int m_iCubeY;
    int m_iCubeZ;
    int m_iCubeXY;
    // size of one cube (in atom coords)
    Real32 m_fCubeSize;

public:
    /**
     * Constructor
     * \param pPDB pointer to the  svt_pdb object containing the molecule
     */
    svt_pdb_surface(svt_pdb* pPDB);

    /**
     * get a value of the volume
     * \param iX x coordinate
     * \param iY y coordinate
     * \param iZ z coordinate
     * \return value
     */
    virtual float getValue(int iX, int iY, int iZ);

private:

    /**
     * create the atom cubes
     */
    void createAtomCubes();

    /**
     * get the atom cube for iX and iY
     * \param iX x coordinate inside the complete volume
     * \param iY y coordinate inside the complete volume
     * \return the atom cube
     */
    svt_array<int>* getAtomCube(int iX, int iY, int iZ);
};

#endif
