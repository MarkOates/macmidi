
#include <MacMidiDeviceEnumerator.hpp>

int main(int argc, char **argv)
{
   MacMidiDeviceEnumerator mac_midi_device_enumerator;
   mac_midi_device_enumerator.enumerate_devices();
   return 0;
}
