import original
import simplified

def ExtendA(num):
    A = [0] * 14
    for i in range(14):
        A[13 - i] = num % 10
        num //= 10
    return A


generator = lambda x: int(((x + 123456531335219)*x) % 1e14)
nums = [generator(x)  for x in range(65534) if 0 not in ExtendA(generator(x))]
y = 0
n = 0
for num in nums:
    try:
        A = ExtendA(num)
        orig = original.Run(A)
        new  = simplified.Run(A)
        assert(orig == new)
        y += 1
    except:
        print(orig, "!=",new)
        n += 1
print("=================================================")
print(f"Correct:   {y:>5}")
print(f"Incorrect: {n:>5}")