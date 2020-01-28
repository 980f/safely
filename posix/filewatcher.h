#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#include "fildes.h"

using WatchMarker =unsigned ;

struct FileWatch {
  enum On {
    Accessed,/* File was accessed.  */
    Modified,/* File was modified.  */
    Attributes,/* Metadata changed.  */
    CloseWrite,/* Writtable file was closed.  */
    CloseRead,/* Unwrittable file closed.  */
//    #define IN_CLOSE	 (IN_CLOSE_WRITE | IN_CLOSE_NOWRITE) /* Close.  */
    Opened,/* File was opened.  */
    MovedFrom,/* File was moved from X.  */
    MovedTo,/* File was moved to Y.  */
//    #define IN_MOVE		 (IN_MOVED_FROM | IN_MOVED_TO) /* Moves.  */
    ChildCreated,/* Subfile was created.  */
    ChildDeleted,/* Subfile was deleted.  */
    Deleted, /* Self was deleted.  */
    Moved,/* Self was moved.  */
  };
  //bit 12 ignored
  enum Was {
    /* Events sent by the kernel.  */
    Unmounted=13,/* Backing fs was unmounted.  */
    OverflowedQueue,/* Event queued overflowed.  */
    Ignored,/* File was ignored.  */
    Directory=30/* Event occurred against dir.  */

  };

  enum Given {
    /* Special flags.  */
    IsDirectory=24,/* Only watch the path if it is a directory.  */
    NotLink,/* Do not follow a sym link.  */
    NotUnlinked, /* Exclude events on unlinked objects. Relevent when file is a directory in which temp files are created.  */
    Additionally=29,/* Add to the mask of an already existing watch.  */
    JustOnce=31 /* Only send event once.  */
  };
  //use the above enums with BitWad: BitWad<CloseWrite,Moved,Deleted>::mask
  WatchMarker wd;
};

/** the layout of what gets read from a FileWatcher fd. */
struct FileEvent {
  WatchMarker wd;                            /* Watch descriptor */
  uint32_t mask;                     /* Mask describing event */
  uint32_t cookie;                   /* Unique cookie associating related events (for rename(2)) */
  uint32_t len;                      /* Size of name field */
  char     namestartshere;//[];   //don't access this directly, it may not exist.
  /** @returns wrapper around null-terminated name */
  Indexer<char> name();
  /* inotify(7) man page: the length of each inotify_event structure is sizeof(struct inotify_event)+len.*/
};


/** a wrapper around inotify calls.
 * One can create a single one of these for all the watching needs of an application.
 * The handler you provide would have to keep a list of wd/item handler callbacks.
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
  /** @returns whether their appears to be a FileEvent pending */
  bool hasEvent();
  using  FileEventHandler =void * (const FileEvent &);
  /** process the next FileEvent in the queue. If @param handler is null the event still gets pulled from the queue.
   * your handler will have to look at a FileEvent.wd and compare that to what addWatch returned.
*/
  void nextEvent(FileEventHandler *handler);
};

#endif // FILEWATCHER_H
