/*-*-c++-*-*****************************************************************
                          sculptor_dlg_transform
                          ------------
    begin                : 08/10/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/


// uic-qt4 include
#include <ui_dlg_transform.h>


class dlg_transform : public QDialog, public Ui::dlg_transform
{
    Q_OBJECT

public:
    dlg_transform(QWidget *parent = NULL);

};
