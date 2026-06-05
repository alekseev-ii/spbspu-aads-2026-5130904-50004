#include "dictionary.h"

#include <fcntl.h>
#include "wstr_functions.h"

alekseev::WordForm::WordForm():
  gender_(nn_gender),
  number_(nn_number),
  case_(nn_case),
  tense_(nn_tense),
  person_(nn_person)
{ }

alekseev::WordForm::WordForm(std::wstring wordform, gender g, number n, alekseev::case_ c, tense t,
    person p):
  word_(std::move(wordform)),
  gender_(g),
  number_(n),
  case_(c),
  tense_(t),
  person_(p)
{ }

alekseev::WordForm::WordForm(std::wstring word, pos p, Vector< std::wstring > tags)
{
  WordForm wf;
  wf.word_ = tags[0];
  for (size_t i = 1; i < tags.getSize(); ++i) {
    WordForm pre = wf;
    if (p == noun || p == adj) {
      if (tags[i] == L"nom") {
        wf.case_ = nominative;
      } else if (tags[i] == L"gen") {
        wf.case_ = genitive;
      } else if (tags[i] == L"dat") {
        wf.case_ = dative;
      } else if (tags[i] == L"acc") {
        wf.case_ = accusative;
      } else if (tags[i] == L"ins") {
        wf.case_ = instrumental;
      } else if (tags[i] == L"pre") {
        wf.case_ = prepositional;
      }
    }
    if (p == verb) {
      if (tags[i] == L"pres") {
        wf.tense_ = present;
      } else if (tags[i] == L"past") {
        wf.tense_ = past;
      } else if (tags[i] == L"fut") {
        wf.tense_ = future;
      }
      if (tags[i] == L"1") {
        wf.person_ = first;
      } else if (tags[i] == L"2") {
        wf.person_ = second;
      } else if (tags[i] == L"3") {
        wf.person_ = third;
      }
    }
    if (tags[i] == L"sing") {
      wf.number_ = singular;
    } else if (tags[i] == L"pl") {
      wf.number_ = plural;
    }
    if (tags[i] == L"masc") {
      wf.gender_ = masculine;
    } else if (tags[i] == L"fem") {
      wf.gender_ = feminine;
    } else if (tags[i] == L"neut") {
      wf.gender_ = neuter;
    }
    if (pre == wf) {
      throw std::invalid_argument("Bad tag: " + std::string(tags[i].begin(), tags[i].end()));
    }
  }
}

bool alekseev::WordForm::operator==(const WordForm & rhs) const
{
  bool result = word_ == rhs.word_;
  result = result && gender_ == rhs.gender_ && number_ == rhs.number_;
  result = result && case_ == rhs.case_ && tense_ == rhs.tense_;
  return result && person_ == rhs.person_;
}

alekseev::Vector< std::wstring > alekseev::tags(const WordForm & wf)
{
  Vector< std::wstring > res(6, {});
  if (wf.gender_ == feminine) {
    res.pushBack(L"fem");
  } else if (wf.gender_ == masculine) {
    res.pushBack(L"masc");
  } else if (wf.gender_ == neuter) {
    res.pushBack(L"neut");
  }
  if (wf.number_ == singular) {
    res.pushBack(L"sing");
  } else if (wf.number_ == plural) {
    res.pushBack(L"pl");
  }
  if (wf.case_ == nominative) {
    res.pushBack(L"nom");
  } else if (wf.case_ == genitive) {
    res.pushBack(L"gen");
  } else if (wf.case_ == dative) {
    res.pushBack(L"dat");
  } else if (wf.case_ == accusative) {
    res.pushBack(L"acc");
  } else if (wf.case_ == instrumental) {
    res.pushBack(L"ins");
  } else if (wf.case_ == prepositional) {
    res.pushBack(L"pre");
  }
  if (wf.tense_ == past) {
    res.pushBack(L"past");
  } else if (wf.tense_ == present) {
    res.pushBack(L"pres");
  } else if (wf.tense_ == future) {
    res.pushBack(L"fut");
  }
  if (wf.person_ == first) {
    res.pushBack(L"1");
  } else if (wf.person_ == second) {
    res.pushBack(L"2");
  } else if (wf.person_ == third) {
    res.pushBack(L"3");
  }
  return res;
}

