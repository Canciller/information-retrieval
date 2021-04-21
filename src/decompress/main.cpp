#include "ir/common.h"

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

  if (argc < 2)
  {
    fprintf(stderr, "Usage: %s INPUT_FILE [CODING_TYPE]\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char *input_path = argv[1];

  int coding_type = NULC;
  if (argc >= 3)
    coding_type = std::atoi(argv[2]);

  try
  {
    Decompressed decompressed;
    decompressed.init(input_path, coding_type);
    unsigned int *output = decompressed.decompress();

    for (int i = 0; i < decompressed.size(); ++i)
      printf("%d ", output[i]);
    putchar('\n');
  }
  catch (const std::runtime_error &e)
  {
    fprintf(stderr, "Error: %s\n", e.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}