#include <iostream>
#include <functional>
#include <iomanip>

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
      void texts(Vector< std::wstring > &);

      void new_(Vector< std::wstring > & args);
      void load_dict(Vector< std::wstring > & args);
      void save_dict(Vector< std::wstring > & args);
      void unload_dict(Vector< std::wstring > & args);
      void current(Vector< std::wstring > & args);
      void add_word(Vector< std::wstring > & args);
      void update_form(Vector< std::wstring > & args);
      void delete_lemma(Vector< std::wstring > & args);
      void delete_form(Vector< std::wstring > & args);
      void dicts(Vector< std::wstring > &);

      void max_variants_txt(Vector< std::wstring > & args);
      void distance_of_find_txt(Vector< std::wstring > & args);
      void max_variants_dict(Vector< std::wstring > & args);
      void distance_of_find_dict(Vector< std::wstring > & args);

      void help(Vector< std::wstring > &);
  };
}

int main()
{
  alekseev::ConsoleSetup console_setup;
  alekseev::Exec exec(std::wcin, std::wcout);

  std::wstring line;
  while (alekseev::wgetline(std::wcin, line)) {
    try {
      if (line == L"exit") {
        return 0;
      }
      exec(line);
    } catch (std::invalid_argument & e) {
      std::wcout << e.what() << L"\n";
    } catch (std::out_of_range & e) {
      std::wcout << e.what() << L"\n";
    } catch (std::exception & e) {
      std::wcout << e.what() << L"\n";
      return 1;
    }
  }
}

alekseev::Exec::Exec(std::wistream & is, std::wostream & os):
  functions_(djb2_hash, poly_hash, equal, 32),
  texts_(dicts_),
  is_(is),
  os_(os)
{
  functions_.insert(L"load_txt", [this](Vector< std::wstring > & args)
  {
    load_txt(args);
  });
  functions_.insert(L"save_txt", [this](Vector< std::wstring > & args)
  {
    save_txt(args);
  });
  functions_.insert(L"unload_txt", [this](Vector< std::wstring > & args)
  {
    unload_txt(args);
  });
  functions_.insert(L"parse", [this](Vector< std::wstring > & args)
  {
    parse(args);
  });
  functions_.insert(L"correct", [this](Vector< std::wstring > & args)
  {
    correct(args);
  });
  functions_.insert(L"process", [this](Vector< std::wstring > & args)
  {
    process(args);
  });
  functions_.insert(L"texts", [this](Vector< std::wstring > & args)
  {
    texts(args);
  });

  functions_.insert(L"new", [this](Vector< std::wstring > & args)
  {
    new_(args);
  });
  functions_.insert(L"load_dict", [this](Vector< std::wstring > & args)
  {
    load_dict(args);
  });
  functions_.insert(L"save_dict", [this](Vector< std::wstring > & args)
  {
    save_dict(args);
  });
  functions_.insert(L"unload_dict", [this](Vector< std::wstring > & args)
  {
    unload_dict(args);
  });
  functions_.insert(L"current", [this](Vector< std::wstring > & args)
  {
    current(args);
  });
  functions_.insert(L"add", [this](Vector< std::wstring > & args)
  {
    add_word(args);
  });
  functions_.insert(L"update", [this](Vector< std::wstring > & args)
  {
    update_form(args);
  });
  functions_.insert(L"delete_lemma", [this](Vector< std::wstring > & args)
  {
    delete_lemma(args);
  });
  functions_.insert(L"delete_form", [this](Vector< std::wstring > & args)
  {
    delete_form(args);
  });
  functions_.insert(L"dicts", [this](Vector< std::wstring > & args)
  {
    dicts(args);
  });

  functions_.insert(L"max_variants_txt", [this](Vector< std::wstring > & args)
  {
    max_variants_txt(args);
  });
  functions_.insert(L"distance_of_find_txt", [this](Vector< std::wstring > & args)
  {
    distance_of_find_txt(args);
  });
  functions_.insert(L"max_variants_dict", [this](Vector< std::wstring > & args)
  {
    max_variants_dict(args);
  });
  functions_.insert(L"distance_of_find_dict", [this](Vector< std::wstring > & args)
  {
    distance_of_find_dict(args);
  });
  functions_.insert(L"help", [this](Vector< std::wstring > & args)
  {
    help(args);
  });

  Vector< std::wstring > opts{L"Load ~1200 lemmas (~ 23 s)", L"Load ~1700 lemmas (~ 1 m 17 s)"};
  size_t opt = choose(opts, is, os, 0, L"Do you want to load default dictionary?",
      L"Do not load default dictionary");
  if (opt == 2) {
    return;
  }
  try {
    dicts_.load(L"default_dictionary_requires",
        L"./default_dictionaries/default_dictionary_requires.txt");
    dicts_.load(L"default_dictionary_functional",
        L"./default_dictionaries/default_dictionary_functional.txt");
    if (opt == 0) {
      dicts_.load(L"default_dictionary_300_adjectives",
          L"./default_dictionaries/default_dictionary_300_adjectives.txt");
      dicts_.load(L"default_dictionary_300_verbs",
          L"./default_dictionaries/default_dictionary_300_verbs.txt");
      dicts_.load(L"default_dictionary_300_nouns",
          L"./default_dictionaries/default_dictionary_300_nouns.txt");
    } else if (opt == 1) {
      dicts_.load(L"default_dictionary_500_adjectives",
          L"./default_dictionaries/default_dictionary_500_adjectives.txt", 2048);
      dicts_.load(L"default_dictionary_500_verbs",
          L"./default_dictionaries/default_dictionary_500_verbs.txt", 2048);
      dicts_.load(L"default_dictionary_500_nouns",
          L"./default_dictionaries/default_dictionary_500_nouns.txt", 2048);
    }
    std::wcout << L"Successfully loaded " << dicts_.size() << L" word forms\n";
  } catch (std::exception & e) {
    std::wcout << L"Unable to load default dictionary: " << e.what() << "\n";
  }
}

