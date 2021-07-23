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

///
/// \file    Timer.h
/// \brief   Class to implement a high resolution timer function
/// \author  Sylvain Chapeland
///

#ifndef COMMON_TIMER_H
#define COMMON_TIMER_H

#include <chrono>

namespace AliceO2
{
namespace Common
{

/// \brief   Class to implement a high resolution timer function.
/// All times in microseconds.
/// Functionality: reset counter, set timeout value, get counter value, check timeout, fixed interval timeout.
/// \author   Sylvain Chapeland
class Timer
{
 public:
  /// Constructor
  /// Timer is reset.
  Timer();
  /// Destructor
  ~Timer();

  /// Reset timer. Accepts an optionnal timeout value.
  /// param timeout (In) Timeout value, in microseconds since reset. (optional)
  void reset(int timeout = 0);

  /// Reset timer by adding timeout value to starting time value.
  /// Usefull when called in loops, prevents loosing time elapsed after previous timeout.
  /// (e.g.: to implement timeout every 1 second without drift in time)
  void increment();

  /// Check if time elapsed since timer reset (or last increment set) is bigger than timeout value set.
  /// \return Returns 1 if timeout, 0 otherwise.
  int isTimeout();

  /// \return Returns time elapsed since reset, in seconds.
  double getTime();

  /// \return Returns the time until next timeout, in seconds. This may be a negative value if timeout occured already.
  double getRemainingTime();

 private:
  std::chrono::time_point<std::chrono::high_resolution_clock> t0; // time of reset
  double tmax;                                                    // duration between reset and timeout condition, in seconds
};

} // namespace Common
} // namespace AliceO2

#endif // COMMON_TIMER_H

