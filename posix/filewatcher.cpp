#include "filewatcher.h"
#include <sys/inotify.h>
#include <climits>

/** the layout of what gets read from a FileWatcher fd. */
struct FileEvent {
  WatchMarker wd; /* Watch descriptor */
  uint32_t mask; /* Mask describing event */
  uint32_t cookie; /* Unique cookie associating related events (for rename(2)) */
  uint32_t len; /* Size of name field */
  char namestartshere[]; //presence is conditional on len>0.
  /** @returns wrapper around null-terminated name */
  Indexer<char> name();

  /* inotify(7) man page: the length of each inotify_event structure is sizeof(struct inotify_event)+len.*/
};

bool FileWatch::handle(const FileEvent& ev)  {
  if (ev.wd==wd) {
    handler(ev);
    return false;
  }
  return true;
}

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
//manpage sez:  sizeof(struct inotify_event)	+ NAME_MAX + 1
  uint8_t buffer[sizeof(FileEvent) + NAME_MAX] __attribute__((aligned(__alignof__(FileEvent))));//maximum FileEvent we can handle //alignment needed on some processors.
  Indexer stuff(buffer, sizeof(buffer));
  if (fd.read(stuff) && fd.lastRead >= unsigned(sizeof(FileEvent))) { //#sign mismatch OK
    auto fe(*reinterpret_cast<const FileEvent *>(buffer));
    if (stuff.used() > sizeof(FileEvent) + fe.len) {
      //now we know fe is a sane FileEvent
      Bundler<FileWatch>::While(&FileWatch::handle,fe);
    }
  }
} // FileWatcher::nextEvent

Indexer<char> FileEvent::name() {
  return Indexer(namestartshere, len);
}
