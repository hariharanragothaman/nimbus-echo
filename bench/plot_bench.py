# plot_bench.py
import json, pandas as pd, matplotlib.pyplot as plt, sys
data = json.load(open("lat.json"))
rows = [ (b["name"], b["real_time"]) for b in data["benchmarks"] ]
df = pd.DataFrame(rows, columns=["Benchmark","us"])
ax = df.plot(kind="bar", x="Benchmark", y="us", legend=False, rot=15)
ax.set_ylabel("µs (lower is better)")
ax.set_title("Round‑trip latency, 64‑byte payload")
plt.tight_layout()
plt.savefig("latency.png", dpi=300)
print("Saved → latency.png")
