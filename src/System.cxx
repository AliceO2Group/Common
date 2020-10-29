/// \file System.cxx
/// \brief Implementation of various useful utilities that don't really belong anywhere in particular
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#include <signal.h>
#include <cstring>
#include <fstream>
#include <memory>
#include <sstream>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include "Common/System.h"
#include "Common/Exception.h"

namespace AliceO2
{
namespace Common
{
namespace System
{

namespace b = boost;
namespace bfs = boost::filesystem;

void setSigIntHandler(void (*function)(int))
{
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = function;
  if (sigfillset(&sa.sa_mask) == -1) {
    int err = errno;
    throw std::runtime_error((b::format("sigfillset returned error (%d)") % err).str());
  }
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    int err = errno;
    throw std::runtime_error((b::format("sigaction returned error (%d) while setting sigint handler") % err).str());
  }
}

bool isSigIntHandlerSet()
{
  struct sigaction sa;
  if (sigaction(SIGINT, NULL, &sa) == -1) {
    int err = errno;
    throw std::runtime_error((b::format("sigaction returned error (%d) while getting sigint handler") % err).str());
  }
  return sa.sa_handler != SIG_DFL && sa.sa_handler != SIG_IGN;
}

void makeParentDirectories(const std::string& path)
{
  /// TODO Implement using boost::filesystem
  auto parent = bfs::path(path).parent_path();
  system(b::str(b::format("mkdir -p %s") % parent).c_str());
}

void touchFile(const std::string& path)
{
  std::ofstream ofs(path.c_str(), std::ios::app);
}

std::string executeCommand(const std::string& command)
{
  std::unique_ptr<FILE, void (*)(FILE*)> input(popen(command.c_str(), "r"), [](FILE* f) { pclose(f); });

  if (!input.get()) {
    BOOST_THROW_EXCEPTION(std::runtime_error("Call to popen failed"));
  }

  std::vector<char> buffer(128);
  std::ostringstream oss;
  while (fgets(buffer.data(), buffer.size(), input.get()) != NULL) {
    oss << buffer.data();
  }

  return oss.str();
}

std::string getFileSystemType(const std::string& path)
{
  std::string type{ "" };
  std::string result = executeCommand(b::str(b::format("df %s") % path));

  // We need the second line of the output (first line is a header)
  std::vector<std::string> splitted;
  boost::split(splitted, result, boost::is_any_of("\n"));
  if (splitted.size() == 3) {
    // Then get the first "column" of the second line, which contains the file system type
    std::string line = splitted.at(1);
    boost::split(splitted, line, boost::is_any_of(" "));
    type = splitted.at(0);
  } else {
    BOOST_THROW_EXCEPTION(std::runtime_error("Unrecognized output from 'df' command"));
  }

  return type;
}

std::pair<bool, std::string> isFileSystemTypeAnyOf(const std::string& path,
                                                   const std::set<std::string>& types)
{
  auto type = getFileSystemType(path);
  return { types.count(type), type };
}

/// Throws if the file system type of the given file/directory is not one of the given valid types
void assertFileSystemType(const std::string& path, const std::set<std::string>& validTypes, std::string name)
{
  bool found;
  std::string type;
  std::tie(found, type) = isFileSystemTypeAnyOf(path, validTypes);

  if (!found) {
    std::ostringstream oss;
    oss << "File-backed shared memory for '" << name << "' file system type invalid (supported: ";
    for (auto i = validTypes.begin(); i != validTypes.end(); i++) {
      oss << *i;
      if (i != validTypes.end()) {
        oss << ",";
      }
    }
    oss << ")";

    BOOST_THROW_EXCEPTION(Exception()
                          << ErrorInfo::Message(oss.str())
                          << ErrorInfo::FileName(path)
                          << ErrorInfo::FilesystemType(type));
  }
}

} // namespace System
} // namespace Common
} // namespace AliceO2
