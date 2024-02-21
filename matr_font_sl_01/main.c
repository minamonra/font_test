#include <stdio.h>
#include <stdlib.h>
#include "myfont.h"

int main(void) 
{
  unsigned int offset; 
  unsigned char Charcode = 48 /* смотрит на 0 */;

  // Доступ к offset через Charcode
  offset = inconsola24x32index[Charcode].offset;


  do // do main 
  { 
   
  } while (1); // main do
}

// End of file
