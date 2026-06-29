#include "text_manager.h"

#include <locale>
#include <codecvt>
#include "dictionary.h"

alekseev::text_t alekseev::from_wstring(wstr_cr orig_text)
{
  text_t res{{}, {}, {}, {}, false};
  res.original = split(replace(orig_text, L"\n", L"\n "));
  res.punctuations = Vector< std::pair< std::wstring, std::wstring > >(res.original.getSize(),
      {{}, {}});
  for (size_t i = 0; i < res.original.getSize(); ++i) {
    size_t orig = res.original[i].size();
    std::wstring word = rtrim(res.original[i], [](wchar_t ch)
    {
      return is_punctuation(ch) || is_whitespace(ch);
    });
    size_t right = word.size();
    word = ltrim(word, [](wchar_t ch)
    {
      return ch == L'\"' || ch == L'\'' || ch == L'«' || ch == L'(' || ch == L'…' || ch == '.';
    });
    size_t left = word.size();
    std::pair< std::wstring, std::wstring > punctuation{{}, {}};
    if (orig != right) {
      punctuation.second = res.original[i].substr(right, orig - right);
    }
    if (left != right) {
      punctuation.first = res.original[i].substr(0, right - left);
    }
    if (word.size() != orig) {
      res.original[i] = word;
      res.punctuations[i] = punctuation;
    }
  }
  return res;
}

std::wstring alekseev::to_wstring(const text_t & orig_text, size_t start, size_t end,
    bool corrected)
{
  const Vector< std::wstring > & to_join = corrected ? orig_text.corrected : orig_text.original;
  return to_wstring(to_join, orig_text.punctuations, start, end);
}

std::wstring alekseev::to_wstring(const Vector< std::wstring > & text,
    const Vector< std::pair< std::wstring, std::wstring > > & punctuation, size_t start, size_t end)
{
  if (end == 0) {
    end = text.getSize();
  }
  if (end > text.getSize() || end <= start) {
    throw std::out_of_range("End greater than size of text or end <= start");
  }
  if (text.getSize() != punctuation.getSize()) {
    throw std::out_of_range("text and punctuations do not match");
  }
  std::wstring res;
  for (size_t i = start; i < end; ++i) {
    res += punctuation[i].first + text[i] + punctuation[i].second;
    if (!endswith(punctuation[i].second, L"\n")) {
      res += L" ";
    }
  }
  return res;
}

alekseev::TextManager::TextManager(DictionaryManager & dict):
  texts_(djb2_hash, poly_hash, equal, 32),
  dict_(dict),
  max_variants_(7),
  distance_(1)
{ }

void alekseev::TextManager::load(wstr_cr file_name, wstr_cr text_name)
{
  if (texts_.contains(text_name)) {
    throw std::invalid_argument("Text already exists");
  }
  std::wstring text;
  std::wstring_convert< std::codecvt_utf8< wchar_t > > converter;
  std::ifstream f(converter.to_bytes(file_name));
  if (!f.is_open()) {
    throw std::invalid_argument("Can not open file!");
  }
  std::string line;
  while (std::getline(f, line)) {
    text += rtrim(converter.from_bytes(line), [](wchar_t ch)
    {
      return ch == L'\r';
    });
    text += L"\n";
  }
  f.close();
  text_t t = from_wstring(text);
  t.saved = true;
  texts_.insert(text_name, t);
  last_loaded_ = text_name;
}

alekseev::wstr_cr alekseev::TextManager::parse(wstr_cr name)
{
  if (name.empty() && last_loaded_.empty()) {
    throw std::invalid_argument("Bad text name for parse!");
  }
  text_t & for_correct = !name.empty() ? texts_.at(name) : texts_.at(last_loaded_);
  if (!for_correct.typos.empty()) {
    for_correct.typos.clear();
  }
  std::wstring last_req;
  bool was_require = false;
  for (size_t i = 0; i < for_correct.original.getSize(); ++i) {
    std::wstring word = lower_case(for_correct.original[i]);
    if (word.empty()) {
      continue;
    }
    if (dict_.is_require(word)) {
      last_req = word;
      was_require = true;
    } else if (dict_.contains_form(word)) {
      if (was_require) {
        if (!dict_.matches_require(last_req, word)) {
          Vector< std::wstring > corrections(1, L"No need correction");
          corrections += dict_.find_by_require(last_req, word, max_variants_, distance_);
          for_correct.typos.push(std::make_pair(i, corrections));
        }
      }
      was_require = false;
    } else {
      Vector< std::wstring > corrections(1, L"No need correction");
      if (was_require) {
        corrections += dict_.find_by_require(last_req, word, max_variants_, distance_);
      } else {
        corrections += dict_.damerau_find_form(word, max_variants_, distance_);
      }
      for_correct.typos.push(std::make_pair(i, corrections));
      was_require = false;
    }
  }
  last_parsed_ = !name.empty() ? name : last_loaded_;
  return last_parsed_;
}

