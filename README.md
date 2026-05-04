# 🚧 C-written Compiler 4 Brainfuck

C-written Compiler 4 Brainfuck (CC4B) is a [brainfuck](https://en.wikipedia.org/wiki/Brainfuck) to Bytecode Compiler with a custom VM, implemented in C. Currently supports generating IR from source code.

## Building

```bash
# Clone the repository
git clone https://github.com/YufangProbably/C-written-Compiler-4-Brainfuck.git
cd C-written-Compiler-4-Brainfuck

# Build the DLL
gcc src/*.c lib/*.c -o bin/cc4b.dll \
    -I src -I lib -shared -D CC4B_EXPORT -O2

# Build a specific test
gcc test/test_TESTNAME.c src/*.c lib/*.c -o bin/test/test_TESTNAME.exe \
    -I test -I src -I lib -D CC4B_STATIC -D CC4B_DEBUG_DUMP
```

or if PowerShell was pre-installed:

```powershell
# Build the DLL
.\please.ps1 build-dll

# Build all the tests
.\please.ps1 build-test
```

## Todo

- [x] Brainfuck source code to IR
- [ ] IR to Bytecode
- [ ] Virtual machine
- [ ] Command line interface
- [ ] Optimizing

## License

Distributed under the MIT license. See [LICENSE](/LICENSE).