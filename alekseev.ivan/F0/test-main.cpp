#define BOOST_TEST_MODULE CuckooHashTest
#include <boost/test/included/unit_test.hpp>
#include "cuckoo_hash.h"
#include <string>

struct SimpleHash {
  size_t operator()(int key) const
  {
    return key;
  }

  size_t operator()(const std::string & key) const
  {
    size_t hash = 0;
    for (char c: key) {
      hash = hash * 31 + c;
    }
    return hash;
  }
};

struct Equal {
  bool operator()(int a, int b) const
  {
    return a == b;
  }

  bool operator()(const std::string & a, const std::string & b) const
  {
    return a == b;
  }
};

BOOST_AUTO_TEST_SUITE(BasicOperations)

  BOOST_AUTO_TEST_CASE(DefaultConstructor)
  {
    alekseev::CuckooHash< int, std::string, SimpleHash, SimpleHash, Equal > table(SimpleHash(),
        SimpleHash(), Equal(), 16);

    BOOST_CHECK_EQUAL(table.size(), 0);
    BOOST_CHECK(table.empty());
    BOOST_CHECK_EQUAL(table.capacity(), 16);
    BOOST_CHECK_CLOSE(table.load_factor(), 0.0, 0.001);
  }

  BOOST_AUTO_TEST_CASE(InsertAndFind)
  {
    alekseev::CuckooHash< int, std::string, SimpleHash, SimpleHash, Equal > table(SimpleHash(),
        SimpleHash(), Equal(), 16);

    int key = 42;
    std::string value = "answer";
    table.insert(key, value);

    BOOST_CHECK_EQUAL(table.size(), 1);
    BOOST_CHECK(!table.empty());

    const std::string & found = table.at(key);
    BOOST_CHECK_EQUAL(found, value);
  }

  BOOST_AUTO_TEST_CASE(InsertDuplicateKey)
  {
    alekseev::CuckooHash< int, std::string, SimpleHash, SimpleHash, Equal > table(SimpleHash(),
        SimpleHash(), Equal(), 16);

    table.insert(1, "one");
    table.insert(1, "ONE");

    BOOST_CHECK_EQUAL(table.size(), 1);
    BOOST_CHECK_EQUAL(table.at(1), "ONE");
  }

  BOOST_AUTO_TEST_CASE(RemoveElement)
  {
    alekseev::CuckooHash< int, std::string, SimpleHash, SimpleHash, Equal > table(SimpleHash(),
        SimpleHash(), Equal(), 16);

    table.insert(1, "one");
    table.insert(2, "two");
    BOOST_CHECK_EQUAL(table.size(), 2);

    table.remove(1);
    BOOST_CHECK_EQUAL(table.size(), 1);

    BOOST_CHECK_THROW(table.at(1), std::out_of_range);
    BOOST_CHECK_NO_THROW(table.at(2));
  }

  BOOST_AUTO_TEST_CASE(RemoveNonexistent)
  {
    alekseev::CuckooHash< int, std::string, SimpleHash, SimpleHash, Equal > table(SimpleHash(),
        SimpleHash(), Equal(), 16);

    table.insert(1, "one");
    size_t old_size = table.size();

    table.remove(999);
    BOOST_CHECK_EQUAL(table.size(), old_size);
  }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Collisions)

  BOOST_AUTO_TEST_CASE(InsertMultipleWithSameHash)
  {
    alekseev::CuckooHash< int, std::string, SimpleHash, SimpleHash, Equal > table(SimpleHash(),
        SimpleHash(), Equal(), 16);

    for (int i = 0; i < 10; ++i) {
      table.insert(i, "value_" + std::to_string(i));
    }

    BOOST_CHECK_EQUAL(table.size(), 10);

    for (int i = 0; i < 10; ++i) {
      BOOST_CHECK_NO_THROW(table.at(i));
      BOOST_CHECK_EQUAL(table.at(i), "value_" + std::to_string(i));
    }
  }

  BOOST_AUTO_TEST_CASE(HighLoadFactor)
  {
    alekseev::CuckooHash< int, std::string, SimpleHash, SimpleHash, Equal > table(SimpleHash(),
        SimpleHash(), Equal(), 32);

    for (int i = 0; i < 25; ++i) {
      table.insert(i, "test");
    }

    BOOST_CHECK(table.load_factor() <= 0.5);
    BOOST_CHECK_EQUAL(table.size(), 25);

    for (int i = 0; i < 25; ++i) {
      BOOST_CHECK_NO_THROW(table.at(i));
    }
  }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Rehash)

  BOOST_AUTO_TEST_CASE(RehashOnFull)
  {
    alekseev::CuckooHash< int, std::string, SimpleHash, SimpleHash, Equal > table(SimpleHash(),
        SimpleHash(), Equal(), 8);

    size_t initial_capacity = table.capacity();

    for (int i = 0; i < 12; ++i) {
      table.insert(i, "value_" + std::to_string(i));
    }

    BOOST_CHECK(table.capacity() > initial_capacity);
    BOOST_CHECK_EQUAL(table.size(), 12);

    for (int i = 0; i < 12; ++i) {
      BOOST_CHECK_EQUAL(table.at(i), "value_" + std::to_string(i));
    }
  }

  BOOST_AUTO_TEST_CASE(ManualRehash)
  {
    alekseev::CuckooHash< int, std::string, SimpleHash, SimpleHash, Equal > table(SimpleHash(),
        SimpleHash(), Equal(), 8);

    for (int i = 0; i < 5; ++i) {
      table.insert(i, "test");
    }

    size_t old_capacity = table.capacity();
    table.rehash();

    BOOST_CHECK(table.capacity() > old_capacity || table.capacity() == old_capacity * 2);
    BOOST_CHECK_EQUAL(table.size(), 5);

    for (int i = 0; i < 5; ++i) {
      BOOST_CHECK_NO_THROW(table.at(i));
    }
  }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(StringKeys)

  BOOST_AUTO_TEST_CASE(StringKeysBasic)
  {
    alekseev::CuckooHash< std::string, int, SimpleHash, SimpleHash, Equal > table(SimpleHash(),
        SimpleHash(), Equal(), 16);

    table.insert("one", 1);
    table.insert("two", 2);
    table.insert("three", 3);

    BOOST_CHECK_EQUAL(table.size(), 3);
    BOOST_CHECK_EQUAL(table.at("one"), 1);
    BOOST_CHECK_EQUAL(table.at("two"), 2);
    BOOST_CHECK_EQUAL(table.at("three"), 3);
  }

  BOOST_AUTO_TEST_CASE(StringKeysCollision)
  {
    alekseev::CuckooHash< std::string, int, SimpleHash, SimpleHash, Equal > table(SimpleHash(),
        SimpleHash(), Equal(), 8);

    std::vector< std::string > keys = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"};

    for (size_t i = 0; i < keys.size(); ++i) {
      table.insert(keys[i], i);
    }

    BOOST_CHECK_EQUAL(table.size(), keys.size());

    for (size_t i = 0; i < keys.size(); ++i) {
      BOOST_CHECK_EQUAL(table.at(keys[i]), static_cast<int>(i));
    }
  }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(AccessMethods)

  BOOST_AUTO_TEST_CASE(KeysMethod)
  {
    alekseev::CuckooHash< int, std::string, SimpleHash, SimpleHash, Equal > table(SimpleHash(),
        SimpleHash(), Equal(), 16);

    for (int i = 0; i < 10; ++i) {
      table.insert(i, "value");
    }

    alekseev::Vector< int > keys = table.keys();
    BOOST_CHECK_EQUAL(keys.getSize(), 10);

    for (int i = 0; i < 10; ++i) {
      bool found = false;
      for (size_t j = 0; j < keys.getSize(); ++j) {
        if (keys[j] == i) {
          found = true;
          break;
        }
      }
      BOOST_CHECK(found);
    }
  }

  BOOST_AUTO_TEST_CASE(Clear)
  {
    alekseev::CuckooHash< int, std::string, SimpleHash, SimpleHash, Equal > table(SimpleHash(),
        SimpleHash(), Equal(), 16);

    for (int i = 0; i < 5; ++i) {
      table.insert(i, "value");
    }

    BOOST_CHECK(!table.empty());
    table.clear();
    BOOST_CHECK(table.empty());
    BOOST_CHECK_EQUAL(table.size(), 0);
    BOOST_CHECK_EQUAL(table.capacity(), 16);

    table.insert(100, "new");
    BOOST_CHECK_EQUAL(table.size(), 1);
    BOOST_CHECK_EQUAL(table.at(100), "new");
  }

  BOOST_AUTO_TEST_CASE(LoadFactor)
  {
    alekseev::CuckooHash< int, std::string, SimpleHash, SimpleHash, Equal > table(SimpleHash(),
        SimpleHash(), Equal(), 32);

    BOOST_CHECK_CLOSE(table.load_factor(), 0.0, 0.001);

    for (int i = 0; i < 16; ++i) {
      table.insert(i, "value");
      double expected = static_cast< double >(i + 1) / 32.0;
      BOOST_CHECK_CLOSE(table.load_factor(), expected, 0.001);
    }
  }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(CopyMove)

  BOOST_AUTO_TEST_CASE(CopyConstructor)
  {
    alekseev::CuckooHash< int, std::string, SimpleHash, SimpleHash, Equal > table1(SimpleHash(),
        SimpleHash(), Equal(), 16);

    for (int i = 0; i < 5; ++i) {
      table1.insert(i, "value_" + std::to_string(i));
    }

    alekseev::CuckooHash< int, std::string, SimpleHash, SimpleHash, Equal > table2 = table1;

    BOOST_CHECK_EQUAL(table2.size(), table1.size());
    BOOST_CHECK_EQUAL(table2.capacity(), table1.capacity());

    for (int i = 0; i < 5; ++i) {
      BOOST_CHECK_EQUAL(table2.at(i), table1.at(i));
    }

    table2.insert(99, "new");
    BOOST_CHECK_EQUAL(table2.size(), 6);
    BOOST_CHECK_EQUAL(table1.size(), 5);
    BOOST_CHECK_THROW(table1.at(99), std::out_of_range);
  }

  BOOST_AUTO_TEST_CASE(AssignmentOperator)
  {
    alekseev::CuckooHash< int, std::string, SimpleHash, SimpleHash, Equal > table1(SimpleHash(),
        SimpleHash(), Equal(), 16);
    alekseev::CuckooHash< int, std::string, SimpleHash, SimpleHash, Equal > table2(SimpleHash(),
        SimpleHash(), Equal(), 32);

    for (int i = 0; i < 5; ++i) {
      table1.insert(i, "from_table1");
    }

    table2 = table1;

    BOOST_CHECK_EQUAL(table2.size(), 5);
    BOOST_CHECK_EQUAL(table2.capacity(), 16);

    for (int i = 0; i < 5; ++i) {
      BOOST_CHECK_EQUAL(table2.at(i), "from_table1");
    }
  }

  BOOST_AUTO_TEST_CASE(MoveConstructor)
  {
    alekseev::CuckooHash< int, std::string, SimpleHash, SimpleHash, Equal > table1(SimpleHash(),
        SimpleHash(), Equal(), 16);

    for (int i = 0; i < 5; ++i) {
      table1.insert(i, "value");
    }

    size_t old_size = table1.size();
    size_t old_capacity = table1.capacity();

    alekseev::CuckooHash< int, std::string, SimpleHash, SimpleHash, Equal > table2 =
        std::move(table1);

    BOOST_CHECK_EQUAL(table2.size(), old_size);
    BOOST_CHECK_EQUAL(table2.capacity(), old_capacity);
    BOOST_CHECK(table1.empty());
  }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Swap)

  BOOST_AUTO_TEST_CASE(SwapMethod)
  {
    alekseev::CuckooHash< int, std::string, SimpleHash, SimpleHash, Equal > table1(SimpleHash(),
        SimpleHash(), Equal(), 16);
    alekseev::CuckooHash< int, std::string, SimpleHash, SimpleHash, Equal > table2(SimpleHash(),
        SimpleHash(), Equal(), 32);

    for (int i = 0; i < 3; ++i) {
      table1.insert(i, "table1");
    }
    for (int i = 0; i < 5; ++i) {
      table2.insert(i, "table2");
    }

    auto size1 = table1.size();
    auto size2 = table2.size();
    auto cap1 = table1.capacity();
    auto cap2 = table2.capacity();

    table1.swap(table2);

    BOOST_CHECK_EQUAL(table1.size(), size2);
    BOOST_CHECK_EQUAL(table2.size(), size1);
    BOOST_CHECK_EQUAL(table1.capacity(), cap2);
    BOOST_CHECK_EQUAL(table2.capacity(), cap1);

    BOOST_CHECK_EQUAL(table1.at(0), "table2");
    BOOST_CHECK_EQUAL(table2.at(0), "table1");
  }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(EdgeCases)

  BOOST_AUTO_TEST_CASE(ManyElements)
  {
    alekseev::CuckooHash< int, int, SimpleHash, SimpleHash, Equal > table(SimpleHash(),
        SimpleHash(), Equal(), 64);

    const int N = 200;
    for (int i = 0; i < N; ++i) {
      table.insert(i, i * 2);
    }

    BOOST_CHECK_EQUAL(table.size(), N);

    for (int i = 0; i < N; ++i) {
      BOOST_CHECK_EQUAL(table.at(i), i * 2);
    }
  }

  BOOST_AUTO_TEST_CASE(InsertAfterRemove)
  {
    alekseev::CuckooHash< int, std::string, SimpleHash, SimpleHash, Equal > table(SimpleHash(),
        SimpleHash(), Equal(), 8);

    for (int round = 0; round < 3; ++round) {
      for (int i = 0; i < 10; ++i) {
        table.insert(i, "round_" + std::to_string(round));
      }
      BOOST_CHECK_EQUAL(table.size(), 10);

      for (int i = 0; i < 10; ++i) {
        BOOST_CHECK_EQUAL(table.at(i), "round_" + std::to_string(round));
      }

      for (int i = 0; i < 10; ++i) {
        table.remove(i);
      }
      BOOST_CHECK(table.empty());
    }
  }

BOOST_AUTO_TEST_SUITE_END()
