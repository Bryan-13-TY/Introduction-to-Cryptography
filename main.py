h = 98383784637647324732

for i in range(10, 0):
    print(f"{(h >> i) & 1}", end="")