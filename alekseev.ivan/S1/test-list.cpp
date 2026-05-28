#define BOOST_TEST_MODULE S1
#include <boost/test/included/unit_test.hpp>
#include "../common/List.h"

BOOST_AUTO_TEST_SUITE(ListTests)

  BOOST_AUTO_TEST_CASE(fake)
  {
    alekseev::List< int > * list = alekseev::fake< int >();
    BOOST_CHECK_EQUAL(list->next, list);
    alekseev::rmfake(list);
  }

  BOOST_AUTO_TEST_CASE(insert)
  {
    alekseev::List< int > * list = alekseev::fake< int >();
    alekseev::insert_after(list, 10);
    BOOST_CHECK_EQUAL(list->next->data, 10);
    BOOST_CHECK_EQUAL(list->next->next, list);
    alekseev::clear(list->next, list);
    alekseev::rmfake(list);
  }

  BOOST_AUTO_TEST_CASE(erase)
  {
    alekseev::List< int > * list = alekseev::fake< int >();
    alekseev::insert_after(list, 10);
    alekseev::insert_after(list, 20);
    BOOST_CHECK_EQUAL(list->next->data, 20);
    alekseev::erase_after(list);
    BOOST_CHECK_EQUAL(list->next->data, 10);
    BOOST_CHECK_EQUAL(list->next->next, list);
    alekseev::clear(list->next, list);
    alekseev::rmfake(list);
  }

BOOST_AUTO_TEST_SUITE_END()
