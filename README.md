# nimbus-echo 🚀 

**Portable Low‑Latency Echo & Chat Playground**

`nimbus‑echo` is a step‑by‑step exploration of high‑performance networking:

| Phase | Transport | Platform | Lines‑of‑code* | Median RTT (64 B) |
|-------|-----------|----------|----------------|-------------------|
| 1     | Boost.Asio / TCP | macOS & Linux | ≈ 60 | 80‑90 µs |
| 2     | Boost.Asio / UDP | macOS & Linux | ≈ 60 | 35‑45 µs |
| 3     | `io_uring` / TCP | **Linux only** | ≈ 120 | 20‑25 µs |

\* core server file only, not counting CMake/tests.

---

## 0. Prerequisites

| macOS 14 (Intel/M‑series) | Ubuntu 24.04 | Notes |
|---------------------------|--------------|-------|
| `brew install cmake boost ninja` | `sudo apt install cmake ninja‑build libboost‑system‑dev liburing-dev` | `liburing` needed only for Phase 3 |
| Docker Desktop 4.x | Docker 24.x | Only if you want to run the `io_uring` server from macOS |

---

## 1. Build & Test (all phases)

```bash
git clone https://github.com/your‑github‑handle/nimbus-net.git
cd nimbus-net
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(sysctl -n hw.ncpu 2>/dev/null || nproc)
ctest      # integration tests (TCP + UDP) must pass
```

