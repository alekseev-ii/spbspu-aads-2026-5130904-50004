#include "dictionary.h"

#include <fcntl.h>
#include "wstr_functions.h"
#include <sstream>

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

alekseev::WordForm::WordForm(const Vector< std::wstring > & tags, pos p):
  WordForm()
{
  *this = from_tags(tags, p);
}

alekseev::WordForm::WordForm(wstr_cr wordform, pos p, const Vector< std::wstring > & tags):
  WordForm()
{
  Vector< std::wstring > word(1, wordform);
  *this = from_tags(word + tags, p);
}

bool alekseev::WordForm::operator==(const WordForm & rhs) const
{
  bool result = word_ == rhs.word_;
  result = result && gender_ == rhs.gender_ && number_ == rhs.number_;
  result = result && case_ == rhs.case_ && tense_ == rhs.tense_;
  return result && person_ == rhs.person_;
}

bool alekseev::matches(const WordForm & lhs, const WordForm & rhs)
{
  bool res = lhs.word_.empty() || rhs.word_.empty() || lhs.word_ == rhs.word_;
  res = res && (lhs.gender_ == nn_gender || rhs.gender_ == nn_gender || lhs.gender_ == rhs.gender_);
  res = res && (lhs.number_ == nn_number || rhs.number_ == nn_number || lhs.number_ == rhs.number_);
  res = res && (lhs.case_ == nn_case || rhs.case_ == nn_case || lhs.case_ == rhs.case_);
  res = res && (lhs.tense_ == nn_tense || rhs.tense_ == nn_tense || lhs.tense_ == rhs.tense_);
  res = res && (lhs.person_ == nn_person || rhs.person_ == nn_person || lhs.person_ == rhs.person_);
  return res;
}

