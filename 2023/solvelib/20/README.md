# Day 20
The code contains the solution for day 20, part 1. Part two was solved analytically.

The problem consists of a set of logic gates named "flip-flop" and "conjunction". First off, let's check out the truth table for a conjunction with two inputs:
 
 | Input 1 | Input 2 | Output |
 | ------- | ------- | ------ |
 | LO      | LO      | HI     |
 | LO      | HI      | HI     |
 | HI      | LO      | HI     |
 | HI      | HI      | LO     |

That's right, this is just a NAND gate.

## Analyzing my input
My input had a bit of a curious pattern. The broadcast signaled four flip-flops, and then these flip-flops started a chain of 12 flip-flops each. These occasionally signalled a conjunction. This conjunction would send signals to either one of the previous flip-flops or another conjunction.

This pattern can be seen in the following image. Squares represent flip-flops and circles are conjunctions.

![](assets/graph.svg)

## Flip-flop chains
Imagine four flip-flops connected in sequence as such
```
D <- C <- B <- A
```
We can represent their states as either 1 (high) or 0 (low). Let's show them all together like DCBA, forming a binary number:
```math
0000_2
```
If we signal A with a low signal, it'll flip and pass a high signal to B, which will not change. Yielding:
```math
0001_2
```
Doing so again causes A to flip and signal B with a low beam:
```math
0010_2
```
If we keep doing so we see a pattern emerge:
```math
0011_2
```

```math
0100_2
```

```math
0101_2
```

```math
0111_2
```
That's right, we are counting! Hence the long chains are counting. They add 1 every time the broadcast is triggered.

## Masking
Let's look at the left chain in my input. We see that certain flip-flops send a signal to `QQ`, the conjunction at the bottom.

A high signal from the conjunction back to the flip-flops will be ignored (per the rules of flip-flops), indeed this is its default state. Hence, we are interested in when it goes low.

It'll become low when the selected flip-flops are high. `QQ` listens to the 1st, 2nd, 3rd, 4th, 7th, 10th, and 12th flip-flops (counting from the bottom). Expressed as a bit mask, this is `111100100101`<sub>2</sub>.

Let's now express the entire counter as a number. Each flip-flop is a bit, with the bottom one (`TR`) being the most significant digit. We'll call it `X`. Then `QQ` will be low only when the following equation holds:
```math
m := 111100100101_2
```

```math
B \text{ is low} \iff X + m = m
```

Note the syntax: and(`x`), or(`+`), and not(`¬`).

Since the chain is just a counter, we know that `QQ` will become low when the counter reaches the smallest number that fits the previous equation. Well, that number is just the mask itself: 3877.

These are the values for all chains:

| Chain | Mask  (binary) | Mask (decimal) |
| ----- | -------------- | -------------- |
| QQ    | 111100100101   | 3877           |
| JC    | 111100110001   | 3889           |
| FJ    | 111101001101   | 3917           |
| VM    | 111011011011   | 3803           |

## Resetting

When `QQ` is finally low, it sends a signal back to the chain; but notice the pattern: it signals the top of the chain plus every flip-flop that did not belong to the mask. You can verify that this is true for all four chains.

This means that we take the number that triggered `QQ`, and flip the bottom bit (setting it to 0) and every non-mask bit (setting them to 1).

Hence, after detecting the mask, the number becomes:
```math
111111111110_2
```
This number is essentially equivalent to -1 because after two iterations we'll overflow to 1:
```math
111111111110_2 + 1 = 111111111111_2   \mod 111111111111_2
```

```math
111111111111_2 + 1 = 000000000001_2   \mod 111111111111_2
```
The chain is ready to count up to the mask again!

## Summarizing
We see that each of the four chains is just counting up to different amounts and starting over. For every broadcast, the counter is increased by one. The counts are just the masks seen in the previous table. Once this mask value is reached, the counter resets to -1 and the bottom conjunction sends a LOW pulse.

## The reduction
Check out the bottom part of the graph. We have established that conjunctions are just NANDs, so we can see that the following equation holds:
```math
XM = \neg(FT \times SV \times JZ \times NG)
```

A NAND with a single input is just a NOT, hence:
```math
XM = \neg(\neg QQ \times \neg JC \times \neg FJ \times \neg VM)
```

Some boolean algebra helps us here:
```math
XM = QQ + JC + FJ + VM
```
That is right, XM is on as long as any of the counters is not at its maximum. Since RX needs a low pulse, we need all counters to reach their maxima simultaneously.

## Conclusion
With all this explained, we can model our problem as a logical circuit:

![](assets/diagram.svg)

So, how many buttons do we need? That'll be the least common multiple of the three counters:
```
LCM(3877, 3889, 3917, 3803) = 224602011344203
```

And that is the solution to the problem.