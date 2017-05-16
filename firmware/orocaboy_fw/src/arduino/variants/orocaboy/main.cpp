//============================================================================
// Name        : main.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++
//============================================================================


#include "main.h"


int main()
{
  bspInit();
  hwInit();


  setup();

  while (1)
  {
    loop();
    if(serialEventRun) serialEventRun();
  }
  return 0;
}