alekseev::Vector< std::wstring > alekseev::to_tags(const WordForm & wf)
{
  Vector< std::wstring > res;
  res.resize(6);
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

alekseev::WordForm alekseev::from_tags(const Vector< std::wstring > & tags, pos p)
{
  WordForm wf;
  wf.word_ = tags[0];
  for (size_t i = 1; i < tags.getSize(); ++i) {
    WordForm pre = wf;
    if (p == noun || p == adj || p == require) {
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
    if (p == verb || p == require) {
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
  return wf;
}

alekseev::Vector< std::wstring > alekseev::to_words(const Vector< WordForm > & wfs)
{
  Vector< std::wstring > res(wfs.getSize(), L"");
  for (size_t i = 0; i < wfs.getSize(); ++i) {
    res[i] = wfs[i].word_;
  }
  return res;
}

std::wstring alekseev::to_wstring(const WordForm & wf)
{
  std::wstring res(wf.word_);
  Vector< std::wstring > t = to_tags(wf);
  for (size_t i = 0; i < t.getSize(); ++i) {
    res += L" " + t[i];
  }
  return res;
}

std::wostream & alekseev::operator<<(std::wostream & os, const WordForm & wf)
{
  Vector< std::wstring > t = to_tags(wf);
  os << wf.word_;
  for (size_t i = 0; i < t.getSize(); ++i) {
    os << L" " << t[i];
  }
  return os;
}

alekseev::Lemma::Lemma():
  pos_(unknown),
  noun_gender_(nn_gender),
  verb_aspect_(nn_aspect)
{ }

std::wstring alekseev::to_wstring(const Lemma & lemma)
{
  std::wstring res;
  res += lemma.lemma_ + L" ";
  if (lemma.pos_ == noun) {
    res += L"noun ";
    if (lemma.noun_gender_ == masculine) {
      res += L"masc";
    } else if (lemma.noun_gender_ == feminine) {
      res += L"fem";
    } else if (lemma.noun_gender_ == neuter) {
      res += L"neut";
    }
    res += L"\n";
  } else if (lemma.pos_ == verb) {
    res += L"verb ";
    if (lemma.verb_aspect_ == perf) {
      res += L"perf";
    } else {
      res += L"imperf";
    }
    res += L"\n";
  } else if (lemma.pos_ == adj) {
    res += L"adj\n";
  } else if (lemma.pos_ == require) {
    res += L"req\n";
  }
  for (size_t j = 0; j < lemma.forms_.getSize(); ++j) {
    const WordForm & wf = lemma.forms_[j];
    res += L"\t" + to_wstring(wf) + L"\n";
  }
  return res;
}

std::wostream & alekseev::operator<<(std::wostream & os, const Lemma & lemma)
{
  os << to_wstring(lemma);
  return os;
}

alekseev::pos alekseev::guess_pos(std::wstring word)
{
  if (word.size() < 3) {
    return require;
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
  forms_(djb2_hash, poly_hash, equal, 16384),
  requires_(djb2_hash, poly_hash, equal, 4096)
{ }

alekseev::Dictionary::Dictionary(wstr_cr file_name):
  lemmas_(djb2_hash, poly_hash, equal, 4096),
  forms_(djb2_hash, poly_hash, equal, 16384),
  requires_(djb2_hash, poly_hash, equal, 4096)
{
  read(file_name);
}

void alekseev::Dictionary::read(wstr_cr file_name)
{
  std::ifstream is(file_name.data(), std::ios::binary);
  if (!is.is_open()) {
    throw std::invalid_argument("Failed to open file");
  }
  try {
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

  std::string line;
  Lemma lemma;
  while (std::getline(is, line)) {
    if (line.empty()) {
      continue;
    }
    if (line.size() > 2) {
      if (static_cast< unsigned char >(line[0]) == 0xEF && static_cast< unsigned char >(line[1]) ==
        0xBB && static_cast< unsigned char >(line[2]) == 0xBF) {
        line = line.substr(3);
        if (line.empty()) {
          continue;
        }
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
        if (lemma.pos_ == require) {
          requires_.insert(lemma.lemma_, lemma);
        } else {
          lemmas_.insert(lemma.lemma_, lemma);
        }
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
      } else if (words[1] == L"req") {
        if (words.getSize() != 2) {
          throw std::invalid_argument("Bad number of tags for require");
        }
        lemma.pos_ = require;
        lemma.noun_gender_ = nn_gender;
        lemma.verb_aspect_ = nn_aspect;
      }
    } else {
      if (lemma.lemma_.empty()) {
        throw std::invalid_argument("Bad format (Word form before lemma)");
      }
      WordForm wf(words, lemma.pos_);
      lemma.forms_.pushBack(wf);
      if (lemma.pos_ != require) {
        if (!forms_.contains(wf.word_)) {
          forms_.insert(wf.word_, Vector< std::pair< std::wstring, size_t > >());
        }
        forms_.at(wf.word_).pushBack(std::make_pair(lemma.lemma_, lemma.forms_.getSize() - 1));
      }
    }
  }
  if (!lemma.lemma_.empty()) {
    if (lemma.pos_ == require) {
      requires_.insert(lemma.lemma_, lemma);
    } else {
      lemmas_.insert(lemma.lemma_, lemma);
    }
  }
  return is;
}

void alekseev::Dictionary::write(wstr_cr file_name)
{
  std::ofstream os(file_name.data(), std::ios::binary);
  if (!os.is_open()) {
    throw std::invalid_argument("Failed to open file");
  }
  try {
    write(os);
  } catch (...) {
    os.close();
    throw;
  }
  os.close();
}

std::ofstream & alekseev::Dictionary::write(std::ofstream & os)
{
  if (!os.is_open() || !os.good()) {
    return os;
  }
  for (auto keys_it = lemmas_.begin(); keys_it != lemmas_.end(); ++keys_it) {
    os << wstring_to_utf8(to_wstring(lemmas_.at(*keys_it))) << "\n";
  }
  for (auto keys_it = requires_.begin(); keys_it != requires_.end(); ++keys_it) {
    os << wstring_to_utf8(to_wstring(requires_.at(*keys_it))) << "\n";
  }
  return os;
}

void alekseev::Dictionary::add_lemma(const std::wstring & lemma, pos pos, gender noun_gender,
    aspect verb_aspect)
{
  Lemma l;
  l.lemma_ = lemma;
  l.pos_ = pos;
  l.verb_aspect_ = verb_aspect;
  l.noun_gender_ = noun_gender;
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

void alekseev::Dictionary::add_require(wstr_cr require)
{
  Lemma r;
  r.lemma_ = require;
  r.pos_ = pos::require;
  r.noun_gender_ = nn_gender;
  r.verb_aspect_ = nn_aspect;
  requires_.insert(require, r);
}

void alekseev::Dictionary::add_req_form(wstr_cr require, wstr_cr reqform, gender g, number n,
    case_ c, tense t, person p)
{
  WordForm wf(reqform, g, n, c, t, p);
  Lemma & l = requires_.at(require);
  l.forms_.pushBack(wf);
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

void alekseev::Dictionary::remove_require(wstr_cr require)
{
  requires_.remove(require);
}

void alekseev::Dictionary::remove_req_form(wstr_cr require, size_t ind)
{
  Vector< WordForm > & wfs = requires_.at(require).forms_;
  if (wfs.getSize() <= ind) {
    throw std::out_of_range("Index out of range (remove_req_form)");
  }
  wfs.erase(ind);
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

bool alekseev::Dictionary::contains_require(wstr_cr req) const
{
  return requires_.contains(req);
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

std::pair< std::wstring, size_t > alekseev::Dictionary::req_pair_by_wordform(
    const WordForm & wordform) const
{
  const Vector< WordForm > & wfs = requires_.at(wordform.word_).forms_;
  for (size_t i = 0; i < wfs.getSize(); ++i) {
    if (wfs[i] == wordform) {
      return std::make_pair(wordform.word_, i);
    }
  }
  throw std::out_of_range("Require not found");
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

const alekseev::Lemma & alekseev::Dictionary::get_lemma(wstr_cr lemma) const
{
  if (lemmas_.contains(lemma)) {
    return lemmas_.at(lemma);
  }
  if (requires_.contains(lemma)) {
    return requires_.at(lemma);
  }
  throw std::out_of_range("lemma not found");
}

alekseev::Vector< std::wstring > alekseev::Dictionary::get_lemmas() const
{
  return lemmas_.keys();
}

alekseev::Vector< alekseev::WordForm > & alekseev::Dictionary::forms_by_lemma(wstr_cr lemma)
{
  if (contains_lemma(lemma)) {
    return lemmas_.at(lemma).forms_;
  } else {
    return requires_.at(lemma).forms_;
  }
}

alekseev::pos alekseev::Dictionary::pos_of_lemma(wstr_cr lemma) const
{
  if (contains_lemma(lemma)) {
    return lemmas_.at(lemma).pos_;
  }
  if (contains_require(lemma)) {
    return require;
  }
  throw std::out_of_range("Lemma not found");
}

alekseev::Vector< alekseev::pos > alekseev::Dictionary::pos_of_form(wstr_cr wordform) const
{
  const Vector< std::pair< std::wstring, size_t > > & hwf = find_homoforms(wordform);
  Vector< pos > res(hwf.getSize(), unknown);
  for (size_t i = 0; i < hwf.getSize(); ++i) {
    res[i] = pos_of_lemma(hwf[i].first);
  }
  return res;
}

alekseev::Vector< alekseev::WordForm > alekseev::Dictionary::filter_by_require(wstr_cr require,
    const Vector< WordForm > & forms) const
{
  Vector< WordForm > res;
  for (size_t i = 0; i < forms.getSize(); ++i) {
    if (matches_require(require, forms[i])) {
      res.pushBack(forms[i]);
    }
  }
  return res;
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

bool alekseev::Dictionary::matches_case(wstr_cr require, wstr_cr word) const
{
  if (!requires_.contains(require)) {
    return false;
  }
  bool matches = false;
  const Vector< WordForm > & rfs = requires_.at(require).forms_;
  for (size_t i = 0; i < rfs.getSize() && !matches; ++i) {
    if (rfs[i].case_ != nn_case) {
      matches = matches_case(word, rfs[i].case_);
    }
  }
  return matches;
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

bool alekseev::Dictionary::matches_person(wstr_cr require, wstr_cr word) const
{
  if (!requires_.contains(require)) {
    return false;
  }
  bool matches = false;
  const Vector< WordForm > & rfs = requires_.at(require).forms_;
  for (size_t i = 0; i < rfs.getSize() && !matches; ++i) {
    if (rfs[i].person_ != nn_person) {
      matches = matches_person(word, rfs[i].person_);
    }
  }
  return matches;
}

bool alekseev::Dictionary::matches_require(wstr_cr require, wstr_cr word) const
{
  if (!requires_.contains(require)) {
    return false;
  }
  Vector< pos > v = pos_of_form(word);
  bool res = false;
  for (size_t i = 0; i < v.getSize() && !res; ++i) {
    if (v[i] == noun) {
      res = matches_case(require, word);
    } else if (v[i] == verb) {
      res = matches_person(require, word);
    } else {
      res = true;
    }
  }
  return res;
}

bool alekseev::Dictionary::matches_require(wstr_cr require, const WordForm & word) const
{
  const Vector< WordForm > & rfs = requires_.at(require).forms_;
  bool res = false;
  for (size_t i = 0; i < rfs.getSize() && !res; ++i) {
    res = matches(rfs[i], word);
  }
  return res;
}

size_t alekseev::Dictionary::size() const
{
  return forms_.size() + requires_.size();
}

alekseev::Vector< alekseev::WordForm > alekseev::Dictionary::damerau_find_wfs(wstr_cr bad_word,
    size_t max_number, size_t distance) const
{
  if (forms_.contains(bad_word)) {
    return get_homoforms(bad_word);
  }
  Vector< WordForm > res;
  size_t m = max_number == 0 ? forms_.size() : max_number;
  long long int bad_word_size = bad_word.size();
  for (auto wfs_it = forms_.begin(); wfs_it != forms_.end() && res.getSize() < max_number; ++
       wfs_it) {
    long long int cur_size = wfs_it->size();
    if (std::abs(bad_word_size - cur_size) <= distance) {
      if (damerau_levenshtein(*wfs_it, bad_word) <= distance) {
        Vector< WordForm > found = get_homoforms(*wfs_it);
        res += found;
      }
    }
  }
  while (res.getSize() > m) {
    res.popBack();
  }
  return res;
}

alekseev::Vector< std::wstring > alekseev::Dictionary::damerau_find_form(wstr_cr bad_form,
    size_t max_number, size_t distance) const
{
  if (forms_.contains(bad_form)) {
    return {1, bad_form};
  }
  return damerau_find(bad_form, forms_.keys(), max_number, distance);
}

alekseev::Vector< std::wstring > alekseev::Dictionary::damerau_find_lemma(wstr_cr bad_lemma,
    size_t max_number, size_t distance) const
{
  if (lemmas_.contains(bad_lemma)) {
    return {1, bad_lemma};
  }
  return damerau_find(bad_lemma, lemmas_.keys(), max_number, distance);
}

alekseev::Vector< std::wstring > alekseev::Dictionary::damerau_find_require(wstr_cr bad_req,
    size_t max_number, size_t distance) const
{
  if (requires_.contains(bad_req)) {
    return {1, bad_req};
  }
  return damerau_find(bad_req, requires_.keys(), max_number, distance);
}

alekseev::DictionaryManager::DictionaryManager():
  dicts_(djb2_hash, poly_hash, equal, 16),
  max_variants_(5),
  distance_(2)
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
  bool l = dict.contains_lemma(w), r = dict.contains_require(w);
  if (l || r) {
    os << L"Word already exists in current dictionary \"" << current_ << L"\"\n";
    wchar_t ans = ask_yes_no(L"Do you want to replace it?", is, os);
    if (ans == L'y') {
      if (l) {
        dict.remove_lemma(w);
      } else {
        dict.remove_require(w);
      }
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
  } else if (p == require) {
    ans = ask_yes_no(L"Is it a require?", is, os);
  } else {
    ans = L'u';
  }
  if (ans != L'y') {
    std::wstring answer;
    os << "Input word class (verb/adj/noun/req): ";
    p = unknown;
    while (p == unknown && std::getline(is, answer)) {
      if (answer == L"verb") {
        p = verb;
      } else if (answer == L"adj") {
        p = adj;
      } else if (answer == L"noun") {
        p = noun;
      } else if (answer == L"req") {
        p = require;
      } else {
        os << L"Unknown word class: " << answer << L"\n";
        os << L"Input some of \"verb\", \"adj\", \"noun\", \"req\"";
        os << "Input word class (verb/adj/noun): ";
      }
    }
  }
  if (p == verb) {
    add_verb(w, is, os);
  } else if (p == adj) {
    add_adj(w, is, os);
  } else if (p == noun) {
    add_noun(w, is, os);
  } else if (p == require) {
    add_req(w, is, os);
  }
}

void alekseev::DictionaryManager::update_word(std::wstring word, std::wistream & is,
    std::wostream & os)
{
  Dictionary & dict = current();
  wchar_t ans = L'u';
  if (!dict.contains_lemma(word) && !dict.contains_require(word)) {
    std::wstring q = L"\"" + word + L"\" not found. Do you want to use with fuzzy search?";
    ans = ask_yes_no(q, is, os);
    if (ans == L'y') {
      Vector< std::wstring > opts = dict.damerau_find_lemma(word, max_variants_, distance_);
      opts += dict.damerau_find_require(word, max_variants_ - opts.getSize(), distance_);
      size_t ind = choose(opts, is, os, 5, L"What lemma do you want to update?");
      if (ind == opts.getSize()) {
        return;
      }
      word = opts[ind];
    } else if (ans == L'n') {
      return;
    } else {
      throw std::invalid_argument("Bad input");
    }
  }
  const Lemma & l = dict.get_lemma(word);
  os << "Found:\n";
  os << l;
  Vector< std::wstring > opts{L"New form", L"Update an existing form"};
  size_t a = choose(opts, is, os, 0, L"What do you want to do?",
      L"It is not a correct word, cancel updating");
  if (a == 2) {
    return;
  } else if (a == 1) {
    size_t ind = choose(l.forms_, is, os, 0, L"What form do you want to update?");
    if (ind == l.forms_.getSize()) {
      return;
    }
    if (l.pos_ == require) {
      dict.remove_req_form(word, ind);
    } else {
      dict.remove_form(l.forms_[ind]);
    }
  }
  os << "Enter new form with tags:\n";
  std::wstring form;
  WordForm wf;
  while (std::getline(is, form)) {
    try {
      wf = WordForm(split(form), l.pos_);
    } catch (std::invalid_argument & e) {
      os << L"Bad input or bad tags\n";
    }
    if (!wf.word_.empty()) {
      if (l.pos_ == require) {
        dict.add_req_form(l.lemma_, wf.word_, wf.gender_, wf.number_, wf.case_, wf.tense_,
            wf.person_);
      } else {
        dict.add_form(l.lemma_, wf.word_, wf.gender_, wf.number_, wf.case_, wf.tense_, wf.person_);
      }
      os << L"Successfully added form " << wf << L"\n";
      return;
    }
  }
  throw std::invalid_argument("Bad input");
}

void alekseev::DictionaryManager::delete_form(wstr_cr wordform, std::wistream & is,
    std::wostream & os)
{
  Dictionary & dict = current();
  std::pair< std::wstring, size_t > lp = choose_wordform(wordform, is, os);

  if (!lp.first.empty()) {
    const WordForm & word = dict.forms_by_lemma(lp.first)[lp.second];
    if (dict.pos_of_lemma(lp.first) == require) {
      dict.remove_req_form(lp.first, lp.second);
    } else {
      dict.remove_form(word);
    }
  }
}

void alekseev::DictionaryManager::delete_lemma(wstr_cr lemma, std::wistream & is,
    std::wostream & os)
{
  Dictionary & dict = current();
  if (dict.contains_lemma(lemma)) {
    dict.remove_lemma(lemma);
  } else if (dict.contains_require(lemma)) {
    dict.remove_require(lemma);
  } else {
    os << L"Lemma \"" << lemma << "\" not found in current dictionary\n";
    wchar_t ans = ask_yes_no(L"Do you want to search using fuzzy search?", is, os);
    if (ans == 'y') {
      Vector< std::wstring > opts = dict.damerau_find_lemma(lemma, max_variants_, distance_);
      opts += dict.damerau_find_require(lemma, max_variants_ - opts.getSize(), distance_);
      size_t ind = choose(opts, is, os, 5, L"What lemma you want to delete?");
      if (ind == opts.getSize()) {
        return;
      }
      if (dict.contains_lemma(lemma)) {
        dict.remove_lemma(lemma);
      } else if (dict.contains_require(lemma)) {
        dict.remove_require(lemma);
      }
    }
  }
}

bool alekseev::DictionaryManager::contains_form(wstr_cr wordform) const
{
  bool result = false;
  for (auto names_it = dicts_.begin(); names_it != dicts_.end(); ++names_it) {
    result = dicts_.at(*names_it).contains_form(wordform);
  }
  return result;
}

bool alekseev::DictionaryManager::is_require(wstr_cr word) const
{
  bool result = false;
  for (auto names_it = dicts_.begin(); names_it != dicts_.end(); ++names_it) {
    result = dicts_.at(*names_it).contains_require(word);
  }
  return result;
}

alekseev::Vector< std::wstring > alekseev::DictionaryManager::damerau_find_form(wstr_cr wordform,
    size_t max_number, size_t distance) const
{
  if (distance == 0) {
    distance = distance_;
  }
  if (max_number == 0) {
    max_number = max_variants_;
  }
  Vector< std::wstring > res;
  auto check = [&max_number, &res]() {
    return res.getSize() < max_number || max_number == 0;
  };
  for (auto names_it = dicts_.begin(); names_it != dicts_.end() && check(); ++names_it) {
    res += dicts_.at(*names_it).damerau_find_form(wordform, max_number, distance);
  }
  while (res.getSize() > max_number && max_number != 0) {
    res.popBack();
  }
  return res;
}

alekseev::Vector< std::wstring > alekseev::DictionaryManager::damerau_find_lemma(wstr_cr wordform,
    size_t max_number, size_t distance) const
{
  if (distance == 0) {
    distance = distance_;
  }
  if (max_number == 0) {
    max_number = max_variants_;
  }
  Vector< std::wstring > res;
  auto check = [&max_number, &res]() {
    return res.getSize() < max_number || max_number == 0;
  };
  for (auto names_it = dicts_.begin(); names_it != dicts_.end() && check(); ++names_it) {
    res += dicts_.at(*names_it).damerau_find_lemma(wordform, max_number, distance);
  }
  while (res.getSize() > max_number && max_number != 0) {
    res.popBack();
  }
  return res;
}

alekseev::Vector< std::wstring > alekseev::DictionaryManager::find_by_require(wstr_cr require,
    wstr_cr wordform, size_t max_number, size_t distance) const
{
  if (distance == 0) {
    distance = distance_;
  }
  if (max_number == 0) {
    max_number = max_variants_;
  }
  Vector< std::wstring > res;
  auto check = [&max_number, &res]() {
    return res.getSize() < max_number || max_number == 0;
  };
  for (auto names_it = dicts_.begin(); names_it != dicts_.end() && check(); ++names_it) {
    const Dictionary & dict = dicts_.at(*names_it);
    res += to_words(
        dict.filter_by_require(require,
            dict.damerau_find_wfs(wordform, 0, distance)
            )
        );
  }
  while (res.getSize() > max_number && max_number != 0) {
    res.popBack();
  }
  return res;
}

bool alekseev::DictionaryManager::matches_case(wstr_cr wordform, case_ expected_case) const
{
  bool result = false;
  for (auto names_it = dicts_.begin(); names_it != dicts_.end() && !result; ++names_it) {
    result = dicts_.at(*names_it).matches_case(wordform, expected_case);
  }
  return result;
}

bool alekseev::DictionaryManager::matches_case(wstr_cr require, wstr_cr word) const
{
  bool result = false;
  for (auto names_it = dicts_.begin(); names_it != dicts_.end() && !result; ++names_it) {
    result = dicts_.at(*names_it).matches_case(require, word);
  }
  return result;
}

bool alekseev::DictionaryManager::matches_person(wstr_cr wordform, person expected_person) const
{
  bool result = false;
  for (auto names_it = dicts_.begin(); names_it != dicts_.end() && !result; ++names_it) {
    result = dicts_.at(*names_it).matches_person(wordform, expected_person);
  }
  return result;
}

bool alekseev::DictionaryManager::matches_person(wstr_cr require, wstr_cr word) const
{
  bool result = false;
  for (auto names_it = dicts_.begin(); names_it != dicts_.end() && !result; ++names_it) {
    result = dicts_.at(*names_it).matches_person(require, word);
  }
  return result;
}

bool alekseev::DictionaryManager::matches_require(wstr_cr require, wstr_cr word) const
{
  bool result = false;
  for (auto names_it = dicts_.begin(); names_it != dicts_.end() && !result; ++names_it) {
    result = dicts_.at(*names_it).matches_require(require, word);
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

bool alekseev::DictionaryManager::contains_dict(wstr_cr dict_name) const
{
  return dicts_.contains(dict_name);
}

void alekseev::DictionaryManager::set_max_variants(size_t max_variants)
{
  max_variants_ = max_variants;
}

void alekseev::DictionaryManager::set_default_distance(size_t distance)
{
  if (distance != 0) {
    distance_ = distance;
  }
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
  Dictionary & dict = current();
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
      os << "\t" << cases_names[j] << ":";
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
  Dictionary & dict = current();
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

void alekseev::DictionaryManager::add_req(wstr_cr word, std::wistream & is, std::wostream & os)
{
  Dictionary dict = current();
  dict.add_require(word);

  os << L"Enter the forms of the require, each on a separate line.\n";
  os << L"Leave the line empty to complete the input.\n";

  std::wstring line;
  size_t c = 0;
  getline(is, line);
  while (is && !line.empty()) {
    ++c;
    WordForm wf(split(line), require);
    dict.add_req_form(word, wf.word_, wf.gender_, wf.number_, wf.case_, wf.tense_, wf.person_);
    std::getline(is, line);
  }
  os << L"Successfully added " << c << " forms!\n";
}

std::pair< std::wstring, size_t > alekseev::DictionaryManager::choose_wordform(wstr_cr word,
    std::wistream & is, std::wostream & os, size_t max_opts, size_t distance) const
{
  if (distance == 0) {
    distance = distance_;
  }
  if (max_opts == 0) {
    max_opts = max_variants_;
  }
  const Dictionary & dict = current();
  Vector< WordForm > wfs;

  if (dict.contains_form(word)) {
    wfs = dict.get_homoforms(word);
  } else if (dict.contains_require(word)) {
    wfs = dict.get_lemma(word).forms_;
  } else {
    os << L"Form " << word << " not found\n";
    wchar_t need_find = ask_yes_no(L"Do you want to search using fuzzy search?", is, os);
    if (need_find == L'y') {
      wfs = dict.damerau_find_wfs(word, max_opts, distance);
      size_t m = max_opts == 0 ? 0 : max_opts - wfs.getSize();
      if (max_opts != wfs.getSize()) {
        Vector< std::wstring > reqs = dict.damerau_find_require(word, m, distance);
        for (size_t i = 0; i < reqs.getSize() && (wfs.getSize() < max_opts || max_opts == 0); ++i) {
          wfs += dict.get_lemma(reqs[i]).forms_;
        }
      }
      if (wfs.isEmpty()) {
        os << L"No word found\n";
        return std::make_pair< std::wstring, size_t >({}, 0);
      }
    }
  }

  if (!wfs.isEmpty()) {
    size_t ind = choose(wfs, is, os, max_opts, L"Choose one form:");
    if (ind < wfs.getSize()) {
      if (dict.contains_require(wfs[ind].word_)) {
        return dict.req_pair_by_wordform(wfs[ind]);
      }
      return dict.lemma_pair_by_wordform(wfs[ind]);
    }
  }
  return {{}, 0};
}
