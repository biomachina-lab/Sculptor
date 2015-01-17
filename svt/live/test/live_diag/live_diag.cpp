// small test programm
#include <live.h>
#include <stdlib.h>
#include <stdio.h>

#include <svt_basics.h>
#include <svt_config.h>
#include <svt_init.h>

int main(int argc, char** argv)
{
    svt_config oConfig(".svtrc");
    svt_setConfig(&oConfig);

    const char* devName ="mouse";
    int chnNum = 1;
    live_channel* chn;
    live_channel* chn_mat;
    Real32* mat;
    Real32 val = 100.0f;
    bool box = true;

    if (argc ==1)
    {
        printf("live_diag usage: live_diag <device name> <channel number> [<output value>]\n");
        exit(1);
    }

    if (argc > 1)
        devName = argv[1];
    if (argc > 2)
        chnNum = atoi(argv[2]);
    if (argc > 3)
        val = atof(argv[3]);

    printf("live_diag: initializing\n");

    printf("live_diag: open device \"%s\" channel: %i\n", devName, chnNum);
    live_device* dev = live_openDevice(devName);

    if (dev == NULL)
    {
        printf("live_diag: couldnt open device!\n");
        exit(1);
    }

    printf("live_diag: get number of channels: %i\n", live_getNumChannels(dev));

    printf("live_diag: update the device\n");
    for(;;)
    {
        live_updateData(dev);
        chn = live_getChannel(dev, chnNum);
        if (chn->direction == LIVE_CHN_INPUT)
        {
	    if (chn->type == LIVE_CHN_MATRIX)
		printf("live_diag: %2.3f %2.3f %2.3f         \r", chn->data.mval[12], chn->data.mval[13], chn->data.mval[14]);
            if (chn->type == LIVE_CHN_INT)
                printf("live_diag: integer value: %i\r", chn->data.ival);
            if (chn->type == LIVE_CHN_FLOAT)
                printf("live_diag: float value: %f\r", chn->data.fval);
            if (chn->type == LIVE_CHN_CHAR)
                printf("live_diag: char value: %c\r", chn->data.cval);
            if (chn->type == LIVE_CHN_UNKNOWN)
                printf("live_diag: unknown channel type!\r");
        } else {
            if (box)
            {
                if (chn->type == LIVE_CHN_FLOAT)
                    chn->data.fval = val;
            } else {
                chn_mat = live_getChannel(dev, 1);
                if (chn_mat->type == LIVE_CHN_MATRIX)
                {
                    if (chn_mat->data.mval[12] > 0.0f)
                        chn->data.fval = val;
                    else
                        chn->data.fval = 0.0f;
                }

            }
        }
    }

    printf("live_diag: close device\n");
    live_closeDevice(dev);

    return 0;
}
