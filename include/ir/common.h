#ifndef IR_COMMON_H
#define IR_COMMON_H

#include "coding_factory.h"
#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>
#include <stdexcept>
#include <new>
#include <cstring>
#include <chrono>

const char *EXT[] = {"rice",
                     "pfor",
                     "vbyt",
                     "trice",
                     "s9",
                     "s16",
                     "nulc"};

const char *
get_extension_for_coding(int type)
{
  if (type >= 7)
    return nullptr;
  return EXT[type];
}

void read_file(const char *path, char **buffer, long *size = NULL, bool zeroTerminated = false)
{
  FILE *file = fopen(path, "r");
  if (!file)
    throw std::runtime_error("Failed to open file for reading");

  fseek(file, 0, SEEK_END);
  long bytes = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *buff = new (std::nothrow) char[zeroTerminated ? bytes + 1 : bytes];
  if (!buff)
    throw std::runtime_error("Failed to create buffer for file");

  size_t bytesRead = fread(buff, 1, bytes, file);
  fclose(file);

  if (bytesRead != bytes)
  {
    delete[] buff;
    throw std::runtime_error("Failed to read whole file");
  }

  if (zeroTerminated)
    buff[bytes] = '\0';

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
    unsigned int m_arr_size;
    long m_bytes;

    double elapsed_time_ms_dec;
    double elapsed_time_ms_init;

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
      auto t_start = std::chrono::high_resolution_clock::now();

      m_input_path = input_path;

      m_coding = coding_factory::get_coder(coding_type);
      if (!m_coding)
        throw std::runtime_error("Failed to get coder");

      read_file(input_path, (char **)&m_input, &m_bytes);
      if (m_bytes < sizeof(unsigned int))
        throw std::runtime_error("Input file is invalid");

      m_arr_size = m_input[0];
      m_size = m_coding->get_size(m_arr_size);
      if (m_size == 0)
        throw std::runtime_error("Cannot decompress empty file");

      m_arr = m_input + 1;

      m_output = new (std::nothrow) unsigned int[m_size + 30];
      if (!m_output)
        throw std::runtime_error("Failed to create output buffer");

      auto t_end = std::chrono::high_resolution_clock::now();
      elapsed_time_ms_init = std::chrono::duration<double, std::milli>(t_end - t_start).count();
    }

    unsigned int *decompress()
    {
      auto t_start = std::chrono::high_resolution_clock::now();

      m_coding->set_size(m_size);
      m_coding->Decompression(m_arr, m_output, m_size);

      auto t_end = std::chrono::high_resolution_clock::now();
      elapsed_time_ms_dec = std::chrono::duration<double, std::milli>(t_end - t_start).count();

      return m_output;
    }

    unsigned int size()
    {
      return m_arr_size;
    }

    unsigned int bytes()
    {
      return m_size * sizeof(unsigned int);
    }

    double compressed_kb()
    {
      return m_bytes / 1024.0;
    }

    double duration()
    {
      return elapsed_time_ms_dec;
    }

    double duration_init()
    {
      return elapsed_time_ms_init;
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

    double elapsed_time_ms;

    coding *m_coding = nullptr;

    bool manage_input = false;

  public:
    ~Compressed()
    {
      if (m_output)
        delete[] m_output;

      if (m_coding)
        delete m_coding;

      if (manage_input && m_input)
        delete[] m_input;
    }

    void init(const char *output_path, unsigned int *input, unsigned int size, int coding_type = NULC)
    {
      m_output_path = output_path;
      m_input = input;
      m_size = size;

      m_coding = coding_factory::get_coder(coding_type);
      if (!m_coding)
        throw std::runtime_error("Failed to get coder");

      m_size = m_coding->get_size(size);
      if (m_size > size)
      {
        manage_input = true;
        m_input = new (std::nothrow) unsigned int[m_size];
        if (!m_input)
          throw std::runtime_error("Failed to create input buffer");
        memcpy(m_input, input, size * sizeof(unsigned int));
      }

      m_output = new (std::nothrow) unsigned int[m_size + 1];
      if (!m_output)
        throw std::runtime_error("Failed to create output buffer");

      m_arr = m_output + 1;
      m_output[0] = size; // real size
      m_bytes = m_size * sizeof(unsigned int);
    }

    void compress()
    {
      auto t_start = std::chrono::high_resolution_clock::now();

      m_coding->set_size(m_size);
      m_com_bytes = m_coding->Compression(m_input, m_arr, m_size) * sizeof(unsigned int);
      m_out_bytes = m_com_bytes + sizeof(unsigned int);

      write_file(m_output_path, (char *)m_output, m_out_bytes);

      auto t_end = std::chrono::high_resolution_clock::now();
      elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_start).count();
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

    double ratio()
    {
      return kb() / kb_compressed();
    }

    double bps()
    {
      return sizeof(unsigned int) * 8 / ratio();
    }

    double duration()
    {
      return elapsed_time_ms;
    }
  };
}

#endif