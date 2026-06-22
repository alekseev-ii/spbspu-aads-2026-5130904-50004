#include <boost/test/unit_test.hpp>

#include "../common/stack.h"

BOOST_AUTO_TEST_CASE (satck_check_size_and_empty)
{
  alekseev::Stack< int > s;
  BOOST_CHECK(s.empty());
  BOOST_CHECK_EQUAL(s.size(), 0);
  s.push(0);
  BOOST_CHECK(!s.empty());
  BOOST_CHECK_EQUAL(s.size(), 1);

  for (int i = 1; i < 10; ++i) {
    s.push(i);
    BOOST_CHECK_EQUAL(s.size(), i + 1);
  }

  for (int i = 9; i >= 0; --i) {
    s.pop();
    BOOST_CHECK_EQUAL(s.size(), i);
  }
}

BOOST_AUTO_TEST_CASE (satck_check_push_pop_top)
{
  alekseev::Stack< int > s;
  s.push(10);
  s.push(20);
  s.push(30);
  BOOST_CHECK_EQUAL(s.top(), 30);
  s.pop();
  BOOST_CHECK_EQUAL(s.top(), 20);
  s.pop();
  BOOST_CHECK_EQUAL(s.top(), 10);
}
