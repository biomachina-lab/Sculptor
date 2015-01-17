//
// includes
//

#include <svt_exception.h>
#include <svt_str.h>
#include <svt_init.h>
#include <svt_HeightField.h>


//
// internal prototypes
//
void usage(const char* exe_name);
void getArgs(int argc, char** argv, char*& in_file, char*& out_file, 
			 svt_HeightField::ResizeMode& eRes, 
			 svt_HeightField::ObjErrMode& eErr,
			 char*& testsize_string);



////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//                                                                //
//                                                                //
//                      M A I N                                   //
//                                                                //
//                                                                //
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{

  try
	{

	//
	// get command line args
	//
	char* in_file=NULL;
	char* out_file=NULL;
	char* testsize_string=NULL;

	svt_HeightField::ObjErrMode eErr = svt_HeightField::Real;
	svt_HeightField::ResizeMode eRes = svt_HeightField::Center;
	
	getArgs(argc, argv, in_file, out_file, eRes,eErr, testsize_string);
	
	//
	// correct / throw if inforamtion is incomplete
	//
	if (!in_file)
	  {
	  if (!testsize_string) 
		{
		usage(argv[0]);
		throw svt_exception("Incorrect Arguments");
		}
	  else 
		{
		out_file=testsize_string;
		}
	  }


	//
	// init svt and create the heightfield
	//

	svt_HeightField oHF(in_file, out_file, NULL, -1, eRes, -1, eErr);
	cout << "constructed the heightfield" << endl;

	//
	// perform precomputations with given options
	//
	oHF.precompute();
	exit(0);

	}
  catch (svt_exception& oErr) {
	oErr.uiMessage();
  }


  return 0;

}


//
// process command line args
//
void getArgs(int argc, char** argv, 
			 char*& in_file, char*& out_file, 
			 svt_HeightField::ResizeMode& eRes,
			 svt_HeightField::ObjErrMode& eErr,
			 char*& testsize_string)
{

  int reg_arg_count=0;
  char* ptr;

  for (int i=1; i<argc; i++)
	{
	if (*(argv[i]) == '-' )

	  //
	  // options
	  //
	  switch (argv[i][1])
		{
		//
		// -m => resize-mode = Center
		//
		case 'm': 
		case 'M':
		  eRes=svt_HeightField::Center;
		  break;

		//
		// -i => resize-mode = Interpolate
		//
		case 'i': 
		case 'I':
		  eRes=svt_HeightField::Interpolate;
		  break;

		//
		// -d  => use real distance
		//
		case 'd': 
		  eErr=svt_HeightField::Real;
		  break;

		//
		// -D  => use z distance
		//
		case 'D': 
		  eErr=svt_HeightField::Z;
		  break;
		//
		// -T, -t  => texture file
		//
		case 'T': 
		case 't': 
		  testsize_string=argv[i]+2;
		  if (strlen(testsize_string)<=0)
			testsize_string=argv[++i];
		  break;

		default: 
		  cerr << "Unknown option -" << argv[i][1] << " - ignoring..." 
			   << endl; 
		  break;
		}

	else
	  {
	  switch (reg_arg_count)
		{
		case 0: 
		  in_file  = argv[i]; 
		  break;
		case 1: 
		  out_file = argv[i]; 
		  break;
		default: 
		  cerr << "Unknown argument " << argv[i] << " - ignoring..." << endl;
		  break;
		}
	  reg_arg_count++;
	  }
	}
}

void usage(const char* exe_name)
{
  cerr << "usage: " << exe_name << "<in_file> <out_file> [-d] [-D] [-m] [-i]" << endl
	   << " with" << endl  
	   << "     in_file   : filename of elevation input data (.height-file)" << endl
	   << "     out_file  : filename of quadtree output data (.qt-file)" << endl
	   << "     -m        : set resize-mode to CENTER" << endl
	   << "     -i        : set resize-mode to INTERPOLATE" << endl
	   << "     -d        : use real distance to compute object error (default)" << endl 
	   << "     -D        : use height distance to compute object error" << endl
	   << "     -t <size> : init a test case with given size" << endl
	   << "                 size can be specified as XxY or as single value X, which is equivalent to XxX"
	   <<  "                (only valid if no height file was specified!)" << endl;
  return;
}
