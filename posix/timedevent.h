/**
// Created by andyh on 1/26/25.
// Copyright (c) 2025 Andy Heilveil, (github/980f). All rights reserved.
*/

#pragma once
#include <epoller.h>
#include <timerfd.h>

/* extend this with your event handler as onFired  */
class TimedEvent : TimerFD, EpollHandler {
public:
  /* @see the base classes for what each paramter is for. */
  TimedEvent(const char *traceName, bool phaseLock, EpollerCore &watcher) : TimerFD{traceName, phaseLock}, watcher{watcher} {}

private:
  EpollerCore &watcher;

protected:
  void beforeChange(int newFD) override;

  void afterChange() override;

  virtual void onFired(unsigned times) {
    /* override to actually do something */
  }

public:
  void onEpoll(unsigned flags) override;
};
