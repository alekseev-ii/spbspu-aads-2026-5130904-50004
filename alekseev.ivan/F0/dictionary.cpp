#include "dictionary.h"

#include <locale>
#include <codecvt>
#include <iomanip>

#include "wstr_functions.h"

alekseev::WordForm::WordForm():
  pos_(unknown),
  gender_(nn_gender),
  number_(nn_number),
  case_(nn_case),
  tense_(nn_tense),
  person_(nn_person)
{ }

alekseev::WordForm::WordForm(std::wstring wordform, pos p, gender g, number n, case_e c, tense t,
    person pe):
  word_(std::move(wordform)),
  pos_(p),
  gender_(g),
  number_(n),
  case_(c),
  tense_(t),
  person_(pe)
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
  bool result = word_ == rhs.word_ && pos_ == rhs.pos_;
  result = result && gender_ == rhs.gender_ && number_ == rhs.number_;
  result = result && case_ == rhs.case_ && tense_ == rhs.tense_;
  return result && person_ == rhs.person_;
}

bool alekseev::matches(const WordForm & req, const WordForm & word)
{
  if (((req.person_ != nn_person || req.tense_ != nn_tense) && word.pos_ != verb) || ((req.person_
    == nn_person && req.tense_ == nn_tense) && word.pos_ == verb)) {
    return true;
  }
  bool res = req.gender_ == nn_gender || word.gender_ == nn_gender || req.gender_ == common || word.
      gender_ == common || req.gender_ == word.gender_;
  res = res && (req.number_ == nn_number || word.number_ == nn_number || req.number_ == word.
    number_);
  res = res && (req.case_ == nn_case || word.case_ == nn_case || req.case_ == word.case_);
  res = res && (req.tense_ == nn_tense || word.tense_ == nn_tense || req.tense_ == word.tense_);
  res = res && (req.person_ == nn_person || word.person_ == nn_person || req.person_ == word.
    person_);
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
  } else if (wf.gender_ == common) {
    res.pushBack(L"common");
  }
  if (wf.number_ == singular) {
    res.pushBack(L"sing");
  } else if (wf.number_ == plural) {
    res.pushBack(L"plur");
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
  WordForm wf(tags[0], p);
  if (p == functional) {
    return wf;
  }
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
    } else if (tags[i] == L"plur") {
      wf.number_ = plural;
    }
    if (p != noun) {
      if (tags[i] == L"masc") {
        wf.gender_ = masculine;
      } else if (tags[i] == L"fem") {
        wf.gender_ = feminine;
      } else if (tags[i] == L"neut") {
        wf.gender_ = neuter;
      } else if (tags[i] == L"common") {
        wf.gender_ = common;
      }
    }
    if (pre == wf) {
      throw std::invalid_argument("Bad tag");
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
    } else if (lemma.noun_gender_ == common) {
      res += L"com";
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
    res += L"    " + to_wstring(wf) + L"\n";
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
  std::wstring v = word;
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

alekseev::Dictionary::Dictionary():
  lemmas_(djb2_hash, poly_hash, equal, 512),
  forms_(djb2_hash, poly_hash, equal, 16388),
  requires_(djb2_hash, poly_hash, equal, 128)
{ }

alekseev::Dictionary::Dictionary(wstr_cr file_name):
  Dictionary()
{
  read(file_name);
}

void alekseev::Dictionary::read(wstr_cr file_name)
{
  std::wstring_convert< std::codecvt_utf8< wchar_t > > converter;
  std::ifstream is(converter.to_bytes(file_name));
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
  std::wstring_convert< std::codecvt_utf8< wchar_t > > converter;
  while (std::getline(is, line)) {
    if (line.empty()) {
      continue;
    }
    std::wstring wline = converter.from_bytes(line);
    if (wline[0] == L'#') {
      continue;
    }

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
      while (lemmas_.contains(lemma.lemma_)) {
        lemma.lemma_ += L"_";
      }
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
        } else if (words[2] == L"com") {
          lemma.noun_gender_ = common;
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
      } else if (words[1] == L"fun") {
        if (words.getSize() != 2) {
          throw std::invalid_argument("Bad number of tags for functional");
        }
        lemma.pos_ = functional;
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
  std::wstring_convert< std::codecvt_utf8< wchar_t > > converter;
  std::ofstream os(converter.to_bytes(file_name));
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
  std::wstring_convert< std::codecvt_utf8< wchar_t > > converter;
  for (auto keys_it = lemmas_.begin(); keys_it != lemmas_.end(); ++keys_it) {
    os << converter.to_bytes(to_wstring(lemmas_.at(*keys_it))) << "\n";
  }
  for (auto keys_it = requires_.begin(); keys_it != requires_.end(); ++keys_it) {
    os << converter.to_bytes(to_wstring(requires_.at(*keys_it))) << "\n";
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
    gender g, number n, case_e c, tense t, person p)
{
  WordForm wf(wordform, pos_of_lemma(lemma), g, n, c, t, p);
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
    case_e c, tense t, person p)
{
  WordForm wf(reqform, pos::require, g, n, c, t, p);
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
    if (!forms_.contains(l.forms_[i].word_)) {
      continue;
    }
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
    size_t ind = wfs[i].second;
    if (l.forms_[ind] == wordform) {
      wfs.erase(i);
      if (wfs.isEmpty()) {
        forms_.remove(wordform.word_);
      }
      for (size_t j = ind + 1; j < l.forms_.getSize(); ++j) {
        lemma_pair_by_wordform(l.forms_[j]).second--;
      }
      l.forms_.erase(ind);
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

alekseev::Vector< alekseev::WordForm > & alekseev::Dictionary::forms_by_lemma(wstr_cr lemma)
{
  if (contains_lemma(lemma)) {
    return lemmas_.at(lemma).forms_;
  }
  return requires_.at(lemma).forms_;
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

bool alekseev::Dictionary::matches_case(wstr_cr wordform, case_e expected_case) const
{
  if (!forms_.contains(wordform)) {
    return false;
  }
  Vector< WordForm > wfs = get_homoforms(wordform);
  for (size_t i = 0; i < wfs.getSize(); ++i) {
    if (wfs[i].case_ == expected_case || wfs[i].case_ == nn_case) {
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
    if (wfs[i].person_ == expected_person || wfs[i].person_ == nn_person) {
      return true;
    }
  }
  return false;
}

bool alekseev::Dictionary::matches_require(wstr_cr require, wstr_cr word) const
{
  if (!requires_.contains(require) || !forms_.contains(word)) {
    return false;
  }
  Vector< WordForm > wfs = get_homoforms(word);
  for (size_t i = 0; i < wfs.getSize(); ++i) {
    if (matches_require(require, wfs[i])) {
      return true;
    }
  }
  return false;
}

bool alekseev::Dictionary::matches_require(wstr_cr require, const WordForm & word) const
{
  if (!contains_require(require)) {
    return false;
  }
  const Vector< WordForm > & reqs = requires_.at(require).forms_;
  for (size_t i = 0; i < reqs.getSize(); ++i) {
    if (matches(reqs[i], word)) {
      return true;
    }
  }
  return false;
}

size_t alekseev::Dictionary::size() const
{
  return forms_.size() + requires_.size();
}

alekseev::Vector< alekseev::WordForm > alekseev::Dictionary::damerau_find_wfs(wstr_cr bad_word,
    size_t max_number, size_t distance) const
{
  Vector< Vector< WordForm > > res(distance + 1, {});
  size_t count = 0;
  size_t m = max_number == 0 ? forms_.size() : max_number;
  long long int bad_word_size = bad_word.size();
  for (auto wfs_it = forms_.begin(); wfs_it != forms_.end() && count < m; ++wfs_it) {
    long long int cur_size = wfs_it->size();
    if (std::abs(bad_word_size - cur_size) <= distance) {
      size_t cur_dist = damerau_levenshtein(*wfs_it, bad_word);
      if (cur_dist <= distance) {
        Vector< WordForm > found = get_homoforms(*wfs_it);
        res[cur_dist] += found;
        count += found.getSize();
      }
    }
  }
  Vector< WordForm > final;
  final.resize(count);
  for (size_t i = 0; i < distance + 1; ++i) {
    final += res[i];
  }
  while (final.getSize() > m) {
    final.popBack();
  }
  return final;
}

alekseev::Vector< std::wstring > alekseev::Dictionary::damerau_find_form(wstr_cr bad_form,
    size_t max_number, size_t distance) const
{
  return damerau_find(bad_form, forms_.begin(), forms_.end(), max_number, distance);
}

alekseev::Vector< std::wstring > alekseev::Dictionary::damerau_find_lemma(wstr_cr bad_lemma,
    size_t max_number, size_t distance) const
{
  return damerau_find(bad_lemma, lemmas_.begin(), lemmas_.end(), max_number, distance);
}

alekseev::Vector< std::wstring > alekseev::Dictionary::damerau_find_require(wstr_cr bad_req,
    size_t max_number, size_t distance) const
{
  return damerau_find(bad_req, requires_.begin(), requires_.end(), max_number, distance);
}

alekseev::DictionaryManager::DictionaryManager():
  dicts_(djb2_hash, poly_hash, equal, 16),
  max_variants_(5),
  distance_(1)
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
  dicts_.insert(name, std::move(d));
  current_ = name;
}

void alekseev::DictionaryManager::save(wstr_cr name, wstr_cr file_name)
{
  dicts_.at(name).write(file_name);
}

void alekseev::DictionaryManager::unload(wstr_cr name)
{
  dicts_.remove(name);
  if (current_ == name) {
    current_ = L"";
  }
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
    Vector< std::wstring > poses{L"verb", L"adjective", L"noun"};
    size_t pos_number = choose(poses, is, os, 0, L"Choose word class:", L"require");
    if (pos_number == 0) {
      p = verb;
    } else if (pos_number == 1) {
      p = adj;
    } else if (pos_number == 2) {
      p = noun;
    } else if (pos_number == 3) {
      p = require;
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
    std::wstring q = L"\"" + word + L"\" not found. Do you want to use fuzzy search?";
    ans = ask_yes_no(q, is, os);
    if (ans == L'y') {
      Vector< std::wstring > opts = dict.damerau_find_lemma(word, max_variants_, distance_);
      opts += dict.damerau_find_require(word, max_variants_ - opts.getSize(), distance_);
      if (opts.isEmpty()) {
        os << L"Nothing found\n";
        return;
      }
      size_t ind = choose(opts, is, os, max_variants_, L"What lemma do you want to update?");
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
  bool has_forms = !l.forms_.isEmpty();
  Vector< std::wstring > opts{L"Add new form"};
  if (has_forms) {
    opts.pushBack(L"Replace an existing form");
  }
  size_t a = choose(opts, is, os, 0, L"What do you want to do?",
      L"It is not a correct word, cancel updating");
  if ((has_forms && a == 2) || (!has_forms && a == 1)) {
    return;
  }
  if (a == 1) {
    size_t ind = choose(l.forms_, is, os, 0, L"What form do you want to replace?");
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
  while (wgetline(is, form)) {
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
    if (word.pos_ == require) {
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
      size_t ind = choose(opts, is, os, max_variants_, L"What lemma you want to delete?");
      if (ind == opts.getSize()) {
        return;
      }
      wstr_cr l = opts[ind];
      if (dict.contains_lemma(l)) {
        dict.remove_lemma(l);
      } else if (dict.contains_require(l)) {
        dict.remove_require(l);
      }
    }
  }
}

bool alekseev::DictionaryManager::contains_form(wstr_cr wordform) const
{
  bool result = false;
  for (auto names_it = dicts_.begin(); names_it != dicts_.end() && !result; ++names_it) {
    result = dicts_.at(*names_it).contains_form(wordform);
  }
  return result;
}

bool alekseev::DictionaryManager::is_require(wstr_cr word) const
{
  bool result = false;
  for (auto names_it = dicts_.begin(); names_it != dicts_.end() && !result; ++names_it) {
    result = dicts_.at(*names_it).contains_require(word);
  }
  return result;
}

alekseev::Vector< alekseev::WordForm > alekseev::DictionaryManager::filter_by_require(
    wstr_cr require, const Vector< WordForm > & wfs) const
{
  Vector< WordForm > result;
  for (size_t i = 0; i < wfs.getSize(); ++i) {
    if (matches_require(require, wfs[i])) {
      result.pushBack(wfs[i]);
    }
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
  auto check = [&max_number, &res]()
  {
    return res.getSize() < max_number || max_number == 0;
  };
  for (auto names_it = dicts_.begin(); names_it != dicts_.end() && check(); ++names_it) {
    size_t n = (max_number == 0) ? 0 : max_number - res.getSize();
    res += dicts_.at(*names_it).damerau_find_form(wordform, n, distance);
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
  auto check = [&max_number, &res]()
  {
    return res.getSize() < max_number || max_number == 0;
  };
  for (auto names_it = dicts_.begin(); names_it != dicts_.end() && check(); ++names_it) {
    size_t n = (max_number == 0) ? 0 : max_number - res.getSize();
    res += dicts_.at(*names_it).damerau_find_lemma(wordform, n, distance);
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
  auto check = [&max_number, &res]()
  {
    return res.getSize() < max_number || max_number == 0;
  };
  for (auto names_it = dicts_.begin(); names_it != dicts_.end() && check(); ++names_it) {
    const Dictionary & dict = dicts_.at(*names_it);
    res += to_words(filter_by_require(require, dict.damerau_find_wfs(wordform, 0, distance)));
  }
  while (res.getSize() > max_number && max_number != 0) {
    res.popBack();
  }
  return res;
}

bool alekseev::DictionaryManager::matches_case(wstr_cr wordform, case_e expected_case) const
{
  bool result = false;
  for (auto names_it = dicts_.begin(); names_it != dicts_.end() && !result; ++names_it) {
    result = dicts_.at(*names_it).matches_case(wordform, expected_case);
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

bool alekseev::DictionaryManager::matches_require(wstr_cr require, wstr_cr word) const
{
  bool res = false;
  for (auto forms_it = dicts_.begin(); forms_it != dicts_.end() && !res; ++forms_it) {
    const Dictionary & cur_dict = dicts_.at(*forms_it);
    if (!cur_dict.contains_form(word)) {
      continue;
    }
    Vector< WordForm > hfs = cur_dict.get_homoforms(word);
    for (size_t i = 0; i < hfs.getSize() && !res; ++i) {
      res = matches_require(require, hfs[i]);
    }
  }
  return res;
}

bool alekseev::DictionaryManager::matches_require(wstr_cr require, const WordForm & word) const
{
  bool res = false;
  for (auto reqs_it = dicts_.begin(); reqs_it != dicts_.end() && !res; ++reqs_it) {
    res = dicts_.at(*reqs_it).matches_require(require, word);
  }
  return res;
}

alekseev::Dictionary & alekseev::DictionaryManager::current()
{
  if (current_.empty()) {
    throw std::logic_error("Current dictionary is not defined");
  }
  return dicts_.at(current_);
}

const alekseev::Dictionary & alekseev::DictionaryManager::current() const
{
  if (current_.empty()) {
    throw std::logic_error("Current dictionary is not defined");
  }
  return dicts_.at(current_);
}

size_t alekseev::DictionaryManager::size() const
{
  size_t s = 0;
  for (auto it = dicts_.begin(); it != dicts_.end(); ++it) {
    s += dicts_.at(*it).size();
  }
  return s;
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
  os << L"    Singular masculine (he) >";
  wgetline(is, past_masc);
  os << L"    Singular feminine (she) >";
  wgetline(is, past_fem);
  os << L"    Singular neut (it)      >";
  wgetline(is, past_neut);
  os << L"    Plural (they)           >";
  wgetline(is, past_pl);

  if (verb_aspect == perf) {
    os << L"Future tense:\n";
  } else {
    os << L"Present tense:\n";
  }
  std::wstring pres_1s, pres_2s, pres_3s, pres_1p, pres_2p, pres_3p;
  os << L"    1st singular (I)      >";
  wgetline(is, pres_1s);
  os << L"    2nd singular (you)    >";
  wgetline(is, pres_2s);
  os << L"    3rd singular (he/she) >";
  wgetline(is, pres_3s);
  os << L"    1st plural (we)       >";
  wgetline(is, pres_1p);
  os << L"    2nd plural (you)      >";
  wgetline(is, pres_2p);
  os << L"    3rd plural (they)     >";
  wgetline(is, pres_3p);

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

  tense t = (verb_aspect == perf) ? future : present;
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
  os << L"Successfully added " << c << L" forms!\n";
}

void alekseev::DictionaryManager::add_adj(wstr_cr word, std::wistream & is, std::wostream & os)
{
  Dictionary & dict = current();
  IOGuard ig(is), og(os);
  dict.add_lemma(word, adj, nn_gender, nn_aspect);
  case_e cases[6]{nominative, genitive, dative, accusative, instrumental, prepositional};
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
    os << L"Singular " << genders_names[i] << L":\n";
    for (size_t j = 0; j < 6; j++) {
      os << L"    " << std::left << std::setw(14) << cases_names[j] << L">";
      wgetline(is, form);
      if (!form.empty()) {
        dict.add_form(word, form, genders[i], singular, cases[j], nn_tense, nn_person);
        ++c;
      }
    }
  }

  os << L"Plural:\n";
  for (size_t i = 0; i < 6; i++) {
    os << "    " << std::left << std::setw(14) << cases_names[i] << " >";
    wgetline(is, form);
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
  IOGuard ig(is), og(os);
  os << L"Enter noun gender (masc/fem/neut/com) >";
  std::wstring gender_ans;
  gender g = nn_gender;
  while (wgetline(is, gender_ans) && g == nn_gender) {
    if (gender_ans == L"masc") {
      g = masculine;
    } else if (gender_ans == L"fem") {
      g = feminine;
    } else if (gender_ans == L"neut") {
      g = neuter;
    } else if (gender_ans == L"com") {
      g = common;
    } else {
      os << L"Bad noun gender: " << gender_ans << L"\n";
      os << L"Enter noun gender (masc/fem/neut/com) >";
    }
  }
  dict.add_lemma(word, noun, g, nn_aspect);
  case_e cases[6]{nominative, genitive, dative, accusative, instrumental, prepositional};
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
    os << numbers_names[i] << L":\n";
    for (size_t j = 0; j < 6; j++) {
      os << L"    " << std::left << std::setw(14) << cases_names[j] << L">";
      wgetline(is, form);
      if (!form.empty()) {
        dict.add_form(word, form, g, numbers[i], cases[j], nn_tense, nn_person);
        ++c;
      }
    }
  }
  os << L"Successfully added " << c << L" forms!\n";
}

void alekseev::DictionaryManager::add_req(wstr_cr word, std::wistream & is, std::wostream & os)
{
  Dictionary & dict = current();
  dict.add_require(word);

  os << L"Enter the forms of the require, each on a separate line.\n";
  os << L"Leave the line empty to complete the input.\n";

  std::wstring line;
  size_t c = 0;
  wgetline(is, line);
  while (is && !line.empty()) {
    ++c;
    WordForm wf(split(line), require);
    dict.add_req_form(word, wf.word_, wf.gender_, wf.number_, wf.case_, wf.tense_, wf.person_);
    wgetline(is, line);
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
    os << L"Form \"" << word << L"\" not found\n";
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
