#include "text_manager.h"

#include "dictionary.h"

alekseev::text_t alekseev::from_wstring(wstr_cr name, wstr_cr orig_text)
{
  text_t res{name, {}, {}, {}, {}};
  res.original = split(orig_text, L' ', true);
  res.punctuations = Vector< std::wstring >(res.original.getSize(), {});
  for (size_t i = 0; i < res.original.getSize(); ++i) {
    std::wstring word = rtrim(res.original[i], is_punctuation);
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
  const Vector< std::wstring > * to_join = corrected ?
                                             std::addressof(orig_text.corrected) :
                                             std::addressof(orig_text.original);
  std::wstring res;
  for (size_t i = 0; i < to_join->getSize(); ++i) {
    res += to_join->at(i) + orig_text.punctuations[i] + L" ";
  }
  return res;
}

alekseev::TextManager::TextManager():
  texts_(djb2_hash, poly_hash, equal, 32)
{ }
