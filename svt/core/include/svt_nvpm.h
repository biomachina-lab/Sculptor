/***************************************************************************
                          svt_nvpm.h  -  description
                             -------------------
    begin                : Sep 25 2008
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/


// If SVT_NVPM is defined, performance data of the graphics hard- and software will be
// collected and displayed.
// Note: Instrumented Driver necessary, see NVPerfSDK on developer.nvidia.com


#ifdef SVT_NVPM

#ifndef SVT_NVPM_H
#define SVT_NVPM_H

#include <svt_types.h>
#include <NVPerfSDK.h>
#include <svt_stlVector.h>

#define NVPM_SAMPLE_STEPS 200

typedef enum {
    SVT_NVPM_AVERAGE_NOT,
    SVT_NVPM_AVERAGE_FRAMES,
    SVT_NVPM_AVERAGE_SECONDS} SVT_NVPM_AVERAGE_OUTPUT;

DLLEXPORT class svt_nvpm
{

 private:

    UINT m_iNumNVPMUintCounter;
    UINT m_iNumNVPMRealCounter;
    UINT m_iNumNVPMCounter;
    vector<UINT> m_aNVPMUintCounters;
    vector<UINT> m_aNVPMRealCounters;
    vector<char*> m_aNVPMUintCountersNames;
    vector<char*> m_aNVPMRealCountersNames;
    vector<UINT64*> m_aNVPMUintData;
    vector<Real64*> m_aNVPMRealData;
    UINT m_iNVPMDataItr;
    UINT m_iNumFramesSoFar;

 public:

    /**
     * Constructor
     */
    svt_nvpm();

    /**
     * Destructor
     */
    virtual ~svt_nvpm();

    /**
     * Initializes the Nvidia Performance Measurement SDK
     */
    bool init();

    /**
     * This function shuts down the Nvidia Performance Measurement SDK.
     */
    void shutdown();

    /**
     * Outputs the sampled data
     * \param eOUTPUT determines if the values should not be averaged, averaged over
     * the number of frames, or over time
     * \param iAmount is the number of frames or time over which the values are
     * to be averaged
     */
    void outputData(SVT_NVPM_AVERAGE_OUTPUT eOUTPUT, unsigned int iAmount);

    /**
     * This function checks if a requested counter is available and if so, adds it to the active counters.
     * It is called from Init()
     */
    void addCounter(char * pCounterName);

    /**
     * This function samples all active counters. Must be called once per frame
     */
    void sampleData();

};

#endif // SVT_NVPM_H

#endif // SVT_NVPM
