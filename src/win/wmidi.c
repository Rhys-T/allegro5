
/*         ______   ___    ___ 
 *        /\  _  \ /\_ \  /\_ \ 
 *        \ \ \L\ \\//\ \ \//\ \      __     __   _ __   ___ 
 *         \ \  __ \ \ \ \  \ \ \   /'__`\ /'_ `\/\`'__\/ __`\
 *          \ \ \/\ \ \_\ \_ \_\ \_/\  __//\ \L\ \ \ \//\ \L\ \
 *           \ \_\ \_\/\____\/\____\ \____\ \____ \ \_\\ \____/
 *            \/_/\/_/\/____/\/____/\/____/\/___L\ \/_/ \/___/
 *                                           /\____/
 *                                           \_/__/
 *
 *      Windows midi driver.
 *
 *      By Stefan Schimanski.
 *
 *      See readme.txt for copyright information.
 */


#include "allegro.h"
#include "allegro/aintern.h"
#include "allegro/aintwin.h"

#ifndef SCAN_DEPEND
   #include <mmsystem.h>

   #ifdef _MSC_VER
      #include <mmreg.h>
   #endif
#endif

#ifndef ALLEGRO_WINDOWS
#error something is wrong with the makefile
#endif


int midi_win32_detect(int input);
int midi_win32_init(int input, int voices);
void midi_win32_exit(int input);
int midi_win32_mixer_volume(int volume);
void midi_win32_raw_midi(int data);


/* driver globals */
static HMIDIOUT midi_device = NULL;


/* dynamically generated driver list */
static _DRIVER_INFO *driver_list = NULL;



/* _get_midi_driver_list:
 *  System driver hook for listing the available MIDI drivers. This generates
 *  the device list at runtime, to match whatever Windows devices are 
 *  available.
 */
_DRIVER_INFO *_get_midi_driver_list()
{
   MIDI_DRIVER *driver;
   MIDIOUTCAPS caps;
   int num_drivers, i;

   if (!driver_list) {
      num_drivers = midiOutGetNumDevs();

      if (num_drivers)  /* include the MIDI mapper, which is -1 */
	 num_drivers++;

      driver_list = malloc(sizeof(_DRIVER_INFO) * (num_drivers+2));

      for (i=0; i<num_drivers; i++) {
	 driver = malloc(sizeof(MIDI_DRIVER));
	 memcpy(driver, &midi_none, sizeof(MIDI_DRIVER));

	 driver->id = MIDI_WIN32(i);

	 midiOutGetDevCaps(i-1, &caps, sizeof(caps));

	 driver->name = driver->desc = empty_string;
	 driver->ascii_name = malloc(strlen(caps.szPname)+1);
	 strcpy((char*) driver->ascii_name, caps.szPname);

	 driver->detect = midi_win32_detect;
	 driver->init = midi_win32_init;
	 driver->exit = midi_win32_exit;
	 driver->mixer_volume = midi_win32_mixer_volume;
	 driver->raw_midi = midi_win32_raw_midi;

	 driver_list[i].id = driver->id;
	 driver_list[i].driver = driver;
	 driver_list[i].autodetect = TRUE;
      }

      driver_list[i].id = MIDI_NONE;
      driver_list[i].driver = &midi_none;
      driver_list[i].autodetect = TRUE;

      driver_list[i+1].id = 0;
      driver_list[i+1].driver = NULL;
      driver_list[i+1].autodetect = FALSE;
   }

   return driver_list;
}



/* midi_win32_detect:
 */
int midi_win32_detect(int input)
{
   /* the current driver doesn't support input */
   if (input)
      return FALSE;

   return TRUE;
}



/* midi_win32_init:
 */
int midi_win32_init(int input, int voices)
{
   MMRESULT hr;
   int id;

   /* deduce our device number from the driver ID code */
   id = (midi_driver->id & 0xFF) - 'A' - 1;

   /* open midi mapper */
   hr = midiOutOpen(&midi_device, id, 0, 0, CALLBACK_NULL);
   if (hr != MMSYSERR_NOERROR) {
      _TRACE("midiOutOpen failed (%x)\n", hr);
      goto Error;
   }

   /* resets midi mapper */
   midiOutReset(midi_device);

   return 0;

 Error:
   midi_win32_exit(input);
   return -1;
}



/* midi_win32_exit:
 */
void midi_win32_exit(int input)
{
   /* close midi stream and release device */
   if (midi_device != NULL) {
      midiOutReset(midi_device);
      midiOutClose(midi_device);
      midi_device = NULL;
   }
}



/* mixer_volume:
 */
int midi_win32_mixer_volume(int volume)
{
   unsigned long win32_midi_vol = (volume << 8) + (volume << 24);
   midiOutSetVolume(midi_device, win32_midi_vol);
   return 1;
}


/* midi_switch_out:
 */
void midi_switch_out(void)
{
   if (midi_device)
      midiOutReset(midi_device);
}


/* midi_win32_raw_midi:
 */
void midi_win32_raw_midi(int data)
{
   static int msg_lengths[8] =
   {3, 3, 3, 3, 2, 2, 3, 0};
   static unsigned long midi_msg;
   static int midi_msg_len;
   static int midi_msg_pos;

   if (data >= 0x80) {
      midi_msg_len = msg_lengths[(data >> 4) & 0x07];
      midi_msg = 0;
      midi_msg_pos = 0;
   }

   if (midi_msg_len > 0) {
      midi_msg |= ((unsigned long)data) << (midi_msg_pos * 8);
      midi_msg_pos++;

      if (midi_msg_pos == midi_msg_len) {
	 if (midi_device != NULL) {
	    if (app_foreground)
	       midiOutShortMsg(midi_device, midi_msg);
	    else
	       midiOutReset(midi_device);
	 }
      }
   }
}