std::wostream & alekseev::operator<<(std::wostream & os, const WordForm & wf)
{
  Vector< std::wstring > t = tags(wf);
  os << wf.word_;
  for (size_t i = 0; i < t.getSize(); ++i) {
    os << " " << t[i];
  }
  return os;
}

alekseev::pos alekseev::guess_pos(std::wstring word)
{
  if (word.size() < 3) {
    return unknown;
  }
  word = lower_case(word);
  std::wstring v;
  if (endswith(word, {L"ся", L"сь"})) {
    v = word.substr(0, word.size() - 2);
  }
  if (endswith(v, {L"ть", L"ти", L"чь"})) {
    return verb;
  }
  if (endswith(word, {L"ый", L"ий", L"ой"})) {
    return adj;
  }
  return noun;
}

alekseev::ConsoleSetup::ConsoleSetup():
  old_cin_mode_(_setmode(_fileno(stdin), _O_U16TEXT)),
  old_cout_mode_(_setmode(_fileno(stdout), _O_U16TEXT)),
  old_cerr_mode_(_setmode(_fileno(stderr), _O_U16TEXT)),
  old_output_cp_(GetConsoleOutputCP()),
  old_input_cp_(GetConsoleCP())
{
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
}

alekseev::ConsoleSetup::~ConsoleSetup()
{
  _setmode(_fileno(stdout), old_cout_mode_);
  _setmode(_fileno(stdin), old_cin_mode_);
  _setmode(_fileno(stderr), old_cerr_mode_);
  SetConsoleOutputCP(old_output_cp_);
  SetConsoleCP(old_input_cp_);
}

alekseev::Dictionary::Dictionary():
  lemmas_(djb2_hash, poly_hash, equal, 4096),
  forms_(djb2_hash, poly_hash, equal, 16384)
{ }

alekseev::Dictionary::Dictionary(wstr_cr file_name):
  lemmas_(djb2_hash, poly_hash, equal, 4096),
  forms_(djb2_hash, poly_hash, equal, 16384)
{
  read(file_name);
}

void alekseev::Dictionary::read(wstr_cr file_name)
{
  std::ifstream is(file_name.data(), std::ios::binary);
  try {
    std::wcout << L"p1\n";
    read(is);
  } catch (...) {
    is.close();
    throw;
  }
  is.close();
}

std::ifstream & alekseev::Dictionary::read(std::ifstream & is)
{
  if (!is.is_open() || !is.good()) {
    return is;
  }

  std::wcout << L"p2\n";
  std::string line;
  Lemma lemma;
  while (std::getline(is, line)) {
    if (line.empty()) {
      continue;
    }
    if (static_cast< unsigned char >(line[0]) == 0xEF && static_cast< unsigned char >(line[1]) ==
      0xBB && static_cast< unsigned char >(line[2]) == 0xBF) {
      line = line.substr(3);
      if (line.empty()) {
        continue;
      }
    }
    if (line[0] == L'#') {
      continue;
    }

    std::wstring wline = utf8_to_wstring(line);
    Vector< std::wstring > words = split(wline, L' ', true);
    if (words.isEmpty()) {
      continue;
    }
    if (wline[0] != L' ' && wline[0] != L'\t') {
      if (!lemma.lemma_.empty()) {
        lemmas_.insert(lemma.lemma_, lemma);
        lemma = Lemma();
      }
      lemma.lemma_ = words[0];
      if (words[1] == L"noun") {
        if (words.getSize() != 3) {
          throw std::invalid_argument("Bad number of tags for noun");
        }
        lemma.pos_ = noun;
        lemma.verb_aspect_ = nn_aspect;
        if (words[2] == L"masc") {
          lemma.noun_gender_ = masculine;
        } else if (words[2] == L"fem") {
          lemma.noun_gender_ = feminine;
        } else if (words[2] == L"neut") {
          lemma.noun_gender_ = neuter;
        } else {
          throw std::invalid_argument("Invalid noun gender");
        }
      } else if (words[1] == L"verb") {
        if (words.getSize() != 3) {
          throw std::invalid_argument("Bad number of tags for verb");
        }
        lemma.pos_ = verb;
        lemma.noun_gender_ = nn_gender;
        if (words[2] == L"perf") {
          lemma.verb_aspect_ = perf;
        } else if (words[2] == L"imperf") {
          lemma.verb_aspect_ = imperf;
        } else {
          throw std::invalid_argument("Invalid verb aspect");
        }
      } else if (words[1] == L"adj") {
        if (words.getSize() != 2) {
          throw std::invalid_argument("Bad number of tags for adjective");
        }
        lemma.pos_ = adj;
        lemma.noun_gender_ = nn_gender;
        lemma.verb_aspect_ = nn_aspect;
      }
    } else {
      if (lemma.lemma_.empty()) {
        throw std::invalid_argument("Bad format (Word form before lemma)");
      }
      Vector< std::wstring > tags(words.getSize() - 1, {});
      tags.insert(0, words, 1, words.getSize());
      WordForm wf(words[0], lemma.pos_, tags);
      lemma.forms_.pushBack(wf);
      if (!forms_.contains(wf.word_)) {
        forms_.insert(wf.word_, Vector< std::pair< std::wstring, size_t > >());
      }
      forms_.at(wf.word_).pushBack(std::make_pair(lemma.lemma_, lemma.forms_.getSize() - 1));
    }
  }
  if (!lemma.lemma_.empty()) {
    lemmas_.insert(lemma.lemma_, lemma);
  }
  return is;
}

