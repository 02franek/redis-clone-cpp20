#pragma once

#include <functional>
#include <string>
#include <unordered_map>

using CommandHandler = std::function<std::string(const std::vector<std::string> &)>;

class CommandDispatcher {
  private:
    std::unordered_map<std::string, CommandHandler> handlers_;
    static auto to_upper(std::string) -> std::string;

  public:
    CommandDispatcher();

    [[nodiscard]] auto execute(const std::vector<std::string> &tokens) const -> std::string;
};
