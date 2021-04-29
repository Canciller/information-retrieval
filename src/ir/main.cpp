#include <iostream>
#include <limits>
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

  size_t distance = v.offset - u.offset;
  if (n - 1 == distance)
    return SearchInterval(u, v);

  return next_phrase(tokens, u);
}

int main(int argc, char const *argv[])
{
  if (argc < 3)
  {
    fprintf(stderr, "Usage: %s INDEX_DIR CODING_TYPE\n", argv[0]);
    return EXIT_FAILURE;
  }

  std::string indir = argv[1];
  int coding_type = std::atoi(argv[2]);

  InvertedIndexSearch in;

  in.set_coding_type(coding_type);
  in.set_indir(indir);

  std::string phrase = "The closest fossil group to the Milky Way is NGC 6482";
  auto tokens = in.tokenize_all(phrase);
  std::cout << "Size: " << tokens.size() << "\n";
  for (auto &token : tokens)
    std::cout << token.lexeme << " ";
  std::cout << "\n";

  SearchInterval result = in.next_phrase(tokens);

  std::cout << "Document: " << result.start.doc << "\n"
            << "Offset: " << result.start.offset << ", " << result.end.offset << "\n"
            << "Position: " << result.start.position << ", " << result.end.position << "\n";

  return EXIT_SUCCESS;
}
