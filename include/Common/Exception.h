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

/// \file   Exception.h
/// \author Pascal Boeschoten
///
/// \brief  Define an exceptions based on boost::exception

#ifndef COMMON_EXCEPTION_H
#define COMMON_EXCEPTION_H

#include <exception>
#include <string>
#include <boost/exception/all.hpp>

namespace AliceO2
{
namespace Common
{

namespace ErrorInfo
{
using Message = boost::error_info<struct _Message, std::string>;
using Input = boost::error_info<struct _Input, std::string>;
using Suffix = boost::error_info<struct _Suffix, std::string>;
using FileName = boost::error_info<struct _FileName, std::string>;
using FilesystemType = boost::error_info<struct _FileSystemType, std::string>;
} // namespace ErrorInfo

struct Exception : virtual std::exception, virtual boost::exception {
  /// The what() function is overridden to use the 'ErrorInfo::Message' when available
  virtual const char* what() const noexcept override;
};

struct ProgramOptionException : virtual Exception {
};

} // namespace Common
} // namespace AliceO2

#endif //COMMON_EXCEPTION_H

