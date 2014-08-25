rec --buffer 512 -c 1 -b 16 -e signed-integer -r 44100 -t raw - |tee  -a practice.dat |./swpitcher 2
