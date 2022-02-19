#include "piper.h"

#include "stdio.h"
#include "onexit.h"

Piper::Piper(const char *tracer):PosixWrapper(tracer){}

Piper::Piper(const char *tracer, const char *command, bool forWrite):Piper(tracer){
  open(command,forWrite);
}

bool Piper::open(const char *command, bool forWrite){
  if(pipef){//already open
    if(!close()){
      dbg("Could not close already active command");
      return false;//even though we could theoretically open the new command we want the caller to know an error occurred
    }
  }
  pipef=popen(command,forWrite?"w":"r");//todo:2 linux "e" extension
  if(pipef){
    return true;
  } else {
    return failure(errno);
  }
}

bool Piper::close(){
  if(pipef){
    ClearOnExit<decltype(pipef)> justryonce(pipef);//we only give ourselves one chance to close
//    return failed(pclose(pipef));
    //until we figureout why we get errors every other time just:
    pclose(pipef);
    return true;
  }
  return true;//ok to close unopened entity
}

Piper::~Piper(){
  close();
}

///////////////////////////////

Piper::Reuser::Reuser(Piper &piper, const char *command, bool forWrite):piper(piper) {
  piper.open(command,forWrite);//not our problem if already open!
}

char *Piper::Reuser::read(char *buf, size_t sizeofbuf){
  if(!piper){
    return nullptr;
  }
  return fgets(buf,sizeofbuf,piper.pipef);
}
