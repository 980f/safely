#ifndef EVENTLOOP_H
#define EVENTLOOP_H

/** poll() based event manager.
 * intended use is to single thread an event driven program. All events have to be detected by this guy or difficulties arise, such as not being able to add a new event to the list.
 * If all events are made interesting due to some other event then there is no problem. To be robust against simple design problems one can set a periodic timer event that inspects system state and determines which events ought to be enabled.
 *
 * an io operation timeout is implemented via adding a timer fd.
 *
 * It is expected that you will have at most one of these per thread, that you are using event driving rather than blocking threads to implement your application.
 *
 * todo: find out how the asynch io lib waits, to see what thread it calls events upon and whether we can subordinate that to this loop so that we don't have multi-thread issues.
*/

#include <vector>
#include "poll.h"

enum EventType {
  /* Event types that can be polled for.  These bits may be set in `events'
     to indicate the interesting event types; they will appear in `revents'
     to indicate the status of the file descriptor.  */
  inbound= POLLIN,//		0x001		/* There is data to read.  */
  ooband=POLLPRI,//		0x002		/* There is urgent data to read.  */
  outbound=POLLOUT,//		0x004		/* Writing now will not block.  */

//  #if defined __USE_XOPEN || defined __USE_XOPEN2K8
//  /* These values are defined in XPG4.2.  */
//  # define POLLRDNORM	0x040		/* Normal data may be read.  */
//  # define POLLRDBAND	0x080		/* Priority data may be read.  */
//  # define POLLWRNORM	0x100		/* Writing now will not block.  */
//  # define POLLWRBAND	0x200		/* Priority data may be written.  */
//  #endif

//  #ifdef __USE_GNU
//  /* These are extensions for Linux.  */
//  # define POLLMSG	0x400
//  # define POLLREMOVE	0x1000
//  # define POLLRDHUP	0x2000
//  #endif

  /* Event types always implicitly polled for.  These bits need not be set in `events', but they will appear in `revents' to indicate the status of the file descriptor.  */
  error= POLLERR,//		0x008		/* Error condition.  */
  hangup=POLLHUP,//		0x010		/* Hung up.  */
  invalid=POLLNVAL,//	0x020		/* Invalid polling request.  */

};

class EventLoop {
  using Handler= void(*)(void);//todo: args?
  std::vector<pollfd> fdlist;
  std::vector<Handler> handlers;
  unsigned fdused;
  int nextTimeout;
  bool beRunning;
public:
  EventLoop();
  /** this function does not normally return, the thread it is invoked upon blocks.
   * it returns for SIGHUP and the like, while SIGSEGV from callbacks might get logged and ignored.
*/
  void run();
  void cancel();//cancels *all* events, via stopping the loop.
  /** @returns a handle number by which to cancel or re-up an installed watch */
  int addInputWatch(int fd, Handler watcher);
  /** cancel a watcher, @param watchID is what was returned by add..Watch, if ~0 then the first watcher with the @param given fd is cancelled, sometimes that is the wrong one. If watchID is still valid then the watcher must match originalfd to be cancelled. IE watchID is like a hashcode, to expedite finding the item with originalfd */
  void cancelWatch(int originalfd,int watchID);
  /** dispatch events, usually not valid to call this except from within the run(), but we like to module test by faking up the poll() return then calling this.*/
  void dispatch();
};

#endif // EVENTLOOP_H
