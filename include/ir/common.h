#ifndef IR_COMMON_H
#define IR_COMMON_H

#include "coding_factory.h"
#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>
#include <stdexcept>
#include <new>

void read_file(const char *path, char **buffer, long *size = NULL)
{
  FILE *file = fopen(path, "r");
  if (!file)
    throw std::runtime_error("Failed to open file for reading");

  fseek(file, 0, SEEK_END);
  long bytes = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *buff = new (std::nothrow) char[bytes];
  if (!buff)
    throw std::runtime_error("Failed to create buffer for file");

  size_t bytesRead = fread(buff, 1, bytes, file);
  fclose(file);

  if (bytesRead != bytes)
  {
    delete[] buff;
    throw std::runtime_error("Failed to read whole file");
  }

  if (size)
    *size = bytes;

  if (buffer)
    *buffer = buff;
}

void write_file(const char *path, char *buffer, long size)
{
  FILE *file = fopen(path, "w");
  if (file == NULL)
    throw std::runtime_error("Failed to open file for writing");

  size_t bytesWrite = fwrite(buffer, 1, size, file);
  fclose(file);

  if (bytesWrite != size)
    throw std::runtime_error("Failed to write whole buffer");
}

namespace ir
{
  class Test
  {
    unsigned int *m_output = nullptr;
    unsigned int m_max;

  public:
    Test(unsigned int max) : m_max(max) {}

    ~Test()
    {
      clear();
    }

    unsigned int *write(const char *path, unsigned int size)
    {
      m_output = new (std::nothrow) unsigned int[size];
      if (!m_output)
        throw std::runtime_error("Failed to create output buffer");

      srand(time(0));
      for (int i = 0; i < size; ++i)
        m_output[i] = rand() % m_max + 1;

      write_file(path, (char *)m_output, size * sizeof(unsigned int));

      return m_output;
    }

    void clear()
    {
      m_max = 0;

      if (m_output)
        delete[] m_output;
    }
  };

  class Input
  {
    unsigned int *m_input = nullptr;
    long m_size;
    long m_bytes;

  public:
    ~Input()
    {
      if (m_input)
        delete[] m_input;
    }

    unsigned int *load(const char *path)
    {
      read_file(path, (char **)&m_input, &m_bytes);
      if (m_bytes < sizeof(unsigned int))
        throw std::runtime_error("Input file is invalid");

      m_size = m_bytes / sizeof(unsigned int);

      return m_input;
    }

    long size()
    {
      return m_size;
    }

    long bytes()
    {
      return m_bytes;
    }
  };

  class Decompressed
  {
    const char *m_input_path = nullptr;

    unsigned int *m_input = nullptr;
    unsigned int *m_output = nullptr;
    unsigned int *m_arr = nullptr;
    unsigned int m_size;
    long m_bytes;

    coding *m_coding = nullptr;

  public:
    ~Decompressed()
    {
      if (m_input)
        delete[] m_input;

      if (m_coding)
        delete m_coding;

      if (m_output)
        delete[] m_output;
    }

    void init(const char *input_path, int coding_type = NULC)
    {
      m_input_path = input_path;

      m_coding = coding_factory::get_coder(coding_type);
      if (!m_coding)
        throw std::runtime_error("Failed to get coder");

      read_file(input_path, (char **)&m_input, &m_bytes);
      if (m_bytes < sizeof(unsigned int))
        throw std::runtime_error("Input file is invalid");

      m_size = m_input[0];
      if (m_size == 0)
        throw std::runtime_error("Cannot decompress empty file");

      m_arr = m_input + 1;

      m_output = new (std::nothrow) unsigned int[m_size + 10];
      if (!m_output)
        throw std::runtime_error("Failed to create output buffer");
    }

    unsigned int *decompress()
    {
      m_coding->set_size(m_size);
      m_coding->Decompression(m_arr, m_output, m_size);

      return m_output;
    }

    unsigned int size()
    {
      return m_size;
    }

    unsigned int bytes()
    {
      return m_size * sizeof(unsigned int);
    }
  };

  class Compressed
  {
    const char *m_output_path = nullptr;

    unsigned int *m_input = nullptr;
    unsigned int m_size;
    unsigned int m_bytes;

    unsigned int m_out_bytes;
    unsigned int m_com_bytes;
    unsigned int *m_output = nullptr;
    unsigned int *m_arr = nullptr;

    coding *m_coding = nullptr;

  public:
    ~Compressed()
    {
      if (m_output)
        delete[] m_output;

      if (m_coding)
        delete m_coding;
    }

    void init(const char *output_path, unsigned int *input, unsigned int size, int coding_type = NULC)
    {
      m_output_path = output_path;
      m_input = input;
      m_size = size;

      m_coding = coding_factory::get_coder(coding_type);
      if (!m_coding)
        throw std::runtime_error("Failed to get coder");

      m_output = new (std::nothrow) unsigned int[size + 1];
      if (!m_output)
        throw std::runtime_error("Failed to create output buffer");

      m_arr = m_output + 1;
      m_output[0] = size;
      m_bytes = size * sizeof(unsigned int);
    }

    void compress()
    {
      m_coding->set_size(m_size);
      m_com_bytes = m_coding->Compression(m_input, m_arr, m_size) * sizeof(unsigned int);
      m_out_bytes = m_com_bytes + sizeof(unsigned int);

      write_file(m_output_path, (char *)m_output, m_out_bytes);
    }

    unsigned int bytes()
    {
      return m_bytes;
    }

    unsigned int bytes_compressed()
    {
      return m_com_bytes;
    }

    double kb()
    {
      return (double)m_bytes / 1000;
    }

    double kb_compressed()
    {
      return (double)m_com_bytes / 1000;
    }
  };
}

#endif