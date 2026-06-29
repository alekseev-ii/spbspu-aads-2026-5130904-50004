#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>
#include "vector.h"
#include "cuckoo_hash.h"
#include <fstream>

namespace alekseev {
  enum pos
  {
    unknown, noun, verb, adj, require, functional
  };

  enum gender
  {
    nn_gender, feminine, masculine, neuter, common
  };

  enum number
  {
    nn_number, singular, plural
  };

  enum case_e
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

  using wstr_cr = const std::wstring &;

  struct WordForm
  {
    WordForm();
    explicit WordForm(std::wstring wordform, pos p, gender g = nn_gender, number n = nn_number,
        case_e c = nn_case, tense t = nn_tense, person pe = nn_person);
    WordForm(const Vector< std::wstring > & tags, pos p);
    WordForm(wstr_cr wordform, pos p, const Vector< std::wstring > & tags);
    std::wstring word_;
    pos pos_;
    gender gender_;
    number number_;
    case_e case_;
    tense tense_;
    person person_;

    bool operator==(const WordForm & rhs) const;
  };

  bool matches(const WordForm & req, const WordForm & word);
  Vector< std::wstring > to_tags(const WordForm & wf);
  WordForm from_tags(Vector< std::wstring >::ConstIterator tags_beg,
      Vector< std::wstring >::ConstIterator tags_end, pos p);
  Vector< std::wstring > to_words(Vector< WordForm >::ConstIterator wfs_beg,
      Vector< WordForm >::ConstIterator wfs_end);
  std::wstring to_wstring(const WordForm & wf);
  std::wostream & operator<<(std::wostream & os, const WordForm & wf);

  struct Lemma
  {
    Lemma();
    std::wstring lemma_;
    Vector< WordForm > forms_;
    pos pos_;
    gender noun_gender_;
    aspect verb_aspect_;
  };

  std::wstring to_wstring(const Lemma & lemma);
  std::wostream & operator<<(std::wostream & os, const Lemma & lemma);

  pos guess_pos(std::wstring word);

  struct Dictionary
  {
    ~Dictionary() = default;
    Dictionary(const Dictionary &) = default;
    Dictionary & operator=(const Dictionary &) = default;
    Dictionary(Dictionary &&) noexcept = default;
    Dictionary & operator=(Dictionary &&) noexcept = default;

    Dictionary();
    Dictionary(size_t lemmas_cap, size_t forms_cap, size_t reqs_cap);
    explicit Dictionary(wstr_cr file_name);

    void read(wstr_cr file_name);
    std::ifstream & read(std::ifstream & is);
    void write(wstr_cr file_name);
    std::ofstream & write(std::ofstream & os);

    void add_lemma(wstr_cr lemma, pos pos, gender noun_gender = nn_gender,
        aspect verb_aspect = nn_aspect);
    void add_form(wstr_cr lemma, wstr_cr wordform, gender g, number n, case_e c, tense t, person p);
    void add_require(wstr_cr require);
    void add_req_form(wstr_cr require, wstr_cr reqform, gender g, number n, case_e c, tense t,
        person p);
    void remove_lemma(wstr_cr lemma);
    void remove_form(const WordForm & wordform);
    void remove_require(wstr_cr require);
    void remove_req_form(wstr_cr require, size_t ind);

    bool contains_lemma(wstr_cr lemma) const;
    bool contains_form(wstr_cr wordform) const;
    bool contains_form(const WordForm & wordform) const;
    bool contains_require(wstr_cr req) const;

    Vector< std::pair< std::wstring, size_t > > & find_homoforms(wstr_cr wordform);
    const Vector< std::pair< std::wstring, size_t > > & find_homoforms(wstr_cr wordform) const;
    std::pair< std::wstring, size_t > & lemma_pair_by_wordform(const WordForm & wordform);
    const std::pair< std::wstring, size_t > & lemma_pair_by_wordform(
        const WordForm & wordform) const;
    std::pair< std::wstring, size_t > req_pair_by_wordform(const WordForm & wordform) const;
    Vector< WordForm > get_homoforms(wstr_cr wordform) const;
    const Lemma & get_lemma(wstr_cr lemma) const;
    Vector< WordForm > & forms_by_lemma(wstr_cr lemma);
    pos pos_of_lemma(wstr_cr lemma) const;
    Vector< pos > pos_of_form(wstr_cr wordform) const;
    Vector< WordForm > filter_by_require(wstr_cr require,
        Vector< WordForm >::ConstIterator forms_beg,
        Vector< WordForm >::ConstIterator forms_end) const;

