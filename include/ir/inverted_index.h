#ifndef IR_INVERTED_INDEX_H
#define IR_INVERTED_INDEX_H

#include <unordered_map>
#include <vector>
#include <algorithm>

struct Entry
{
  uint index;
  uint position;

  Entry(uint index = 0, uint position = 0) : index(index), position(position) {}

  bool operator<(const Entry &other) const
  {
    return index < other.index;
  }
};

struct Posting
{
  uint doc;
  std::vector<Entry> entries;

  Posting(int doc = 0) : doc(doc) {}

  void add_entry(Entry &&entry)
  {
    entries.push_back(std::move(entry));
  }

  bool operator<(const Posting &other) const
  {
    return doc < other.doc;
  }

  void sort_entries()
  {
    std::sort(entries.begin(), entries.end());
  }
};

using Term = std::string;
using Postings = std::vector<Posting>;

using Keys = std::vector<Term>;
using Dictionary = std::unordered_map<Term, Postings>;

class InvertedIndex
{
  Dictionary m_dictionary;
  Keys m_keys;

private:
public:
  Posting &get_posting(const Term &term, int doc)
  {
    Postings &postings = m_dictionary[term];
    Postings::iterator it;

    for (it = postings.begin(); it != postings.end(); ++it)
      if (doc == it->doc)
        break;

    if (it == postings.end())
    {
      postings.push_back(Posting(doc));
      return postings.back();
    }

    return *it;
  }

  Dictionary &dictonary()
  {
    return m_dictionary;
  }

  void sort()
  {
    for (auto &pair : m_dictionary)
    {
      auto &postings = pair.second;
      std::sort(postings.begin(), postings.end());
      for (auto &posting : postings)
      {
        auto &entries = posting.entries;
        std::sort(entries.begin(), entries.end());
      }
    }

    sort_keys();
  }

  void sort_keys()
  {
    m_keys.reserve(m_dictionary.size());
    for (auto &it : m_dictionary)
      m_keys.push_back(it.first);

    std::sort(m_keys.begin(), m_keys.end());
  }

  std::vector<Term> &keys()
  {
    return m_keys;
  }
};

#endif