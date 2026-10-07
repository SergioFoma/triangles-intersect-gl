"""Generate spatially uniform triangle centers in the reader's text format."""

import argparse
from pathlib import Path

import numpy as np


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--count", type=int, default=10_000_000)
    parser.add_argument("--seed", type=int, default=42)
    parser.add_argument("--output", type=Path,
                        default=Path("materials/triangles_10000000.txt"))
    args = parser.parse_args()
    if not 0 < args.count <= 10_000_000:
        parser.error("count must be between 1 and 10000000")

    rng = np.random.default_rng(args.seed)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    with args.output.open("w") as output:
        output.write(f"{args.count}\n")
        for start in range(0, args.count, 100_000):
            size = min(100_000, args.count - start)
            centers = rng.uniform(-50.0, 50.0, (size, 1, 3))
            offsets = rng.uniform(-0.25, 0.25, (size, 3, 3))
            offsets -= offsets.mean(axis=1, keepdims=True)
            vertices = centers + offsets
            np.savetxt(output, vertices.reshape(size, 9), fmt="%.6f")
            print(f"{start + size}/{args.count}", flush=True)


if __name__ == "__main__":
    main()
