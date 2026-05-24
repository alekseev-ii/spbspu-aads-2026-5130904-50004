#include "BSTree.h"
#include <iostream>
#include <fstream>

namespace alekseev {
  using big_tree_t = BSTree< std::string, BSTree< int, std::string, std::less< > >, std::less< > >;
  std::ifstream & input_dicts(std::ifstream & is, big_tree_t bigTree);

  void print(big_tree_t bigTree, std::string args);
  void complement(big_tree_t bigTree, std::string args);
  void intersect(big_tree_t bigTree, std::string args);
  void union_(big_tree_t bigTree, std::string args);

  struct Exec {
    big_tree_t bigTree;
    BSTree< std::string, void(*)(big_tree_t, std::string), std::less< > > cmds;
    Exec();
    void operator()(std::string args);
  };
}

int main();
