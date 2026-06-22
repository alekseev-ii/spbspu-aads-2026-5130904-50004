#define BOOST_TEST_MODULE BSTree_tests
#include <boost/test/included/unit_test.hpp>
#include <functional>
#include <stdexcept>
#include "BSTree.h"

BOOST_AUTO_TEST_SUITE(BSTreeTests)

  BOOST_AUTO_TEST_CASE(test_push_and_at)
  {
    alekseev::BSTree< int, std::string, std::less< int > > tree{std::less< int >{}};

    tree.push(5, "five");
    tree.push(3, "three");
    tree.push(7, "seven");

    BOOST_CHECK_EQUAL(tree.at(5), "five");
    BOOST_CHECK_EQUAL(tree.at(3), "three");
    BOOST_CHECK_EQUAL(tree.at(7), "seven");

    tree.push(5, "FIVE");
    BOOST_CHECK_EQUAL(tree.at(5), "FIVE");
  }

  BOOST_AUTO_TEST_CASE(test_at_throws)
  {
    alekseev::BSTree< int, std::string, std::less< int > > tree{std::less< int >{}};
    tree.push(1, "one");

    BOOST_CHECK_THROW(tree.at(2), std::out_of_range);
  }

  BOOST_AUTO_TEST_CASE(test_remove)
  {
    alekseev::BSTree< int, std::string, std::less< int > > tree{std::less< int >{}};
    tree.push(5, "five");
    tree.push(3, "three");
    tree.push(7, "seven");

    tree.remove(3);
    BOOST_CHECK_THROW(tree.at(3), std::out_of_range);
    BOOST_CHECK_EQUAL(tree.at(5), "five");
    BOOST_CHECK_EQUAL(tree.at(7), "seven");

    tree.remove(5);
    BOOST_CHECK_THROW(tree.at(5), std::out_of_range);
    BOOST_CHECK_EQUAL(tree.at(7), "seven");
  }

  BOOST_AUTO_TEST_CASE(test_clear)
  {
    alekseev::BSTree< int, std::string, std::less< int > > tree{std::less< int >{}};
    tree.push(1, "one");
    tree.push(2, "two");
    tree.clear();

    BOOST_CHECK_THROW(tree.at(1), std::out_of_range);
    BOOST_CHECK_THROW(tree.at(2), std::out_of_range);
  }

  BOOST_AUTO_TEST_CASE(test_copy)
  {
    alekseev::BSTree< int, std::string, std::less< int > > tree1{std::less< int >{}};
    tree1.push(1, "one");
    tree1.push(2, "two");

    alekseev::BSTree< int, std::string, std::less< int > > tree2{tree1};

    BOOST_CHECK_EQUAL(tree2.at(1), "one");
    BOOST_CHECK_EQUAL(tree2.at(2), "two");

    tree1.push(3, "three");
    BOOST_CHECK_THROW(tree2.at(3), std::out_of_range);
  }

  BOOST_AUTO_TEST_CASE(test_move)
  {
    alekseev::BSTree< int, std::string, std::less< int > > tree1{std::less< int >{}};
    tree1.push(1, "one");
    tree1.push(2, "two");

    alekseev::BSTree< int, std::string, std::less< int > > tree2{std::move(tree1)};

    BOOST_CHECK_EQUAL(tree2.at(1), "one");
    BOOST_CHECK_EQUAL(tree2.at(2), "two");
  }

  BOOST_AUTO_TEST_CASE(test_height)
  {
    alekseev::BSTree< int, std::string, std::less< int > > tree{std::less< int >{}};

    tree.push(5, "five");
    tree.push(3, "three");
    tree.push(7, "seven");
    tree.push(1, "one");
    tree.push(4, "four");

    BOOST_CHECK_EQUAL(tree.height(), 3);

    tree.push(0, "zero");
    BOOST_CHECK_EQUAL(tree.height(), 4);
  }

  BOOST_AUTO_TEST_CASE(test_iterators)
  {
    alekseev::BSTree< int, std::string, std::less< int > > tree{std::less< int >{}};
    tree.push(3, "three");
    tree.push(1, "one");
    tree.push(4, "four");
    tree.push(2, "two");

    auto it = tree.begin();
    BOOST_CHECK_EQUAL(*it, "one");
    ++it;
    BOOST_CHECK_EQUAL(*it, "two");
    ++it;
    BOOST_CHECK_EQUAL(*it, "three");
    ++it;
    BOOST_CHECK_EQUAL(*it, "four");
  }

  BOOST_AUTO_TEST_CASE(test_rotate_left_right)
  {
    alekseev::BSTree< int, std::string, std::less< int > > tree{std::less< int >{}};
    tree.push(5, "five");
    tree.push(3, "three");
    tree.push(7, "seven");
    tree.push(1, "one");
    tree.push(4, "four");

    auto it = tree.begin();
    ++it;
    ++it;

    BOOST_CHECK_NO_THROW(tree.rotateLeft(it));
    BOOST_CHECK_EQUAL(*it, "four");
    BOOST_CHECK_NO_THROW(tree.rotateRight(it));
    BOOST_CHECK_EQUAL(*it, "four");
  }

BOOST_AUTO_TEST_SUITE_END()
