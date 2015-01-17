/***************************************************************************
                          live_channel.h
                          --------------
    begin                : 11.04.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef LIVE_CHANNEL_H
#define LIVE_CHANNEL_H

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define LIVE_CHN_INPUT   1
#define LIVE_CHN_OUTPUT  2

#define LIVE_CHN_MATRIX  1
#define LIVE_CHN_INT     2
#define LIVE_CHN_FLOAT   3
#define LIVE_CHN_CHAR    4
#define LIVE_CHN_UNKNOWN 0

/**
 * the channel structure
 */
typedef struct
{
    /** type (LIVE_CHN_MATRIX, LIVE_CHN_INT, LIVE_CHN_FLOAT, LIVE_CHN_CHAR, LIVE_CHN_UNKNOWN) */
    UInt8 type;

    /** direction of the channel (true = data flows from the device to the application!) */
    UInt8 direction;

    /** the current data of the channel (mval = matrix, fval = float, ival = int, cval = char) */
    union {
        Real32 mval[16];
        Real32 fval;
        Int16 ival;
        char cval;
    } data;
} live_channel;

#endif
