#include "expressions_processing.h"

#include <stdexcept>
#include <cctype>
#include "stack.h"
#include "safety_math.h"

long long alekseev::count_from_string(const std::string & str_expr)
{
  QLCh postfix = infix_to_postfix(str_to_infix(str_expr));
  long long res = count_postfix(postfix);
  return res;
}

alekseev::QLCh alekseev::str_to_infix(const std::string & str_expr)
{
  QLCh res;
  size_t i = 0;
  while (i < str_expr.size()) {
    char current_char = str_expr[i++];
    List< char > cur;
    auto cur_tail = cur.before_begin();
    while (current_char != ' ' && i < str_expr.size()) {
      cur.insert_after(cur_tail, current_char);
      ++cur_tail;
      current_char = str_expr[i++];
    }
    if (current_char != ' ') {
      cur.insert_after(cur_tail, current_char);
      ++cur_tail;
    }
    if (cur.empty()) {
      continue;
    }

    char cfnd = cur.front();
    if (!is_operator(cfnd) && cfnd != '(' && cfnd != ')' && !is_number(cur)) {
      throw std::invalid_argument("Bad input: not a number or operator");
    }

    res.push(cur);
  }
  return res;
}

alekseev::QLCh alekseev::infix_to_postfix(QLCh infix)
{
  Stack< List< char > > stack;
  QLCh postfix;

  while (!infix.empty()) {
    List< char > current = infix.front();
    infix.pop();
    char first_char = current.front();
    if (first_char == '(') {
      stack.push(current);
    } else if (first_char == ')') {
      if (!stack.empty()) {
        while (stack.top().front() != '(') {
          postfix.push(stack.top());
          stack.pop();
          if (stack.empty()) {
            throw std::invalid_argument("Invalid expression");
          }
        }
        stack.pop();
      } else {
        throw std::invalid_argument("Invalid expression");
      }
    } else if (is_operator(first_char)) {
      if (!stack.empty()) {
        while (priority_of(stack.top().front()) >= priority_of(first_char)) {
          postfix.push(stack.top());
          stack.pop();
          if (stack.empty()) {
            break;
          }
        }
      }
      stack.push(current);
    } else {
      postfix.push(current);
    }
  }
  while (!stack.empty()) {
    if (stack.top().front() == '(') {
      throw std::invalid_argument("Invalid expression");
    }
    postfix.push(stack.top());
    stack.pop();
  }
  return postfix;
}

long long alekseev::count_postfix(QLCh postfix)
{
  Stack< long long > stack;
  while (!postfix.empty()) {
    List< char > current = postfix.front();

    if (is_operator(current.front())) {
      char op = current.front();
      if (op == '#') {
        if (stack.empty()) {
          throw std::invalid_argument("Invalid expression");
        }
        long long n = stack.top();
        stack.pop();
        stack.push(flip(n));
      } else {
        if (stack.size() < 2) {
          throw std::invalid_argument("Invalid expression");
        }
        long long b = stack.top();
        stack.pop();
        long long a = stack.top();
        stack.pop();
        stack.push(count(a, b, op));
      }
    } else {
      stack.push(ltoll(current));
    }
    postfix.pop();
  }
  if (stack.size() != 1) {
    throw std::invalid_argument("Invalid expression");
  }
  return stack.top();
}

short alekseev::priority_of(char op)
{
  if (op == '#') {
    return 3;
  } else if (op == '*' || op == '/' || op == '%') {
    return 2;
  } else if (op == '+' || op == '-') {
    return 1;
  }
  return 0;
}

bool alekseev::is_operator(char op)
{
  return op == '#' || op == '*' || op == '/' || op == '%' || op == '+' || op == '-';
}

bool alekseev::is_number(const List< char > & li)
{
  for (auto cur_ch = li.begin(); cur_ch != li.end(); ++cur_ch) {
    if (!isdigit(*cur_ch)) {
      return false;
    }
  }
  return true;
}

long long alekseev::ltoll(const List< char > & li)
{
  std::string res;
  for (auto cur_ch = li.begin(); cur_ch != li.end(); ++cur_ch) {
    res += *cur_ch;
  }
  return stoll(res);
}

long long alekseev::count(long long a, long long b, char op)
{
  if (op == '*') {
    return mul(a, b);
  } else if (op == '/') {
    return div(a, b);
  } else if (op == '%') {
    return mod(a, b);
  } else if (op == '+') {
    return sum(a, b);
  } else if (op == '-') {
    return sub(a, b);
  }
  throw std::invalid_argument("Invalid operation");
}