alekseev::wstr_cr alekseev::TextManager::correct(std::wistream & is, std::wostream & os,
    wstr_cr name)
{
  if (name.empty() && last_parsed_.empty()) {
    throw std::invalid_argument("Bad text name for correct!");
  }
  text_t & for_correct = !name.empty() ? texts_.at(name) : texts_.at(last_parsed_);
  if (for_correct.typos.empty()) {
    throw std::invalid_argument("Text not parsed!");
  }
  size_t s = for_correct.original.getSize();
  Vector< std::wstring > corrected(for_correct.original);

  while (!for_correct.typos.empty()) {
    std::pair< size_t, Vector< std::wstring > > & err = for_correct.typos.front();
    size_t i = err.first;
    size_t start = i > 5 ? i - 5 : 0;
    size_t end = s - i > 5 ? i + 5 : s;
    if (start != i) {
      os << to_wstring(corrected, for_correct.punctuations, start, i);
    }
    os << L" [!] " << for_correct.punctuations[i].first;
    os << for_correct.original[i] << for_correct.punctuations[i].second << L" [!] ";
    if (i + 1 < s) {
      os << to_wstring(for_correct, i + 1, end, false);
    }
    os << L"\n";
    size_t n_opts = max_variants_ == 0 ?
                      err.second.getSize() :
                      std::min(max_variants_ + 1, err.second.getSize());
    size_t ans = choose(err.second, is, os, n_opts, L"Choose correction:", L"Your variant...");
    if (ans == n_opts) {
      os << L"Enter your variant >";
      std::wstring word;
      wgetline(is, word);
      corrected[i] = word;
      for_correct.saved = false;
    } else if (ans > 0) {
      corrected[i] = case_from_mask(err.second[ans], mask_from_case(for_correct.original[i]));
      for_correct.saved = false;
    }
    for_correct.typos.pop();
  }
  for_correct.corrected = corrected;
  last_corrected_ = !name.empty() ? name : last_parsed_;
  if (name.empty()) {
    last_parsed_ = L"";
  }
  return last_corrected_;
}

alekseev::wstr_cr alekseev::TextManager::save(wstr_cr file_name, wstr_cr text_name)
{
  if (text_name.empty() && last_corrected_.empty()) {
    throw std::invalid_argument("Bad text name for save!");
  }
  text_t & for_save = text_name.empty() ? texts_.at(last_corrected_) : texts_.at(text_name);
  std::wstring_convert< std::codecvt_utf8< wchar_t > > converter;
  std::ofstream f(converter.to_bytes(file_name));
  if (!f.is_open()) {
    throw std::invalid_argument("Can not open file!");
  }
  f << converter.to_bytes(to_wstring(for_save, 0, 0, !for_save.corrected.isEmpty()));
  if (f.good()) {
    for_save.saved = true;
  }
  f.close();
  return text_name.empty() ? last_corrected_ : text_name;
}

void alekseev::TextManager::unload(wstr_cr name) noexcept
{
  if (name.empty()) {
    return;
  }
  if (!contains(name)) {
    return;
  }
  texts_.remove(name);
  if (last_loaded_ == name) {
    last_loaded_.clear();
  }
  if (last_parsed_ == name) {
    last_parsed_.clear();
  }
  if (last_corrected_ == name) {
    last_corrected_.clear();
  }
}

bool alekseev::TextManager::contains(wstr_cr text_name) const noexcept
{
  return texts_.contains(text_name);
}

bool alekseev::TextManager::is_saved(wstr_cr name) const
{
  return texts_.at(name).saved;
}

size_t alekseev::TextManager::number_of_typos(wstr_cr name) const
{
  return texts_.at(name).typos.size();
}

size_t alekseev::TextManager::max_variants() const noexcept
{
  return max_variants_;
}

void alekseev::TextManager::max_variants(size_t max_variants) noexcept
{
  max_variants_ = max_variants;
}

size_t alekseev::TextManager::default_distance() const noexcept
{
  return distance_;
}

void alekseev::TextManager::default_distance(size_t distance) noexcept
{
  distance_ = distance;
}

alekseev::CuckooHash< std::wstring, alekseev::text_t, size_t (*)(alekseev::wstr_cr),
  size_t (*)(alekseev::wstr_cr), bool(*)(alekseev::wstr_cr,
      alekseev::wstr_cr) >::KeyIterator alekseev::TextManager::texts_begin() const
{
  return texts_.begin();
}

alekseev::CuckooHash< std::wstring, alekseev::text_t, size_t (*)(alekseev::wstr_cr),
  size_t (*)(alekseev::wstr_cr), bool(*)(alekseev::wstr_cr,
      alekseev::wstr_cr) >::KeyIterator alekseev::TextManager::texts_end() const
{
  return texts_.end();
}

std::wstring alekseev::TextManager::last_loaded() const
{
  return last_loaded_;
}

std::wstring alekseev::TextManager::last_parsed() const
{
  return last_parsed_;
}

std::wstring alekseev::TextManager::last_corrected() const
{
  return last_corrected_;
}
