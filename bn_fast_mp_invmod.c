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

/* computes the modular inverse via binary extended euclidean algorithm, that is c = 1/a mod b */
int
fast_mp_invmod (mp_int * a, mp_int * b, mp_int * c)
{
  mp_int    x, y, u, v, B, D;
  int       res, neg;


  if ((res = mp_init (&x)) != MP_OKAY) {
    goto __ERR;
  }

  if ((res = mp_init (&y)) != MP_OKAY) {
    goto __X;
  }

  if ((res = mp_init (&u)) != MP_OKAY) {
    goto __Y;
  }

  if ((res = mp_init (&v)) != MP_OKAY) {
    goto __U;
  }

  if ((res = mp_init (&B)) != MP_OKAY) {
    goto __V;
  }

  if ((res = mp_init (&D)) != MP_OKAY) {
    goto __B;
  }

  /* x == modulus, y == value to invert */
  if ((res = mp_copy (b, &x)) != MP_OKAY) {
    goto __D;
  }
  if ((res = mp_copy (a, &y)) != MP_OKAY) {
    goto __D;
  }

  if ((res = mp_abs (&y, &y)) != MP_OKAY) {
    goto __D;
  }

  /* 2. [modified] if x,y are both even then return an error! */
  if (mp_iseven (&x) == 1 && mp_iseven (&y) == 1) {
    res = MP_VAL;
    goto __D;
  }

  /* 3. u=x, v=y, A=1, B=0, C=0,D=1 */
  if ((res = mp_copy (&x, &u)) != MP_OKAY) {
    goto __D;
  }
  if ((res = mp_copy (&y, &v)) != MP_OKAY) {
    goto __D;
  }
  mp_set (&D, 1);


top:
  /* 4.  while u is even do */
  while (mp_iseven (&u) == 1) {
    /* 4.1 u = u/2 */
    if ((res = mp_div_2 (&u, &u)) != MP_OKAY) {
      goto __D;
    }
    /* 4.2 if A or B is odd then */
    if (mp_iseven (&B) == 0) {
      if ((res = mp_sub (&B, &x, &B)) != MP_OKAY) {
	goto __D;
      }
    }
    /* A = A/2, B = B/2 */
    if ((res = mp_div_2 (&B, &B)) != MP_OKAY) {
      goto __D;
    }
  }


  /* 5.  while v is even do */
  while (mp_iseven (&v) == 1) {
    /* 5.1 v = v/2 */
    if ((res = mp_div_2 (&v, &v)) != MP_OKAY) {
      goto __D;
    }
    /* 5.2 if C,D are even then */
    if (mp_iseven (&D) == 0) {
      /* C = (C+y)/2, D = (D-x)/2 */
      if ((res = mp_sub (&D, &x, &D)) != MP_OKAY) {
	goto __D;
      }
    }
    /* C = C/2, D = D/2 */
    if ((res = mp_div_2 (&D, &D)) != MP_OKAY) {
      goto __D;
    }
  }

  /* 6.  if u >= v then */
  if (mp_cmp (&u, &v) != MP_LT) {
    /* u = u - v, A = A - C, B = B - D */
    if ((res = mp_sub (&u, &v, &u)) != MP_OKAY) {
      goto __D;
    }

    if ((res = mp_sub (&B, &D, &B)) != MP_OKAY) {
      goto __D;
    }
  } else {
    /* v - v - u, C = C - A, D = D - B */
    if ((res = mp_sub (&v, &u, &v)) != MP_OKAY) {
      goto __D;
    }

    if ((res = mp_sub (&D, &B, &D)) != MP_OKAY) {
      goto __D;
    }
  }

  /* if not zero goto step 4 */
  if (mp_iszero (&u) == 0)
    goto top;

  /* now a = C, b = D, gcd == g*v */

  /* if v != 1 then there is no inverse */
  if (mp_cmp_d (&v, 1) != MP_EQ) {
    res = MP_VAL;
    goto __D;
  }

  /* b is now the inverse */
  neg = a->sign;
  while (D.sign == MP_NEG) {
    if ((res = mp_add (&D, b, &D)) != MP_OKAY) {
      goto __D;
    }
  }
  mp_exch (&D, c);
  c->sign = neg;
  res = MP_OKAY;

__D:mp_clear (&D);
__B:mp_clear (&B);
__V:mp_clear (&v);
__U:mp_clear (&u);
__Y:mp_clear (&y);
__X:mp_clear (&x);
__ERR:
  return res;
}
