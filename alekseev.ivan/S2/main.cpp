#include <iostream>
#include <fstream>
#include "expressions_processing.h"
#include "stack.h"

int main(int argc, char ** argv)
{
  std::ifstream file;
  if (argc == 2) {
    file.open(argv[1]);
    if (!file.is_open()) {
      std::cerr << "Cannot open file\n";
      return 1;
    }
  }
  std::istream & input = argc == 2 ? file : std::cin;

  alekseev::Stack< long long > res;
  std::string line;
  while (std::getline(input, line)) {
    try {
      if (!line.empty()) {
        res.push(alekseev::count_from_string(line));
      }
    } catch (const std::exception & e) {
      std::cerr << e.what() << "\n";
      return 1;
    }
  }

  if (res.empty()) {
    std::cout << "\n";
    return 0;
  }
  std::cout << res.top();
  res.pop();
  while (!res.empty()) {
    std::cout << " " << res.top();
    res.pop();
  }
  std::cout << "\n";
}
