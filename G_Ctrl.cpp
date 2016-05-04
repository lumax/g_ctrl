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


#include "G_Ctrl.h"

namespace EuMax01
{

  G_Ctrl::~G_Ctrl(){}
  G_Ctrl::G_Ctrl(int verb)
  {
    this->setVerbose(verb);
    cmdLen = 64;
  }

  void G_Ctrl::setVerbose(int verb)
  {
    this->verbose = verb;    
  }

  char* G_Ctrl::getAxis(int axis)
  {
    switch(axis)
      {
      case nTinyG_Y:{
	return (char*)"Y";
      }
      case nTinyG_Z:{
	return (char*)"Z";
      }
      case nTinyG_A:{
	return (char*)"A";
      }
      default:{//nTinyG_X
	return (char*)"X";
      }
      };
  }

  void G_Ctrl::cmdG1(int axis,int range, int velocity)
  {
    char * Axis = 0;

    Axis = getAxis(axis);

    snprintf(this->cmdBuf,cmdLen,"G1 %s%i,F%i",Axis,range,velocity);
    if(0!=this->verbose)
      {
	printf("cmdG1: %s\n",this->cmdBuf);
      }
  }

  int G_Ctrl::openUart()
  {
    int     status = 0;
    int     fd;
    struct termios newtio;

    // open the tty
    fd = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0)
      {
	if(this->verbose){
	  printf(" openUart error opening uart\n");
	}
	return fd;
      }

    // flush serial port
    status = tcflush(fd, TCIFLUSH);
    if (status < 0)
      {
	if(this->verbose){
	  printf(" openUart error tcflush\n");
	}
	close(fd);
	return -1;
      }
    /* get current terminal state */
    tcgetattr(fd,&newtio);
    cfmakeraw(&newtio);

    status = cfsetspeed(&newtio, 115200);
    if (status < 0)
      {
	if(this->verbose){
	  printf(" openUart error cfsetspeed\n");
	}
	close(fd);
	fd = -1;
	return fd;
      }

    // set its new attrigutes
    status = tcsetattr(fd,TCSANOW,&newtio);
    if (status < 0)
      {
	if(this->verbose){
	  printf(" openUart error tcsetattr\n");
	}
	close(fd);
	fd = -1;
	return fd;
      }
    return fd;
  }
}
