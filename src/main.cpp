#include "coding_factory.h"
#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>

#define MAX_NUMBER 10000

/*
RICE 0
PFOR 1
VBYT 2
TRICE 3
S9 4
S16 5
NULC 6
*/

char *read_file(const char *path, long *size = NULL)
{
  FILE *file = fopen(path, "r");
  if (file == NULL)
    return NULL;

  fseek(file, 0, SEEK_END);
  long fsize = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *buff = new char[fsize];

  // TODO: check read error
  fread(buff, 1, fsize, file);
  fclose(file);

  if (size)
    *size = fsize;

  return buff;
}

int write_file(const char *path, char *buff, long size)
{
  FILE *file = fopen(path, "w");
  if (file == NULL)
    return -1;

  // TODO: check write error
  fwrite(buff, 1, size, file);
  fclose(file);

  return 0;
}

int decompress(int argc, char const *argv[])
{
  if (argc < 4)
    return EXIT_FAILURE;

  const char *file = argv[2];
  int type = std::atoi(argv[3]);

  //printf("Coding type: %d\n", type);

  coding *cod = coding_factory::get_coder(type);
  if (cod == NULL)
  {
    fprintf(stderr, "Unknown coding type\n");
    return EXIT_FAILURE;
  }

  long bytes;
  unsigned int *input = (unsigned int *)read_file(file, &bytes);

  if (input == NULL)
  {
    return EXIT_FAILURE;
  }
  else if (bytes < sizeof(unsigned int))
  {
    return EXIT_FAILURE;
  }

  unsigned int size = input[0];
  if (size == 0)
  {
    return EXIT_FAILURE;
  }

  //printf("Size: %d\n", size);

  unsigned int *output = new unsigned int[size];

  cod->set_size(size);
  cod->Decompression(input + 1, output, size);

  for (int i = 0; i < size; ++i)
    printf("%d ", output[i]);
  putchar('\n');

  delete[] output;
  delete[] input;
  delete cod;

  return EXIT_SUCCESS;
}

int compress(int argc, char const *argv[])
{
  if (argc < 4)
    return EXIT_FAILURE;

  int type = NULC;

  if (argc >= 5)
    type = std::atoi(argv[4]);

  printf("Coding type: %d\n", type);

  // TODO: delete cod
  coding *cod = coding_factory::get_coder(type);
  if (cod == NULL)
  {
    fprintf(stderr, "Unknown coding type\n");
    return EXIT_FAILURE;
  }

  const char *input_path = argv[2],
             *output_path = argv[3];

  long ibytes; // input size in bytes
  // TODO: delete input
  unsigned int *input = (unsigned int *)read_file(input_path, &ibytes);

  if (input == NULL)
  {
    fprintf(stderr, "Failed to read input file: '%s'\n", input_path);
    return EXIT_FAILURE;
  }
  else if (ibytes < sizeof(unsigned int))
  {
    fprintf(stderr, "Cannot read empty input file: '%s'\n", input_path);
    return EXIT_FAILURE;
  }

  int size = ibytes / sizeof(unsigned int);
  // TODO: delete output
  unsigned int *output = new unsigned int[size + 1];
  output[0] = size;

  cod->set_size(size);
  int obytes = cod->Compression(input, output + 1, size) * sizeof(unsigned int) + sizeof(unsigned int);
  int write = write_file(output_path, (char *)output, obytes);
  if (write == -1)
  {
    fprintf(stderr, "Failed to write to output file: '%s'\n", output_path);
    return EXIT_FAILURE;
  }

  delete[] output;
  delete[] input;
  delete cod;

  return EXIT_SUCCESS;
}

int create_test(int argc, char const *argv[])
{
  if (argc < 4)
    return EXIT_FAILURE;

  const char *output_file = argv[2];
  int size = atoi(argv[3]);

  // TODO: delete output
  unsigned int *output = new unsigned int[size];

  for (int i = 0; i < size; ++i)
    output[i] = rand() % MAX_NUMBER + 1;

  int write = write_file(output_file, (char *)output, size * sizeof(unsigned int));
  if (write == -1)
  {
    fprintf(stderr, "Failed to write to output file: '%s'\n", output_file);
    return EXIT_FAILURE;
  }

  delete[] output;

  return EXIT_SUCCESS;
}

int print_test(int argc, char const *argv[])
{
  if (argc < 3)
    return EXIT_FAILURE;

  long bytes;
  const char *file = argv[2];
  char *buff = read_file(file, &bytes);
  if (buff == NULL)
  {
    fprintf(stderr, "Failed to read test file: '%s'\n", file);
    return EXIT_FAILURE;
  }
  else if (bytes < sizeof(unsigned int))
  {
    return EXIT_FAILURE;
  }

  int size = bytes / sizeof(unsigned int);
  unsigned int *input = (unsigned int *)buff;

  for (int i = 0; i < size; ++i)
    printf("%d ", input[i]);
  putchar('\n');

  delete[] buff;

  return EXIT_SUCCESS;
}

int main(int argc, char const *argv[])
{
  if (argc < 2)
    return EXIT_FAILURE;

  char action = argv[1][0];

  switch (action)
  {
  case 'c': // compress
    return compress(argc, argv);
  case 'd': // decompress
    return decompress(argc, argv);
  case 't': // create test file
    return create_test(argc, argv);
  case 's': // print test file
    return print_test(argc, argv);
  default:
    fprintf(stderr, "Unknown action\n");
    return EXIT_FAILURE;
  }
}