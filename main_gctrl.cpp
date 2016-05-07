//#include <iostream>
#include <poll.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
//#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "LL.h"
#include "Poll.h"

#include "G_Ctrl.h"
#include "ExaktG.h"

using namespace std;
using namespace EuMax01;

static int get_sdtin(char * buf,int buflen);

class App:IPollReadListener,IPollTimerListener
{
public:
  App();
  ~App()
  {
    delete(pm);
    delete(pr_stdio);
  }
  virtual void pollReadEvent(PollSource * s);
  virtual void pollTimerExpired(long us);
  int poll();
  void addPollReader(PollReader * pPR);

private:
  int bufLen;
  PollManager* pm;
  PollReader* pr_stdio;
  PollTimer* pt;
  char buf[1024];
};

App::App(){
  bufLen = 1024;
  pm = new PollManager();
  pr_stdio = new PollReader(this);
  pr_stdio->setReadSource(STDIN_FILENO,(char*)"StandardIO");
  pm->addSource(pr_stdio);
  pt = new PollTimer(500,this);
  //pm->addTimer(this->pt);
}

int App::poll()
{
  return pm->call_poll();
}

void App::addPollReader(PollReader * pPR)
{
  this->pm->addSource(pPR);
}

void App::pollReadEvent(PollSource * ps)
{
  if(1==get_sdtin(this->buf,this->bufLen))
    {
      this->pm->stopPolling();
    }
}

void App::pollTimerExpired(long us)
{
  printf("App timerExp %li \n",us);
}

static int verboseExakt = 1;
static int verboseG = 1;

G_Ctrl * pG = 0;
App * pApp = 0;
ExaktG * pExaktG = 0;

static void onExit(int i,void* pv)
{
  if(0!=pG)
    {
      pG->closeUart();
    }
  printf("onExit\n");
}

int main(int argc, char *argv[])
{
  int ret = 0;
  pApp = new App();
  pExaktG = new ExaktG(verboseExakt,verboseG);
  pG = pExaktG->getG_Ctrl();

  printf("main_gctrl\n");
  ret = pG->openUart((char*)"/dev/ttyUSB0",115200);
  if(ret>0)
    {
      pExaktG->setFD(pG->getFd());
      pApp->addPollReader(pExaktG->getPollReader());
    }


  ret = pApp->poll();
  printf("main_gctrl App poll returns: %i\n",ret);

  on_exit(onExit,0);
}

static int get_sdtin(char * buf,int buflen)
{
  if(fgets(buf,buflen,stdin)!=0){
    if(!strncmp("test",buf,4)){
      printf("test output\n");
    }
    else if(!strncmp("g1x 10",buf,5)){
      pG->cmdG1(nTinyG_X,10,100);
    }
    else if(!strncmp("g1y -10",buf,6)){
      pG->cmdG1(nTinyG_Y,-10,100);
    }
    else if( (!strncmp("g90",buf,3)) || (!strncmp("G90",buf,3)) ){
      pG->cmdG((char*)"G90");
    }
    else if( (!strncmp("g91",buf,3)) || (!strncmp("G91",buf,3)) ){
      pG->cmdG((char*)"G91");
    }
    else if(!strncmp("exit",buf,4)){
      return 1;
    }
    else{
      printf("supported cmds: exit,test\n");
    }
  }
  return 0;
}
