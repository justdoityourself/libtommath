/* Generates provable primes
 *
 * See http://iahu.ca:8080/papers/pp.pdf for more info.
 *
 * Tom St Denis, tomstdenis@iahu.ca, http://tom.iahu.ca
 */
#include <time.h>
#include "bn.h"

/* fast square root */
static    mp_digit
i_sqrt (mp_word x)
{
  mp_word   x1, x2;

  x2 = x;
  do {
    x1 = x2;
    x2 = x1 - ((x1 * x1) - x) / (2 * x1);
  } while (x1 != x2);

  if (x1 * x1 > x) {
    --x1;
  }

  return x1;
}


/* generates a prime digit */
static    mp_digit
prime_digit ()
{
  mp_digit  r, x, y, next;

  /* make a DIGIT_BIT-bit random number */
  for (r = x = 0; x < DIGIT_BIT; x++) {
    r = (r << 1) | (rand () & 1);
  }

  /* now force it odd */
  r |= 1;

  /* force it to be >30 */
  if (r < 30) {
    r += 30;
  }

  /* get square root, since if 'r' is composite its factors must be < than this */
  y = i_sqrt (r);
  next = (y + 1) * (y + 1);

  do {
    r += 2;			/* next candidate */

    /* update sqrt ? */
    if (next <= r) {
      ++y;
      next = (y + 1) * (y + 1);
    }

    /* loop if divisible by 3,5,7,11,13,17,19,23,29  */
    if ((r % 3) == 0) {
      x = 0;
      continue;
    }
    if ((r % 5) == 0) {
      x = 0;
      continue;
    }
    if ((r % 7) == 0) {
      x = 0;
      continue;
    }
    if ((r % 11) == 0) {
      x = 0;
      continue;
    }
    if ((r % 13) == 0) {
      x = 0;
      continue;
    }
    if ((r % 17) == 0) {
      x = 0;
      continue;
    }
    if ((r % 19) == 0) {
      x = 0;
      continue;
    }
    if ((r % 23) == 0) {
      x = 0;
      continue;
    }
    if ((r % 29) == 0) {
      x = 0;
      continue;
    }

    /* now check if r is divisible by x + k={1,7,11,13,17,19,23,29} */
    for (x = 30; x <= y; x += 30) {
      if ((r % (x + 1)) == 0) {
	x = 0;
	break;
      }
      if ((r % (x + 7)) == 0) {
	x = 0;
	break;
      }
      if ((r % (x + 11)) == 0) {
	x = 0;
	break;
      }
      if ((r % (x + 13)) == 0) {
	x = 0;
	break;
      }
      if ((r % (x + 17)) == 0) {
	x = 0;
	break;
      }
      if ((r % (x + 19)) == 0) {
	x = 0;
	break;
      }
      if ((r % (x + 23)) == 0) {
	x = 0;
	break;
      }
      if ((r % (x + 29)) == 0) {
	x = 0;
	break;
      }
    }
  } while (x == 0);

  return r;
}

