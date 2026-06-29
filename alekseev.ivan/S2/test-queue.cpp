#include <boost/test/unit_test.hpp>

#include "../common/queue.h"

BOOST_AUTO_TEST_CASE (queue_check_size_and_empty)
{
  alekseev::Queue< int > q;
  BOOST_CHECK(q.empty());
  BOOST_CHECK_EQUAL(q.size(), 0);
  q.push(0);
  BOOST_CHECK(!q.empty());
  BOOST_CHECK_EQUAL(q.size(), 1);

  for (int i = 1; i < 10; ++i) {
    q.push(i);
    BOOST_CHECK_EQUAL(q.size(), i + 1);
  }

  for (int i = 9; i >= 0; --i) {
    q.pop();
    BOOST_CHECK_EQUAL(q.size(), i);
  }
}

BOOST_AUTO_TEST_CASE (queue_check_push_pop_front_back)
{
  alekseev::Queue< int > q;
  q.push(10);
  q.push(20);
  q.push(30);
  BOOST_CHECK_EQUAL(q.front(), 10);
  BOOST_CHECK_EQUAL(q.back(), 30);
  q.pop();
  BOOST_CHECK_EQUAL(q.front(), 20);
  BOOST_CHECK_EQUAL(q.back(), 30);
  q.pop();
  BOOST_CHECK_EQUAL(q.front(), 30);
  BOOST_CHECK_EQUAL(q.back(), 30);
}
