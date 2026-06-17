#ifndef CONSOLE_SETUP_H
#define CONSOLE_SETUP_H

#include <string>

#ifdef _WIN32
#include <fcntl.h>
#include <windows.h>
#endif

namespace alekseev {
  struct Console
  {
    Console();
    ~Console();
    Console(const Console &) = delete;
    Console & operator=(const Console &) = delete;
    Console(Console &&) = delete;
    Console & operator=(Console &&) = delete;

    std::wstring input() const;

    private:
#ifdef _WIN32
      int _oldStdoutMode, _oldStdinMode, _oldStderrMode;
      UINT _oldOutputCP, _oldInputCP;
#else
      const char * _oldLocale = nullptr;
#endif
  };
}
#endif
