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

std::wstring alekseev::to_wstring(const text_t & orig_text, bool corrected)
{
  const Vector< std::wstring > & to_join = corrected ? orig_text.corrected : orig_text.original;
  std::wstring res;
  for (size_t i = 0; i < to_join.getSize(); ++i) {
    res += to_join[i] + orig_text.punctuations[i] + L" ";
  }
  return res;
}

alekseev::TextManager::TextManager(DictionaryManager & dict):
  texts_(djb2_hash, poly_hash, equal, 32),
  dict_(dict)
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
          Vector< std::wstring > corrections = dict_.find_by_require(last_req, word);
          for_correct.errors.push(std::make_pair(i, corrections));
        }
      }
      was_require = false;
    } else {
      for_correct.errors.push(std::make_pair(i, dict_.damerau_find_form(word)));
      was_require = false;
    }
  }
  last_parsed_ = !name.empty() ? name : last_loaded_;
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
  bool corrected = !text.errors.empty();
  std::wofstream f(name.data());
  if (!f.is_open()) {
    throw std::invalid_argument("Can not open file!");
  }
  f << to_wstring(text, corrected);
  f.close();
}
