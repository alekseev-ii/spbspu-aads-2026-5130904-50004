#include "unified_io.h"

#include <string.h>
#include <iostream>
#include <locale>

alekseev::Console::Console()
{
#ifdef _WIN32
  _oldStdoutMode = _setmode(_fileno(stdout), _O_U16TEXT);
  _oldStdinMode = _setmode(_fileno(stdin), _O_U16TEXT);
  _oldStderrMode = _setmode(_fileno(stderr), _O_U16TEXT);
  _oldOutputCP = GetConsoleOutputCP();
  _oldInputCP = GetConsoleCP();
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
#else
  char * old = std::setlocale(LC_ALL, nullptr); if (old) {
    _oldLocale = strdup(old);
  } const char * locales[] = {"", "C.UTF-8", "en_US.UTF-8", "ru_RU.UTF-8"}; for (
    const char * loc: locales) {
    if (std::setlocale(LC_ALL, loc)) {
      break;
    }
  }
#endif
}

alekseev::Console::~Console()
{
#ifdef _WIN32
  _setmode(_fileno(stdout), _oldStdoutMode);
  _setmode(_fileno(stdin), _oldStdinMode);
  _setmode(_fileno(stderr), _oldStderrMode);
  SetConsoleOutputCP(_oldOutputCP);
  SetConsoleCP(_oldInputCP);
#else
  if (_oldLocale) {
    std::setlocale(LC_ALL, _oldLocale);
    free(const_cast< char * >(_oldLocale));
    _oldLocale = nullptr;
  }
#endif
}

std::wstring alekseev::Console::input() const
{
  std::wstring result;
  std::getline(std::wcin, result);
#ifdef _WIN32
  std::wcin.ignore();
#endif
  return result;
}
