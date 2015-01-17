/***************************************************************************
                          tempmotors.h  -  description
                             -------------------
    begin                : 06.11.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef TEMPMOTORSDLG_H
#define TEMPMOTORSDLG_H

#include <qvariant.h>
#include <qdialog.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QProgressBar;

class sculptor_scene;

class TempMotorsDlg : public QDialog
{ 
    Q_OBJECT

public:
    TempMotorsDlg( sculptor_scene* pScene, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~TempMotorsDlg();

    QLabel* TextLabel4_2;
    QLabel* TextLabel4_2_2;
    QLabel* TextLabel4_3;
    QLabel* TextLabel4_3_2;
    QLabel* TextLabel4_3_3;
    QProgressBar* m_pProgTempRotX;
    QProgressBar* m_pProgTempRotY;
    QProgressBar* m_pProgTempRotZ;
    QProgressBar* m_pProgTempY;
    QProgressBar* m_pProgTempZ;
    QProgressBar* m_pProgTempX;
    QLabel* test;

    sculptor_scene* m_pScene;

private slots:

    /**
     * update the temperatures of the motors
     */
    void updateTemp();

protected:
    QGridLayout* TempMotorsDlgLayout;
};

#endif // TEMPMOTORSDLG_H
