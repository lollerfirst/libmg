#include <gmp.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

/**
 * @brief Structure holding key information for the Montgomery form
 * 
 */
typedef struct __mg_t
{
    bool init;
    mpz_t ctx;
    mpz_t r;
    mpz_t r_sq;
    mpz_t n;
    mpz_t n_inv;
} mg_t;

/**
 * @brief Prints the contents of the mg_t structure
 * 
 * This function outputs the initialization status and the values of the 
 * Montgomery form parameters stored in the mg_t structure.
 * 
 * @param mg Pointer to the mg_t structure to be printed
 */
void print_mg_struct(const mg_t *mg) {
    printf("Init: %s\n", mg->init ? "true" : "false");
    gmp_printf("ctx: %Zd\n", mg->ctx);
    gmp_printf("r: %Zd\n", mg->r);
    gmp_printf("r_sq: %Zd\n", mg->r_sq);
    gmp_printf("n: %Zd\n", mg->n);
    gmp_printf("n_inv: %Zd\n", mg->n_inv);
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

    int l = mpz_sizeinbase(mg->r, 2);
    mpz_mod_2exp(mg->ctx, x, l-1);
    mpz_mul(mg->ctx, mg->ctx, mg->n_inv);
    mpz_mod_2exp(mg->ctx, mg->ctx, l-1);

    mpz_mul(mg->ctx, mg->ctx, mg->n);
    mpz_sub(x, x, mg->ctx);
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
   //assert(mpz_probab_prime_p(n, 7) > 0);

    if (mg->init)
        return -1;

    mpz_inits(mg->n, mg->n_inv, mg->r, mg->r_sq, mg->ctx, NULL);
    mpz_set(mg->n, n);
    mpz_set_ui(mg->r, 1UL);
    int l = mpz_sizeinbase(mg->n, 2);
    mpz_mul_2exp(mg->r, mg->r, l);
    mpz_mul_2exp(mg->r_sq, mg->r, l);
    mpz_mod(mg->r_sq, mg->r_sq, mg->n);
    int l = mpz_sizeinbase(mg->r, 2);
    int sqrt_l = (int)sqrt(l);
    if (sqrt_l * sqrt_l == l) {
        mg_inv_mod2(mg->n_inv, mg->n, l);
    } else {
        mpz_invert(mg->n_inv, mg->n, mg->r);
    }
    mg->init = true;
    return 0;
}

static void mg_inv_mod2(mpz_t inv, mpz_t a, unsigned int m) {
    mpz_set_ui(inv, 1UL);
    mpz_t temp;
    mpz_init(temp);
    for (unsigned int i=2; i<m; i <<= 1) {
        mpz_mul(temp, inv, a);
        mpz_neg(temp, temp);
        mpz_add_ui(temp, temp, 2);
        mpz_mul(inv, inv, temp);
        mpz_mod_2exp(inv, inv, i);
    }
    mpz_clear(temp);
}

/**
 * @brief Initializes the mg_t structure with a specific value for r
 * 
 * @param mg struct mg_t to be initialized
 * @param r power of two bigger than n
 * @param n modulus
 * @return 0 on success, other on error.
 */
int mg_init_r(mg_t *mg, mpz_t r, mpz_t n)
{
    assert(mpz_cmp(r, n) > 0);

    if (mg->init)
        return -1;

    mpz_t rem;
    mpz_init(rem);

    mpz_mod_ui(rem, n, 2);
    if (mpz_cmp_ui(rem, 2) != 0) {
        mpz_clear(rem);
        return -1;
    }

    mpz_inits(mg->n, mg->n_inv, mg->r, mg->r_sq, mg->ctx, NULL);
    mpz_set(mg->n, n);
    mpz_set(mg->r, r);
    int l = mpz_sizeinbase(mg->r, 2);
    mpz_mul_2exp(mg->r_sq, mg->r, l-1);
    mpz_mod(mg->r_sq, mg->r_sq, mg->n);
    int l = mpz_sizeinbase(mg->r, 2);
    int sqrt_l = (int)sqrt(l);
    if (sqrt_l * sqrt_l == l) {
        mg_inv_mod2(mg->n_inv, mg->n, l);
    } else {
        mpz_invert(mg->n_inv, mg->n, mg->r);
    }
    mg->init = true;

    mpz_clear(rem);
    return 0;
}

/**
 * @brief Releases the resources held by the mg_t structure
 * 
 * This function clears the memory used by the mpz_t fields within the mg_t 
 * structure and marks it as uninitialized.
 * 
 * @param mg Pointer to the mg_t structure to be released
 * @return 0 on success, other on error.
 */
int mg_release(mg_t *mg)
{
    if (!mg->init)
        return -1;
    mpz_clears(mg->n, mg->n_inv, mg->r, mg->r_sq, mg->ctx, NULL);
    mg->init = false;
    return 0;
}
