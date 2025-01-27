
/**
// Created by andyh on 1/26/25.
// Copyright (c) 2025 Andy Heilveil, (github/980f). All rights reserved.
*/

#include "timedevent.h"

void TimedEvent::beforeChange(int newFD) {
  TimerFD::beforeChange(newFD);
  if (fd!=BADFD) {
    watcher.remove(fd);
  }
}

void TimedEvent::afterChange() {
  TimerFD::afterChange();
  if (fd!=BADFD) {
    watcher.watch(fd,EPOLLIN,thunker());
  }
}

void TimedEvent::onEpoll(unsigned flags) {
  if (flags&EPOLLIN) {
    auto expirations = read(uint64_t(~0)); //#type here is chosen by timer fd stuff, not us.
    onFired(expirations);
  }
}
