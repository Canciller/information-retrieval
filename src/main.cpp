#include <cstdlib>
#include <cstdio>

#include <golomb.h>
#include <coding_policy.h>
#include <s16.h>

void pfordelta(unsigned int *input, int size)
{
  unsigned int *coded, *decoded;

  coded = (unsigned int *)malloc(size * sizeof(unsigned int));
  decoded = (unsigned int *)malloc(size * sizeof(unsigned int));

  int newsize;
  int finalsize;

  newsize = compress_pfordelta(input, coded, size, 32);
  finalsize = decompress_pfordelta(coded, decoded, size, 32);

  printf("Normal size: %d\n", size * sizeof(unsigned int));
  printf("Encoded size: %d\n", newsize * sizeof(unsigned int));

  for (int i = 0; i < size; ++i)
    printf("%d ", decoded[i]);

  printf("\n");
}

void simple16(unsigned int *input, int size)
{
  unsigned int *coded, *decoded;

  coded = (unsigned int *)malloc(size * sizeof(unsigned int));
  decoded = (unsigned int *)malloc(size * sizeof(unsigned int));

  int newsize;
  int finalsize;

  newsize = s16_compress(input, coded, size);
  finalsize = s16_decompress(coded, decoded, size);

  printf("Normal size: %d\n", size * sizeof(unsigned int));
  printf("Encoded size: %d\n", newsize * sizeof(unsigned int));

  for (int i = 0; i < size; ++i)
    printf("%d ", decoded[i]);

  printf("\n");
}

void golomb(unsigned int *input, int size)
{
  unsigned char *ge, *gd;
  unsigned long ge_size, gd_size;
  unsigned int golomb_param;

  int result = golomb_encode((void *)input, size * sizeof(unsigned int), (void **) &ge, &ge_size, &golomb_param);
  if(result == 0) {
    int result = golomb_decode(ge, ge_size, golomb_param, (void **)&gd, &gd_size);
    if(result == 0) {
      printf("Normal size: %ld\n", size * sizeof(unsigned int));
      printf("Enconded size: %ld\n", ge_size);
      printf("Decoded size: %ld\n", gd_size);

      unsigned int *out = (unsigned int *) gd;

      for(int i = 0; i < size; ++i) {
        printf("%d ", *(out + i));
      }
      printf("\n");
    }
  }
}

int main(int argc, char const *argv[])
{
  unsigned int input[128] = {13, 32, 5, 54, 3, 32, 3, 12, 31, 23, 3, 43, 12,312,321 ,312,31,231,2312,3,123,123,123, 12,312};
  int size = 128;

  printf("PForDelta\n");
  pfordelta(input, size);

  printf("\nGolomb\n");
  golomb(input, size);

  printf("\nSimple16\n");
  simple16(input, size);

  return 0;
}
