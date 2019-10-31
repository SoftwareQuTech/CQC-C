NOTE: The CQC libraries was previously part of the SimulaQron repo but have now
been split of since CQC is independent from SimulaQron and can be used for
other simulators that supports it or real hardware. However, if you wish to use
this library for SimulaQron, checkout the repo
[https://github.com/SoftwareQuTech/SimulaQron]() and the docs
[https://softwarequtech.github.io/SimulaQron/html/index.html]().

This is the C library for accessing the CQC backend. The packet format is
specified in cqc.h. Evidently, you will need a C compiler to compile and run
these files, as well as a unix socket library (winsock is not supported).

To build and test the library, proceed as follows:
- `make` - compile the library and tests
- `run_test.sh` - spin up the SimulaQron nodes and run the tests

You should first run `./run_test.sh` in order to verify the install. This will
check the gates using tomography. This requires a small amount of time, so be
patient :-)
