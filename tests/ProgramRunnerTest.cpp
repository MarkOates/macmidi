
#include <gtest/gtest.h>

#include <ProgramRunner.hpp>

TEST(ProgramRunnerTest, run__returns_the_expected_response)
{
   ProgramRunner program_runner;
   std::string expected_string = "Hello World!";
   EXPECT_EQ(expected_string, program_runner.run());
}
