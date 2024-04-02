#include <stdio.h>
#include "mg.h"

int main() {
    // Initialize variables
    mg_t mg;
    mpz_t number;
    mpz_init(number);

    // Example modulus (replace with your own value)
    mpz_t modulus;
    mpz_init_set_str(modulus, "65537", 10); // 2^16 + 1

    gmp_printf("modulus: %Zd\n", modulus);

    // Initialize Montgomery structure
    mg_init(&mg, modulus);

    // Example number (replace with your own value)
    mpz_set_ui(number, 12345);

    gmp_printf("number: %Zd\n", number);

    // Convert number to Montgomery form
    mg_i2mg(&mg, number);

    // Perform some operations in Montgomery form (for demonstration purposes)
    // (e.g., here you can perform modular multiplication, exponentiation, etc.)
    gmp_printf("number: %Zd\n", number);

    // Convert number back from Montgomery form
    mg_mg2i(&mg, number);

    // Output the result
    gmp_printf("Original number: %Zd\n", number);

    // Release resources
    mpz_clear(number);
    mpz_clear(modulus);
    mg_release(&mg);

    return 0;
}