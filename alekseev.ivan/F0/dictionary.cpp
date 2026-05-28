#include "dictionary.h"

#include <utility>

alekseev::WordForm::WordForm():
  word_(),
  gender_(nn_gender),
  number_(nn_number),
  case_(nn_case),
  tense_(nn_tense),
  person_(nn_person)
{ }

alekseev::WordForm::WordForm(std::string wordform, gender g, number n, alekseev::case_ c, tense t,
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

size_t alekseev::djb2_hash(str_cr line)
{
  size_t hash = 5381;
  for (size_t i = 0; i < line.size(); ++i) {
    hash = ((hash << 5) + hash) + line[i];
  }
  return hash;
}

size_t alekseev::poly_hash(str_cr line)
{
  size_t hash = 0;
  int p = 67;
  for (size_t i = 0; i < line.size(); ++i) {
    hash = hash * p + line[i];
  }
  return hash;
}

bool alekseev::equal(str_cr s1, str_cr s2)
{
  return s1 == s2;
}

alekseev::Vector< std::string > alekseev::split(const std::string & s, char delim)
{
  size_t start = 0;
  Vector< std::string > res;
  size_t i = 0;
  for (; i < s.size(); ++i) {
    if (s[i] == delim) {
      if (i > start) {
        res.pushBack(s.substr(start, i - start));
      }
      start = i + 1;
    }
  }
  res.pushBack(s.substr(start, i - start));
  return res;
}

alekseev::Dictionary::Dictionary():
  lemmas_(djb2_hash, poly_hash, equal, 4096),
  forms_(djb2_hash, poly_hash, equal, 16384)
{ }

alekseev::Dictionary::Dictionary(str_cr file_name):
  lemmas_(djb2_hash, poly_hash, equal, 4096),
  forms_(djb2_hash, poly_hash, equal, 16384)
{
  std::ifstream is(file_name);
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
    if (line[1] == '#') {
      continue;
    }
    Vector< std::string > words = split(line, ' ');
    if (words.isEmpty()) {
      continue;
    }
    if (line[0] != ' ' && line[0] != '\t') {
      if (!lemma.lemma_.empty()) {
        lemmas_.insert(lemma.lemma_, lemma);
        lemma = Lemma();
      }
      lemma.lemma_ = words[0];
      if (words[1] == "noun") {
        lemma.pos_ = noun;
        lemma.verb_aspect_ = nn_aspect;
        if (words[2] == "masc") {
          lemma.noun_gender_ = masculine;
        } else if (words[2] == "fem") {
          lemma.noun_gender_ = feminine;
        } else if (words[2] == "neut") {
          lemma.noun_gender_ = neuter;
        } else {
          throw std::invalid_argument("Invalid noun gender");
        }
      } else if (words[1] == "verb") {
        lemma.pos_ = verb;
        lemma.noun_gender_ = nn_gender;
        if (words[2] == "perf") {
          lemma.verb_aspect_ = perf;
        } else if (words[2] == "imperf") {
          lemma.verb_aspect_ = imperf;
        } else {
          throw std::invalid_argument("Invalid verb aspect");
        }
      } else if (words[1] == "adj") {
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
          if (words[i] == "nom") {
            wf.case_ = nominative;
          } else if (words[i] == "gen") {
            wf.case_ = genitive;
          } else if (words[i] == "dat") {
            wf.case_ = dative;
          } else if (words[i] == "acc") {
            wf.case_ = accusative;
          } else if (words[i] == "ins") {
            wf.case_ = instrumental;
          } else if (words[i] == "pre") {
            wf.case_ = prepositional;
          }
        }
        if (lemma.pos_ == verb) {
          if (words[i] == "pres") {
            wf.tense_ = present;
          } else if (words[i] == "past") {
            wf.tense_ = past;
          } else if (words[i] == "fut") {
            wf.tense_ = future;
          }
          if (words[i] == "1") {
            wf.person_ = first;
          } else if (words[i] == "2") {
            wf.person_ = second;
          } else if (words[i] == "3") {
            wf.person_ = third;
          }
        }
        if (words[i] == "sing") {
          wf.number_ = singular;
        } else if (words[i] == "pl") {
          wf.number_ = plural;
        }
        if (words[i] == "masc") {
          wf.gender_ = masculine;
        } else if (words[i] == "fem") {
          wf.gender_ = feminine;
        } else if (words[i] == "neut") {
          wf.gender_ = neuter;
        }
        if (pre == wf) {
          throw std::invalid_argument("Bad tag: " + words[i]);
        }
      }
      lemma.forms_.pushBack(wf);
      forms_.insert(wf.word_, std::make_pair(lemma.lemma_, lemma.forms_.getSize() - 1));
    }
  }
  if (!lemma.lemma_.empty()) {
    lemmas_.insert(lemma.lemma_, lemma);
  }
  return is;
}

