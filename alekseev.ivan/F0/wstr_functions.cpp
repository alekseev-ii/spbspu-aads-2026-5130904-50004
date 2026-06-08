#include "wstr_functions.h"

#include <windows.h>
#include <fcntl.h>

size_t alekseev::djb2_hash(wstr_cr line)
{
  size_t hash = 5381;
  for (size_t i = 0; i < line.size(); ++i) {
    hash = ((hash << 5) + hash) + line[i];
  }
  return hash;
}

size_t alekseev::poly_hash(wstr_cr line)
{
  size_t hash = 0;
  int p = 67;
  for (size_t i = 0; i < line.size(); ++i) {
    hash = hash * p + line[i];
  }
  return hash;
}

bool alekseev::equal(wstr_cr s1, wstr_cr s2)
{
  return s1 == s2;
}

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

  size_t size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast< int >(str.size()),
      nullptr, 0);
  std::wstring wstr(size_needed, 0);
  MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast< int >(str.size()), &wstr[0],
      size_needed);
  return wstr;
}

std::string alekseev::wstring_to_utf8(wstr_cr wstr)
{
  if (wstr.empty()) {
    return {};
  }
  size_t size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
      static_cast< int >(wstr.size()),
      nullptr, 0, nullptr, nullptr);
  std::string result(size_needed, 0);
  WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast< int >(wstr.size()),
      &result[0], size_needed, nullptr, nullptr);
  return result;
}

std::wstring alekseev::trim(const std::wstring & str)
{
  return trim(str, is_whitespace);
}

std::wstring alekseev::ltrim(const std::wstring & str)
{
  return ltrim(str, is_whitespace);
}

std::wstring alekseev::rtrim(const std::wstring & str)
{
  return rtrim(str, is_whitespace);
}

bool alekseev::is_lower(wchar_t ch)
{
  return ch == to_lower(ch);
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

bool alekseev::is_upper(wchar_t ch)
{
  return ch == to_upper(ch);
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
    res[i] = is_upper(ch);
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

wchar_t alekseev::ask_yes_no(wstr_cr question, std::wistream & is, std::wostream & os,
    bool need_cycle, bool add_variants)
{
  os << question;
  if (add_variants) {
    os << L" (yes/no) ";
  }
  std::wstring answer;
  while (std::getline(is, answer)) {
    wchar_t ans = yes_no(answer);
    if (ans == L'y' || ans == L'n' || !need_cycle) {
      return ans;
    } else {
      os << L"Unknown answer: " << answer << L"\n";
      os << question;
      if (add_variants) {
        os << L" (yes/no) ";
      }
    }
  }
  return L'u';
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
    res = endswith(str, *i);
  }
  return res;
}

bool alekseev::is_punctuation(wchar_t ch)
{
  std::wstring punctuation = L"!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~«»„“—–…№";
  for (size_t i = 0; i < punctuation.size(); ++i) {
    if (punctuation[i] == ch) {
      return true;
    }
  }
  return false;
}

bool alekseev::is_punctuation_str(wstr_cr str)
{
  bool res = is_punctuation(str[0]);
  for (size_t i = 1; i < str.size() && res; ++i) {
    res = is_punctuation(str[i]);
  }
  return res;
}

bool alekseev::is_whitespace(wchar_t ch)
{
  std::wstring ws = L" \t\n\r\v\f\u00A0\u2000\u2001\u2002\u2003\u2004"
      "\u2005\u2006\u2007\u2008\u2009\u200A\u2028\u2029\u202F\u205F\u3000";
  for (size_t i = 0; i < ws.size(); ++i) {
    if (ws[i] == ch) {
      return true;
    }
  }
  return false;
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

alekseev::Vector< std::wstring > alekseev::damerau_find(wstr_cr bad_word,
    const Vector< std::wstring > & candidates, size_t max_number, size_t distance)
{
  Vector< std::wstring > res;
  size_t m = max_number == 0 ? candidates.getSize() : max_number;
  for (size_t i = 0; i < candidates.getSize() && res.getSize() < m; ++i) {
    if (candidates[i].size() - bad_word.size() <= distance) {
      if (damerau_levenshtein(candidates[i], bad_word) <= distance) {
        res.pushBack(candidates[i]);
      }
    }
  }
  return res;
}
