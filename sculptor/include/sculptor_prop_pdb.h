/*-*-c++-*-*****************************************************************
			  sculptor_prop_pdb.h
			  -------------------
    begin                : 10/11/2010
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// This file is part of the sculptor program package
///////////////////////////////////////////////////////////////////////////////

#ifndef SCULPTOR_PROP_PDB_H
#define SCULPTOR_PROP_PDB_H

// uic-qt4 includes
#include <ui_prop_pdb.h>
// sculptor includes
class pdb_document;
// svt includes
#include <svt_types.h>


/*
 * This class inherits from the QT4 Designer prop_pdb class and implements its functionality.
 * \author Manuel Wahle
 */
class prop_pdb : public QWidget, private Ui::prop_pdb
{
    Q_OBJECT

    // the sculptor document
    pdb_document* m_pDoc;

public:

    /**
     * Constructor
     */
    prop_pdb( QWidget *parent, pdb_document* pDoc );

    /**
     * Destructor
     */
    virtual ~prop_pdb();

    /**
     * \name Methods related to transformations
     */
    //@{

    /**
     *
     */
    void transformAdd(const char * pName);

    /**
     *
     */
    void transformRemove(int iIndex);

    /**
     *
     */
    void transformUp(int iIndex);

    /**
     *
     */
    void transformDown(int iIndex);

    /**
     *
     */
    int getTransformCount();

    /**
     *
     */
    int getCurrentTransform();

    /**
     *
     */
    void setTransformText(int iIndex, QString oText);

    /**
     *
     */
    QString getTransformText(int iIndex);

    /**
     *
     */
    void transformsClear();

    //@}

    /**
     * \name Methods related to pdb graphics modes
     */
    //@{

    /**
     *
     */
    void pdbModeAdd(QString oStr);

    /**
     *
     */
    int getCurrentPdbModeIndex();

    /**
     *
     */
    void setCurrentPdbModeIndex(int iIndex);

    /**
     *
     */
    void setPdbMode(int iIndex, QString oStr);

    /**
     *
     */
    void pdbModeRemove(int iIndex);

    /**
     *
     */
    void pdbModesClear();

    /**
     *
     */
    void updatePdbMode();

    /**
     *
     */
    int getGM_AtomType();

    /**
     *
     */
    int getGM_AtomName();

    /**
     *
     */
    int getGM_AtomTo();

    /**
     *
     */
    int getGM_GraphicsMode();

    /**
     *
     */
    int getGM_ColorMap();

    /**
     *
     */
    int getGM_TextLabel();

public slots:

    /**
     * update the pdb mode buttons according to selected mode
     * this is a QT slot.
     */
    void sUpdatePdbModeButtons();

    //@}

public:

    /**
     *
     */
    void updateTabInformation();

    /**
     *
     */
    void updateTabDocking();

    /**
     *
     */
    void showDockingTab();

public slots:

    /**
     * the user changed the color map combo box
     * this is a QT slot.
     */
    void sChangeColorMap(int i);

    /**
     * the user changed the text label combo box
     * this is a QT slot.
     */
    void sChangeTextLabel(int i);

    /**
     * the user changed the graphics mode combo box.
     * this is a QT slot.
     * \param iGraphicsMode the new selected graphics mode
     */
    void sChangeGM(int);

    /**
    * get the currently selected colorm apping
    */
    int getCurrentColorMap();

    /**
     * the user has clicked on the selection name combo
     * this is a QT slot.
     */
    void sChangeSelectionName(int);

    /**
     * the user has clicked on the selection mode combo
     * this is a QT slot.
     */
    void sChangeSelectionMode(int mode);

    /**
     * \name Methods related to dynamics
     */
    //@{

public:

    /**
     * update playback-speed with the current value
     * \param iTPS curent timesteps per second
     */
    void updateDynTPS( int iTPS = -1 );

    /**
     * update timestep with the current value
     * \param iTimestep curent timestep
     */
    void updateDynTimestep( int iTimestep = -1, int iMaxTimestep = -1 );

    /**
     * update loop mode with the current value
     * \param iLoop curent loop mode
     */
    void updateDynLoop(int iLoop = -1);

    /**
     * update playback buttons
     * \param iButtons curent buttons mode
     */
    void updateDynButtons(int iPlaying = -1);

    /**
     * set the export movie button checked or not
     * \param bActive button checked or not
     */
    void updateDynMovieButton(bool bActive);

public slots:

    /**
     * set timesteps per second from the slider
     */
    void sSliderTPSChanged(int iTPS);

    /**
     * set timesteps per second from the spinbox
     */
    void sSpinboxTPSChanged();

    /**
     * set timestep from the slider
     */
    void sSliderTimestepChanged(int iTimestep);

    /**
     * set timestep from the spinbox
     */
    void sSpinboxTimestepChanged();

    //@}
};

#endif // SCULPTOR_PROP_PDB_H
