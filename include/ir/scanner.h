#ifndef IR_SCANNER_H
#define IR_SCANNER_H

#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include "ir/common.h"

struct Token
{
  std::string lexeme;
  unsigned int position;
  unsigned int index;

  bool operator<(const Token &token) const
  {
    return lexeme < token.lexeme;
  }
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
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
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

  void load(const std::string &str)
  {
    size_t size = str.size();
    m_str = new char[size + 1];
    strcpy(m_str, str.c_str());
  }

  TokenArray &scan(int ignore = 0, bool sort = true)
  {
    ignoreLines(ignore);

    //std::cout << "Document length: " << strlen(m_str) << "\n";
    //std::cout << "Content start index: " << m_start << "\n";

    while (m_str[m_i] != '\0')
    {
      char c = m_str[m_i];

      if (!isAlphanumeric(c))
        add_token();

      m_i++;
    }

    if (sort)
      std::sort(m_tokens.begin(), m_tokens.end());

    return m_tokens;
  }
};

#endif