/***************************************************************************
                          gpgpu
                          ---------------------
    begin                : 07/25/2008
    author               : Manuel Wahle
    email                : sculptor@biomachina.org
 ***************************************************************************/


#include <svt_gpgpu.h>
#include <svt_point_cloud.h>
#include <svt_types.h>

#ifdef MACOSX
  #include <cg.h>
  #include <cgGL.h>
#else
  #include <Cg/cg.h>
  #include <Cg/cgGL.h>
#endif

#include <svt_init.h>


int main(int argc, char *argv[])
{

    //init svt
    svt_init(argc,argv);

    svt_gpgpu oGPGPU;

    //svt_point_cloud oPointCLoud();
    //    oPointCloud.loadPDB("/home/mwahle/pdbs/1ATN.pdb");

    Real32 * pCoords = new Real32[1024*3];

    for (int i=0; i<3064; i+=3)
    {
	pCoords[ i ] = (Real32) i + 1.0f;
	pCoords[i+1] = (Real32) i + 1.0f;
	pCoords[i+2] = (Real32) i + 1.0f;
    }

    Real32 * pResult;// = new Real32[3064*3064];

    pResult = new Real32[1024 * 1024 * 3];
    printf("new Real32[1024*1024*3]\n");

    for (int i=0; i<3145728; i+=3)
    {
	pResult[ i ] = 0.0f;
	pResult[i+1] = 0.0f;
	pResult[i+2] = 0.0f;
    }




    oGPGPU.addShader(true, NULL, "/home/mwahle/Sculptor/cvs/svt/core/test/gpgpu/fragment_shader.cg", CG_PROFILE_UNKNOWN, CG_PROFILE_ARBFP1);
    printf("------------------------------------------------- oGPGPU.addShader done.\n");

    oGPGPU.setInputData(pCoords, 1024);
    printf("------------------------------------------------- oGPGPU.setInputData done.\n");

    oGPGPU.compute(pResult);
    printf("------------------------------------------------- oGPGPU.compute done.\n");



 
    for (int i=0; i<120; i+=3)//3145728; i+=3)
    {
	printf ("%3.3f %3.3f %3.3f\n", pResult[ i ], pResult[i+1], pResult[i+2]);
    }

    return 0;
}
