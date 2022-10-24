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

#include <Common/SimpleLog.h>

#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include <ctype.h>
#include <string.h>
#include <algorithm>
#include <string>

class SimpleLog::Impl
{
 public:
  // constructor
  Impl();

  // destructor
  ~Impl();

  // list of possible message severity levels
  enum Severity { Info,
                  Error,
                  Warning };

  // base log function, with printf-like format arguments
  // \param severity  Message severity.
  // \param message   Message content, printf-like format.
  // \param ap        Variable list of arguments associated with message.
  int logV(SimpleLog::Impl::Severity severity, const char* message, va_list ap);

 protected:
  FILE* fp; // descriptor to be used. If NULL, using stdout/stderr.
  int formatOptions;
  int fdStdout;
  int fdStderr;
  bool disableOutput = 0; // when set, messages completely dropped (logfile=/dev/null)

  // log rotation settings
  unsigned long rotateMaxBytes = 0;
  unsigned int rotateMaxFiles = 0;
  std::string logFilePath; // need to keep file path for later
  size_t logFileSize = 0;  // keep track of its size for rotation

  void closeLogFile();
  int openLogFile();
  void rotate(); // this renames older files

  friend class SimpleLog;
};

SimpleLog::Impl::Impl()
{
  fp = NULL;
  formatOptions = SimpleLog::FormatOption::ShowTimeStamp | SimpleLog::FormatOption::ShowSeveritySymbol | SimpleLog::FormatOption::ShowMessage;
  fdStdout = fileno(stdout);
  fdStderr = fileno(stderr);
}

SimpleLog::Impl::~Impl()
{
  closeLogFile();
}

int SimpleLog::Impl::logV(SimpleLog::Impl::Severity s, const char* message, va_list ap)
{
  // immediate return if output disabled
  if (disableOutput) {
    return 0;
  }

  char buffer[1024] = "";
  size_t len = sizeof(buffer) - 2;
  size_t ix = 0;

  if (formatOptions & SimpleLog::FormatOption::ShowTimeStamp) {
    // timestamp (microsecond)
    struct timeval tv;
    double fullTimeNow = -1;
    if (gettimeofday(&tv, NULL) == -1) {
      fullTimeNow = time(NULL);
    } else {
      fullTimeNow = (double)tv.tv_sec + (double)tv.tv_usec / 1000000;
    }
    time_t now;
    struct tm tm_str;
    now = (time_t)fullTimeNow;
    localtime_r(&now, &tm_str);
    double fractionOfSecond = fullTimeNow - now;
    ix += strftime(&buffer[ix], len - ix, "%Y-%m-%d %T", &tm_str);
    char str_fractionOfSecond[10];
    snprintf(str_fractionOfSecond, sizeof(str_fractionOfSecond), "%.6lf", fractionOfSecond);
    ix += snprintf(&buffer[ix], len - ix, ".%s", &str_fractionOfSecond[2]);
    if (ix > len) {
      ix = len;
    }
  }

  if (formatOptions & SimpleLog::FormatOption::ShowSeveritySymbol) {
    if (s == Severity::Error) {
      ix += snprintf(&buffer[ix], len - ix, " !!! ");
    } else if (s == Severity::Warning) {
      ix += snprintf(&buffer[ix], len - ix, "  !  ");
    } else {
      ix += snprintf(&buffer[ix], len - ix, "     ");
    }
  }

  if (formatOptions & SimpleLog::FormatOption::ShowSeverityTxt) {
    if (s == Severity::Error) {
      ix += snprintf(&buffer[ix], len - ix, "Error - ");
    } else if (s == Severity::Warning) {
      ix += snprintf(&buffer[ix], len - ix, "Warning - ");
    } else {
      //ix+=snprintf(&buffer[ix], len-ix, "");
    }
  }

  if (formatOptions & SimpleLog::FormatOption::ShowMessage) {
    ix += vsnprintf(&buffer[ix], len - ix, message, ap);
    if (ix > len) {
      ix = len;
    }
  }

  buffer[ix] = '\n';
  ix++;
  buffer[ix] = 0;

  int fd;
  if (fp != NULL) {
    if ((ix + logFileSize > rotateMaxBytes) && (rotateMaxBytes > 0)) {
      closeLogFile();
      if (rotateMaxFiles == 1) {
        // stop after first file
        disableOutput = 1;
        return -1;
      }
      rotate();
      if (openLogFile())
        return -1;
    }
    fd = fileno(fp);
  } else {
    if (s == Severity::Error) {
      fd = fdStderr;
    } else {
      fd = fdStdout;
    }
  }
  int nBytes = write(fd, buffer, ix);
  if ((fp != NULL) && (nBytes > 0)) {
    logFileSize += nBytes;
  }
  if (nBytes != (int)ix) {
    return -1;
  }

  return 0;
}

SimpleLog::SimpleLog(const char* logFilePath)
{
  pImpl = std::make_unique<SimpleLog::Impl>();
  if (pImpl == NULL) {
    throw __LINE__;
  }
  setLogFile(logFilePath);
}

SimpleLog::~SimpleLog()
{
  setLogFile(NULL);
}

int SimpleLog::setLogFile(const char* logFilePath, unsigned long rotateMaxBytes, unsigned int rotateMaxFiles, unsigned int rotateNow)
{
  pImpl->closeLogFile();
  pImpl->logFilePath = "";
  pImpl->rotateMaxFiles = 0;
  pImpl->rotateMaxBytes = 0;
  pImpl->logFileSize = 0;
  pImpl->disableOutput = 0;
  if (logFilePath != NULL) {
    pImpl->logFilePath = logFilePath;
    if (!strcmp(logFilePath, "/dev/null")) {
      pImpl->disableOutput = 1;
      return 0;
    }
    if (rotateMaxFiles < 0) {
      rotateMaxFiles = 1;
    }
    pImpl->rotateMaxBytes = rotateMaxBytes;
    pImpl->rotateMaxFiles = rotateMaxFiles;
    if (rotateNow) {
      pImpl->rotate();
    }
    if (pImpl->openLogFile()) {
      return -1;
    }
  }
  return 0;
}

