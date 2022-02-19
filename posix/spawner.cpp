#include "spawner.h"
#include "wait.h"

Spawner::Spawner(const char *tracer):PosixWrapper(tracer){

}

bool Spawner::operator()(const char *path, char **argv, char **envp){
  exitstatus=0;
  exitsignal=0;
  return posix_spawnp(&childpid,path,factions,attrs,argv,envp);
}

void Spawner::killby(int signal){
  if(childpid) {//checking, although kill process 0 is probably ignored, and even if not then it will ignore sigusr1
    kill(childpid,signal);//todo:1 does kill return something worthy of recording in errornumber?
  }
}

Spawner::operator bool(){

  if(!failed(waitpid(childpid,&status,WUNTRACED|WCONTINUED))){
    //how do we deal with anegregious error?
  }
  bool exited=WIFEXITED(status);
  if(exited){
    exitstatus=WEXITSTATUS(status);
  }
  bool signalled=WIFSIGNALED(status);
  if(signalled){
    exitsignal=WTERMSIG(status);
  }
  bool stopped=WIFSTOPPED(status);
  if(stopped){
    exitsignal=WSTOPSIG(status);
  }
  bool continued=WIFCONTINUED(status);
  return !exited ||!signalled;
}

Spawner::Attr::Attr():PosixWrapper("SPATTR"){
  posix_spawnattr_init(&attr);
}

Spawner::Attr::~Attr(){
  //todo: only call destory if init was ok?
  posix_spawnattr_destroy(&attr);
}

bool Spawner::Attr::setFlags(short flags){
  return failed(posix_spawnattr_setflags(&attr, flags));
}

Spawner::FileActions::FileActions():PosixWrapper("SPFA"){
  failed(posix_spawn_file_actions_init(&factions));
}

Spawner::FileActions::~FileActions(){
  //todo: only call destory if init was ok?
  posix_spawn_file_actions_destroy(&factions);
}

bool Spawner::FileActions::addClose(int fileno){
  //todo: type for filenumbers
  return failed(posix_spawn_file_actions_addclose(&factions,fileno));
}
