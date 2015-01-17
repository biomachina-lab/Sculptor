/***************************************************************************
                          sculptor_splitter
                          -----------------
    begin                : 04/30/2008
    author               : Jochen Heyd
    email                : sculptor@biomachina.org
 ***************************************************************************/


// sculptor includes
#include <sculptor_splitter.h>

/**
 * Constructor
 */
sculptor_splitter::sculptor_splitter( Qt::Orientation pOrient, QWidget* pParent )
    : QSplitter( pOrient, pParent )
{
    // as default, distribute space 50-50
    m_oSizes.push_back(1);
    m_oSizes.push_back(1);

    // sizes for collapsed vertical splitter: top widget gets all space, lower widget none
    m_oSizesCollapsedVertical.push_back(1);
    m_oSizesCollapsedVertical.push_back(0);

    // sizes for collapsed horizontal splitter: right widget gets all space, left widget none
    m_oSizesCollapsedHorizontal.push_back(0);
    m_oSizesCollapsedHorizontal.push_back(1);
}

/**
 * Destructor
 */
sculptor_splitter::~sculptor_splitter()
{
}

/**
 *
 */
bool sculptor_splitter::getCollapsed()
{
    if ( orientation() == Qt::Horizontal )
    {
        // horizontal: main splitter
        // here, collapsed means left widget collapsed (documentlist)
        if ( sizes()[0] == 0 )
            return true;
        else
            return false;
    }
    else
    {
        // vertical: left and right splitter
        // here, collapsed means lower widget collapsed (property dialog/log and lua)
        if ( sizes()[1] == 0 )
            return true;
        else
            return false;
    }
}

/**
 *
 */
void sculptor_splitter::setCollapsed(bool bCollapse)
{
    if ( bCollapse == getCollapsed() )
        return;

    if ( bCollapse )
    {
        // before collapsing, store the current sizes
        if ( sizes().size() == 2 )
            m_oSizes = sizes();

        if ( orientation() == Qt::Vertical )
            setSizes(m_oSizesCollapsedVertical);
        else
            setSizes(m_oSizesCollapsedHorizontal);
    }
    else
    {
        setSizes( m_oSizes );
    }
}
