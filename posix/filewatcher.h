#pragma once

#include <bundler.h>
#include <hook.h>

#include "fildes.h"
#include <sys/inotify.h>
using WatchMarker = decltype(inotify_event::wd);

/* our reconstruction of inotify_event will not have the filename attached to the tail end.
 * Do not use the name member of the contained inotify_event, only use the filename wrapper object,and note that the FileEvent you get passed is likely to be stack allocated and must be deep copied if you are going to retain it when your handler is called.
 */
struct FileEvent {
  Indexer<uint8_t>filename;
  //inotify_event must come last as it has a flex array member, which we aren't going to use but must accomodate.
  inotify_event ent;//reason for name is evident at points of use.
};

struct FileWatch:Bundler<FileWatch> {
  //being derived from Bundler means that all instances are in a global list, access by static functions of Bundler<> class.
/** bits for file watcher function calls.
 * These enums are bit numbers, not masks, ie 2 means 1<<2 at the lower level calls. */
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
  bool handle(const FileEvent& ev);
};


/** a wrapper around inotify calls.
 * One can create a single one of these for all the watching needs of an application.
 * The handler you provide would have to keep a list of wd/item handler callbacks.
 * To be useful you will pass this guy's fd to an Epoller waiting for reads.
 *
 * This guy may be a soliton as all FileWatches are in a single list in the kernel and as such there is no real benefit to having more than one.
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
