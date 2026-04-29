#include <arpa/inet.h>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <string_view>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

class TcpSocket {
  private:
    int fd_{-1};

  public:
    TcpSocket() = default;
    explicit TcpSocket(int fd) : fd_(fd) {}
    ~TcpSocket()
    {
        if (fd_ != -1) {
            close(fd_);
        }
    }

    TcpSocket(const TcpSocket &) = delete;
    auto operator=(const TcpSocket &) -> TcpSocket & = delete;

    TcpSocket(TcpSocket &&other) noexcept : fd_(other.fd_) { other.fd_ = -1; }
    auto operator=(TcpSocket &&other) noexcept -> TcpSocket &
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

    [[nodiscard]] auto get() const -> int { return fd_; }
    [[nodiscard]] auto is_valid() const -> bool { return fd_ != -1; }
};

class RedisSession {
  private:
    TcpSocket socket_;

  public:
    explicit RedisSession(TcpSocket socket) : socket_(std::move(socket)) {}

    void run()
    {
        std::array<char, 1024> buffer{};
        constexpr std::string_view res = "+PONG\r\n";

        while (recv(socket_.get(), buffer.data(), buffer.size(), 0) > 0) {
            send(socket_.get(), res.data(), res.size(), MSG_NOSIGNAL);
        }
    }
};

class RedisServer {
  private:
    TcpSocket server_socket_;
    int port_;

  public:
    explicit RedisServer(int port) : port_(port)
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

    void start()
    {
        std::cout << "Redis server listening on port " << port_ << "...\n";

        struct sockaddr_in client_addr {};
        socklen_t client_addr_len = sizeof(client_addr);

        while (true) {
            int client_fd =
                accept(server_socket_.get(), reinterpret_cast<struct sockaddr *>(&client_addr),
                       &client_addr_len);
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
};

int main(int argc, char **argv)
{
    constexpr int SERVER_PORT = 6379;

    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    try {
        RedisServer server(SERVER_PORT);
        server.start();
    } catch (const std::exception &e) {
        std::cerr << "Fatal Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
