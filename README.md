# HashEnsemble

[HashEnsemble]() is just an ensemble of a few good hash functions (_e.g._ xxhash).


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

On a workstation with
