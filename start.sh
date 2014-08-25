rec --buffer 512 -c 1 -b 16 -e signed-integer -r 96000 -t raw - |tee  -a practice.96000.dat |./swpitcher 2
