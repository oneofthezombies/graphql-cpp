#!/usr/bin/env bash

set -ex

# Check C++
clang --version
clang++ --version
clang-format --version
[[ "$(which lld)" == "/opt/llvm/bin/lld" ]] || exit 1
cmake --version
ctest --version
ninja --version
[[ "$(which gdb)" == "/usr/bin/gdb" ]] || exit 1

# Check JavaScript
volta --version
node --version
npm --version
gitmoji --version

# Check Python
pipx --version
clang-format --version
