#include <cstdlib>
#include <cstdio>
#include <cstring>

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

  int result = golomb_encode((void *)input, size * sizeof(unsigned int), (void **)&ge, &ge_size, &golomb_param);
  if (result == 0)
  {
    int result = golomb_decode(ge, ge_size, golomb_param, (void **)&gd, &gd_size);
    if (result == 0)
    {
      printf("Normal size: %ld\n", size * sizeof(unsigned int));
      printf("Enconded size: %ld\n", ge_size);
      printf("Decoded size: %ld\n", gd_size);

      unsigned int *out = (unsigned int *)gd;

      for (int i = 0; i < size; ++i)
      {
        printf("%d ", *(out + i));
      }
      printf("\n");
    }
  }
}

char *readFile(const char *path, long *size)
{
  FILE *file = fopen(path, "r");
  if (file == NULL)
  {
    fprintf(stderr, "Failed to open file\n");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long fsize = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *str = (char *)malloc(fsize);
  fread(str, 1, fsize, file);
  fclose(file);

  if (size)
    *size = fsize;
  return str;
}

int writeFile(const char *path, char *buffer, int size)
{
  FILE *outfile = fopen(path, "wb");
  if (outfile == NULL)
  {
    fprintf(stderr, "Failed to open file\n");
    return -1;
  }

  fwrite(buffer, 1, size, outfile);
  fclose(outfile);

  return 0;
}

int golomb_encode_to_file(const char *path, unsigned int *input, int size)
{
  unsigned char *ge;
  unsigned long ge_size;
  unsigned int golomb_param;

  int encode = golomb_encode((void *)input, size * sizeof(unsigned int), (void **)&ge, &ge_size, &golomb_param);
  if (encode != 0)
  {
    fprintf(stderr, "Failed to encode\n");
    return -1;
  }

  double ratio = size * sizeof(unsigned int) / (double)ge_size;
  printf("Ratio: %lf\n", ratio);
  printf("Bits per Symbol: %lf\n", sizeof(unsigned int) * 8 / ratio);

  unsigned long ge_cpy_size = ge_size + sizeof(unsigned int);
  unsigned char *ge_cpy = (unsigned char *)malloc(ge_cpy_size);
  if (ge_cpy == NULL)
    return -1;

  memcpy(ge_cpy + sizeof(unsigned int), ge, ge_size);
  memcpy(ge_cpy, &golomb_param, sizeof(unsigned int));

  free(ge);

  int write = writeFile(path, (char *)ge_cpy, ge_cpy_size);

  free(ge_cpy);

  return write;
}

unsigned int *golomb_decode_from_file(const char *path, unsigned long *dsize)
{
  long fsize;
  char *buff = readFile(path, &fsize);
  if (buff == NULL)
    return NULL;

  long size = fsize - sizeof(unsigned int);

  unsigned char *gd;
  unsigned long gd_size;
  unsigned int golomb_param = *((unsigned int *)buff);

  int decode = golomb_decode(buff + sizeof(unsigned int), size, golomb_param, (void **)&gd, &gd_size);
  if (decode != 0)
  {
    fprintf(stderr, "Failed to decode\n");
    return NULL;
  }

  if (dsize)
    *dsize = gd_size / sizeof(unsigned int);

  free(buff);

  return (unsigned int *)gd;
}

void test_golomb_decode()
{
  printf("Decode Golomb\n");
  const char *file = "golomb";

  unsigned long size;
  unsigned int *output = golomb_decode_from_file(file, &size);

  printf("Decoded size: %ld\n", size);
  for (int i = 0; i < size; ++i)
    printf("%d ", *(output + i));

  putchar('\n');

  free(output);
}

void test_golomb_encode()
{
  printf("Encode Golomb\n");
  const char *file = "golomb";

  unsigned int input[128] = {
      13,
      32,
      5,
      54,
      3,
      32,
      3,
      12,
      31,
      23,
      3243,
      343,
      43,
      43,
      43,
      434,
      3,
      43,
      43,
      43,
      4,
      34,
      3,
      43,
      43,
      4,
      3,
      43,
      43,
      4,
      34,
      3,
      43,
      43,
      43,
      4,
      3,
      43,
  };
  int size = 128;

  golomb_encode_to_file(file, input, size);
}

int main(int argc, char const *argv[])
{
  if (argc == 2)
  {
    test_golomb_decode();
    putchar('\n');
  }
  else
  {
    test_golomb_encode();
    putchar('\n');
  }

  return EXIT_SUCCESS;
}
