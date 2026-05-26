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
