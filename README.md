# swgetf0

swgetf0 is an application that listens to speech input and plots pitch
in real time. Pitch is calculated using code borrowed from the ESPS
Toolkit. The algorithm is described in the the paper [A Robust
Algorithm for Pitch Tracking
(RAPT)](http://www.ee.columbia.edu/~dpwe/papers/Talkin95-rapt.pdf) by
David Talkin.

## Building and running on Linux

```
$ git clone https://github.com/sarahkw/swgetf0.git
$ cd swgetf0/
$ cmake .
$ make -j4
$ ./swgetf0
```
