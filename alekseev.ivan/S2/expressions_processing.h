#ifndef EXPRESSIONS_PROCESSING_H
#define EXPRESSIONS_PROCESSING_H

#include <string>
#include "queue.h"

namespace alekseev {
  using QLCh = Queue< List< char > >;
  long long count_from_string(const std::string & str_expr);
  QLCh str_to_infix(const std::string & str_expr);
  QLCh infix_to_postfix(const QLCh & infix);
  long long count_postfix(const QLCh & postfix);
  short priority_of(char op);
  bool is_operator(char op);
  bool is_number(const List< char > & li);
  long long ltoll(const List< char > & li);
  long long count(long long a, long long b, char op);
}

#endif
