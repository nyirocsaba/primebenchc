primebenchc
===========

primebenchc - multi-threaded prime number benchmark

Compile: make release

Execute test: ./bin/Release/primebenchc [interval top value] [max number of threads]


Sample:
./bin/Release/primebenchc 100000 8

Will measure required time to find and count all the prime numbers on [0,100000] interval starting with 1 thread then incrementing up to 8 threads

How can you help:

1. Report bugs via the issue tracker

2. Suggest new features via the issue tracker

3. E-mail to <primebenchc@gmail.com> with subject "My Results" and attache your results and PC configuration.

You can output results directly to file: ./bin/Release/primebenchc [interval top value] [max number of threads] > results.txt

Also could be grat if you also attache the lshw output of the system you run the tests.


Thank you,
Csaba
