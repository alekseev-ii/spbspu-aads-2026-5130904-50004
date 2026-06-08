#include <iostream>

#include "dictionary.h"
#include "wstr_functions.h"
#include "text_manager.h"
#include "cuckoo_hash.h"

namespace alekseev {
  struct parameters_t
  {
    Vector< std::wstring > args_;
    std::wistream & is_;
    std::wostream & os_;
    DictionaryManager dicts_;
    TextManager texts_;
  };

  void load_txt(parameters_t & parameters);
  void save_txt(parameters_t & parameters);
  void unload_txt(parameters_t & parameters);
  void parse_txt(parameters_t & parameters);
  void correct_txt(parameters_t & parameters);
  void process_txt(parameters_t & parameters);

  void new_dict(parameters_t & parameters);
  void load_dict(parameters_t & parameters);
  void save_dict(parameters_t & parameters);
  void unload_dict(parameters_t & parameters);
  void current_dict(parameters_t & parameters);
  void add_word_dict(parameters_t & parameters);
  void update_form_dict(parameters_t & parameters);
  void delete_lemma_dict(parameters_t & parameters);
  void delete_form_dict(parameters_t & parameters);

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
      parameters_t parameters_;
  };
}

int main()
{ }
