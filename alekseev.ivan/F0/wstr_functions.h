#ifndef WSTR_FUNCTIONS_H
#define WSTR_FUNCTIONS_H
#include <string>
#include "../common/vector.h"
#include <initializer_list>

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
  wchar_t yes_no(std::wstring answer);
  wchar_t ask_yes_no(wstr_cr question, std::wistream & is, std::wostream & os,
      bool need_cycle = false, bool add_variants = true);
  bool endswith(wstr_cr str, wstr_cr substr);
  bool endswith(wstr_cr str, std::initializer_list< std::wstring > substr);

  size_t damerau_levenshtein(wstr_cr a, wstr_cr b);
}

#endif
