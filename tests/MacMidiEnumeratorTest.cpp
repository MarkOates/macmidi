
#include <gtest/gtest.h>

#include <MacMidiEnumerator.hpp>

TEST(MacMidiEnumeratorTest, get_num_devices__returns_the_number_of_connected_midi_devices)
{
   MacMidiEnumerator mac_midi_enumerator;
   ASSERT_EQ(3, mac_midi_enumerator.get_num_devices());
}

TEST(MacMidiEnumeratorTest, enumerate_devices__returns_a_list_of_the_devices_with_their_info)
{
   MacMidiEnumerator mac_midi_enumerator;
   mac_midi_enumerator.enumerate_devices();
}

