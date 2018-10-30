# bfc

## About
A simple feature-rich Brainfuck interpreter written in C++ for \*nix systems.

You can read about Brainfuck [here](https://esolangs.org/wiki/brainfuck).

## Install

### Dependencies

Any C++17 compliant compiler. Check if your compiler version is compatible
[here](https://en.cppreference.com/w/cpp/compiler_support#C.2B.2B17_features)

```
git clone https://github.com/canopeerus/bfc
cd bfc
make && make install
```

## Usage
```
Usage
    bfc <file>      :   Read input from file
    bfc -c <code>   :   Feed Brainfuck code directly as input
    bfc -h          :   Show help/usage guide
```
Calling `bfc` with no arguments will lead to reading input directly
from STDIN. When providing input to `bfc` from STDIN you'll have to
manually signal EOF to terminate the input and begin execution. In
Unix-like systems this is done by pressing CTRL-D to terminate input.

## Bugs
~~* Nested looping still doesn't work~~

## License
[LICENSE](LICENSE)
