#include "filewatcher.h"
#include <sys/inotify.h>
#include <climits>

FileWatcher::FileWatcher(bool blocking) : fd("FileWatcher") {
  if (!fd.preopened(inotify_init1(blocking ? 0 : IN_NONBLOCK))) {
    fd.failure(errno);
  }
}

WatchMarker FileWatcher::addWatch(const char *pathname, uint32_t mask) {
  unsigned wd = BadIndex; //init for debug.
  if (fd.okValue(wd, inotify_add_watch(fd, pathname, mask))) {
    return wd;
  } else {
    return BadIndex;
  }
}

bool FileWatcher::removeWatch(FileEvent &fe) {
  return !fd.failed(inotify_rm_watch(fd, int(fe.wd))); //maydo: don't call rm_watch if wd is badIndex.
}

bool FileWatcher::hasEvent() {
  return fd.available() >= sizeof(FileEvent);
}

void FileWatcher::nextEvent() {
  uint8_t buffer[sizeof(FileEvent) + NAME_MAX] __attribute__((aligned(__alignof__(FileEvent))));//maximum FileEvent we can handle
  Indexer stuff(buffer, sizeof(buffer));
  if (fd.read(stuff) && fd.lastRead >= unsigned(sizeof(FileEvent))) { //#sign mismatch OK
    FileEvent &fe(*reinterpret_cast<FileEvent *>(buffer));
    if (stuff.used() > sizeof(FileEvent) + fe.len) {
      //now we know fe is a sane FileEvent
      Bundler<FileWatch>::While<void(const FileEvent&),const FileEvent&>(&FileWatch::handle,fe);
    }
  }
} // FileWatcher::nextEvent

Indexer<char> FileEvent::name() {
  return Indexer(&namestartshere, len);
}
