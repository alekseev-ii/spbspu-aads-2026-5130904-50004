#include <iostream>
#include <functional>

#include "dictionary.h"
#include "wstr_functions.h"
#include "text_manager.h"
#include "cuckoo_hash.h"

namespace alekseev {
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

      void load_txt(Vector< std::wstring > & args);
      void save_txt(Vector< std::wstring > & args);
      void unload_txt(Vector< std::wstring > & args);
      void parse(Vector< std::wstring > & args);
      void correct(Vector< std::wstring > & args);
      void process(Vector< std::wstring > & args);

      void new_(Vector< std::wstring > & args);
      void load_dict(Vector< std::wstring > & args);
      void save_dict(Vector< std::wstring > & args);
      void unload_dict(Vector< std::wstring > & args);
      void current(Vector< std::wstring > & args);
      void add_word(Vector< std::wstring > & args);
      void update_form(Vector< std::wstring > & args);
      void delete_lemma(Vector< std::wstring > & args);
      void delete_form(Vector< std::wstring > & args);
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
    throw std::invalid_argument("Bad command name!");
  }
  Vector< std::wstring > args = words;
  args.erase(0);
  functions_.at(words[0])(args);
}

void alekseev::Exec::load_txt(Vector< std::wstring > & args)
{
  if (args.getSize() != 2) {
    throw std::invalid_argument("Bad arguments number!");
  }
  os_ << L"Loading text \"" << args[0] << "\" from " << args[1] << L"\n";
  texts_.load(args[1], args[0]);
  os_ << args[0] << L" successfully loaded\n";
}

void alekseev::Exec::save_txt(Vector< std::wstring > & args)
{
  if (args.getSize() == 1) {
    os_ << texts_.save(args[0]) << L" successfully saved\n";
  } else if (args.getSize() == 2) {
    os_ << texts_.save(args[1], args[0]) << L" successfully saved\n";
  } else {
    throw std::invalid_argument("Bad arguments number!");
  }
}

void alekseev::Exec::unload_txt(Vector< std::wstring > & args)
{
  if (args.getSize() == 1) {
    if (!texts_.is_saved(args[0])) {
      wchar_t need_save = ask_yes_no(L"Do you want to save text before unloading?", is_, os_);
      if (need_save == 'y') {
        os_ << "Enter file name for saving: ";
        std::wstring file_name;
        std::getline(is_, file_name);
        os_ << L"Saving " << args[0] << L" to " << file_name << L"\n";
        texts_.save(file_name, args[0]);
        os_ << args[0] << L" successfully saved\n";
      }
    }
    texts_.unload(args[0]);
    os_ << args[0] << L" unloaded\n";
  } else {
    throw std::invalid_argument("Bad arguments number!");
  }
}

void alekseev::Exec::parse(Vector< std::wstring > & args)
{
  if (args.isEmpty()) {
    os_ << L"Parsing...\n";
    os_ << texts_.parse() << L" successfully parsed";
  } else if (args.getSize() == 1) {
    os_ << L"Parsing...\n";
    os_ << texts_.parse(args[0]) << L" successfully parsed";
  } else {
    throw std::invalid_argument("Bad arguments number!");
  }
}

void alekseev::Exec::correct(Vector< std::wstring > & args)
{
  if (args.isEmpty()) {
    texts_.correct(is_, os_);
  } else if (args.getSize() == 1) {
    texts_.correct(is_, os_, args[0]);
  } else {
    throw std::invalid_argument("Bad arguments number!");
  }
}

void alekseev::Exec::process(Vector< std::wstring > & args)
{
  if (args.getSize() != 1) {
    throw std::invalid_argument("Bad arguments number!");
  }
  std::wstring temp_name = L"__temporary_text_name_for_process__";
  while (texts_.contains(temp_name)) {
    temp_name += L"_";
  }
  texts_.load(args[0], temp_name);
  os_ << L"Text loaded\nParsing...\n";
  try {
    texts_.parse(temp_name);
    os_ << L"Text parsed\n";
    texts_.correct(is_, os_, temp_name);
    os_ << L"Saving...\n";
    texts_.save(args[0], temp_name);
    os_ << L"Saved\n";
    texts_.unload(temp_name);
    os_ << L"Unloaded\n";
    os_ << args[0] << L" processed successfully!\n";
  } catch (...) {
    texts_.unload(temp_name);
  }
}

void alekseev::Exec::new_(Vector< std::wstring > & args)
{
  if (args.getSize() != 1) {
    throw std::invalid_argument("Bad arguments number!");
  }
  dicts_.create(args[0]);
  os_ << L"An empty dictionary \"" << args[0] << "\" has been created";
}

void alekseev::Exec::load_dict(Vector< std::wstring > & args)
{
  if (args.getSize() != 2) {
    throw std::invalid_argument("Bad arguments number!");
  }
  os_ << "Loading...\n";
  dicts_.load(args[0], args[1]);
  os_ << L"Successfully loaded dictionary \"" << args[0] << "\" from \"" << args[1] << "\"\n";
}

void alekseev::Exec::save_dict(Vector< std::wstring > & args)
{
  if (args.getSize() != 2) {
    throw std::invalid_argument("Bad arguments number!");
  }
  os_ << "Saving...\n";
  dicts_.save(args[0], args[1]);
  os_ << L"Saved dictionary \"" << args[0] << "\" to \"" << args[1] << "\"\n";
}

void alekseev::Exec::unload_dict(Vector< std::wstring > & args)
{
  if (args.getSize() != 1) {
    throw std::invalid_argument("Bad arguments number!");
  }
  dicts_.unload(args[0]);
  os_ << L"Unloaded dictionary \"" << args[0] << "\"\n";
}

void alekseev::Exec::current(Vector< std::wstring > & args)
{
  if (args.getSize() != 1) {
    throw std::invalid_argument("Bad arguments number!");
  }
  dicts_.set_current(args[0]);
}

void alekseev::Exec::add_word(Vector< std::wstring > & args)
{
  if (args.getSize() != 1) {
    throw std::invalid_argument("Bad arguments number!");
  }
  dicts_.add_word(args[0], is_, os_);
}

void alekseev::Exec::update_form(Vector< std::wstring > & args)
{
  if (args.getSize() != 1) {
    throw std::invalid_argument("Bad arguments number!");
  }
  dicts_.update_word(args[0], is_, os_);
}

void alekseev::Exec::delete_lemma(Vector< std::wstring > & args)
{
  if (args.getSize() != 1) {
    throw std::invalid_argument("Bad arguments number!");
  }
  dicts_.delete_lemma(args[0], is_, os_);
}

void alekseev::Exec::delete_form(Vector< std::wstring > & args)
{
  if (args.getSize() != 1) {
    throw std::invalid_argument("Bad arguments number!");
  }
  dicts_.delete_form(args[0], is_, os_);
}
