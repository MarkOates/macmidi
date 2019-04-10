//
// Programmer:	Craig Stuart Sapp
// Date:	Mon Jun  8 14:54:42 PDT 2009
// Filename:	testout.c
// Syntax:	C; Apple OSX CoreMIDI
// $Smake:	gcc -o %b %f -framework CoreMIDI -framework CoreServices
//              note: CoreServices needed for GetMacOSSStatusErrorString().
//
// Description:	This program plays two MIDI notes (middle C, and C#) on all 
//              MIDI output ports which the program can find. Similar to
//              testout.c, but writes all MIDI messages to CoreMIDI at
//              the same time and relies on time stamps for the timing
//              of the notes rather than sleep().
//
// Derived from "Audio and MIDI on Mac OS X" Preliminary Documentation, 
// May 2001 Apple Computer, Inc. found in PDF form on the developer.apple.com
// website, as well as using links at the bottom of the file.
//

#include <iostream>
#include <CoreMIDI/CoreMIDI.h>    /* interface to MIDI in Macintosh OS X */
#include <unistd.h>               /* for sleep() function                */
#include <mach/mach_time.h>       /* for mach_absolute_time()            */
#define MESSAGESIZE 3             /* byte count for MIDI note messages   */

void playPacketListOnAllDevices   (MIDIPortRef     midiout, 
                                   const MIDIPacketList* pktlist);

#define ZERO_NULL 0

/////////////////////////////////////////////////////////////////////////

#define BYTE unsigned char


void play_note_on(MIDIPortRef &midiout, BYTE pitch, BYTE velocity=90)
{
   // Prepare a MIDI Note-On message to send 
   BYTE buffer[64];             // storage space for MIDI Packets (max 65536)
   MIDIPacketList *packetlist = (MIDIPacketList*)buffer;
   MIDIPacket *currentpacket = MIDIPacketListInit(packetlist);

   BYTE notemessage[MESSAGESIZE] = {0x90, pitch, velocity};
   currentpacket = MIDIPacketListAdd(packetlist, sizeof(buffer), currentpacket, mach_absolute_time(), MESSAGESIZE, notemessage);

   playPacketListOnAllDevices(midiout, packetlist);
}


void play_note_off(MIDIPortRef &midiout, BYTE pitch)
{
   // Prepare a MIDI Note-On message to send 
   BYTE buffer[64];             // storage space for MIDI Packets (max 65536)
   MIDIPacketList *packetlist = (MIDIPacketList*)buffer;
   MIDIPacket *currentpacket = MIDIPacketListInit(packetlist);


   // Prepare a MIDI Note-OFF message to send 
   BYTE note_message_off[MESSAGESIZE] = {0x90, pitch, 0x00};
   currentpacket = MIDIPacketListAdd(packetlist, sizeof(buffer), currentpacket, mach_absolute_time(), MESSAGESIZE, note_message_off);

   playPacketListOnAllDevices(midiout, packetlist);
}



void sleep(float seconds)
{
   usleep(seconds * 1000000);
}



int main(void) {

   // Prepare MIDI Interface Client/Port for writing MIDI data:
   MIDIClientRef midiclient  = ZERO_NULL;
   MIDIPortRef   midiout     = ZERO_NULL;
   OSStatus status;

   if ((status = MIDIClientCreate(CFSTR("TeStInG"), NULL, NULL, &midiclient))) {
       printf("Error trying to create MIDI Client structure: %d\n", status);
       //printf("%s\n", GetMacOSStatusErrorString(status));
       exit(status);
   }
   if ((status = MIDIOutputPortCreate(midiclient, CFSTR("OuTpUt"), &midiout))) {
       printf("Error trying to create MIDI output port: %d\n", status);
       //printf("%s\n", GetMacOSStatusErrorString(status));
       exit(status);
   }


   // https://www.midi.org/specifications-old/item/table-1-summary-of-midi-message

   BYTE byte_pitch = 60;

   play_note_on(midiout, byte_pitch);

   sleep(0.5f);

   play_note_off(midiout, byte_pitch);


   /*
   //setup_a_second_note(Byte *notemessage, int channel_num_2, int pitch_diff, MIDITimeStamp &timestamp, MIDIPacket *currentpacket, Byte *buffer, MIDIPacketList *packetlist);
   setup_a_second_note(notemessage, channel_num_2, pitch_diff, timestamp, currentpacket, buffer, packetlist);
   //setup_a_second_note();
   // setup another note to play one second later with same loudness
   //notemessage[0] = channel_num_2;   // channel
   //notemessage[1] = 67-pitch_diff;            // pitch = G4
   //timestamp += 1000000000;        // one billion nanoseconds later
   //currentpacket = MIDIPacketListAdd(packetlist, sizeof(buffer), 
          //currentpacket, timestamp, MESSAGESIZE, notemessage);

   // turn off the second note played one second later
   notemessage[0] = channel_num_2;   // channel
   notemessage[1] = 67-pitch_diff;            // pitch = G4
   notemessage[2] = 0;             // turn off the note
   timestamp += 1000000000;        // one billion nanoseconds later
   currentpacket = MIDIPacketListAdd(packetlist, sizeof(buffer), 
          currentpacket, timestamp, MESSAGESIZE, notemessage);

   // turn off the first note played one second later
   notemessage[0] = channel_num;   // channel
   notemessage[1] = 60-pitch_diff;            // pitch = C4
   notemessage[2] = 0;             // turn off the note
   timestamp += 1000000000;        // one billion nanoseconds later
   currentpacket = MIDIPacketListAdd(packetlist, sizeof(buffer), 
          currentpacket, timestamp, MESSAGESIZE, notemessage);

   // send the MIDI data and don't wait around for a little while
   // to see what happens.
   */

   //playPacketListOnAllDevices(midiout, packetlist);


   if ((status = MIDIPortDispose(midiout))) {
      printf("Error trying to close MIDI output port: %d\n", status);
      //printf("%s\n", GetMacOSStatusErrorString(status));
      exit(status);
   }
   midiout = ZERO_NULL;

   if ((status = MIDIClientDispose(midiclient))) {
      printf("Error trying to close MIDI client: %d\n", status);
      //printf("%s\n", GetMacOSStatusErrorString(status));
      exit(status);
   }
   midiclient = ZERO_NULL;

   printf("Program appears to have run successfully.");

   return 0;
}


/////////////////////////////////////////////////////////////////////////

//////////////////////////////
//
// playPacketOnAllDevices -- play the list of MIDI packets
//    on all MIDI output devices which the computer knows about.
//    (Send the MIDI message(s) to all MIDI out ports).
//

void playPacketListOnAllDevices(MIDIPortRef midiout, 
      const MIDIPacketList* pktlist) {
   // send MIDI message to all MIDI output devices connected to computer:
   std::cout << "Playing packet list..." << std::endl;
   ItemCount nDests = MIDIGetNumberOfDestinations();
   ItemCount iDest;
   OSStatus status;
   MIDIEndpointRef dest;
   for(iDest=0; iDest<nDests; iDest++) {
      dest = MIDIGetDestination(iDest);
      if ((status = MIDISend(midiout, dest, pktlist))) {
          printf("Problem sendint MIDI data on port %d\n", (int)iDest);
          //printf("%s\n", GetMacOSStatusErrorString(status));
          exit(status);
      }
   }
   std::cout << "... Finished packet list." << std::endl;
}
