#ifndef IR_INVERTED_INDEX_SEARCH_H
#define IR_INVERTED_INDEX_SEARCH_H

#include "ir/inverted_index.h"
#include <unordered_map>

struct DocOffsetPair
{
  static const long PINF;
  static const long NINF;

  static DocOffsetPair PINF_PAIR;
  static DocOffsetPair NINF_PAIR;

  long doc;
  long offset;
  uint position;

  DocOffsetPair(long doc = 0, long offset = 0, uint position = 0) : doc(doc), offset(offset), position(position) {}

  bool operator<(const DocOffsetPair &other) const
  {
    bool result = doc < other.doc;

    if (doc == other.doc)
      result = offset < other.offset;

    return result;
  }

  bool operator<=(const DocOffsetPair &other) const
  {
    bool result = doc <= other.doc;

    if (doc == other.doc)
      result = offset <= other.offset;

    return result;
  }

  bool operator>(const DocOffsetPair &other) const
  {
    bool result = doc > other.doc;

    if (doc == other.doc)
      result = offset > other.offset;

    return result;
  }

  bool operator>=(const DocOffsetPair &other) const
  {
    bool result = doc >= other.doc;

    if (doc == other.doc)
      result = offset >= other.offset;

    return result;
  }

  bool operator==(const DocOffsetPair &other) const
  {
    return doc == other.doc && offset == other.offset;
  }
};

struct SearchInterval
{
  DocOffsetPair start;
  DocOffsetPair end;

  SearchInterval() {}

  SearchInterval(DocOffsetPair start, DocOffsetPair end) : start(start), end(end) {}
};

using SearchResult = std::vector<SearchInterval>;

class InvertedIndexSearch : public InvertedIndex
{
  std::unordered_map<std::string, std::vector<DocOffsetPair>> m_pairs;

protected:
  using TermRef = const Term &;

  /**
   * @brief 
   * 
   * @param term 
   * @param low 
   * @param high 
   * @param current 
   */
  void binary_search(const std::string &lexeme, size_t &low, size_t &high, DocOffsetPair pair, bool prev = false);

  std::vector<DocOffsetPair> toDocOffsetPairs(Postings &postings)
  {
    std::vector<DocOffsetPair> pairs;

    for (auto &posting : postings)
    {
      auto doc = posting.doc;
      for (auto &entry : posting.entries)
        pairs.push_back(
            DocOffsetPair(doc, entry.index, entry.position));
    }

    return pairs;
  }

  std::vector<DocOffsetPair> &get(const std::string &lexeme)
  {
    std::vector<DocOffsetPair> pairs;

    const auto &it = m_pairs.find(lexeme);
    if (it == m_pairs.end())
    {
      Postings &postings = get_with_lexeme(lexeme);
      m_pairs[lexeme] = toDocOffsetPairs(postings);
      clear_postings(lexeme);
    }

    return m_pairs[lexeme];
  }

public:
  /**
   * @brief Construct a new Inverted Index object
   */
  InvertedIndexSearch() = default;

  /**
   * @brief Destroy the Inverted Index object
   */
  virtual ~InvertedIndexSearch() = default;

  DocOffsetPair next(const std::string &lexeme, DocOffsetPair current = DocOffsetPair::NINF_PAIR);

  DocOffsetPair prev(const std::string &lexeme, DocOffsetPair current = DocOffsetPair::PINF_PAIR);

  DocOffsetPair first(const std::string &lexeme);

  DocOffsetPair last(const std::string &lexeme);

  SearchInterval next_phrase(const TokenArray &tokens, DocOffsetPair position = DocOffsetPair::NINF_PAIR);
};

#endif