void alekseev::Dictionary::write(wstr_cr file_name)
{
  std::wofstream os(file_name.data());
  if (!os.is_open()) {
    throw std::invalid_argument("Failed to open file");
  }
  write(os);
  os.close();
}

std::wofstream & alekseev::Dictionary::write(std::wofstream & os)
{
  if (!os.is_open() || !os.good()) {
    return os;
  }
  Vector< std::wstring > keys = lemmas_.keys();
  for (size_t i = 0; i < keys.getSize(); ++i) {
    Lemma & lemma = lemmas_.at(keys[i]);
    os << lemma.lemma_ << L" ";
    if (lemma.pos_ == noun) {
      os << L"noun" << L" ";
      if (lemma.noun_gender_ == masculine) {
        os << L"masc";
      } else if (lemma.noun_gender_ == feminine) {
        os << L"fem";
      } else if (lemma.noun_gender_ == neuter) {
        os << L"neut";
      }
      os << L"\n";
    } else if (lemma.pos_ == adj) {
      os << L"adj" << L"\n";
    } else {
      os << L"verb" << L" ";
      if (lemma.verb_aspect_ == perf) {
        os << L"perf";
      } else {
        os << L"imperf";
      }
      os << L"\n";
    }
    for (size_t j = 0; j < lemma.forms_.getSize(); ++j) {
      const WordForm & wf = lemma.forms_[j];
      os << L"\t" << wf << L"\n";
    }
    os << L"\n";
  }
  return os;
}

void alekseev::Dictionary::add_lemma(const std::wstring & lemma, pos pos, gender noun_gender,
    aspect verb_aspect)
{
  Lemma l{lemma, Vector< WordForm >(), pos, noun_gender, verb_aspect};
  lemmas_.insert(lemma, l);
}

void alekseev::Dictionary::add_form(const std::wstring & lemma, const std::wstring & wordform,
    gender g, number n, case_ c, tense t, person p)
{
  WordForm wf(wordform, g, n, c, t, p);
  Lemma & l = lemmas_.at(lemma);
  l.forms_.pushBack(wf);
  if (!forms_.contains(wf.word_)) {
    forms_.insert(wf.word_, Vector< std::pair< std::wstring, size_t > >());
  }
  forms_.at(wf.word_).pushBack(std::make_pair(l.lemma_, l.forms_.getSize() - 1));
}

void alekseev::Dictionary::remove_lemma(const std::wstring & lemma)
{
  if (!lemmas_.contains(lemma)) {
    return;
  }
  Lemma l = lemmas_.at(lemma);
  lemmas_.remove(lemma);
  for (size_t i = 0; i < l.forms_.getSize(); ++i) {
    Vector< std::pair< std::wstring, size_t > > & wfs = find_homoforms(l.forms_[i].word_);
    for (size_t j = 0; j < wfs.getSize(); ++j) {
      if (wfs[j].first == lemma) {
        wfs.erase(j);
      }
    }
    if (wfs.isEmpty()) {
      forms_.remove(l.forms_[i].word_);
    }
  }
}

