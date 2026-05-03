# C-written Compiler 4 Brainfuck

C-written Compiler 4 Brainfuck (CC4B) is a brainfuck to Bytecode Compiler with a custom VM, implemented in C. Currently supports generating IR from source code.

## Quick <s>Start</s> Test

### Optional Dependencies

* PowerShell *(for the build script)*
* MinGW-w64 GCC 8.1.0 or newer

### Build

```powershell
# Clone the repository
git clone https://github.com/YufangProbably/C-Compiler-4-Brainfuck.git
cd C-Compiler-4-Brainfuck

# Run tests
.\please.ps1 build-test
.\please.ps1 run-test
```

## Todo

- [x] Brainfuck source code to IR
- [ ] IR to Bytecode
- [ ] Virtual machine
- [ ] Optimizing

## License

Distributed under the MIT license. See [LICENSE](/LICENSE).