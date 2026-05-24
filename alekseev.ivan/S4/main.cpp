#include "BSTree.h"
#include "../common/vector.h"
#include <iostream>
#include <fstream>

namespace alekseev {
  using big_tree_t = BSTree< std::string, BSTree< int, std::string, std::less< > >, std::less< > >;
  Vector< std::string > split(const std::string & s, char delim = ' ');

  void print(big_tree_t & bigTree, const Vector< std::string > & args);
  void complement(big_tree_t & bigTree, const Vector< std::string > & args);
  void intersect(big_tree_t & bigTree, const Vector< std::string > & args);
  void union_(big_tree_t & bigTree, const Vector< std::string > & args);

  struct Exec {
    big_tree_t bigTree;
    BSTree< std::string, void(*)(big_tree_t &, const Vector< std::string > &), std::less< > > cmds;
    Exec();
    void operator()(const std::string & line);
    std::ifstream & input_dicts(std::ifstream & is);
  };
}

int main(int argc, char * argv[])
{
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " input_file\n";
    return 1;
  }
  std::ifstream input_file(argv[1]);
  if (!input_file.is_open()) {
    std::cerr << "Failed to open file " << argv[1] << "\n";
  }
  alekseev::Exec exec;
  exec.input_dicts(input_file);
  input_file.close();
  std::string line;
  while (std::getline(std::cin, line)) {
    try {
      exec(line);
    } catch (std::invalid_argument & e) {
      std::cout << "<INVALID COMMAND>" << "\n";
    } catch (std::out_of_range & e) {
      std::cout << "<INVALID COMMAND>" << "\n";
    } catch (std::exception & e) {
      std::cerr << e.what() << "\n";
      return 1;
    }
  }
  if (!std::cin.eof()) {
    std::cerr << "Input fail" << "\n";
    return 1;
  }
}

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

void alekseev::print(big_tree_t & bigTree, const Vector< std::string > & args)
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

void alekseev::complement(big_tree_t & bigTree, const Vector< std::string > & args)
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

void alekseev::intersect(big_tree_t & bigTree, const Vector< std::string > & args)
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

void alekseev::union_(big_tree_t & bigTree, const Vector< std::string > & args)
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

alekseev::Exec::Exec():
  bigTree(std::less< >{}),
  cmds(std::less< >{})
{
  cmds.push("print", print);
  cmds.push("complement", complement);
  cmds.push("intersect", intersect);
  cmds.push("union", union_);
}

void alekseev::Exec::operator()(const std::string & line)
{
  Vector< std::string > words = split(line, ' ');
  if (words.isEmpty()) {
    throw std::invalid_argument("wrong input");
  }
  if (!cmds.contains(words[0])) {
    throw std::invalid_argument("wrong command name");
  }
  Vector< std::string > args;
  args.insert(0, words, 1, words.getSize());
  cmds.at(words[0])(bigTree, args);
}

std::ifstream & alekseev::Exec::input_dicts(std::ifstream & is)
{
  if (!is) {
    return is;
  }
  std::string line;
  while (std::getline(is, line)) {
    Vector< std::string > words = split(line, ' ');
    if (words.isEmpty()) {
      continue;
    }
    if (words.getSize() % 2 != 1) {
      throw std::invalid_argument("wrong input");
    }
    std::string name = words[0];
    BSTree< int, std::string, std::less< > > dataset(std::less< >{});
    for (size_t i = 1; i < words.getSize(); i += 2) {
      dataset.push(std::stoi(words[i]), words[i + 1]);
    }
    bigTree.push(name, dataset);
  }
  return is;
}
