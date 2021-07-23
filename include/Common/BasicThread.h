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

/// \file BasicThread.h
/// \brief Definition of BasicThread class
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef ALICEO2_FLPPROTOTYPE_COMMON_BASICTHREAD_H_
#define ALICEO2_FLPPROTOTYPE_COMMON_BASICTHREAD_H_

#include <atomic>
#include <iostream>
#include <functional>
#include <stdexcept>
#include <system_error>
#include <string>
#include <thread>

namespace AliceO2
{
namespace Common
{

/// Simple thread class that executes a function in a new thread. The function takes an atomic flag which indicates
/// the function should stop its work.
class BasicThread
{
 public:
  /// Start thread
  /// \param stopFlag Pointer to flag indicating the function should stop
  void start(std::function<void(std::atomic<bool>* stopFlag)> function)
  {
    join();
    mStopFlag = false;
    mThread = std::thread(function, &mStopFlag);
  }

  void stop()
  {
    mStopFlag = true;
  }

  void join()
  {
    stop();
    if (mThread.joinable()) {
      mThread.join();
    }
  }

  ~BasicThread()
  {
    try {
      join();
    } catch (const std::system_error& e) {
      std::cout << "Failed to join thread: " << e.what() << '\n';
    } catch (const std::exception& e) {
      std::cout << "Unexpected exception while joining thread: " << e.what() << '\n';
    }
  }

 private:
  /// Thread object
  std::thread mThread;

  /// Flag to stop the thread
  std::atomic<bool> mStopFlag;
};

} // namespace Common
} // namespace AliceO2

#endif // ALICEO2_FLPPROTOTYPE_COMMON_BASICTHREAD_H_

