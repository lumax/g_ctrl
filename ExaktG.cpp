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
#include "jsmn.h"

#include "ExaktG.h"

namespace EuMax01
{

  ExaktG::ExaktG(int verbExakt,int verbG):GCtrl(verbG)
  {
    pr_gcodes = new PollReader(this);
    this->setVerbose(verbExakt);
    jsmn_init(&p);
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
    int ret = 0;
    //sleep(1);
    memset(buf, 0, 1024);
    buf[1023] = '\n';
    printf("###\n");
    len = read(s->thePollfd.fd,buf,buflen-1);
    if(len){
      printf("%s",buf);
      return;
    }
    if(len){
      printf("%s\n",buf);
      ret = jsonParser(buf,len);

      if(JSMN_ERROR_INVAL == ret)
	{
	  printf("jsonParser: bad token, JSON string is corrupted\n");
	}
      else if(JSMN_ERROR_NOMEM == ret)
	{
	  printf("jsonParser: not enough tokens, JSON string is too large\n");
	}
      else if(JSMN_ERROR_PART == ret)
	{
	  printf("jsonParser:  JSON string is too short, expecting more JSON data\n");
	}
      else
	{
	  printf("jsonParser: unknown return\n");
	}
    }
  }

  int ExaktG::jsoneq(const char *json, jsmntok_t *tok, const char *s)
  {
    if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
	strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
      return 0;
    }
    return -1;
  }

  int ExaktG::jsonParser(char * buffer, int buflen)
  {
    	int i;
	int r;

	r = jsmn_parse(&p, buffer, buflen, t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return r;
	}
	printf("jsmn_parse returned: %i\n",r);

	/* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		printf("Object expected\n");
		return r;
	}

	/* Loop over all keys of the root object */
	for (i = 1; i < r; i++) {
		if (jsoneq(buffer, &t[i], "r") == 0) {
			/* We may use strndup() to fetch string value */
			printf("- User: %.*s\n", t[i+1].end-t[i+1].start,
					buffer + t[i+1].start);
			i++;
		} else if (jsoneq(buffer, &t[i], "xxx") == 0) {
			/* We may additionally check if the value is either "true" or "false" */
			printf("- xxx: %.*s\n", t[i+1].end-t[i+1].start,
					buffer + t[i+1].start);
			i++;
		} else if (jsoneq(buffer, &t[i], "uid") == 0) {
			/* We may want to do strtol() here to get numeric value */
			printf("- UID: %.*s\n", t[i+1].end-t[i+1].start,
					buffer + t[i+1].start);
			i++;
		} else if (jsoneq(buffer, &t[i], "f") == 0) {
			int j;
			printf("- f:\n");
			if (t[i+1].type != JSMN_ARRAY) {
				continue; /* We expect groups to be an array of strings */
			}
			for (j = 0; j < t[i+1].size; j++) {
				jsmntok_t *g = &t[i+j+2];
				printf("  * %.*s\n", g->end - g->start, buffer + g->start);
			}
			i += t[i+1].size + 1;
		} else {
			printf("Unexpected key: %.*s\n", t[i].end-t[i].start,
					buffer + t[i].start);
		}
	}
	return 0;
  }

}
