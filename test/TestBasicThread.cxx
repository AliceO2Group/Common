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

#include "Common/BasicThread.h"
#include <chrono>
#include <thread>
#include <functional>

#define BOOST_TEST_MODULE BasicThread
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(TestBasicThread)
{
  AliceO2::Common::BasicThread thread;
  std::atomic<int64_t> number;
  thread.start([&](std::atomic<bool>* stopFlag) {
    while (!stopFlag->load(std::memory_order_relaxed)) {
      number++;
    }
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  thread.join();
  BOOST_CHECK(number > 0);
}

