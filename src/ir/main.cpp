#include <iostream>
#include <limits>
#include <chrono>
#include "ir/inverted_index_search.h"

const long DocOffsetPair::PINF = std::numeric_limits<long>::max();
const long DocOffsetPair::NINF = std::numeric_limits<long>::min();

DocOffsetPair DocOffsetPair::PINF_PAIR = DocOffsetPair(DocOffsetPair::PINF, DocOffsetPair::PINF);
DocOffsetPair DocOffsetPair::NINF_PAIR = DocOffsetPair(DocOffsetPair::NINF, DocOffsetPair::NINF);

void InvertedIndexSearch::binary_search(const std::string &lexeme, size_t &low, size_t &high, DocOffsetPair current, bool prev)
{
  auto &list = get(lexeme);

  size_t mid = 0;
  while (high - low > 1)
  {
    mid = (high + low) / 2;

    if (prev)
    {
      if (list[mid] >= current)
        high = mid;
      else
        low = mid;
    }
    else
    {
      if (list[mid] <= current)
        low = mid;
      else
        high = mid;
    }
  }
}

DocOffsetPair InvertedIndexSearch::next(const std::string &lexeme, DocOffsetPair current)
{
  auto &list = get(lexeme);
  if (list.size() == 0 || list.back() <= current)
    return DocOffsetPair::PINF_PAIR;

  if (list.front() > current)
    return list.front();

  size_t low = 0,
         high = list.size() - 1;

  binary_search(lexeme, low, high, current);

  return list[high];
}

DocOffsetPair InvertedIndexSearch::prev(const std::string &lexeme, DocOffsetPair current)
{
  auto &list = get(lexeme);
  if (list.size() == 0 || list.front() >= current)
    return DocOffsetPair::NINF_PAIR;

  if (list.back() < current)
    return list.back();

  size_t low = 0,
         high = list.size() - 1;

  binary_search(lexeme, low, high, current, true);

  return list[low];
}

DocOffsetPair InvertedIndexSearch::first(const std::string &lexeme)
{
  return next(lexeme);
}

DocOffsetPair InvertedIndexSearch::last(const std::string &lexeme)
{
  return prev(lexeme);
}

SearchInterval InvertedIndexSearch::next_phrase(const TokenArray &tokens, DocOffsetPair position)
{
  static SearchInterval infinf = SearchInterval(DocOffsetPair::PINF_PAIR, DocOffsetPair::PINF_PAIR);

  size_t n = tokens.size();

  if (n == 0)
    return infinf;

  if (n == 1)
  {
    Token token = tokens[0];
    DocOffsetPair p = next(token.lexeme, position);
    return SearchInterval(p, p);
  }

  ssize_t i = 0;
  DocOffsetPair v = position;

  for (; i < n; ++i)
    v = next(tokens[i].lexeme, v);

  if (v == DocOffsetPair::PINF_PAIR)
    return infinf;

  DocOffsetPair u = v;

  for (i = n - 2; i >= 0; --i)
    u = prev(tokens[i].lexeme, u);

  if (u.doc == v.doc)
  {
    size_t distance = v.offset - u.offset;
    if (n - 1 == distance)
      return SearchInterval(u, v);
  }

  return next_phrase(tokens, u);
}

void InvertedIndexSearch::search_all(SearchResult &result, const TokenArray &tokens, DocOffsetPair position)
{
  auto interval = next_phrase(tokens, position);
  if (interval.end != DocOffsetPair::PINF_PAIR)
  {
    result.push_back(interval);
    return search_all(result, tokens, interval.end);
  }

  return;
}

InvertedIndexSearch in;
const char *docsdir;

void find_phrase(const std::string &phrase)
{
  auto tokens = in.tokenize_all(phrase);
  std::cout << "Size: " << tokens.size() << "\n";
  std::cout << "Tokens: ";
  for (auto &token : tokens)
    std::cout << token.lexeme << " ";
  std::cout << "\n---------------------------------------\n";

  SearchResult results;
  in.search_all(results, tokens);

  if (results.empty())
  {
    std::cout << "No results for query: " << phrase << "\n";
    std::cout << "---------------------------------------\n";
    return;
  }

  for (auto &result : results)
  {
    uint doc = result.start.doc;

    uint extra_chars = 50;

    if (doc != DocOffsetPair::PINF)
    {
      uint start = result.start.position;
      uint end = result.end.position + tokens.back().lexeme.size();

      uint term_start = start;
      uint term_end = end;

      std::cout << "Document: " << doc << "\n"
                << "Offset: " << result.start.offset << ", " << result.end.offset << "\n"
                << "Position: " << start << ", " << end << "\n";

      if (start >= extra_chars)
        start -= extra_chars;
      else
        start = 0;

      char *buffer = nullptr;
      long size;

      std::string path = docsdir;
      if (!path.empty())
      {
        path += "/" + std::to_string(doc) + ".txt";

        read_file(path.c_str(), &buffer, &size);

        if (end >= size - extra_chars - 1)
          end = size - 1;
        else
          end += extra_chars;

        std::cout << "\n";
        std::cout << "Extract from document: " << path << "\n";
        fwrite(buffer + start, 1, term_start - start, stdout);
        printf("\e[32m");
        fwrite(buffer + term_start, 1, term_end - term_start, stdout);
        printf("\e[0m");
        fwrite(buffer + term_end, 1, end - term_end, stdout);
        std::cout << "\n---------------------------------------\n";

        delete[] buffer;
      }
    }
  }

  std::cout << results.size() << " results found for query: " << phrase << "\n";
  std::cout << "---------------------------------------\n";
}

int main(int argc, char const *argv[])
{
  if (argc < 4)
  {
    fprintf(stderr, "Usage: %s INDEX_DIR DOCS_DIR CODING_TYPE\n", argv[0]);
    return EXIT_FAILURE;
  }

  std::string indir = argv[1];
  docsdir = argv[2];
  int coding_type = std::atoi(argv[3]);

  in.set_coding_type(coding_type);
  in.set_indir(indir);

  std::string line;
  while (true)
  {
    std::cout << "Search: ";
    if (!std::getline(std::cin, line))
      break;
    std::cout << "\n";

    if (line.empty())
    {
      std::cout << "Phrase cannot be empty\n";
      continue;
    }

    find_phrase(line);
    std::cout << "\n";
  }

  return EXIT_SUCCESS;
}
