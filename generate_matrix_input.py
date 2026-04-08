#!/usr/bin/env python3
import argparse
import random
import sys


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Generuje vstup pre programy na nasobenie matic."
    )
    parser.add_argument("--size", type=int, required=True, help="Velkost matice NxN")
    parser.add_argument("--seed", type=int, default=42, help="Nahodny seed")
    args = parser.parse_args()

    n: int = args.size
    if n <= 0:
        raise SystemExit("Velkost musi byt kladne cele cislo.")

    random.seed(args.seed)

    print(f"{n}\n{n}\n{n}")

    # hodnoty matice A
    for _ in range(n * n):
        print(f"{random.uniform(-100.0, 100.0):.6f}")

    # hodnoty matice B
    for _ in range(n * n):
        print(f"{random.uniform(-100.0, 100.0):.6f}")

    print("n")


if __name__ == "__main__":
    main()

