#include <live.h>
#include <svt_matrix4f.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <liveForceField.h>

// ghost includes
#include <stdlib.h>
#include <gstBasic.h>
#include <gstPHANToM.h>
#include <gstScene.h>
#include <gstSeparator.h>
#include <gstCylinder.h>

///////////////////////////////////////////////////////////////////////////////
// phantom functions
///////////////////////////////////////////////////////////////////////////////

gstPHANToM* m_pPhantom;
gstScene* m_pScene;
liveForceField* m_pField;

int g_iButton = 0;
svt_matrix4f g_cMatrix("position");
Real32 g_oOutputMatrix[16];

float aTemp[6];

int iLastTC = 0;

double start_pos_x = 0.0f;
double start_pos_y = 0.0f;
double start_pos_z = 0.0f;

/**
 * initialize phantom
 */
void phanInit()
{
    // messagebox with the calibration message
#ifdef WIN32
    MessageBox(NULL, "Please hold the PHANToM in the neutral position and press OK", "PHANToM calibration", MB_OK);
#else
    printf("phantom> Please hold the PHANToM in the neutral position and press a key\n");
    //getchar();
#endif

    // create the scene
    m_pScene = new gstScene;
    gstSeparator* root = new gstSeparator;
    m_pScene->setRoot(root);

    // create phantom
    m_pPhantom = new gstPHANToM("Default PHANToM",TRUE);
    if (!m_pPhantom->getValidConstruction()){
#ifdef WIN32
	MessageBox(NULL, "PHANToM is not responding.", "PHANToM Error", MB_OK);
#endif
        printf("gstPhantom constructor failed!\n");
        exit(0);
    }
    root->addChild(m_pPhantom);

    // create the force
    m_pField = new liveForceField();

    // Create the center point for the bounding box
    gstPoint *theCenter = new gstPoint();
    theCenter->init(0.0, 0.0, 0.0);
    // You must bind the force, (or there will be an error).
    // The choices are by box(boundByBox) or by sphere(boundBySphere).
    m_pField->boundByBox(theCenter, 1000.0, 1000.0, 1000.0);
    // An attenuation distance must be set (method of gstForceField)
    // or else the phantom will react with buzzing and jerking moves.
    // The argumnet of the method is the distance from the bound force
    m_pField->setAttenuationDistance(4);
    root->addChild(m_pField);

    //m_pPhantom->setDamping(70.0f);

    // start the servoloop
    if (!m_pScene->startServoLoop())
    {
        printf("servoLoop unable to start!\n");
        exit(0);
    }
}

/**
 * stop servo loop
 */
void phanStop()
{
    m_pScene->stopServoLoop();
}

/**
 * exit program
 */
void phanExit()
{
    exit(1);
}

///////////////////////////////////////////////////////////////////////////////
// public functions
///////////////////////////////////////////////////////////////////////////////

/**
 * open the device
 */
extern "C" DLLEXPORT UInt16 devOpen()
{
    phanInit();

    return 1;
}

/**
 * close the device
 */
extern "C" DLLEXPORT UInt16 devClose()
{
    phanStop();
    return 1;
}

/**
 * update the device data
 */
extern "C" DLLEXPORT void devUpdate()
{
    if (m_pPhantom->getStatus())
    {
        // messagebox with the calibration message
        //MessageBox(NULL, "PHANToM device fault, press OK", "PHANToM error", MB_OK);
        //phanExit();
    }

    // FIXME port to svt_time!!!!
#ifdef WIN32
    if (GetTickCount() - iLastTC > 5000)
    {
        iLastTC = GetTickCount();
        m_pPhantom->getMotorOverheatState(aTemp);

        int i;
        for(i=0;i<6;i++)
        {
            if (aTemp[i] == 1.0f)
            {
                cout << "live_phantom: Attention: Motors are overheating!" << endl;
                phanExit();
            }
        }
    }
#endif

    /*gstPoint oPoint = m_pPhantom->getPosition_WC();

    if (oPoint.x() != 0 && oPoint.x() != 0 && oPoint.x() != 0)
    {
        g_cMatrix.setId();

        gstVector oVector = m_pPhantom->getGimbalAngles();
        g_cMatrix.addRotation(0,(oVector.x()*PI)/1.80f);
        g_cMatrix.addRotation(1,(oVector.y()*PI)/1.80f);
        g_cMatrix.addRotation(2,(oVector.z()*PI)/1.80f);

        g_cMatrix.mat[3][0] = oPoint.x() * 0.01f;
        g_cMatrix.mat[3][1] = oPoint.y() * 0.01f;
        g_cMatrix.mat[3][2] = oPoint.z() * 0.01f;
    }*/

    //gstTransformMatrix oMatrix = m_pPhantom->getCumulativeTransformMatrix();
    const double* mat = m_pPhantom->getTransformMatrix().getOpenGLMatrix();

    int x,y;
    for(x=0;x<4;x++)
        for(y=0;y<4;y++)
            g_cMatrix.mat[y][x] = mat[x+(y*4)];

    g_cMatrix.mat[3][0] *= 0.01f;
    g_cMatrix.mat[3][1] *= 0.01f;
    g_cMatrix.mat[3][2] *= 0.01f;

    if (m_pPhantom->getStylusSwitch())
        g_iButton = 1;
    else
        g_iButton = 0;
}

