/**
// Created by andyh on 1/29/25.
// Copyright (c) 2025 Andy Heilveil, (github/980f). All rights reserved.
*/

#pragma once

/** libevent wrapper */
#include <event.h>
#include <functional>
#include <hook.h>
#include <microseconds.h>
#include <system_error>
#include_next <event2/event.h>

/** libevent insist on dynamically creating event trackers. We wish to be able to statically declare them so our base class is configuration which contains a pointer to the runtime handle.*/
class Event {
protected:
  ~Event() = default;

  /** library's stuff*/
  event *raw = nullptr;
  /* configuration fields, changes may nottake effect without notifying the event_base (Looper) */
  evutil_socket_t fd = ~0; //an fd that is often that of a socket
  unsigned priority = ~0; //~0 will give lowest priority
  short type = 0; //combo of read|write|timeout|signal, and perhaps edge triggered and persistent
  /* libevent will call this function with parameter usuallyThis. So, the thunker should cast usuallyThis into an appropriate class and call that classes event handler with this event and the context arg that libevent passes to the thunker*/
  event_callback_fn thunker = nullptr; //
  void *usuallyThis = nullptr; //union of everything sizeof pointer or less.

  virtual void onReadEvent() {}
  virtual void onWriteEvent() {}
  virtual void onErrorEvent(bool onRead, bool onWrite) {}
  virtual void onTimeoutEvent(bool onRead, bool onWrite) {}

  virtual void handle(short evflags);

  static void Thunk(evutil_socket_t ignored, short evflags, void *obj);

  int del() {
    return event_del(raw);
  }

  int killTimeout() {
    return event_remove_timer(raw);
  }

  /* if @param which is 0 then the configured type is used for the 'pending' check: */
  bool isPending(decltype(type) which, MicroSeconds *due = nullptr);

public:
  /** the driver, listener, what have you, called "base" but "root" or "list" are better descriptions .
   * This is inside Event for easy access to its members, as it is the prime consumer of those members.
   *
   * todo:1 use or do something similar to PosixWrapper to handle int returns that are pass/fail.
   *
   */
  class Looper {
    struct Configurator {
      event_config *raw;

      Configurator() {
        raw = event_config_new();
      }

      int set_flag(event_base_config_flag flag) {
        return event_config_set_flag(raw, flag);
      }

      //todo: avoid_method, require_features , max_dispatch_interval(timeval)
      ~Configurator() {
        event_config_free(raw);
      }
    };

    event_base *raw;
    int numPriorities = 0; //zero is "default" which is presently ==1 .

  public:
    Looper() {
      raw = event_base_new();
    }

    Looper(event_base *existing) {
      raw = existing;
      numPriorities = event_base_get_npriorities(existing);
      //NB: if we cache more fields on the object we must recover them here for guaranteed functioning.
    }

    bool operator==(const Looper &other) const {
      return raw == other.raw;
    }

    ~Looper() {
      event_base_free(raw);
    }

    /**if not using defaults call this.*/
    Looper(bool onething, unsigned numPriorities);

    int enroll(Event &ev) const;

    ///////////////////////////////
    /// loop invocation and management
    ///

    int operator()(bool once, bool nonBlocking, bool ignoreEmpty) {
      return event_base_loop(raw, once | nonBlocking << 1 | ignoreEmpty << 2); //todo:make this formally correct
    }

    /* an event handler might call this to move the list iterator back to a higher priority, so that an event just created gets handled before pending but lower priority ones do*/
    int recheck() const {
      return event_base_loopcontinue(raw);
    }

    /* an event handler can call this to stop event polling */
    void stop() {
      event_base_loopbreak(raw);
    }

    /** @returns whether the loop exited due to a call to stop() */
    bool stopped() {
      return event_base_got_break(raw);
    }

    /** a handler can call this to have the loop terminate soon. Most sensible use is to call this before invoking the loop instead of having a timer event calling stop() */
    void exitAfter(MicroSeconds delay) {
      event_base_loopexit(raw, &delay);
    }

    /** @returns whether the loop terminated due to a timed exit (@see exitAfter) */
    bool exited() const {
      return event_base_got_exit(raw);
    }

    /* get shared timestamp, if @param fresh is true then updates it.
     * The value here is normally updated once per scan of the event list so that all events handled in one pass have a common timestamp.
     */
    MicroSeconds now(bool fresh = false) const;


    // f = event_base_get_features(base);
    // if ((f & EV_FEATURE_ET))
    //   printf("  Edge-triggered events are supported.");
    // if ((f & EV_FEATURE_O1))
    //   printf("  O(1) event notification is supported.");
    // if ((f & EV_FEATURE_FDS))
    //   printf("  All FD types are supported.");
  }; //end Looper


  /* the object returned here is an additional view into a shared object, beware that it is not independent of other references and most especially is not the same object as the event was enroll with. It works great as a local variable used in the Event's handler, but should not be part of a struct or otherwise static entity */
  Looper owner() {
    return event_get_base(raw);
  }

  /** @see Looper::enroll() */
  int attachTo(Looper &myList) {
    return myList.enroll(*this);
  }

  /* fire an event ar random, not from event dispatcher.
   * The humorous name is to get you to think long and hard before calling this, the execution environment isn't the same as when fired by a Looper, especially including that the Looper timestamp is (pootentially) garbage or and NPE depending upon whether this event was ever enrolled */
  void misfire() {
    event_active(raw, 0, 0);
  }

  /////////////////

  /** this is designed to convert an attempt by libevent to kill the process into an exception thrown to the code that is either fatally misconfiguring a Looper before starting it, or from whereever a loop poll is invoked */
  static void exceptInsteadOfExt(int errcode) {
    throw std::system_error(errcode, std::system_category());//will likely exit, but canbe caught in a main to allow for recording overall state when  libevent croaked.
  }

  /* sets up Event and Looper independent features of the library, such as how to exit when it gets upset at some error */
  static void libraryInit() {
    event_set_fatal_callback(&exceptInsteadOfExt);
  }
};

/* an event with no file or data flow, just a handler */
class TimerEvent : public Event {
public:
  TimerEvent() {
    //most base class defaults are fine
    type = EV_TIMEOUT;
  }

  Hook<> lambder;

  void onTimeoutEvent(bool onRead, bool onWrite) override {
    lambder();
  }
};

/** wraps libevent bufferevent
 * First up is file to socket, for http sending,
 * then socket to application, for http request reception.
 *
 * The associated http library will be mined for info, but this is tying libevent to safely and that is best done at a lower level.
 *
 */
class FileEvent : public Event {
  public:

};
