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

#ifndef DATAFORMAT_MEMPOOL
#define DATAFORMAT_MEMPOOL

#include <atomic>

// class to create a pool of memory pages from standard allocated memory.
// lock-free mechanism for fast concurrent page request/release

class MemPool
{
 public:
  MemPool(int numberOfPages, int pageSize = 1024 * 1024, int align = 1024);
  ~MemPool();

  void* getPage();              // thread-safe ... can be called in parallel
  void releasePage(void* page); // thread-safe ... can be called in parallel

  int getPageSize();

 private:
  void deletePages(); // release memory allocated for data members of this object

  int numberOfPages;
  int pageSize;

  void** pageTable;             // table with list of allocated pages
  std::atomic_flag* pageIsUsed; // table to flag pages in use

  std::atomic<int> lastPageIndexGet;     // cache for last page index reserved, will start search of free page from there
  std::atomic<int> lastPageIndexRelease; // cache for last page index reserved, will start search of free page from there
};

#endif

