#include <iostream>
#include <functional>

#include "dictionary.h"
#include "wstr_functions.h"
#include "text_manager.h"
#include "cuckoo_hash.h"

namespace alekseev {
  struct parameters_t
  {
    Vector< std::wstring > & args_;
    std::wistream & is_;
    std::wostream & os_;
    DictionaryManager & dicts_;
    TextManager & texts_;
  };

  struct Exec
  {
    Exec(std::wistream & is, std::wostream & os);
    ~Exec() = default;
    Exec(const Exec &) = delete;
    Exec & operator=(const Exec &) = delete;
    Exec(Exec &&) = delete;
    Exec & operator=(Exec &&) = delete;

    void operator()(wstr_cr line);

    private:
      CuckooHash< std::wstring, std::function< void(Vector< std::wstring > &) >, size_t(*)(wstr_cr),
        size_t(*)(wstr_cr), bool(*)(wstr_cr, wstr_cr) > functions_;
      DictionaryManager dicts_;
      TextManager texts_;
      std::wistream & is_;
      std::wostream & os_;

      void load_txt(Vector< std::wstring > & args_);
      void save_txt(Vector< std::wstring > & args_);
      void unload_txt(Vector< std::wstring > & args_);
      void parse(Vector< std::wstring > & args_);
      void correct(Vector< std::wstring > & args_);
      void process(Vector< std::wstring > & args_);

      void new_(Vector< std::wstring > & args_);
      void load_dict(Vector< std::wstring > & args_);
      void save_dict(Vector< std::wstring > & args_);
      void unload_dict(Vector< std::wstring > & args_);
      void current(Vector< std::wstring > & args_);
      void add_word(Vector< std::wstring > & args_);
      void update_form(Vector< std::wstring > & args_);
      void delete_lemma(Vector< std::wstring > & args_);
      void delete_form(Vector< std::wstring > & args_);
  };
}

int main()
{ }

alekseev::Exec::Exec(std::wistream & is, std::wostream & os):
  functions_(djb2_hash, poly_hash, equal, 32),
  texts_(dicts_),
  is_(is),
  os_(os)
{
  functions_.insert(L"load_txt", [this](Vector< std::wstring > & args) {
    load_txt(args);
  });
  functions_.insert(L"save_txt", [this](Vector< std::wstring > & args) {
    save_txt(args);
  });
  functions_.insert(L"unload_txt", [this](Vector< std::wstring > & args) {
    unload_txt(args);
  });
  functions_.insert(L"parse", [this](Vector< std::wstring > & args) {
    parse(args);
  });
  functions_.insert(L"correct", [this](Vector< std::wstring > & args) {
    correct(args);
  });
  functions_.insert(L"process", [this](Vector< std::wstring > & args) {
    process(args);
  });

  functions_.insert(L"new_dict", [this](Vector< std::wstring > & args) {
    new_(args);
  });
  functions_.insert(L"load_dict", [this](Vector< std::wstring > & args) {
    load_dict(args);
  });
  functions_.insert(L"save_dict", [this](Vector< std::wstring > & args) {
    save_dict(args);
  });
  functions_.insert(L"unload_dict", [this](Vector< std::wstring > & args) {
    unload_dict(args);
  });
  functions_.insert(L"current_dict", [this](Vector< std::wstring > & args) {
    current(args);
  });
  functions_.insert(L"add", [this](Vector< std::wstring > & args) {
    add_word(args);
  });
  functions_.insert(L"update", [this](Vector< std::wstring > & args) {
    update_form(args);
  });
  functions_.insert(L"delete_lemma", [this](Vector< std::wstring > & args) {
    delete_lemma(args);
  });
  functions_.insert(L"delete_form", [this](Vector< std::wstring > & args) {
    delete_form(args);
  });
}

void alekseev::Exec::operator()(wstr_cr line)
{
  Vector< std::wstring > words = split(line, L' ', true);
  if (words.isEmpty()) {
    throw std::invalid_argument("Empty input!");
  }
  if (!functions_.contains(words[0])) {
    throw std::invalid_argument("Bad command!");
  }
  Vector< std::wstring > args = words;
  args.erase(0);
  functions_.at(words[0])(args);
}
