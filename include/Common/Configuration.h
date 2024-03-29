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

/// \file Configuration.h
/// \brief C++ interface to retrieve configuration parameters.
///
/// \author Sylvain Chapeland, CERN

#ifndef SRC_CONFIGURATION_H_
#define SRC_CONFIGURATION_H_

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

class ConfigFilePrivate
{
 public:
  ConfigFilePrivate();
  ~ConfigFilePrivate();

  friend class ConfigFile;
  friend class ConfigFileBrowser;

 protected:
  boost::property_tree::ptree pt;
};

class ConfigFile
{
 public:
  ConfigFile();

  ~ConfigFile();

  // explicitely disable automatically generated methods
  // disable copy constructor
  ConfigFile(const ConfigFile&) = delete;
  // disable copy assignment operator
  ConfigFile& operator=(const ConfigFile&) = delete;

  /// Load the configuration from given path.
  /// Existing ConfigFile data loaded previously is overwritten.
  /// \param path  Path to configuration data.
  ///              Example: file:/configDir/example.cfg
  ///              Accepted prefix:
  ///                 file:     for a file accessible from the filesystem
  ///              Accepted suffix, to define file format (TODO: separate doc for file formats):
  ///                 .ini, .cfg    see example.cfg
  /// \exception   Throws a <std::string> exception on error.
  void load(const std::string path);

  /// Load the configuration directly from a given boost property_tree.
  /// Existing ConfigFile data loaded previously is overwritten.
  /// \param tree  boost property_tree object to be used as configuration input. It is copied.
  /// \exception   Throws a <std::string> exception on error.
  void load(boost::property_tree::ptree const& tree);

  /// Get the configuration value for given key path (by reference), failure causes exception.
  /// \param key   Key name (possibly hierarchical)
  /// \param value Result value found (possible types: int, float, std::string), by reference (this variable is modified in case of success)
  /// \returns     Nothing
  /// \exception   Throws a <std::string> exception on error.
  template <typename T>
  void getValue(const std::string key, T& value)
  {
    try {
      value = dPtr->pt.get<T>(key);
    } catch (const boost::property_tree::ptree_error& e) {
      throw std::string(e.what());
    }
  }

  /// Get the configuration value for given key path (by reference), failure does not cause exception.
  /// \param key   Key name (possibly hierarchical)
  /// \param value Result value found (possible types: int, float, std::string), by reference (this variable is modified in case of success)
  /// \returns     0 if value found, 1 if not found
  /// \exception   Does not throw exception if value not found
  template <typename T>
  int getOptionalValue(const std::string key, T& value)
  {
    try {
      value = dPtr->pt.get<T>(key);
    } catch (...) {
      return 1;
    }
    return 0;
  }

  /// Get the configuration value for given key path (by reference), failure does not cause exception, and provided default value is assigned instead.
  /// \param key   Key name (possibly hierarchical)
  /// \param value Result value found (possible types: int, float, std::string), by reference (this variable is modified in case of success)
  /// \param defaultValue Default value to be assigned in case of failure
  /// \returns     0 if value found, 1 if not found
  /// \exception   Does not throw exception if value not found
  template <typename T>
  int getOptionalValue(const std::string key, T& value, T defaultValue)
  {
    try {
      value = dPtr->pt.get<T>(key);
    } catch (...) {
      value = defaultValue;
      return 1;
    }
    return 0;
  }

  /// Get the configuration value for given key path (by result)
  /// \param key   Key name (possibly hierarchical)
  /// \returns     Result value found (possible types: int, float, std::string)
  /// \exception   Throws a <std::string> exception on error.
  template <typename T>
  T getValue(const std::string key)
  {
    T res;
    getValue(key, res);
    return res;
  }

  /// Print configuration tree
  void print();

  /// Get configuration tree
  boost::property_tree::ptree& get()
  {
    return dPtr->pt;
  }

 private:
  ConfigFilePrivate* dPtr;

  friend class ConfigFileBrowser;
};

/// Helper class to iterate over 1st level sub-tree of config file with a for range-based loop.
///
/// \example        To print section names starting with "part" found in a .ini file:
/// \example          for (auto sectionName : ConfigFileBrowser (&myConfigFile,"part") { std::cout << "Found section : " << sectionName << std::endl;}
class ConfigFileBrowser
{

 public:
  /// Constructor
  /// \param cfgPtr   Pointer to a ConfigFile object
  /// \param filter   Optional string to be matched with sub-tree name. Non-matching elements will be skipped. Only sub-tree names starting by this string will be returned.
  ConfigFileBrowser(ConfigFile* cfgPtr, std::string filter = "", std::string startingNode = "");
  ~ConfigFileBrowser();

  class Iterator;
  Iterator begin();
  Iterator end();

 private:
  ConfigFile* p;
  std::string filter;
  std::string startingNode;
  boost::property_tree::ptree* ptPtr;
};

/// Internal class used to implement ConfigFileBrowser iterator
class ConfigFileBrowser::Iterator
{

  typedef boost::property_tree::ptree::iterator t_Iterator;

 private:
  ConfigFileBrowser* bPtr;
  t_Iterator it;
  t_Iterator itEnd;
  void findNext();

 public:
  Iterator(ConfigFileBrowser* _bPtr, Iterator::t_Iterator _it, Iterator::t_Iterator _itEnd);
  const std::string& operator*();
  Iterator& operator++();
  bool operator!=(const Iterator& _it) const;
};

#endif /* SRC_CONFIGURATION_H_ */

