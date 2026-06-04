#include "text_manager.h"

bool alekseev::position_t::operator==(const position_t & rhs) const
{
  return sentence == rhs.sentence && word == rhs.word;
}

alekseev::text_t alekseev::from_wstring(wstr_cr orig_text)
{
  Vector< std::wstring > sentences = split(orig_text, L'.', true);
  Vector< Vector< std::wstring > > words(sentences.getSize(), {});
  Queue< std::pair< std::wstring, position_t > > punctuations;
  for (size_t i = 0; i < sentences.getSize(); ++i) {
    Vector< std::wstring > wds = split(sentences[i], L' ', true);
    size_t punct_words = 0;
    for (size_t j = 0; j < wds.getSize(); ++j) {
      if (is_punctuation_str(wds[j])) {
        punctuations.push(std::make_pair(L" " + wds[j] + L" ", position_t{i, j - ++punct_words}));
      } else {
        std::wstring & before = wds[j];
        std::wstring after = trim(before, is_punctuation);
        size_t trimmed = before.size() - after.size();
        if (trimmed != 0) {
          punctuations.push(std::make_pair(before.substr(before.size() - trimmed, trimmed),
              position_t{i, j - punct_words}));
        }
        words[i].pushBack(after);
      }
    }
  }
  return {{}, words, {}, {}, punctuations};
}

std::wstring alekseev::to_wstring(const text_t & orig_text, bool corrected)
{
  Vector< Vector< std::wstring > > words = corrected ? orig_text.corrected : orig_text.original;
  Queue< std::pair< std::wstring, position_t > > punctuations = orig_text.punctuations;
  std::pair< std::wstring, position_t > dummy{{}, {0, 0}};
  std::pair< std::wstring, position_t > & next_punct = dummy;
  if (!punctuations.empty()) {
    next_punct = punctuations.front();
    punctuations.pop();
  }
  std::wstring result;

  for (size_t i = 0; i < words.getSize(); ++i) {
    if (i > 0) {
      result += L" ";
    }
    for (size_t j = 0; j < words[i].getSize(); ++j) {
      result += words[i][j];
      if (next_punct != dummy) {
        if (position_t{i, j} == next_punct.second) {
          result += next_punct.first;
          if (!punctuations.empty()) {
            next_punct = punctuations.front();
            punctuations.pop();
          } else {
            next_punct = dummy;
          }
        }
      }
      result += L" ";
    }
    result += L".";
  }
  return result;
}
