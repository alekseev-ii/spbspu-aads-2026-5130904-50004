#include "dictionary.h"
#include <iostream>

#include "wstr_functions.h"

int main()
{
  alekseev::ConsoleSetup c;

  alekseev::Dictionary d;
  try {
    alekseev::Dictionary t(L"dictionary.txt");
    d = t;
  } catch (std::exception & e) {
    std::wcout << e.what() << "\n";
    return 1;
  }
  std::wcout << d.size() << L"\n";
  std::wcout << L"Dist: " << alekseev::damerau_levenshtein(L"стол", L"стола") << L"\n";
  alekseev::Vector< alekseev::WordForm > wfs = d.damerau_find_wfs(L"стул", 2);
  std::wcout << wfs.getSize() << L"\n";
  for (size_t i = 0; i < wfs.getSize(); ++i) {
    std::wcout << wfs[i].word_ << L" " << wfs[i].case_ << L"\n";
  }
}
