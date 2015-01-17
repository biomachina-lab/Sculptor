/*-*-c++-*-****************************************************************
                          sculptor_splitter
                          -----------------
    begin                : 04/30/2008
    author               : Jochen Heyd
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SCULPTOR_SPLITTER_H
#define SCULPTOR_SPLITTER_H

// qt4 includes
#include <QSplitter>

/**
  * The sculptor_splitter inherits QSplitter.  The added funtionality allows the splitter to be
  * collapsed and uncollapsed by function calls.
  * \brief  An adapted QSplitter.
  * \author Jochen Heyd
  */
class sculptor_splitter : public QSplitter
{

    // list to remember sizes before collapsing
    QList<int> m_oSizes;
    // sizes for collapsed vertical splitter
    QList<int> m_oSizesCollapsedVertical;
    // sizes for collapsed horizontal splitter
    QList<int> m_oSizesCollapsedHorizontal;

public:

    /**
     * Constructor
     */
    sculptor_splitter( Qt::Orientation, QWidget* parent = 0 );

    /**
     * Destructor
     */
    virtual ~sculptor_splitter();

    /**
     * Is the splitter collapsed?
     * \return true if collapsed
     */
    bool getCollapsed();

    /**
     * (un)collapse splitter
     * \param bCollapse true to collapse, false to uncollapse
     */
    void setCollapsed(bool bCollapse = true);
};

#endif
