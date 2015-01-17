/***************************************************************************
                          sculptor_dlg_about.ccp
                          -----------------
    begin                : 11.02.2010
    author               : Zbigniew Starosolski
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

// sculptor includes
#include <sculptor_version.h>
#include <sculptor_dlg_about.h>
// qt4 includes
//#include QString;


/**
 * Constructor
 */
dlg_about::dlg_about(QWidget* pParent)
    : QDialog( pParent )
{
    setupUi(this);

    // compute the version string and set it in the label
    QString oString = QString("<p align=\"center\"><b>Version: ") + QString( SCULPTOR_VERSION ) + QString("</b>");
#ifdef __DATE__
    oString += "<br><font size=\"-4\" face=\"Arial\">Build: ";
    oString += __DATE__;
#ifdef __TIME__
    oString += " (";
    oString += __TIME__;
    oString += ")<br>";
#endif
    oString += "</font></p>";
#endif

    m_pTextVersion->setText( oString );


    // if user hits OK button, check if the string has correct PDB ID format
    connect( m_pClose, SIGNAL(clicked()), this, SLOT(hide()) );
}

/**
 * Destructor
 */

dlg_about::~dlg_about()
{
}