void alekseev::Dictionary::remove_form(const WordForm & wordform)
{
  if (!forms_.contains(wordform.word_)) {
    return;
  }
  Vector< std::pair< std::wstring, size_t > > & wfs = find_homoforms(wordform.word_);
  for (size_t i = 0; i < wfs.getSize(); ++i) {
    Lemma & l = lemmas_.at(wfs[i].first);
    if (l.forms_[wfs[i].second] == wordform) {
      l.forms_.erase(wfs[i].second);
      wfs.erase(i);
      if (wfs.isEmpty()) {
        forms_.remove(wordform.word_);
      }
      for (size_t j = wfs[i].second; j < l.forms_.getSize(); ++j) {
        lemma_pair_by_wordform(l.forms_[j]).second--;
      }
      return;
    }
  }
}

bool alekseev::Dictionary::contains_lemma(const std::wstring & lemma) const
{
  return lemmas_.contains(lemma);
}

bool alekseev::Dictionary::contains_form(const std::wstring & wordform) const
{
  return forms_.contains(wordform);
}

bool alekseev::Dictionary::contains_form(const WordForm & wordform) const
{
  if (!forms_.contains(wordform.word_)) {
    return false;
  }
  Vector< std::pair< std::wstring, size_t > > wfs = forms_.at(wordform.word_);
  for (size_t i = 0; i < wfs.getSize(); ++i) {
    const Lemma & l = lemmas_.at(wfs[i].first);
    if (l.forms_[wfs[i].second] == wordform) {
      return true;
    }
  }
  return false;
}

alekseev::Vector< std::pair< std::wstring, size_t > > & alekseev::Dictionary::find_homoforms(
    const std::wstring & wordform)
{
  return forms_.at(wordform);
}

const alekseev::Vector< std::pair< std::wstring, size_t > > & alekseev::Dictionary::find_homoforms(
    wstr_cr wordform) const
{
  return forms_.at(wordform);
}

std::pair< std::wstring, size_t > & alekseev::Dictionary::lemma_pair_by_wordform(
    const WordForm & wordform)
{
  Vector< std::pair< std::wstring, size_t > > & wfs = find_homoforms(wordform.word_);
  for (size_t i = 0; i < wfs.getSize(); ++i) {
    Lemma & l = lemmas_.at(wfs[i].first);
    if (l.forms_[wfs[i].second] == wordform) {
      return wfs[i];
    }
  }
  throw std::out_of_range("Wordform not found");
}

const std::pair< std::wstring, size_t > & alekseev::Dictionary::lemma_pair_by_wordform(
    const WordForm & wordform) const
{
  const Vector< std::pair< std::wstring, size_t > > & wfs = find_homoforms(wordform.word_);
  for (size_t i = 0; i < wfs.getSize(); ++i) {
    const Lemma & l = lemmas_.at(wfs[i].first);
    if (l.forms_[wfs[i].second] == wordform) {
      return wfs[i];
    }
  }
  throw std::out_of_range("Wordform not found");
}

alekseev::Vector< alekseev::WordForm > alekseev::Dictionary::get_homoforms(wstr_cr wordform) const
{
  Vector< WordForm > res;
  const Vector< std::pair< std::wstring, size_t > > & wfs = forms_.at(wordform);
  for (size_t i = 0; i < wfs.getSize(); ++i) {
    res.pushBack(lemmas_.at(wfs[i].first).forms_[wfs[i].second]);
  }
  return res;
}

alekseev::Vector< std::wstring > alekseev::Dictionary::get_lemmas() const
{
  return lemmas_.keys();
}

alekseev::Vector< alekseev::WordForm > & alekseev::Dictionary::forms_by_lemma(wstr_cr lemma)
{
  return lemmas_.at(lemma).forms_;
}

alekseev::pos alekseev::Dictionary::pos_of_lemma(wstr_cr lemma) const
{
  return lemmas_.at(lemma).pos_;
}

