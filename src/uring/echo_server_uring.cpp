//
// Created by Hariharan Ragothaman on 5/15/25.
//

// Extremely small io_uring TCP echo server (edge‑triggered)
#include <liburing.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

constexpr uint16_t PORT = 9002;
constexpr unsigned QUEUE_DEPTH = 256;
constexpr unsigned BUF_SZ = 4096;

struct ConnData {
    int fd;
    char buf[BUF_SZ];
};

int main() {
    // 1. Classic BSD socket setup
    int listen_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    sockaddr_in addr{}; addr.sin_family = AF_INET; addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(listen_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    listen(listen_fd, SOMAXCONN);

    // 2. uring setup
    io_uring ring{};
    io_uring_queue_init(QUEUE_DEPTH, &ring, 0);

    // helper lambda: submit an accept sqe
    auto prep_accept = [&]() {
        io_uring_sqe* sqe = io_uring_get_sqe(&ring);
        sockaddr_in* client = new sockaddr_in;
        socklen_t* len      = new socklen_t(sizeof(sockaddr_in));
        io_uring_prep_accept(sqe, listen_fd,
                             reinterpret_cast<sockaddr*>(client), len, SOCK_NONBLOCK);
        io_uring_sqe_set_data(sqe, client);   // stash ptr so we can free later
    };
    prep_accept();
    io_uring_submit(&ring);

    std::cout << "⚡  io_uring TCP echo on 0.0.0.0:" << PORT << '\n';

    // 3. Main completion loop
    while (true) {
        io_uring_cqe* cqe;
        int ret = io_uring_wait_cqe(&ring, &cqe);
        if (ret < 0) { perror("wait_cqe"); break; }

        void* data = io_uring_cqe_get_data(cqe);
        unsigned op = io_uring_cqe_get_res(cqe);

        // Accept completed → op = client_fd
        if (data && data != nullptr && op >= 0 && op < 0xFFFF) {
            int client_fd = op;
            delete static_cast<sockaddr_in*>(data); // free sockaddr
            io_uring_cqe_seen(&ring, cqe);

            // schedule next accept right away
            prep_accept();

            // schedule first read
            ConnData* cd = new ConnData{client_fd, {}};
            io_uring_sqe* r_sqe = io_uring_get_sqe(&ring);
            io_uring_prep_recv(r_sqe, client_fd, cd->buf, BUF_SZ, 0);
            io_uring_sqe_set_data(r_sqe, cd);
            io_uring_submit(&ring);
            continue;
        }

        // Read completed → if >0 bytes, write them back
        ConnData* cd = static_cast<ConnData*>(data);
        if (op > 0) {
            io_uring_sqe* w_sqe = io_uring_get_sqe(&ring);
            io_uring_prep_send(w_sqe, cd->fd, cd->buf, op, 0);
            io_uring_sqe_set_data(w_sqe, cd);  // reuse struct
            io_uring_submit(&ring);
        } else { // client closed
            close(cd->fd);
            delete cd;
        }
        io_uring_cqe_seen(&ring, cqe);
    }
    close(listen_fd);
    io_uring_queue_exit(&ring);
    return 0;
}
