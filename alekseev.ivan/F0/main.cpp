#include "dictionary.h"
#include <fstream>

int main()
{
  std::string inp = "inp.txt";
  std::string outp = "outp.txt";
  try {
    alekseev::Dictionary dic(inp);
    std::ofstream out(outp);
    dic.write(out);
  } catch (std::exception & e) {
    std::cout << e.what() << "\n";
    return 1;
  }
}
