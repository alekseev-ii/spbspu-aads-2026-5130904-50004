#include <iostream>
#include <limits>
#include "List.h"

namespace alekseev {
  template< class T >
  bool is_empty(const List< List< T > > & l);
  const size_t MAX_SIZE_T = std::numeric_limits< size_t >::max();
  size_t safety_sum(size_t a, size_t b);
}

int main()
{
  alekseev::List< std::string > names;
  auto cur_name = names.before_begin();
  alekseev::List< alekseev::List< size_t > > seqs;
  auto cur_seq = seqs.before_begin();

  std::string name;
  while (std::cin >> name) {
    names.insert_after(cur_name, name);
    ++cur_name;
    seqs.insert_after(cur_seq, alekseev::List< size_t >());
    ++cur_seq;
    auto current = cur_seq->before_begin();

    int next_char = std::cin.get();
    while (std::cin && next_char != '\n' && next_char != EOF) {
      size_t n = 0;
      std::cin >> n;
      cur_seq->insert_after(current, n);
      ++current;
      next_char = std::cin.get();
    }
  }

  if (names.empty()) {
    std::cout << "0\n";
    return 0;
  }
  alekseev::List< std::string >::LCIter cname = names.begin();
  std::cout << *(cname++);
  for (; cname != names.end(); ++cname) {
    std::cout << " " << *cname;
  }
  std::cout << "\n";

  alekseev::List< size_t > sums;
  auto cur_sum = sums.before_begin();
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
    sums.insert_after(cur_sum, 0ull);
    ++cur_sum;
    last = buffer.begin();
    try {
      *cur_sum = alekseev::safety_sum(*cur_sum, *last);
      std::cout << *last++;
      for (; last != buffer.end(); ++last) {
        std::cout << " " << *last;
        *cur_sum = alekseev::safety_sum(*cur_sum, *last);
      }
      std::cout << "\n";
    } catch (std::overflow_error & e) {
      std::cout << "overflow error\n";
      return 1;
    }
  }

  if (sums.empty()) {
    std::cout << "0\n";
    return 0;
  }
  auto s = sums.begin();
  std::cout << *(s++);
  for (; s != sums.end(); ++s) {
    std::cout << " " << *s;
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

size_t alekseev::safety_sum(size_t a, size_t b)
{
  if (MAX_SIZE_T - a < b) {
    throw std::overflow_error("Overflow!");
  }
  return a + b;
}
