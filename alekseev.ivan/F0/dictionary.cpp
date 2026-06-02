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

bool alekseev::WordForm::operator==(const WordForm & rhs) const
{
  bool result = word_ == rhs.word_;
  result = result && gender_ == rhs.gender_ && number_ == rhs.number_;
  result = result && case_ == rhs.case_ && tense_ == rhs.tense_;
  return result && person_ == rhs.person_;
}

size_t alekseev::djb2_hash(wstr_cr line)
{
  size_t hash = 5381;
  for (size_t i = 0; i < line.size(); ++i) {
    hash = ((hash << 5) + hash) + line[i];
  }
  return hash;
}

size_t alekseev::poly_hash(wstr_cr line)
{
  size_t hash = 0;
  int p = 67;
  for (size_t i = 0; i < line.size(); ++i) {
    hash = hash * p + line[i];
  }
  return hash;
}

bool alekseev::equal(wstr_cr s1, wstr_cr s2)
{
  return s1 == s2;
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
      WordForm wf;
      wf.word_ = words[0];
      for (size_t i = 1; i < words.getSize(); ++i) {
        WordForm pre = wf;
        if (lemma.pos_ == noun || lemma.pos_ == adj) {
          if (words[i] == L"nom") {
            wf.case_ = nominative;
          } else if (words[i] == L"gen") {
            wf.case_ = genitive;
          } else if (words[i] == L"dat") {
            wf.case_ = dative;
          } else if (words[i] == L"acc") {
            wf.case_ = accusative;
          } else if (words[i] == L"ins") {
            wf.case_ = instrumental;
          } else if (words[i] == L"pre") {
            wf.case_ = prepositional;
          }
        }
        if (lemma.pos_ == verb) {
          if (words[i] == L"pres") {
            wf.tense_ = present;
          } else if (words[i] == L"past") {
            wf.tense_ = past;
          } else if (words[i] == L"fut") {
            wf.tense_ = future;
          }
          if (words[i] == L"1") {
            wf.person_ = first;
          } else if (words[i] == L"2") {
            wf.person_ = second;
          } else if (words[i] == L"3") {
            wf.person_ = third;
          }
        }
        if (words[i] == L"sing") {
          wf.number_ = singular;
        } else if (words[i] == L"pl") {
          wf.number_ = plural;
        }
        if (words[i] == L"masc") {
          wf.gender_ = masculine;
        } else if (words[i] == L"fem") {
          wf.gender_ = feminine;
        } else if (words[i] == L"neut") {
          wf.gender_ = neuter;
        }
        if (pre == wf) {
          throw std::invalid_argument("Bad tag: " + std::string(words[i].begin(), words[i].end()));
        }
      }
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
      WordForm wf = lemma.forms_[j];
      os << L"\t" << wf.word_ << L" ";
      if (wf.gender_ == masculine) {
        os << L"masc" << L" ";
      } else if (wf.gender_ == feminine) {
        os << L"fem" << L" ";
      } else if (wf.gender_ == neuter) {
        os << L"neut" << L" ";
      }
      if (wf.number_ == singular) {
        os << L"sing" << L" ";
      } else if (wf.number_ == plural) {
        os << L"pl" << L" ";
      }
      if (wf.case_ == nominative) {
        os << L"nom" << L" ";
      } else if (wf.case_ == genitive) {
        os << L"gen" << L" ";
      } else if (wf.case_ == dative) {
        os << L"dat" << L" ";
      } else if (wf.case_ == accusative) {
        os << L"acc" << L" ";
      } else if (wf.case_ == instrumental) {
        os << L"ins" << L" ";
      } else if (wf.case_ == prepositional) {
        os << L"pre" << L" ";
      }
      if (wf.tense_ == past) {
        os << L"past" << L" ";
      } else if (wf.tense_ == future) {
        os << L"fut" << L" ";
      } else if (wf.tense_ == present) {
        os << L"pres" << L" ";
      }
      if (wf.person_ == first) {
        os << L"1" << L" ";
      } else if (wf.person_ == second) {
        os << L"2" << L" ";
      } else if (wf.person_ == third) {
        os << L"3" << L" ";
      }
      os << L"\n";
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

const alekseev::Vector< alekseev::WordForm > & alekseev::Dictionary::forms_by_lemma(
    wstr_cr lemma) const
{
  return lemmas_.at(lemma).forms_;
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

alekseev::Vector< alekseev::WordForm > alekseev::Dictionary::damerau_find(wstr_cr bad_word,
    size_t distance)
{
  if (forms_.contains(bad_word)) {
    return get_homoforms(bad_word);
  }
  Vector< WordForm > res;
  Vector< std::wstring > wfs = forms_.keys();
  for (size_t i = 0; i < wfs.getSize(); ++i) {
    if (wfs[i].size() - bad_word.size() < distance) {
      if (damerau_levenshtein(wfs[i], bad_word) < distance) {
        Vector< WordForm > found = get_homoforms(wfs[i]);
        res += found;
      }
    }
  }
  return res;
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

void alekseev::DictionaryManager::add_word(std::wstring word, std::wistream & is,
    std::wostream & os)
{
  Dictionary & dict = dicts_.at(current_);
  if (dict.contains_lemma(word)) {
    os << L"Word already exists in current dictionary \"" << current_ << L"\"\n";
    wchar_t ans = ask_yes_no(L"Do you want to replace it?", is, os, true);
    if (ans == L'y') {
      dict.remove_lemma(word);
    } else if (ans == L'n') {
      return;
    } else {
      throw std::invalid_argument("Bad input");
    }
  }
  pos p = guess_pos(word);
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
    os << "Input word class: ";
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
        os << "Input word class: ";
      }
    }
  }
}
