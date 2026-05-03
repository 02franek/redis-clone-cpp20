#include "../src/CommandDispatcher.hpp"
#include <gtest/gtest.h>

/*
 * Happy path
 */
TEST(CommandDispatcherTest, ExecutesPingCommand)
{
    CommandDispatcher dispatcher;
    std::vector<std::string> tokens = {"PING"};
    std::string response = dispatcher.execute(tokens);

    EXPECT_EQ(response, "+PONG\r\n");
}

TEST(CommandDispatcherTest, ExecutesEchoCommandValid)
{
    CommandDispatcher dispatcher;
    std::vector<std::string> tokens = {"ECHO", "hello world"};
    std::string response = dispatcher.execute(tokens);

    EXPECT_EQ(response, "$11\r\nhello world\r\n");
}

TEST(CommandDispatcherTest, IsCaseInsensitive)
{
    CommandDispatcher dispatcher;
    std::vector<std::string> tokens1 = {"pinG"};
    std::vector<std::string> tokens2 = {"eChO", "test"};

    EXPECT_EQ(dispatcher.execute(tokens1), "+PONG\r\n");
    EXPECT_EQ(dispatcher.execute(tokens2), "$4\r\ntest\r\n");
}

/*
 * Edge cases, errors
 */
TEST(CommandDispatcherTest, RejectsEchoWithoutArguments)
{
    CommandDispatcher dispatcher;
    std::vector<std::string> tokens = {"ECHO"};
    std::string response = dispatcher.execute(tokens);

    EXPECT_EQ(response, "-ERR wrong number of arguments for ECHO command\r\n");
}

TEST(CommandDispatcherTest, HandlesUnknownCommand)
{
    CommandDispatcher dispatcher;
    std::vector<std::string> tokens = {"notacommand", "arg"};
    std::string response = dispatcher.execute(tokens);

    EXPECT_EQ(response, "-ERR unknown command 'NOTACOMMAND'\r\n");
}

TEST(CommandDispatcherTest, HandlesEmptyTokens)
{
    CommandDispatcher dispatcher;
    std::vector<std::string> tokens = {};
    std::string response = dispatcher.execute(tokens);

    EXPECT_EQ(response, "-ERR empty command\r\n");
}
