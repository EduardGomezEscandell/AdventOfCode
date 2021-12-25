def ExtendA(num):
    A = [0] * 14
    for i in range(14):
        A[13 - i] = num % 10
        num //= 10
    return A