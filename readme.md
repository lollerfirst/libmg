# Installation
Ensure that you have the following installed on your system:
- CMake (version 3.0 or higher)
- GMP library (`libgmp`)

## Building the Project

Follow these steps to build the project:

1. **Clone the repository**

   ```sh
   git clone <repository_url>
   cd MyProject
   ```

2. **Create a build directory**

   ```sh
   mkdir build
   cd build
   ```

3. **Run CMake**

   ```sh
   cmake ..
   ```

4. **Build the project**

   ```sh
   make
   ```

## Running the Executables

After the build is complete, you will find the executables `test_mg` and `poc_fhe` in the `build` directory. You can run them using:

```sh
./test_mg
./poc_fhe
```

## Troubleshooting

If you encounter the following error:

```
libgmp is not installed. Please install libgmp.
```

It means that the GMP library is not found on your system. Install it using your package manager. For example, on Debian-based systems:

```sh
sudo apt-get install libgmp-dev
```

On Red Hat-based systems:

```sh
sudo yum install gmp-devel
```

# Using the Montgomery Reduction Library

This library provides functionality for performing Montgomery reductions, which are useful in modular arithmetic operations, particularly in cryptographic applications.

## Prerequisites

Ensure that you have the GMP library (`libgmp`) installed on your system.

## Initialization

Before using the Montgomery reduction functions, you must initialize the `mg_t` structure. There are two initialization functions available:

1. **Standard Initialization**

   ```c
   mg_t mg;
   mpz_t n;
   mpz_init_set_str(n, "YOUR_MODULUS_HERE", 10);  // Set your modulus here
   mg_init(&mg, n);
   mpz_clear(n);
   ```

2. **Initialization with a Specific Value for `r`**

   ```c
   mg_t mg;
   mpz_t r, n;
   mpz_init_set_str(r, "YOUR_R_HERE", 10);  // Set your r here
   mpz_init_set_str(n, "YOUR_MODULUS_HERE", 10);  // Set your modulus here
   mg_init_r(&mg, r, n);
   mpz_clear(r);
   mpz_clear(n);
   ```

## Converting Numbers

To work with numbers in Montgomery form, use the following functions:

1. **Convert to Montgomery Form**

   ```c
   mpz_t x;
   mpz_init_set_str(x, "YOUR_NUMBER_HERE", 10);  // Set your number here
   mg_i2mg(&mg, x);
   ```

2. **Convert from Montgomery Form**

   ```c
   mg_mg2i(&mg, x);
   ```

## Performing Montgomery Reduction

To perform a Montgomery reduction on a number `x`, use the `mg_redc` function:

```c
mg_redc(&mg, x);
```

## Printing the Montgomery Structure

To print the contents of the `mg_t` structure for debugging purposes, use:

```c
print_mg_struct(&mg);
```

## Releasing Resources

Once you are done using the Montgomery reduction library, release the resources associated with the `mg_t` structure:

```c
mg_release(&mg);
```

## Example Usage

Here is a complete example that demonstrates the initialization, conversion, reduction, and cleanup:

```c
#include <gmp.h>
#include <stdio.h>
#include "libmg.h"  // Include your header file

int main() {
    mg_t mg;
    mpz_t n, r, x, y;

    // Initialize modulus and r
    mpz_init_set_str(n, "YOUR_MODULUS_HERE", 10);
    mpz_init_set_str(r, "YOUR_R_HERE", 10);

    // Initialize the Montgomery structure
    mg_init_r(&mg, r, n);

    // Initialize and set x
    mpz_init_set_str(x, "YOUR_NUMBER_HERE", 10);
    mpz_init_set_str(y, "YOUR_NUMBER_HERE", 10);

    // Convert x to Montgomery form
    mg_i2mg(&mg, x);
    mg_i2mg(&mg, y);

    // Perform Montgomery reduction
    mpz_mul(x, x, y);
    mg_redc(&mg, x);

    // Convert x out of Montgomery form
    mg_mg2i(&mg, x);

    // Print the result
    gmp_printf("Result: %Zd\n", x);

    // Release resources
    mg_release(&mg);

    // Clear mpz_t variables
    mpz_clear(n);
    mpz_clear(r);
    mpz_clear(x);
    mpz_clear(y);

    return 0;
}
```

Replace `YOUR_MODULUS_HERE`, `YOUR_R_HERE`, and `YOUR_NUMBER_HERE` with appropriate values for your application.

## Notes

- Ensure that the modulus `n` is a prime number for the Montgomery reduction to work correctly.
- The value of `r` should be a power of two that is greater than `n`.

By following these instructions, you can effectively utilize the Montgomery reduction library in your applications.

# Montgomery Multiplication

Many algorithms in number theory, like prime testing or integer factorization, and in cryptography, like RSA, require lots of operations modulo a large number.

A multiplication like
```math
x \cdot y \pmod n
```
is quite slow to compute with the typical algorithms, since it requires a division to know how many times $n$ has to be subtracted from the product. And division is a really expensive operation, especially with big numbers.

The Montgomery (modular) multiplication is a method that allows computing such multiplications faster. Instead of dividing the product and subtracting  
$n$  multiple times, it adds multiples of  $n$  to cancel out the lower bits and then just discards the lower bits.

# Montgomery Representation

