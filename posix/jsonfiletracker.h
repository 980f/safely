#ifndef JSONFILETRACKER_H
#define JSONFILETRACKER_H
#include <filewatcher.h>
#include <jsonfile.h>

struct JsonFileTracker {
  FileWatcher watcher;
  int wd=BadIndex;
  JsonFile jfile;
  void reload(const FileEvent &fe);
  JsonFileTracker(Storable &root);
  int load(Cstr filename);
};
#endif // JSONFILETRACKER_H
