#pragma once

#include <bundler.h>
#include <hook.h>

#include "fildes.h"

using WatchMarker = int;//from manpages, but unsigned would also work as a negative value is an error code when otherwise a wd handle would be returned.

/** the layout of what gets read from a FileWatcher fd. */
struct FileEvent {
  WatchMarker wd; /* Watch descriptor */
  uint32_t mask; /* Mask describing event */
  uint32_t cookie; /* Unique cookie associating related events (for rename(2)) */
  uint32_t len; /* Size of name field */
  char namestartshere; //[];   //don't access this directly, it may not exist.
  /** @returns wrapper around null-terminated name */
  Indexer<char> name();

  /* inotify(7) man page: the length of each inotify_event structure is sizeof(struct inotify_event)+len.*/
};

/** bits for file watcher function calls.
 * These enums are bit numbers, not masks, ie 2 means 1<<2 at the lower level calls. */
struct FileWatch:Bundler<FileWatch> {
  enum On {
    Accessed, /* File was accessed.  */
    Modified, /* File was modified.  */
    Attributes, /* Metadata changed.  */
    CloseWrite, /* Writtable file was closed.  */
    CloseRead, /* Unwrittable file closed.  */
    //    #define IN_CLOSE	 (IN_CLOSE_WRITE | IN_CLOSE_NOWRITE) /* Close.  */
    Opened, /* File was opened.  */
    MovedFrom, /* File was moved from X.  */
    MovedTo, /* File was moved to Y.  */
    //    #define IN_MOVE		 (IN_MOVED_FROM | IN_MOVED_TO) /* Moves.  */
    ChildCreated, /* Subfile was created.  */
    ChildDeleted, /* Subfile was deleted.  */
    Deleted, /* Self was deleted.  */
    Moved, /* Self was moved.  */
  };

  //bit 12 ignored
  enum Was {
    /* Events sent by the kernel.  */
    Unmounted = 13, /* Backing fs was unmounted.  */
    OverflowedQueue, /* Event queued overflowed.  */
    Ignored, /* File was ignored.  */
    Directory = 30 /* Event occurred against dir.  */
  };

  enum Given {
    /* Special flags.  */
    IsDirectory = 24, /* Only watch the path if it is a directory.  */
    NotLink, /* Do not follow a sym link.  */
    NotUnlinked, /* Exclude events on unlinked objects. Relevant when file is a directory in which temp files are created.  */
    Additionally = 29, /* Add to the mask of an already existing watch.  */
    JustOnce = 31 /* Only send event once.  */
  };

  //use the above enums with BitWad,e.g.: BitWad<CloseWrite,Moved,Deleted>::mask
  WatchMarker wd;
  //need to add handler here, and then FileWatcher needs a list of Filewatches searchable by wd
  Hook<const FileEvent&> handler;

  /** @returns whether the event was pertinent, intended for use with @see Bundler::While */
  bool handle(const FileEvent& ev)  {
    if (ev.wd==wd) {
      handler(ev);
      return false;
    }
    return true;
  }
};


/** a wrapper around inotify calls.
 * One can create a single one of these for all the watching needs of an application.
 * The handler you provide would have to keep a list of wd/item handler callbacks.
 * To be useful you will pass this guy's fd to an Epoller waiting for reads.
 *
 * This guy may be a soliton as all FileWatches are in a single list and as such there is no real benefit to having more than one.
 */
class FileWatcher {
  /** the fd that does the watching, created by constructor. */
  Fildes fd;

public:
  FileWatcher(bool blocking = false);

  /** @returns handle of some sort, BadIndex on failure.
   * for @param mask use BitWad<comma list of enums>::mask */
  WatchMarker addWatch(const char *pathname, uint32_t mask);

  /** remove a watch that just fired off. Handy to stop watching a deleted file.*/
  bool removeWatch(FileEvent &fe);

  /** @returns whether there appears to be a FileEvent pending */
  bool hasEvent();

  /** process the next FileEvent in the queue. If @param handler is null the event still gets pulled from the queue.
   * your handler will have to look at a FileEvent.wd and compare that to what addWatch returned.
   */
  void nextEvent();
};