void alekseev::Exec::operator()(wstr_cr line)
{
  Vector< std::wstring > words = split(line, L' ', true);
  if (words.isEmpty()) {
    throw std::invalid_argument("Empty input!");
  }
  std::wstring func_name = words[0];
  if (!functions_.contains(func_name)) {
    os_ << L"Bad command name!\n";

    Vector< std::wstring > corrections;
    if (functions_.contains(func_name + L"_txt")) {
      corrections.pushBack(func_name + L"_txt");
    }
    if (functions_.contains(func_name + L"_dict")) {
      corrections.pushBack(func_name + L"_dict");
    }
    corrections += damerau_find(func_name, functions_.begin(), functions_.end());
    size_t ans = choose(corrections, is_, os_, 0, L"Perhaps you mean...");
    if (ans == corrections.getSize()) {
      return;
    } else {
      func_name = corrections.at(ans);
    }
  }
  Vector< std::wstring > args = words;
  args.erase(0);
  functions_.at(func_name)(args);
}

void alekseev::Exec::load_txt(Vector< std::wstring > & args)
{
  if (args.getSize() != 2) {
    throw std::invalid_argument("Bad arguments number! Using: load_txt <text_name> <path_to_file>");
  }
  os_ << L"Loading text \"" << args[0] << "\" from " << args[1] << L"\n";
  texts_.load(args[1], args[0]);
  os_ << L"\"" << args[0] << L"\" successfully loaded\n";
}

void alekseev::Exec::save_txt(Vector< std::wstring > & args)
{
  if (args.getSize() == 1) {
    os_ << L"\"" << texts_.save(args[0]) << L"\" successfully saved\n";
  } else if (args.getSize() == 2) {
    os_ << L"\"" << texts_.save(args[1], args[0]) << L"\" successfully saved\n";
  } else {
    throw std::invalid_argument("Bad arguments number! Using: save_txt [text_name] <path_to_file>");
  }
}