bool alekseev::Dictionary::matches_case(wstr_cr wordform, case_ expected_case) const
{
  if (!forms_.contains(wordform)) {
    return false;
  }
  Vector< WordForm > wfs = get_homoforms(wordform);
  for (size_t i = 0; i < wfs.getSize(); ++i) {
    if (wfs[i].case_ == expected_case) {
      return true;
    }
  }
  return false;
}

bool alekseev::Dictionary::matches_person(wstr_cr wordform, person expected_person) const
{
  if (!forms_.contains(wordform)) {
    return false;
  }
  Vector< WordForm > wfs = get_homoforms(wordform);
  for (size_t i = 0; i < wfs.getSize(); ++i) {
    if (wfs[i].person_ == expected_person) {
      return true;
    }
  }
  return false;
}

size_t alekseev::Dictionary::size() const
{
  return forms_.size();
}

alekseev::Vector< alekseev::WordForm > alekseev::Dictionary::damerau_find_wfs(wstr_cr bad_word,
    size_t distance) const
{
  if (forms_.contains(bad_word)) {
    return get_homoforms(bad_word);
  }
  Vector< WordForm > res;
  Vector< std::wstring > wfs = forms_.keys();
  for (size_t i = 0; i < wfs.getSize(); ++i) {
    if (wfs[i].size() - bad_word.size() <= distance) {
      if (damerau_levenshtein(wfs[i], bad_word) <= distance) {
        Vector< WordForm > found = get_homoforms(wfs[i]);
        res += found;
      }
    }
  }
  return res;
}

alekseev::Vector< std::wstring > alekseev::Dictionary::damerau_find_form(wstr_cr bad_form,
    size_t distance) const
{
  if (forms_.contains(bad_form)) {
    return Vector< std::wstring >(1, bad_form);
  }
  return damerau_find(bad_form, forms_.keys(), distance);
}

alekseev::Vector< std::wstring > alekseev::Dictionary::damerau_find_lemma(wstr_cr bad_lemma,
    size_t distance) const
{
  if (lemmas_.contains(bad_lemma)) {
    return Vector< std::wstring >(1, bad_lemma);
  }
  return damerau_find(bad_lemma, lemmas_.keys(), distance);
}

alekseev::DictionaryManager::DictionaryManager():
  dicts_(djb2_hash, poly_hash, equal, 16)
{ }

void alekseev::DictionaryManager::create(wstr_cr name)
{
  if (dicts_.contains(name)) {
    throw std::invalid_argument("Dictionary already exists");
  }
  dicts_.insert(name, Dictionary());
  current_ = name;
}

void alekseev::DictionaryManager::load(wstr_cr name, wstr_cr file_name)
{
  if (dicts_.contains(name)) {
    throw std::invalid_argument("Dictionary already exists");
  }
  Dictionary d(file_name);
  dicts_.insert(name, Dictionary(d));
  current_ = name;
}

void alekseev::DictionaryManager::save(wstr_cr name, wstr_cr file_name)
{
  dicts_.at(name).write(file_name);
}

void alekseev::DictionaryManager::unload(wstr_cr name)
{
  dicts_.remove(name);
}

void alekseev::DictionaryManager::set_current(wstr_cr name_of_loaded_dict)
{
  if (!dicts_.contains(name_of_loaded_dict)) {
    throw std::invalid_argument("Dictionary not found");
  }
  current_ = name_of_loaded_dict;
}

void alekseev::DictionaryManager::add_word(wstr_cr word, std::wistream & is, std::wostream & os)
{
  Dictionary & dict = current();
  std::wstring w = lower_case(word);
  if (dict.contains_lemma(w)) {
    os << L"Word already exists in current dictionary \"" << current_ << L"\"\n";
    wchar_t ans = ask_yes_no(L"Do you want to replace it?", is, os, true);
    if (ans == L'y') {
      dict.remove_lemma(w);
    } else if (ans == L'n') {
      return;
    } else {
      throw std::invalid_argument("Bad input");
    }
  }
  pos p = guess_pos(w);
  wchar_t ans;
  if (p == verb) {
    ans = ask_yes_no(L"Is it a verb?", is, os);
  } else if (p == adj) {
    ans = ask_yes_no(L"Is it an adjective?", is, os);
  } else if (p == noun) {
    ans = ask_yes_no(L"Is it a noun?", is, os);
  } else {
    ans = L'u';
  }
  if (ans != L'y') {
    std::wstring answer;
    os << "Input word class (verb/adj/noun): ";
    p = unknown;
    while (p == unknown && std::getline(is, answer)) {
      if (answer == L"verb") {
        p = verb;
      } else if (answer == L"adj") {
        p = adj;
      } else if (answer == L"noun") {
        p = noun;
      } else {
        os << L"Unknown word class: " << answer << L"\n";
        os << L"Input some of \"verb\", \"adj\", \"noun\"";
        os << "Input word class (verb/adj/noun): ";
      }
    }
    if (p == verb) {
      add_verb(w, is, os);
    } else if (p == adj) {
      add_adj(w, is, os);
    } else {
      add_noun(w, is, os);
    }
  }
}

