#include "storedloggermanager.h"

static Logger dbl("Logm",false);//debug the debuggers


StoredLoggerManager::StoredLoggerManager(Storable &node):StoredGroup<StoredBoolean> (node){
  //last in wins, prior ones leak.
  //todo:1 message wtf if prior is not null
  Logger::manager=this;
}

void StoredLoggerManager::Set(Cstr name, bool enable){
  StoredLoggerManager *me=static_cast<StoredLoggerManager*>(Logger::manager);
  if(me){
    StoredBoolean &logger=me->child(name);
    logger=enable;
  }
}

//before file load:LoadJSON,DBG,
void StoredLoggerManager::onCreation(Logger &logger){
  dbl("Logger %s defaults %sabled",logger.prefix,logger.enabled?"en":"dis");
  bool existed=false;
  StoredBoolean &controller= StoredGroup::child(logger.prefix,&existed);
  if(!existed){//program defaults are used to init a file, but after that the file is supreme.
    controller=logger.enabled;
  }
  controller.onChangeUpdate(logger.enabled);
  dbl("Logger %s set to %sabled",logger.prefix,logger.enabled?"en":"dis");
}

void StoredLoggerManager::onDestruction(Logger &logger){
  dbl("Logger %s at exit was %sabled",logger.prefix,logger.enabled?"en":"dis");
  logger.enabled=false;//reduce impact on use after free.
  //sigctracker should take care of deregistering the updater.
  //we retain the setting for file storage.
}


