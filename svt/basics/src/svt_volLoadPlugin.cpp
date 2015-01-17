/***************************************************************************
                          svt_volLoadPlugin
                          -----------------
    begin                : 01/13/2004
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_volLoadPlugin.h>
#include <svt_iostream.h>

#ifndef WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif

/**
 * Constructor
 * \param pName pointer to char with the name of the plugin
 */
svt_volLoadPlugin::svt_volLoadPlugin(char* pName) :
    m_fpGetExtension(NULL),
    m_fpGetExtensionDesc(NULL),
    m_fpLoadVolume(NULL),
    m_fpSaveVolume(NULL),
    m_bLoaded(false)
{
    if (pName != NULL)
        loadPlugin(pName);
}
svt_volLoadPlugin:: ~svt_volLoadPlugin()
{
}

/**
 * Load a plugin
 * \param pName pointer to char with the name of the plugin
 */
bool svt_volLoadPlugin::loadPlugin(char* pName)
{
    char pBuf[256];

#ifndef WIN32

    sprintf(pBuf,"libsvt_vlp_%s.so", pName);
    void* pLib = dlopen(pBuf, RTLD_LAZY);

    if (pLib == NULL)
    {
	cout << "svt_volLoadPlugin: sorry, couldnt open the plugin lib " <<  pBuf << "!" << endl;
	return false;
    }

    m_fpGetExtension  = ( char*(*)() ) dlsym(pLib,"getExtension");
    m_fpGetExtensionDesc = ( char*(*)() ) dlsym(pLib, "getExtensionDesc");
    m_fpLoadVolume = ( svt_vol(*)(char*) ) dlsym(pLib, "loadVolume");
    m_fpSaveVolume = ( bool(*)(svt_vol,char*) ) dlsym(pLib, "saveVolume");

#else

    HINSTANCE pLib;

    sprintf(pBuf,"svt_vlp_%s.DLL", pName);
    pLib = LoadLibrary(pBuf);

    if (pLib == NULL)
    {
	cout << "svt_volLoadPlugin: sorry, couldnt open the plugin lib " << pBuf << "!" << endl;
	return false;
    }

    m_fpGetExtension  = ( char* (*)() ) GetProcAddress(pLib,"getExtension");
    m_fpGetExtensionDesc = ( char* (*)() ) GetProcAddress(pLib, "getExtensionDesc");
    m_fpLoadVolume = ( svt_vol (*)(char*) ) GetProcAddress(pLib, "loadVolume");
    m_fpSaveVolume = ( bool (*)(svt_vol,char*) ) GetProcAddress(pLib, "saveVolume");

#endif

    if (m_fpGetExtension == NULL)
    {
	cout << "svt_volLoadPlugin: sorry, this is not a legal plugin: " << pBuf << "!" << endl;
	return false;
    }

    m_bLoaded = true;

    return true;
}
/**
 * Load all plugins in a directory
 * \param pDir pointer to char with the directory name
 */
bool svt_volLoadPlugin::loadPluginDir(char*)
{
    return true; //has to be filled with source!!!!!
}

/**
 * load a volumetric dataset
 * \param pFile pointer to char with the filename of the volumetric dataset
 */
svt_volume<float> svt_volLoadPlugin::loadVolume(char* pFile)
{
    svt_vol oVol = (*m_fpLoadVolume)(pFile);

    // now convert svt_vol into full blown svt_volume class
    svt_volume<float> oSvtVol;
    oSvtVol.setData( oVol.iSizeX, oVol.iSizeY, oVol.iSizeZ, oVol.pData);

    // and return it...
    return oSvtVol;
}
/**
 * save a volumetric dataset
 * \param oSvtVol svt_vol object with the informations about the volume data
 * \param pFile pointer to char with the filename of the volumetric dataset
 */
bool svt_volLoadPlugin::saveVolume(svt_volume<float> oSvtVol, char* pFile)
{
    // convert svt_volume object into simple svt_vol c object
    svt_vol oVol;

    oVol.iSizeX = oSvtVol.getSizeX();
    oVol.iSizeY = oSvtVol.getSizeY();
    oVol.iSizeZ = oSvtVol.getSizeZ();
    oVol.pData = oSvtVol.getData();
    oVol.fGridSpacing = oSvtVol.getWidth();

    // now call function pointer
    return (*m_fpSaveVolume)(oVol, pFile);
}

/**
 * get the file extension of the files this plugin supports
 * \return pointer to char
 */
char* svt_volLoadPlugin::getExtension()
{
    if (m_bLoaded && m_fpGetExtension != NULL)
	return (*m_fpGetExtension)();
    else
	return NULL;
}
/**
 * get the description for the file extension (e.g. for a file open dialog box)
 * \return pointe to char
 */
char* svt_volLoadPlugin::getExtensionDesc()
{
    if (m_bLoaded && m_fpGetExtensionDesc != NULL)
	return (*m_fpGetExtensionDesc)();
    else
	return NULL;
}