void alekseev::DictionaryManager::update_word(wstr_cr word, std::wistream & is, std::wostream & os)
{
  Dictionary & dict = current();
  std::pair< std::wstring, size_t > lp = choose_wordform(word, is, os);
  if (!lp.first.empty()) {
    WordForm & wf = dict.forms_by_lemma(lp.first)[lp.second];
    os << L"Enter new tags (enter all tags that you think this form should have):\n";
    std::wstring tags_string;
    std::getline(is, tags_string);
    Vector< std::wstring > tags = split(tags_string, L' ', true);
    WordForm nwf(wf.word_, dict.pos_of_lemma(lp.first), tags);
    wf = nwf;
    return;
  }

  wchar_t nf = ask_yes_no(L"Do you want to add a new form?", is, os);
  if (nf != L'y') {
    return;
  }
  std::wstring lemma;
  os << L"Enter the lemma for which you want to add a new form: ";
  std::getline(is, lemma);
  if (!dict.contains_lemma(lemma)) {
    throw std::invalid_argument("Lemma does not exist");
  }
  os << L"Enter tags for the \"" << word << "\": ";
  std::wstring tags_string;
  std::getline(is, tags_string);
  WordForm wf = WordForm(word, dict.pos_of_lemma(lemma), split(tags_string, L' ', true));
  dict.add_form(lemma, word, wf.gender_, wf.number_, wf.case_, wf.tense_, wf.person_);
}

void alekseev::DictionaryManager::delete_form(wstr_cr wordform, std::wistream & is,
    std::wostream & os)
{
  Dictionary & dict = current();
  std::pair< std::wstring, size_t > lp = choose_wordform(wordform, is, os);

  if (!lp.first.empty()) {
    const WordForm & word = dict.forms_by_lemma(lp.first)[lp.second];
    dict.remove_form(word);
  }
}

void alekseev::DictionaryManager::delete_lemma(wstr_cr lemma, std::wistream & is,
    std::wostream & os)
{
  Dictionary & dict = current();
  if (dict.contains_lemma(lemma)) {
    dict.remove_lemma(lemma);
  } else {
    os << L"Lemma \"" << lemma << "\" does not exist\n";
  }
}

bool alekseev::DictionaryManager::contains_form(wstr_cr wordform) const
{
  Vector< std::wstring > names = dicts_.keys();
  bool result = false;
  for (size_t i = 0; i < names.getSize() && !result; ++i) {
    result = dicts_.at(names[i]).contains_form(wordform);
  }
  return result;
}

alekseev::Vector< std::wstring > alekseev::DictionaryManager::damerau_find_form(wstr_cr wordform,
    size_t distance) const
{
  Vector< std::wstring > names = dicts_.keys();
  Vector< std::wstring > result{};
  for (size_t i = 0; i < names.getSize(); ++i) {
    result += dicts_.at(names[i]).damerau_find_form(wordform, distance);
  }
  return result;
}

alekseev::Vector< std::wstring > alekseev::DictionaryManager::damerau_find_lemma(wstr_cr wordform,
    size_t distance) const
{
  Vector< std::wstring > names = dicts_.keys();
  Vector< std::wstring > result{};
  for (size_t i = 0; i < names.getSize(); ++i) {
    result += dicts_.at(names[i]).damerau_find_lemma(wordform, distance);
  }
  return result;
}

