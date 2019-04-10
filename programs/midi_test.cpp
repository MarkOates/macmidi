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

#include <vector>

void playPacketListOnAllDevices   (MIDIPortRef     midiout, 
                                   const MIDIPacketList* pktlist);

#define ZERO_NULL 0

/////////////////////////////////////////////////////////////////////////


#define BYTE unsigned char


class MidiNote
{
public:
   BYTE pitch;
   BYTE velocity;
   BYTE channel;

   MidiNote(BYTE pitch, BYTE velocity, BYTE channel)
      : pitch(pitch)
      , velocity(velocity)
      , channel(channel)
   {}
};



class PitchTransformer
{
public:
   static std::vector<BYTE> reverse(std::vector<BYTE> notes)
   {
      std::reverse(notes.begin(), notes.end());
      return notes;
   }

   static std::vector<BYTE> octave_down(std::vector<BYTE> notes)
   {
      std::vector<BYTE> result;
      for (BYTE &note : notes)
      {
         result.push_back(note + (unsigned char)(-12));
      }
      return result;
   }
};


class MidiContext
{
private:
   MIDIClientRef midiclient;
   MIDIPortRef midiout;

public:
   MidiContext()
      : midiclient(ZERO_NULL)
      , midiout(ZERO_NULL)
   {}

   void initialize()
   {
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
   }

   MIDIPortRef &get_midiout() { return midiout; }
   MIDIClientRef &get_midiclient() { return midiclient; }

   void shutdown()
   {
      OSStatus status;

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
   }

   //
   // playPacketOnAllDevices -- play the list of MIDI packets
   //    on all MIDI output devices which the computer knows about.
   //    (Send the MIDI message(s) to all MIDI out ports).
   //

   static void playPacketListOnAllDevices(MIDIPortRef midiout, const MIDIPacketList* pktlist)
   {
      // send MIDI message to all MIDI output devices connected to computer:
      std::cout << "Playing packet list..." << std::endl;
      ItemCount nDests = MIDIGetNumberOfDestinations();
      ItemCount iDest;
      OSStatus status;
      MIDIEndpointRef dest;
      for(iDest=0; iDest<nDests; iDest++)
      {
         dest = MIDIGetDestination(iDest);
         if ((status = MIDISend(midiout, dest, pktlist))) {
             printf("Problem sendint MIDI data on port %d\n", (int)iDest);
             //printf("%s\n", GetMacOSStatusErrorString(status));
             exit(status);
         }
      }
      std::cout << "... Finished packet list." << std::endl;
   }
};



void play_note_on(MIDIPortRef &midiout, BYTE pitch, BYTE velocity=90, BYTE channel=0x00)
{
   const unsigned int CHANNEL_BITS = 0x0F;
   // Prepare a MIDI Note-On message to send 
   BYTE buffer[64];             // storage space for MIDI Packets (max 65536)
   MIDIPacketList *packetlist = (MIDIPacketList*)buffer;
   MIDIPacket *currentpacket = MIDIPacketListInit(packetlist);

   BYTE notemessage[MESSAGESIZE] = {(BYTE)(0x90 + channel), pitch, velocity};
   currentpacket = MIDIPacketListAdd(packetlist, sizeof(buffer), currentpacket, mach_absolute_time(), MESSAGESIZE, notemessage);

   MidiContext::playPacketListOnAllDevices(midiout, packetlist);
}


void play_note_off(MIDIPortRef &midiout, BYTE pitch, BYTE channel=0x00)
{
   // Prepare a MIDI Note-On message to send 
   BYTE buffer[64];             // storage space for MIDI Packets (max 65536)
   MIDIPacketList *packetlist = (MIDIPacketList*)buffer;
   MIDIPacket *currentpacket = MIDIPacketListInit(packetlist);


   // Prepare a MIDI Note-OFF message to send 
   BYTE note_message_off[MESSAGESIZE] = {(BYTE)(0x90 + channel), pitch, 0x00};
   currentpacket = MIDIPacketListAdd(packetlist, sizeof(buffer), currentpacket, mach_absolute_time(), MESSAGESIZE, note_message_off);

   MidiContext::playPacketListOnAllDevices(midiout, packetlist);
}