/**
 * chn1 = 4x4 matrix transformation
 * chn2 = int button
 * chn3 = float force x axis
 * chn4 = float force y axis
 * chn5 = float force z axis
 * chn6 = float torque force x axis
 * chn7 = float torque force y axis
 * chn8 = float torque force z axis
 * chn9...chn14 temperature of all 6 motors (0.0f .. 1.0f - 1.0f is very hot)
 */
extern "C" DLLEXPORT UInt16 chnNum()
{
    return 14;
}
extern "C" DLLEXPORT UInt8 chnType(UInt16 chn)
{
    switch(chn)
    {
    case 1:
        return LIVE_CHN_MATRIX;
        break;
    case 2:
        return LIVE_CHN_INT;
        break;
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        return LIVE_CHN_FLOAT;
        break;
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
        return LIVE_CHN_FLOAT;
        break;
    default:
        return LIVE_CHN_UNKNOWN;
        break;
    }
}

/**
 * direction is input for channel 1,2 and output for 3-8
 */
extern "C" DLLEXPORT UInt8 chnDir(UInt16 chn)
{
    switch(chn)
    {
    case 1:
    case 2:
        return LIVE_CHN_INPUT;
        break;
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        return LIVE_CHN_OUTPUT;
        break;
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
        return LIVE_CHN_INPUT;
        break;
    default:
        return LIVE_CHN_INPUT;
    }
}

/**
 * return the integer data (only valid for the button, chn2)
 */
extern "C" DLLEXPORT Int16 chnGetDataINT(UInt16 chn)
{
    if (chn == 2)
        return g_iButton;
    else
        return 0;
}

/**
 * return the char data (not valid here)
 */
extern "C" DLLEXPORT char chnGetDataCHAR(UInt16 chn)
{
    return 0;
}

/**
 * return the float data
 */
extern "C" DLLEXPORT Real32 chnGetDataFLOAT(UInt16 chn)
{
    if (chn > 8 && chn < 15)
        return aTemp[chn-9];
    else
        return 0;
}
/**
 * return the matrix data
 */
extern "C" DLLEXPORT Real32* chnGetDataMAT(UInt16 chn)
{
    int x,y;
    for(x=0;x<4;x++)
        for(y=0;y<4;y++)
            g_oOutputMatrix[x+(y*4)] = g_cMatrix.mat[y][x];

    return g_oOutputMatrix;
}

/**
 * the functions for writing data to the device
 */
extern "C" DLLEXPORT void chnSetDataINT(UInt16, Int16){};
extern "C" DLLEXPORT void chnSetDataCHAR(UInt16, char){};

extern "C" DLLEXPORT void chnSetDataFLOAT(UInt16 chn , Real32 val)
{
    if (val > 100.0f)
        val = 100.0f;
    if (val < -100.0f)
        val = -100.0f;

    switch(chn)
    {
    case 3:
        m_pField->setForceX( val );
        break;
    case 4:
        m_pField->setForceY( val );
        break;
    case 5:
        m_pField->setForceZ( val );
        break;
    case 6:
        m_pField->setTorqueForceX( val );
        break;
    case 7:
        m_pField->setTorqueForceY( val );
        break;
    case 8:
        m_pField->setTorqueForceZ( val );
        break;
    default:
        break;
    }
};

extern "C" DLLEXPORT void chnSetDataMAT(UInt16, Real32*){};
