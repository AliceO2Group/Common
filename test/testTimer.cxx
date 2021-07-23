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

#include "../include/Common/Timer.h"

#define BOOST_TEST_MODULE Timer test
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <cmath>
#include <boost/test/unit_test.hpp>
#include <assert.h>

#include <time.h>

BOOST_AUTO_TEST_CASE(timer_test)
{
  AliceO2::Common::Timer t;
  time_t t0;
  t0 = time(NULL);

  while (time(NULL) == t0) {
  }
  t.reset();
  while (time(NULL) <= t0 + 1) {
  }

  double elapsed = t.getTime();

  printf("Timer elapsed=%.4lf\n", elapsed);
  int success = 0;
  if (std::fabs(elapsed - 1.0) < 0.01) {
    success = 1;
  }

  BOOST_CHECK_EQUAL(success, 1);
}