void alekseev::Exec::unload_txt(Vector< std::wstring > & args)
{
  if (args.getSize() == 1) {
    if (!texts_.is_saved(args[0])) {
      wchar_t need_save = ask_yes_no(L"Do you want to save text before unloading?", is_, os_);
      if (need_save == 'y') {
        os_ << "Enter file name for saving >";
        std::wstring file_name;
        std::getline(is_, file_name);
        os_ << L"Saving " << args[0] << L" to " << file_name << L"\n";
        texts_.save(file_name, args[0]);
        os_ << L"\"" << args[0] << L"\" successfully saved\n";
      }
    }
    texts_.unload(args[0]);
    os_ << L"\"" << args[0] << L"\" unloaded\n";
  } else {
    throw std::invalid_argument("Bad arguments number! Using: unload_txt <text_name>");
  }
}

void alekseev::Exec::parse(Vector< std::wstring > & args)
{
  if (args.isEmpty()) {
    os_ << L"Parsing...\n";
    os_ << L"\"" << texts_.parse() << L"\" successfully parsed\n";
  } else if (args.getSize() == 1) {
    os_ << L"Parsing...\n";
    os_ << L"\"" << texts_.parse(args[0]) << L"\" successfully parsed\n";
  } else {
    throw std::invalid_argument("Bad arguments number! Using: parse [text_name]");
  }
}

void alekseev::Exec::correct(Vector< std::wstring > & args)
{
  std::wstring text_name;
  if (args.isEmpty()) {
    text_name = texts_.correct(is_, os_);
  } else if (args.getSize() == 1) {
    text_name = texts_.correct(is_, os_, args[0]);
  } else {
    throw std::invalid_argument("Bad arguments number! Using: correct [text_name]");
  }
  os_ << L"\"" << text_name << L"\" successfully corrected\n";
}

void alekseev::Exec::process(Vector< std::wstring > & args)
{
  if (args.getSize() != 1) {
    throw std::invalid_argument("Bad arguments number! Using: process <path_to_file>");
  }
  std::wstring temp_name = L"__temporary_text_name_for_process__";
  while (texts_.contains(temp_name)) {
    temp_name += L"_";
  }
  texts_.load(args[0], temp_name);
  os_ << L"Text loaded\n"
      "Parsing...\n";
  try {
    texts_.parse(temp_name);
    os_ << L"Text parsed\n";
    texts_.correct(is_, os_, temp_name);
    os_ << L"Saving...\n";
    texts_.save(args[0], temp_name);
    os_ << L"Saved\n";
    texts_.unload(temp_name);
    os_ << L"Unloaded\n";
    os_ << L"\"" << args[0] << L"\" processed successfully!\n";
  } catch (...) {
    texts_.unload(temp_name);
  }
}

void alekseev::Exec::texts(Vector< std::wstring > &)
{
  os_ << L"Loaded texts:\n";
  std::wstring last_corrected = texts_.last_corrected();
  std::wstring last_loaded = texts_.last_loaded();
  std::wstring last_parsed = texts_.last_parsed();
  for (auto name = texts_.texts_begin(); name != texts_.texts_end(); ++name) {
    std::wstring tags;
    tags += *name == last_corrected ? L"c" : L"";
    tags += *name == last_loaded ? L"l" : L"";
    tags += *name == last_parsed ? L"p" : L"";
    os_ << std::left << std::setw(4) << tags << *name << L"\n";
  }
  os_ << L"(c - last corrected text; l - last loaded; p - last parsed)\n";
}

void alekseev::Exec::new_(Vector< std::wstring > & args)
{
  if (args.getSize() != 1) {
    throw std::invalid_argument("Bad arguments number! Using: new <dict_name>");
  }
  dicts_.create(args[0]);
  os_ << L"An empty dictionary \"" << args[0] << "\" has been created\n";
}

void alekseev::Exec::load_dict(Vector< std::wstring > & args)
{
  if (args.getSize() != 2 && args.getSize() != 3) {
    throw std::invalid_argument(
        "Bad arguments number! Using: load_dict <dict_name> <path_to_file> [lemmas_number]");
  }
  size_t n = 1024;
  if (args.getSize() == 3) {
    wchar_t * end_ptr = nullptr;
    n = wcstoull(args[3].c_str(), std::addressof(end_ptr), 10);
    if (*end_ptr != L'\0') {
      throw std::invalid_argument("Bad lemmas_number");
    }
  }
  os_ << "Loading...\n";
  dicts_.load(args[0], args[1], n);
  os_ << L"Successfully loaded dictionary \"" << args[0] << "\" with " << dicts_.current().size();
  os_ << " word forms from \"" << args[1] << "\"\n";
}

