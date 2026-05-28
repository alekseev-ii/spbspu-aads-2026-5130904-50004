#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>
#include "../common/vector.h"
#include "cuckoo_hash.h"
#include <fstream>

namespace alekseev {
  enum pos {
    noun, verb, adj
  };

  enum gender {
    nn_gender, feminine, masculine, neuter
  };

  enum number {
    nn_number, singular, plural
  };

  enum case_ {
    nn_case, nominative, genitive, dative, accusative, instrumental, prepositional
  };

  enum tense {
    nn_tense, past, present, future
  };

  enum person {
    nn_person, first, second, third
  };

  enum aspect {
    nn_aspect, perf, imperf
  };

  struct WordForm {
    WordForm();
    std::string word_;
    gender gender_;
    number number_;
    case_ case_;
    tense tense_;
    person person_;

    bool operator==(const WordForm &rhs) const;
  };

  struct Lemma {
    std::string lemma_;
    Vector< WordForm > forms_;
    pos pos_;
    gender noun_gender_;
    aspect verb_aspect_;
  };

  using str_cr = const std::string &;
  size_t djb2_hash(str_cr line);
  size_t poly_hash(str_cr line);
  bool equal(str_cr s1, str_cr s2);
  Vector< std::string > split(const std::string & s, char delim);

  struct Dictionary {
    ~Dictionary() = default;
    Dictionary(const Dictionary &) = default;
    Dictionary & operator=(const Dictionary &) = default;
    Dictionary(Dictionary &&) noexcept = default;
    Dictionary & operator=(Dictionary &&) noexcept = default;

    Dictionary();
    explicit Dictionary(str_cr file_name);

    std::ifstream & read(std::ifstream & is);
    std::ofstream & write(std::ofstream & os);
    size_t size() const;

    private:
      CuckooHash< std::string, Lemma, size_t (*)(str_cr), size_t (*)(str_cr), bool(*)(str_cr,
          str_cr) > lemmas_;
  };
}

#endif
