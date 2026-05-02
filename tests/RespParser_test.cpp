#include "../src/RespParser.hpp"
#include <gtest/gtest.h>

/*
 * Happy path
 */
TEST(RespParserTest, ParsesValidEchoCommand)
{
    std::string_view input = "*2\r\n$4\r\nECHO\r\n$9\r\nGreetings\r\n";

    auto tokens = RespParser::parse(input);

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0], "ECHO");
    EXPECT_EQ(tokens[1], "Greetings");
}

TEST(RespParserTest, ParsesValidPingCommand)
{
    std::string_view input = "*1\r\n$4\r\nPING\r\n";

    auto tokens = RespParser::parse(input);

    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0], "PING");
}

/*
 * Edge cases, errors
 */
TEST(RespParserTest, ReturnsEmptyOnMissingRespArrayPrefix)
{
    std::string_view input = "PING\r\n";

    auto tokens = RespParser::parse(input);

    EXPECT_TRUE(tokens.empty());
}

TEST(RespParserTest, ReturnsEmptyOnIncompleteData)
{
    std::string_view input = "*2\r\n$4\r\nECHO\r\n";

    auto tokens = RespParser::parse(input);

    EXPECT_TRUE(tokens.empty());
}

TEST(RespParserTest, ReturnsEmptyOnMissingCRLF)
{
    std::string_view input = "*2\r\n$4\r\nECHO\r\n$5\r\nHELLO\n";

    auto tokens = RespParser::parse(input);

    EXPECT_TRUE(tokens.empty());
}

TEST(RespParserTest, ReturnsEmptyOnStringLengthMismatch)
{
    std::string_view input = "*2\r\n$4\r\nECHO\r\n$10\r\nHELLO\r\n";

    auto tokens = RespParser::parse(input);

    EXPECT_TRUE(tokens.empty());
}
