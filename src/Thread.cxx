// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include <Common/Thread.h>
#include <stdlib.h>
#include <unistd.h>
using namespace AliceO2::Common;

Thread::Thread(Thread::CallbackResult (*vLoopCallback)(void*), void* vLoopArg, std::string vThreadName, int vLoopSleepTime)
{
  shutdown = 0;
  running = 0;
  theThread = NULL;
  name = vThreadName;
  loopCallback = vLoopCallback;
  loopArg = vLoopArg;
  loopSleepTime = vLoopSleepTime;
}

Thread::~Thread()
{
  if (theThread != NULL) {
    stop();
    join();
  }
}

void Thread::start()
{
  if (theThread == NULL) {
    shutdown = 0;
    running = 0;
    theThread = new std::thread(threadMain, this);
  }
  return;
}

void Thread::stop()
{
  if (theThread != NULL) {
    shutdown = 1;
  }
}

void Thread::join()
{
  if (theThread != NULL) {
    shutdown = 1;
    theThread->join();
    delete theThread;
    theThread = NULL;
  }
  return;
}

void Thread::threadMain(Thread* e)
{
  e->running = 1;
  int maxIterOnShutdown = 100;
  int nIterOnShutdown = 0;

  for (;;) {
    if (e->shutdown) {
      if (nIterOnShutdown >= maxIterOnShutdown)
        break;
      nIterOnShutdown++;
    }
    int r = e->doLoop();
    if (r == Thread::CallbackResult::Ok) {
    } else if (r == Thread::CallbackResult::Idle) {
      if (e->shutdown)
        break; // exit immediately on shutdown
      if (e->loopSleepTime > 0) {
        usleep(e->loopSleepTime);
      }
    } else if (r == Thread::CallbackResult::Error) {
      // account this error... maybe do something if repetitive
      if (e->shutdown)
        break; // exit immediately on shutdown
    } else {
      break;
    }
  }
  e->running = 0;
}

Thread::CallbackResult Thread::doLoop()
{
  if (loopCallback != NULL) {
    return loopCallback(loopArg);
  }
  return Thread::CallbackResult::Idle;
}

std::string Thread::getName()
{
  return name;
}