The representative $\bar{x}$ of a number $x$ in the Montgomery space is defined as:

$\bar{x} := x\cdot r \bmod n$

Inside the Montgomery space you can still perform most operations as usual. You can add two elements, subtract, check for equality, and compute the GCD.

However this is not the case for multiplication.

Since normal multiplication would give us:

$\bar{x} \cdot \bar{y} = (x \cdot y) \cdot r \cdot r \bmod n$

Therefore we need the multiplication -inside the Montgomery space- defined as:

$\bar{x} * \bar{y} := x \cdot y \cdot r^{-1} \bmod n$

# Montgomery reduction

The multiplication of two numbers in the Montgomery space requires an efficient computation of  
$x \cdot r^{-1} \bmod n$ . This operation is called the Montgomery reduction, and is also known as the algorithm REDC.

Because  
```math
\gcd(n, r) = 1
```
we know that there are two numbers
```math
r^{-1}  \ \text{ and } \ n^{\prime}
```
with 
$0 < r^{-1}, n^{\prime} < n$  with
 
$r \cdot r^{-1} + n \cdot n^{\prime} = 1$, as per the Bézout Identity.

Using this identity we can write $x \cdot r^{-1}$ as:
 
```math
\begin{aligned} x \cdot r^{-1} &= x \cdot r \cdot r^{-1} / r = x \cdot (-n \cdot n' + 1) / r \\ &= (-x \cdot n \cdot n^{\prime} + x) / r \equiv (-x \cdot n \cdot n^{\prime} + l \cdot r \cdot n + x) / r \bmod n\\ &\equiv ((-x \cdot n^{\prime} + l \cdot r) \cdot n + x) / r \bmod n \end{aligned}$
```
Where $(-x\cdot n' + l \cdot r)$ basically means we can subtract any arbitrary
integer $l$ times $r$ from $x\cdot n'$. So that means we can compute a $$q := x \cdot n' \bmod r$$, and then
compute
```math
x\cdot r^{-1} \bmod n = ((x - q\cdot n) / r) \bmod n
```
. Remember that if $r$ is chosen as a
power of 2 (and larger than $n$) then all multiplication and divisions can be done by shifting.

That means we don't really need to calculate $r^{-1}$ but just $n'$.

# Fast Inverse Trick

for computing $$n' := n^{-1} \bmod r$$ efficiently, we can use the following relation:

$$ a \cdot x \equiv 1 \bmod 2^k \Longrightarrow a \cdot x \cdot (2-a\cdot x) \equiv 1 \bmod 2^{2k} $$

Where $$k = \log_2r$$. This can easily be proven. If we have $$a \cdot x = 1 + m \cdot 2^k$$, then we have:

```math
\begin{aligned} a \cdot x \cdot (2 - a \cdot x) &= 2 \cdot a \cdot x - (a \cdot x)^2 \\ &= 2 \cdot (1 + m \cdot 2^k) - (1 + m \cdot 2^k)^2 \\ &= 2 + 2 \cdot m \cdot 2^k - 1 - 2 \cdot m \cdot 2^k - m^2 \cdot 2^{2k} \\ &= 1 - m^2 \cdot 2^{2k} \\ &\equiv 1 \bmod 2^{2k}. \end{aligned}
```

This means we can start with something true: $x = 1$ as the inverse of any $a \bmod 2^1$. Then apply the trick until we reach the full $r$.

# Using the Montgomery Form for FH Encryption (Experimental)
Turns out we can use the Montgomery form not just for its intended use case, but also to obfuscate values we want to remain private such that they preserve their structure.
This in turn allows for multiplying, adding ciphertexts and producing valid ciphertext.

We pick a private and a public modulus, $P$ and $PQ$ where $P, Q$ are both large secure primes.
We then choose the $r$ value of the Montgomery form to be the smallest power of 2 bigger than $PQ$.

We can now encrypt our values by bringing them in Montgomery form under the private modulus $P$:

```math
\begin{align*}
c_0 \equiv v_0 r \pmod P\\
c_1 \equiv v_1 r \pmod P
\end{align*}
```
Which can be done by applying `REDC` to the product $v_ir^2$ as follows:

```math
\begin{align*}
c_0 = \text{REDC}_P(v_0r^2)\\
c_1 = \text{REDC}_P(v_1r^2)
\end{align*}
```

Now we can broadcast the encrypted values $c_0, c_1$ as well as the public modulus $PQ$ to a third party that will carry out -for example- a multiplication.
The party doesn't know the value of $P$ unless it can factor $PQ$, but the modular algebra that works under $P$ will work under $PQ$ either.
So, the third party computes the product of the encrypted values following the rules of the Montgomery form:

```math
c_* = \text{REDC}_{PQ}(c_0c_1)
```

Now it can send back the result to the host, which will be able to decrypt with:

```math
v_* = \text{REDC}_P(c_*)
```
and get back the multiplication of the two original values!

**This scheme would have been possible without the Montgomery form**

In fact, here is a link to a PoC i've written on the matter: [link](https://colab.research.google.com/drive/1lyAAcTkV8LonhTNlxv8VE_Oe4D_EVx4X).
But the Montgomery form makes it much more feasible to use because it reduces by a lot CPU cycles needed to carry out the computations on encrypted values, especially if said computations are many!
