#ifndef SAFETY_MATH_H
#define SAFETY_MATH_H

#include <limits>

namespace alekseev {
  long long sum(long long a, long long b);
  long long sub(long long a, long long b);
  long long mul(long long a, long long b);
  long long div(long long a, long long b);
  long long mod(long long a, long long b);
  long long flip(long long a);

  const long long MAX = std::numeric_limits< long long >::max();
  const long long MIN = std::numeric_limits< long long >::min();
}

#endif
