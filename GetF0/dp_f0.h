#ifndef INCLUDED_DP_F0
#define INCLUDED_DP_F0

typedef struct f0_params F0_params;

int init_dp_f0(double freq, F0_params* par, long* buffsize, long* sdstep);
int dp_f0(const float* fdata, int buff_size, int sdstep, double freq,
          F0_params* par, float** f0p_pt, float** vuvp_pt,
          float** rms_speech_pt, float** acpkp_pt, int* vecsize, int last_time);

#endif