void alekseev::Exec::save_dict(Vector< std::wstring > & args)
{
  if (args.getSize() != 2) {
    throw std::invalid_argument(
        "Bad arguments number! Using: save_dict <dict_name> <path_to_file>");
  }
  os_ << "Saving...\n";
  dicts_.save(args[0], args[1]);
  os_ << L"Saved dictionary \"" << args[0] << "\" to \"" << args[1] << "\"\n";
}

void alekseev::Exec::unload_dict(Vector< std::wstring > & args)
{
  if (args.getSize() != 1) {
    throw std::invalid_argument("Bad arguments number! Using: <dict_name>");
  }
  dicts_.unload(args[0]);
  os_ << L"Unloaded dictionary \"" << args[0] << "\"\n";
}

void alekseev::Exec::current(Vector< std::wstring > & args)
{
  if (args.getSize() != 1) {
    throw std::invalid_argument("Bad arguments number! Using: current <name_of_loaded_dictionary>");
  }
  dicts_.set_current(args[0]);
}

void alekseev::Exec::add_word(Vector< std::wstring > & args)
{
  if (args.getSize() != 1) {
    throw std::invalid_argument("Bad arguments number! Using: add <lemma>");
  }
  dicts_.add_word(args[0], is_, os_);
}

void alekseev::Exec::update_form(Vector< std::wstring > & args)
{
  if (args.getSize() != 1) {
    throw std::invalid_argument("Bad arguments number! Using: update <lemma>");
  }
  dicts_.update_word(args[0], is_, os_);
}

void alekseev::Exec::delete_lemma(Vector< std::wstring > & args)
{
  if (args.getSize() != 1) {
    throw std::invalid_argument("Bad arguments number! Using: delete_lemma <lemma>");
  }
  dicts_.delete_lemma(args[0], is_, os_);
}

void alekseev::Exec::delete_form(Vector< std::wstring > & args)
{
  if (args.getSize() != 1) {
    throw std::invalid_argument("Bad arguments number! Using: delete_form <word_form>");
  }
  dicts_.delete_form(args[0], is_, os_);
}

void alekseev::Exec::dicts(Vector< std::wstring > &)
{
  std::wcout << L"Loaded dictionaries:\n";
  for (auto name = dicts_.dicts_begin(); name != dicts_.dicts_end(); ++name) {
    os_ << "  ";
    if (*name == dicts_.current_dict_name()) {
      os_ << L"* ";
    } else {
      os_ << L"  ";
    }
    os_ << *name << L"\n";
  }
  os_ << L"(* - current dictionary)\n";
}

void alekseev::Exec::max_variants_txt(Vector< std::wstring > & args)
{
  if (args.getSize() != 1) {
    throw std::invalid_argument("Bad arguments number! Using: max_variants_txt <number>");
  }
  wchar_t * end_ptr = nullptr;
  size_t n = wcstoull(args[0].c_str(), std::addressof(end_ptr), 10);
  if (*end_ptr != L'\0') {
    throw std::invalid_argument("Bad input");
  }
  texts_.set_max_variants(n);
}

void alekseev::Exec::distance_of_find_txt(Vector< std::wstring > & args)
{
  if (args.getSize() != 1) {
    throw std::invalid_argument("Bad arguments number! Using: distance_of_find_txt <number>");
  }
  wchar_t * end_ptr = nullptr;
  size_t n = wcstoull(args[0].c_str(), std::addressof(end_ptr), 10);
  if (*end_ptr != L'\0') {
    throw std::invalid_argument("Bad input");
  }
  texts_.set_default_distance(n);
}

