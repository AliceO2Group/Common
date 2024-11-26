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

/// \file SuffixNumber.cxx
/// \brief Implementation of the SuffixNumber class.
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#include "Common/SuffixNumber.h"
#include <vector>

namespace AliceO2
{
namespace Common
{
namespace _SuffixNumberTable
{

const std::vector<std::pair<std::string, const size_t>>& get()
{
  static const std::vector<std::pair<std::string, const size_t>> units{
    { "k", 1000 },
    { "M", 1000000 },
    { "G", 1000000000 },
    { "T", 1000000000000 },
    { "P", 1000000000000000 },
    { "E", 1000000000000000000 },
    //    {"Z", 1000000000000000000000}, // Too large
    //    {"Y", 1000000000000000000000000}, // Too large
    { "Ki", 1024 },
    { "Mi", 1048576 },
    { "Gi", 1073741824 },
    { "Ti", 1099511627776 },
    { "Pi", 1125899906842624 },
    { "Ei", 1152921504606846976 },
    //    {"Zi", 1180591620717411303424}, // Too large
    //    {"Yi", 1208925819614629174706176}, // Too large
  };
  return units;
}

} // namespace _SuffixNumberTable
} // namespace Common
} // namespace AliceO2
