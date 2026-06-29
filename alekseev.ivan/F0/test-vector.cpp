#include <boost/test/unit_test.hpp>
#include <string>

#include "dictionary.h"
#include "vector.h"

BOOST_AUTO_TEST_SUITE(VectorConstruction)

  BOOST_AUTO_TEST_CASE(DefaultConstructor)
  {
    alekseev::Vector< int > v;
    BOOST_CHECK(v.empty());
    BOOST_CHECK_EQUAL(v.size(), 0);
    BOOST_CHECK_EQUAL(v.capacity(), 0);
  }

  BOOST_AUTO_TEST_CASE(ValueConstructor)
  {
    alekseev::Vector< int > v(5, 42);
    BOOST_CHECK_EQUAL(v.size(), 5);
    BOOST_CHECK_EQUAL(v.capacity(), 5);
    for (size_t i = 0; i < v.size(); ++i) {
      BOOST_CHECK_EQUAL(v[i], 42);
    }
  }

  BOOST_AUTO_TEST_CASE(InitializerListConstructor)
  {
    alekseev::Vector< int > v = {1, 2, 3, 4, 5};
    BOOST_CHECK_EQUAL(v.size(), 5);
    for (size_t i = 0; i < v.size(); ++i) {
      BOOST_CHECK_EQUAL(v[i], i + 1);
    }
  }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(VectorAccess)

  BOOST_AUTO_TEST_CASE(OperatorBrackets)
  {
    alekseev::Vector< int > v = {1, 2, 3};
    BOOST_CHECK_EQUAL(v[0], 1);
    BOOST_CHECK_EQUAL(v[1], 2);
    BOOST_CHECK_EQUAL(v[2], 3);

    v[1] = 10;
    BOOST_CHECK_EQUAL(v[1], 10);
  }

  BOOST_AUTO_TEST_CASE(AtMethodValid)
  {
    alekseev::Vector< int > v = {1, 2, 3};
    BOOST_CHECK_EQUAL(v.at(0), 1);
    BOOST_CHECK_EQUAL(v.at(1), 2);
    BOOST_CHECK_EQUAL(v.at(2), 3);
  }

  BOOST_AUTO_TEST_CASE(AtMethodInvalid)
  {
    alekseev::Vector< int > v = {1, 2, 3};
    BOOST_CHECK_THROW(v.at(3), std::out_of_range);
    BOOST_CHECK_THROW(v.at(10), std::out_of_range);
  }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(VectorModifiers)

  BOOST_AUTO_TEST_CASE(PushBack)
  {
    alekseev::Vector< int > v;
    v.pushBack(1);
    v.pushBack(2);
    v.pushBack(3);

    BOOST_CHECK_EQUAL(v.size(), 3);
    BOOST_CHECK_EQUAL(v[0], 1);
    BOOST_CHECK_EQUAL(v[1], 2);
    BOOST_CHECK_EQUAL(v[2], 3);
    BOOST_CHECK(v.capacity() >= 3);
  }

  BOOST_AUTO_TEST_CASE(PushFront)
  {
    alekseev::Vector< int > v;
    v.pushFront(1);
    v.pushFront(2);
    v.pushFront(3);

    BOOST_CHECK_EQUAL(v.size(), 3);
    BOOST_CHECK_EQUAL(v[0], 3);
    BOOST_CHECK_EQUAL(v[1], 2);
    BOOST_CHECK_EQUAL(v[2], 1);
  }

  BOOST_AUTO_TEST_CASE(PopBack)
  {
    alekseev::Vector< int > v = {1, 2, 3};
    v.popBack();
    BOOST_CHECK_EQUAL(v.size(), 2);
    BOOST_CHECK_EQUAL(v[0], 1);
    BOOST_CHECK_EQUAL(v[1], 2);
  }

  BOOST_AUTO_TEST_CASE(PopBackEmpty)
  {
    alekseev::Vector< int > v;
    BOOST_CHECK_THROW(v.popBack(), std::logic_error);
  }

  BOOST_AUTO_TEST_CASE(InsertSingle)
  {
    alekseev::Vector< int > v = {1, 2, 4, 5};
    v.insert(2, 3);
    BOOST_CHECK_EQUAL(v.size(), 5);
    BOOST_CHECK_EQUAL(v[0], 1);
    BOOST_CHECK_EQUAL(v[1], 2);
    BOOST_CHECK_EQUAL(v[2], 3);
    BOOST_CHECK_EQUAL(v[3], 4);
    BOOST_CHECK_EQUAL(v[4], 5);
  }

  BOOST_AUTO_TEST_CASE(EraseSingle)
  {
    alekseev::Vector< int > v = {1, 2, 3, 4, 5};
    v.erase(2);
    BOOST_CHECK_EQUAL(v.size(), 4);
    BOOST_CHECK_EQUAL(v[0], 1);
    BOOST_CHECK_EQUAL(v[1], 2);
    BOOST_CHECK_EQUAL(v[2], 4);
    BOOST_CHECK_EQUAL(v[3], 5);
  }

  BOOST_AUTO_TEST_CASE(EraseRange)
  {
    alekseev::Vector< int > v = {1, 2, 3, 4, 5};
    v.erase(1, 3);
    BOOST_CHECK_EQUAL(v.size(), 3);
    BOOST_CHECK_EQUAL(v[0], 1);
    BOOST_CHECK_EQUAL(v[1], 4);
    BOOST_CHECK_EQUAL(v[2], 5);
  }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(VectorOperations)

  BOOST_AUTO_TEST_CASE(Concatenation)
  {
    alekseev::Vector< int > v1 = {1, 2, 3};
    alekseev::Vector< int > v2 = {4, 5, 6};
    alekseev::Vector< int > v3 = v1 + v2;

    BOOST_CHECK_EQUAL(v3.size(), 6);
    for (int i = 0; i < 6; ++i) {
      BOOST_CHECK_EQUAL(v3[i], i + 1);
    }
  }

  BOOST_AUTO_TEST_CASE(PlusEquals)
  {
    alekseev::Vector< int > v1 = {1, 2, 3};
    alekseev::Vector< int > v2 = {4, 5, 6};
    v1 += v2;

    BOOST_CHECK_EQUAL(v1.size(), 6);
    for (int i = 0; i < 6; ++i) {
      BOOST_CHECK_EQUAL(v1[i], i + 1);
    }
  }

  BOOST_AUTO_TEST_CASE(Equality)
  {
    alekseev::Vector< int > v1 = {1, 2, 3};
    alekseev::Vector< int > v2 = {1, 2, 3};
    alekseev::Vector< int > v3 = {1, 2, 4};
    alekseev::Vector< int > v4 = {1, 2};

    BOOST_CHECK(v1 == v2);
    BOOST_CHECK(!(v1 == v3));
    BOOST_CHECK(!(v1 == v4));
  }

  BOOST_AUTO_TEST_CASE(Swap)
  {
    alekseev::Vector< int > v1 = {1, 2, 3};
    alekseev::Vector< int > v2 = {4, 5, 6, 7};

    size_t size1 = v1.size();
    size_t size2 = v2.size();

    v1.swap(v2);

    BOOST_CHECK_EQUAL(v1.size(), size2);
    BOOST_CHECK_EQUAL(v2.size(), size1);
    BOOST_CHECK_EQUAL(v1[0], 4);
    BOOST_CHECK_EQUAL(v2[0], 1);
  }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(VectorCopyMove)

  BOOST_AUTO_TEST_CASE(CopyConstructor)
  {
    alekseev::Vector< int > v1 = {1, 2, 3};
    alekseev::Vector< int > v2(v1);

    BOOST_CHECK_EQUAL(v2.size(), v1.size());
    BOOST_CHECK(v1 == v2);

    v2[0] = 10;
    BOOST_CHECK_NE(v1[0], v2[0]);
  }

  BOOST_AUTO_TEST_CASE(CopyAssignment)
  {
    alekseev::Vector< int > v1 = {1, 2, 3};
    alekseev::Vector< int > v2 = {4, 5};
    v2 = v1;

    BOOST_CHECK_EQUAL(v2.size(), v1.size());
    BOOST_CHECK(v1 == v2);
  }

  BOOST_AUTO_TEST_CASE(MoveConstructor)
  {
    alekseev::Vector< int > v1 = {1, 2, 3};
    size_t old_size = v1.size();

    alekseev::Vector< int > v2 = std::move(v1);

    BOOST_CHECK_EQUAL(v2.size(), old_size);
    BOOST_CHECK(v1.empty());
    BOOST_CHECK_EQUAL(v2[0], 1);
    BOOST_CHECK_EQUAL(v2[1], 2);
    BOOST_CHECK_EQUAL(v2[2], 3);
  }

  BOOST_AUTO_TEST_CASE(MoveAssignment)
  {
    alekseev::Vector< int > v1 = {1, 2, 3};
    alekseev::Vector< int > v2 = {4, 5};
    size_t old_size = v1.size();

    v2 = std::move(v1);

    BOOST_CHECK_EQUAL(v2.size(), old_size);
    BOOST_CHECK(v1.empty());
    BOOST_CHECK_EQUAL(v2[0], 1);
  }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(VectorResize)

  BOOST_AUTO_TEST_CASE(ResizeLarger)
  {
    alekseev::Vector< int > v = {1, 2, 3};
    size_t old_capacity = v.capacity();
    v.resize(10);

    BOOST_CHECK(v.capacity() >= 10);
    BOOST_CHECK_EQUAL(v.size(), 3);
    BOOST_CHECK_EQUAL(v[0], 1);
    BOOST_CHECK_EQUAL(v[1], 2);
    BOOST_CHECK_EQUAL(v[2], 3);
  }

  BOOST_AUTO_TEST_CASE(ResizeSmaller)
  {
    alekseev::Vector< int > v = {1, 2, 3, 4, 5};
    size_t old_capacity = v.capacity();
    v.resize(3);

    BOOST_CHECK_EQUAL(v.capacity(), old_capacity);
    BOOST_CHECK_EQUAL(v.size(), 5);
    BOOST_CHECK_EQUAL(v[0], 1);
    BOOST_CHECK_EQUAL(v[1], 2);
    BOOST_CHECK_EQUAL(v[2], 3);
  }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(VectorBubbleSort)

  BOOST_AUTO_TEST_CASE(SortAscending)
  {
    alekseev::Vector< int > v = {3, 1, 4, 1, 5, 9, 2, 6, 5};
    v.bubbleSort([](int a, int b)
    {
      return a < b;
    });

    for (size_t i = 0; i < v.size() - 1; ++i) {
      BOOST_CHECK(v[i] <= v[i + 1]);
    }
  }

  BOOST_AUTO_TEST_CASE(SortDescending)
  {
    alekseev::Vector< int > v = {3, 1, 4, 1, 5, 9, 2, 6, 5};
    v.bubbleSort([](int a, int b)
    {
      return a > b;
    });

    for (size_t i = 0; i < v.size() - 1; ++i) {
      BOOST_CHECK(v[i] >= v[i + 1]);
    }
  }

  BOOST_AUTO_TEST_CASE(SortAlreadySorted)
  {
    alekseev::Vector< int > v = {1, 2, 3, 4, 5};
    v.bubbleSort([](int a, int b)
    {
      return a < b;
    });

    for (size_t i = 0; i < v.size(); ++i) {
      BOOST_CHECK_EQUAL(v[i], i + 1);
    }
  }

  BOOST_AUTO_TEST_CASE(SortEmpty)
  {
    alekseev::Vector< int > v;
    v.bubbleSort([](int a, int b)
    {
      return a < b;
    });
    BOOST_CHECK(v.empty());
  }

  BOOST_AUTO_TEST_CASE(SortSingleElement)
  {
    alekseev::Vector< int > v = {42};
    v.bubbleSort([](int a, int b)
    {
      return a < b;
    });
    BOOST_CHECK_EQUAL(v[0], 42);
  }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(VectorIterator)

  BOOST_AUTO_TEST_CASE(BeginEnd)
  {
    alekseev::Vector< int > v = {1, 2, 3, 4, 5};
    auto it = v.begin();

    for (int i = 1; i <= 5; ++i, ++it) {
      BOOST_CHECK_EQUAL(*it, i);
    }
    BOOST_CHECK(it == v.end());
  }

  BOOST_AUTO_TEST_CASE(ConstBeginEnd)
  {
    const alekseev::Vector< int > v = {1, 2, 3};
    auto it = v.begin();

    for (int i = 1; i <= 3; ++i, ++it) {
      BOOST_CHECK_EQUAL(*it, i);
    }
  }

  BOOST_AUTO_TEST_CASE(IteratorArithmetic)
  {
    alekseev::Vector< int > v = {1, 2, 3, 4, 5};
    auto it = v.begin();

    BOOST_CHECK_EQUAL(*(it + 2), 3);
    BOOST_CHECK_EQUAL(*(it + 4), 5);

    it += 3;
    BOOST_CHECK_EQUAL(*it, 4);
    BOOST_CHECK_EQUAL(*(it - 2), 2);
  }

  BOOST_AUTO_TEST_CASE(IteratorComparisons)
  {
    alekseev::Vector< int > v = {1, 2, 3, 4, 5};
    auto it1 = v.begin();
    auto it2 = v.begin() + 2;
    auto it3 = v.begin() + 4;

    BOOST_CHECK(it1 < it2);
    BOOST_CHECK(it2 < it3);
    BOOST_CHECK(it3 > it1);
    BOOST_CHECK(it1 != it2);
    BOOST_CHECK(it1 == v.begin());
  }

  BOOST_AUTO_TEST_CASE(IteratorPrePostIncrement)
  {
    alekseev::Vector< int > v = {1, 2, 3};
    auto it = v.begin();

    auto post = it++;
    BOOST_CHECK_EQUAL(*post, 1);
    BOOST_CHECK_EQUAL(*it, 2);

    auto pre = ++it;
    BOOST_CHECK_EQUAL(*pre, 3);
    BOOST_CHECK_EQUAL(*it, 3);
  }

  BOOST_AUTO_TEST_CASE(IteratorOperatorArrow)
  {
    struct TestStruct
    {
      int value;

      TestStruct():
        value(0)
      { }

      explicit TestStruct(int v):
        value(v)
      { }
    };

    alekseev::Vector< TestStruct > v = {TestStruct(1), TestStruct(2), TestStruct(3)};
    auto it = v.begin();

    BOOST_CHECK_EQUAL(it->value, 1);
    ++it;
    BOOST_CHECK_EQUAL(it->value, 2);
  }

  BOOST_AUTO_TEST_CASE(IteratorPrePostIncrement_Const)
  {
    const alekseev::Vector< int > v = {1, 2, 3};
    auto it = v.cbegin();

    auto post = it++;
    BOOST_CHECK_EQUAL(*post, 1);
    BOOST_CHECK_EQUAL(*it, 2);

    auto pre = ++it;
    BOOST_CHECK_EQUAL(*pre, 3);
    BOOST_CHECK_EQUAL(*it, 3);
  }

BOOST_AUTO_TEST_SUITE_END()
