#include <stdio.h>
#include "mg.h"

int main() {
    // Initialize variables
    mg_t mg;
    mpz_t number;
    mpz_t number2;
    mpz_t reference;
    mpz_init(number);
    mpz_init(number2);
    mpz_init(reference);

    // Example modulus (replace with your own value)
    mpz_t modulus;
    mpz_init_set_str(modulus, "447183295099964424624663934151", 10);

    gmp_printf("modulus: %Zd\n", modulus);

    // Initialize Montgomery structure
    mg_init(&mg, modulus);

    print_mg_struct(&mg);

    // Example number (replace with your own value)
    mpz_set_ui(number, 123456);
    mpz_set_ui(number2, 222100);
    
    mpz_mul(reference, number, number2);
    mpz_mod(reference, reference, modulus);
    
    gmp_printf("number: %Zd\n", number);
    gmp_printf("number2: %Zd\n", number2);

    // Convert number to Montgomery form
    mg_i2mg(&mg, number);
    mg_i2mg(&mg, number2);
    
    gmp_printf("mg number: %Zd\n", number);
    gmp_printf("mg number2: %Zd\n", number2);
    
    mpz_mul(number, number, number2);
    mg_redc(&mg, number);

    // Convert number back from Montgomery form
    mg_mg2i(&mg, number);

    // Output the result
    gmp_printf("multiplied and converted back: %Zd\n", number);
    gmp_printf("multiplied original result: %Zd\n", reference);

    // Release resources
    mpz_clear(number);
    mpz_clear(modulus);
    mpz_clear(number2);
    mpz_clear(reference);
    mg_release(&mg);

    return 0;
}
