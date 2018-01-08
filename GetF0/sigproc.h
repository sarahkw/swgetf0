#ifndef INCLUDED_SIGPROC
#define INCLUDED_SIGPROC

void a_to_aca(float* a, float* b, float* c, int p);

int window(float* din, float* dout, int n, float preemp, int type);

int lpc(int lpc_ord, float lpc_stabl, int wsize, float* data, float* lpca,
        float* ar, float* lpck, float* normerr, float* rms, float preemp,
        int type);

void crossf(const float* data, int size, int start, int nlags, float* engref,
            int* maxloc, float* maxval, float* correl);

void crossfi(const float* data, int size, int start0, int nlags0, int nlags,
             float* engref, int* maxloc, float* maxval, float* correl,
             int* locs, int nlocs);

#endif
