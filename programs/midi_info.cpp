
#include <MacMidiEnumerator.hpp>

int main(int argc, char **argv)
{
   MacMidiEnumerator mac_midi_enumerator;
   mac_midi_enumerator.enumerate_devices();
   return 0;
}
