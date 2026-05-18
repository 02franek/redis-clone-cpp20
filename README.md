# Redis Clone in Modern C++20

A lightweight, multi-threaded Redis server implemented from scratch in modern C++20. This project is actively being developed to deepen my understanding of network programming, concurrent execution, and modern C++ paradigms.

> **Note:** This is a Work in Progress (WIP). Currently, it handles basic RESP (REdis Serialization Protocol) parsing and fundamental commands, serving as a solid foundation for more complex features.

## 🚀 Current Features

*   **TCP/IP Networking:** Custom POSIX socket wrapper utilizing RAII for safe resource management.
*   **Concurrency:** Multi-threaded client handling (`std::thread` per connection).
*   **RESP Parser:** A fast, zero-copy parser utilizing C++20 `std::string_view` and `std::from_chars`.
*   **Command Dispatcher:** A highly modular, lambda-based command router using `std::unordered_map` for $O(1)$ command lookups.
*   **Supported Commands:**
    *   `PING`
    *   `ECHO`

## 🛠️ Tech Stack & Tools

*   **Language:** C++20
*   **Build System:** CMake
*   **Package Manager:** vcpkg
*   **Testing:** Google Test (GTest)

## 🏗️ Architecture Overview

The codebase is structured to enforce separation of concerns:
1.  `TcpSocket` & `RedisSession`: Handles the dirty work of Linux OS-level networking, abstracting file descriptors safely using RAII.
2.  `RespParser`: A pure-logic, header-only class responsible for decoding incoming TCP byte streams into a structured format (Tokenization).
3.  `CommandDispatcher`: Maps incoming string tokens to executable lambda functions (Command Pattern).

## 💻 Getting Started

### Prerequisites
*   A C++20 compatible compiler (GCC 10+ or Clang 10+)
*   CMake (3.15+)
*   vcpkg (for resolving GTest)

### Building the Project

```bash
# Clone the repository
git clone https://github.com/02franek/redis-clone-cpp20.git
cd redis-clone-cpp20

# Configure the project with CMake (ensure vcpkg toolchain is linked)
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[path_to_your_vcpkg]/scripts/buildsystems/vcpkg.cmake

# Build the server and tests
cmake --build build -j 4
```

### Running the server 

```bash
# Start the Redis server (listens on port 6379 by default)
./build/redis
```

You can test the server using standard `redis-cli` or `netcat`:
```bash
redis-cli PING
# Output: PONG
```

### Running Tests

The pure-logic components(`RespParser`, `CommandDispatcher`) are covered by unit tests.
```bash
./build/redis_tests
```
## Roadmap
*   **Key-Value Store**: Implement `SET` and `GET` commands.
*   **Expiration**: Add TTL support for keys.
*   **Architecture refactor**: Migrate from thread-per-client to `thread pool` or `event loop` model.
*   **CI/CD**: Add GitHub Actions pipeline for automated CMake builds and GTest testing.
