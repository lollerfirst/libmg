#ifndef _MG_H
#define _MG_H

#include <gmp.h>
#include <stdbool.h>
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
 * @brief Initializes the mg_t structure
 * 
 * @param mg struct mg_t to be initialized
 * @param n modulus
 * @return 0 on success, other on error.
 */
int mg_init(mg_t *mg, mpz_t n);

/**
 * @brief Initializes the mg_t structure with a specific value for r
 * 
 * @param mg struct mg_t to be initialized
 * @param r power of two bigger than n
 * @param n modulus
 * @return 0 on success, other on error.
 */
int mg_init_r(mg_t *mg, mpz_t r, mpz_t n);

void print_mg_struct(const mg_t *mg);
int mg_release(mg_t *mg);

/**
 * @brief Performs x * r^-1   (mod n) without using divisions.
 * 
 * @param mg 
 * @param x 
 */
void mg_redc(mg_t *mg, mpz_t x);

/**
 * @brief Converts x in montgomery form
 * 
 * @param mg mg_t struct associated with the transformation
 * @param x OUT -- x gets converted into montgomery form 
 */
void mg_i2mg(mg_t *mg, mpz_t x);

/**
 * @brief Converts x out of montgomery form
 * 
 * @param mg mg_t struct associated with the transformation
 * @param x OUT -- x gets converted out of montgomery form
 */
void mg_mg2i(mg_t *mg, mpz_t x);

#endif 
