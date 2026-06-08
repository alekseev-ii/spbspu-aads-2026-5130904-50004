#include "text_manager.h"

#include "dictionary.h"

alekseev::text_t alekseev::from_wstring(wstr_cr orig_text)
{
  text_t res{{}, {}, {}, {}};
  res.original = split(orig_text, L' ');
  res.punctuations = Vector< std::wstring >(res.original.getSize(), {});
  for (size_t i = 0; i < res.original.getSize(); ++i) {
    std::wstring word = rtrim(res.original[i], [](wchar_t ch) {
      return is_punctuation(ch) || is_whitespace(ch);
    });
    size_t a = res.original[i].size(), b = word.size();
    if (a != b) {
      res.punctuations[i] = res.original[i].substr(b, a - b);
      res.original[i] = word;
    }
  }
  return res;
}

std::wstring alekseev::to_wstring(const text_t & orig_text, size_t start, size_t end,
    bool corrected)
{
  const Vector< std::wstring > & to_join = corrected ? orig_text.corrected : orig_text.original;
  if (start == 0 && end == 0) {
    end = to_join.getSize();
  }
  if (end > to_join.getSize() || end <= start) {
    throw std::out_of_range("End greater than size of text or end <= start");
  }
  std::wstring res;
  for (size_t i = start; i < end; ++i) {
    res += to_join[i] + orig_text.punctuations[i] + L" ";
  }
  return res;
}

alekseev::TextManager::TextManager(DictionaryManager & dict):
  texts_(djb2_hash, poly_hash, equal, 32),
  dict_(dict),
  max_variants_(4),
  distance_(2)
{ }

void alekseev::TextManager::read(wstr_cr file_name, wstr_cr text_name)
{
  std::wifstream f(file_name.data());
  if (!f.is_open()) {
    throw std::invalid_argument("Can not open file!");
  }
  std::wstring text;
  while (std::getline(f, text)) {
    text += L"\n";
  }
  f.close();
  texts_.insert(text_name, from_wstring(text));
  last_loaded_ = text_name;
}

void alekseev::TextManager::parse(wstr_cr name)
{
  if (name.empty() && last_loaded_.empty()) {
    throw std::invalid_argument("No text loaded!");
  }
  text_t & for_correct = !name.empty() ? texts_.at(name) : texts_.at(last_loaded_);
  std::wstring last_req;
  bool was_require = false;
  for (size_t i = 0; i < for_correct.original.getSize(); ++i) {
    std::wstring word = lower_case(for_correct.original[i]);
    if (dict_.is_require(word)) {
      last_req = word;
      was_require = true;
    } else if (dict_.contains_form(word)) {
      if (was_require) {
        if (!dict_.matches_require(last_req, word)) {
          Vector< std::wstring > corrections = dict_.find_by_require(last_req, word, max_variants_,
              distance_);
          for_correct.errors.push(std::make_pair(i, corrections));
        }
      }
      was_require = false;
    } else {
      for_correct.errors.push(std::make_pair(i,
          dict_.damerau_find_form(word, max_variants_, distance_)));
      was_require = false;
    }
  }
  last_parsed_ = !name.empty() ? name : last_loaded_;
}

void alekseev::TextManager::correct(std::wistream & is, std::wostream & os, wstr_cr name)
{
  text_t & for_correct = !name.empty() ? texts_.at(name) : texts_.at(last_parsed_);
  if (for_correct.errors.empty()) {
    throw std::invalid_argument("Text not parsed!");
  }
  size_t s = for_correct.original.getSize();
  Vector< std::wstring > corrected(for_correct.original);

  while (!for_correct.errors.empty()) {
    std::pair< size_t, Vector< std::wstring > > & err = for_correct.errors.front();
    size_t i = err.first;
    size_t start = i > 5 ? i - 5 : 0;
    size_t end = s - i > 5 ? i + 5 : s;
    os << to_wstring(for_correct, start, i, false) << L"[!]" << for_correct.original[i];
    os << L"[!]" << to_wstring(for_correct, i + 1, end, false) << L"\n";
    size_t ans = choose(err.second, is, os, max_variants_, L"Choose correction:",
        L"Your variant...");
    if (ans == max_variants_) {
      os << "Enter your variant: ";
      std::wstring word;
      std::getline(is, word);
      corrected[i] = word;
    } else {
      corrected[i] = err.second[ans];
    }
    for_correct.errors.pop();
  }
  last_corrected_ = name;
}

void alekseev::TextManager::save(wstr_cr file_name, wstr_cr text_name)
{
  std::wstring name;
  if (!text_name.empty()) {
    name = text_name;
  } else if (!last_corrected_.empty()) {
    name = last_corrected_;
  } else if (!last_loaded_.empty()) {
    name = last_loaded_;
  } else {
    throw std::invalid_argument("Do not know what to save!");
  }
  text_t & text = texts_.at(name);
  std::wofstream f(name.data());
  if (!f.is_open()) {
    throw std::invalid_argument("Can not open file!");
  }
  f << to_wstring(text, 0, 0, !text.errors.empty());
  f.close();
}
