# How I solved it

## Simplifying the input
First, I simplified the assembly code so that irrelevant instructions (such as `div z 1`) were removed.

With some extra analysis, some blocks could be removed altogether. See this block at the very beginning of my problem input:
```
inp w
mul x 0
add x z
mod x 26
div z 1
add x 13
eql x w
eql x 0
```
This can be rewritten in a more readable way:
```
w = read()
x *= 0
x += z
x %= 26
z /= 1
x += 13
x = int(x == w)
x = int(x == 0)
```
Note that `z` is 0 at the beginning. Then the code can be simplified to:
```
w = read()
x = 13
x = int(x == w)
x = int(x == 0)
```
The combination of the two `eq` operations is equivalent to a `!=`:
```
w = read()
x = int(13 != w)
```
Now, since the input is bounded within [1, 9], we know this comparison to always be false. Since `x` starts at zero, the resulting equivalent code is:
```
w = read()
```
Applying this process carefully results in the code visible in `../data/24/adapted_input.txt`.


## Blocks
In the aforementioned file, we can see that the code is separated in blocks. All blocks have the same operations:
```
w = read()
x = (z % 26 + some_value == some_other_value)
y = f(x,w)
z = f(x,y,z)
```
The only exception is the first block, which can be further simplified due to the fact that we know the value of `z` to be zero at the start of the program.


The most important observation is that only the value of z carries forward from block to block, with `w`, `x` and `y` being overwritten every time. Hence each block can be simplified to:
```
w = read()
z = f(w,z)
```

The exact function `f(z,w)` for every block can be seen in `deprecated/simulate.c`. There we can see them fully simplified. We see how two tipes of blocks appear.

## Pushing and popping

The first type of block is a *pushing block*. It has a function of type:
```
f_i(w, z) = 26z + w + k_i
```
where `k_i` is some value between 0 and 16. This means that `w+k` is always a value within [1, 25].

The second type of block is a *popping block*. It has a function of type:
```
           {   z/26 + w + k_i       if z%26 + c_i != w
f_i(w,z) = {
           {   z/26                 if z%26 + c_i == w
```
where `k_i` is again a value between 0 and 16 and hence `w+k` is within [1, 25].

These two operations are the basic operations of a stack. The assembly code is operating on a stack by using a number in base 26:
- The operation to see the top of the stack is the modulo 26.
- The operation to pop the stack is the integer division by 26.
- The operation to push onto the stack is to multiply by 26 and sum the pushed value (so long as the pushed value is less than 26).

Hence, every *pushing block* is adding a digit at the end of the number, and every *popping block* is removing the last digit, and conditionally adding a new one.

## Analyzing the blocks
We can then classify our blocks. With my input we see the following pattern:

| Block | Type | k  | c   |
|-------|------|----|-----|
| 0     | push | 8  |     |
| 1     | push | 16 |     |
| 2     | push | 4  |     |
| 3     | pop  | 1  | -11 |
| 4     | push | 13 |     |
| 5     | push | 5  |     |
| 6     | push | 0  |     |
| 7     | pop  | 10 | -5  |
| 8     | push | 7  |     |
| 9     | pop  | 2  |  0  |
| 10    | pop  | 13 | -11 |
| 11    | pop  | 15 | -13 |
| 12    | pop  | 14 | -13 |
| 13    | pop  | 9  | -11 |

An important detail to notice is that there are 7 push and seven pop operations. We also know that the values in the stack are never zero, since pushing operations always push `w_i+k_i`, and this value is bounded by `[1,9] + [0,16] = [1,25]`.

The conclusion is that every value that is pushed must be popped for the end result to be zero. Therefore, the comparison in every *popping block* must be equal, else the stack won't be empty at the end of the last block.

The stack must therefore evolve as:

| Block | Operation | Stack |
|-------|-----------|-------|
| 0     | push a    | a     |
| 1     | push b    | ab    |
| 2     | push c    | abc   |
| 3     | pop c     | ab    |
| 4     | push e    | abe   |
| 5     | push f    | abf   |
| 6     | push g    | abefg |
| 7     | pop g     | abef  |
| 8     | push i    | abefi |
| 9     | pop i     | abef  |
| 10    | pop f     | abe   |
| 11    | pop e     | ab    |
| 12    | pop b     | a     |
| 13    | pop a     |       |

where each letter `a..j` is the result of that step's `w_i + k_i`.

## Constraining the values
We now take each popping block and enforce the comparison:
```
pop() - c_i == w_i
```
Using the previous table to know what value is being popped, this allows us to obtain the following relationships:

| Block | Condition       | Expanded       |
|-------|-----------------|----------------|
| 3     | `c - 11 == w3`  | `w3  = w2 - 7` |
| 7     | `g -  5 == w7`  | `w7  = w6 - 5` |
| 9     | `c +  0 == w9`  | `w9  = w8 + 7` |
| 10    | `f - 11 == w10` | `w10 = w5 - 6` |
| 11    | `e - 13 == w11` | `w11 = w4 + 0` |
| 12    | `b - 13 == w12` | `w12 = w1 + 3` |
| 13    | `a - 11 == w13` | `w13 = w0 - 3` |

Hence we have the values for 7 of the digits. Let's now look at the expanded form of the 3rd block. Knowing that digit \#3 is digit \#2 minus 7, the latter must be at least 8, since any lesser would mean that the former is 0 or less.

Applying this logic we obtain the following table:

| Digit | Lower bound | Upper bound |
|-------|-------------|-------------|
| 0     | 4           |  9          |
| 1     | 1           |  6          |
| 2     | 8           |  9          |
| 4     | 1           |  9          |
| 5     | 7           |  9          |
| 6     | 6           |  9          |
| 8     | 1           |  2          |

## Result

Hence, in order to compute the maximum possible value, we take the upper bound for all the bounded digits, and compute the conditional ones. In order to compute the minimum, we simply use the lower bound.
- Maximum: `969x999x2xxxxx`
- Minimum: `418x176x1xxxxx`

After computing the popping digits:
- Maximum: `96929994293996`
- Minimum: `41811761181141`
