#include "expressions_processing.h"

#include <stdexcept>
#include <cctype>
#include "stack.h"
#include "safety_math.h"

long long alekseev::count_from_string(const std::string & str_expr)
{
  Queue< List< char > * > infix = str_to_infix(str_expr);
  Queue< List< char > * > postfix;
  try {
    postfix = infix_to_postfix(infix);
  } catch (...) {
    clear_QLCh(infix);
    throw;
  }
  clear_QLCh(infix);
  long long res = 0;
  try {
    res = count_postfix(postfix);
  } catch (...) {
    clear_QLCh(postfix);
    throw;
  }
  clear_QLCh(postfix);
  return res;
}

alekseev::Queue< alekseev::List< char > * > alekseev::str_to_infix(const std::string & str_expr)
{
  QLCh res;
  size_t i = 0;
  while (i < str_expr.size()) {
    char current_char = str_expr[i++];
    List< char > * cur_fake = nullptr;
    try {
      cur_fake = fake< char >();
      List< char > * cur_tail = cur_fake;
      while (current_char != ' ' && i < str_expr.size()) {
        cur_tail = insert_after(cur_tail, current_char);
        current_char = str_expr[i++];
      }
      if (current_char != ' ') {
        insert_after(cur_tail, current_char);
      }
      if (cur_fake->next == cur_fake) {
        rmfake(cur_fake);
        continue;
      }

      char cfnd = cur_fake->next->data;
      if (!is_operator(cfnd) && cfnd != '(' && cfnd != ')' && !is_number(cur_fake)) {
        throw std::invalid_argument("Bad input: not a number or operator");
      }

      res.push(cur_fake);
    } catch (...) {
      clear(cur_fake->next, cur_fake);
      rmfake(cur_fake);
      clear_QLCh(res);
      throw;
    }
  }
  return res;
}

alekseev::QLCh alekseev::infix_to_postfix(QLCh infix)
{
  Stack< List< char > * > stack;
  QLCh postfix;
  List< char > * current = nullptr;
  try {
    while (!infix.empty()) {
      current = deep_copy(infix.front());
      infix.pop();
      char first_char = current->next->data;
      if (first_char == '(') {
        stack.push(current);
        current = nullptr;
      } else if (first_char == ')') {
        if (!stack.empty()) {
          while (stack.top()->next->data != '(') {
            postfix.push(stack.top());
            stack.pop();
            if (stack.empty()) {
              throw std::invalid_argument("Invalid expression");
            }
          }
          List< char > * tmp = stack.top();
          stack.pop();
          clear(tmp->next, tmp);
          rmfake(tmp);
        } else {
          throw std::invalid_argument("Invalid expression");
        }
        clear(current->next, current);
        rmfake(current);
        current = nullptr;
      } else if (is_operator(first_char)) {
        if (!stack.empty()) {
          while (priority_of(stack.top()->next->data) >= priority_of(first_char)) {
            postfix.push(stack.top());
            stack.pop();
            if (stack.empty()) {
              break;
            }
          }
        }
        stack.push(current);
        current = nullptr;
      } else {
        postfix.push(current);
        current = nullptr;
      }
    }
    while (!stack.empty()) {
      if (stack.top()->next->data == '(') {
        throw std::invalid_argument("Invalid expression");
      }
      postfix.push(stack.top());
      stack.pop();
    }
  } catch (...) {
    while (!stack.empty()) {
      List< char > * tmp = stack.top();
      stack.pop();
      clear(tmp->next, tmp);
      rmfake(tmp);
    }
    clear_QLCh(postfix);
    if (current) {
      clear(current->next, current);
      rmfake(current);
    }
    throw;
  }
  return postfix;
}

long long alekseev::count_postfix(QLCh postfix)
{
  Stack< long long > stack;
  while (!postfix.empty()) {
    List< char > * current = postfix.front();
    postfix.pop();
    if (is_operator(current->next->data)) {
      char op = current->next->data;
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

bool alekseev::is_number(List< char > * li)
{
  List< char > * current = li->next;
  while (current != li) {
    if (!isdigit(current->data)) {
      return false;
    }
    current = current->next;
  }
  return true;
}

long long alekseev::ltoll(List< char > * li)
{
  List< char > * current = li->next;
  std::string res;
  while (current != li) {
    res += current->data;
    current = current->next;
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

void alekseev::clear_QLCh(QLCh & q)
{
  while (!q.empty()) {
    List< char > * tmp = q.front();
    q.pop();
    clear(tmp->next, tmp);
    rmfake(tmp);
  }
}
