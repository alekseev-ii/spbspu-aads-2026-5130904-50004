#ifndef TEXT_MANAGER_H
#define TEXT_MANAGER_H

#include <string>
#include "wstr_functions.h"
#include "vector.h"
#include "cuckoo_hash.h"
#include "queue.h"
#include "dictionary.h"

namespace alekseev {
  struct text_t
  {
    Vector< std::wstring > original;
    Vector< std::wstring > corrected;
    Vector< std::pair< std::wstring, std::wstring > > punctuations;
    Queue< std::pair< size_t, Vector< std::wstring > > > typos;
    bool saved = false;
  };

  using wstr_cr = const std::wstring &;
  text_t from_wstring(wstr_cr orig_text);
  std::wstring to_wstring(const text_t & orig_text, size_t start = 0,
      size_t end = 0, bool corrected = true);
  std::wstring to_wstring(const Vector< std::wstring > & text,
      const Vector< std::pair< std::wstring, std::wstring > > & punctuation,
      size_t start = 0, size_t end = 0);

  struct TextManager
  {
    explicit TextManager(DictionaryManager & dict);
    ~TextManager() = default;
    TextManager(const TextManager &) = delete;
    TextManager & operator=(const TextManager &) = delete;
    TextManager(TextManager &&) noexcept = default;
    TextManager & operator=(TextManager &&) noexcept = delete;

    void load(wstr_cr file_name, wstr_cr text_name);
    wstr_cr parse(wstr_cr name = L"");
    wstr_cr correct(std::wistream & is, std::wostream & os, wstr_cr name = L"");
    wstr_cr save(wstr_cr file_name, wstr_cr text_name = L"");
    void unload(wstr_cr name) noexcept;

    bool contains(wstr_cr text_name) const noexcept;
    bool is_saved(wstr_cr name) const;
    size_t number_of_typos(wstr_cr name) const;
    size_t max_variants() const noexcept;
    void max_variants(size_t max_variants) noexcept;
    size_t default_distance() const noexcept;
    void default_distance(size_t distance) noexcept;

    CuckooHash< std::wstring, text_t, size_t (*)(wstr_cr), size_t (*)(wstr_cr), bool(*)(wstr_cr,
        wstr_cr) >::KeyIterator texts_begin() const;
    CuckooHash< std::wstring, text_t, size_t (*)(wstr_cr), size_t (*)(wstr_cr), bool(*)(wstr_cr,
        wstr_cr) >::KeyIterator texts_end() const;
    std::wstring last_loaded() const;
    std::wstring last_parsed() const;
    std::wstring last_corrected() const;

    private:
      CuckooHash< std::wstring, text_t, size_t (*)(wstr_cr), size_t (*)(wstr_cr), bool(*)(wstr_cr,
          wstr_cr) > texts_;
      std::wstring last_loaded_;
      std::wstring last_parsed_;
      std::wstring last_corrected_;
      DictionaryManager & dict_;
      size_t max_variants_;
      size_t distance_;
  };
}

#endif
