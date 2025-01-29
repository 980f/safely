/**
// Created by andyh on 1/29/25.
// Copyright (c) 2025 Andy Heilveil, (github/980f). All rights reserved.
*/

#pragma once

/** libevent wrapper */
#include <event.h>
#include <functional>
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
   * todo:1 use or do something similar to PosixWrapper to handle int returns that are pass/fail.
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

    int recheck() const {
      return event_base_loopcontinue(raw);
    }

    void stop() {
      event_base_loopbreak(raw);
    }

    bool stopped() {
      return event_base_got_break(raw);
    }

    void exitAfter(MicroSeconds delay) {
      event_base_loopexit(raw, &delay);
    }

    bool exited() const {
      return event_base_got_exit(raw);
    }

    //get shared timestamp
    MicroSeconds now(bool fresh = false) const;


    // f = event_base_get_features(base);
    // if ((f & EV_FEATURE_ET))
    //   printf("  Edge-triggered events are supported.");
    // if ((f & EV_FEATURE_O1))
    //   printf("  O(1) event notification is supported.");
    // if ((f & EV_FEATURE_FDS))
    //   printf("  All FD types are supported.");
  }; //end Looper


  /* the object returned here is an additional view into a shared object, beware that it is not independent other other references. */
  Looper owner() {
    return event_get_base(raw);
  }

  /** @see Looper::enroll() */
  int attachTo(Looper &myList) {
    return myList.enroll(*this);
  }

  /* fire an event ar random, not from event dispatcher */
  void misfire() {
    event_active(raw, 0, 0);
  }

  /////////////////

  static void exceptInsteadOfExt(int errcode) {
    throw std::system_error(errcode, std::system_category());//will likely exit, but canbe caught in a main to allow for recording overall state when  libevent croaked.
  }

  static void libraryInit() {
    event_set_fatal_callback(&exceptInsteadOfExt);
  }
};

/*an event with no file or data flow, just a handler*/
class TimerEvent : public Event {
public:
  TimerEvent() {
    //most base class defaults are fine
    type = EV_TIMEOUT;
  }

  std::function<void()> lambder;

  void onTimeoutEvent(bool onRead, bool onWrite) override {
    lambder();
  }
};

/** wraps libevent bufferevent */
class FileEvent : public Event {
  public:

};
