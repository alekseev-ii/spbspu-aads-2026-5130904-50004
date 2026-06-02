#include "wstr_functions.h"

#include <windows.h>
#include <fcntl.h>

alekseev::Vector< std::wstring > alekseev::split(const std::wstring & s, wchar_t delim,
    bool need_trim)
{
  size_t start = 0;
  Vector< std::wstring > res;
  size_t i = 0;
  for (; i < s.size(); ++i) {
    if (s[i] == delim) {
      if (i > start) {
        std::wstring word = s.substr(start, i - start);
        if (need_trim) {
          word = trim(word);
        }
        res.pushBack(word);
      }
      start = i + 1;
    }
  }
  std::wstring last = s.substr(start, i - start);
  if (!last.empty()) {
    if (need_trim) {
      last = trim(last);
    }
    res.pushBack(last);
  }
  return res;
}

std::wstring alekseev::utf8_to_wstring(const std::string & str)
{
  if (str.empty()) {
    return {};
  }

  int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast< int >(str.size()),
      nullptr, 0);
  std::wstring wstr(size_needed, 0);
  MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast< int >(str.size()), &wstr[0],
      size_needed);
  return wstr;
}

std::wstring alekseev::trim(const std::wstring & str)
{
  size_t start = 0, end = str.size();
  while (start < end) {
    wchar_t c = str[start];
    if (c == L' ' || c == L'\t' || c == L'\r' || c == L'\n') {
      ++start;
    } else {
      break;
    }
  }
  while (end > start) {
    wchar_t c = str[end - 1];
    if (c == L' ' || c == L'\t' || c == L'\r' || c == L'\n') {
      --end;
    } else {
      break;
    }
  }
  return str.substr(start, end - start);
}

wchar_t alekseev::to_lower(wchar_t ch)
{
  if (ch >= L'A' && ch <= L'Z') {
    return ch + (L'a' - L'A');
  }
  if (ch >= L'А' && ch <= L'Я') {
    return ch + (L'А' - L'а');
  }
  if (ch == L'Ё') {
    return L'ё';
  }
  return ch;
}

std::wstring alekseev::lower_case(wstr_cr str)
{
  std::wstring res;
  for (size_t i = 0; i < str.size(); ++i) {
    res += to_lower(str[i]);
  }
  return res;
}

wchar_t alekseev::to_upper(wchar_t ch)
{
  if (ch >= L'a' && ch <= L'z') {
    return ch - (L'a' - L'A');
  }
  if (ch >= L'а' && ch <= L'я') {
    return ch - (L'а' - L'А');
  }
  if (ch == L'ё') {
    return L'Ё';
  }
  return ch;
}

std::wstring alekseev::upper_case(wstr_cr str)
{
  std::wstring res;
  for (size_t i = 0; i < str.size(); ++i) {
    res += to_upper(str[i]);
  }
  return res;
}

alekseev::Vector< bool > alekseev::mask_from_case(wstr_cr str)
{
  Vector< bool > res(str.size(), false);
  for (size_t i = 0; i < str.size(); ++i) {
    wchar_t ch = str[i];
    res[i] = (ch == to_upper(ch));
  }
  return res;
}

std::wstring alekseev::case_from_mask(wstr_cr str, const Vector< bool > & mask)
{
  std::wstring res;
  for (size_t i = 0; i < std::min(str.size(), mask.getSize()); ++i) {
    if (mask[i]) {
      res += to_upper(str[i]);
    } else {
      res += to_lower(str[i]);
    }
  }
  if (str.size() > mask.getSize()) {
    res += str.substr(mask.getSize(), str.size() - mask.getSize());
  }
  return res;
}

wchar_t alekseev::yes_no(std::wstring answer)
{
  answer = lower_case(answer);
  if (answer == L"yes" || answer == L"y" || answer == L"да" || answer == L"д") {
    return L'y';
  } else if (answer == L"no" || answer == L"n" || answer == L"нет" || answer == L"н") {
    return L'n';
  } else {
    return L'u';
  }
}

bool alekseev::endswith(wstr_cr str, wstr_cr substr)
{
  if (str.size() < substr.size()) {
    return false;
  }
  return str.substr(str.size() - substr.size(), substr.size()) == substr;
}

bool alekseev::endswith(wstr_cr str, std::initializer_list< std::wstring > substr)
{
  bool res = false;
  for (auto i = substr.begin(); i != substr.end() && !res; ++i) {
    res = res || endswith(str, *i);
  }
  return res;
}

size_t alekseev::damerau_levenshtein(wstr_cr a, wstr_cr b)
{
  if (a == b) {
    return 0;
  }
  size_t n = a.size() + 1, m = b.size() + 1;
  size_t * mtx = new size_t[n * m]{0};
  for (size_t i = 0; i < n; ++i) {
    mtx[i * m + 0] = i;
  }
  for (size_t j = 0; j < m; ++j) {
    mtx[0 * m + j] = j;
  }
  for (size_t i = 1; i < n; ++i) {
    for (size_t j = 1; j < m; ++j) {
      size_t candidate0 = mtx[(i - 1) * m + (j - 1)] + (a[i - 1] == b[j - 1] ? 0 : 1);
      size_t candidate1 = std::min(mtx[(i - 1) * m + j] + 1, mtx[i * m + (j - 1)] + 1);
      mtx[i * m + j] = std::min(candidate0, candidate1);
      if (i > 1 && j > 1) {
        if (a[i - 1] == b[j - 2] && a[i - 2] == b[j - 1]) {
          mtx[i * m + j] = std::min(mtx[i * m + j], mtx[(i - 2) * m + (j - 2)] + 1);
        }
      }
    }
  }
  size_t ans = mtx[(n * m - 1)];
  delete[] mtx;
  return ans;
}
