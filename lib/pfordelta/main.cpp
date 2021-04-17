#include <golomb.h>
#include <pfordelta.h>

void test_pfordelta()
{
  //unsigned int input[] = {2322231,2,3,4,5,6,7,8,9,10};
  unsigned int input[128] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 10, 10, 10, 10, 10, 10, 10, 10};
  unsigned int *coded;
  unsigned int *output;

  int size = 128;
  int newsize;
  int finalsize;
  int i;

  coded = (unsigned int *)malloc(size * sizeof(unsigned int));
  output = (unsigned int *)malloc(size * sizeof(unsigned int));
  //printf("coded = %X\n", coded);
  //printf("output = %X\n", output);
  newsize = compress_pfordelta(input, coded, size, 32);
  finalsize = decompress_pfordelta(coded, output, size, 32);

  printf("Normal size: %d\n", size);
  printf("Compress size: %d\n", newsize);
  printf("Consumed size: %d\n", finalsize);
  for (i = 0; i < size; i++)
  {
    printf("%u -> %X -> %u\n", input[i], coded[i], output[i]);
  }
}

int main(int argc, char const *argv[])
{
  test_pfordelta() return 0;
}
