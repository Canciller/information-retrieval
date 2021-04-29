#ifndef IR_INVERTED_INDEX_H
#define IR_INVERTED_INDEX_H

#include "ir/common.h"
#include "ir/scanner.h"
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

using Entries = std::vector<Entry>;

struct Posting
{
  uint doc;
  Entries entries;

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

  uint size()
  {
    return entries.size() * 2 + 2;
  }
};

using Term = std::string;
using Postings = std::vector<Posting>;

using Keys = std::vector<Term>;
using Dictionary = std::unordered_map<Term, Postings>;
using Loaded = std::unordered_map<Term, bool>;

class InvertedIndex
{
  Dictionary m_dictionary;
  Loaded m_loaded;
  Keys m_keys;

  std::string indir;
  int coding_type = NULC;

private:
  Postings get_decompressed_postings(const std::string &lexeme)
  {
    using namespace ir;

    const char *extension = get_extension_for_coding(coding_type);
    if (!extension)
      throw std::runtime_error("Unknown coding type");

    std::string path = indir + "/" + lexeme + "." + extension;
    Decompressed dec;
    try
    {
      dec.init(path.c_str(), coding_type);
    }
    catch (std::runtime_error &e)
    {
      return Postings();
    }

    uint *output = dec.decompress();
    uint size = dec.size();

    Postings postings;

    for (uint i = 0; i < size;)
    {
      uint doc = output[i];
      i++;

      uint len = output[i];
      i++;

      Entries entries;
      entries.reserve(len);

      for (uint j = 0; j < len; ++j)
      {
        Entry entry;
        entry.index = output[i];
        i++;
        entry.position = output[i];
        i++;
        entries.push_back(entry);
      }

      Posting posting(doc);
      posting.entries = entries;

      postings.push_back(posting);
    }

    m_loaded[lexeme] = true;

    return postings;
  }

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

  Postings &get_postings(const Term &term)
  {
    Scanner sc;
    sc.load(term + " ");
    auto &tokens = sc.scan();

    if (tokens.empty())
      throw std::runtime_error("Term is empty");

    Token &token = tokens.front();

    std::string &lexeme = token.lexeme;
    if (m_loaded[lexeme])
      return m_dictionary[lexeme];

    m_dictionary[lexeme] = get_decompressed_postings(lexeme);

    return m_dictionary[lexeme];
  }

  void set_indir(const std::string &indir)
  {
    this->indir = indir;
  }

  void set_coding_type(int type)
  {
    this->coding_type = type;
  }

  void compress(const std::string &outdir, int coding_type)
  {
    using namespace ir;

    const char *extension = get_extension_for_coding(coding_type);
    if (!extension)
      throw std::runtime_error("Unknown coding type");

    double avgbps = 0;
    double avgratio = 0;
    double duration = 0;
    double kb = 0;
    double ckb = 0;

    for (auto &key : m_keys)
    {

      std::vector<unsigned int> output;
      uint size = 0;

      auto &postings = m_dictionary[key];
      for (auto &posting : postings)
        size += posting.size();

      output.reserve(size);
      for (auto &posting : postings)
      {
        output.push_back(posting.doc);
        output.push_back(posting.entries.size());
        for (auto &entry : posting.entries)
        {
          output.push_back(entry.index);
          output.push_back(entry.position);
        }
      }

      std::string path = outdir + "/" + key + "." + extension;

      Compressed com;

      com.init(path.c_str(), output.data(), output.size(), coding_type);
      com.compress();

      double ratio = com.ratio();
      double bps = com.bps();
      double time = com.duration();

      avgbps += bps;
      avgratio += ratio;
      duration += time;

      kb += com.kb();
      ckb += com.kb_compressed();

      std::cout << "Term: '" << key << "'\n";
      std::cout << "Size: " << com.kb() << " KB\n"
                << "Size compressed: " << com.kb_compressed() << " KB\n"
                << "Bits per symbol: " << bps << "\n"
                << "Compress ratio: " << ratio << "\n"
                << "Compress duration: " << time << " ms\n";

      std::cout << "-------------------------\n";
    }

    std::cout << "Coding type: " << coding_type << "\n";
    std::cout << "Size: "
              << kb << " KB\n";
    std::cout << "Size compressed: "
              << ckb << " KB\n";

    int size = m_keys.size();
    if (size > 0)
    {
      avgratio /= size;
      avgbps /= size;

      std::cout << "Avg. Bits per symbol: " << avgbps << "\n";
      std::cout << "Avg. Compress ratio: " << avgratio << "\n";
    }
    std::cout << "Total compress duration: " << duration << " ms\n";
  }
};

#endif