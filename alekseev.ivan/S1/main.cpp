#include <iostream>
#include <limits>
#include "List.h"

namespace alekseev {
  template< class T >
  bool is_empty(const List< List< T > > & l);
  const size_t MAX_SIZE_T = std::numeric_limits< size_t >::max();
  template< class FwdIter >
  std::ostream & print(std::ostream & os, FwdIter beg, FwdIter end);
}

int main()
{
  alekseev::List< std::string > names;
  auto cur_name = names.before_begin();
  alekseev::List< alekseev::List< size_t > > seqs;
  auto cur_seq = seqs.before_begin();
  alekseev::List< size_t > sums;

  std::string name;
  while (std::cin >> name) {
    names.insert_after(cur_name, name);
    ++cur_name;
    seqs.insert_after(cur_seq, alekseev::List< size_t >());
    ++cur_seq;
    auto current = cur_seq->before_begin();
    auto cur_sum = sums.before_begin();

    int next_char = std::cin.get();
    while (std::cin && next_char != '\n' && next_char != EOF) {
      size_t n = 0;
      std::cin >> n;
      if (sums.size() == cur_seq->size()) {
        sums.insert_after(cur_sum, 0ull);
      }
      ++cur_sum;
      cur_seq->insert_after(current, n);
      ++current;
      if (alekseev::MAX_SIZE_T - n < *cur_sum) {
        std::cerr << "Overflow!\n";
        return 1;
      }
      *cur_sum += n;
      next_char = std::cin.get();
    }
  }

  if (names.empty()) {
    std::cout << "0\n";
    return 0;
  }
  alekseev::print(std::cout, names.begin(), names.end());

  while (true) {
    alekseev::List< size_t > buffer;
    auto last = buffer.before_begin();
    for (auto seq = seqs.begin(); seq != seqs.end(); ++seq) {
      if (!seq->empty()) {
        size_t n = seq->front();
        buffer.insert_after(last, n);
        ++last;
        seq->pop_front();
      }
    }
    if (buffer.empty()) {
      break;
    }
    alekseev::print(std::cout, buffer.begin(), buffer.end());
  }

  if (sums.empty()) {
    std::cout << "0\n";
  } else {
    alekseev::print(std::cout, sums.begin(), sums.end());
  }
}

template< class T >
bool alekseev::is_empty(const List< List< T > > & l)
{
  for (auto it = l.begin(); it != l.end(); ++it) {
    if (!it->empty()) {
      return false;
    }
  }
  return true;
}

template< class FwdIter >
std::ostream & alekseev::print(std::ostream & os, FwdIter beg, FwdIter end)
{
  if (beg == end) {
    return os;
  }
  os << *beg++;
  for (; beg != end; ++beg) {
    os << " " << *beg;
  }
  return os << "\n";
}