void sleep(float seconds)
{
   usleep(seconds * 1000000);
}



class SongFactory
{
public:
   static void play_song_1(MidiContext &midi_context)
   {
      BYTE pitch = 60;
      play_note_on(midi_context.get_midiout(), pitch);
      sleep(0.5f);
      play_note_off(midi_context.get_midiout(), pitch);
   }

   static void play_song_2(MidiContext &midi_context)
   {
      std::vector<BYTE> pitches = {60, 62, 64, 67, 60, 62, 64, 67};
      for (auto &pitch : pitches)
      {
         play_note_on(midi_context.get_midiout(), pitch);
         sleep(0.15f);
         play_note_off(midi_context.get_midiout(), pitch);
      }
   }

   static void play_song_3(MidiContext &midi_context)
   {
      BYTE violin_channel = 0x00;
      BYTE cello_channel = 0x03;

      std::vector<BYTE> pitches = {60, 62, 64, 67, 60, 62, 64, 67};
      for (auto &pitch : pitches)
      {
         BYTE channel = violin_channel;
         play_note_on(midi_context.get_midiout(), pitch, 90, violin_channel);
         sleep(0.15f);
         play_note_off(midi_context.get_midiout(), pitch, violin_channel);
      }

      std::vector<BYTE> pitches2 = {60, 64, 67, 68, 60, 64, 67, 68};
      pitches2 = PitchTransformer::octave_down(pitches2);
      pitches2 = PitchTransformer::reverse(pitches2);
      for (auto &pitch : pitches2)
      {
         BYTE channel = cello_channel;
         play_note_on(midi_context.get_midiout(), pitch, 90, cello_channel);
         sleep(0.15f);
         play_note_off(midi_context.get_midiout(), pitch, cello_channel);
      }
   }

   static void setup_long_soft_patches(MidiContext &midi_context)
   {
      BYTE violin_channel = 0x00;
      BYTE cello_channel = 0x03;
      BYTE bass_channel = 0x04;

      BYTE velocity = 60;

      std::vector<MidiNote> notes = {

         {60-24+1, velocity, violin_channel}, // vel range
         {60-36+1, velocity, violin_channel}, // articulation
         {60-24+5, velocity, violin_channel}, // type

         {60-36+1, velocity, cello_channel}, // vel range
         {60+24+1, velocity, cello_channel}, // articulation
         {60+24+5, velocity, cello_channel}, // type

         {60+24+1, velocity, bass_channel}, // vel range
         {60+12+1, velocity, bass_channel}, // articulation
         {60+24+5, velocity, bass_channel}, // type
      };

      for (auto &note : notes) play_note_on(midi_context.get_midiout(), note.pitch, note.velocity, note.channel);
      sleep(0.001f);
      for (auto &note : notes) play_note_off(midi_context.get_midiout(), note.pitch, note.channel);
   }

   static void play_song_4(MidiContext &midi_context)
   {
      setup_long_soft_patches(midi_context);

      BYTE violin_channel = 0x00;
      BYTE cello_channel = 0x03;
      BYTE bass_channel = 0x04;

      BYTE velocity = 60;

      std::vector<MidiNote> notes = {
         {60+7+7, velocity, violin_channel},
         {60+7, velocity, violin_channel},
         {60, velocity, violin_channel},
         {60, velocity, violin_channel},
         {60-7-12, velocity, cello_channel},
         {60-7-12-5, velocity, bass_channel},
      };

      for (auto &note : notes)
         play_note_on(midi_context.get_midiout(), note.pitch, note.velocity, note.channel);

      sleep(3.0f);

      for (auto &note : notes)
         play_note_off(midi_context.get_midiout(), note.pitch, note.channel);
   }
};



int main(void)
{
   MidiContext midi_context;
   midi_context.initialize();

   //SongFactory::setup_long_soft_patches(midi_context);
   SongFactory::play_song_4(midi_context);

   midi_context.shutdown();
   printf("Program appears to have run successfully.");
   return 0;
}
