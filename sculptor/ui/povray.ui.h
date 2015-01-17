/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

#include <qfiledialog.h>
#include <qlineedit.h>
#include <qstring.h>

/**
  * Browse filesystem for output file
  */
void povrayDlg::browse()
{
    QFileDialog oFD(this);
    QString oFilterPNG( "Image (*.png)" );
    oFD.setFilter( oFilterPNG );
    oFD.setMode( QFileDialog::AnyFile );
    oFD.setViewMode( QFileDialog::List );
//QT3FIXME    oFD.setPreviewMode( QFileDialog::NoPreview );
//QT3FIXME     oFD.setSelectedFilter( oFilterPNG );

    QString oFile;

    if ( oFD.exec() == QDialog::Accepted )
    {
 oFile = oFD.selectedFile();
 m_pFileOut->setText( oFile );
    }
}
