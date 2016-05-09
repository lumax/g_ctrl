/*
Bastian Ruppert
*/

//#include <sys/stat.h>
//#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "StreamScanner.h"

namespace EuMax01
{

  StreamScanner::StreamScanner(int verb):maxScans(16),		\
					 maxPayloadLen(16),	\
					 streamBufLen(256)
  {
    this->setVerbose(verb);
    scanslen = 0;
  }

  void StreamScanner::scan(char c)
  {
    for(int i=0; i<this->scanslen;i++){
      scanOneByte(c,i);
    }
  }

  inline void StreamScanner::statePayload(char c, int index)
  {
    if(c == scans[index].pcDelim1[0] || c == scans[index].pcDelim2[0]){
      scans[index].pcStreamBuf[scans[index].len] = '\0';
      printf("delim: %s found, scanned: %s\n",scans[index].pcDelim1,scans[index].pcStreamBuf);
      if(0 != scans[index].fnkScanResult){
	(*scans[index].fnkScanResult)(&scans[index]);
      }
      scans[index].state = nStreamScannerState_payload;
      resetScan(index);
    }else{
      scans[index].pcStreamBuf[scans[index].len] = c;
      scans[index].len++;
    }
    if(scans[index].len>=this->maxPayloadLen){
      scans[index].state = nStreamScannerState_payload;
      resetScan(index);
    }
  }

  inline void StreamScanner::statePreamble(char c, int index)
  {
    if(c != scans[index].pcPreamble[scans[index].len]){
      resetScan(index);
    }
    else{
      scans[index].pcStreamBuf[scans[index].len] = c;
      scans[index].len++;
    }
    if(scans[index].len >= (int)scans[index].preambleLen){
      scans[index].state = nStreamScannerState_payload;
    }
  }

  inline void StreamScanner::scanOneByte(char c,int index)
  {
    switch(scans[index].state){
    case nStreamScannerState_payload:{
      statePayload(c,index);
      break;
    }
    default:{//nStreamScannerState_preamble
      statePreamble(c,index);
      break;
    }
    }
  }

  int StreamScanner::addScanner(int type,				\
				const char * preamble,			\
				char * delim1,				\
				char * delim2,				\
				void (*fnkScanRes)(struct StreamScanner_t *))
  {
    /*noch Platz?*/
    if(this->scanslen>=this->maxScans){
      return -1;
    }
    /*Passen die Parameter?*/
    if(0==preamble){
      return -2;
    }
    if(0==delim1 && 0==delim2){
      return -3;
    }
    if(this->streamBufLen <= (int)(strlen(preamble) + this->maxPayloadLen + 1/*delim*/)){
      return -4;
    }

    /*Type: im Zweifel immer Float*/
    if(nStreamScannerType_int == type){
      scans[this->scanslen].typeToScanFor = nStreamScannerType_int;
    }else{
      scans[this->scanslen].typeToScanFor = nStreamScannerType_float;
    }
    scans[this->scanslen].pcPreamble = preamble;
    scans[this->scanslen].pcDelim1 = delim1;
    scans[this->scanslen].pcDelim2 = delim2;
    if(0==delim1){
      scans[this->scanslen].pcDelim1 = delim2; 
    }
    if(0==delim2){
      scans[this->scanslen].pcDelim2 = delim1; 
    }
    scans[this->scanslen].fnkScanResult = fnkScanRes;

    scans[this->scanslen].preambleLen = strlen(scans[this->scanslen].pcPreamble);

    resetScan(this->scanslen);

    for(int i = 0;i < this->streamBufLen;i++){
	scans[this->scanslen].pcStreamBuf[i] = 0x00;
    }

    scanslen++;

    return 0;
  }

  inline void StreamScanner::resetScan(int index)
  {
    scans[index].state = nStreamScannerState_preamble;
    scans[index].len = 0;   
  }

  void StreamScanner::setVerbose(int verb)
  {
    this->verbose = verb;    
  }
}
