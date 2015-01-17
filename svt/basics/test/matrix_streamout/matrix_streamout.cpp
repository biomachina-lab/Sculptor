#include <stdlib.h>
#include <stdio.h>

#include <svt_exception.h>
#include <svt_matrix.h>

int main()
{
  svt_matrix<int> a(15,30);

  int i=0;
  for (int iRow=0; iRow<15; iRow++)
	for (int iCol=0; iCol<30;iCol++)
	  a[iRow][iCol] = i++;


  cout << a;

  return 0;
}
  
