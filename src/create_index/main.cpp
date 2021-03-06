#include "ir/common.h"
#include "ir/scanner.h"
#include "ir/inverted_index.h"
#include <string>
#include <vector>
#include <iostream>
#include <cctype>

int main(int argc, char const *argv[])
{
  if (argc < 5)
  {
    fprintf(stderr, "Usage: %s TOTAL_DOCS DOCS_DIR OUTPUT_DIR CODING_TYPE\n", argv[0]);
    return EXIT_FAILURE;
  }

  InvertedIndex inverted_index;

  int total_docs = std::atoi(argv[1]);
  int coding_type = std::atoi(argv[4]);
  std::string dir(argv[2]);
  std::string outdir(argv[3]);

  try
  {
    char *buffer = nullptr;
    long size;

    for (int doc = 0; doc < total_docs; ++doc)
    {
      std::string path = dir + '/' + std::to_string(doc) + ".txt";

      Scanner sc;
      sc.load(path.c_str());

      auto &tokens = sc.scan(1);
      unsigned int size = (unsigned int)tokens.size();

      for (auto &token : tokens)
      {
        Posting &posting = inverted_index.get_posting(token.lexeme, doc);
        posting.add_entry(Entry(token.index, token.position));
      }
    }

    inverted_index.sort();

    inverted_index.compress(outdir, coding_type);
  }
  catch (std::runtime_error &e)
  {
    fprintf(stderr, "Error: %s\n", e.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}