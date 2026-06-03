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
  template< class T >
  size_t choose(const Vector< T > &, std::wistream & is, std::wostream & os, size_t max_opts = 5,
      wstr_cr m0 = L"Only one opt:", wstr_cr m1 = L"options are available",
      wstr_cr m2 = L"Choose one of them", bool need_cycle = false);
  bool endswith(wstr_cr str, wstr_cr substr);
  bool endswith(wstr_cr str, std::initializer_list< std::wstring > substr);

  size_t damerau_levenshtein(wstr_cr a, wstr_cr b);
  Vector< std::wstring > damerau_find(wstr_cr bad_word, const Vector< std::wstring > & candidates,
      size_t distance = 2);

  template< class T >
  size_t choose(const Vector< T > & candidates, std::wistream & is, std::wostream & os,
      size_t max_opts, wstr_cr m0, wstr_cr m1, wstr_cr m2, bool need_cycle)
  {
    if (candidates.isEmpty()) {
      throw std::invalid_argument("Empty candidates");
    }
    if (candidates.getSize() == 1) {
      os << m0 << L" " << candidates[0] << L"\n";
      return 0;
    }
    size_t s = candidates.getSize();
    size_t n_opts = (max_opts == 0) ? s : std::min(max_opts, s);
    os << n_opts << L" " << m1 << L"\n";

    for (size_t i = 0; i < n_opts; ++i) {
      os << "\t" << i + 1 << ". " << candidates[i] << L"\n";
    }
    os << m2 << L" (1-" << n_opts << L") ";
    std::wstring answer;
    wchar_t * end_ptr = nullptr;
    while (std::getline(is, answer)) {
      size_t ind = wcstoull(answer.c_str(), std::addressof(end_ptr), 10);
      if (!(*end_ptr == L'\0')) {
        throw std::invalid_argument("Bad input");
      }
      if (0 < ind && ind <= n_opts) {
        return ind - 1;
      }
      if (!need_cycle) {
        throw std::invalid_argument("Bad input");
      }
    }
  }
}
#endif
