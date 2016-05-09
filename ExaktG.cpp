/*
Bastian Ruppert
*/

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include "LL.h"
#include "Poll.h"
#include "G_Ctrl.h"
#include "StreamScanner.h"

#include "ExaktG.h"

namespace EuMax01
{

  static void streamScanResult(struct StreamScanner_t * ps)
  {
    printf("StreamScannter Result Int: %i Float: %f\n",	\
	   ps->scannedInt, ps->scannedFloat);
  }

  ExaktG::ExaktG(int verbExakt,int verbG):GCtrl(verbG)
  {
    pr_gcodes = new PollReader(this);
    this->setVerbose(verbExakt);
    sScan.addScanner(nStreamScannerType_float,		\
		     (char*)"\"posx\":",		\
		     (char*)"}",(char*)",",		\
		     streamScanResult);
    sScan.addScanner(nStreamScannerType_float,				\
		     (char*)"\"posy\":",				\
		     (char*)"}",(char*)",",				\
		     streamScanResult);
  }

  void ExaktG::setFD(int fd)
  {
    this->pr_gcodes->setReadSource(fd,(char*)"ExaktG_Ctrl");
  }

  void ExaktG::setVerbose(int verb)
  {
    this->verbose = verb;    
  }

  G_Ctrl * ExaktG::getG_Ctrl(void)
  {
    return &GCtrl;
  }

  PollReader * ExaktG::getPollReader(void)
  {
    return this->pr_gcodes;
  }

  void ExaktG::pollReadEvent(PollSource * s)
  {
    char buf[1024];
    int buflen = 1024;
    int len = 0;
    //int ret = 0;
    //sleep(1);
    memset(buf, 0, 1024);
    buf[1023] = '\n';
    //printf("###\n");
    len = read(s->thePollfd.fd,buf,buflen-1);
    if(len){
      for(int i = 0;i<len;i++){
	this->sScan.scan(buf[i]);
      }
      printf("%s",buf);
    }
  }
}
