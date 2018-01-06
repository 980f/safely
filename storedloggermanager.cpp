#include "storedloggermanager.h"

static Logger dbl("Logm",false);//debug the debuggers


StoredLoggerManager::StoredLoggerManager(Storable &node):StoredGroup<LoggerControl> (node){
  //last in wins, prior ones leak.
  //todo:1 message wtf if prior is not null
  Logger::manager=this;
}

void StoredLoggerManager::Set(Cstr name, bool enable){
  StoredLoggerManager *me=static_cast<StoredLoggerManager*>(Logger::manager);
  if(me){
    LoggerControl &logger=me->child(name);
    logger.assign(enable);
  }
}

//before file load:LoadJSON,DBG,
void StoredLoggerManager::onCreation(Logger &logger){
  dbl("Logger %s defaults %sabled",logger.prefix,logger.enabled?"en":"dis");
  bool existed=false;
  LoggerControl &controller= StoredGroup::child(logger.prefix,&existed);
  if(existed){
    logger.enabled=controller;
  } else {//program defaults are used to init a file, but after that the file is supreme.
    controller.setDefault(logger.enabled);
  }
  controller.myLoggers.append(&logger);
  dbl("Logger %s set to %sabled",logger.prefix,logger.enabled?"en":"dis");
}

void StoredLoggerManager::onDestruction(Logger &logger){
  dbl("Logger %s at exit was %sabled",logger.prefix,logger.enabled?"en":"dis");
  logger.enabled=false;//reduce impact on use after free.
  LoggerControl *controller= StoredGroup::existing(logger.prefix);
  if(controller){
    controller->myLoggers.remove(&logger);
  }
  //sigctracker should take care of deregistering the updater.
  //we retain the setting for file storage.
}




LoggerControl::LoggerControl(Storable &node):StoredBoolean(node),
  myLoggers(false){//we track, we don't own.
  sendChanges(MyHandler(LoggerControl::updateLoggers));
}

void LoggerControl::updateLoggers(bool newlevel){
  ChainScanner<Logger> scan(myLoggers);
  while(scan.hasNext()){
    scan.next().enabled=newlevel;
  }
}
