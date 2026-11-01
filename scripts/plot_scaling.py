"""Plot render time vs. thread count from bench_results.csv and bench_results_atomic.csv into scaling.png."""
import csv
from collections import defaultdict

import matplotlib.pyplot as plt


def load(path):
    data = defaultdict(dict)
    with open(path) as f:
        for scene, threads, seconds in csv.reader(f):
            data[scene][int(threads)] = float(seconds)
    return data


final = load("bench_results.csv")
atomic = load("bench_results_atomic.csv")

fig, (left, right) = plt.subplots(1, 2, figsize=(12, 4.5))

for scene, times in final.items():
    threads = sorted(times)
    left.plot(threads, [times[1] / times[t] for t in threads], marker="o", label=scene)
left.plot([1, 12], [1, 12], "k:", alpha=0.4, label="ideal (linear)")
left.axvline(8, color="gray", alpha=0.3)
left.text(8.2, 1.3, "8 performance cores", color="gray", fontsize=9)
left.set_xlabel("threads")
left.set_ylabel("speedup over 1 thread")
left.set_title("Speedup with per-thread counters (1920x1080)")
left.legend()
left.grid(alpha=0.3)

scene = "dragon"
threads = sorted(final[scene])
right.plot(threads, [atomic[scene][t] for t in threads], marker="o", color="tab:red", label="shared atomic counters")
right.plot(threads, [final[scene][t] for t in threads], marker="o", color="tab:green", label="per-thread counters")
right.set_xlabel("threads")
right.set_ylabel("render time (s)")
right.set_title("Dragon: why the stats counters mattered")
right.legend()
right.grid(alpha=0.3)

fig.tight_layout()
fig.savefig("scaling.png", dpi=150)
print("wrote scaling.png")
