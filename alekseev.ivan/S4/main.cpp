#include "BSTree.h"
#include "../common/vector.h"
#include <iostream>
#include <fstream>

namespace alekseev {
  using big_tree_t = BSTree< std::string, BSTree< int, std::string, std::less< > >, std::less< > >;
  std::ifstream & input_dicts(std::ifstream & is, big_tree_t bigTree);
  Vector< std::string > split(const std::string & s, char delim = ' ');

  void print(big_tree_t bigTree, Vector< std::string > args);
  void complement(big_tree_t bigTree, Vector< std::string > args);
  void intersect(big_tree_t bigTree, Vector< std::string > args);
  void union_(big_tree_t bigTree, Vector< std::string > args);

  struct Exec {
    big_tree_t bigTree;
    BSTree< std::string, void(*)(big_tree_t, Vector< std::string >), std::less< > > cmds;
    Exec();
    void operator()(std::string args);
  };
}

int main();

alekseev::Vector< std::string > alekseev::split(const std::string & s, char delim)
{
  size_t start = 0;
  Vector< std::string > res;
  size_t i = 0;
  for (; i < s.size(); ++i) {
    if (s[i] == delim) {
      if (i > start) {
        res.pushBack(s.substr(start, i - start));
      }
      start = i + 1;
    }
  }
  res.pushBack(s.substr(start, i - start));
  return res;
}

void alekseev::print(big_tree_t bigTree, Vector< std::string > args)
{
  if (args.getSize() != 1) {
    throw std::invalid_argument("Wrong number of arguments");
  }

  BSTree< int, std::string, std::less< > > & tree = bigTree.at(args[0]);
  BSTConstIterator< int, std::string > current = tree.cbegin();
  BSTConstIterator< int, std::string > end = tree.cend();
  std::cout << args[0];
  for (; current != end; ++current) {
    std::cout << ' ' << current.current_->key << ' ' << *current;
  }
  std::cout << "\n";
}

void alekseev::complement(big_tree_t bigTree, Vector< std::string > args)
{
  if (args.getSize() != 3) {
    throw std::invalid_argument("Wrong number of arguments");
  }
  if (bigTree.contains(args[0])) {
    throw std::invalid_argument("newdataset already exists");
  }
  BSTree< int, std::string, std::less< > > & a = bigTree.at(args[1]);
  BSTConstIterator< int, std::string > current = a.cbegin();
  BSTConstIterator< int, std::string > end = a.cend();
  BSTree< int, std::string, std::less< > > & b = bigTree.at(args[2]);
  BSTree< int, std::string, std::less< > > res(std::less< >{});

  for (; current != end; ++current) {
    if (!b.contains(current.current_->key)) {
      res.push(current.current_->key, *current);
    }
  }
  bigTree.push(args[0], res);
}

void alekseev::intersect(big_tree_t bigTree, Vector< std::string > args)
{
  if (args.getSize() != 3) {
    throw std::invalid_argument("Wrong number of arguments");
  }
  if (bigTree.contains(args[0])) {
    throw std::invalid_argument("newdataset already exists");
  }
  BSTree< int, std::string, std::less< > > & a = bigTree.at(args[1]);
  BSTConstIterator< int, std::string > current = a.cbegin();
  BSTConstIterator< int, std::string > end = a.cend();
  BSTree< int, std::string, std::less< > > & b = bigTree.at(args[2]);
  BSTree< int, std::string, std::less< > > res(std::less< >{});

  for (; current != end; ++current) {
    if (b.contains(current.current_->key)) {
      res.push(current.current_->key, *current);
    }
  }
  bigTree.push(args[0], res);
}

void alekseev::union_(big_tree_t bigTree, Vector< std::string > args)
{
  if (args.getSize() != 3) {
    throw std::invalid_argument("Wrong number of arguments");
  }
  if (bigTree.contains(args[0])) {
    throw std::invalid_argument("newdataset already exists");
  }
  BSTree< int, std::string, std::less< > > & a = bigTree.at(args[1]);
  BSTree< int, std::string, std::less< > > & b = bigTree.at(args[2]);
  BSTConstIterator< int, std::string > current = b.cbegin();
  BSTConstIterator< int, std::string > end = b.cend();
  BSTree< int, std::string, std::less< > > res(a);

  for (; current != end; ++current) {
    if (!a.contains(current.current_->key)) {
      res.push(current.current_->key, *current);
    }
  }
  bigTree.push(args[0], res);
}
