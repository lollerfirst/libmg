# Montgomery Multiplication

Many algorithms in number theory, like prime testing or integer factorization, and in cryptography, like RSA, require lots of operations modulo a large number. A multiplications like  $ x\cdot y \bmod{n} $  is quite slow to compute with the typical algorithms, since it requires a division to know how many times  

$n$  has to be subtracted from the product. And division is a really expensive operation, especially with big numbers.

The Montgomery (modular) multiplication is a method that allows computing such multiplications faster. Instead of dividing the product and subtracting  
$n$  multiple times, it adds multiples of  $n$  to cancel out the lower bits and then just discards the lower bits.

