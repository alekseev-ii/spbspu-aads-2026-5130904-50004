#ifndef TEXT_MANAGER_H
#define TEXT_MANAGER_H

#include <string>
#include "wstr_functions.h"
#include "../common/vector.h"
#include "cuckoo_hash.h"
#include "../common/queue.h"

namespace alekseev {
  struct position_t
  {
    size_t sentence, word;
    bool operator==(const position_t & rhs) const;
  };

  struct text_t
  {
    std::wstring name;
    Vector< Vector< std::wstring > > original;
    Vector< Vector< std::wstring > > corrected;
    Vector< position_t > errors;
    Queue< std::pair< std::wstring, position_t > > punctuations;
  };

  using wstr_cr = const std::wstring &;
  text_t from_wstring(wstr_cr orig_text);
  std::wstring to_wstring(const text_t & orig_text, bool corrected = true);

  struct TextManager
  {
    void read(wstr_cr file_name, wstr_cr text_name);
    void parse(wstr_cr name = L"");
    void correct(std::wistream & is, std::wostream & os, wstr_cr name = L"");
    void save(wstr_cr file_name, wstr_cr text_name = L"");

    private:
      CuckooHash< std::wstring, text_t, size_t (*)(wstr_cr), size_t (*)(wstr_cr), bool(*)(wstr_cr,
          wstr_cr) > texts_;
      std::wstring last_loaded_;
      std::wstring last_parsed_;
      std::wstring last_corrected_;
  };
}

#endif
