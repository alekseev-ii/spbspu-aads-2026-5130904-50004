#ifndef TEXT_MANAGER_H
#define TEXT_MANAGER_H

#include <string>
#include "wstr_functions.h"
#include "../common/vector.h"
#include "cuckoo_hash.h"
#include "../common/queue.h"
#include "dictionary.h"

namespace alekseev {
  struct text_t
  {
    Vector< std::wstring > original;
    Vector< std::wstring > corrected;
    Vector< std::wstring > punctuations;
    Queue< std::pair< size_t, Vector< std::wstring > > > errors;
  };

  using wstr_cr = const std::wstring &;
  text_t from_wstring(wstr_cr orig_text);
  std::wstring to_wstring(const text_t & orig_text, size_t start = 0, size_t end = 0, bool corrected = true);

  struct TextManager
  {
    explicit TextManager(DictionaryManager & dict);
    ~TextManager() = default;
    TextManager(const TextManager &) = delete;
    TextManager & operator=(const TextManager &) = delete;
    TextManager(TextManager &&) noexcept = default;
    TextManager & operator=(TextManager &&) noexcept = delete;

    void load(wstr_cr file_name, wstr_cr text_name);
    void parse(wstr_cr name = L"");
    void correct(std::wistream & is, std::wostream & os, wstr_cr name = L"");
    void save(wstr_cr file_name, wstr_cr text_name = L"");
    void unload(wstr_cr name = L"");

    private:
      CuckooHash< std::wstring, text_t, size_t (*)(wstr_cr), size_t (*)(wstr_cr), bool(*)(wstr_cr,
          wstr_cr) > texts_;
      std::wstring last_loaded_;
      std::wstring last_parsed_;
      std::wstring last_corrected_;
      std::wstring last_saved_;
      DictionaryManager & dict_;
      size_t max_variants_;
      size_t distance_;
  };
}

#endif