bool alekseev::DictionaryManager::matches_case(wstr_cr wordform, case_ expected_case) const
{
  Vector< std::wstring > names = dicts_.keys();
  bool result = false;
  for (size_t i = 0; i < names.getSize() && !result; ++i) {
    result = dicts_.at(names[i]).matches_case(wordform, expected_case);
  }
  return result;
}

bool alekseev::DictionaryManager::matches_person(wstr_cr wordform, person expected_person) const
{
  Vector< std::wstring > names = dicts_.keys();
  bool result = false;
  for (size_t i = 0; i < names.getSize() && !result; ++i) {
    result = dicts_.at(names[i]).matches_person(wordform, expected_person);
  }
  return result;
}

alekseev::Dictionary & alekseev::DictionaryManager::current()
{
  return dicts_.at(current_);
}

const alekseev::Dictionary & alekseev::DictionaryManager::current() const
{
  return dicts_.at(current_);
}

void alekseev::DictionaryManager::add_verb(wstr_cr word, std::wistream & is, std::wostream & os)
{
  Dictionary & dict = current();
  aspect verb_aspect = nn_aspect;
  wchar_t aspect = ask_yes_no(L"Is this verb is perfect?", is, os);
  if (aspect == L'y') {
    verb_aspect = perf;
  } else {
    verb_aspect = imperf;
  }
  dict.add_lemma(word, verb, nn_gender, verb_aspect);
  std::wstring past_masc, past_fem, past_neut, past_pl;
  os << L"Enter forms, leave the non-existing ones empty\n";
  os << "Past tense:\n";
  os << L"\tSingular masculine (he): ";
  std::getline(is, past_masc);
  os << L"\tSingular feminine (she): ";
  std::getline(is, past_fem);
  os << L"\tSingular neut (it): ";
  std::getline(is, past_neut);
  os << L"\tPlural (they): ";
  std::getline(is, past_pl);

  os << L"Present/Future tense:\n";
  std::wstring pres_1s, pres_2s, pres_3s, pres_1p, pres_2p, pres_3p;
  os << L"\t1st singular (I): ";
  std::getline(is, pres_1s);
  os << L"\t2nd singular (you): ";
  std::getline(is, pres_2s);
  os << L"\t3rd singular (he/she): ";
  std::getline(is, pres_3s);
  os << L"\t1st plural (we): ";
  std::getline(is, pres_1p);
  os << L"\t2nd plural (you): ";
  std::getline(is, pres_2p);
  os << L"\t3rd plural (they): ";
  std::getline(is, pres_3p);

  size_t c = 0;
  if (!past_masc.empty()) {
    dict.add_form(word, past_masc, masculine, singular, nn_case, past, nn_person);
    ++c;
  }
  if (!past_fem.empty()) {
    dict.add_form(word, past_fem, feminine, singular, nn_case, past, nn_person);
    ++c;
  }
  if (!past_neut.empty()) {
    dict.add_form(word, past_neut, neuter, singular, nn_case, past, nn_person);
    ++c;
  }
  if (!past_pl.empty()) {
    dict.add_form(word, past_pl, nn_gender, plural, nn_case, past, nn_person);
    ++c;
  }

  tense t = verb_aspect == perf ? future : present;
  if (!pres_1s.empty()) {
    dict.add_form(word, pres_1s, nn_gender, singular, nn_case, t, first);
    ++c;
  }
  if (!pres_2s.empty()) {
    dict.add_form(word, pres_2s, nn_gender, singular, nn_case, t, second);
    ++c;
  }
  if (!pres_3s.empty()) {
    dict.add_form(word, pres_3s, nn_gender, singular, nn_case, t, third);
    ++c;
  }
  if (!pres_1p.empty()) {
    dict.add_form(word, pres_1p, nn_gender, plural, nn_case, t, first);
    ++c;
  }
  if (!pres_2p.empty()) {
    dict.add_form(word, pres_2p, nn_gender, plural, nn_case, t, second);
    ++c;
  }
  if (!pres_3p.empty()) {
    dict.add_form(word, pres_3p, nn_gender, plural, nn_case, t, third);
    ++c;
  }
  os << L"Successfully added " << c << " forms!\n";
}

