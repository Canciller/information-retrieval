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

  InvertedIndex inverted_index;

  std::string dir(argv[2]);
  int total_docs = std::atoi(argv[1]);
  int coding_type = std::atoi(argv[3]);

  try
  {
    char *buffer = nullptr;
    long size;

    for (int doc = 0; doc < total_docs; ++doc)
    {
      std::string path = dir + '/' + std::to_string(doc) + ".txt";

      Scanner sc;
      sc.load(path.c_str());

      auto &tokens = sc.scan();
      unsigned int size = (unsigned int)tokens.size();

      for (auto &token : tokens)
      {
        Posting &posting = inverted_index.get_posting(token.lexeme, doc);
        posting.add_entry(Entry(token.index, token.position));
      }
    }

    inverted_index.sort();

    Dictionary &dic = inverted_index.dictonary();
    Keys &keys = inverted_index.keys();

    Dictionary::iterator it;

    for (auto &key : keys)
    {
      auto &postings = dic[key];
      std::cout << key << " -> ";

      for (auto &posting : postings)
      {
        std::cout << "{ ";
        std::cout << posting.doc << ", ";

        auto &entries = posting.entries;
        for (auto &entry : entries)
        {
          std::cout << entry.index << ":" << entry.position << " ";
        }

        std::cout << "} ";
      }

      std::cout << "\n";
    }
  }
  catch (std::runtime_error &e)
  {
    fprintf(stderr, "Error: %s\n", e.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}