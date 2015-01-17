/*-*-c++-*-*****************************************************************
                          sculptor_dlg_lualist.h
			  -------------------
    begin                : Nov 11, 2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_LUALIST_H
#define SCULPTOR_DLG_LUALIST_H

// sculptor includes
class sculptor_window;
// uic-qt4 includes
#include <ui_dlg_lualist.h>
// qt4 includes
#include <QString>
class QListWidgetItem;


/**
 * \brief Intermediate class that implements GUI methods for the lua script list
 * \author Manuel Wahle
 */
class dlg_lualist : public QWidget, private Ui::dlg_lualist
{
    Q_OBJECT

private slots:

    /**
     * the user clicked on an entry in the list, which has to be signaled to the main window
     */
    void sItemClicked(QListWidgetItem* pItem);

public:

    /*
     * Constructor
     */
    dlg_lualist(QWidget* pParent);

    /*
     * Destructor
     */
    virtual ~dlg_lualist();
    
    /*
     * Set all the connections from button to methods in sculptor_window
     */
    void initConnections(sculptor_window* pWindow);

    /*
     * Add a script to the list
     */
    void addScriptName(QString oScriptName);
 
    /*
     * set a script's name
     */
    void setScriptName(int iIndex, QString oName);

    /*
     * Remove a script from the list
     */
    void removeScriptName(int iIndex);

    /*
     * Change the name of a script
     */
    void updateScriptListName(int iIndex, QString oName);

    /*
     * Select a script
     */
    void setCurrentScript(int iIndex);

    /*
     * get the currently selected script index
     */
    int getCurrentScript();

    /*
     * Show that a script is running, e.g. disable the run button, enable stop button
     */
    void showScriptRunning(bool bRunning = true);

    /**
     *  User would like to move a script down in the list
     */
    void scriptDown(int iIndex);

    /**
     *  User would like to move a script up in the list
     */
    void scriptUp(int iIndex);
};

#endif // SCULPTOR_DLG_LUALIST_H
