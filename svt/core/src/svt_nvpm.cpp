/***************************************************************************
                          svt_nvpm.cpp  -  implementation
                             -------------------
    begin                : Dec 22 2006
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/


// If SVT_NVPM is defined, performance data of the graphics hard- and software will be
// collected and displayed.
// Note: Instrumented Driver necessary, see NVPerfSDK on developer.nvidia.com

#ifdef SVT_NVPM

#include <svt_nvpm.h>
#include <svt_iostream.h>


/**
 * Constructor
 */
svt_nvpm::svt_nvpm()
{
    m_iNumNVPMUintCounter = 0;
    m_iNumNVPMRealCounter = 0;
    m_iNumNVPMCounter = 0;
    m_iNVPMDataItr = 0;
    m_iNumFramesSoFar = 0;
}

/**
 * Destructor
 */
svt_nvpm::~svt_nvpm()
{
}

/**
 * Outputs the sampled data
 * \param eOUTPUT determines if the values should not be averaged, averaged over
 * the number of frames, or over time
 * \param iAmount is the number of frames or time over which the values are
 * to be averaged
 */
void svt_nvpm::outputData(SVT_NVPM_AVERAGE_OUTPUT eOUTPUT, unsigned int iAmount)
{
    if (m_iNumNVPMCounter == 0)
	return;

    // count how many frames were rendered so far (for the averaged output)
//    if (m_iNumFramesSoFar < NVPM_SAMPLE_STEPS )
    {
	if (m_iNumFramesSoFar < iAmount)
	{
	    ++m_iNumFramesSoFar;
	    return;
	}
    }

    UINT iCounterNameLength;
    char cCounterName[64];
    unsigned int j, k;

    // k is the index where the values were stored in the last (most current) sampling round
    (m_iNVPMDataItr == 0) ? k = NVPM_SAMPLE_STEPS - 1 : k = m_iNVPMDataItr-1;

    if (eOUTPUT == SVT_NVPM_AVERAGE_NOT)
    {
	// output the percentage counters
	for (j=0; j<m_iNumNVPMRealCounter; ++j)
	{
// 	    // what's this counter's name?
// 	    iCounterNameLength = 64;
// 	    NVPMGetCounterName(m_aNVPMRealCounters[j], cCounterName, &iCounterNameLength);

	    //printf("%s%2u%s", "CounterIndex: ", (unsigned int)m_aNVPMRealCounters[j], " ");
	    printf("%29s%s%5.2f%s", m_aNVPMRealCountersNames[j], " percent: ", m_aNVPMRealData[j][k], "\n");

	}
	// now the counters whose values are absolute
	for (j=0; j<m_iNumNVPMUintCounter; ++j)
	{
// 	    iCounterNameLength = 64;
// 	    NVPMGetCounterName(m_aNVPMUintCounters[j], cCounterName, &iCounterNameLength);

	    //printf("%s%2u%s", "CounterIndex: ", (unsigned int)m_aNVPMUintCounters[j], " ");
	    printf("%29s%s%u%s", m_aNVPMUintCountersNames[j], "   value: ", (unsigned int)m_aNVPMUintData[j][k], "\n");

	}
	printf("\n");
    }
    else if (eOUTPUT == SVT_NVPM_AVERAGE_FRAMES)
    {
	if (iAmount > NVPM_SAMPLE_STEPS)
	    iAmount = NVPM_SAMPLE_STEPS;

	Real64 aSumPercentages[m_iNumNVPMRealCounter];
	UINT64 aSumAbsolutes[m_iNumNVPMUintCounter];

	// in these two arrays the values are to be summed up
	unsigned int l;
	for (l=0; l<m_iNumNVPMRealCounter; ++l)
	    aSumPercentages[l] = 0;
	for (l=0; l<m_iNumNVPMUintCounter; ++l)
	    aSumAbsolutes[l] = 0;

	//(m_iNVPMDataItr == 0) ? k = NVPM_SAMPLE_STEPS - 1 : k = m_iNVPMDataItr-1;
	int i = k - iAmount + 1;
	if (i < 0)
	    i = NVPM_SAMPLE_STEPS + i;

	// sum up the values
	k=0;
	while (k<iAmount)
	{
	    for (l=0; l<m_iNumNVPMRealCounter; ++l)
		aSumPercentages[l] += m_aNVPMRealData[l][i];
	    for (l=0; l<m_iNumNVPMUintCounter; ++l)
		aSumAbsolutes[l] += m_aNVPMUintData[l][i];
	    ++k;
	    ++i;
	    i %= NVPM_SAMPLE_STEPS;
	}

	// calculate the average
	for (l=0; l<m_iNumNVPMRealCounter; ++l)
	    aSumPercentages[l] /= (Real64)iAmount;
	for (l=0; l<m_iNumNVPMUintCounter; ++l)
	    aSumAbsolutes[l] /= iAmount;

	// and output the averages
	// first, the percentage counters
	for (j=0; j<m_iNumNVPMRealCounter; ++j)
	{
	    // what's this counter's name?
	    iCounterNameLength = 64;
	    NVPMGetCounterName(m_aNVPMRealCounters[j], cCounterName, &iCounterNameLength);
	    printf("%29s%s%5.2f%s", cCounterName, " percent: ", aSumPercentages[j], "\n");
	}
	// now the counters whose values are absolute
	for (j=0; j<m_iNumNVPMUintCounter; ++j)
	{
	    iCounterNameLength = 64;
	    NVPMGetCounterName(m_aNVPMUintCounters[j], cCounterName, &iCounterNameLength);
	    printf("%29s%s%u%s", cCounterName, "   value: ", (unsigned int)aSumAbsolutes[j], "\n");
	}
	printf("\n");
    }
    else if (eOUTPUT == SVT_NVPM_AVERAGE_SECONDS)
    {
	// you are welcome to write the code for that
    }
}