std::ofstream & alekseev::Dictionary::write(std::ofstream & os)
{
  if (!os.is_open() || !os.good()) {
    return os;
  }
  Vector< std::string > keys = lemmas_.keys();
  for (size_t i = 0; i < keys.getSize(); ++i) {
    Lemma & lemma = lemmas_.at(keys[i]);
    os << lemma.lemma_ << " ";
    if (lemma.pos_ == noun) {
      os << "noun" << " ";
      if (lemma.noun_gender_ == masculine) {
        os << "masc";
      } else if (lemma.noun_gender_ == feminine) {
        os << "fem";
      } else if (lemma.noun_gender_ == neuter) {
        os << "neut";
      }
      os << "\n";
    } else if (lemma.pos_ == adj) {
      os << "adj" << "\n";
    } else {
      os << "verb" << " ";
      if (lemma.verb_aspect_ == perf) {
        os << "perf";
      } else {
        os << "imperf";
      }
      os << "\n";
    }
    for (size_t j = 0; j < lemma.forms_.getSize(); ++j) {
      WordForm wf = lemma.forms_[j];
      os << "\t" << wf.word_ << " ";
      if (wf.gender_ == masculine) {
        os << "masc" << " ";
      } else if (wf.gender_ == feminine) {
        os << "fem" << " ";
      } else if (wf.gender_ == neuter) {
        os << "neut" << " ";
      }
      if (wf.number_ == singular) {
        os << "sing" << " ";
      } else if (wf.number_ == plural) {
        os << "pl" << " ";
      }
      if (wf.case_ == nominative) {
        os << "nom" << " ";
      } else if (wf.case_ == genitive) {
        os << "gen" << " ";
      } else if (wf.case_ == dative) {
        os << "dat" << " ";
      } else if (wf.case_ == accusative) {
        os << "acc" << " ";
      } else if (wf.case_ == instrumental) {
        os << "ins" << " ";
      } else if (wf.case_ == prepositional) {
        os << "pre" << " ";
      }
      if (wf.tense_ == past) {
        os << "past" << " ";
      } else if (wf.tense_ == future) {
        os << "fut" << " ";
      } else if (wf.tense_ == present) {
        os << "pres" << " ";
      }
      if (wf.person_ == first) {
        os << "1" << " ";
      } else if (wf.person_ == second) {
        os << "2" << " ";
      } else if (wf.person_ == third) {
        os << "3" << " ";
      }
      os << "\n";
    }
    os << "\n";
  }
  return os;
}

void alekseev::Dictionary::add_lemma(const std::string & lemma, pos pos, gender noun_gender,
    aspect verb_aspect)
{
  Lemma l{lemma, Vector< WordForm >(), pos, noun_gender, verb_aspect};
  lemmas_.insert(lemma, l);
}

void alekseev::Dictionary::add_form(const std::string & lemma, const std::string & wordform,
    gender g, number n, case_ c, tense t, person p)
{
  WordForm wf(wordform, g, n, c, t, p);
  Lemma & l = lemmas_.at(lemma);
  l.forms_.pushBack(wf);
  forms_.insert(wordform, std::make_pair(lemma, l.forms_.getSize() - 1));
}

void alekseev::Dictionary::remove_lemma(const std::string & lemma)
{
  if (!lemmas_.contains(lemma)) {
    return;
  }
  Lemma l = lemmas_.at(lemma);
  lemmas_.remove(lemma);
  for (size_t i = 0; i < l.forms_.getSize(); ++i) {
    forms_.remove(l.forms_[i].word_);
  }
}

void alekseev::Dictionary::remove_form(const std::string & wordform)
{
  if (!forms_.contains(wordform)) {
    return;
  }
  std::pair< std::string, size_t > wf = forms_.at(wordform);
  lemmas_.at(wf.first).forms_.erase(wf.second);
  forms_.remove(wordform);
}

void alekseev::Dictionary::update_form(const std::string & old_wordform, const WordForm & new_form)
{
  if (!forms_.contains(old_wordform)) {
    return;
  }
  std::string lemma = forms_.at(old_wordform).first;
  remove_form(old_wordform);
  add_form(lemma, new_form.word_, new_form.gender_, new_form.number_, new_form.case_,
      new_form.tense_, new_form.person_);
}

size_t alekseev::Dictionary::size() const
{
  return lemmas_.size();
}
