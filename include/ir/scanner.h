#ifndef IR_SCANNER_H
#define IR_SCANNER_H

#include <vector>
#include <string>
#include <cstring>
#include "ir/common.h"

struct Token
{
  std::string lexeme;
  unsigned int position;
  unsigned int index;
};

using TokenArray = std::vector<Token>;

class Scanner
{
  TokenArray m_tokens;

  char *m_str = nullptr;
  int m_i = 0;
  int m_index = 0;
  int m_start = 0;
  char m_c;

private:
  bool isAlphanumeric(char c)
  {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_';
  }

  void ignoreLines(int n)
  {
    int count = 0;

    while (count != n && m_str[m_i] != '\0')
    {
      if (m_str[m_i] == '\n')
        ++count;

      m_i++;
    }

    m_start = m_i;
  }

  void add_token()
  {
    long size = m_i - m_start;
    if (size > 0)
    {
      std::string lexeme;

      for (int i = 0; i < size; ++i)
        lexeme.push_back(toupper(*(m_str + m_start + i)));

      Token token;
      token.lexeme = std::move(lexeme);
      token.position = m_start;
      token.index = m_index;

      m_tokens.push_back(token);
      m_index++;
    }

    m_start = m_i + 1;
  }

public:
  ~Scanner()
  {
    if (m_str)
      delete[] m_str;
  }

  void load(const char *path)
  {
    read_file(path, &m_str, NULL, true);
  }

  TokenArray &scan()
  {
    ignoreLines(1);

    std::cout << "Document length: " << strlen(m_str) << "\n";
    std::cout << "Content start index: " << m_start << "\n";

    while (m_str[m_i] != '\0')
    {
      char c = m_str[m_i];

      if (!isAlphanumeric(c))
        add_token();

      m_i++;
    }

    return m_tokens;
  }
};

#endif