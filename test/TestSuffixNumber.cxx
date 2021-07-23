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

#include "Common/SuffixNumber.h"

#define BOOST_TEST_MODULE SuffixNumber
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using AliceO2::Common::SuffixNumber;

BOOST_AUTO_TEST_CASE(TestInteger)
{
  SuffixNumber<int32_t> sn;

  sn.setNumber("0");
  BOOST_CHECK_EQUAL(sn.getNumber(), 0);

  sn.setNumber("2147483647");
  BOOST_CHECK_EQUAL(sn.getNumber(), 2147483647);
  BOOST_CHECK_THROW(sn.setNumber("2147483648"), AliceO2::Common::Exception);

  sn.setNumber("-2147483648");
  BOOST_CHECK_EQUAL(sn.getNumber(), -2147483648);
  BOOST_CHECK_THROW(sn.setNumber("-2147483649"), AliceO2::Common::Exception);
}

BOOST_AUTO_TEST_CASE(TestFloatingPoint)
{
  SuffixNumber<double> sn;

  sn.setNumber("0");
  BOOST_CHECK_EQUAL(sn.getNumber(), 0);

  sn.setNumber("123");
  BOOST_CHECK_EQUAL(sn.getNumber(), 123);

  sn.setNumber("-123");
  BOOST_CHECK_EQUAL(sn.getNumber(), -123);
}

