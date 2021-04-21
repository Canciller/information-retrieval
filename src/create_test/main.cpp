#include "ir/common.h"

#define MAX 10000

int main(int argc, char const *argv[])
{
  using namespace ir;

  if (argc < 3)
  {
    fprintf(stderr, "Usage: %s OUTPUT_FILE SIZE\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char *output_path = argv[1];
  int size = std::atoi(argv[2]);

  try
  {
    Test test(MAX);
    unsigned int *output = test.write(output_path, size);

    for (int i = 0; i < size; ++i)
      printf("%d ", output[i]);
    putchar('\n');
  }
  catch (const std::runtime_error &e)
  {
    fprintf(stderr, "Error: %s\n", e.what());
  }

  return EXIT_SUCCESS;
}