void alekseev::DictionaryManager::add_adj(wstr_cr word, std::wistream & is, std::wostream & os)
{
  Dictionary & dict = dicts_.at(word);
  dict.add_lemma(word, adj, nn_gender, nn_aspect);
  case_ cases[6]{nominative, genitive, dative, accusative, instrumental, prepositional};
  const wchar_t * cases_names[6]{
    L"Nominative",
    L"Genitive",
    L"Dative",
    L"Accusative",
    L"Instrumental",
    L"Prepositional"
  };
  gender genders[3]{masculine, feminine, neuter};
  const wchar_t * genders_names[3]{L"Masculine", L"Feminine", L"Neuter"};

  size_t c = 0;
  std::wstring form;
  os << L"Enter forms, leave the non-existing ones empty\n";
  for (size_t i = 0; i < 3; i++) {
    os << "Singular " << genders_names[i] << ":\n";
    for (size_t j = 0; j < 6; j++) {
      os << "\t" << cases_names[j] << ": ";
      getline(is, form);
      if (!form.empty()) {
        dict.add_form(word, form, genders[i], singular, cases[j], nn_tense, nn_person);
        ++c;
      }
    }
  }

  os << L"Plural:\n";
  for (size_t i = 0; i < 6; i++) {
    os << "\t" << cases_names[i] << ": ";
    getline(is, form);
    if (!form.empty()) {
      dict.add_form(word, form, nn_gender, plural, cases[i], nn_tense, nn_person);
      ++c;
    }
  }
  os << L"Successfully added " << c << " forms!\n";
}

void alekseev::DictionaryManager::add_noun(wstr_cr word, std::wistream & is, std::wostream & os)
{
  Dictionary dict = dicts_.at(word);
  os << L"Enter noun gender (masc/fem/neut): ";
  std::wstring gender_ans;
  gender g = nn_gender;
  while (getline(is, gender_ans) && g == nn_gender) {
    if (gender_ans == L"masc") {
      g = masculine;
    } else if (gender_ans == L"fem") {
      g = feminine;
    } else if (gender_ans == L"neut") {
      g = neuter;
    } else {
      os << L"Bad noun gender: " << gender_ans << L"\n";
      os << L"Enter noun gender (masc/fem/neut): ";
    }
  }
  dict.add_lemma(word, noun, g, nn_aspect);
  case_ cases[6]{nominative, genitive, dative, accusative, instrumental, prepositional};
  const wchar_t * cases_names[6]{
    L"Nominative",
    L"Genitive",
    L"Dative",
    L"Accusative",
    L"Instrumental",
    L"Prepositional"
  };
  number numbers[2]{singular, plural};
  const wchar_t * numbers_names[2]{L"Singular", L"Plural"};

  os << L"Enter forms, leave the non-existing ones empty\n";
  size_t c = 0;
  std::wstring form;
  for (size_t i = 0; i < 2; i++) {
    os << numbers_names[i] << ":\n";
    for (size_t j = 0; j < 6; j++) {
      os << "\t" << cases_names[j] << ": ";
      getline(is, form);
      if (!form.empty()) {
        dict.add_form(word, form, g, numbers[i], cases[j], nn_tense, nn_person);
        ++c;
      }
    }
  }
  os << L"Successfully added " << c << " forms!\n";
}

std::pair< std::wstring, size_t > alekseev::DictionaryManager::choose_wordform(wstr_cr word,
    std::wistream & is, std::wostream & os) const
{
  const Dictionary & dict = current();
  Vector< WordForm > wfs;

  if (dict.contains_form(word)) {
    wfs = dict.get_homoforms(word);
  } else {
    os << L"Form " << word << " not found\n";
    wchar_t need_find = ask_yes_no(L"Do you want to search using fuzzy search?", is, os);
    if (need_find == L'y') {
      wfs = dict.damerau_find_wfs(word, 1);
      if (wfs.isEmpty()) {
        os << L"No word found\n";
        return {{}, 0};
      }
    }
  }
  if (!wfs.isEmpty()) {
    size_t ind = choose(wfs, is, os, 0, L"Found form:", L"forms were found",
        L"Which one do you want to change?");
    return dict.lemma_pair_by_wordform(wfs[ind]);
  }
  return {{}, 0};
}
