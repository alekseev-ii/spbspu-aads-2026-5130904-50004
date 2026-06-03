#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>
#include "../common/vector.h"
#include "cuckoo_hash.h"
#include <fstream>
#include <windows.h>

namespace alekseev {
  enum pos
  {
    noun, verb, adj, unknown
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
     WordForm(std::wstring word, pos p, Vector< std::wstring > tags);
    std::wstring word_;
    gender gender_;
    number number_;
    case_ case_;
    tense tense_;
    person person_;

    bool operator==(const WordForm & rhs) const;
  };

  Vector< std::wstring > tags(const WordForm & wf);
  std::wostream & operator<<(std::wostream & os, const WordForm & wf);

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
  pos guess_pos(std::wstring word);

  struct ConsoleSetup
  {
    ConsoleSetup();
    ~ConsoleSetup();

    private:
      int old_cin_mode_, old_cout_mode_, old_cerr_mode_;
      UINT old_output_cp_, old_input_cp_;
  };

  struct Dictionary
  {
    ~Dictionary() = default;
    Dictionary(const Dictionary &) = default;
    Dictionary & operator=(const Dictionary &) = default;
    Dictionary(Dictionary &&) noexcept = default;
    Dictionary & operator=(Dictionary &&) noexcept = default;

    Dictionary();
    explicit Dictionary(wstr_cr file_name);

    void read(wstr_cr file_name);
    std::ifstream & read(std::ifstream & is);
    void write(wstr_cr file_name);
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

    Vector< std::pair< std::wstring, size_t > > & find_homoforms(wstr_cr wordform);
    std::pair< std::wstring, size_t > & lemma_pair_by_wordform(const WordForm & wordform);
    Vector< WordForm > get_homoforms(wstr_cr wordform) const;
    Vector< std::wstring > get_lemmas() const;
    Vector< WordForm > & forms_by_lemma(wstr_cr lemma);
    pos pos_of_lemma(wstr_cr lemma) const;

    bool matches_case(wstr_cr wordform, case_ expected_case) const;
    bool matches_person(wstr_cr wordform, person expected_person) const;

    size_t size() const;

    Vector< WordForm > damerau_find_wfs(wstr_cr bad_word, size_t distance = 2);
    Vector< std::wstring > damerau_find_form(wstr_cr bad_form, size_t distance = 2) const;
    Vector< std::wstring > damerau_find_lemma(wstr_cr bad_lemma, size_t distance = 2) const;

    private:
      CuckooHash< std::wstring, Lemma, size_t (*)(wstr_cr), size_t (*)(wstr_cr), bool(*)(wstr_cr,
          wstr_cr) > lemmas_;
      CuckooHash< std::wstring, Vector< std::pair< std::wstring, size_t > >, size_t (*)(wstr_cr),
        size_t (*)(wstr_cr), bool(*)(wstr_cr, wstr_cr) > forms_;
  };

  struct DictionaryManager
  {
    ~DictionaryManager() = default;
    DictionaryManager(const DictionaryManager &) = default;
    DictionaryManager & operator=(const DictionaryManager &) = default;
    DictionaryManager(DictionaryManager &&) noexcept = default;
    DictionaryManager & operator=(DictionaryManager &&) noexcept = default;

    DictionaryManager();

    void create(wstr_cr name);
    void load(wstr_cr name, wstr_cr file_name);
    void save(wstr_cr name, wstr_cr file_name);
    void unload(wstr_cr name);
    void set_current(wstr_cr name_of_loaded_dict);
    void add_word(wstr_cr word, std::wistream & is, std::wostream & os);
    void update_word(wstr_cr word, std::wistream & is, std::wostream & os);

    private:
      CuckooHash< std::wstring, Dictionary, size_t (*)(wstr_cr), size_t (*)(wstr_cr), bool(*)(
          wstr_cr, wstr_cr) > dicts_;
      std::wstring current_;

      void add_verb(wstr_cr word, std::wistream & is, std::wostream & os);
      void add_adj(wstr_cr word, std::wistream & is, std::wostream & os);
      void add_noun(wstr_cr word, std::wistream & is, std::wostream & os);
  };
}

#endif
