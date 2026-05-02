#pragma once

class TcpSocket {
  private:
    int fd_{-1};

  public:
    TcpSocket() = default;
    explicit TcpSocket(int fd);
    ~TcpSocket();

    TcpSocket(const TcpSocket &) = delete;
    auto operator=(const TcpSocket &) -> TcpSocket & = delete;

    TcpSocket(TcpSocket &&other) noexcept;
    auto operator=(TcpSocket &&other) noexcept -> TcpSocket &;

    [[nodiscard]] auto get() const -> int;
    [[nodiscard]] auto is_valid() const -> bool;
};

class RedisSession {
  private:
    TcpSocket socket_;

  public:
    explicit RedisSession(TcpSocket socket);
    void run();
};

class RedisServer {
  private:
    TcpSocket server_socket_;
    int port_;

  public:
    explicit RedisServer(int port);
    void start();
};
