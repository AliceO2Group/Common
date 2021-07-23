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

/// \file Exception.cxx
/// \brief Implementation of the RORC exceptions and related functions.
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#include "Common/Exception.h"
#include <stdexcept>
#include <boost/exception/all.hpp>

namespace AliceO2
{
namespace Common
{

const char* Exception::what() const noexcept
{
  try {
    if (auto info = boost::get_error_info<ErrorInfo::Message>(*this)) {
      return info->data();
    } else {
      return "AliceO2::Common::Exception";
    }
  } catch (const std::exception& e) {
    return "AliceO2::Common::Exception";
  }
}

} // namespace Common
} // namespace AliceO2

