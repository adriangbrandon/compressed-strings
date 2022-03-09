# libCSD64

This is a fork of [libCSD](https://github.com/migumar2/libCSD). The purpose is to clean it up and make it work with big (>4GB) dictionaries.

Read the original libCSD README for usage instructions.

## Building

You can build two targets. First change directory into the root folder and run:

* Release: `cmake -Bbuild/Release -DCMAKE_BUILD_TYPE=Release && cmake --build build/Release/`
* Debug: `cmake -Bbuild/Debug -DCMAKE_BUILD_TYPE=Debug && cmake --build build/Debug/`

## Benchmarks

There are tests in the `benchmark` folder. For those, the following were used:

* CPU: Intel(R) Xeon(R) Silver 4110 CPU @ 2.10GHz
* Dictionary source: Random strings from wikidata

It was not possible to run tests with big dictionaries using FMINDEX and RPHTFC, because those don't support 64 bit dictionaries yet.

You can find some useful scripts used for running the benchmarks in `benchmark/scripts`