void alekseev::Exec::max_variants_dict(Vector< std::wstring > & args)
{
  if (args.getSize() != 1) {
    throw std::invalid_argument("Bad arguments number! Using: max_variants_dict <number>");
  }
  wchar_t * end_ptr = nullptr;
  size_t n = wcstoull(args[0].c_str(), std::addressof(end_ptr), 10);
  if (*end_ptr != L'\0') {
    throw std::invalid_argument("Bad input");
  }
  dicts_.set_max_variants(n);
}

void alekseev::Exec::distance_of_find_dict(Vector< std::wstring > & args)
{
  if (args.getSize() != 1) {
    throw std::invalid_argument("Bad arguments number! Using: distance_of_find_dict <number>");
  }
  wchar_t * end_ptr = nullptr;
  size_t n = wcstoull(args[0].c_str(), std::addressof(end_ptr), 10);
  if (*end_ptr != L'\0') {
    throw std::invalid_argument("Bad input");
  }
  dicts_.set_default_distance(n);
}

void alekseev::Exec::help(Vector< std::wstring > &)
{
  os_ << L"A program for finding typos in the text, correcting them, "
      "and managing dictionaries for these tasks\n"
      "\nFunctions for texts:\n"
      "    1. load_txt <text_name> <path_to_file>\n"
      "        Reads text from a file, assigns it a name in the program\n"
      "    2. parse [text_name]\n"
      "        Searches for typos in the text, prepares for correction "
      "(may work slowly on large texts) (if the text name is not specified, "
      "it works with the last loaded \"load_txt\" command)\n"
      "    3. correct [text_name]\n"
      "        Interactive typo correction mode in the text (if the text name is not specified, "
      "it works with the last parsed \"parse\" command)\n"
      "    4. save_txt [text_name] <path_to_file>\n"
      "        Writes text to a file (if the text name is not specified, "
      "it works with the last corrected \"correct\" command )\n"
      "    5. unload_txt <text_name>\n"
      "        Removes text from the program\n"
      "    6. process <path_to_file>\n"
      "        consistently causes:\n"
      "            load_txt default_text_name path_to_file\n"
      "            parse default_text_name\n"
      "            correct default_text_name\n"
      "            save_txt default_text_name path_to_file\n"
      "            unload_txt default_text_name\n"
      "    7. texts\n"
      "        Shows names of loaded texts\n"

      "\nFunctions for dictionaries:\n"
      "    1. load_dict <dict_name> <path_to_file> [lemmas_number]\n"
      "        Reads dictionary from a file, assigns it a name in the program. "
      "you can set lemmas_number equal to twice the number of lemmas in the loaded dictionary "
      "to avoid rehashing during load (default 1024)\n"
      "    2. new <dict_name>\n"
      "        Creates an empty dictionary with the appropriate name\n"
      "    3. save_dict <dict_name> <path_to_file>\n"
      "        Saves dictionary to file\n"
      "    4. unload_dict <dict_name>\n"
      "        Removes dictionary from the program\n"
      "    5. current <name_of_loaded_dict>\n"
      "        Set dictionary as current\n"
      "    6. add <lemma>\n"
      "        Interactive addition of a new lemma to the current dictionary\n"
      "    7. update <lemma>\n"
      "        Changing or adding a new word form to a lemma from the current dictionary\n"
      "    8. delete_lemma <lemma>\n"
      "        Delete lemma from current dictionary\n"
      "    9. delete_form <word_form>\n"
      "        Choose and delete word form from current dictionary\n"
      "    10. dicts\n"
      "        Shows names of loaded dicts\n"

      "\nAdditional:\n"
      "    1. max_variants_txt <number>\n"
      "        Sets the maximum number of options when working with texts (default 7)\n"
      "    2. max_variants_dict <number>\n"
      "        Sets the maximum number of options when working with dictionaries (default 7)\n"
      "    3. distance_of_find_txt <number>\n"
      "        Sets the maximum Damerau-Levenshtein distance for fuzzy search "
      "when working with texts (default 1)\n"
      "    4. distance_of_find_dict <number>\n"
      "        Sets the maximum Damerau-Levenshtein distance for fuzzy search "
      "when working with dictionaries (default 1)\n"
      "    5. help\n"
      "        Shows this help\n"
      "    6. exit\n"
      "        Terminate program\n";
}
