/***************************************************************************
                          svt_volLoadPlugin
                          -----------------
    begin                : 01/13/2004
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_VOLLOADPLUGIN
#define SVT_VOLLOADPLUGIN

#include <svt_basics.h>
#include <svt_volume.h>

// summary of a volumetric dataset
typedef struct
{
    int iSizeX; // x dimension of the data
    int iSizeY; // y dimension of the data
    int iSizeZ; // z dimension of the data
    float fGridSpacing; // size of voxel in real space (in Angstroem!)
    float* pData; // pointer to data in memory

} svt_vol;

extern "C" typedef char* (*fpGetExtension)(void);
extern "C" typedef char* (*fpGetExtensionDesc)(void);
extern "C" typedef svt_vol (*fpLoadVolume)(char*);
extern "C" typedef bool (*fpSaveVolume)(svt_vol,char*);

/**
 * this class represents a svt_volume plugin that can load files of a single volumetric file type
 */
class svt_volLoadPlugin
{

protected:
    // function pointers
    fpGetExtension m_fpGetExtension;
    fpGetExtensionDesc m_fpGetExtensionDesc;
    fpLoadVolume m_fpLoadVolume;
    fpSaveVolume m_fpSaveVolume;

    // was the plugin already loaded? If not we cannot use it...
    bool m_bLoaded;

public:

    /**
     * Constructor
     * \param pName pointer to char with the name of the plugin
     */
    svt_volLoadPlugin(char* pName =NULL);
    ~svt_volLoadPlugin();

    /**
     * Load a plugin
     * \param pName pointer to char with the name of the plugin
     */
    bool loadPlugin(char* pName);
    /**
     * Load all plugins in a directory
     * \param pDir pointer to char with the directory name
     */
    bool loadPluginDir(char* pDir);

    /**
     * load a volumetric dataset
     * \param pFile pointer to char with the filename of the volumetric dataset
     */
    svt_volume<float> loadVolume(char* pFile);
    /**
     * save a volumetric dataset
     * \param oVol svt_vol object with the informations about the volume data
     * \param pFile pointer to char with the filename of the volumetric dataset
     */
    bool saveVolume(svt_volume<float> oVol, char* pFile);

    /**
     * get the file extension of the files this plugin supports
     * \return pointer to char
     */
    char* getExtension();
    /**
     * get the description for the file extension (e.g. for a file open dialog box)
     * \return pointe to char
     */
    char* getExtensionDesc();

};
#endif
