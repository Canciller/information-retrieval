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

  if (argc < 2)
  {
    fprintf(stderr, "Usage: %s INPUT_FILE [CODING_TYPE]\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char *input_path = argv[1];
  const char *txt_output_path = nullptr;

  int coding_type = NULC;
  if (argc >= 3)
    coding_type = std::atoi(argv[2]);

  if (argc >= 4)
    txt_output_path = argv[3];

  try
  {
    auto t_start = std::chrono::high_resolution_clock::now();

    Decompressed decompressed;
    decompressed.init(input_path, coding_type);
    unsigned int *output = decompressed.decompress();

    auto t_end = std::chrono::high_resolution_clock::now();
    double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_start).count();

    printf("Duration: %lf ms\n", elapsed_time_ms);

    FILE *outfile = stdout;

    if (txt_output_path)
      outfile = fopen(txt_output_path, "w");

    if (!outfile)
      throw std::runtime_error("Failed to open txt output path");

    for (int i = 0; i < decompressed.size(); ++i)
      fprintf(outfile, "%d ", output[i]);
    fprintf(outfile, "\n");
  }
  catch (const std::runtime_error &e)
  {
    fprintf(stderr, "Error: %s\n", e.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}