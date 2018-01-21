# erasure-codes

This repository contains an implementation of reed-solomon erasure codes packaged as a C library.
For more information on reed-solomon error correction see 
[wikipedia](https://en.wikipedia.org/wiki/Reed%E2%80%93Solomon_error_correction)
or this [blog post](https://www.backblaze.com/blog/reed-solomon/).

## Algorithm Description

The implementation of erasure codes use here works by calculating parity shards based on the
original data. If parts of the data are lost, these parity shards can be used to recover
the original data. This allows for data to be protected against drive failure or network loss
without replicating the data multiple times. Instead erasure coding allows for a much lower
overhead. This overhead can be chosed by setting the number of data shards and the total number
of shards when setting up the library.

## Basic Usage

The simplest way to use the library is to use the block API. First create an encoder
using the `erasure_create_encoder` function, then use `erasure_encode` and `erasure_recover`
or `erasure_recover_data` to decode when data is missing. If data is not missing then the 
origininal message is contained in the data shards. The number of data shards is selected when 
creating the encoder (it is the parameter `k`). Otherwise `k` shards are required to recover the
original data. Note that the encoder does not check that the data has not been corrupted when
recovering the data, even though this can be done via erasure coding.

## Stream API

When not all data is available right away, the stream API can be used to decode data in fixed
blocks without having to recalculate the encoding matrix each time (which is expensive). 
This can be done by creating an `erasure_encode_stream` or `erasure_recover_stream` and 
then using that stream to encode or recover data using `erasure_stream_encode` or 
`erasure_stream_recover_data`. If the number of missing shards in the data stream does 
not change then this will be faster than using the block API (much faster for smaller block 
sizes). If the missing shards change then this will be just as fast as the block API.

## Building

Erasure-Codes is build via cmake. To build, use cmake to generate the build system of your choice,
then use that to build the library. This can be done as follows
```
cmake /path/to/erasure/codes
cmake --build .
```

Note that erasure-codes does not currently support installing into the target platform.
