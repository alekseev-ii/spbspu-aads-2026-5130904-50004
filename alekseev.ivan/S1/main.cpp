#include <iostream>
#include <limits>
#include "../common/ListIterators.h"

namespace alekseev {
  using PStr_ull = std::pair< std::string, List< size_t > * >;
  const size_t MAX_SIZE_T = std::numeric_limits< size_t >::max();
  void destroy_matter_iter(LIter< PStr_ull > matter_iter);
}

int main()
{
  alekseev::List< alekseev::PStr_ull > * matter_list = alekseev::fake< alekseev::PStr_ull >();
  alekseev::LIter< alekseev::PStr_ull > matter_iter = alekseev::before_begin(matter_list);
  size_t matter_size = 0;
  alekseev::List< size_t > * sizes = nullptr;
  try {
    sizes = alekseev::fake< size_t >();
  } catch (const std::bad_alloc & e) {
    alekseev::rmfake(matter_list);
  }
  alekseev::LIter< size_t > sizes_iter = alekseev::before_begin(sizes);

  std::string name;
  while (std::cin >> name && !std::cin.eof()) {
    if (std::cin.fail()) {
      alekseev::destroy_matter_iter(matter_iter);
      alekseev::destroy(++sizes_iter);
      std::cerr << "bad input\n";
      return 1;
    }
    alekseev::List< size_t > * some_list = nullptr;
    try {
      some_list = alekseev::fake< size_t >();
    } catch (const std::bad_alloc & e) {
      alekseev::destroy_matter_iter(matter_iter);
      alekseev::destroy(++sizes_iter);
      std::cerr << e.what() << "\n";
      return 1;
    }
    alekseev::LIter< size_t > some_iter = alekseev::before_begin(some_list);

    try {
      size_t number = 0;
      size_t current_size = 0;
      while (true) {
        while (std::cin.peek() == ' ') {
          std::cin.ignore();
        }
        if (std::cin.peek() == '\n' || std::cin.peek() == EOF) {
          break;
        }
        std::cin >> number;
        if (std::cin.fail()) {
          alekseev::destroy_matter_iter(matter_iter);
          alekseev::destroy(++sizes_iter);
          alekseev::destroy(++some_iter);
          std::cerr << "bad input\n";
          return 1;
        }
        some_iter = alekseev::insert_after(some_iter, number);
        ++current_size;
      }
      alekseev::PStr_ull tmp(name, some_list);
      alekseev::insert_after< alekseev::PStr_ull >(matter_iter, tmp);
      ++matter_size;
      alekseev::insert_after(sizes_iter, current_size);
    } catch (const std::bad_alloc & e) {
      alekseev::destroy_matter_iter(matter_iter);
      alekseev::destroy(++sizes_iter);
      alekseev::destroy(++some_iter);
      std::cerr << e.what() << "\n";
      return 1;
    }
  }
  if (matter_size == 0) {
    std::cout << "0\n";
    alekseev::destroy_matter_iter(matter_iter);
    alekseev::destroy(++sizes_iter);
    return 0;
  }

  ++matter_iter;
  alekseev::LCIter< size_t > * iterators = nullptr;
  alekseev::List< size_t > * sums = nullptr;
  try {
    iterators = new alekseev::LCIter< size_t >[matter_size];

    ++matter_iter;
    std::cout << matter_iter->first;
    iterators[0] = alekseev::before_begin(matter_iter->second);
    for (size_t i = 1; i < matter_size; ++i) {
      ++matter_iter;
      std::cout << matter_iter->first;
      iterators[i] = alekseev::before_begin(matter_iter->second);
    }
    std::cout << "\n";

    sums = alekseev::fake< size_t >();
  } catch (const std::bad_alloc & e) {
    alekseev::destroy_matter_iter(matter_iter);
    delete[] iterators;
    alekseev::destroy(++sizes_iter);
    std::cerr << e.what() << "\n";
    return 1;
  }
  alekseev::LIter< size_t > sums_iter = alekseev::before_begin(sums);

  size_t j = 0;
  bool is_smth_printed = true;
  bool sums_error = false;
  try {
    while (is_smth_printed) {
      is_smth_printed = false;

      ++sizes_iter;
      alekseev::insert_after(sums_iter, 0);
      bool first = true;
      for (size_t i = 0; i < matter_size; ++i) {
        if (j < *(++sizes_iter)) {
          size_t n = *(++iterators[i]);
          if (first) {
            first = false;
          } else {
            std::cout << " ";
          }
          std::cout << n;

          if (!sums_error && alekseev::MAX_SIZE_T - n > *sums_iter) {
            *sums_iter += n;
          } else {
            sums_error = true;
          }
          is_smth_printed = true;
        }
      }
      if (is_smth_printed) {
        ++j;
        std::cout << "\n";
      }
    }
    if (sums_error) {
      throw std::overflow_error("error while counting sums");
    }
  } catch (const std::exception & e) {
    alekseev::destroy_matter_iter(matter_iter);
    delete[] iterators;
    alekseev::destroy(++sizes_iter);
    alekseev::destroy(++sums_iter);
    std::cerr << e.what() << "\n";
    return 1;
  }

  if (!j) {
    std::cout << "0\n";
  } else {
    ++sums_iter;
    std::cout << *(++sums_iter);
    for (size_t i = 1; i < j; ++i) {
      std::cout << " " << *(++sums_iter);
    }
    std::cout << "\n";
  }
  alekseev::destroy_matter_iter(matter_iter);
  delete[] iterators;
  alekseev::destroy(++sizes_iter);
  alekseev::destroy(++sums_iter);
}

void alekseev::destroy_matter_iter(LIter< PStr_ull > matter_iter)
{
  LIter< PStr_ull > start = ++matter_iter;
  while (++start != matter_iter) {
    List< size_t > * li = start->second;
    clear(li->next, li);
    rmfake(li);
  }
  destroy(matter_iter);
}
