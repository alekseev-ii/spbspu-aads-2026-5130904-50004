#define BOOST_TEST_MODULE BSTree_tests
#include <boost/test/included/unit_test.hpp>
#include "BSTree.h"
#include <functional>
#include <stdexcept>

struct TreeFixture {
  alekseev::BSTree< int, std::string, std::less< int > > tree{std::less< int >{}};

  TreeFixture()
  {
    tree.push(5, "five");
    tree.push(3, "three");
    tree.push(7, "seven");
    tree.push(1, "one");
    tree.push(4, "four");
    tree.push(6, "six");
    tree.push(8, "eight");
  }
};

BOOST_AUTO_TEST_SUITE(BSTreeTests)

  BOOST_FIXTURE_TEST_CASE(test_push_and_at, TreeFixture)
  {
    BOOST_CHECK_EQUAL(tree.at(5), "five");
    BOOST_CHECK_EQUAL(tree.at(3), "three");
    BOOST_CHECK_EQUAL(tree.at(7), "seven");
    BOOST_CHECK_EQUAL(tree.at(1), "one");
    BOOST_CHECK_EQUAL(tree.at(4), "four");
    BOOST_CHECK_EQUAL(tree.at(6), "six");
    BOOST_CHECK_EQUAL(tree.at(8), "eight");

    tree.push(9, "nine");
    BOOST_CHECK_EQUAL(tree.at(9), "nine");

    tree.push(5, "FIVE");
    BOOST_CHECK_EQUAL(tree.at(5), "FIVE");
  }

  BOOST_FIXTURE_TEST_CASE(test_at_throws, TreeFixture)
  {
    BOOST_CHECK_THROW(tree.at(100), std::out_of_range);
  }

  BOOST_FIXTURE_TEST_CASE(test_remove, TreeFixture)
  {
    tree.remove(3);
    BOOST_CHECK_THROW(tree.at(3), std::out_of_range);
    BOOST_CHECK_EQUAL(tree.at(5), "five");
    BOOST_CHECK_EQUAL(tree.at(7), "seven");

    tree.remove(5);
    BOOST_CHECK_THROW(tree.at(5), std::out_of_range);

    tree.remove(1);
    tree.remove(4);
    tree.remove(6);
    tree.remove(7);
    tree.remove(8);
    BOOST_CHECK_THROW(tree.at(1), std::out_of_range);
  }

  BOOST_FIXTURE_TEST_CASE(test_clear, TreeFixture)
  {
    tree.clear();
    BOOST_CHECK_THROW(tree.at(5), std::out_of_range);
  }

  BOOST_AUTO_TEST_CASE(test_empty_tree)
  {
    alekseev::BSTree< int, std::string, std::less< int > > empty{std::less< int >{}};
    BOOST_CHECK_THROW(empty.at(1), std::out_of_range);
    empty.push(10, "ten");
    BOOST_CHECK_EQUAL(empty.at(10), "ten");
  }

  BOOST_FIXTURE_TEST_CASE(test_copy_constructor, TreeFixture)
  {
    alekseev::BSTree< int, std::string, std::less< int > > copy{tree};
    BOOST_CHECK_EQUAL(copy.at(5), "five");
    BOOST_CHECK_EQUAL(copy.at(3), "three");
    BOOST_CHECK_EQUAL(copy.at(7), "seven");

    copy.push(9, "nine");
    BOOST_CHECK_EQUAL(copy.at(9), "nine");
    BOOST_CHECK_THROW(tree.at(9), std::out_of_range);
  }

  BOOST_FIXTURE_TEST_CASE(test_copy_assignment, TreeFixture)
  {
    alekseev::BSTree< int, std::string, std::less< int > > copy{std::less< int >{}};
    copy = tree;
    BOOST_CHECK_EQUAL(copy.at(5), "five");
    BOOST_CHECK_EQUAL(copy.at(3), "three");
  }

  BOOST_FIXTURE_TEST_CASE(test_move_constructor, TreeFixture)
  {
    alekseev::BSTree< int, std::string, std::less< int > > moved{std::move(tree)};
    BOOST_CHECK_EQUAL(moved.at(5), "five");
    BOOST_CHECK_EQUAL(moved.at(3), "three");
  }

  BOOST_FIXTURE_TEST_CASE(test_move_assignment, TreeFixture)
  {
    alekseev::BSTree< int, std::string, std::less< int > > moved{std::less< int >{}};
    moved = std::move(tree);
    BOOST_CHECK_EQUAL(moved.at(5), "five");
    BOOST_CHECK_EQUAL(moved.at(3), "three");
  }

  BOOST_FIXTURE_TEST_CASE(test_swap, TreeFixture)
  {
    alekseev::BSTree< int, std::string, std::less< int > > other{std::less< int >{}};
    other.push(100, "hundred");

    tree.swap(other);

    BOOST_CHECK_EQUAL(tree.at(100), "hundred");
    BOOST_CHECK_EQUAL(other.at(5), "five");
  }

  BOOST_FIXTURE_TEST_CASE(test_height, TreeFixture)
  {
    BOOST_CHECK_EQUAL(tree.height(), 3);

    tree.push(0, "zero");
    BOOST_CHECK_EQUAL(tree.height(), 3);

    tree.push(9, "nine");
    BOOST_CHECK_EQUAL(tree.height(), 3);

    tree.push(10, "ten");
    BOOST_CHECK_EQUAL(tree.height(), 4);
  }

  BOOST_FIXTURE_TEST_CASE(test_height_from_iterator, TreeFixture)
  {
    auto it = tree.begin();
    BOOST_CHECK_EQUAL(tree.height(it), 1);
  }

  BOOST_FIXTURE_TEST_CASE(test_rotate_left, TreeFixture)
  {
    auto it = tree.begin();
    ++it;
    ++it;
    auto new_it = tree.rotateLeft(it);
    BOOST_CHECK_EQUAL((*new_it), "five");
  }

  BOOST_FIXTURE_TEST_CASE(test_rotate_right, TreeFixture)
  {
    auto it = tree.begin();
    ++it;
    auto new_it = tree.rotateRight(it);
    BOOST_CHECK_EQUAL((*new_it), "three");
  }

  BOOST_FIXTURE_TEST_CASE(test_rotate_large_left, TreeFixture)
  {
    auto it = tree.begin();
    ++it;
    ++it;
    auto new_it = tree.rotateLargeLeft(it);
    BOOST_CHECK_NO_THROW(*new_it);
  }

  BOOST_FIXTURE_TEST_CASE(test_rotate_large_right, TreeFixture)
  {
    auto it = tree.begin();
    ++it;
    auto new_it = tree.rotateLargeRight(it);
    BOOST_CHECK_NO_THROW(*new_it);
  }

  BOOST_FIXTURE_TEST_CASE(test_iterator_increment, TreeFixture)
  {
    auto it = tree.begin();
    BOOST_CHECK_EQUAL(*it, "one");
    ++it;
    BOOST_CHECK_EQUAL(*it, "three");
    ++it;
    BOOST_CHECK_EQUAL(*it, "four");
    ++it;
    BOOST_CHECK_EQUAL(*it, "five");
    ++it;
    BOOST_CHECK_EQUAL(*it, "six");
    ++it;
    BOOST_CHECK_EQUAL(*it, "seven");
    ++it;
    BOOST_CHECK_EQUAL(*it, "eight");
  }

  BOOST_FIXTURE_TEST_CASE(test_iterator_decrement, TreeFixture)
  {
    auto it = tree.end();
    --it;
    BOOST_CHECK_EQUAL(*it, "eight");
    --it;
    BOOST_CHECK_EQUAL(*it, "seven");
    --it;
    BOOST_CHECK_EQUAL(*it, "six");
  }

  BOOST_FIXTURE_TEST_CASE(test_const_iterator, TreeFixture)
  {
    auto it = tree.cbegin();
    BOOST_CHECK_EQUAL(*it, "one");
    ++it;
    BOOST_CHECK_EQUAL(*it, "three");
  }

  BOOST_FIXTURE_TEST_CASE(test_iterator_equality, TreeFixture)
  {
    auto it1 = tree.begin();
    auto it2 = tree.begin();
    BOOST_CHECK(it1 == it2);
    ++it1;
    BOOST_CHECK(it1 != it2);
  }

  BOOST_FIXTURE_TEST_CASE(test_string_keys, TreeFixture)
  {
    alekseev::BSTree< std::string, int, std::less< std::string > > strTree{
      std::less< std::string >{}
    };
    strTree.push("apple", 1);
    strTree.push("banana", 2);
    strTree.push("cherry", 3);

    BOOST_CHECK_EQUAL(strTree.at("apple"), 1);
    BOOST_CHECK_EQUAL(strTree.at("banana"), 2);
    BOOST_CHECK_EQUAL(strTree.at("cherry"), 3);
  }

BOOST_AUTO_TEST_SUITE_END()
