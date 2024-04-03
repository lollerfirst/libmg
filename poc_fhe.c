#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "mg.h"
#include <assert.h>

#define BIT_LENGTH 512

// Function to generate a prime number within a given limit
void get_prime(gmp_randstate_t state, mpz_t prime) {
    mpz_urandomb(prime, state, BIT_LENGTH>>1);
    mpz_nextprime(prime, prime);
}

unsigned long long rdtsc() {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((unsigned long long)hi << 32) | lo;
}

// In this PoC, encrypting/decrypting the values is the same as bringing in or out of Montgomery Form.
int main() {
    mpz_t P, Q, PQ, e, d, v_c, p_c, z_c, D, vp_c, vp, vp_d, z;
    unsigned long int v = 98, p = 14, z_val = 101;
    
    mpz_inits(P, Q, PQ, e, d, v_c, p_c, z_c, D, vp_c, vp, vp_d, z, NULL);
    
    // Seed random number generator with /dev/urandom
    FILE *urand = fopen("/dev/urandom", "rb");
    unsigned long seed;
    fread(&seed, sizeof(seed), 1, urand);
    fclose(urand);
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, seed);
    
    // Modulus for encryption
    unsigned long long start, end;
    start = rdtsc();
    get_prime(state, P);
    get_prime(state, Q);
    mpz_mul(PQ, P, Q);
    end = rdtsc();
    printf("CPU cycles taken to generate primes and compute PQ: %llu\n", end - start);

    // Compute value e as the smallest power of two such that e > PQ
    start = rdtsc();
    mpz_set_ui(e, 1);
    int l = mpz_sizeinbase(PQ, 2);
    mpz_mul_2exp(e, e, l);
    end = rdtsc();
    printf("CPU cycles taken to generate random value e: %llu\n", end - start);

    // Here the user would initialize his private mg struct with his private modulus
    // the server initializes a public mg struct with the public modulus
    mg_t private_mg, public_mg;
    mg_init_r(&private_mg, e, P);
    mg_init_r(&public_mg, e, PQ);

    print_mg_struct(&private_mg);
    print_mg_struct(&public_mg);

    // Encrypt the plaintext values -- Bring into Montgomery Space
    start = rdtsc();
    mpz_set_ui(v_c, v);
    mg_i2mg(&private_mg, v_c);
    mpz_set_ui(p_c, p);
    mg_i2mg(&private_mg, p_c);
    mpz_set_ui(z_c, z_val);
    mg_i2mg(&private_mg, z_c);
    end = rdtsc();
    printf("CPU cycles taken to encrypt plaintext values: %llu\n", end - start);
    
    // Print encrypted values
    gmp_printf("Encrypted value of v: %Zd\n", v_c);
    gmp_printf("Encrypted value of p: %Zd\n", p_c);
    gmp_printf("Encrypted value of z: %Zd\n", z_c);

    // THIS IS THE THIRD PARTY SERVER-SIDE STUFF
    // Compute vp_c = (v_c * p_c * R^-1 + z_c) % PQ
    start = rdtsc();
    mpz_mul(vp_c, v_c, p_c);
    mg_redc(&public_mg, vp_c);
    mpz_add(vp_c, vp_c, z_c);
    end = rdtsc();
    printf("CPU cycles taken for modular multiplication and addition: %llu\n", end - start);

    // Print computed value vp_c
    gmp_printf("Computed value vp_c: %ZX\n", vp_c);

    // Compare with plain multiplication and addition on host, modulo P
    start = rdtsc();
    mpz_set_ui(z, v);
    mpz_mul_ui(vp, z, p);
    mpz_add_ui(vp, vp, z_val);
    mpz_mod(vp, vp, P);
    end = rdtsc();
    printf("CPU cycles for multiplication and addition (plain computation): %lld\n", end-start);

    // Decryption of the product of encrypted values -- Bring out of Montgomery Form
    start = rdtsc();
    mg_mg2i(&private_mg, vp_c);
    end = rdtsc();
    printf("CPU cycles taken for decryption: %llu\n", end - start);

    // Print decrypted value
    gmp_printf("Decrypted result: %Zd\n", vp_c);

    // Print original result
    gmp_printf("Original result: %Zd\n", vp);
    
    // Ensure correctness
    assert(mpz_cmp(vp, vp_c) == 0);

    // Clear allocated memory
    mpz_clears(P, Q, PQ, e, d, v_c, p_c, z_c, D, vp_c, vp, vp_d, z, NULL);
    gmp_randclear(state);

    mg_release(&private_mg);
    mg_release(&public_mg);
    return 0;
}
