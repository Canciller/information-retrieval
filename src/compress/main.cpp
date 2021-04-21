#include "ir/common.h"
#include <chrono>

/*
RICE 0
PFOR 1
VBYT 2
TRICE 3
S9 4
S16 5
NULC 6
*/

int main(int argc, char const *argv[])
{
  using namespace ir;

  if (argc < 3)
  {
    fprintf(stderr, "Usage: %s INPUT_FILE OUTPUT_FILE [CODING_TYPE]\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char *input_path = argv[1],
             *output_path = argv[2];

  int coding_type = NULC;
  if (argc >= 4)
    coding_type = std::atoi(argv[3]);

  printf("Input path: %s\n", input_path);
  printf("Output path: %s\n", output_path);
  printf("Coding type: %d\n", coding_type);

  try
  {
    Input in;
    unsigned int *input = in.load(input_path);

    putchar('\n');
    printf("Input length: %ld\n", in.size());
    /*
    for (int i = 0; i < in.size(); ++i)
      printf("%d ", input[i]);
    putchar('\n');
    */

    auto t_start = std::chrono::high_resolution_clock::now();

    Compressed compressed;
    compressed.init(output_path, input, in.size(), coding_type);
    compressed.compress();

    auto t_end = std::chrono::high_resolution_clock::now();
    double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_start).count();

    putchar('\n');
    printf("Normal size bytes: %d\n", compressed.bytes());
    printf("Compressed size bytes: %d\n", compressed.bytes_compressed());

    putchar('\n');
    printf("Normal size kb: %lf\n", compressed.kb());
    printf("Compressed size kb: %lf\n", compressed.kb_compressed());

    double ratio = compressed.kb() / compressed.kb_compressed();

    putchar('\n');
    printf("Ratio: %lf\n", ratio);
    printf("Bits per symbol: %lf\n", sizeof(unsigned int) * 8 / ratio);

    putchar('\n');
    printf("Duration: %lf ms\n", elapsed_time_ms);
  }
  catch (const std::runtime_error &e)
  {
    fprintf(stderr, "Error: %s\n", e.what());
  }

  return EXIT_SUCCESS;
}

/*
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

  //printf("Coding type: %d\n", type);

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

  srand(time(NULL));

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
*/