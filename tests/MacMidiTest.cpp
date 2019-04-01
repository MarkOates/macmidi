
#include <gtest/gtest.h>

#include <MacMidi.hpp>

TEST(MacMidiTest, initialize__works_without_raising_an_exception)
{
   MacMidi program_runner;
   program_runner.initialize();
}

