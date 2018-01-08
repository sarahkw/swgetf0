#ifndef INCLUDED_GET_CANDS
#define INCLUDED_GET_CANDS

typedef struct cross_rec Cross;
typedef struct f0_params F0_params;

void get_fast_cands(const float* const fdata, const float* const fdsdata,
                    const int ind, const int step, const int size,
                    const int dec, const int start, const int nlags,
                    float* engref, int* maxloc, float* maxval, Cross* cp,
                    float* peaks, int* locs, int* ncand,
                    const F0_params* const par);


float* downsample(float* input, int samsin, int state_idx, double freq,
                  int* samsout, int decimate, int first_time, int last_time);

#endif
