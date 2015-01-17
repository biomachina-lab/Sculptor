/***************************************************************************
                          fileform.h  -  description
                             -------------------
    begin                : 30.07.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef FILEFORM_H
#define FILEFORM_H

#include <qvariant.h>
#include <qgrid.h>
#include <qdialog.h>
#include <qdragobject.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QListBox;
class QListBoxItem;
class QPushButton;

/** This class is a list for the currently opened documents.
 * @author Stefan Birmanns
 */
class FileForm : public QGrid
{ 
    Q_OBJECT

public:
    FileForm( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~FileForm();

    QPushButton* m_pButtonDelete;
    QPushButton* m_pButtonMove;
    QPushButton* m_pButtonVisible;
    QPushButton* m_pButtonProperties;
    QPushButton* m_pButtonUp;
    QPushButton* m_pButtonDown;
    QPushButton* m_pButtonTop;
    QPushButton* m_pButtonSave;
    QListBox*    m_pListFile;

    void dropEvent(QDropEvent* event);
    void dragEnterEvent(QDragEnterEvent* event);

    virtual QSize sizeHint() const;

signals:

    void drop( QString );

protected:
    QGridLayout* FileFormLayout;
    QGridLayout* Layout2;
    QHBoxLayout* Layout1;
};

#endif
