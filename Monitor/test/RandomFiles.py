from random import randint

file_sizes = ["1_000", "10_000", "100_000", "1_000_000",
              "10_000_000", "100_000_000", "1_000_000_000"]

for size in file_sizes:
    filename = f"random_{size}.txt"
    with open(filename, "w") as f:
        for _ in range(int(size)):
            f.write(chr(ord('A') + randint(0, 25)))
        f.write('\n')
    print(filename, "done")