    bool matches_case(wstr_cr wordform, case_e expected_case) const;
    bool matches_person(wstr_cr wordform, person expected_person) const;
    bool matches_require(wstr_cr require, wstr_cr word) const;
    bool matches_require(wstr_cr require, const WordForm & word) const;

    size_t size() const;

    Vector< WordForm > damerau_find_wfs(wstr_cr bad_word, size_t max_number = 0,
        size_t distance = 2) const;
    Vector< std::wstring > damerau_find_form(wstr_cr bad_form, size_t max_number = 0,
        size_t distance = 2) const;
    Vector< std::wstring > damerau_find_lemma(wstr_cr bad_lemma, size_t max_number = 0,
        size_t distance = 2) const;
    Vector< std::wstring > damerau_find_require(wstr_cr bad_req, size_t max_number = 0,
        size_t distance = 2) const;

    private:
      CuckooHashWStr< Lemma > lemmas_;
      CuckooHashWStr< Vector< std::pair< std::wstring, size_t > > > forms_;
      CuckooHashWStr< Lemma > requires_;
  };

  struct DictionaryManager
  {
    ~DictionaryManager() = default;
    DictionaryManager(const DictionaryManager &) = delete;
    DictionaryManager & operator=(const DictionaryManager &) = delete;
    DictionaryManager(DictionaryManager &&) noexcept = default;
    DictionaryManager & operator=(DictionaryManager &&) noexcept = default;

    DictionaryManager();

    void create(wstr_cr name);
    void load(wstr_cr name, wstr_cr file_name, size_t lemmas = 1024);
    void save(wstr_cr name, wstr_cr file_name);
    void unload(wstr_cr name);
    void set_current(wstr_cr name_of_loaded_dict);
    void add_word(wstr_cr word, std::wistream & is, std::wostream & os);
    void update_word(std::wstring word, std::wistream & is, std::wostream & os);
    bool delete_form(wstr_cr wordform, std::wistream & is, std::wostream & os, WordForm & deleted);
    bool delete_lemma(wstr_cr lemma, std::wistream & is, std::wostream & os,
        std::wstring & deleted);

    bool contains_form(wstr_cr wordform) const;
    bool is_require(wstr_cr word) const;
    Vector< WordForm > filter_by_require(wstr_cr require,
        Vector< WordForm >::ConstIterator wfs_beg,
        Vector< WordForm >::ConstIterator wfs_end) const;
    Vector< std::wstring > damerau_find_form(wstr_cr wordform, size_t max_number = 0,
        size_t distance = 0) const;
    Vector< std::wstring > damerau_find_lemma(wstr_cr wordform, size_t max_number = 0,
        size_t distance = 0) const;
    Vector< std::wstring > find_by_require(wstr_cr require, wstr_cr wordform, size_t max_number = 0,
        size_t distance = 0) const;

    bool matches_case(wstr_cr wordform, case_e expected_case) const;
    bool matches_person(wstr_cr wordform, person expected_person) const;
    bool matches_require(wstr_cr require, wstr_cr word) const;
    bool matches_require(wstr_cr require, const WordForm & word) const;

    Dictionary & current();
    const Dictionary & current() const;

    size_t size() const;
    bool contains_dict(wstr_cr dict_name) const;

    size_t max_variants() const noexcept;
    void max_variants(size_t max_variants) noexcept;
    size_t default_distance() const noexcept;
    void default_distance(size_t distance) noexcept;
    CuckooHash< std::wstring, Dictionary, size_t (*)(wstr_cr), size_t (*)(wstr_cr), bool(*)(
        wstr_cr, wstr_cr) >::KeyIterator dicts_begin() const;
    CuckooHash< std::wstring, Dictionary, size_t (*)(wstr_cr), size_t (*)(wstr_cr), bool(*)(
        wstr_cr, wstr_cr) >::KeyIterator dicts_end() const;
    std::wstring current_dict_name() const;
    void reset_current();

    private:
      CuckooHashWStr< Dictionary > dicts_;
      std::wstring current_;
      size_t max_variants_;
      size_t distance_;

      void add_verb(wstr_cr word, std::wistream & is, std::wostream & os);
      void add_adj(wstr_cr word, std::wistream & is, std::wostream & os);
      void add_noun(wstr_cr word, std::wistream & is, std::wostream & os);
      void add_req(wstr_cr word, std::wistream & is, std::wostream & os);
      void add_functional(wstr_cr word, std::wostream & os);
      std::pair< std::wstring, size_t > choose_wordform(wstr_cr word, std::wistream & is,
          std::wostream & os, size_t max_opts = 0, size_t distance = 0) const;
  };
}

#endif
