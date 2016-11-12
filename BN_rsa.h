#include <openssl/bn.h>
#include <openssl/err.h>

typedef BIGNUM BN;

void BN_error(char *);
int BN_isPrime(BN *);
int BN_f(BN *, BN *, BN_CTX *);
BN * BN_rho(BN *);
BN * BN_totient(BN *);

void BN_error(char *msg){
  char err[256];
  unsigned long nr;
  printf("%s\n", msg);

  nr = ERR_get_error();
  ERR_error_string(nr, err);
  printf("%s\n", err);
  ERR_print_errors_fp(stderr);
}

int BN_isPrime(BN *n){
  if(BN_is_one(n))  return 1;
  BN_CTX *ctx = BN_CTX_new();
  int result = BN_is_prime_ex(n, 1000, ctx, NULL);
  BN_CTX_free(ctx);

  return result;
}

// Helper function for BN_rho. Returns x^2 (mod m)
int BN_f(BN *x, BN *m, BN_CTX *ctx){
  return BN_mod_sqr(x, x, m, ctx);
}

BN * BN_rho(BN *n){
  BN *x = BN_new(), *y = BN_new(), *g = BN_new(), *bna = BN_new(), *tmp = BN_new();
  BN_CTX *ctx = BN_CTX_new();

  // printf("initialized vars\n");
  BN_set_word(x, 2);
  BN_set_word(y, 2);
  // printf("set words\n");

  if(BN_isPrime(n)) return n;
  if(BN_is_one(n))  return (BN *)BN_value_one();  // Typecast to rid of the const

  // printf("wasn't prime or one\n");
  // If n is even return 2
  // x is set to 2 at this point so return it
  if(!BN_is_odd(n)) return x;
  // printf("wasn't even\n");

  // printf("entering loop\n");
  while(1){
    // bna = a
    BN_set_word(bna, 1);

    // x = f(x) + a (mod n)
    BN_f(x, n, ctx);
    BN_mod_add(x, x, bna, n, ctx);
    // BN_mod(x, x, n, ctx);

    // y = f(f(y) + a (mod n)) + a (mod n)
    BN_f(y, n, ctx);
    BN_mod_add(y, y, bna, n, ctx);
    // BN_mod(y, y, n, ctx);
    BN_f(y, n, ctx);
    BN_mod_add(y, y, bna, n, ctx);
    // BN_mod(y, y, n, ctx);

    // g = abs(x - y)
    BN_sub(g, x, y);
    BN_set_negative(g, 0);

    BN_gcd(g, g, n, ctx);
    if(!BN_cmp(x, y) || !BN_cmp(g, n)){
      // printf("updating the values\n");
      // x = y = rand() % n
      BN_pseudo_rand_range(x, n);
      BN_copy(y, x);
      // bna = rand() % 100
      BN_set_word(tmp, 100);
      BN_pseudo_rand_range(bna, tmp);
    } else if(BN_cmp(g, BN_value_one())){
      if(!BN_isPrime(g)){
        BN_free(x); BN_free(y); BN_free(bna); BN_free(tmp); BN_CTX_free(ctx);
        return BN_rho(g);
      }
      BN_free(x); BN_free(y); BN_free(bna); BN_free(tmp); BN_CTX_free(ctx);
      return g;
    }
  }
}

BN * BN_totient(BN *n){
  BN *result = BN_new();

  if(BN_isPrime(n)){
    BN_sub(result, n, BN_value_one());
    return result;
  }

  return 0;
}
