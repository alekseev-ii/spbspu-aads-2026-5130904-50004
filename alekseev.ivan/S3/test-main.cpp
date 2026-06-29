#define BOOST_TEST_MODULE S3
#include <boost/test/included/unit_test.hpp>
#include <stdexcept>
#include "hash_table.h"

using namespace alekseev;

using str = std::string;

size_t test_str_hasher(const str & name)
{
  size_t res = 0;
  for (size_t i = 0; i < name.size(); ++i) {
    res += name[i];
  }
  return res;
}

bool test_str_equal(const str & a, const str & b)
{
  return a == b;
}

bool test_pair_equal(const std::pair< str, str > & lhs,
    const std::pair< str, str > & rhs)
{
  return lhs.first == rhs.first && lhs.second == rhs.second;
}

size_t test_pair_hasher(const std::pair< str, str > & key)
{
  return test_str_hasher(key.first) ^ test_str_hasher(key.second);
}

BOOST_AUTO_TEST_SUITE(HashTableTests)

  BOOST_AUTO_TEST_CASE(insert_and_contains)
  {
    HashTable< str, int, size_t(*)(const str &), bool(*)(const str &,
            const str &) >
        ht(test_str_hasher, test_str_equal, 4);

    ht.insert("one", 1);
    ht.insert("two", 2);

    BOOST_CHECK(ht.contains("one"));
    BOOST_CHECK(ht.contains("two"));
    BOOST_CHECK(!ht.contains("three"));
  }

  BOOST_AUTO_TEST_CASE(at_throws_if_not_found)
  {
    HashTable< str, int, size_t(*)(const str &), bool(*)(const str &,
            const str &) >
        ht(test_str_hasher, test_str_equal, 4);

    ht.insert("key", 42);

    BOOST_CHECK_EQUAL(ht.at("key"), 42);
    BOOST_CHECK_THROW(ht.at("missing"), std::out_of_range);
  }

  BOOST_AUTO_TEST_CASE(remove_existing_key)
  {
    HashTable< str, int, size_t(*)(const str &), bool(*)(const str &,
            const str &) >
        ht(test_str_hasher, test_str_equal, 4);

    ht.insert("a", 1);
    ht.insert("b", 2);

    ht.remove("a");

    BOOST_CHECK(!ht.contains("a"));
    BOOST_CHECK(ht.contains("b"));
    BOOST_CHECK_EQUAL(ht.at("b"), 2);
  }

  BOOST_AUTO_TEST_CASE(clear_removes_all)
  {
    HashTable< str, int, size_t(*)(const str &), bool(*)(const str &,
            const str &) >
        ht(test_str_hasher, test_str_equal, 4);

    ht.insert("x", 10);
    ht.insert("y", 20);
    ht.clear();

    BOOST_CHECK(!ht.contains("x"));
    BOOST_CHECK(!ht.contains("y"));
    BOOST_CHECK_EQUAL(ht.size(), 0);
  }

  BOOST_AUTO_TEST_CASE(rehash_works)
  {
    HashTable< str, int, size_t(*)(const str &), bool(*)(const str &,
            const str &) >
        ht(test_str_hasher, test_str_equal, 2);

    ht.insert("apple", 100);
    ht.insert("banana", 200);
    ht.insert("cherry", 300);
    ht.rehash(10);

    BOOST_CHECK(ht.contains("apple"));
    BOOST_CHECK(ht.contains("banana"));
    BOOST_CHECK(ht.contains("cherry"));
    BOOST_CHECK_GE(ht.capacity(), 2);
  }

  BOOST_AUTO_TEST_CASE(keys_returns_all_keys)
  {
    HashTable< str, int, size_t(*)(const str &), bool(*)(const str &,
            const str &) >
        ht(test_str_hasher, test_str_equal, 4);

    ht.insert("first", 1);
    ht.insert("second", 2);

    Vector< str > keys = ht.keys();
    BOOST_CHECK_EQUAL(keys.size(), 2);
  }

BOOST_AUTO_TEST_SUITE_END()
