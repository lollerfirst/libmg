#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>

// Prime with 15 digits
static const int64_t P = 153975451609999; 

// a*x =\ 1 (mod 2^k) ==> a * x * (2 - a*x) =\ 1 (mod 2^2k)
int64_t inv_mod2(int64_t n, int64_t r)
{
  int64_t inv = 1;
  for (int i=0; i<__builtin_ffsll(r); ++i)
    inv *= 2 - n*inv;

  return inv;
}

// r is a power of 2
int64_t redc(int64_t x, int64_t r, int64_t n, int64_t inv_n)
{
  int64_t q = ((x & (r-1)) * inv_n) & (r-1);
  x = (x - q*n) >> (__builtin_ffsll(r) - 1);
  if (x < 0)
    x += n;
  return x;
}

// convert into Montgomery space
int64_t i2mg(int64_t x, int64_t r, int64_t n)
{
  x %= n;

  for (int i=0; i<__builtin_ffsll(r); ++i)
  {
    x <<= 1;
    if (x >= n)
      x -= n;
  }

  return x;
}


int main(void)
{
  srand48(time(NULL));
  
  int64_t r = 1 << __builtin_clzll(P); // First power of 2 > P
  int64_t n = P;
  
  int64_t r_inv, n_inv;

  n_inv = inv_mod2(n, r);
  printf("r = %ld, n = %ld, r_inv = %ld, n_inv = %ld\n", r, n, r_inv, n_inv);

  int64_t x = lrand48() % n;

  x = i2mg(x, r, n);

  int64_t redc_x = redc(x, r, n, n_inv);


  printf("x = %ld, redc_x = %ld\n", x, redc_x);

  return 0;
}
