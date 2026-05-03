#include "RedisServer.hpp"
#include "RespParser.hpp"

#include <arpa/inet.h>
#include <array>
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <utility>

TcpSocket::TcpSocket(int fd) : fd_(fd) {}

TcpSocket::~TcpSocket()
{
    if (fd_ != -1) {
        close(fd_);
    }
}

TcpSocket::TcpSocket(TcpSocket &&other) noexcept : fd_(other.fd_)
{
    other.fd_ = -1;
}

auto TcpSocket::operator=(TcpSocket &&other) noexcept -> TcpSocket &
{
    if (this != &other) {
        if (fd_ != -1) {
            close(fd_);
        }
        fd_ = other.fd_;
        other.fd_ = -1;
    }
    return *this;
}

auto TcpSocket::get() const -> int
{
    return fd_;
}
auto TcpSocket::is_valid() const -> bool
{
    return fd_ != -1;
}

RedisSession::RedisSession(TcpSocket socket) : socket_(std::move(socket)) {}

void RedisSession::run()
{
    std::array<char, 1024> buffer{};

    while (recv(socket_.get(), buffer.data(), buffer.size(), 0) > 0) {
        auto tokens = RespParser::parse(buffer.data());

        std::string response = dispatcher_.execute(tokens);

        send(socket_.get(), response.data(), response.size(), MSG_NOSIGNAL);

        buffer.fill(0);
    }
}

RedisServer::RedisServer(int port) : port_(port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    server_socket_ = TcpSocket(fd);

    int reuse_opt = 1;
    setsockopt(server_socket_.get(), SOL_SOCKET, SO_REUSEADDR, &reuse_opt, sizeof(reuse_opt));

    struct sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(static_cast<uint16_t>(port_));

    if (bind(server_socket_.get(), reinterpret_cast<struct sockaddr *>(&server_addr),
             sizeof(server_addr)) != 0) {
        throw std::runtime_error("Failed to bind to port");
    }

    if (listen(server_socket_.get(), 10) != 0) {
        throw std::runtime_error("Listen failed");
    }
}

void RedisServer::start()
{
    std::cout << "Redis server listening on port " << port_ << "...\n";

    struct sockaddr_in client_addr {};
    socklen_t client_addr_len = sizeof(client_addr);

    while (true) {
        int client_fd = accept(server_socket_.get(),
                               reinterpret_cast<struct sockaddr *>(&client_addr), &client_addr_len);
        if (client_fd < 0) {
            continue;
        }

        TcpSocket client_socket(client_fd);
        std::cout << "Client connected\n";

        std::thread([session = RedisSession(std::move(client_socket))]() mutable {
            session.run();
        }).detach();
    }
}
