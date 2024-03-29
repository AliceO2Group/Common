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

/// \file SuffixNumber.h
/// \brief Definition of the SuffixOption class.
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef ALICEO2_FLPPROTOTYPE_COMMON_SUFFIXNUMBER_H_
#define ALICEO2_FLPPROTOTYPE_COMMON_SUFFIXNUMBER_H_

#include <istream>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include "Common/Exceptions.h"

namespace AliceO2
{
namespace Common
{
namespace _SuffixNumberTable
{
const std::vector<std::pair<std::string, const size_t>>& get();
} // namespace _SuffixNumberTable

/// Number with SI suffix.
/// Supported suffixes:
/// - Power of 10: k, M, G, T, P, E
/// - Power of 2: Ki, Mi, Gi, Ti, Pi, Ei
template <typename Number>
class SuffixNumber
{
 public:
  using NumberType = Number;

  SuffixNumber() : mNumber(0)
  {
  }

  SuffixNumber(Number number) : mNumber(number)
  {
  }

  SuffixNumber(std::string input) : mNumber(parse(input))
  {
  }

  Number getNumber() const
  {
    return mNumber;
  }

  void setNumber(std::string input)
  {
    mNumber = parse(input);
  }

  void setNumber(Number number)
  {
    mNumber = number;
  }

 private:
  Number parse(std::string input) const
  {
    // Find the non-numeric suffix
    auto pos = input.find_first_not_of("-.0123456789");

    // Convert numeric part
    auto numberString = input.substr(0, pos);
    Number number;
    if (!boost::conversion::try_lexical_convert<Number>(numberString, number)) {
      BOOST_THROW_EXCEPTION(Exception() << ErrorInfo::Message("Could not convert number")
                                        << ErrorInfo::Input(numberString));
    }

    if (pos == std::string::npos) {
      // There's no unit
      return number;
    }

    // Find unit and multiply number with it
    auto unitString = input.substr(pos);
    for (const auto& unit : _SuffixNumberTable::get()) {
      if (unitString == unit.first) {
        // We found the right unit, multiply it with the number
        Number a = number;
        Number b = unit.second;
        Number multiplied = a * b;
        if (std::is_integral<Number>::value) {
          // Check for overflow for integers
          if ((a != 0) && ((multiplied / a) != b)) {
            BOOST_THROW_EXCEPTION(Exception() << ErrorInfo::Message("Number too large for representation")
                                              << ErrorInfo::Input(input));
          }
        }
        return multiplied;
      }
    }
    BOOST_THROW_EXCEPTION(Exception() << ErrorInfo::Message("Unrecognized unit") << ErrorInfo::Suffix(unitString));
  }

  Number mNumber;
};

template <typename T>
std::istream& operator>>(std::istream& stream, SuffixNumber<T>& suffixNumber)
{
  std::string string;
  stream >> string;
  suffixNumber.setNumber(string);
  return stream;
}

template <typename T>
std::ostream& operator<<(std::ostream& stream, const SuffixNumber<T>& suffixNumber)
{
  return stream << suffixNumber.getNumber();
}

} // namespace Common
} // namespace AliceO2

#endif // ALICEO2_FLPPROTOTYPE_COMMON_SUFFIXNUMBER_H_

