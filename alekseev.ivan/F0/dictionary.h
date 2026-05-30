#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>
#include "../common/vector.h"
#include "cuckoo_hash.h"
#include <fstream>

namespace alekseev {
  enum pos
  {
    noun, verb, adj
  };

  enum gender
  {
    nn_gender, feminine, masculine, neuter
  };

  enum number
  {
    nn_number, singular, plural
  };

  enum case_
  {
    nn_case, nominative, genitive, dative, accusative, instrumental, prepositional
  };

  enum tense
  {
    nn_tense, past, present, future
  };

  enum person
  {
    nn_person, first, second, third
  };

  enum aspect
  {
    nn_aspect, perf, imperf
  };

  struct WordForm
  {
    WordForm();
    explicit WordForm(std::wstring wordform, gender g = nn_gender, number n = nn_number,
        case_ c = nn_case, tense t = nn_tense, person p = nn_person);
    std::wstring word_;
    gender gender_;
    number number_;
    case_ case_;
    tense tense_;
    person person_;

    bool operator==(const WordForm & rhs) const;
  };

  struct Lemma
  {
    std::wstring lemma_;
    Vector< WordForm > forms_;
    pos pos_;
    gender noun_gender_;
    aspect verb_aspect_;
  };

  using wstr_cr = const std::wstring &;
  size_t djb2_hash(wstr_cr line);
  size_t poly_hash(wstr_cr line);
  bool equal(wstr_cr s1, wstr_cr s2);
  Vector< std::wstring > split(wstr_cr s, wchar_t delim);
  size_t damerau_levenshtein(wstr_cr a, wstr_cr b);

  struct Dictionary
  {
    ~Dictionary() = default;
    Dictionary(const Dictionary &) = default;
    Dictionary & operator=(const Dictionary &) = default;
    Dictionary(Dictionary &&) noexcept = default;
    Dictionary & operator=(Dictionary &&) noexcept = default;

    Dictionary();
    explicit Dictionary(wstr_cr file_name);

    std::wifstream & read(std::wifstream & is);
    std::wofstream & write(std::wofstream & os);
    void add_lemma(wstr_cr lemma, pos pos, gender noun_gender = nn_gender,
        aspect verb_aspect = nn_aspect);
    void add_form(wstr_cr lemma, wstr_cr wordform, gender g = nn_gender, number n = nn_number,
        case_ c = nn_case, tense t = nn_tense, person p = nn_person);
    void remove_lemma(wstr_cr lemma);
    void remove_form(const WordForm & wordform);

    bool contains_lemma(wstr_cr lemma) const;
    bool contains_form(wstr_cr wordform) const;
    bool contains_form(const WordForm & wordform) const;
    Vector< std::pair< std::wstring, size_t > > & find_forms(wstr_cr wordform);
    std::pair< std::wstring, size_t > & find_lemma(const WordForm & wordform);
    const Vector< WordForm > & get_forms(wstr_cr lemma) const;
    Vector< std::wstring > get_lemmas() const;
    size_t size() const;

    private:
      CuckooHash< std::wstring, Lemma, size_t (*)(wstr_cr), size_t (*)(wstr_cr), bool(*)(wstr_cr,
          wstr_cr) > lemmas_;
      CuckooHash< std::wstring, Vector< std::pair< std::wstring, size_t > >, size_t (*)(wstr_cr),
        size_t (*)(wstr_cr), bool(*)(wstr_cr, wstr_cr) > forms_;
  };
}

#endif
