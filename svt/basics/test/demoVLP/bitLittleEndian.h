/***************************************************************************
                          bigLittleEndian
                          ---------------
    begin                : 01/16/2004
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// THIS FILE IS PART OF THE SVT/SENSITUS VOL I/O PLUGIN SDK

/**
 * are the CPU registers in Big-Endian-Format?
 * \return true if big endian cpu
 */
bool bigEndianMachine(void);

/**
 * swap a double value
 * \param pValue pointer to the double variable
 */
void swapDouble(double *pValue);

/**
 * swap a float value
 * \param pValue pointer to the float variable
 */
void swapFloat(float *pValue);

/**
 * swap an short int value
 * \param pValue pointer to the short variable
 */
void swapShort(short *pValue);

/**
 * swap an int value
 * \param pValue pointer to the Int32 variable
 */
void swapInt(int *pValue);
