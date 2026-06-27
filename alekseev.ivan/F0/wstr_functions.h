#ifndef WSTR_FUNCTIONS_H
#define WSTR_FUNCTIONS_H

#ifdef _WIN32
#include <fcntl.h>
#include <windows.h>
#endif
#include <string>
#include <initializer_list>
#include "vector.h"

namespace alekseev {
  using wstr_cr = const std::wstring &;
  size_t djb2_hash(wstr_cr line);
  size_t poly_hash(wstr_cr line);
  bool equal(wstr_cr s1, wstr_cr s2);

  Vector< std::wstring > split(wstr_cr s, wchar_t delim = L' ', bool need_trim = false);
  std::wstring replace(wstr_cr orig_str, wstr_cr old, wstr_cr replacement);
  std::wistream & wgetline(std::wistream & is, std::wstring & str);

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
      size_t max_opts = 5, wstr_cr what_is_your_choice = L"What is your choice?",
      wstr_cr no_one = L"No one of it", bool need_cycle = true);

  bool endswith(wstr_cr str, wstr_cr substr);
  bool endswith(wstr_cr str, std::initializer_list< std::wstring > substr);
  bool is_punctuation(wchar_t ch);
  bool is_punctuation_str(wstr_cr str);
  bool is_whitespace(wchar_t ch);

  struct ConsoleSetup
  {
    ConsoleSetup();
    ~ConsoleSetup();
    ConsoleSetup(const ConsoleSetup &) = delete;
    ConsoleSetup & operator=(const ConsoleSetup &) = delete;
    ConsoleSetup(ConsoleSetup &&) = delete;
    ConsoleSetup & operator=(ConsoleSetup &&) = delete;

    private:
#ifdef _WIN32
      int _oldStdoutMode, _oldStdinMode, _oldStderrMode;
      UINT _oldOutputCP, _oldInputCP;
#else
      const char * _oldLocale = nullptr;
#endif
  };

  struct IOGuard
  {
    explicit IOGuard(std::basic_ios< wchar_t > & stream);
    ~IOGuard();
    IOGuard(const IOGuard &) = delete;
    IOGuard & operator=(const IOGuard &) = delete;
    IOGuard(IOGuard &&) = delete;
    IOGuard & operator=(IOGuard &&) = delete;

    private:
      std::basic_ios< wchar_t > & stream_;
      std::streamsize precision_;
      std::streamsize width_;
      std::basic_ios< wchar_t >::fmtflags flags_;
      wchar_t fill_;
  };

  size_t damerau_levenshtein(wstr_cr a, wstr_cr b);
  Vector< std::wstring > damerau_find(wstr_cr bad_word, const Vector< std::wstring > & candidates,
      size_t max_number = 0, size_t distance = 2);
  template< class FwdIter >
  Vector< std::wstring > damerau_find(wstr_cr bad_word, FwdIter begin, FwdIter end,
      size_t max_number = 0, size_t distance = 2);

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
      wstr_cr what_is_your_choice, wstr_cr no_one, bool need_cycle)
  {
    if (opts.isEmpty()) {
      throw std::invalid_argument("Empty candidates");
    }
    size_t s = opts.getSize();
    size_t n_opts = (max_opts == 0) ? s : std::min(max_opts, s);
    os << what_is_your_choice << L"\n";
    for (size_t i = 0; i < n_opts; ++i) {
      os << "\t" << i + 1 << ". " << opts[i] << L"\n";
    }
    os << "\t" << n_opts + 1 << ". " << no_one << L"\n";
    os << L"(1-" << n_opts + 1 << L") >";
    std::wstring answer;
    wchar_t * end_ptr = nullptr;
    while (wgetline(is, answer)) {
      size_t ind = wcstoull(answer.c_str(), std::addressof(end_ptr), 10);
      if (*end_ptr != L'\0' && !need_cycle) {
        throw std::invalid_argument("Bad input");
      }
      if (*end_ptr == L'\0') {
        if (0 < ind && ind <= n_opts + 1) {
          return ind - 1;
        }
      }
      if (!need_cycle) {
        throw std::invalid_argument("Bad input");
      }
      os << L"(1-" << n_opts + 1 << L") >";
    }
    throw std::invalid_argument("Bad input");
  }

  template< class FwdIter >
  Vector< std::wstring > damerau_find(wstr_cr bad_word, FwdIter begin, FwdIter end,
      size_t max_number, size_t distance)
  {
    Vector< Vector< std::wstring > > res(distance + 1, {});
    size_t count = 0;
    size_t bad_word_size = bad_word.size();
    for (auto it = begin; it != end && (count < max_number || max_number == 0); ++it) {
      size_t cur_size = it->size();
      size_t dif = bad_word_size > cur_size ? bad_word_size - cur_size : cur_size - bad_word_size;
      if (dif <= distance) {
        size_t cur_dist = damerau_levenshtein(*it, bad_word);
        if (cur_dist <= distance) {
          res[cur_dist].pushBack(*it);
          ++count;
        }
      }
    }
    Vector< std::wstring > final;
    final.resize(count);
    for (size_t i = 0; i < distance + 1; ++i) {
      final += res[i];
    }
    return final;
  }
}
#endif
