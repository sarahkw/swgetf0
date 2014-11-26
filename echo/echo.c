/*
  Copyright 2014 Sarah Wong

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
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
