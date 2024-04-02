#include <gmp.h>
#include <stdbool.h>
#include <assert.h>

/**
 * @brief Structure holding key information for the Montgomery form
 * 
 */
typedef struct __mg_t
{
    bool init;
    mpz_t ctx;
    mpz_t r;
    mpz_t r_mask;
    mpz_t r_sq;
    mpz_t n;
    mpz_t n_inv;
} mg_t;

/**
 * @brief Finds the inverse of a prime mod a power of 2 without the Euclidian Algorithm, faster.
 * 
 * @param inv inverse output
 * @param n prime modulus
 * @param r power of 2
 */
static void mg_inv_mod2(mpz_t inv, mpz_t n, mpz_t r)
{
    int i;
    mpz_set_ui(inv, 1);
    for (i=0; i<mpz_sizeinbase(r, 2)-1; ++i)
    {
        mpz_mul(inv, inv, n);
        mpz_ui_sub(inv, 2, inv);
    }

    if (mpz_cmp_ui(inv, 0) < 0)
        mpz_add(inv, inv, r);
}

/**
 * @brief Performs x * r^-1   (mod n) without using divisions.
 * 
 * @param mg 
 * @param x 
 */
void mg_redc(mg_t *mg, mpz_t x)
{
    assert(mg->init);

    mpz_and(mg->ctx, x, mg->r_mask);
    mpz_mul(mg->ctx, mg->ctx, mg->n_inv);
    mpz_and(mg->ctx, mg->ctx, mg->r_mask);

    mpz_mul(mg->ctx, mg->ctx, mg->n);
    mpz_sub(x, x, mg->ctx);
    int l = mpz_sizeinbase(mg->r, 2);
    mpz_div_2exp(x, x, l-1);

    if (mpz_cmp_ui(x, 0) < 0)
        mpz_add(x, x, mg->n);
}

/**
 * @brief Converts x in montgomery form
 * 
 * @param mg mg_t struct associated with the transformation
 * @param x OUT -- x gets converted into montgomery form 
 */
void mg_i2mg(mg_t *mg, mpz_t x)
{
    assert(mg->init);
    mpz_mul(x, x, mg->r_sq);
    mg_redc(mg, x);
}
/**
 * @brief Converts x out of montgomery form
 * 
 * @param mg mg_t struct associated with the transformation
 * @param x OUT -- x gets converted out of montgomery form
 */
void mg_mg2i(mg_t *mg, mpz_t x)
{
    assert(mg->init);
    mg_redc(mg, x);
}

/**
 * @brief Initializes the mg_t structure
 * 
 * @param mg struct mg_t to be initialized
 * @param n modulus
 * @return 0 on success, other on error.
 */
int mg_init(mg_t *mg, mpz_t n)
{
    assert(mpz_probab_prime_p(n, 7) > 0);

    if (mg->init)
        return -1;

    mpz_inits(mg->n, mg->n_inv, mg->r, mg->r_sq, mg->r_mask, mg->ctx, NULL);
    mpz_set(mg->n, n);
    mpz_set_ui(mg->r, 1UL);
    int l = mpz_sizeinbase(mg->n, 2);
    mpz_mul_2exp(mg->r, mg->r, l);
    mpz_mul(mg->r_sq, mg->r, mg->r);
    mpz_mod(mg->r_sq, mg->r_sq, mg->n);
    mpz_sub_ui(mg->r_mask, mg->r, 1);
    mg_inv_mod2(mg->n_inv, mg->n, mg->r);
    mg->init = true;
    return 0;
}

int mg_release(mg_t *mg)
{
    if (!mg->init)
        return -1;
    mpz_clears(mg->n, mg->n_inv, mg->r, mg->r_sq, mg->r_mask, mg->ctx, NULL);
    mg->init = false;
    return 0;
}