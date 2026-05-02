#pragma once

#include <charconv>
#include <string>
#include <string_view>
#include <vector>

class RespParser {
  public:
    static auto parse(std::string_view buffer) -> std::vector<std::string>
    {
        std::vector<std::string> tokens;
        size_t pos = 0;

        if (pos >= buffer.size() || buffer[pos] != '*') {
            return {};
        }
        ++pos;

        size_t crlf_pos = buffer.find("\r\n", pos);
        if (crlf_pos == std::string_view::npos) {
            return {};
        }

        int element_count{};
        std::from_chars(buffer.data() + pos, buffer.data() + crlf_pos, element_count);
        pos = crlf_pos + 2;

        for (int i = 0; i != element_count; ++i) {
            if (pos >= buffer.size() || buffer[pos] != '$') {
                return {};
            }
            ++pos;

            crlf_pos = buffer.find("\r\n", pos);
            if (crlf_pos == std::string_view::npos) {
                return {};
            }

            int str_length{};
            std::from_chars(buffer.data() + pos, buffer.data() + crlf_pos, str_length);
            pos = crlf_pos + 2;

            // Check if buffer contains enough data to extract token and discard separator ("\r\n")
            if (pos + 2 + static_cast<size_t>(str_length) > buffer.size()) {
                return {};
            }

            tokens.emplace_back(buffer.substr(pos, static_cast<size_t>(str_length)));
            pos += static_cast<size_t>(str_length);

            if (buffer.substr(pos, 2) != "\r\n") {
                return {};
            }
            pos += 2;
        }
        return tokens;
    }
};
