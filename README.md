PrimeBenchc v0.2
================

primebenchc - multi-threaded prime number benchmark

Compile: make release

Usage: ./bin/Debug/primebenchc [options]
Options are:
	--im <numeric>         Set intervcal maximum
	--tmin <numeric>       Set the minimum number of threads
	--tmax <numeric>       Set the maximum number of threads
	--showlicense          List license message
	--hidelicense          Don't list license message
	--default-scheduler    Use the system default scheduler
	                       Only 0 priority is accepted
	--fifo-scheduler       Use the real time scheduler
	                       Priority from 0 to 90
	--rr-scheduler         Use the round robin scheduler
	                       Priority from 0 to 90
	--scheduler-priority   Set the threads priority
	                       to -1 and highest possible
	                       priority will be used
Defaults: ./bin/Debug/primebenchc --im 100000 --tmin 1 --tmax 8 --showlicense --default-scheduler


How can you help:

1. Report bugs via the issue tracker

2. Suggest new features via the issue tracker

3. E-mail to <primebenchc@gmail.com> with subject "My Results" and attach your results and PC configuration.

You can output results directly to file: ./primebenchc [options] > results.txt

Also could be grat if you also attach the lshw output of the system you run the tests.


Thank you,
Csaba
