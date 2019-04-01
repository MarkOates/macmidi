
#include <MacMidi.hpp>

int main(int argc, char **argv)
{
   MacMidi mac_midi;
   mac_midi.enumerate_devices();
   return 0;
}
