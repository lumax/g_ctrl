/*
Bastian Ruppert
*/

namespace EuMax01
{
#ifndef __ExaktG_H__
#define __ExaktG_H__

#include "jsmn.h"

  class ExaktG:IPollReadListener
  {
  public:
    ExaktG(int verbExakt,int verbGCtrl);
    ~ExaktG(){
      delete(pr_gcodes);
    }

    void setVerbose(int verb);
    virtual void (pollReadEvent)(PollSource * s);
    void setFD(int fd);
    PollReader* getPollReader(void);
    G_Ctrl * getG_Ctrl(void);
    int jsonParser(char * buffer, int buflen);

  private:
    PollReader * pr_gcodes;
    int verbose;
    G_Ctrl GCtrl;
    jsmn_parser p;
    jsmntok_t t[128]; /* We expect no more than 128 tokens */
    int jsoneq(const char *json, jsmntok_t *tok, const char *s);
  };

#endif /* __ExaktG_H__*/
}//end namespace