int SimpleLog::info(const char* message, ...)
{
  int err = 0;

  va_list ap;
  va_start(ap, message);
  err = pImpl->logV(Impl::Severity::Info, message, ap);
  va_end(ap);

  return err;
}

int SimpleLog::error(const char* message, ...)
{
  int err = 0;

  va_list ap;
  va_start(ap, message);
  err = pImpl->logV(Impl::Severity::Error, message, ap);
  va_end(ap);

  return err;
}

int SimpleLog::warning(const char* message, ...)
{
  int err = 0;

  va_list ap;
  va_start(ap, message);
  err = pImpl->logV(Impl::Severity::Warning, message, ap);
  va_end(ap);

  return err;
}

void SimpleLog::setOutputFormat(int opts)
{
  pImpl->formatOptions = opts;
}

void SimpleLog::setFileDescriptors(int fdStdout, int fdStderr)
{
  pImpl->fdStdout = fdStdout;
  pImpl->fdStderr = fdStderr;
}

void SimpleLog::Impl::closeLogFile()
{
  if (fp != NULL) {
    fclose(fp);
    fp = NULL;
  }
  logFileSize = 0;
}

int SimpleLog::Impl::openLogFile()
{
  logFileSize = 0;
  if (logFilePath.length() == 0) {
    return 0;
  }
  const char *mode = "a";
  if (rotateMaxFiles == 1) {
    mode = "w";
  }
  fp = fopen(logFilePath.c_str(), "a");
  if (fp == NULL) {
    return -1;
  }
  // get file size - this is where we are (append mode)
  long fs = ftell(fp);
  if (fs >= 0) {
    logFileSize = (size_t)fs;
  }
  return 0;
}

void SimpleLog::Impl::rotate()
{

  if (logFilePath.length() == 0) {
    return;
  }

  // extract dir + file names
  std::string dirName;
  std::string fileName;
  size_t pos = logFilePath.find_last_of('/');
  if (pos != std::string::npos) {
    fileName = logFilePath.substr(pos + 1);
    dirName = logFilePath.substr(0, pos + 1);
  } else {
    fileName = logFilePath;
    dirName = "./";
  }

  // build list of file indexes to be rotated
  std::vector<unsigned int> rotateIx;

  // scan directory for matching file names = filename.12345
  // and append list of indexes to be rotated
  //
  // would love to use std::filesystem but not readily available with O2/gcc 7.3.0
  // needs include experimental/ + use link flag -lstdc++fs
  DIR* dp;
  struct dirent* ep;
  dp = opendir(dirName.c_str());
  if (dp != NULL) {
    for (;;) {
      ep = readdir(dp);
      if (ep == NULL)
        break;
      if (!strcmp(fileName.c_str(), ep->d_name)) {
        // we use index 0 for base file name, without extensions.
        // other indexes start with 1
        rotateIx.push_back(0);
        continue;
      }
      if (strlen(ep->d_name) <= fileName.length() + 1) {
        continue;
      }
      if (strncmp(fileName.c_str(), ep->d_name, fileName.length())) {
        continue;
      }
      if (ep->d_name[fileName.length()] != '.') {
        continue;
      }
      std::string postfix = &ep->d_name[fileName.length() + 1];
      bool isOk = 1;
      for (const char& c : postfix) {
        if (!isdigit(c)) {
          isOk = 0;
          break;
        }
      }
      if (isOk) {
        rotateIx.push_back(std::stoi(postfix));
      }
    }
    free(dp);
  }

  // sort indexes in order
  std::sort(rotateIx.begin(), rotateIx.end());

  // find a free slot ('hole') in existing index list
  // before this -> rename in decreasing order
  // after this (included) -> rename in increasing order
  unsigned int holePos = 0; // position in rotateIx[] of first index after hole
  unsigned int holeIx = 1;  // value of contiguous id that shall be used for this hole
  for (; holePos < rotateIx.size(); holePos++) {
    if (holeIx != rotateIx[holePos] + 1) {
      // found a hole
      break;
    }
    holeIx++;
  }

  // function to rotate file (rename +1 or delete)
  auto rotateFile = [&](unsigned int oldIndex, unsigned int newIndex) {
    std::string inFile;
    if (oldIndex == 0) {
      // index 0 is reserved for base file name
      inFile = dirName + fileName;
    } else {
      inFile = dirName + fileName + "." + std::to_string(oldIndex);
    }
    if ((newIndex >= rotateMaxFiles) && (rotateMaxFiles != 0)) {
      // this file should be removed
      unlink(inFile.c_str());
    } else {
      // this file should be renamed (if necessary)
      if (oldIndex != newIndex) {
        std::string outFile = dirName + fileName + "." + std::to_string(newIndex);
        rename(inFile.c_str(), outFile.c_str());
      }
    }
  };

  unsigned int newIx; // counter for new file index

  // before hole, rename files (increment up) in backward order (newIx >= oldIx)
  newIx = holeIx - 1;
  for (int i = (int)(holePos)-1; i >= 0; i--) {
    rotateFile(rotateIx[i], newIx);
    newIx--;
  }

  // after hole, rename files (pack down) in increasing order (newIx <= oldIx)
  newIx = holeIx;
  for (unsigned int i = holePos; i < rotateIx.size(); i++) {
    rotateFile(rotateIx[i], newIx);
    newIx++;
  }
}

/// \todo: thread to flush output every 1 second

