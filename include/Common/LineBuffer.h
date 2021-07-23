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

///
/// \file    LineBuffer.h
/// \brief   Class to implement a buffer to read from file descriptor and get out data in lines.
/// \author  Sylvain Chapeland
///

#include <string>
#include <queue>

class LineBuffer
{
 public:
  /// Constructor
  LineBuffer();

  /// Destructor
  ~LineBuffer();

  /// Read from file descriptor and add content to buffer
  /// \param[in] fd          file descriptor to read from
  /// \param[in] timeout     timeout in milliseconds, -1 for blocking call
  /// \return 0 on success, -1 if EOF
  int appendFromFileDescriptor(int fd, int timeout);

  // Retrieve next complete line from buffer, immediate returns.
  /// \param[out] nextLine    Next complete line from buffer.
  /// \return 0 on success, -1 if no complete line yet
  int getNextLine(std::string& nextLine);

  //void appendString(const char *s);

 private:
  std::queue<std::string> completeLines;
  std::string pendingLine;
};

