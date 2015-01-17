/*-*-c++-*-*****************************************************************
                          sculptor_dlg_luaeditor.h
			  -------------------
    begin                : Nov 11, 2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_DLG_LUAEDITOR_H
#define SCULPTOR_DLG_LUAEDITOR_H

// sculptor includes
class sculptor_window;
// uic-qt4 includes
#include <ui_dlg_luaeditor.h>
// qt4 includes
#include <QString>


/**
 * \brief Intermediate class that implements GUI methods for the lua editor
 * \author Manuel Wahle
 */
class dlg_luaeditor : public QWidget, private Ui::dlg_luaeditor
{
    Q_OBJECT

public:

    /*
     * Constructor
     */
    dlg_luaeditor(QWidget* pParent);

    /*
     * Destructor
     */
    virtual ~dlg_luaeditor();

    /*
     * Set all the connections from button to methods in sculptor_window
     */
    void initConnections(sculptor_window* pWindow);

    /*
     * Add a script to the script list dropdown
     */
    void addScriptName(QString oScriptName);

    /*
     * Remove a script from the script list dropdown
     */
    void removeScriptName(int iIndex);

    /*
     * Select a script and put it into the editor window
     */
    void setCurrentScript(int iIndex, QString oScript, QString oName = QString(""));

    /*
     * Change the name of a script
     */
    void updateScriptListName(int iIndex);

    /*
     * Show that a script is running, e.g. disable the run button, enable stop button
     */
    void showScriptRunning(bool bRunning = true);

    /*
     * Return the currently displayed text in the editor
     */
    QString getScriptText();

    /*
     * set a script's name
     */
    void setScriptName(int iIndex, QString oName);

    /*
     * Return the currently displayed script name
     */
    QString getScriptName();

    /*
     * Return a p[ointer to the actual lua texteditor
     */
    QTextEdit* getEditor();

    /**
     *  User would like to move a script down in the list
     */
    void scriptDown(int iIndex, QString oScript);

    /**
     *  User would like to move a script up in the list
     */
    void scriptUp(int iIndex, QString oScript);
};

#endif // SCULPTOR_DLG_LUAEDITOR_H
