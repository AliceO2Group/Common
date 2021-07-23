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
/// @file    Timer.cxx
/// @author  Sylvain Chapeland
///

#include "Common/Timer.h"

namespace AliceO2
{
namespace Common
{

Timer::Timer()
{
  reset(0);
}

Timer::~Timer()
{
}

void Timer::reset(int timeout)
{
  t0 = std::chrono::high_resolution_clock::now();
  tmax = timeout / 1000000.0;
}

void Timer::increment()
{
  t0 += std::chrono::microseconds((int)(tmax * 1000000.0));
}

int Timer::isTimeout()
{
  std::chrono::duration<double> tdiff = std::chrono::high_resolution_clock::now() - t0;
  if (tdiff.count() >= tmax) {
    return 1;
  }
  return 0;
}

double Timer::getTime()
{
  std::chrono::duration<double> tdiff = std::chrono::high_resolution_clock::now() - t0;
  return tdiff.count();
}

double Timer::getRemainingTime()
{
  std::chrono::duration<double> tdiff = std::chrono::high_resolution_clock::now() - t0;
  return tmax - tdiff.count();
}

} // namespace Common
} // namespace AliceO2

