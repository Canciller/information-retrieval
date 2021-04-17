#include <cstdlib>
#include <cstdio>

#include <golomb.h>
#include <coding_policy.h>

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

void pfordelta(unsigned int *input, int size)
{

  unsigned int *coded, *decoded;

  coded = (unsigned int *)malloc(size * sizeof(unsigned int));
  decoded = (unsigned int *)malloc(size * sizeof(unsigned int));

  int newsize;
  int finalsize;

  newsize = compress_pfordelta(input, coded, size, 4);
  finalsize = decompress_pfordelta(coded, decoded, size, 4);

  printf("Normal size: %d\n", size);
  printf("Compress size: %d\n", newsize);
  printf("Consumed size: %d\n", finalsize);

  for (int i = 0; i < size; ++i)
    printf("%d ", decoded[i]);

  printf("\n");
}

void golomb()
{
}

int main(int argc, char const *argv[])
{
  unsigned int input[10] = {13, 32, 5, 54, 3, 32, 3, 12, 31, 23};
  int size = 10;

  pfordelta(input, size);

  golomb();

  return 0;
}
