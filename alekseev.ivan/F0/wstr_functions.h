#ifndef WSTR_FUNCTIONS_H
#define WSTR_FUNCTIONS_H
#include <string>
#include "../common/vector.h"

namespace alekseev {
  using wstr_cr = const std::wstring &;
  Vector< std::wstring > split(wstr_cr s, wchar_t delim = L' ', bool need_trim = false);
  std::wstring utf8_to_wstring(const std::string & str);
  std::wstring trim(wstr_cr str);
  wchar_t to_lower(wchar_t ch);
  std::wstring lower_case(wstr_cr str);
  wchar_t to_upper(wchar_t ch);
  std::wstring upper_case(wstr_cr str);
  Vector< bool > mask_from_case(wstr_cr str);
  std::wstring case_from_mask(wstr_cr str, const Vector< bool > & mask);

  size_t damerau_levenshtein(wstr_cr a, wstr_cr b);
}

#endif
