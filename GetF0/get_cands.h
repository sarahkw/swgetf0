#ifndef INCLUDED_GET_CANDS
#define INCLUDED_GET_CANDS

// TODO(sw) Definitions for functions in get_cands.c that are used
// externally. Putting the TODO here because this may not yet be
// complete. Adding as I go.

float* downsample(float* input, int samsin, int state_idx, double freq,
                  int* samsout, int decimate, int first_time, int last_time);

#endif
