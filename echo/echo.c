/*
    Copyright (C) 2014  Sarah Wong

    This file is part of swgetf0.

    swgetf0 is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>

#include <pulse/simple.h>

/*
  gcc `pkg-config libpulse-simple --libs --cflags` echo.c -o echo
 */

int main()
{
  pa_sample_spec ss;
  ss.format = PA_SAMPLE_S16LE;
  ss.channels = 1;
  ss.rate = 96000;

  pa_buffer_attr ba;
  ba.maxlength = (uint32_t)-1;
  ba.tlength = 0;
  ba.prebuf = (uint32_t)-1;
  ba.minreq = (uint32_t)-1;
  ba.fragsize = (uint32_t)-1;

  pa_simple* s_rec = pa_simple_new(NULL, "echo", PA_STREAM_RECORD, NULL,
                                   "Record", &ss, NULL, &ba, NULL);

  pa_simple* s_play = pa_simple_new(NULL, "echo", PA_STREAM_PLAYBACK, NULL,
                                    "Echo", &ss, NULL, &ba, NULL);

  if (!s_rec) {
    fprintf(stderr, "Pulse rec failed.\n");
    return 1;
  }

  if (!s_play) {
    fprintf(stderr, "Pulse play failed.\n");
    return 1;
  }


  char buffer[sizeof(short) * 64];
  for (;;) {
    pa_simple_read(s_rec, buffer, sizeof(buffer), NULL);
    pa_simple_write(s_play, buffer, sizeof(buffer), NULL);
  }

  return 0;
}
