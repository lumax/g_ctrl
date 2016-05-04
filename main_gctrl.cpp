//#include <iostream>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
//#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "G_Ctrl.h"

using namespace std;
using namespace EuMax01;

static int need_exit = 0;
static int verboseG = 1;

static int get_sdtin(void);

G_Ctrl * pG = 0;

static void onExit(int i,void* pv)
{
  printf("onExit\n");
}

int main(int argc, char *argv[])
{

  pG = new G_Ctrl(verboseG);

  printf("main_gctrl\n");

  while(0==need_exit)
    {
      get_sdtin();
    }

  on_exit(onExit,0);
}

static int get_sdtin(void)
{
  char buf[256];

  if(fgets(buf,256,stdin)!=0){
    if(!strncmp("test",buf,4)){
      printf("test output\n");
    }
    else if(!strncmp("g1x 10",buf,5)){
      pG->cmdG1(nTinyG_X,10,100);
    }
    else if(!strncmp("exit",buf,4)){
      need_exit=1;
    }
    else{
      printf("supported cmds: exit,test\n");
    }
  }
  return 0;
}
