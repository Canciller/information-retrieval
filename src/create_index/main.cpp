#include "ir/common.h"
#include "ir/scanner.h"
#include "ir/inverted_index.h"
#include <string>
#include <vector>
#include <iostream>
#include <cctype>

int main(int argc, char const *argv[])
{
  if (argc < 4)
  {
    fprintf(stderr, "Usage: %s TOTAL_DOCS DOCS_DIR CODING_TYPE\n", argv[0]);
    return EXIT_FAILURE;
  }

  std::string dir(argv[2]);
  int total_docs = std::atoi(argv[1]);
  int coding_type = std::atoi(argv[3]);

  try
  {
    char *buffer = nullptr;
    long size;

    for (int i = 0; i < total_docs; ++i)
    {
      std::string path = dir + '/' + std::to_string(i) + ".txt";

      std::cout << "Document path: " << path << "\n";
      std::cout << "Document id: " << i << "\n";

      Scanner sc;
      sc.load(path.c_str());

      auto &tokens = sc.scan();
      /*
      std::cout << "|";
      for (auto &token : tokens)
        std::cout << token.lexeme << ":" << token.index << "|";
      std::cout << "\n\n";
      */
    }
  }
  catch (std::runtime_error &e)
  {
    fprintf(stderr, "Error: %s\n", e.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}