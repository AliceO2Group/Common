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

#include <Common/Configuration.h>
#include <iostream>
#include <vector>
#include <boost/algorithm/string/predicate.hpp>

using namespace std;

ConfigFilePrivate::ConfigFilePrivate()
{
}

ConfigFilePrivate::~ConfigFilePrivate()
{
}

ConfigFile::ConfigFile()
{
  dPtr = new ConfigFilePrivate();
}

ConfigFile::~ConfigFile()
{
  if (dPtr != NULL) {
    delete dPtr;
  }
}

#define PREFIX_FILE "file:"
#define SUFFIX_FILE_INI \
  {                     \
    ".ini", ".cfg"      \
  }

void ConfigFile::load(const std::string path)
{
  if (path.length() == 0) {
    throw std::string("Invalid argument");
  }

  //
  // open location according to prefix
  //

  // filesystem file
  if (boost::algorithm::starts_with(path.c_str(), PREFIX_FILE)) {
    const char* filename;
    filename = &path[strlen(PREFIX_FILE)];

    // TODO: filter out comments in file with boost filtering_stream

    //
    // parse file according to suffix
    //

    // INI file
    std::vector<const char*> suffix_ini SUFFIX_FILE_INI;
    for (auto suffix : suffix_ini) {
      if (boost::algorithm::ends_with(filename, suffix)) {
        try {
          boost::property_tree::ini_parser::read_ini(filename, dPtr->pt);
        } catch (boost::property_tree::ini_parser::ini_parser_error const& perr) {
          std::stringstream ss;
          if (perr.line()) {
            ss << perr.message() << " in " << perr.filename() << " line " << perr.line();
          } else {
            ss << perr.message() << " " << perr.filename();
          }
          throw ss.str();
        }
        return;
      }
    }
    throw std::string("Invalid type in file name");
  } else {
    throw std::string("Invalid path prefix");
  }
}

void ConfigFile::load(boost::property_tree::ptree const& tree)
{
  // copy input property_tree object
  dPtr->pt = tree;
}

void getValueSpecialization(void)
{
  ConfigFile f;
  const std::string s("");
  int vInt;
  f.getValue(s, vInt);
  vInt = f.getValue<int>(s);
  float vFloat;
  f.getValue("", vFloat);
  vFloat = f.getValue<float>(s);
  std::string vString;
  f.getValue("", vString);
  vString = f.getValue<std::string>(s);
}

// template void ConfigFile::getValue<float>(std::string, float&);

// http://stackoverflow.com/questions/4586768/how-to-iterate-a-boost-property-tree
string indent(int level)
{
  string s;
  for (int i = 0; i < level; i++)
    s += "  ";
  return s;
}

void printTree(boost::property_tree::ptree& pt, int level)
{
  if (pt.empty()) {
    cerr << "\"" << pt.data() << "\"";
  } else {
    if (level)
      cerr << endl;
    cerr << indent(level) << "{" << endl;
    for (boost::property_tree::ptree::iterator pos = pt.begin(); pos != pt.end();) {
      cerr << indent(level + 1) << "\"" << pos->first << "\": ";
      printTree(pos->second, level + 1);
      ++pos;
      if (pos != pt.end()) {
        cerr << ",";
      }
      cerr << endl;
    }
    cerr << indent(level) << " }";
  }
  return;
}

void ConfigFile::print()
{
  printTree(dPtr->pt, 0);
}

ConfigFileBrowser::Iterator::Iterator(ConfigFileBrowser* _bPtr, ConfigFileBrowser::Iterator::t_Iterator _it, ConfigFileBrowser::Iterator::t_Iterator _itEnd)
  : bPtr(_bPtr), it(_it), itEnd(_itEnd)
{
  // skip initial elements if filter defined
  findNext();
}

// iterate until matching sub-tree name found, in case a filter is defined
void ConfigFileBrowser::Iterator::findNext()
{
  int l = bPtr->filter.length();
  if (l) {
    while (it != itEnd) {
      std::string s((*it).first);
      if (!s.compare(0, l, bPtr->filter)) {
        break;
      }
      ++it;
    }
  }
}

const std::string& ConfigFileBrowser::Iterator::operator*()
{
  return (*it).first;
}

ConfigFileBrowser::Iterator& ConfigFileBrowser::Iterator::operator++()
{
  ++it;
  findNext();
  return *this;
}

bool ConfigFileBrowser::Iterator::operator!=(const ConfigFileBrowser::Iterator& _it) const
{
  return it != _it.it;
}

/*
 To use separator character other than default '.', each of the get versions has another form, which takes an additional parameter in front of path. This parameter of type char/wchar_t specifies the separating character. This is a lifesaving device for those who may have dots in their keys:

pt.get<float>('/', "p.a.t.h/t.o/v.a.l.u.e");
pt.get('/', "p.a.t.h/t.o/v.a.l.u.e", 0, NULL);
pt.get_optional<std::string>('/', "p.a.t.h/t.o/v.a.l.u.e");
*/

ConfigFileBrowser::ConfigFileBrowser(ConfigFile* _p, std::string _filter, std::string _startingNode)
{
  p = _p;
  filter = _filter;
  startingNode = _startingNode;
  if (startingNode.length() > 0) {
    ptPtr = &p->dPtr->pt.get_child(startingNode);
  } else {
    ptPtr = &p->dPtr->pt;
  }
}

ConfigFileBrowser::~ConfigFileBrowser()
{
}

ConfigFileBrowser::Iterator ConfigFileBrowser::begin()
{
  return { this, ptPtr->begin(), ptPtr->end() };
}

ConfigFileBrowser::Iterator ConfigFileBrowser::end()
{
  return Iterator(this, ptPtr->end(), ptPtr->end());
}
