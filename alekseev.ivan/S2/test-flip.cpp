#include <boost/test/unit_test.hpp>

#include "safety_math.h"

BOOST_AUTO_TEST_CASE (test_flip)
{
  BOOST_CHECK_EQUAL(alekseev::flip(0), 0);
  BOOST_CHECK_EQUAL(alekseev::flip(1), 1);
  BOOST_CHECK_EQUAL(alekseev::flip(123), 321);
  BOOST_CHECK_EQUAL(alekseev::flip(120), 21);
  BOOST_CHECK_EQUAL(alekseev::flip(1000), 1);
  BOOST_CHECK_EQUAL(alekseev::flip(123456789), 987654321);

  BOOST_CHECK_EQUAL(alekseev::flip(-1), -1);
  BOOST_CHECK_EQUAL(alekseev::flip(-123), -321);
  BOOST_CHECK_EQUAL(alekseev::flip(-100), -1);
  BOOST_CHECK_EQUAL(alekseev::flip(-120), -21);
  BOOST_CHECK_EQUAL(alekseev::flip(-123456789), -987654321);
}
