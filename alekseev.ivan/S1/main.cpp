#include <iostream>
#include <limits>
#include "List.h"

namespace alekseev {
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

  alekseev::List< size_t > sums;
  auto cur_sum = sums.before_begin();
  alekseev::List< alekseev::List< size_t > > bufs;
  auto last_buf = bufs.before_begin();
  while (true) {
    bufs.insert_after(last_buf, alekseev::List< size_t >());
    ++last_buf;
    auto last = last_buf->before_begin();

    for (auto seq = seqs.begin(); seq != seqs.end(); ++seq) {
      if (!seq->empty()) {
        if (last_buf->empty()) {
          sums.insert_after(cur_sum, 0ull);
          ++cur_sum;
        }
        size_t n = seq->front();
        if (alekseev::MAX_SIZE_T - n < *cur_sum) {
          std::cerr << "Overflow!\n";
          return 1;
        }
        *cur_sum += n;
        last_buf->insert_after(last, n);
        ++last;
        seq->pop_front();
      }
    }
    if (last_buf->empty()) {
      break;
    }
  }

  if (names.empty()) {
    std::cout << "0\n";
    return 0;
  }
  alekseev::print(std::cout, names.begin(), names.end());
  for (auto buf = bufs.begin(); buf != bufs.end(); ++buf) {
    alekseev::print(std::cout, buf->begin(), buf->end());
  }
  if (sums.empty()) {
    std::cout << "0\n";
  } else {
    alekseev::print(std::cout, sums.begin(), sums.end());
  }
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
