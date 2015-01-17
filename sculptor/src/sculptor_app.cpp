/***************************************************************************
                          sculptor_app.cpp
			  -------------------
    begin                : 
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_init.h>
// sculptor includes
#include <sculptor_app.h>
//#include <sculptor_style.h> QT3FIXME (see below)

sculptor_app::sculptor_app(int* argc, char** argv[])
    : QApplication(*argc, *argv)
{
}

void sculptor_app::quit()
{
    QApplication::quit();
    svt_exit(0);
}

/**
 * Create and set a new style
 * \param style string with the style name, e.g. Sculptor or Windows
 */
void sculptor_app::makeStyle(const QString &style)
{
//     if (style == "Sculptor") {                    QT3FIXME
// #ifndef QT_NO_STYLE_WINDOWS                       with Qt4, this old style compatible to Qt3
//         qApp->setStyle( new sculptor_style );     probably needs a lot of attentioin, so
// #endif                                            it is commented out for now because not vital anyway
//     } else
        qApp->setStyle(style);

    m_oStyleName = style;
}

/**
 * Get the name of the current style
 */
QString sculptor_app::getStyleName()
{
    return m_oStyleName;
}
