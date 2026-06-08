#include <iostream>

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

  void load_txt(parameters_t & parameters);
  void save_txt(parameters_t & parameters);
  void unload_txt(parameters_t & parameters);
  void parse(parameters_t & parameters);
  void correct(parameters_t & parameters);
  void process(parameters_t & parameters);

  void new_(parameters_t & parameters);
  void load_dict(parameters_t & parameters);
  void save_dict(parameters_t & parameters);
  void unload_dict(parameters_t & parameters);
  void current(parameters_t & parameters);
  void add_word(parameters_t & parameters);
  void update_form(parameters_t & parameters);
  void delete_lemma(parameters_t & parameters);
  void delete_form(parameters_t & parameters);

  struct Exec
  {
    Exec();
    ~Exec() = default;
    Exec(const Exec &) = delete;
    Exec & operator=(const Exec &) = delete;
    Exec(Exec &&) = delete;
    Exec & operator=(Exec &&) = delete;

    void operator()(wstr_cr line);

    private:
      CuckooHash< std::wstring, void(*)(parameters_t &), size_t(*)(wstr_cr),
        size_t(*)(wstr_cr), bool(*)(wstr_cr, wstr_cr) > functions_;
      DictionaryManager dicts_;
      TextManager texts_;
  };
}

int main()
{ }

alekseev::Exec::Exec():
  functions_(djb2_hash, poly_hash, equal, 32),
  texts_(dicts_)
{
  functions_.insert(L"load_txt", std::addressof(load_txt));
  functions_.insert(L"save_txt", std::addressof(save_txt));
  functions_.insert(L"unload_txt", std::addressof(unload_txt));
  functions_.insert(L"parse", std::addressof(parse));
  functions_.insert(L"correct", std::addressof(correct));
  functions_.insert(L"process", std::addressof(process));

  functions_.insert(L"new_dict", std::addressof(new_));
  functions_.insert(L"load_dict", std::addressof(load_dict));
  functions_.insert(L"save_dict", std::addressof(save_dict));
  functions_.insert(L"unload_dict", std::addressof(unload_dict));
  functions_.insert(L"current_dict", std::addressof(current));
  functions_.insert(L"add", std::addressof(add_word));
  functions_.insert(L"update", std::addressof(update_form));
  functions_.insert(L"delete_lemma", std::addressof(delete_lemma));
  functions_.insert(L"delete_form", std::addressof(delete_form));
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
  parameters_t params{args, std::wcin, std::wcout, dicts_, texts_};
  functions_.at(words[0])(params);
}
