#include "RedisServer.hpp"
#include <exception>
#include <iostream>

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
