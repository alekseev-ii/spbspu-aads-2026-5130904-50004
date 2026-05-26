#include "dictionary.h"

size_t alekseev::djb2_hash(str_cr line)
{
  size_t hash = 5381;
  for (size_t i = 0; i < line.size(); ++i) {
    hash = ((hash << 5) + hash) + line[i];
  }
  return hash;
}

size_t alekseev::poly_hash(str_cr line)
{
  size_t hash = 0;
  int p = 67;
  for (size_t i = 0; i < line.size(); ++i) {
    hash = hash * p + line[i];
  }
  return hash;
}

bool alekseev::equal(str_cr s1, str_cr s2)
{
  return s1 == s2;
}

alekseev::Vector< std::string > alekseev::split(const std::string & s, char delim)
{
  size_t start = 0;
  Vector< std::string > res;
  size_t i = 0;
  for (; i < s.size(); ++i) {
    if (s[i] == delim) {
      if (i > start) {
        res.pushBack(s.substr(start, i - start));
      }
      start = i + 1;
    }
  }
  res.pushBack(s.substr(start, i - start));
  return res;
}

alekseev::Dictionary::Dictionary():
  lemmas_(djb2_hash, poly_hash, equal, 16384)
{
}

alekseev::Dictionary::Dictionary(str_cr file_name):
  lemmas_(djb2_hash, poly_hash, equal, 16384)
{
  std::ifstream is(file_name);
  try {
    read(is);
  } catch (...) {
    is.close();
    throw;
  }
  is.close();
}

std::ifstream & alekseev::Dictionary::read(std::ifstream & is)
{
  std::string line;
  Lemma lemma;
  while (std::getline(is, line)) {
    if (line[1] == '#') {
      continue;
    }
    if (line[0] != ' ') {
      Vector< std::string > words = split(line, ' ');
      lemma.lemma_ = words[0];
      if (words[1] == "noun") {
        lemma.default_pos = noun;
      } else if (words[1] == "verb") {
        lemma.default_pos = verb;
      } else if (words[1] == "adj") {
        lemma.default_pos = adj;
      }
    }
  }
}
