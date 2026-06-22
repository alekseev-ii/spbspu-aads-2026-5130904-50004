#include "safety_math.h"

#include <stdexcept>
#include "queue.h"

long long alekseev::sum(long long a, long long b)
{
  if (a > 0 && b > 0) {
    if (a > MAX - b) {
      throw std::overflow_error("Sum overflow");
    }
  } else if (a < 0 && b < 0) {
    if (a < MIN - b) {
      throw std::underflow_error("Sum underflow");
    }
  }
  return a + b;
}

long long alekseev::sub(long long a, long long b)
{
  if (a > 0 && b < 0) {
    if (a > MAX + b) {
      throw std::overflow_error("Sub overflow");
    }
  } else if (a < 0 && b > 0) {
    if (a < MIN + b) {
      throw std::underflow_error("Sub underflow");
    }
  }
  return a - b;
}

long long alekseev::mul(long long a, long long b)
{
  if (!a || !b) {
    return 0;
  }
  if (a > 0 && b > 0) {
    if (a > MAX / b) {
      throw std::overflow_error("Mul overflow");
    }
  } else if (a < 0 && b < 0) {
    if (a < MAX / b) {
      throw std::overflow_error("Mul overflow");
    }
  } else if (a < 0 && b > 0) {
    if (a < MIN / b) {
      throw std::underflow_error("Mul underflow");
    }
  } else if (a > 0 && b < 0) {
    if (b < MIN / a) {
      throw std::underflow_error("Mul underflow");
    }
  }
  return a * b;
}

long long alekseev::div(long long a, long long b)
{
  if (!b) {
    throw std::logic_error("Division by zero");
  }
  return a / b;
}

long long alekseev::mod(long long a, long long b)
{
  if (!b) {
    throw std::logic_error("Division by zero");
  }
  return (a % b + b) % b;
}

long long alekseev::flip(long long a)
{
  bool negative = a < 0;
  if (negative) {
    a = -a;
  }
  Queue< long long > tmp;
  while (a) {
    tmp.push(a % 10);
    a /= 10;
  }
  long long res = 0;
  while (!tmp.empty()) {
    res = sum(mul(res, 10), tmp.front());
    tmp.pop();
  }
  return negative ? -res : res;
}
