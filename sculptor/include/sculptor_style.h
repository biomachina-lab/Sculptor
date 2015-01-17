/***************************************************************************
                          sculptor_style
                          --------------
    begin                : 15.01.2009
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_STYLE_H
#define SCULPTOR_STYLE_H

#include <qpalette.h>

#ifndef QT_NO_STYLE_WINDOWS

#include <qwindowsstyle.h>

class sculptor_style : public QWindowsStyle
{
public:
    sculptor_style();

    virtual int pixelMetric ( PixelMetric metric, const QWidget * widget = 0 ) const;

    void polish( QApplication*);
    void unPolish( QApplication*);

    void polish( QWidget* );
    void unPolish( QWidget* );

private:

    QPalette m_oOldPalette;
    QPalette m_oNewPalette;
};

#endif

#endif
