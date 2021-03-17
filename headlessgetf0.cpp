#include <iostream>

#include "GetF0/get_f0_stream.h"

struct F0HeadlessStream : public GetF0::GetF0Stream {

    void write_output_reversed(float* f0p, float* vuvp, float* rms_speech,
                               float* acpkp, float* maxsamplevalp,
                               int vecsize) override
    {
        for (int i = vecsize - 1; i >= 0; --i) {
            std::cout << f0p[i] << "\t" << maxsamplevalp[i] << "\n";
        }
    }

    long read_stream_samples(short* buffer, long num_records) override
    {
        return fread(buffer, sizeof(*buffer), num_records, stdin);
    }
};


int main(int argc, char* argv[]) {

    // rec -r 44100 -c 1 -t s16 - | ./headlessgetf0

    F0HeadlessStream headless;
    headless.init(44100);
    headless.run();
    return 0;
}