int SVT_NVPM_enumCallback(UINT unCounterIndex, char * pcCounterName)
{
    printf("svt_nvpm             > %3i: %s\n", unCounterIndex, pcCounterName);
    return NVPM_OK;
}
/**
 * Initializes the Nvidia Performance Measurement SDK
 */
bool svt_nvpm::init()
{
    if (NVPMInit() == NVPM_OK)
    {
	cout << "svt_nvpm             > NVPerfSDK initialized, data will be output to the console." << endl;

	// uncomment next line if you want all available counters for your graphics card listed
	//NVPMEnumCounters(SVT_NVPM_enumCallback);

	// Some OpenGL counters:
	//
	addCounter("OGL FPS");
	addCounter("OGL frame time");
	// addCounter("OGL driver sleeping");
	// addCounter("OGL % driver waiting");
	// addCounter("OGL AGP/PCI-E usage (MB)");
	// addCounter("OGL AGP/PCI-E usage (bytes)");
	// addCounter("OGL vidmem MB");
	// addCounter("OGL vidmem bytes");
	// addCounter("OGL vidmem total bytes");
	// addCounter("OGL vidmem total MB");
	// addCounter("OGL Frame Batch Count");
	addCounter("OGL Frame Vertex Count");
	// addCounter("OGL Frame Primitive Count");
 
	// Some GPU counters:
	//
	addCounter("gpu_idle");
	// addCounter("pixel_shader_busy");
	// addCounter("shader_waits_for_texture");
	// addCounter("shader_waits_for_rop");
	// addCounter("rop_busy");
	// addCounter("fast_z_count");
	// addCounter("vertex_attribute_count");
	// addCounter("vertex_shader_busy");
	// addCounter("shaded_pixel_count");
	// addCounter("vertex_count");
	// addCounter("triangle_count");
	// addCounter("primitive_count");
	// addCounter("culled_primitive_count");

	// See the NVPerfSDK Documentation for DirectX counters if interested
    }
    else
    {
	cout << "svt_nvpm             > NVPerfSDK failed to initialize -- no performance data will be available." << endl;
	return false;
    }
    return true;
};


/**
 * This function checks if a requested counter is available and if so, adds it to the active counters.
 * It is called from SVT_NVPM_Init()
 */
void svt_nvpm::addCounter(char * pCounterName)
{
    UINT m_pCounterIndex;
    UINT64 iDisplayHint;

    if (NVPMGetCounterIndex(pCounterName, &m_pCounterIndex) != NVPM_OK)
    {
	cout << "Unable to add counter \"" << pCounterName << "\"." << endl;
	return;
    }

    // determine if the counter returns a value that is an absolute value or a percentage
    NVPMGetCounterAttribute(m_pCounterIndex, NVPMA_COUNTER_DISPLAY_HINT, &iDisplayHint);

    // store the values in an array for percentage values...
    if (iDisplayHint==NVPM_CDH_PERCENT)
    {
	m_aNVPMRealData.push_back(new Real64[NVPM_SAMPLE_STEPS]);
	++m_iNumNVPMRealCounter;
	++m_iNumNVPMCounter;
	m_aNVPMRealCounters.push_back(m_pCounterIndex);
	m_aNVPMRealCountersNames.push_back(pCounterName);
    }
    // ... and one for absolute values
    else if (iDisplayHint==NVPM_CDH_RAW)
    {
	m_aNVPMUintData.push_back(new UINT64[NVPM_SAMPLE_STEPS]);
	++m_iNumNVPMUintCounter;
	++m_iNumNVPMCounter;
	m_aNVPMUintCounters.push_back(m_pCounterIndex);
	m_aNVPMUintCountersNames.push_back(pCounterName);
    }

    // this actually activates the counter
    NVPMAddCounter(m_pCounterIndex);
};


/**
 * This function samples all active counters. Must be called once per frame
 */
void svt_nvpm::sampleData()
{
    if (m_iNumNVPMCounter == 0)
	return;

    unsigned int j;
    UINT64 iValue;
    UINT64 iCycles;
    Real64 fPercent;

    UINT UNUSED = m_iNumNVPMCounter;

    // sample the counters
    NVPMSample(NULL, &UNUSED);

    // first, store the counter's values that return a value that is to be interpreted as a percentage
    for (j=0; j<m_iNumNVPMRealCounter; ++j)
    {
	// get the counter's value
	NVPMGetCounterValue(m_aNVPMRealCounters[j], 0, &iValue, &iCycles);

	// compute the percentage
	fPercent=(double)iValue / (double)iCycles;

	// keep the last NVPM_SAMPLE_STEPS values in an array, so we can compute averages
	m_aNVPMRealData[j][m_iNVPMDataItr] = fPercent;
    }

    // now for the counter whose values are absolute
    for (j=0; j<m_iNumNVPMUintCounter; ++j)
    {
	NVPMGetCounterValue(m_aNVPMUintCounters[j], 0, &iValue, &iCycles);

	m_aNVPMUintData[j][m_iNVPMDataItr] = iValue;
    }

    // m_iNVPMDataItr points to the index where a sampled value is to be stored the next time when
    // the values are sampled. store a maximum of NVPM_SAMPLE_STEPS values
    ++m_iNVPMDataItr %= NVPM_SAMPLE_STEPS;
};


/**
 * This function shuts down the Nvidia Performance Measurement SDK.
 */
void svt_nvpm::shutdown()
{
    NVPMShutdown();
};

#endif // SVT_NVPM
