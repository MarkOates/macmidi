
#include <gtest/gtest.h>

#include <MacMidi.hpp>

TEST(MacMidiTest, initialize__works_without_raising_an_exception)
{
   MacMidi program_runner;
   program_runner.initialize();
}

TEST(MacMidiTest, get_num_devices__returns_the_number_of_connected_midi_devices)
{
   MacMidi mac_midi;
   ASSERT_EQ(3, mac_midi.get_num_devices());
}

