/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is library that provides for multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library is designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@iahu.ca, http://libtommath.iahu.ca
 */
#include <tommath.h>

/* copy, b = a */
int
mp_copy (mp_int * a, mp_int * b)
{
  int       res, n;


  /* if dst == src do nothing */
  if (a == b || a->dp == b->dp) {
    return MP_OKAY;
  }

  /* grow dest */
  if ((res = mp_grow (b, a->used)) != MP_OKAY) {
    return res;
  }

  /* zero b and copy the parameters over */
  b->used = a->used;
  b->sign = a->sign;

  /* copy all the digits */
  for (n = 0; n < a->used; n++) {
    b->dp[n] = a->dp[n];
  }

  /* clear high digits */
  for (n = b->used; n < b->alloc; n++) {
    b->dp[n] = 0;
  }
  return MP_OKAY;
}
