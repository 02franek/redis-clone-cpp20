#include "CommandDispatcher.hpp"
#include <algorithm>
#include <cctype>
#include <string>

auto CommandDispatcher::to_upper(std::string s) -> std::string
{
    std::ranges::transform(s, s.begin(), [](unsigned char c) { return std::toupper(c); });
    return s;
}

CommandDispatcher::CommandDispatcher()
{
    handlers_["PING"] = [](const std::vector<std::string> & /*tokens*/) -> std::string {
        return "+PONG\r\n";
    };

    handlers_["ECHO"] = [](const std::vector<std::string> &tokens) -> std::string {
        if (tokens.size() < 2) {
            return "-ERR wrong number of arguments for ECHO command\r\n";
        }
        const auto &arg = tokens[1];
        return "$" + std::to_string(arg.size()) + "\r\n" + arg + "\r\n";
    };
}

auto CommandDispatcher::execute(const std::vector<std::string> &tokens) const -> std::string
{
    if (tokens.empty()) {
        return "-ERR empty command\r\n";
    }

    std::string command = to_upper(tokens[0]);

    if (auto it = handlers_.find(command); it != handlers_.end()) {
        return it->second(tokens);
    }

    return "-ERR unknown command '" + command + "'\r\n";
}
