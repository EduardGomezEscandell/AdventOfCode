def Run(A, A_dict):
    z = RunPart0(A[0x0], 0)
    z = RunPart1(A[0x1], z)
    z = RunPart2(A[0x2], z)
    z = RunPart3(A[0x3], z)
    if z in A_dict:
        A_dict[z] += 1
    A_dict[z] = 1

    z = RunPart4(A[0x4], z)
    z = RunPart5(A[0x5], z)
    z = RunPart6(A[0x6], z)
    z = RunPart7(A[0x7], z)
    z = RunPart8(A[0x8], z)
    z = RunPart9(A[0x9], z)
    z = RunPartA(A[0xA], z)
    z = RunPartB(A[0xB], z)
    z = RunPartC(A[0xC], z)
    z = RunPartD(A[0xD], z)
    return z

def RunPart0(digit, z):
    return digit + 8

def RunPart1(digit, z):
    return 26*z + digit + 16

def RunPart2(digit, z):
    return 26*z + digit + 4

def RunPart3(digit, z):
    c3 = (z%26 - 11) != digit
    return z//26 * (25*c3 + 1) + (digit + 1)*c3

def RunPart4(digit, z):
    return 26*z + digit + 13

def RunPart5(digit, z):
    return 26*z + digit + 5

def RunPart6(digit, z):
    return 26*z + digit

def RunPart7(digit, z):
    c7 = (z%26 - 5) != digit
    return (z//26)*(25*c7+1) + (digit+10)*c7

def RunPart8(digit, z):
    return 26*z + digit+7

def RunPart9(digit, z):
    c9 = (z%26) != digit
    return (z//26) * (25*c9 + 1) + (digit + 2)*c9

def RunPartA(digit, z):
    cA = (z%26 - 11) != digit
    return (z//26)*(25*cA + 1) + (digit+13)*cA

def RunPartB(digit, z):
    cB = (z%26 - 13) != digit
    return (z//26)*(25*cB + 1) + (digit + 15)*cB

def RunPartC(digit, z):
    cC = (z%26 - 13) != digit
    return (z//26)*(25*cC + 1) + (digit + 14)*cC
    
def RunPartD(digit, z):
    # Part 13
    cD = ((z%26) - 11) != digit
    return (z//26) * (25*cD + 1) + (digit + 9)*cD


def Validate(digit):
    if (digit < 1) or (digit>9):
        raise RuntimeError()
    return digit


def ReversePartC(z):
    if z < 12 or z > 20:
        raise RuntimeError()
    return [Validate(i) for i in range(1,10)]

def ReversePartD(z):
    if z > 26:
        raise RuntimeError()
    return [Validate(z - 11)]



correct_pairs = []
for d_c in range(1,10):        
    for z_c in range(1024):
        try:
            z_d = RunPartC(d_c, z_c)

            for d_d in ReversePartD(z_d):
                r = RunPartD(d_d, z_d)

        except RuntimeError:
            r = 1

        if r==0:
            correct_pairs.append((z_c, d_c))
            print(f"z={z_c}, d={d_c}{d_d}")

for z in range(1024):
    try:
        for d_c in ReversePartC(z):
            r = RunPartC(d_c, z)

            for d_d in ReversePartD(z):
                r = RunPartD(d_d, z)

    except RuntimeError:
        r = 1

    if r==0 and (z, d_c) not in correct_pairs:
        print(f"Error! False positive: (z={z}, d={d_c}{d_d}) ")

    if(r!=0 and (z, d_c) in correct_pairs):
        print(f"Error! False begative: (d={d}, d={d_c}{d_d})")

print("Done")



if __name__ == "__main__":
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
    A_dict = {}
    for num in nums:
        try:
            A = ExtendA(num)
            orig = bool(original.Run(A))
            new  = bool(simplified.Run(A,A_dict))
            assert(orig == new)
            y += 1
        except:
            print(orig, "!=",new)
            n += 1
    print("=================================================")
    print(f"Correct:   {y:>5}")
    print(f"Incorrect: {n:>5}")