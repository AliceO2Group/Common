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

#include "../include/Common/Fifo.h"

#define BOOST_TEST_MODULE Fifo test
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <assert.h>

BOOST_AUTO_TEST_CASE(fifo_test)
{
  int fifoSz = 100;

  AliceO2::Common::Fifo<int> f(fifoSz);
  int* v = new int[fifoSz];
  int j = -1;
  int sum1 = 0;
  int sum2 = 0;

  BOOST_CHECK_EQUAL(f.isEmpty(), 1);
  BOOST_CHECK_EQUAL(f.isFull(), 0);
  BOOST_CHECK_PREDICATE(std::not_equal_to<int>(), (f.pop(j))(0));
  for (int i = 0; i < fifoSz; i++) {
    v[i] = i;
    sum1 += i;
    BOOST_CHECK_EQUAL(f.isFull(), 0);
    BOOST_CHECK_EQUAL(f.push(v[i]), 0);
    BOOST_CHECK_EQUAL(f.isEmpty(), 0);
  }
  BOOST_CHECK_EQUAL(f.isFull(), 1);
  BOOST_CHECK_PREDICATE(std::not_equal_to<int>(), (f.push(-1))(0));
  BOOST_CHECK_EQUAL(f.front(j), 0);
  BOOST_CHECK_EQUAL(j, 0);

  for (int i = 0; i < fifoSz; i++) {
    j = -1;
    BOOST_CHECK_EQUAL(f.isEmpty(), 0);
    BOOST_CHECK_EQUAL(f.pop(j), 0);
    BOOST_CHECK_EQUAL(f.isFull(), 0);
    sum2 += j;
  }
  BOOST_CHECK_EQUAL(f.isEmpty(), 1);
  BOOST_CHECK_PREDICATE(std::not_equal_to<int>(), (f.pop(j))(0));

  BOOST_CHECK_EQUAL(sum1, sum2);
  delete[] v;

  printf("fifoSz=%d sum=%d\n", fifoSz, sum2);
}
