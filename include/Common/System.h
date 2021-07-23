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

/// \file System.h
/// \brief Definition of various useful utilities that don't really belong anywhere in particular
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef ALICEO2_FLPPROTOTYPE_COMMON_SYSTEM_H_
#define ALICEO2_FLPPROTOTYPE_COMMON_SYSTEM_H_

#include <set>
#include <string>
#include <boost/filesystem/path.hpp>

namespace AliceO2
{
namespace Common
{
namespace System
{

/// Sets the given function as the SIGINT handler
void setSigIntHandler(void (*function)(int));

/// Checks if there's a SIGINT handler installed (not sure if it actually works correctly)
bool isSigIntHandlerSet();

std::string executeCommand(const std::string& command);

/// Like the "mkdir -p" command.
/// TODO Currently it actually calls that command.. not very portable, should refactor
void makeParentDirectories(const std::string& path);

/// Similar to the "touch" Linux command
void touchFile(const std::string& path);

/// Get the file system type of the given directory
std::string getFileSystemType(const std::string& path);

/// Check if the file system of the given directory is any of the types given in the set
/// \return pair with:
///   bool: true if it is any of types
///   std::string: the found type
std::pair<bool, std::string> isFileSystemTypeAnyOf(const std::string& path, const std::set<std::string>& types);

/// Throws if the file system type of the given file/directory is not one of the given valid types
void assertFileSystemType(const std::string&, const std::set<std::string>& validTypes, std::string name);

} // namespace System
} // namespace Common
} // namespace AliceO2

#endif // ALICEO2_FLPPROTOTYPE_COMMON_SYSTEM_H_

