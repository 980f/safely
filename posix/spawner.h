#ifndef SPAWNER_H
#define SPAWNER_H
//#include "unistd.h"
#include "spawn.h"
#include "posixwrapper.h"

class Spawner : public PosixWrapper {
public:
  class Attr :PosixWrapper{
      posix_spawnattr_t attr;
public:
      Attr();
      ~Attr();

      //add members as we learn how to use features, with argument testers
      operator decltype(attr)*(){
        return &attr;
      }
      operator bool(){
          return isOk();
      }


      bool setFlags(short flags);

  };

  class FileActions:public PosixWrapper {
      posix_spawn_file_actions_t factions;
  public:
      FileActions();
      ~FileActions();

      operator decltype(factions)*(){
        return &factions;
      }
      operator bool(){
          return isOk();
      }

      bool addClose(int fileno);
  };
private:
  pid_t childpid;
public:
  pid_t pid() const {
    return childpid;
  }
  //let user whack these in order to detect updates
  int status;
  int exitstatus;
  int exitsignal;
  Attr attrs;
  FileActions factions;
  Spawner(const char *tracer);
  bool operator()(const char *path, char **argv, char **envp);

  /** @returns whether child is still running */
  operator bool ();

  /** set environment variable, updating this object's errno*/
  bool env(const char * name, const char *value,bool force=true);

  void killby(int signal);
};

#endif // SPAWNER_H
