#ifndef INCLUDED_GET_CANDS
#define INCLUDED_GET_CANDS

typedef struct cross_rec Cross;
typedef struct f0_params F0_params;

void get_fast_cands(float* fdata, float* fdsdata, int ind, int step, int size,
                    int dec, int start, int nlags, float* engref, int* maxloc,
                    float* maxval, Cross* cp, float* peaks, int* locs,
                    int* ncand, F0_params* par);

float* downsample(float* input, int samsin, int state_idx, double freq,
                  int* samsout, int decimate, int first_time, int last_time);

#endif
