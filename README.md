# HashEnsemble
[![Build Status](https://travis-ci.org/long-gong/HashEnsemble.svg?branch=master)](https://travis-ci.org/long-gong/HashEnsemble)

[HashEnsemble](https://github.com/long-gong/HashEnsemble) is just an ensemble of a few good hash functions (_e.g._ xxhash).


## Dependencies

+ [googletest](https://github.com/google/googletest)
+ [fmtlib](https://github.com/fmtlib/fmt)
+ [boost-dynamic_bitset](https://github.com/boostorg/dynamic_bitset)
+ [boost-serialization](https://github.com/boostorg/serialization)

Install:
```bash
chmod +x ./scripts/install_dependencies.sh
./scripts/install_dependencies.sh
```

**!!ONLY REQUIRED if you want to use benchmarks!!**

## Build

```bash
mkdir build && cd build
cmake ..
make
```

## Benchmarks

On a workstation with Intel(R) Core(TM) i7-9800X CPU @ 3.80GHz running Ubuntu 18.04, we obtained the following results
```csv
name,key type,speed (million keys per second),raw_speed (million keys per second),degradation (%),collision_1 (%),collision_2 (%),collision_5 (%),collision_10 (%)
xxhash,64-bit integer,601.86,1065.02,43.49,36.79,10.66,1.87,0.48
wyhash,64-bit integer,944.51,1518.76,37.81,36.78,10.65,1.87,0.48
MurmurHash3,64-bit integer,201.88,147.03,-37.30,36.79,10.66,1.88,0.49
Spooky,64-bit integer,147.88,163.45,9.53,36.78,10.65,1.87,0.48
xxhash_ct,64-bit integer,1075.82,1063.99,-1.11,36.79,10.66,1.87,0.48
wyhash_ct,64-bit integer,1026.66,1590.61,35.46,36.78,10.65,1.87,0.48
```

`_rt` means the hash function was selected during the runtime whereas `_ct` means selection was done during compile time.

**Overall Performance**:
> wyhash > xxhash > MurmurHash3 > Spooky

Significant performance degradation was obversed for `wyhash` (**Reasons to be figured out). So if you
decide to use `wyhash` and speed matters much in your program, then you are not suggested to use HashEnsemble.

For more professional benchmarks on more hash functions, please refer to [aappleby/smhasher](https://github.com/aappleby/smhasher) and [rurban/smhasher](https://github.com/aappleby/smhasher).
