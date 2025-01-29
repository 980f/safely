
/**
// Created by andyh on 1/29/25.
// Copyright (c) 2025 Andy Heilveil, (github/980f). All rights reserved.
*/

#include "event.h"
void Event::handle(short evflags) {
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

void Event::Thunk(int ignored, short evflags, void *obj) {
  Event &self = *static_cast<Event *>(obj);
  if (ignored != self.fd) {
    //then 980f doesn't understand libevent.
  }
  self.handle(evflags);
}

bool Event::isPending(decltype(type) which, MicroSeconds *due) {
  if (due) {
    *due = MicroSeconds::Never; //erase potentially stale value
  }
  return event_pending(raw, which ? which : type, due); //todo:1 see if we need to mask off mode bits from type.
}

Event::Looper::Looper(bool onething, unsigned numPriorities) { //dummy stuff just to test compile.
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

int Event::Looper::enroll(Event &ev) const {
  ev.raw = event_new(raw, ev.fd, ev.type, ev.thunker, ev.usuallyThis);
  if (ev.priority >= numPriorities) {
    ev.priority = numPriorities - 1;
  }
  event_priority_set(ev.raw, ev.priority); //todo: limit to priority max
  return event_add(ev.raw, nullptr); //todo: timeout argument, convolved with periodic flag which tends to EV_PERSIST flagging.
}

MicroSeconds Event::Looper::now(bool fresh) const {
  MicroSeconds result(MicroSeconds::Never);
  if (fresh) {
    event_base_update_cache_time(raw); //todo: check return
  }
  event_base_gettimeofday_cached(raw, &result);
  //todo: if ok
  return result;
}
