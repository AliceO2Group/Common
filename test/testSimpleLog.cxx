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

// most simple daemon example
// uses dummy default doLoop() function from the class
// helps to test e.g. command line parameters settings

#include <Common/SimpleLog.h>
#include <unistd.h>
#include <thread>
#include <vector>

void logWorker(SimpleLog& log, int threadId, int count) {
  for (int i = 0; i < count; ++i) {
    log.info("Thread %d - test message %d", threadId, i);
  }
}

int main()
{

  SimpleLog theLog;
  theLog.setLogFile("/tmp/test.log", 100, 4, 0);
  for (int i = 0; i < 10; i++) {
    theLog.info("test message %d", i);
  }
  // sleep(10);

  // test parallel threads
  theLog.setLogFile("/tmp/testthread.log", 1000, 10, 0);
  const int numThreads = 10;
  const int messagesPerThread = 10;
  std::vector<std::thread> threads;
  for (int t = 0; t < numThreads; ++t) {
    threads.emplace_back(logWorker, std::ref(theLog), t, messagesPerThread);
  }
  for (auto& th : threads) {
    th.join();
  }

  return 0;
}
