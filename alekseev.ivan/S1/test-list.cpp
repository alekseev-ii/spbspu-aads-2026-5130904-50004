#define BOOST_TEST_MODULE ListTest
#include <boost/test/unit_test.hpp>
#include "../common/List.h"

using namespace alekseev;

BOOST_AUTO_TEST_CASE(EmptyList)
{
  List< int > list;
  BOOST_CHECK(list.empty());
  BOOST_CHECK_EQUAL(list.size(), 0);
}

BOOST_AUTO_TEST_CASE(PushFrontAndFront)
{
  List< int > list;
  list.push_front(1);
  list.push_front(2);
  list.push_front(3);

  BOOST_CHECK_EQUAL(list.size(), 3);
  BOOST_CHECK_EQUAL(list.front(), 3);
}

BOOST_AUTO_TEST_CASE(PopFront)
{
  List< int > list;
  list.push_front(1);
  list.push_front(2);
  list.push_front(3);

  list.pop_front();
  BOOST_CHECK_EQUAL(list.size(), 2);
  BOOST_CHECK_EQUAL(list.front(), 2);

  list.pop_front();
  list.pop_front();
  BOOST_CHECK(list.empty());
}

BOOST_AUTO_TEST_CASE(IteratorTraversal)
{
  List< int > list;
  list.push_front(1);
  list.push_front(2);
  list.push_front(3);

  int expected[] = {3, 2, 1};
  int i = 0;
  for (auto it = list.begin(); it != list.end(); ++it) {
    BOOST_CHECK_EQUAL(*it, expected[i++]);
  }
  BOOST_CHECK_EQUAL(i, 3);
}

BOOST_AUTO_TEST_CASE(InsertAfter)
{
  List< int > list;
  list.push_front(1);
  list.push_front(3);

  auto it = list.begin(); // указывает на 3
  list.insert_after(it, 2); // вставляем 2 после 3

  BOOST_CHECK_EQUAL(list.size(), 3);

  it = list.begin();
  BOOST_CHECK_EQUAL(*it, 3);
  ++it;
  BOOST_CHECK_EQUAL(*it, 2);
  ++it;
  BOOST_CHECK_EQUAL(*it, 1);
}

BOOST_AUTO_TEST_CASE(EraseAfter)
{
  List< int > list;
  list.push_front(1);
  list.push_front(2);
  list.push_front(3);

  list.erase_after(list.begin()); // удаляем 2

  BOOST_CHECK_EQUAL(list.size(), 2);

  auto it = list.begin();
  BOOST_CHECK_EQUAL(*it, 3);
  ++it;
  BOOST_CHECK_EQUAL(*it, 1);
}

BOOST_AUTO_TEST_CASE(CopyConstructor)
{
  List< int > list1;
  list1.push_front(1);
  list1.push_front(2);
  list1.push_front(3);

  List< int > list2(list1);
  BOOST_CHECK_EQUAL(list2.size(), list1.size());

  list2.pop_front();
  BOOST_CHECK_EQUAL(list2.size(), 2);
  BOOST_CHECK_EQUAL(list1.size(), 3);
}

BOOST_AUTO_TEST_CASE(MoveConstructor)
{
  List< int > list1;
  list1.push_front(1);
  list1.push_front(2);

  List< int > list2(std::move(list1));
  BOOST_CHECK_EQUAL(list2.size(), 2);
  BOOST_CHECK(list1.empty());
}

BOOST_AUTO_TEST_CASE(Clear)
{
  List< int > list;
  list.push_front(1);
  list.push_front(2);
  list.push_front(3);

  list.clear();
  BOOST_CHECK(list.empty());
  BOOST_CHECK_EQUAL(list.size(), 0);
}

BOOST_AUTO_TEST_CASE(Swap)
{
  List< int > list1;
  list1.push_front(1);
  list1.push_front(2);

  List< int > list2;
  list2.push_front(3);
  list2.push_front(4);
  list2.push_front(5);

  list1.swap(list2);
  BOOST_CHECK_EQUAL(list1.size(), 3);
  BOOST_CHECK_EQUAL(list1.front(), 5);
  BOOST_CHECK_EQUAL(list2.size(), 2);
  BOOST_CHECK_EQUAL(list2.front(), 2);
}

BOOST_AUTO_TEST_CASE(ConstIterator)
{
  List< int > list;
  list.push_front(1);
  list.push_front(2);

  const List< int > & constList = list;
  auto it = constList.begin();
  BOOST_CHECK_EQUAL(*it, 2);
  ++it;
  BOOST_CHECK_EQUAL(*it, 1);
}

BOOST_AUTO_TEST_CASE(StringType)
{
  List< std::string > list;
  list.push_front("hello");
  list.push_front("world");

  BOOST_CHECK_EQUAL(list.size(), 2);
  BOOST_CHECK_EQUAL(list.front(), "world");
}

BOOST_AUTO_TEST_CASE(EdgeCases)
{
  List< int > list;

  // pop_front on empty list
  list.pop_front();
  BOOST_CHECK(list.empty());

  // Single element
  list.push_front(42);
  BOOST_CHECK_EQUAL(list.size(), 1);
  BOOST_CHECK_EQUAL(list.front(), 42);

  list.pop_front();
  BOOST_CHECK(list.empty());
}