/* makes a prime of at least k bits */
int
pprime (int k, int li, mp_int * p, mp_int * q)
{
  mp_int    a, b, c, n, x, y, z, v;
  int       res, ii;
  static const mp_digit bases[] = { 2, 3, 5, 7, 11, 13, 17, 19 };

  /* single digit ? */
  if (k <= (int) DIGIT_BIT) {
    mp_set (p, prime_digit ());
    return MP_OKAY;
  }

  if ((res = mp_init (&c)) != MP_OKAY) {
    return res;
  }

  if ((res = mp_init (&v)) != MP_OKAY) {
    goto __C;
  }

  /* product of first 50 primes */
  if ((res =
       mp_read_radix (&v,
		      "19078266889580195013601891820992757757219839668357012055907516904309700014933909014729740190",
		      10)) != MP_OKAY) {
    goto __V;
  }

  if ((res = mp_init (&a)) != MP_OKAY) {
    goto __V;
  }

  /* set the prime */
  mp_set (&a, prime_digit ());

  if ((res = mp_init (&b)) != MP_OKAY) {
    goto __A;
  }

  if ((res = mp_init (&n)) != MP_OKAY) {
    goto __B;
  }

  if ((res = mp_init (&x)) != MP_OKAY) {
    goto __N;
  }

  if ((res = mp_init (&y)) != MP_OKAY) {
    goto __X;
  }

  if ((res = mp_init (&z)) != MP_OKAY) {
    goto __Y;
  }

  /* now loop making the single digit */
  while (mp_count_bits (&a) < k) {
    printf ("prime has %4d bits left\r", k - mp_count_bits (&a));
    fflush (stdout);
  top:
    mp_set (&b, prime_digit ());

    /* now compute z = a * b * 2 */
    if ((res = mp_mul (&a, &b, &z)) != MP_OKAY) {	/* z = a * b */
      goto __Z;
    }

    if ((res = mp_copy (&z, &c)) != MP_OKAY) {	/* c = a * b */
      goto __Z;
    }

    if ((res = mp_mul_2 (&z, &z)) != MP_OKAY) {	/* z = 2 * a * b */
      goto __Z;
    }

    /* n = z + 1 */
    if ((res = mp_add_d (&z, 1, &n)) != MP_OKAY) {	/* n = z + 1 */
      goto __Z;
    }

    /* check (n, v) == 1 */
    if ((res = mp_gcd (&n, &v, &y)) != MP_OKAY) {	/* y = (n, v) */
      goto __Z;
    }

    if (mp_cmp_d (&y, 1) != MP_EQ)
      goto top;

    /* now try base x=bases[ii]  */
    for (ii = 0; ii < li; ii++) {
      mp_set (&x, bases[ii]);

      /* compute x^a mod n */
      if ((res = mp_exptmod (&x, &a, &n, &y)) != MP_OKAY) {	/* y = x^a mod n */
	goto __Z;
      }

      /* if y == 1 loop */
      if (mp_cmp_d (&y, 1) == MP_EQ)
	continue;

      /* now x^2a mod n */
      if ((res = mp_sqrmod (&y, &n, &y)) != MP_OKAY) {	/* y = x^2a mod n */
	goto __Z;
      }

      if (mp_cmp_d (&y, 1) == MP_EQ)
	continue;

      /* compute x^b mod n */
      if ((res = mp_exptmod (&x, &b, &n, &y)) != MP_OKAY) {	/* y = x^b mod n */
	goto __Z;
      }

      /* if y == 1 loop */
      if (mp_cmp_d (&y, 1) == MP_EQ)
	continue;

      /* now x^2b mod n */
      if ((res = mp_sqrmod (&y, &n, &y)) != MP_OKAY) {	/* y = x^2b mod n */
	goto __Z;
      }

      if (mp_cmp_d (&y, 1) == MP_EQ)
	continue;

      /* compute x^c mod n == x^ab mod n */
      if ((res = mp_exptmod (&x, &c, &n, &y)) != MP_OKAY) {	/* y = x^ab mod n */
	goto __Z;
      }

      /* if y == 1 loop */
      if (mp_cmp_d (&y, 1) == MP_EQ)
	continue;

      /* now compute (x^c mod n)^2 */
      if ((res = mp_sqrmod (&y, &n, &y)) != MP_OKAY) {	/* y = x^2ab mod n */
	goto __Z;
      }

      /* y should be 1 */
      if (mp_cmp_d (&y, 1) != MP_EQ)
	continue;
      break;
    }

    /* no bases worked? */
    if (ii == li)
      goto top;

/*
{
   char buf[4096];
   
   mp_toradix(&n, buf, 10);
   printf("Certificate of primality for:\n%s\n\n", buf);
   mp_toradix(&a, buf, 10);
   printf("A == \n%s\n\n", buf);
   mp_toradix(&b, buf, 10);
   printf("B == \n%s\n", buf);
   printf("----------------------------------------------------------------\n");
}   
*/
    /* a = n */
    mp_copy (&n, &a);
  }

  /* get q to be the order of the large prime subgroup */
  mp_sub_d (&n, 1, q);
  mp_div_2 (q, q);
  mp_div (q, &b, q, NULL);

  mp_exch (&n, p);

  res = MP_OKAY;
__Z:mp_clear (&z);
__Y:mp_clear (&y);
__X:mp_clear (&x);
__N:mp_clear (&n);
__B:mp_clear (&b);
__A:mp_clear (&a);
__V:mp_clear (&v);
__C:mp_clear (&c);
  return res;
}


int
main (void)
{
  mp_int    p, q;
  char      buf[4096];
  int       k, li;
  clock_t   t1;

  srand (time (NULL));

  printf ("Enter # of bits: \n");
  fgets (buf, sizeof (buf), stdin);
  sscanf (buf, "%d", &k);

  printf ("Enter number of bases to try (1 to 8):\n");
  fgets (buf, sizeof (buf), stdin);
  sscanf (buf, "%d", &li);


  mp_init (&p);
  mp_init (&q);

  t1 = clock ();
  pprime (k, li, &p, &q);
  t1 = clock () - t1;

  printf ("\n\nTook %ld ticks, %d bits\n", t1, mp_count_bits (&p));

  mp_toradix (&p, buf, 10);
  printf ("P == %s\n", buf);
  mp_toradix (&q, buf, 10);
  printf ("Q == %s\n", buf);

  return 0;
}
