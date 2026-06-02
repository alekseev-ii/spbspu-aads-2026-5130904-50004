#ifndef WSTR_FUNCTIONS_H
#define WSTR_FUNCTIONS_H
#include <string>
#include "../common/vector.h"

namespace alekseev {
  using wstr_cr = const std::wstring &;
  Vector< std::wstring > split(wstr_cr s, wchar_t delim = L' ', bool need_trim = false);
  std::wstring utf8_to_wstring(const std::string & str);
  std::wstring trim(wstr_cr str);

  size_t damerau_levenshtein(wstr_cr a, wstr_cr b);
}

#endif
