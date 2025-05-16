# nimbusnet/Dockerfile
FROM ubuntu:24.04 AS build

RUN apt-get update && \
    apt-get install -y g++ cmake libboost-system-dev liburing-dev git

WORKDIR /opt/nimbusnet
COPY . .

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
 && cmake --build build --target nimbus_uring_echo -j$(nproc)

# ── Runtime image ──────────────────────────────────────────────
FROM ubuntu:24.04
COPY --from=build /opt/nimbusnet/build/src/nimbus_uring_echo /usr/local/bin/
EXPOSE 9002
ENTRYPOINT ["nimbus_uring_echo"]
