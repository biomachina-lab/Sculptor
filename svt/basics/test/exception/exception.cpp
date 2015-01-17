#include <stdlib.h>

#ifdef WIN32_MSVC
#  define usleep(i) Sleep((i)/1000)
#else
#  include <unistd.h>
#endif


//#include <svt_iostream.h>
#include <svt_exception.h>
#include <svt_messages.h>
#include <svt_str.h>
#include <svt_stlList.h>



int main()
{


  cout << endl 
	   << "-----------------------------------------------------------------" << endl
	   << "              Simulating all available dialogs                   " << endl
	   << "-----------------------------------------------------------------" << endl
	   << endl << endl;


  usleep(500000);
  cout << "---- info dialog -----" << endl << endl; 
  usleep(500000);
  svt_exception::ui()->info("(info-message)");
  cout << endl << endl << endl
	   << "-----------------------------------------------------------------"
	   << endl << endl;
  
  usleep(500000);
  cout << "---- error dialog -----" << endl << endl; 
  usleep(500000);
  svt_exception::ui()->error("(error-message)");
  cout << endl << endl << endl
	   << "-----------------------------------------------------------------"
	   << endl << endl;
  

  unsigned i;

  usleep(500000);
  cout << "---- busy dialog -----" << endl << endl; 
  usleep(500000);
  try {
	svt_exception::ui()->busyPopup("(busy-message)");
	for ( i=0; i<1000; i++ ) 
	  {
	  usleep(30000);
	  svt_exception::ui()->busyRotate();
	  }
	svt_exception::ui()->busyPopdown();
	}
  catch (svt_userInterrupt& ) {
  }

  cout << endl << endl << endl
	   << "-----------------------------------------------------------------"
	   << endl << endl;
  


  usleep(500000);
  cout << "---- help dialog -----" << endl << endl; 
  usleep(500000);
  svt_exception::ui()->helpPopup("(help-message)");
  usleep(500000);
  svt_exception::ui()->helpPopdown();
  cout << endl << endl << endl
	   << "-----------------------------------------------------------------"
	   << endl << endl;
  


  usleep(500000);
  cout << "---- simple question dialog -----" << endl << endl; 
  usleep(500000);
  if (!svt_exception::ui()->question("Wanna see more?"))
	return 0;
  usleep(500000);
  cout << endl << endl << endl
	   << "-----------------------------------------------------------------"
	   << endl << endl;
  
  

  usleep(500000);
  cout << "---- question dialog with cancel and configured answers -----" << endl << endl; 
  usleep(500000);

  try 
	{
	if( !svt_exception::ui()->question("Wanna see even more?",true, 
									   "Of course", "Enough of that sh..",
									   false))
	  return 0;
	else
	  cout << "Excellent choice!" << endl;	  
	}
  catch ( svt_userInterrupt& )
	{
	}	

  usleep(500000);
  cout << endl << endl << endl
	   << "-----------------------------------------------------------------"
	   << endl << endl;
  

  usleep(500000);
  cout << "---- question dialog  -----" << endl << endl; 
  usleep(500000);
  list<svt_str> oList;
  oList.push_back("So, what do you think of this message system?");
  oList.push_back("Great - this stuff is really chilling out!");
  oList.push_back("It´s quite ok");
  oList.push_back("No comment...");

  
  try 
	{
	i=svt_exception::ui()->question(oList, true, 2);
	
	cout << "answer " << i << " was chosen" << endl;
	}
  catch ( svt_userInterrupt& )
	{
	}	
  
  usleep(500000);
  cout << endl << endl << endl
	   << "-----------------------------------------------------------------"
	   << endl << endl;
  


  usleep(500000);
  cout << "---- progress bar with percent values  -----" << endl << endl; 
  usleep(500000);

  try
	{
	svt_exception::ui()->progressPopup("(progress-message)");
	for (i=0;i<=100;i++)
	  {
	  usleep(50000);
	  svt_exception::ui()->progress(i);
	  }
	}

  catch (svt_userInterrupt&)
	{
	}

  usleep(500000);
  cout << endl << endl << endl
	   << "-----------------------------------------------------------------"
	   << endl << endl;

  
  usleep(500000);
  cout << "---- progress bar (other version)  -----" << endl << endl; 
  usleep(500000);

  try
	{
	i=0;
	svt_exception::ui()->progressPopup("(progress-message)", i, 1000);
	for (i=1;i<=1000;i++)
	  {
	  usleep(10000);
	  svt_exception::ui()->progress(i,1000);
	  } 
	}

  catch (svt_userInterrupt&)
	{
	}


  usleep(500000);
  cout << endl << endl << endl
	   << "-----------------------------------------------------------------"
	   << endl << endl;


  char szString[] = "T h a t ' s   A l l ,   F o l k s !";
  cout << "     ";
  for ( i=0; i<strlen(szString);i++)
	{
	cout << szString[i] << flush;
	usleep(50000);
	}
  cout << endl;

  return 0;
}
