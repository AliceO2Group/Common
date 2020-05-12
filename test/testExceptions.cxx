#include "../include/Common/Exceptions.h"

#define BOOST_TEST_MODULE Exceptions test
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <cmath>
#include <boost/test/unit_test.hpp>
#include <assert.h>
#include <boost/test/output_test_stream.hpp>
#include <iostream>
#include <time.h>

using namespace std;
using namespace AliceO2::Common;
using boost::test_tools::output_test_stream;

void foo()
{
  BOOST_THROW_EXCEPTION(ObjectNotFoundError() << errinfo_object_name("object1"));
}

void bar()
{
  BOOST_THROW_EXCEPTION(ObjectNotFoundError());
}

BOOST_AUTO_TEST_CASE(exceptions_test)
{
  BOOST_CHECK_THROW(foo(), ObjectNotFoundError);

  try {
    foo();
  } catch (ObjectNotFoundError& e) {
    cout << e.what() << endl;
    output_test_stream output;
    output << e.what();
    BOOST_TEST( !output.is_empty( false ) );
    BOOST_TEST( output.is_equal( "Object not found: object1" ) );
  }

  try {
    bar();
  } catch (ObjectNotFoundError& e) {
    cout << e.what() << endl;
    output_test_stream output;
    output << e.what();
    BOOST_TEST(!output.is_empty(false));
    BOOST_TEST(output.is_equal("Object not found: (object_name not specified)"));
  }
}
