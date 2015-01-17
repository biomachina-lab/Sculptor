/***************************************************************************
                          bigLittleEndian
                          ---------------
    begin                : 01/16/2004
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// THIS FILE IS PART OF THE SVT/SENSITUS VOL I/O PLUGIN SDK

short int g_iSwapTest=0x0102;

/**
 * are the CPU registers in Big-Endian-Format?
 * \return true if big endian cpu
 */
bool bigEndianMachine(void)
{
    char *p;
 
    p=(char *)&g_iSwapTest;
    return (*p == 0x01);
}

/**
 * swap a double value
 * \param pValue pointer to the double variable
 */
void swapDouble(double *pValue)
{
    char *cp, temp;

    cp=(char *)pValue;
    swap(cp, cp+7);
    cp++;
    swap(cp, cp+5);
    cp++;
    swap(cp, cp+3);
    cp++;
    swap(cp, cp+1);
}

/**
 * swap a float value
 * \param pValue pointer to the float variable
 */
void swapFloat(float *pValue)
{
    char *cp, temp;

    cp=(char *)pValue;
    swap(cp, cp+3);
    cp++;
    swap(cp, cp+1);
}

/**
 * swap an short int value
 * \param pValue pointer to the short variable
 */
void swapShort(short *pValue)
{
    char *cp, temp;

    cp=(char *)pValue;
    swap(cp, cp+1);
}

/**
 * swap an int value
 * \param pValue pointer to the Int32 variable
 */
void swapInt(int *pValue)
{
    char *cp, temp;

    cp=(char *)pValue;
    swap(cp, cp+3);
    cp++;
    swap(cp, cp+1);
}
