/*
Bastian Ruppert
*/

namespace EuMax01
{
#ifndef __ExaktG_H__
#define __ExaktG_H__

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

  private:
    PollReader * pr_gcodes;
    int verbose;
    G_Ctrl GCtrl;
    StreamScanner sScan;
  };

#endif /* __ExaktG_H__*/
}//end namespace
