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

/// \file GuardFunction.h
/// \brief Definition of various useful utilities that don't really belong anywhere in particular
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef ALICEO2_FLPPROTOTYPE_COMMON_GUARDFUNCTION_H_
#define ALICEO2_FLPPROTOTYPE_COMMON_GUARDFUNCTION_H_

#include <functional>

namespace AliceO2
{
namespace Common
{

/// Class that executes a function on scope exit
class GuardFunction
{
 public:
  /// \param function Function to execute on scope exit. Must not throw.
  GuardFunction(std::function<void()> function)
    : mFunction(function)
  {
  }

  ~GuardFunction()
  {
    mFunction();
  }

 private:
  std::function<void()> mFunction;
};

} // namespace Common
} // namespace AliceO2

#endif // ALICEO2_FLPPROTOTYPE_COMMON_GUARDFUNCTION_H_
