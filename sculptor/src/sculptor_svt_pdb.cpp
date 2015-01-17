/***************************************************************************
                          sculptor_svt_pdb
                          ----------------
    begin                : 11/08/2010
    author               : Zbigniew Starosolski
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#include <sculptor_svt_pdb.h>


/**
 *
 */
sculptor_svt_pdb::sculptor_svt_pdb(char* pFileA, int iTypeA, char* pFileB, int iTypeB, svt_pos* pPos, svt_properties* pProp)
    : svt_pdb( pFileA, iTypeA, pFileB, iTypeB, pPos, pProp ),
      m_pWidget( NULL )
{
}

/**
 *
 */
sculptor_svt_pdb::sculptor_svt_pdb( svt_point_cloud_pdb< svt_vector4<Real64 > >& rPDB )
    : svt_pdb( rPDB ),
      m_pWidget( NULL )
{
}

/**
 *
 */
sculptor_svt_pdb::sculptor_svt_pdb()
    : svt_pdb(),
      m_pWidget( NULL )
{
}

/**
 *
 */
sculptor_svt_pdb::sculptor_svt_pdb( svt_pdb& rPDB )
    : svt_pdb( rPDB ),
      m_pWidget( NULL )
{
}

/**
 *
 */
void sculptor_svt_pdb::prepareContainer()
{
    if (m_pWidget != NULL)
        m_pWidget->setEnabled( FALSE );

    svt_pdb::prepareContainer();

    if (m_pWidget != NULL)
    {
        m_pWidget->setEnabled( TRUE );
        m_pWidget = NULL;
    }
}

/**
 *
 */
void sculptor_svt_pdb::blockWidget( QWidget* pWidget )
{
    m_pWidget = pWidget;
}
