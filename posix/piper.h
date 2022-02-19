#ifndef PIPER_H
#define PIPER_H  "(C)2022 Andy Heilveil (github/980f)"

#include "stdio.h" //FILE
#include "posixwrapper.h"
/**
 * wrapper for posix pipe utility, run a program that you either talk to or listen to, but not both.
*/
class Piper : public PosixWrapper {
public:
  FILE * pipef=nullptr;
  Piper(const char *tracer);

  Piper(const char *tracer,const char *command,bool forWrite=false);

  bool open(const char *command,bool forWrite=false);

  /** @returns whether probably OK to use */
  operator bool (){
    return pipef!=nullptr;
  }

  bool close();

  ~Piper();

  /** to retain error message between retries or other uses: */
  class Reuser {
    Piper &piper;
  public:
    Reuser(Piper &piper,const char *command,bool forWrite=false);
    ~Reuser(){
      piper.close();
    }

    operator bool(){
      return bool(piper);
    }

    char * read(char *buf, size_t sizeofbuf);
  };
};

#endif // PIPER_H
