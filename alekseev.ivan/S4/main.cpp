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
