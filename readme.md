# Montgomery Multiplication

Many algorithms in number theory, like prime testing or integer factorization, and in cryptography, like RSA, require lots of operations modulo a large number. A multiplications like  $$ x \cdot y \bmod n $$  is quite slow to compute with the typical algorithms, since it requires a division to know how many times  

$n$ has to be subtracted from the product. And division is a really expensive operation, especially with big numbers.

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

Because  $\gcd(n, r) = 1$ , we know that there are two numbers  $r^{-1}$  and  $n^{\prime}$  with  

$0 < r^{-1}, n^{\prime} < n$  with
 
$r \cdot r^{-1} + n \cdot n^{\prime} = 1$, as per the Bézout Identity.

Using this identity we can write $x \cdot r^{-1}$ as:
 
$$\begin{aligned} x \cdot r^{-1} &= x \cdot r \cdot r^{-1} / r = x \cdot (-n \cdot n' + 1) / r \\ &= (-x \cdot n \cdot n^{\prime} + x) / r \equiv (-x \cdot n \cdot n^{\prime} + l \cdot r \cdot n + x) / r \bmod n\\ &\equiv ((-x \cdot n^{\prime} + l \cdot r) \cdot n + x) / r \bmod n \end{aligned}$$ 

Where $$(-x\cdot n' + l \cdot r)$$ basically means we can subtract any arbitrary
integer $l$ times $r$ from $x\cdot n'$. So that means we can compute a $$q := x \cdot n' \bmod r$$, and then
compute $$ x\cdot r^{-1} \bmod n = ((x - q\cdot n) / r) \bmod n$$. Remember that if $r$ is chosen as a
power of 2 (and larger than $n$) then all multiplication and divisions can be done by shifting.

That means we don't really need to calculate $r^{-1}$ but just $n'$.

# Fast Inverse Trick.

for computing $$n' := n^{-1} \bmod r$$ efficiently, we can use the following relation:

$$ a \cdot x \equiv 1 \bmod 2^k \Longrightarrow a \cdot x \cdot (2-a\cdot x) \equiv 1 \bmod 2^{2k} $$

Where $$k = \log_2r$$. This can easily be proven. If we have $$a \cdot x = 1 + m \cdot 2^k$$, then we have:

$$\begin{aligned} a \cdot x \cdot (2 - a \cdot x) &= 2 \cdot a \cdot x - (a \cdot x)^2 \\ &= 2 \cdot (1 + m \cdot 2^k) - (1 + m \cdot 2^k)^2 \\ &= 2 + 2 \cdot m \cdot 2^k - 1 - 2 \cdot m \cdot 2^k - m^2 \cdot 2^{2k} \\ &= 1 - m^2 \cdot 2^{2k} \\ &\equiv 1 \bmod 2^{2k}. \end{aligned}$$ 

This means we can start with something true: $x = 1$ as the inverse of any $a \bmod 2^1$. Then apply the trick until we reach the full $r$.

