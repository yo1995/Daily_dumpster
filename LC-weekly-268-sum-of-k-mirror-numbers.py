from collections import defaultdict

n = 11

def numberToBaseBool(n, b) -> bool:
    digits = []
    while n:
        digits.append(int(n % b))
        n //= b
    return digits[::-1] == digits

def numberToBase(n, b) -> str:
    digits = []
    while n:
        digits.append(int(n % b))
        n //= b
    return ''.join(str(d) for d in digits[::-1])


# Generate all palindromic numbers less than n
def createPalindrome(inp, b, isOdd):
    n = inp
    palindrome = inp
    if isOdd:
        n = n // b

    while n > 0:
        palindrome = palindrome * b + (n % b)
        n = n // b
    return palindrome

def generatePalindromes(n):
    result = []
    for j in range(2):
        i = 1
        while (createPalindrome(i, 10, j % 2) < n):
            result.append(createPalindrome(i, 10, j % 2))
            i = i + 1
    return sorted(result)

# Driver Program to test above function
ll = generatePalindromes(100000000000)

#This code is contributed by Afzal Ansari

d = defaultdict(list)
for k in range(2, 10):
    print(d[k-1])
    i = 0
    for num in ll:
        s = str(num)
        if numberToBaseBool(num, k) and s == s[::-1]:
            i += 1
            d[k].append(num)
            if len(d[k]) == 30:
                # print(f'{k} has 30 nums.')
                break

print(d)

# for k in d:
#     values = d[k]
#     print(k)
#     for value in values:
#         print(value, numberToBase(value, k))
