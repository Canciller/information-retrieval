#include "io_utils.h"
#include <cstdio>
#include <stdexcept>

void read_file(const char *path, char **buffer, long *size, bool zeroTerminated)
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