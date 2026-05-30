#include "dictionary.h"
#include <fstream>

int main()
{
  std::cout << alekseev::damerau_levenshtein(L"лабрадор", L"гибралтар") << "\n";
}
