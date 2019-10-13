# Fuzzing notes

# Dependencies
- install afl in ubuntu
```shell script
sudo apt install afl
```

# Build
```shell script
mkdir build
cd build
cmake -DCMAKE_CXX_COMPILER=afl-clang-fast++ ..
make
```

#Run Fuzzers
```shell script
screen afl-fuzz -i ../tests/user/fuzz-testcases/ -o syncdir/ -M fuzzer1 -- ./fuzzing_user
```

`ctrl+a d` to detach
 
