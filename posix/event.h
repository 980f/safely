/**
// Created by andyh on 1/29/25.
// Copyright (c) 2025 Andy Heilveil, (github/980f). All rights reserved.
*/

#pragma once

/** libevent wrapper */
#include <functional>
#include <microseconds.h>
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

  virtual void handle(short evflags) {
    bool reader = evflags & EV_READ;
    bool writer = evflags & EV_WRITE;

    if (evflags & EV_TIMEOUT) {
      onTimeoutEvent(reader, writer);
    } else {
      if (reader) {
        //process some data.
        onReadEvent();
      }
      //# no else here, read and write can be simultaneous.
      if (writer) {
        onWriteEvent();
      }
    }
  }

  static void Thunk(evutil_socket_t ignored, short evflags, void *obj) {
    Event &self = *static_cast<Event *>(obj);
    if (ignored != self.fd) {
      //then 980f doesn't understand libevent.
    }
    self.handle(evflags);
  }

  int del() {
    return event_del(raw);
  }

  int killTimeout() {
    return event_remove_timer(raw);
  }

  /* this is not useful, other than a sa linkage check.*/
  bool isPending(MicroSeconds *due) {
    if (due) {
      *due= MicroSeconds::Never;
    }
    return event_pending(raw,EV_TIMEOUT, due);
  }

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

    ~Looper() {
      event_base_free(raw);
    }

    /**if not using defaults call this.*/
    Looper(bool onething, unsigned numPriorities) { //dummy stuff just to test compile.
      Configurator config;
      if (onething) {
        config.set_flag(EVENT_BASE_FLAG_PRECISE_TIMER);
      }
      raw = event_base_new_with_config(config.raw);

      if (numPriorities > 0) { //todo: check EVENT_MAX_PRIORITIES
        event_base_priority_init(raw, numPriorities);
        this->numPriorities = event_base_get_npriorities(raw); //looping through the library
      }
    }

    int enroll(Event &ev) {
      ev.raw = event_new(raw, ev.fd, ev.type, ev.thunker, ev.usuallyThis);
      if (ev.priority >= numPriorities) {
        ev.priority = numPriorities - 1;
      }
      event_priority_set(ev.raw, ev.priority); //todo: limit to priority max
      return event_add(ev.raw, nullptr); //todo: timeout argument, convolved with periodic flag which tends to EV_PERSIST flagging.
    }

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
    MicroSeconds now(bool fresh = false) {
      MicroSeconds result(MicroSeconds::Never);
      if (fresh) {
        event_base_update_cache_time(raw); //todo: check return
      }
      event_base_gettimeofday_cached(raw, &result);
      //todo: if ok
      return result;
    }


    // f = event_base_get_features(base);
    // if ((f & EV_FEATURE_ET))
    //   printf("  Edge-triggered events are supported.");
    // if ((f & EV_FEATURE_O1))
    //   printf("  O(1) event notification is supported.");
    // if ((f & EV_FEATURE_FDS))
    //   printf("  All FD types are supported.");
  }; //end Looper

  int attachTo(Looper &myList) {
    this->thunker = &Event::Thunk;
    this->usuallyThis = this;
    return myList.enroll(*this);
  }
};

/*an event with no file or data flow, just a handler*/
class TimerEvent : public Event {
public:
  TimerEvent() {
    //most base class defaults are fine
    type=EV_TIMEOUT;
  }
  std::function<void()> lambder;
  void onTimeoutEvent(bool onRead, bool onWrite) override {
    lambder();
  }
};
