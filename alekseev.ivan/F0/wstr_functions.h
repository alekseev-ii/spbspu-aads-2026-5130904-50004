#ifndef WSTR_FUNCTIONS_H
#define WSTR_FUNCTIONS_H
#include <string>
#include "../common/vector.h"
#include <initializer_list>

namespace alekseev {
  using wstr_cr = const std::wstring &;
  size_t djb2_hash(wstr_cr line);
  size_t poly_hash(wstr_cr line);
  bool equal(wstr_cr s1, wstr_cr s2);

  Vector< std::wstring > split(wstr_cr s, wchar_t delim = L' ', bool need_trim = false);
  std::wstring utf8_to_wstring(const std::string & str);
  std::string wstring_to_utf8(wstr_cr wstr);

  std::wstring trim(wstr_cr str);
  std::wstring ltrim(wstr_cr str);
  std::wstring rtrim(wstr_cr str);
  template< class Cond >
  std::wstring trim(wstr_cr str, Cond condition);
  template< class Cond >
  std::wstring ltrim(wstr_cr str, Cond condition);
  template< class Cond >
  std::wstring rtrim(wstr_cr str, Cond condition);

  bool is_lower(wchar_t ch);
  wchar_t to_lower(wchar_t ch);
  std::wstring lower_case(wstr_cr str);
  bool is_upper(wchar_t ch);
  wchar_t to_upper(wchar_t ch);
  std::wstring upper_case(wstr_cr str);
  Vector< bool > mask_from_case(wstr_cr str);
  std::wstring case_from_mask(wstr_cr str, const Vector< bool > & mask);

  wchar_t yes_no(std::wstring answer);
  wchar_t ask_yes_no(wstr_cr question, std::wistream & is, std::wostream & os,
      bool need_cycle = true, bool add_variants = true);
  template< class T >
  size_t choose(const Vector< T > & opts, std::wistream & is, std::wostream & os,
      size_t max_opts = 5, wstr_cr only_one_opt = L"Only one opt:",
      wstr_cr what_is_your_choice = L"What is your choice?", wstr_cr no_one = L"No one of it",
      bool need_cycle = true);

  bool endswith(wstr_cr str, wstr_cr substr);
  bool endswith(wstr_cr str, std::initializer_list< std::wstring > substr);
  bool is_punctuation(wchar_t ch);
  bool is_punctuation_str(wstr_cr str);
  bool is_whitespace(wchar_t ch);

  size_t damerau_levenshtein(wstr_cr a, wstr_cr b);
  Vector< std::wstring > damerau_find(wstr_cr bad_word, const Vector< std::wstring > & candidates,
      size_t distance = 2);

  template< class Cond >
  std::wstring trim(wstr_cr str, Cond condition)
  {
    return ltrim(rtrim(str, condition), condition);
  }

  template< class Cond >
  std::wstring ltrim(wstr_cr str, Cond condition)
  {
    size_t start = 0, end = str.size();
    while (start < end) {
      wchar_t c = str[start];
      if (condition(c)) {
        ++start;
      } else {
        break;
      }
    }
    return str.substr(start, end - start);
  }

  template< class Cond >
  std::wstring rtrim(wstr_cr str, Cond condition)
  {
    size_t end = str.size();
    while (end > 0) {
      wchar_t c = str[end - 1];
      if (condition(c)) {
        --end;
      } else {
        break;
      }
    }
    return str.substr(0, end);
  }

  template< class T >
  size_t choose(const Vector< T > & opts, std::wistream & is, std::wostream & os, size_t max_opts,
      wstr_cr only_one_opt, wstr_cr what_is_your_choice,wstr_cr no_one, bool need_cycle)
  {
    if (opts.isEmpty()) {
      throw std::invalid_argument("Empty candidates");
    }
    if (opts.getSize() == 1) {
      os << only_one_opt << L" " << opts[0] << L"\n";
      return 0;
    }
    size_t s = opts.getSize();
    size_t n_opts = (max_opts == 0) ? s : std::min(max_opts, s);
    os << what_is_your_choice << L"\n";
    for (size_t i = 0; i < n_opts; ++i) {
      os << "\t" << i + 1 << ". " << opts[i] << L"\n";
    }
    os << "\t" << n_opts + 1 << ". " << no_one << L"\n";
    os << L"(1-" << n_opts + 2 << L"): ";
    std::wstring answer;
    wchar_t * end_ptr = nullptr;
    while (std::getline(is, answer)) {
      size_t ind = wcstoull(answer.c_str(), std::addressof(end_ptr), 10);
      if (!(*end_ptr == L'\0')) {
        throw std::invalid_argument("Bad input");
      }
      if (0 < ind && ind <= n_opts + 1) {
        return ind - 1;
      }
      if (!need_cycle) {
        throw std::invalid_argument("Bad input");
      }
    }
    throw std::invalid_argument("Bad input");
  }
}
#endif
